#include "AnimationClip.h"
#include "../Component/Transform/Transform.h"
#include "../Time.h"


using namespace FrameWork;

AnimationClip::AnimationClip() : frameCnt(0), isLoop(false), speed(1.0f), maxFrame(0), isEnd(false)
{
}

AnimationClip::~AnimationClip()
{
}

void AnimationClip::DrawImGui(int id)
{
	std::string strId = "##AnimationClip" + std::to_string(id);
	ImGui::Checkbox(("IsLoop" + strId).c_str(), &this->isLoop);
	ImGui::DragInt(("MaxFrame" + strId).c_str(), &this->maxFrame, 0.1f, 0);
	ImGui::SliderFloat(("FrameCnt" + strId).c_str(), &this->frameCnt, 0.0f, (float)this->maxFrame);
	ImGui::DragFloat(("Speed" + strId).c_str(), &this->speed, 0.1f);
}

void AnimationClip::Initialize(std::string name, std::weak_ptr<TakeNode> takeNode)
{
	this->name = name;
	this->takeNode = takeNode;
}

void AnimationClip::SetAnimationTake(Transform * hierarchy, int frame, Matrix4 & matrix)
{
	std::vector<int> indexArray;
	SearchHeirarchy(hierarchy, hierarchy->GetBossParent().lock().get(), &indexArray);

	SetAnimationTake(&indexArray, frame, matrix);

	// 解放処理
	std::vector<int>().swap(indexArray);
}

void AnimationClip::SetAnimationTake(std::vector<int> * indexArray, int frame, Matrix4 & matrix)
{
	TakeNode * targetNode;

	if (indexArray->size() == 0)
	{
		targetNode = this->takeNode.lock().get();
	}
	else
	{
		targetNode = this->takeNode.lock()->GetIndexArrayToTakeNode(&(*indexArray)[0], (int)indexArray->size());
	}

	// まだ生成してない場合は生成
	if (!targetNode->animTake)
		targetNode->animTake = std::shared_ptr<AnimationTake>(new AnimationTake());

	// 目的の階層にデータを挿入
	targetNode->animTake->AddTakePosition(frame, matrix.position());
	targetNode->animTake->AddTakeScale(frame, matrix.scale());
	targetNode->animTake->AddTakeRotation(frame, matrix.rotation());
}

void AnimationClip::DeleteAnimationTake(Transform * hierarchy, int frame)
{
	std::vector<int> indexArray;
	SearchHeirarchy(hierarchy, hierarchy->GetBossParent().lock().get(), &indexArray);

	DeleteAnimationTake(indexArray, frame);

	// 解放処理
	std::vector<int>().swap(indexArray);
}

void AnimationClip::DeleteAnimationTake(std::vector<int> indexArray, int frame)
{
	TakeNode * targetNode;

	if (indexArray.size() == 0)
	{
		targetNode = this->takeNode.lock().get();
	}
	else
	{
		targetNode = this->takeNode.lock()->SearchIndexArrayToTakeNode(&indexArray[0], (int)indexArray.size());
	}

	if (targetNode)
	{
		targetNode->animTake->DeleteTakePosition(frame);
		targetNode->animTake->DeleteTakeScale(frame);
		targetNode->animTake->DeleteTakeRotation(frame);
	}
}

void AnimationClip::SetCallBackFrame(int frameCnt, std::function<void(void)> callBackFunc)
{
	callBackFuncMap[frameCnt] = callBackFunc;
}

void AnimationClip::UpdateAnimation(Transform * transform)
{
	HierarchyUpdate(transform, takeNode.lock().get(), frameCnt);
	UpdateFrame();
}

void AnimationClip::UpdateFrame()
{
	frameCnt += speed * Time::DeltaTime();
	if (frameCnt >= 0.0f && frameCnt < (float)maxFrame)
	{
		isEnd = false;

		if (oldFrameCnt != (int)frameCnt)
		{
			if (callBackFuncMap.count((int)frameCnt) > 0)
			{
				callBackFuncMap[(int)frameCnt]();
			}

			oldFrameCnt = (int)frameCnt;
		}
	}
	else
	{
		isEnd = true;
		frameCnt = (frameCnt < 0.0f) ? 0.0f : (float)maxFrame;

		if (oldFrameCnt != (int)frameCnt)
		{
			if (callBackFuncMap.count((int)frameCnt) > 0)
			{
				callBackFuncMap[(int)frameCnt]();
			}

			oldFrameCnt = (int)frameCnt;
		}

		if (isLoop)
		{
			frameCnt = (frameCnt != 0.0f) ? 0.0f : (float)maxFrame;
		}
	}
}

void AnimationClip::UpdateBlendAnimation(Transform * transform, AnimationClip * anim1, AnimationClip * anim2, const float weight)
{
	HierarchyBlendUpdate(transform, anim1->takeNode.lock().get(), anim2->takeNode.lock().get(), anim1->frameCnt, anim2->frameCnt, weight);
	anim1->UpdateFrame();
	anim2->UpdateFrame();
}

AnimationClip * AnimationClip::CreateClone()
{
	AnimationClip * clone = new AnimationClip();
	// ノードの参照を渡す
	clone->takeNode = this->takeNode;
	clone->name = this->name;
	clone->isLoop = this->isLoop;
	clone->maxFrame = this->maxFrame;
	clone->speed = this->speed;

	return clone;
}

bool AnimationClip::SearchHeirarchy(Transform * hierarchy, Transform * parent, std::vector<int>* indexArray)
{
	// 見つけたらtrueを返す
	if (hierarchy == parent)
		return true;

	int cnt = 0;
	for (auto & child : parent->childs)
	{
		indexArray->emplace_back(cnt);

		if (SearchHeirarchy(hierarchy, child.lock().get(), indexArray))
		{
			return true;
		}
		
		// 見つけられなかったらその階層ではないので消す
		indexArray->erase(indexArray->end());
		cnt++;
	}

	// 見つけられなかったらfalseを返す
	return false;
}

void AnimationClip::OnStart()
{
	oldFrameCnt = -1;
	isEnd = false;
	frameCnt = 0.0f;
}

inline void AnimationClip::HierarchyUpdate(Transform * transform, TakeNode * takeNode, const float frameCnt)
{
	// テイクを生成していれば更新
	if (takeNode->animTake)
	{
		Vector3 pos;
		Vector3 sca;
		Quaternion rot;
		takeNode->animTake->UpdateTake(frameCnt, &pos, &sca, &rot);
		transform->SetLocalMatrix(pos, sca, rot);
		
		// 親がいない場合
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// 親がいる場合
		else
			transform->worldMatrix = transform->parent.lock()->worldMatrix * transform->localMatrix;
	}

	if (transform->childs.size() == 0)
		return;

	auto itr = transform->childs.begin();
	auto itr_end = transform->childs.end();
	for (auto & take : takeNode->childTakes)
	{
		if (itr == itr_end)
			break;

		HierarchyUpdate(itr->lock().get(), &take, frameCnt);

		++itr;
	}
}

inline void AnimationClip::HierarchyBlendUpdate(Transform * transform, TakeNode * takeNode1, TakeNode * takeNode2, const float frameCnt1, const float frameCnt2, const float weight)
{
	// 二つのテイクがあればブレンド
	if (takeNode1->animTake && takeNode2->animTake)
	{
		Vector3 pos1,pos2;
		Vector3 sca1,sca2;
		Quaternion rot1,rot2;
		takeNode1->animTake->UpdateTake(frameCnt1, &pos1, &sca1, &rot1);
		takeNode2->animTake->UpdateTake(frameCnt2, &pos2, &sca2, &rot2);

		Vector3 pos = Vector3::Lerp(pos1, pos2, weight);
		Vector3 sca = Vector3::Lerp(sca1, sca2, weight);
		Quaternion rot = Quaternion::Slerp(rot1, rot2, weight);
		transform->SetLocalMatrix(pos, sca, rot);

		// 親がいない場合
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// 親がいる場合
		else
			transform->worldMatrix = transform->parent.lock()->worldMatrix * transform->localMatrix;
	}
	else if (takeNode1->animTake)
	{
		Vector3 pos;
		Vector3 sca;
		Quaternion rot;
		takeNode1->animTake->UpdateTake(frameCnt1, &pos, &sca, &rot);
		transform->SetLocalMatrix(pos, sca, rot);

		// 親がいない場合
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// 親がいる場合
		else
			transform->worldMatrix = transform->parent.lock()->worldMatrix * transform->localMatrix;
	}
	else if (takeNode2->animTake)
	{
		Vector3 pos;
		Vector3 sca;
		Quaternion rot;
		takeNode2->animTake->UpdateTake(frameCnt2, &pos, &sca, &rot);
		transform->SetLocalMatrix(pos, sca, rot);

		// 親がいない場合
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// 親がいる場合
		else
			transform->worldMatrix = transform->parent.lock()->worldMatrix * transform->localMatrix;
	}


	auto itr_take1 = takeNode1->childTakes.begin();
	auto itr_take1_end = takeNode1->childTakes.end();
	auto itr_take2 = takeNode2->childTakes.begin();
	auto itr_take2_end = takeNode2->childTakes.end();

	for (auto & child : transform->childs)
	{
		bool b_take1 = (itr_take1 != itr_take1_end);
		bool b_take2 = (itr_take2 != itr_take2_end);
		// 二つともノードが存在した場合はブレンドする
		if (b_take1 && b_take2)
		{
			HierarchyBlendUpdate(child.lock().get(), &(*itr_take1), &(*itr_take2), frameCnt1, frameCnt2, weight);
		}
		else if(b_take1)
		{
			HierarchyUpdate(child.lock().get(), &(*itr_take1), frameCnt1);
		}
		else if (b_take2)
		{
			HierarchyUpdate(child.lock().get(), &(*itr_take2), frameCnt2);
		}
		else
		{
			break;	// ノードが両方ない場合は抜ける
		}

		if(b_take1) ++itr_take1;
		if(b_take2) ++itr_take2;
	}
}

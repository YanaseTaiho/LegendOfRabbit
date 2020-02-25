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

void AnimationClip::SetAnimationTake(Transform * const hierarchy, int frame, Matrix4 & matrix)
{
	std::vector<int> indexArray;
	SearchHeirarchy(hierarchy, hierarchy->GetBossParent().lock().get(), &indexArray);

	SetAnimationTake(&indexArray, frame, matrix);

	// �������
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

	// �܂��������ĂȂ��ꍇ�͐���
	if (!targetNode->animTake)
		targetNode->animTake = std::shared_ptr<AnimationTake>(new AnimationTake());

	// �ړI�̊K�w�Ƀf�[�^��}��
	targetNode->animTake->AddTakePosition(frame, matrix.position());
	targetNode->animTake->AddTakeScale(frame, matrix.scale());
	targetNode->animTake->AddTakeRotation(frame, matrix.rotation());
}

void AnimationClip::DeleteAnimationTake(Transform * const hierarchy, int frame)
{
	std::vector<int> indexArray;
	SearchHeirarchy(hierarchy, hierarchy->GetBossParent().lock().get(), &indexArray);

	DeleteAnimationTake(indexArray, frame);

	// �������
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

void AnimationClip::UpdateAnimation(Transform * const transform)
{
	HierarchyUpdate(transform, takeNode.lock().get(), frameCnt);
	UpdateFrame();
}

void AnimationClip::UpdateFrame()
{
	frameCnt += speed * Time::DeltaTime();
	// �A�j���[�V�����Đ�
	if (frameCnt >= 0.0f && frameCnt <= (float)maxFrame)
	{
		isEnd = false;

		int diffCnt = (int)frameCnt - (int)oldFrameCnt;
		if (diffCnt > 0)
		{
			for (int i = 1; i <= diffCnt; i++)
			{
				int cnt = (int)oldFrameCnt + i;
				if (callBackFuncMap.count(cnt) > 0)
				{
					callBackFuncMap[cnt]();
				}
			}
			oldFrameCnt = (int)frameCnt;
		}
		else if(diffCnt < 0)
		{
			for (int i = 1; i <= -diffCnt; i++)
			{
				int cnt = (int)oldFrameCnt - i;
				if (callBackFuncMap.count(cnt) > 0)
				{
					callBackFuncMap[cnt]();
				}
			}
			oldFrameCnt = (int)frameCnt;
		}
	}
	// �A�j���[�V�����I��
	else
	{
		isEnd = true;

		// �t�Đ�
		if (frameCnt < 0.0f)
		{
			frameCnt = 0.0f;

			int diffCnt = (int)oldFrameCnt - (int)frameCnt;

			if (diffCnt != 0)
			{
				for (int i = 1; i <= diffCnt; i++)
				{
					int cnt = (int)oldFrameCnt - i;
					if (callBackFuncMap.count(cnt) > 0)
					{
						callBackFuncMap[cnt]();
					}
				}
			}
			if (isLoop)
			{
				frameCnt = (float)maxFrame;
				oldFrameCnt = maxFrame + 1;
			}
		}
		// �ʏ�Đ�
		else
		{
			frameCnt = (float)maxFrame;

			int diffCnt = (int)frameCnt - (int)oldFrameCnt;

			if (diffCnt != 0)
			{
				for (int i = 1; i <= diffCnt; i++)
				{
					int cnt = (int)oldFrameCnt + i;
					if (callBackFuncMap.count(cnt) > 0)
					{
						callBackFuncMap[cnt]();
					}
				}
			}

			if (isLoop)
			{
				frameCnt = 0.0f;
				oldFrameCnt = -1;
			}
		}
	}
}

void AnimationClip::UpdateBlendAnimation(Transform * const transform, AnimationClip * const anim1, AnimationClip * const anim2, const float weight)
{
	HierarchyBlendUpdate(transform, anim1->takeNode.lock().get(), anim2->takeNode.lock().get(), anim1->frameCnt, anim2->frameCnt, weight);
	anim1->UpdateFrame();
	anim2->UpdateFrame();
}

AnimationClip * AnimationClip::CreateClone()
{
	AnimationClip * clone = new AnimationClip();
	// �m�[�h�̎Q�Ƃ�n��
	clone->takeNode = this->takeNode;
	clone->name = this->name;
	clone->isLoop = this->isLoop;
	clone->maxFrame = this->maxFrame;
	clone->speed = this->speed;

	return clone;
}

bool AnimationClip::SearchHeirarchy(Transform * const hierarchy, Transform * const parent, std::vector<int>* indexArray)
{
	// ��������true��Ԃ�
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
		
		// �������Ȃ������炻�̊K�w�ł͂Ȃ��̂ŏ���
		indexArray->erase(indexArray->end());
		cnt++;
	}

	// �������Ȃ�������false��Ԃ�
	return false;
}

void AnimationClip::OnStart()
{
	oldFrameCnt = -1;
	isEnd = false;
	frameCnt = 0.0f;
}

inline void AnimationClip::HierarchyUpdate(Transform * const transform, TakeNode * const takeNode, const float frameCnt)
{
	// �e�C�N�𐶐����Ă���΍X�V
	if (takeNode->animTake)
	{
		Vector3 pos;
		Vector3 sca;
		Quaternion rot;
		takeNode->animTake->UpdateTake(frameCnt, &pos, &sca, &rot);
		transform->SetLocalMatrix(pos, sca, rot);
		
		// �e�����Ȃ��ꍇ
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// �e������ꍇ
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

inline void AnimationClip::HierarchyBlendUpdate(Transform * const transform, TakeNode * const takeNode1, TakeNode * const takeNode2, const float frameCnt1, const float frameCnt2, const float weight)
{
	// ��̃e�C�N������΃u�����h
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

		// �e�����Ȃ��ꍇ
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// �e������ꍇ
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

		// �e�����Ȃ��ꍇ
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// �e������ꍇ
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

		// �e�����Ȃ��ꍇ
		if (transform->parent.expired())
			transform->worldMatrix = transform->localMatrix;
		// �e������ꍇ
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
		// ��Ƃ��m�[�h�����݂����ꍇ�̓u�����h����
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
			break;	// �m�[�h�������Ȃ��ꍇ�͔�����
		}

		if(b_take1) ++itr_take1;
		if(b_take2) ++itr_take2;
	}
}

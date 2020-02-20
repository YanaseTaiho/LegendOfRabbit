#include "LocusController.h"

void LocusController::DrawImGui(int id)
{
	std::string strId = "##" + std::to_string(id);
	ImGui::Text("locusRenderer");
	MyImGui::DropTargetComponent(locusRenderer, strId);

	if (locusRenderer.expired()) return;

	ImGui::Text("locusPoint 1");
	MyImGui::DropTargetComponent(locusTransform1, strId);
	ImGui::Text("locusPoint 2");
	MyImGui::DropTargetComponent(locusTransform2, strId);


	if (ImGui::DragInt(("Frame Max" + strId).c_str(), &frameMax))
	{
		frameMax = Mathf::Min(frameMax, 0);
		framePos1.resize(frameMax);
		framePos2.resize(frameMax);
		locusRenderer.lock()->CreateMesh(frameMax);
	}
	ImGui::DragInt(("Remain Time" + strId).c_str(), &remainTime);

	ImGui::DragInt(("Start FrameCnt" + strId).c_str(), &startFrameCnt);
	ImGui::DragInt(("End FrameCnt" + strId).c_str(), &endFrameCnt);

	if (ImGui::Button("Start")) LocusStart();
}

LocusController::~LocusController()
{
}

void LocusController::SetCollision(std::weak_ptr<Rigidbody> myRigidbody, std::function<void(MeshCastInfo&, MeshPoints&, float)> hitFunc)
{
	this->myRigidbody = myRigidbody;
	this->CollisionFunc = hitFunc;
}

void LocusController::LocusStop()
{
	startFrameCnt = 0;
	endFrameCnt = 0;
	remainCnt = 0;
	isStart = false;
	frameCnt = 0;
}

void LocusController::LocusStart(int collisionInterval)
{
	startFrameCnt = 0;
	endFrameCnt = 0;
	remainCnt = remainTime;
	isStart = true;
	this->collisionInterval = collisionInterval;
	frameCnt = 0;
}

void LocusController::LateUpdate()
{
	if (locusRenderer.expired() || locusTransform1.expired() || locusTransform2.expired()) return;

	if (!isStart || startFrameCnt >= frameMax - 1)
	{
		locusRenderer.lock()->SetEnable(false);
		isStart = false;
		return;
	}

	locusRenderer.lock()->SetEnable(true);

	// フレーム毎にターゲットの座標を保存する
	framePos1[endFrameCnt] = locusTransform1.lock()->GetWorldPosition();
	framePos2[endFrameCnt] = locusTransform2.lock()->GetWorldPosition();

	// 数フレーム毎に当たり判定を行う
	if (frameCnt == 0)
	{
		// 当たり判定を行う
		if (CollisionFunc)
		{
			int colMeshNum = endFrameCnt - startFrameCnt;
			if (colMeshNum > 1)
			{
				std::vector<MeshPoints> points(1);
				// メッシュの先頭から末端のポイントを当たり判定に設定
				points[0].point[0] = framePos1[startFrameCnt];
				points[0].point[1] = framePos2[startFrameCnt];
				points[0].point[2] = framePos1[endFrameCnt];
				points[0].point[3] = framePos2[endFrameCnt];

				float locusLength = (points[0].point[0] - points[0].point[1]).Length();

				auto HitFunc = [this, points, locusLength](MeshCastInfo& info)
				{
					frameCnt = collisionInterval;
					CollisionFunc(info, (MeshPoints &)points[0], locusLength);
				};

				MeshCast::JudgeAllCollision(points, HitFunc, myRigidbody.lock()->collisions);
			}
		}
	}

	endFrameCnt++;
	endFrameCnt = Mathf::Max(endFrameCnt, frameMax - 1);

	// ここで軌跡描画コンポーネントにセット
	locusRenderer.lock()->SetMesh(framePos1, framePos2, startFrameCnt, endFrameCnt);


	remainCnt--;
	if (remainCnt <= 0)
	{
		remainCnt = 0;
		startFrameCnt++;
		startFrameCnt = Mathf::Max(startFrameCnt, frameMax - 1);
	}

	if(frameCnt > 0) frameCnt--;
}

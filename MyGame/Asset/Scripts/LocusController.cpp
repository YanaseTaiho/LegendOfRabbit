#include "LocusController.h"

void LocusController::DrawImGui(int id)
{
	std::string strId = "##" + std::to_string(id);
	MyImGui::DropTargetComponent(locusRenderer, strId);

	if (locusRenderer.expired()) return;

	ImGui::Text("locusTransform 1");
	MyImGui::DropTargetComponent(locusTransform1, strId);
	ImGui::Text("locusTransform 2");
	MyImGui::DropTargetComponent(locusTransform2, strId);

	//テスト
	ImGui::Text("locusTransform 3");
	MyImGui::DropTargetComponent(locusTransform3, strId);
	ImGui::Text("locusTransform 4");
	MyImGui::DropTargetComponent(locusTransform4, strId);

	if (ImGui::DragInt(("Frame Max" + strId).c_str(), &frameMax))
	{
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

void LocusController::LocusStart()
{
	startFrameCnt = 0;
	endFrameCnt = 0;
	remainCnt = remainTime;
	isStart = true;
}

void LocusController::LateUpdate()
{
	if (locusRenderer.expired() || locusTransform1.expired() || locusTransform2.expired()) return;

	if (!locusTransform3.expired() && !locusTransform4.expired())
	{
		Vector3 p[4] = 
		{
			locusTransform1.lock()->GetWorldPosition(),
			locusTransform2.lock()->GetWorldPosition(),
			locusTransform3.lock()->GetWorldPosition(),
			locusTransform4.lock()->GetWorldPosition()
		};
		if (MeshCast::JudgeAllCollision(p))
		{
			ImGui::Text("Hit");
		}
	}

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

	// ここで軌跡描画コンポーネントにセット
	locusRenderer.lock()->SetMesh(framePos1, framePos2, startFrameCnt, endFrameCnt);

	endFrameCnt++;
	endFrameCnt = Mathf::Max(endFrameCnt, frameMax - 1);

	remainCnt--;
	if (remainCnt <= 0)
	{
		remainCnt = 0;
		startFrameCnt++;
		startFrameCnt = Mathf::Max(startFrameCnt, frameMax - 1);
	}
}

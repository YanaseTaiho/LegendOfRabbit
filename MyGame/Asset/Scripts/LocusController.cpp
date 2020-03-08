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

	// �t���[�����Ƀ^�[�Q�b�g�̍��W��ۑ�����
	framePos1[endFrameCnt] = locusTransform1.lock()->GetWorldPosition();
	framePos2[endFrameCnt] = locusTransform2.lock()->GetWorldPosition();

	// ���t���[�����ɓ����蔻����s��
	if (frameCnt == 0)
	{
		// �����蔻����s��
		if (CollisionFunc)
		{
			int colMeshNum = endFrameCnt - startFrameCnt;
			if (colMeshNum > 0)
			{
				int start = endFrameCnt - 6;
				start = Mathf::Min(start, startFrameCnt);

				std::vector<MeshPoints> points(1);
				// ���b�V���̐擪���疖�[�̃|�C���g�𓖂��蔻��ɐݒ�
				points[0].point[0] = framePos1[start];
				points[0].point[1] = framePos2[start];
				// �O�t���[���ƕϓ����Ȃ��ꍇ������̂ŏ����ʒu�����炷
				points[0].point[2] = framePos1[endFrameCnt] + Vector3::one() * 0.01f;
				points[0].point[3] = framePos2[endFrameCnt] + Vector3::one() * 0.01f;

				DebugLine::DrawRay(points[0].point[0], points[0].point[1], Color::red(), 2.0f);
				DebugLine::DrawRay(points[0].point[0], points[0].point[2], Color::red(), 2.0f);
				DebugLine::DrawRay(points[0].point[1], points[0].point[3], Color::red(), 2.0f);
				DebugLine::DrawRay(points[0].point[2], points[0].point[3], Color::red(), 2.0f);

				float locusLength = (points[0].point[0] - points[0].point[1]).Length();

				auto HitFunc = [this, points, locusLength](MeshCastInfo& info)
				{
					if (info.collision.lock()->isTrigger) return;
					frameCnt = collisionInterval;
					CollisionFunc(info, (MeshPoints &)points[0], locusLength);
				};

				MeshCast::JudgeAllCollision(points, HitFunc, myRigidbody.lock()->collisions);
			}
		}
	}

	endFrameCnt++;
	endFrameCnt = Mathf::Max(endFrameCnt, frameMax - 1);

	// �����ŋO�Օ`��R���|�[�l���g�ɃZ�b�g
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

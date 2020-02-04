#include "CameraController.h"
#include "../Actor/Player/PlayerActor.h"

#include "Plugin/CharacterCameraPlugin.h"
#include "Plugin/RockOnCameraPlugin.h"

void CameraController::DrawImGui(int id)
{
	ImGui::Text("Camera Transform");
	MyImGui::DropTargetComponent(cameraTransform, std::to_string(id));
	ImGui::Text("Vertical Transform");
	MyImGui::DropTargetComponent(verticalTransform, std::to_string(id));
	ImGui::Text("Target Transform");
	MyImGui::DropTargetComponent(targetTransform, std::to_string(id));
	ImGui::Text("Player Actor");
	MyImGui::DropTargetComponent(playerActor, std::to_string(id));

	ImGui::DragFloat("targetDistance", &targetDistance);
	ImGui::DragFloat("distanceSpeed", &distanceSpeed);
	ImGui::DragFloat("offsetHeight", &offsetHeight);
}

void CameraController::OnStart()
{
	AddPlugin(Plugin::Character, new CharacterCameraPlugin());
	AddPlugin(Plugin::RockOn, new RockOnCameraPlugin());
}

void CameraController::OnUpdate()
{
	if (currentPlugin.expired()) return;
	currentPlugin.lock()->OnUpdate(this);
}

void CameraController::OnLateUpdate()
{
	if (currentPlugin.expired()) return;
	currentPlugin.lock()->OnLateUpdate(this);

	if (!CollisionCheck())
	{
		UpdateDistance(this->targetDistance, this->distanceSpeed);
	}
}

void CameraController::AddPlugin(Plugin key, CameraPlugin * plugin)
{
	pluginMap[key].reset(plugin);
}

void CameraController::ChangePlugin(Plugin key)
{
	if (pluginMap.count(key) == 0) return;

	if (!currentPlugin.expired())
	{
		currentPlugin.lock()->OnDestroy(this);
	}

	currentPlugin = pluginMap[key];
	currentPlugin.lock()->OnStart(this);
}

bool CameraController::CollisionCheck()
{
	Transform* pTr = playerActor.lock()->transform.lock().get();	// プレイヤーのトランスフォームポインタ
	Transform* cTr = cameraTransform.lock().get();					// カメラのトランスフォームポインタ

	// プレイヤーとカメラの間に障害物があるか探す
	Ray ray;
	RayCastInfo castInfo;
	float finalDistance = -1.0f;
	float curDist = -cTr->GetLocalPosition().z;
	Vector3 playerPos = pTr->GetWorldPosition() + Vector3(0.0f, offsetHeight, 0.0f);
	Vector3 dir = cTr->GetWorldPosition() - playerPos;
	ray.Set(playerPos, dir.Normalized(), curDist * 2.0f);
	
	if (RayCast::JudgeAllCollision(&ray, &castInfo))
	{
		finalDistance = castInfo.distance;
	}
	dir = cTr->GetWorldPosition() + cTr->right() * 1.0f - playerPos;
	ray.Set(playerPos, dir.Normalized(), curDist * 2.0f);

	if (RayCast::JudgeAllCollision(&ray, &castInfo))
	{
		if(finalDistance < 0.0f || finalDistance > castInfo.distance)
			finalDistance = castInfo.distance;
	}
	dir = cTr->GetWorldPosition() - cTr->right() * 1.0f - playerPos;
	ray.Set(playerPos, dir.Normalized(), curDist * 2.0f);

	if (RayCast::JudgeAllCollision(&ray, &castInfo))
	{
		if (finalDistance < 0.0f || finalDistance > castInfo.distance)
			finalDistance = castInfo.distance;
	}

	if (finalDistance > 0)
	{
		if (finalDistance < this->targetDistance + 0.5f)
		{
			UpdateDistance(finalDistance, 50.0f);
		}
		return true;
	}

	return false;
}

void CameraController::UpdateDistance(float distance, float speed)
{
	Transform* cTr = cameraTransform.lock().get();
	float curDist = -cTr->GetLocalPosition().z;

	float t = speed * Time::DeltaTime();
	float dist = Mathf::Lerp(curDist, distance, t);

	cTr->SetLocalPosition(Vector3(0.0f, 0.0f, -dist));
}

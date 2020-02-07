#include "RockOnCameraPlugin.h"
#include "../CameraController.h"
#include "../../Actor/Player/PlayerActor.h"

void RockOnCameraPlugin::OnStart(CameraController * controller)
{
	defaultDistance = 60.0f;
}

void RockOnCameraPlugin::OnLateUpdate(CameraController * controller)
{
	auto player = controller->playerActor.lock();
	auto rootTransform = controller->transform.lock();
	auto verticalTransform = controller->verticalTransform.lock();
	auto cameraTransform = controller->cameraTransform.lock();

	Vector3 rootPos = rootTransform->GetWorldPosition();
	Vector3 playerPos = controller->targetTransform.lock()->GetWorldPosition() + Vector3(0.0f, controller->offsetHeight, 0.0f);
	float vertical = 10.0f;

	// プレイヤーがターゲットを注目時
	if (!player->targetTransform.expired())
	{
		// プレイヤーとターゲットの間が中心になるようにする
		Vector3 tPos = player->targetTransform.lock()->GetWorldPosition();
		Vector3 targetPos = Vector3::Lerp(playerPos, tPos, 0.3f);
		rootPos = Vector3::Lerp(rootPos, targetPos, Time::DeltaTime() * 5.0f);
		rootTransform->SetWorldPosition(rootPos);

		Vector3 cameraPos = cameraTransform->GetWorldPosition();
		Vector3 toDir = tPos - cameraPos;
		Vector3 toDirNoY = Vector3(toDir.x, cameraPos.y, toDir.z);
		float toLen = toDir.Length();
		controller->targetDistance = defaultDistance + toLen * 0.2f;

		vertical = Mathf::ACosf(Vector3::Dot(toDirNoY.Normalize(), toDir.Normalize())) * Mathf::RadToDeg();
		vertical *= Mathf::Signf(Vector3::Dot(-Vector3::up(), toDir));
		vertical = Mathf::Clamp(vertical, -40.0f, 30.0f);
	}
	else
	{
	}
	rootPos = Vector3::Lerp(rootPos, playerPos, Time::DeltaTime() * 5.0f);
	rootTransform->SetWorldPosition(rootPos);
	

	Quaternion rotation = rootTransform->GetWorldRotation();
	Vector3 lookAt = player->transform.lock()->forward();
	Quaternion look = Quaternion::LookRotation(lookAt).Normalized();
	rotation = rotation.Slerp(look, 10.0f * Time::DeltaTime());
	rootTransform->SetWorldRotation(rotation.Normalized());

	// 極位方向の回転
	Quaternion vQ = Quaternion::EulerAngles(vertical, 0.0f, 0.0f);
	Quaternion vS = Quaternion::Slerp(verticalTransform->GetLocalRotation(), vQ, Time::DeltaTime() * 1.5f);
	verticalTransform->SetLocalRotation(vS.Normalize());


	if (!controller->CollisionCheck())
	{
		controller->UpdateDistance(controller->targetDistance, controller->distanceSpeed);
	}
}

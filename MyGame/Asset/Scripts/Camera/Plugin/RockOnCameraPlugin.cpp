#include "RockOnCameraPlugin.h"
#include "../CameraController.h"
#include "../../Actor/Player/PlayerActor.h"

void RockOnCameraPlugin::OnLateUpdate(CameraController * controller)
{
	auto player = controller->playerActor.lock();
	auto rootTransform = controller->transform.lock();

	Vector3 rootPos = rootTransform->GetWorldPosition();
	Vector3 playerPos = controller->targetTransform.lock()->GetWorldPosition() + Vector3(0.0f, controller->offsetHeight, 0.0f);

	rootPos = Vector3::Lerp(rootPos, playerPos, Time::DeltaTime() * 5.0f);
	rootTransform->SetWorldPosition(rootPos);

	Quaternion rotation = rootTransform->GetWorldRotation();
	Vector3 lookAt = player->transform.lock()->forward() + Vector3::up() * -0.2f;
	Quaternion look = Quaternion::LookRotation(lookAt).Normalized();
	rotation = rotation.Slerp(look, 10.0f * Time::DeltaTime());
	rootTransform->SetWorldRotation(rotation.Normalized());
}

#include "RockOnCameraPlugin.h"
#include "../CameraController.h"
#include "../../Actor/Player/PlayerActor.h"

void RockOnCameraPlugin::OnStart(CameraController * controller)
{
	defaultDistance = 50.0f;
}

void RockOnCameraPlugin::OnLateUpdate(CameraController * controller)
{
	auto player = controller->playerActor.lock();
	auto rootTransform = controller->transform.lock();
	auto verticalTransform = controller->verticalTransform.lock();
	auto cameraTransform = controller->cameraTransform.lock();

	Vector3 rootPos = rootTransform->GetWorldPosition();
	Vector3 playerPos = controller->targetTransform.lock()->GetWorldPosition() + Vector3(0.0f, controller->offsetHeight, 0.0f);
	float vertical = 0.0f;
	float rotationSpeed = 0.0f;

	// プレイヤーがターゲットを注目時
	if (!player->targetTransform.expired())
	{
		// プレイヤーとターゲットの間が中心になるようにする
		Vector3 targetPos = player->targetTransform.lock()->GetWorldPosition();
		Vector3 tPos = Vector3::Lerp(playerPos, targetPos, 0.3f);	// プレイヤーと敵の間に中心を持っていく
		rootPos = Vector3::Lerp(rootPos, tPos, Time::DeltaTime() * 5.0f);
		rootTransform->SetWorldPosition(rootPos);

		//Vector3 cameraPos = playerPos;// cameraTransform->GetWorldPosition();
		Vector3 toDir = targetPos - playerPos;
		Vector3 toDirNoY = Vector3(toDir.x, playerPos.y, toDir.z);
		float hightDiff = toDir.y;
		float toLen = toDir.Length();
		controller->targetDistance = defaultDistance + toLen * 0.4f;
 
		vertical = Mathf::ACosf(Vector3::Dot(toDirNoY.Normalize(), toDir.Normalize())) * Mathf::RadToDeg() * 0.6f;
		vertical = (hightDiff > 30.0f) ? -vertical : vertical;	// 上下の確認
		vertical = Mathf::Clamp(vertical, -40.0f, 40.0f);

		Vector3 c_to_p = playerPos - cameraTransform->GetWorldPosition();
		c_to_p.y = 0.0f;
		toDirNoY.y = 0.0f;	// カメラとの角度の比較に使う

		// 一定の角度以上なら回転
		float cpDot = Vector3::Dot(toDirNoY.Normalize(), c_to_p.Normalize());
		if (cpDot < 0.8f)
		{
			float angle = 30.0f;
			Vector3 cross = Vector3::Cross(Vector3::up(), toDirNoY);
			angle = (Vector3::Dot(cross, c_to_p) > 0) ? angle : -angle;
			Quaternion q = Quaternion::AxisAngle(Vector3::up(), angle).Normalize();

			// カメラの軸回転
			Vector3 lookAt = toDirNoY;
			rotation = q * Quaternion::LookRotation(lookAt).Normalized();
		}

		rotationSpeed = 1.0f;
	}
	// ターゲットがいない時
	else
	{
		controller->targetDistance = defaultDistance;

		rootPos = Vector3::Lerp(rootPos, playerPos, Time::DeltaTime() * 5.0f);
		rootTransform->SetWorldPosition(rootPos);

		// カメラの軸回転
		Vector3 lookAt = player->transform.lock()->forward();
		rotation = Quaternion::LookRotation(lookAt).Normalized();

		rotationSpeed = 10.0f;
	}

	// 水平方向の回転
	Quaternion rot = rootTransform->GetWorldRotation();
	rot = rot.Slerp(rotation, rotationSpeed * Time::DeltaTime());
	rootTransform->SetWorldRotation(rot.Normalized());

	// 極位方向の回転
	Quaternion cRot = rootTransform->GetWorldRotation();
	Quaternion vQ = Quaternion::AxisAngle(rootTransform->right(), vertical).Normalized() * cRot;
	Quaternion vS = Quaternion::Slerp(verticalTransform->GetWorldRotation(), vQ, Time::DeltaTime() * 1.5f);
	verticalTransform->SetWorldRotation(vS.Normalized());

	// カメラ自体の回転
	Quaternion cameraLook;
	cameraLook = Quaternion::LookRotation(rootPos - cameraTransform->GetWorldPosition()).Normalized();
	cameraLook = Quaternion::Slerp(cameraTransform->GetWorldRotation(), cameraLook, Time::DeltaTime() * 1.0f);
	cameraTransform->SetWorldRotation(cameraLook.Normalized());

	if (!controller->CollisionCheck())
	{
		controller->UpdateDistance(controller->targetDistance, controller->distanceSpeed);
	}
}

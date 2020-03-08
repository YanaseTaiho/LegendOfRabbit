#include "CharacterCameraPlugin.h"
#include "../CameraController.h"
#include "../../Actor/Player/PlayerActor.h"
#include "../../../DirectX/Common.h"

CharacterCameraPlugin::CharacterCameraPlugin()
{
	this->defaultDistance = 50.0f;
	this->lookSpeed = 20.0f;
	this->moveSpeed = 5.0f;

	this->lookValue = 0.0f;
	this->lookVertical = 20.0f;
}

void CharacterCameraPlugin::OnStart(CameraController * controller)
{
	this->lookValue = 0.0f;

	Vector3 verF = controller->verticalTransform.lock()->forward();
	float deg = Mathf::ACosf(Vector3::Dot(verF, controller->transform.lock()->forward())) * Mathf::RadToDeg();
	if (Vector3::Dot(verF, controller->transform.lock()->up()) > 0) deg = -deg;

	this->lookVertical = deg;
	if (std::isnan(this->lookVertical))
		this->lookVertical = 20.0f;
}

void CharacterCameraPlugin::OnUpdate(CameraController * controller)
{

}

void CharacterCameraPlugin::OnLateUpdate(CameraController * controller)
{
	auto player = controller->playerActor.lock();
	auto rootTransform = controller->transform.lock();
	auto verticalTransform = controller->verticalTransform.lock();

	Vector3 rootPos = rootTransform->GetWorldPosition();
	Quaternion rootRotation = rootTransform->GetWorldRotation();
	Vector3 playerPos = controller->targetTransform.lock()->GetWorldPosition() + Vector3(0.0f, controller->offsetHeight, 0.0f);

	rootPos = Vector3::Lerp(rootPos, playerPos, Time::DeltaTime() * moveSpeed);

	Vector3 rootForward = rootTransform->forward(); rootForward.y = 0.0f;
	rootForward.Normalize();
	Vector3 rootRight = rootTransform->right(); rootRight.y = 0.0f;
	rootRight.Normalize();

	bool input = false;

	if (Input::Keyboad::IsPress('K')) {
		this->lookValue -= 30.0f * Time::DeltaTime();
		input = true;
	}
	if (Input::Keyboad::IsPress('L')) {
		this->lookValue += 30.0f * Time::DeltaTime();
		input = true;
	}

	if (Input::Mouse::IsPress(Input::Mouse::Button::Left))
	{
		Vector2 mouse;
		Input::Mouse::GetMove(&mouse);
		this->lookValue += mouse.x * Time::DeltaTime();
		this->lookVertical += mouse.y * Time::DeltaTime() * 5.0f;
		input = true;
	}

	float thumbRX = GamePad::ThumbRX();
	if (Mathf::Absf(thumbRX) > 0.2f)
	{
		this->lookValue += 30.0f * thumbRX * Time::DeltaTime();
		input = true;
	}
	float thumbRY = GamePad::ThumbRY();
	if (Mathf::Absf(thumbRY) > 0.2f)
	{
		this->lookVertical -= 80.0f * thumbRY * Time::DeltaTime();
		input = true;
	}

	//controller->targetDistance += Input::Mouse::GetHwheel();

	if (!input)
	{
		switch (player->currentState)
		{
		case PlayerActor::State::Idle:
			break;
		case PlayerActor::State::Move:
		case PlayerActor::State::Air:

			float dotValue = Vector3::Dot(rootForward, player->moveDir);
			float degree = Mathf::ACosf(dotValue) * Mathf::RadToDeg();
			float value = lookSpeed * Time::DeltaTime();

			if (degree <= 170.0f)
			{
				float def = degree - value;
				value = (def < 0) ? degree : value;
				if (Vector3::Dot(rootRight, player->moveDir) < 0.0f)
					value = -value;

				lookValue += value * (degree / 180.0f) * player->moveAmount;

				if (fabsf(lookValue) > degree)
					lookValue = (lookValue >= 0) ? degree : -degree;

				rootForward = rootTransform->forward(); rootForward.y = 0.0f;
				rootForward.Normalize();
				rootRight = rootTransform->right(); rootRight.y = 0.0f;
				rootRight.Normalize();
			}

			break;
		}
	}

	
	// ‹ÉˆÊ•ûŒü‚Ì‰ñ“]Šp‚ÌŽZo
//	float rayLen = 150.0f;
//	float forwardRayLen = 15.0f;
//	Vector3 rayHeight = Vector3(0.0f, 30.0f, 0.0f);
//	Ray rayForward(playerPos, rootForward, forwardRayLen);
//	Ray rayForwardDown((playerPos + rayHeight) + (rootForward * forwardRayLen), Vector3::down(), rayLen);
////	Ray rayDown(rootPos, Vector3::down(), rayLen);										
//
//	DebugLine::DrawRay(rayForward.start, rayForward.end, Color::green());
//	DebugLine::DrawRay(rayForwardDown.start, rayForwardDown.end, Color::green());
//
//	RayCastInfo castInfo;
//	// ‘O‚É•Ç‚ª‚ ‚é‚Ì‚©‚ðŠm”F‚·‚é
//	if (RayCast::JudgeAllCollision(&rayForward, &castInfo, player->gameObject))
//	{
//		// ƒvƒŒƒCƒ„[‚©‚çƒJƒƒ‰‚Ì•ûŒü‚Ì‘O•û•ûŒü‚ÌˆÊ’u‚©‚ç~‚ë‚·ƒŒƒC
//		if (RayCast::JudgeAllCollision(&rayForwardDown, &castInfo, player->gameObject))
//		{
//			float h = castInfo.point.y;
//			float targetHeight = 10.0f;
//			if (h > player->castGroundInfo.point.y + targetHeight)
//			{
//				lookVertical = -10.0f;
//			}
//			else
//			{
//				lookVertical = 20.0f;
//			}
//		}
//		else
//		{
//			lookVertical = 20.0f;
//		}
//	}
//	else
//	{
//		// ƒvƒŒƒCƒ„[‚©‚çƒJƒƒ‰‚Ì•ûŒü‚Ì‘O•û•ûŒü‚ÌˆÊ’u‚©‚ç~‚ë‚·ƒŒƒC
//		if (RayCast::JudgeAllCollision(&rayForwardDown, &castInfo, player->gameObject))
//		{
//			float h = castInfo.point.y;
//			float targetHeight = 5.0f;
//			if (h < player->castGroundInfo.point.y - targetHeight)
//			{
//				lookVertical = 40.0f;
//			}
//			else if (h > player->castGroundInfo.point.y + targetHeight)
//			{
//				lookVertical = -5.0f;
//			}
//			else
//			{
//				lookVertical = 20.0f;
//			}
//		}
//	}

	// ã‹L‚ÌŒvŽZŒ‹‰Ê‚Ì”½‰f
	rootTransform->SetWorldPosition(rootPos);

	// …•½•ûŒü‚Ì‰ñ“]Šp‚ÌŽZo
	rootRotation = Quaternion::AxisAngle(Vector3::up(), lookValue) * rootRotation;
	rootTransform->SetWorldRotation(rootRotation.Normalize());
	lookValue *= 0.8f;

	// ‹ÉˆÊ•ûŒü‚Ì‰ñ“]
	this->lookVertical = Mathf::Clamp(this->lookVertical, -50.0f, 50.0f);
	Quaternion cRot = rootTransform->GetWorldRotation();
	Quaternion vQ = Quaternion::AxisAngle(rootTransform->right(), this->lookVertical).Normalized() * cRot;
	//Quaternion vS = Quaternion::Slerp(verticalTransform->GetWorldRotation(), vQ, Time::DeltaTime() * 1.5f);
	verticalTransform->SetWorldRotation(vQ.Normalized());

	// ƒJƒƒ‰Ž©‘Ì‚Ì‰ñ“]
	auto camera = controller->cameraTransform.lock();
	Quaternion cameraLook;
	cameraLook = Quaternion::LookRotation(playerPos - camera->GetWorldPosition()).Normalized();
	cameraLook = Quaternion::Slerp(camera->GetWorldRotation(), cameraLook, Time::DeltaTime() * 1.0f);
	camera->SetWorldRotation(cameraLook.Normalized());

	controller->targetDistance = defaultDistance;

	if (!controller->CollisionCheck())
	{
		controller->UpdateDistance(controller->targetDistance, controller->distanceSpeed);
	}
}

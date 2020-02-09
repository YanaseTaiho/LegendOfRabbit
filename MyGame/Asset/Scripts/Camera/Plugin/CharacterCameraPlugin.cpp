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
	this->lookHolizontal = 0.0f;
	this->lookVertical = 20.0f;
}

void CharacterCameraPlugin::OnStart(CameraController * controller)
{

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
		lookValue -= 30.0f * Time::DeltaTime();
		input = true;
	}
	if (Input::Keyboad::IsPress('L')) {
		lookValue += 30.0f * Time::DeltaTime();
		input = true;
	}

	if (Input::Mouse::IsPress(Input::Mouse::Button::Left))
	{
		Vector2 mouse;
		Input::Mouse::GetMove(&mouse);
		lookValue += mouse.x * Time::DeltaTime();
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

	// ���������̉�]�p�̎Z�o
	//lookValue = Mathf::Clamp(lookValue, -50.0f, 50.0f);
	lookHolizontal += lookValue;
	lookValue *= 0.9f;

	// �Ɉʕ����̉�]�p�̎Z�o
	float rayLen = 150.0f;
	float forwardRayLen = 15.0f;
	Vector3 rayHeight = Vector3(0.0f, 30.0f, 0.0f);
	Ray rayForward(playerPos, rootForward, forwardRayLen);
	Ray rayForwardDown((playerPos + rayHeight) + (rootForward * forwardRayLen), Vector3::down(), rayLen);
//	Ray rayDown(rootPos, Vector3::down(), rayLen);										

	DebugLine::DrawRay(rayForward.start, rayForward.end, Color::green());
	DebugLine::DrawRay(rayForwardDown.start, rayForwardDown.end, Color::green());

	RayCastInfo castInfo;
	// �O�ɕǂ�����̂����m�F����
	if (RayCast::JudgeAllCollision(&rayForward, &castInfo, player->gameObject))
	{
		// �v���C���[����J�����̕����̑O�������̈ʒu����~�낷���C
		if (RayCast::JudgeAllCollision(&rayForwardDown, &castInfo, player->gameObject))
		{
			float h = castInfo.point.y;
			float targetHeight = 10.0f;
			if (h > player->castGroundInfo.point.y + targetHeight)
			{
				lookVertical = -10.0f;
			}
			else
			{
				lookVertical = 20.0f;
			}
		}
		else
		{
			lookVertical = 20.0f;
		}
	}
	else
	{
		// �v���C���[����J�����̕����̑O�������̈ʒu����~�낷���C
		if (RayCast::JudgeAllCollision(&rayForwardDown, &castInfo, player->gameObject))
		{
			float h = castInfo.point.y;
			float targetHeight = 5.0f;
			if (h < player->castGroundInfo.point.y - targetHeight)
			{
				lookVertical = 40.0f;
			}
			else if (h > player->castGroundInfo.point.y + targetHeight)
			{
				lookVertical = -5.0f;
			}
			else
			{
				lookVertical = 20.0f;
			}
		}
	}
	// �J�����̈ʒu����~�낷���C


	// ��L�̌v�Z���ʂ̔��f
	rootTransform->SetWorldPosition(rootPos);

	rootRotation = Quaternion::AxisAngle(Vector3::up(), lookValue) * rootRotation;
	rootTransform->SetWorldRotation(rootRotation.Normalize());

	// �Ɉʕ����̉�]
	Quaternion vQ = Quaternion::EulerAngles(lookVertical, 0.0f, 0.0f);
	Quaternion vS = Quaternion::Slerp(verticalTransform->GetLocalRotation(), vQ, Time::DeltaTime() * 0.3f);
	verticalTransform->SetLocalRotation(vS.Normalize());

	// �J�������̂̉�]
	auto camera = controller->cameraTransform.lock();
	Quaternion cameraLook;
	cameraLook = Quaternion::LookRotation(playerPos - camera->GetWorldPosition());
	cameraLook = Quaternion::Slerp(camera->GetWorldRotation(), cameraLook, Time::DeltaTime() * 1.0f);
	camera->SetWorldRotation(cameraLook);

	controller->targetDistance = defaultDistance;

	if (!controller->CollisionCheck())
	{
		controller->UpdateDistance(controller->targetDistance, controller->distanceSpeed);
	}
}

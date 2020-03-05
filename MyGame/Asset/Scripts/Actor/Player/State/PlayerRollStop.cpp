#include "PlayerRollStop.h"
#include "../../../Camera/CameraController.h"

void PlayerRollStop::OnStart(PlayerActor * actor)
{
	Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_RollHit);
	// カメラバイブレーション
	actor->cameraController.lock()->vibration.SetVibration(Vector3::up() * 1.3f);

	if (!actor->animator.lock()->IsCurrentAnimation("Roll"))
	{
		actor->ChangeState(PlayerActor::State::Idle);
		return;
	}

	auto rb = actor->rigidbody.lock();

	rb->velocity.x = 0.0f;
	rb->velocity.z = 0.0f;

	rb->AddForce(-actor->transform.lock()->forward() * 60.0f * Time::DeltaTime());

	actor->animator.lock()->SetFloat("WalkValue", 0.0f);
	actor->animator.lock()->SetTrigger("RollStopTrigger");
	actor->animator.lock()->SetAnimationCallBack("RollStop", 27, [=]()
	{
		actor->ChangeState(PlayerActor::State::Idle);
	});
}

void PlayerRollStop::OnUpdate(PlayerActor * actor)
{
	if (!actor->animator.lock()->IsCurrentAnimation("RollStop"))
	{
		actor->ChangeState(PlayerActor::State::Idle);
		return;
	}
}

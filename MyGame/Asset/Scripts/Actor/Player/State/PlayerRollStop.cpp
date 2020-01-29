#include "PlayerRollStop.h"

void PlayerRollStop::OnStart(PlayerActor * actor)
{
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

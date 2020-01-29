#include "PlayerAttackJump.h"

void PlayerAttackJump::OnStart(PlayerActor * actor)
{
	actor->rigidbody.lock()->velocity.y = 0.0f;

	Vector3 force = Vector3::up() * actor->jumpForce * 0.5f;
	force += actor->transform.lock()->forward() * 80.0f * Time::DeltaTime();
	actor->rigidbody.lock()->AddForce(force);

	actor->animator.lock()->SetTrigger("Attack_Jump_Trigger");
}

void PlayerAttackJump::OnUpdate(PlayerActor * actor)
{
	if (!actor->animator.lock()->IsCurrentAnimation("Attack_Jump")
	 && !actor->animator.lock()->IsCurrentAnimation("Attack_Jump_Land"))
	{
		actor->ChangeState(PlayerActor::State::Idle);
	}
}

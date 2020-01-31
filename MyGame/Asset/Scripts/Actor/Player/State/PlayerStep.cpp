#include "PlayerStep.h"

void PlayerStep::OnStart(PlayerActor * actor)
{
	actor->horizontalRegistance = 1.0f;
	actor->animator.lock()->SetTrigger("Step_Trigger");
	actor->animator.lock()->SetTrigger("Step_Trigger_Land", false);
}

void PlayerStep::OnUpdate(PlayerActor * actor)
{
	actor->horizontalRegistance = 1.0f;

	if (!actor->animator.lock()->IsCurrentAnimation("RockOn_Back_Step"))
	{
		if (Input::Keyboad::IsTrigger('R'))
		{
			actor->ChangeState(PlayerActor::State::AttackJump);
			return;
		}
	}
	
	if (actor->onGround 
		&& actor->rigidbody.lock()->velocity.y <= 0.0f
		//&& actor->animator.lock()->GetCurrentPercent() > 0.3f
		)
	{
		actor->animator.lock()->SetTrigger("Step_Trigger_Land");

		// ÃŽ~
		if (actor->moveAmount < 0.1f)
		{
			actor->ChangeState(PlayerActor::State::Idle);
			return;
		}
		if (actor->moveAmount >= 0.1f && actor->moveDir != Vector3::zero())
		{
			actor->ChangeState(PlayerActor::State::Move);
			return;
		}
	}
}

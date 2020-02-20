#include "PlayerAttackFlip.h"

void PlayerAttackFlip::OnStart(PlayerActor * actor)
{
	actor->animator.lock()->SetTrigger("Attack_Flip_Trigger");
	frameCnt = 15;
}

void PlayerAttackFlip::OnUpdate(PlayerActor * actor)
{
	/*if (!actor->animator.lock()->IsCurrentAnimation("Attack_Filter")
		&& !actor->animator.lock()->IsCurrentAnimation("Attack_Flip"))
	{
		actor->ChangeState(PlayerActor::State::Idle);
	}*/
	if (frameCnt <= 0.0f)
	{
		actor->ChangeState(PlayerActor::State::Idle);
	}
	frameCnt--;
}

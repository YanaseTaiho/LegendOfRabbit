#include "PlayerCliffJump.h"

void PlayerCliffJump::OnStart(PlayerActor * actor)
{
	Vector3 dir = -actor->castCliffWallInfo.normal; dir.y = 0.0;
	Quaternion look = Quaternion::LookRotation(dir).Normalized();
	actor->transform.lock()->SetWorldRotation(look);

	actor->rigidbody.lock()->velocity = Vector3::zero();
	actor->animator.lock()->SetTrigger("Cliff_Jump_Trigger");

	actor->animator.lock()->SetAnimationCallBack("Cliff_Jump", 10, [=]()
	{
		Vector3 force = Vector3::up() * actor->cliffJumpForce;
		actor->rigidbody.lock()->AddForce(force);
		actor->ChangeState(PlayerActor::State::Air);
	});
}

void PlayerCliffJump::OnUpdate(PlayerActor * actor)
{
	if(!actor->animator.lock()->IsCurrentAnimation("Cliff_Jump"))
	{
		// ‹ó’†
		if (!actor->onGround)
		{
			actor->ChangeState(PlayerActor::State::Air);
		}
		else
		{
			// ÃŽ~
			actor->ChangeState(PlayerActor::State::Idle);
		}
	}
}

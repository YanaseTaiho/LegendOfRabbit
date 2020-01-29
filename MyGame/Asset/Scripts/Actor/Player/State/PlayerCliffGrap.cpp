#include "PlayerCliffGrap.h"

void PlayerCliffGrap::OnStart(PlayerActor * actor)
{
	actor->animator.lock()->SetBool("IsCliff_Grap", true);

	actor->animator.lock()->SetAnimationCallBack("Cliff_Up", 29, [=]()
	{
		actor->ChangeState(PlayerActor::State::Idle);
	});

	isUp = false;
}

void PlayerCliffGrap::OnUpdate(PlayerActor * actor)
{
	actor->rigidbody.lock()->velocity = Vector3::zero();

	if (isUp && !actor->animator.lock()->IsCurrentAnimation("Cliff_Up"))
	{
		actor->ChangeState(PlayerActor::State::Idle);
	}
	if (!actor->onGround)
	{
		actor->animator.lock()->SetBool("IsCliff_Grap", false);
		actor->ChangeState(PlayerActor::State::Air);
	}

	if (Input::Keyboad::IsTrigger('W'))
	{
		actor->animator.lock()->SetBool("IsCliff_Grap", false);
		actor->animator.lock()->SetTrigger("Cliff_Up_Trigger");
		isUp = true;
	}
}

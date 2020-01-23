#include "PlayerCliffGrap.h"

void PlayerCliffGrap::OnStart(PlayerActor * actor)
{
	actor->animator.lock()->SetBool("IsCliff_Grap", true);

	actor->animator.lock()->SetAnimationCallBack("Cliff_Up", 29, [=]()
	{
		actor->ChangeState(PlayerActor::State::Idle);
	});
}

void PlayerCliffGrap::OnUpdate(PlayerActor * actor)
{
	actor->rigidbody.lock()->velocity = Vector3::zero();
	if (Input::Keyboad::IsTrigger('W'))
	{
		actor->animator.lock()->SetBool("IsCliff_Grap", false);
		actor->animator.lock()->SetTrigger("Cliff_Up_Trigger");
	}

}

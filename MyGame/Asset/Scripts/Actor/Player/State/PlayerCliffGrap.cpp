#include "PlayerCliffGrap.h"

void PlayerCliffGrap::OnStart(PlayerActor * actor)
{
	actor->animator.lock()->SetBool("IsCliff_Grap", true);

	Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_SwordPachin);

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

	if (actor->moveAmount > 0.3f)
	{
		float moveDot = Vector3::Dot(actor->transform.lock()->forward(), actor->moveDir);
		if (moveDot > 0.5f)
		{
			actor->animator.lock()->SetBool("IsCliff_Grap", false);
			actor->animator.lock()->SetTrigger("Cliff_Up_Trigger");
			isUp = true;
		}
	}
}

void PlayerCliffGrap::OnDestroy(PlayerActor * actor)
{
	actor->animator.lock()->SetBool("IsCliff_Grap", false);
}

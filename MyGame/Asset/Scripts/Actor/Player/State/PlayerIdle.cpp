#include "PlayerIdle.h"
#include "PlayerMove.h"
#include "../PlayerActor.h"

void PlayerIdle::OnStart(PlayerActor * actor)
{
	actor->animator.lock()->SetFloat("WalkValue", 0.0f);
}

void PlayerIdle::OnUpdate(PlayerActor * actor)
{
	if (Input::Keyboad::IsTrigger(VK_SPACE))
	{
		Vector3 force = Vector3::up() * actor->jumpForce;
		actor->rigidbody.lock()->AddForce(force);
		actor->animator.lock()->SetTrigger("JumpTrigger");
		actor->ChangeState(PlayerActor::State::Air);
		return;
	}

	if (!actor->onGround)
	{
		actor->ChangeState(PlayerActor::State::Air);
		return;
	}

	if (Input::Keyboad::IsTrigger('E'))
	{
		actor->ChangeState(PlayerActor::State::Attack);
		return;
	}

	if (actor->isRockOn)
	{
		if (Input::Keyboad::IsTrigger('Q'))
		{
			actor->ChangeState(PlayerActor::State::AttackJump);
			return;
		}
	}
	

	if (actor->moveAmount > 0.1f && actor->moveDir != Vector3::zero())
	{
		//Quaternion look = Quaternion::LookRotation(actor->moveDir);
		//actor->transform.lock()->SetWorldRotation(look);

		actor->ChangeState(PlayerActor::State::Move);
	}
}

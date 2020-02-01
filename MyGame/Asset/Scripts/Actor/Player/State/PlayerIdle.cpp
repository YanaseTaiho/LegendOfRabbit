#include "PlayerIdle.h"
#include "PlayerMove.h"
#include "PlayerAttack.h"
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

	if (Input::Keyboad::IsTrigger('1'))
	{
		if (!actor->isWeaponHold) actor->WeaponHold();
		else actor->WeaponNotHold();
	}

	if (Input::Keyboad::IsTrigger('E'))
	{
		if (actor->isWeaponHold)
		{
			actor->ChangeState(PlayerActor::State::Attack);
		}
		else
		{
			actor->WeaponHold([=]()
			{
				actor->ChangeState(PlayerActor::State::Attack);
				auto attack = std::static_pointer_cast<PlayerAttack>(actor->fsmManager->GetState((int)PlayerActor::State::Attack).lock());
				attack->Attack(actor);
			});
		}
		return;
	}

	if (actor->isRockOn)
	{
		if (Input::Keyboad::IsTrigger('R'))
		{
			if (actor->isWeaponHold)
			{
				actor->ChangeState(PlayerActor::State::AttackJump);
			}
			else
			{
				actor->WeaponHold([=]()
				{
					actor->ChangeState(PlayerActor::State::AttackJump);
				});
			}
			return;
		}
	}
	

	if (actor->moveAmount > 0.1f && actor->moveDir != Vector3::zero()
		&& !actor->animator.lock()->IsCurrentAnimation("Weapon_Change"))
	{
		//Quaternion look = Quaternion::LookRotation(actor->moveDir);
		//actor->transform.lock()->SetWorldRotation(look);

		actor->ChangeState(PlayerActor::State::Move);
	}
}

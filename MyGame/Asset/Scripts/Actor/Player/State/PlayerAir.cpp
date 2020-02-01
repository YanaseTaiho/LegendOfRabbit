#include "PlayerAir.h"
#include "PlayerIdle.h"
#include "PlayerMove.h"

#include "../PlayerActor.h"

void PlayerAir::OnStart(PlayerActor * actor)
{
	moveSpeed = 2.0f;
}

void PlayerAir::OnUpdate(PlayerActor * actor)
{	
	bool isCliffJump = actor->animator.lock()->IsCurrentAnimation("Cliff_Jump");

	if (Input::Keyboad::IsTrigger(VK_SPACE))
	{
		Vector3 force = Vector3::up() * actor->jumpForce;
		actor->rigidbody.lock()->AddForce(force);
		actor->animator.lock()->SetTrigger("JumpTrigger");
	}

	// �R�͂ݔ���
	if (!actor->castCliffGroundInfo.collision.expired() && !actor->castCliffWallInfo.collision.expired()
		&& actor->rigidbody.lock()->velocity.y < 0										// �͂��������ɓ����Ă��鎞�̂�
		&& actor->castCliffGroundInfo.point.y - actor->castCliffWallInfo.point.y < 5.0f	// �������m�F
		&& Vector3::Dot(actor->castCliffGroundInfo.normal, Vector3::up()) > 0.7f)		// �͂߂�R�̊p�x���m�F
	{
		Vector3 dir = -actor->castCliffWallInfo.normal; dir.y = 0.0;
		Quaternion look = Quaternion::LookRotation(dir).Normalized();
		actor->transform.lock()->SetWorldRotation(look);

		actor->transform.lock()->SetWorldPosition(actor->castCliffGroundInfo.point);
		actor->ChangeState(PlayerActor::State::CliffGrap);
		return;
	}

	if (actor->onGround)
	{
		//actor->rigidbody.lock()->resistance = 0.9f;
		if (actor->moveAmount > 0.1f)
		{
			actor->ChangeState(PlayerActor::State::Move);
			return;
		}
		else
		{
			actor->ChangeState(PlayerActor::State::Idle);
			return;
		}
	}

	if (!isCliffJump)
	{
		// �W�����v�؂�
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
				actor->ChangeState(PlayerActor::State::Idle);
			}

			return;
		}

		Vector3 force;
		auto rb = actor->rigidbody.lock();
		force += actor->moveDir * (actor->moveAmount * moveSpeed * Time::DeltaTime());
		rb->AddForce(force);
	}
}

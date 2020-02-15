#include "PlayerMove.h"
#include "PlayerAttack.h"
#include "../PlayerActor.h"
#include "../../../../DirectX/Common.h"

PlayerMove::PlayerMove()
{
	frameCnt = 0;
	lookSpeed = 10.0f;
}

void PlayerMove::OnStart(PlayerActor * actor)
{
	frameCnt = 0;
	actor->animator.lock()->SetFloat("WalkValue", actor->forceAmount);
}

void PlayerMove::OnUpdate(PlayerActor * actor)
{
	actor->animator.lock()->SetFloat("WalkValue", actor->forceAmount);

	if (Input::Keyboad::IsTrigger(VK_SPACE))
	{
		actor->rigidbody.lock()->AddForce(Vector3::up() * actor->jumpForce);
		actor->animator.lock()->SetTrigger("JumpTrigger");
		actor->ChangeState(PlayerActor::State::Air);
	}

	// �Î~
	if (actor->moveAmount < 0.1f)
	{
		actor->ChangeState(PlayerActor::State::Idle);
		return;
	}
	if (Input::Keyboad::IsTrigger('E') || GamePad::IsTrigger(GamePad::Button::A))
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
			actor->ChangeState(PlayerActor::State::Idle);
			//return;
		}
		
		return;
	}

	Vector3 forward = actor->transform.lock()->forward();
	forward.y = 0.0f;

	// ���b�N�I����
	if (actor->isRockOn)
	{
		frameCnt = 0;

		// �^�[�Q�b�g������΂�����������
		if (!actor->targetTransform.expired())
		{
			Vector3 dir = actor->targetTransform.lock()->GetWorldPosition() - actor->transform.lock()->GetWorldPosition();
			dir.y = 0.0f;
			Quaternion look = Quaternion::LookRotation(dir);
			Quaternion rot = actor->transform.lock()->GetWorldRotation();
			rot = rot.Slerp(look, Time::DeltaTime() * 10.0f);
			actor->transform.lock()->SetWorldRotation(rot);
		}

		CheckDirection(actor);

		if (actor->moveDir != Vector3::zero()
			&& (Input::Keyboad::IsTrigger('R') || GamePad::IsTrigger(GamePad::Button::B)))
		{
			switch (moveDirection)
			{
				// �W�����v�؂�
			case PlayerActor::Direction::Forward:

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

				// �T�C�h�X�e�b�v
			case PlayerActor::Direction::Left:
			case PlayerActor::Direction::Right:

			{
				Vector3 force = Vector3::up() * actor->jumpForce * 0.50f;
				force += actor->moveDir * 200.0f * Time::DeltaTime();
				actor->rigidbody.lock()->AddForce(force);

				actor->ChangeState(PlayerActor::State::Step);
				return;
			}
			// �o�N��
			case PlayerActor::Direction::Back:
			{
				Vector3 force = Vector3::up() * actor->jumpForce * 0.75f;
				force += actor->moveDir * 70.0f * Time::DeltaTime();
				actor->rigidbody.lock()->AddForce(force);

				actor->ChangeState(PlayerActor::State::Step);
				return;
			}

			}
		}
		
		// �ړ�����
		Vector3 force;
		force += actor->moveDir * (actor->moveAmount * actor->moveForce * Time::DeltaTime());
		auto rb = actor->rigidbody.lock();
		rb->AddForce(force);
	}
	// �ʏ�
	else
	{
		// �R������̂����m�F
		if (!actor->castCliffGroundInfo.collision.expired()
			&& Vector3::Dot(actor->castCliffGroundInfo.normal, Vector3::up()) > 0.7f)	// �͂߂�R�̊p�x���m�F)
		{
			frameCnt++;
			if (frameCnt > 15)
			{
				frameCnt = 0;
				actor->ChangeState(PlayerActor::State::CliffJump);
				return;
			}
		}
		else
		{
			frameCnt = 0;
		}

		// ��
		if (!actor->onGround)
		{
			//�W�����v����
			Ray downRay;
			RayCastInfo info;
			Vector3 pos = actor->transform.lock()->GetWorldPosition() + actor->transform.lock()->forward();
			downRay.Set(pos + Vector3(0.0f, actor->rayStart, 0.0f), Vector3::down(), actor->rayLength * 1.3f);
			DebugLine::DrawRay(downRay.start, downRay.end, Color::yellow());
			if (!RayCast::JudgeAllCollision(&downRay, &info, actor->gameObject) || Vector3::Dot(info.normal, Vector3::up()) < 0.8f)
			{
				actor->rigidbody.lock()->AddForce(Vector3::up() * actor->jumpForce * actor->forceAmount);
				actor->animator.lock()->SetTrigger("JumpTrigger");
			}

			actor->ChangeState(PlayerActor::State::Air);
			return;
		}

		// �]����
		if (actor->moveAmount > 0.1f
			&& (Input::Keyboad::IsTrigger('R') || GamePad::IsTrigger(GamePad::Button::B)))
		{
			actor->ChangeState(PlayerActor::State::Roll);
			return;
		}

		// ��]����
		Quaternion q1 = actor->transform.lock()->GetWorldRotation();
		if (actor->moveAmount > 0.1f && actor->moveDir != Vector3::zero())
		{
			Quaternion look = Quaternion::LookRotation(actor->moveDir).Normalized();
			bool flag = Vector3::Dot(forward, actor->moveDir) >= -1.0f;
			q1 = (flag) ?
				q1.Slerp(look, Time::DeltaTime() * lookSpeed) : look;

			actor->transform.lock()->SetWorldRotation(q1.Normalize());
		}

		// �ړ�����
		Vector3 force;
		forward = actor->transform.lock()->forward();
		force += forward * (actor->moveAmount * actor->moveForce * Time::DeltaTime());
		Rigidbody *rb = actor->rigidbody.lock().get();
		rb->AddForce(force);
	}
}

void PlayerMove::CheckDirection(PlayerActor * actor)
{
	float attackDir = Vector3::Dot(actor->transform.lock()->forward(), actor->moveDir);
	bool isRight = Vector3::Dot(actor->transform.lock()->right(), actor->moveDir) >= 0;

	float boundar = 0.90f;

	if (attackDir >= boundar)
	{
		moveDirection = PlayerActor::Direction::Forward;
	}
	else if (!isRight && attackDir > -boundar && attackDir < boundar)
	{
		moveDirection = PlayerActor::Direction::Left;
	}
	else if (isRight && attackDir > -boundar && attackDir < boundar)
	{
		moveDirection = PlayerActor::Direction::Right;
	}
	else if (attackDir <= -boundar)
	{
		moveDirection = PlayerActor::Direction::Back;
	}

	actor->animator.lock()->SetInt("RockOnDirection", (int)moveDirection);
	//actor->animator.lock()->SetFloat("WalkValue", actor->moveAmount);
}

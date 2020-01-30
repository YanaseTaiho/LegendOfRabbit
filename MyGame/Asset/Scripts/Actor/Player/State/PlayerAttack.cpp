#include "PlayerAttack.h"
#include "../../../RotationFixedController.h"

void PlayerAttack::OnStart(PlayerActor * actor)
{
	frameCnt = 0;
	combo = 0;
	actor->rigidbody.lock()->velocity.x = 0.0f;
	actor->rigidbody.lock()->velocity.z = 0.0f;

	OnUpdate(actor);
}

void PlayerAttack::OnUpdate(PlayerActor * actor)
{
	actor->animator.lock()->SetFloat("WalkValue", actor->forceAmount);
	actor->horizontalRegistance = 0.90f;
	actor->sword_HandContorller.lock()->SetWeight(-1.0f);

	if (!actor->onGround)
	{
		actor->ChangeState(PlayerActor::State::Air);
		return;
	}

	if (!actor->animator.lock()->IsCurrentAnimation("Attack_Inside_Filter")
		&& !actor->animator.lock()->IsCurrentAnimation("Attack_Outside_Filter")
		&& !actor->animator.lock()->IsCurrentAnimation("Attack_Upper_Filter")
		&& !actor->animator.lock()->IsCurrentAnimation("Attack_Thrust_Filter"))
	{
		if (Input::Keyboad::IsTrigger('E'))
		{
			CheckAttackType(actor);

			actor->animator.lock()->SetTrigger("Attack_Trigger");
			actor->animator.lock()->SetInt("Attack_Type", (int)attackType);
			actor->animator.lock()->SetInt("Attack_Combo", combo);

			// ���b�N�I����
			if (actor->isRockOn)
			{
				Vector3 forward = actor->transform.lock()->forward();
				float attackDot = Vector3::Dot(forward, actor->moveDir);
				if (attackDot < 0.3f) attackDot = 0.3f;
				Vector3 force = forward * attackDot * Time::DeltaTime() * 150.0f;
				actor->rigidbody.lock()->AddForce(force);
			}
			// �ʏ펞
			else
			{
				if (actor->moveAmount > 0.1f && actor->moveDir != Vector3::zero())
				{
					Quaternion look = Quaternion::LookRotation(actor->moveDir);
					actor->transform.lock()->SetWorldRotation(look);

					Vector3 forward = actor->transform.lock()->forward();
					Vector3 force = forward * Time::DeltaTime() * 100.0f;
					actor->rigidbody.lock()->AddForce(force);
				}
			}

			if (combo == 2)
			{
				Vector3 forward = actor->transform.lock()->forward();
				Vector3 force = forward * Time::DeltaTime() * 100.0f;
				actor->rigidbody.lock()->AddForce(force);
			}

			combo++;
			combo = Mathf::Loop(combo, 0, 2);
		}

		frameCnt++;
		if (frameCnt > 8)
		{
			if (actor->moveAmount > 0.2f)
			{
				actor->ChangeState(PlayerActor::State::Move);
			}
			else
			{
				actor->ChangeState(PlayerActor::State::Idle);
			}
		}
	}
	else
	{
		frameCnt = 0;
	}
	ImGui::Text("Combo : %d", combo);
}

void PlayerAttack::CheckAttackType(PlayerActor * actor)
{
	auto SetAttackType = [&](PlayerActor::AttackType type)
	{
		if (attackType != type) combo = 0;
		attackType = type;
	};

	if (actor->isRockOn)
	{
		float attackDir = Vector3::Dot(actor->transform.lock()->forward(), actor->moveDir);
		bool isRight = Vector3::Dot(actor->transform.lock()->right(), actor->moveDir) >= 0;

		if (actor->moveDir == Vector3::zero())
		{
			SetAttackType(PlayerActor::AttackType::Upper);
		}
		else if (attackDir >= 0.5f)
		{
			SetAttackType(PlayerActor::AttackType::Thrust);
		}
		else if (!isRight && attackDir > -0.5f && attackDir < 0.5f)
		{
			SetAttackType(PlayerActor::AttackType::Outside);
		}
		else if (isRight && attackDir > -0.5f && attackDir < 0.5f)
		{
			SetAttackType(PlayerActor::AttackType::Inside);
		}
		else if (attackDir <= -0.5f)
		{
			SetAttackType(PlayerActor::AttackType::Inside);
		}
	}
	else
	{
		if (actor->moveAmount > 0.3f)
		{
			SetAttackType(PlayerActor::AttackType::Upper);
		}
		else
		{
			SetAttackType(PlayerActor::AttackType::Inside);
		}
	}
}

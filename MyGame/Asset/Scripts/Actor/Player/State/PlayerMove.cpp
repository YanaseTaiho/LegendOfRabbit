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

	// 静止
	if (actor->moveAmount < 0.1f)
	{
		actor->ChangeState(PlayerActor::State::Idle);
		return;
	}
	// 空中
	if (!actor->onGround)
	{
		//ジャンプ処理
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
			actor->ChangeState(PlayerActor::State::Idle);
			//return;
		}
		
		return;
	}

	// 崖があるのかを確認
	if (!actor->castCliffGroundInfo.collision.expired()
		&& Vector3::Dot(actor->castCliffGroundInfo.normal, Vector3::up()) > 0.7f)	// 掴める崖の角度を確認)
	{
		frameCnt++;
		if (frameCnt > 10)
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

	Vector3 forward = actor->transform.lock()->forward();
	forward.y = 0.0f;

	// ロックオン中
	if (actor->isRockOn)
	{
		CheckDirection(actor);

		if (actor->moveDir != Vector3::zero() && Input::Keyboad::IsTrigger('R'))
		{
			switch (moveDirection)
			{
				// ジャンプ切り
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

				// サイドステップ
			case PlayerActor::Direction::Left:
			case PlayerActor::Direction::Right:

			{
				Vector3 force = Vector3::up() * actor->jumpForce * 0.50f;
				force += actor->moveDir * 200.0f * Time::DeltaTime();
				actor->rigidbody.lock()->AddForce(force);

				actor->ChangeState(PlayerActor::State::Step);
				return;
			}
			// バク宙
			case PlayerActor::Direction::Back:
			{
				Vector3 force = Vector3::up() * actor->jumpForce * 0.75f;
				force += actor->moveDir * 100.0f * Time::DeltaTime();
				actor->rigidbody.lock()->AddForce(force);

				actor->ChangeState(PlayerActor::State::Step);
				return;
			}

			}	// !switch (moveDirection)
		}	// !actor->moveDir != Vector3::zero() && Input::Keyboad::IsTrigger('Q')
		
		// 移動処理
		Vector3 force;
		force += actor->moveDir * (actor->moveAmount * actor->moveForce * Time::DeltaTime());
		auto rb = actor->rigidbody.lock();
		rb->AddForce(force);
	}
	// 通常
	else
	{
		// 転がる
		if (Input::Keyboad::IsTrigger('R') && actor->moveAmount > 0.1f)
		{
			actor->ChangeState(PlayerActor::State::Roll);
			return;
		}

		// 回転処理
		Quaternion q1 = actor->transform.lock()->GetWorldRotation();
		if (actor->moveAmount > 0.1f && actor->moveDir != Vector3::zero())
		{
			Quaternion look = Quaternion::LookRotation(actor->moveDir).Normalized();
			bool flag = Vector3::Dot(forward, actor->moveDir) >= -1.0f;
			q1 = (flag) ?
				q1.Slerp(look, Time::DeltaTime() * lookSpeed) : look;

			actor->transform.lock()->SetWorldRotation(q1.Normalize());
		}

		// 移動処理
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

	if (attackDir >= 0.9f)
	{
		moveDirection = PlayerActor::Direction::Forward;
	}
	else if (!isRight && attackDir > -0.9f && attackDir < 0.9f)
	{
		moveDirection = PlayerActor::Direction::Left;
	}
	else if (isRight && attackDir > -0.9f && attackDir < 0.9f)
	{
		moveDirection = PlayerActor::Direction::Right;
	}
	else if (attackDir <= -0.9f)
	{
		moveDirection = PlayerActor::Direction::Back;
	}

	actor->animator.lock()->SetInt("RockOnDirection", (int)moveDirection);
	//actor->animator.lock()->SetFloat("WalkValue", actor->moveAmount);
}

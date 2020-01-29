#include "PlayerMove.h"
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
	// ��
	if (!actor->onGround)
	{
		//�W�����v����
		Ray downRay;
		RayCastInfo info;
		Vector3 pos = actor->transform.lock()->GetWorldPosition() + actor->transform.lock()->forward();
		downRay.Set(pos + Vector3(0.0f, actor->rayStart, 0.0f), Vector3::down(), actor->rayLength * 1.3f);
		DebugLine::DrawRay(downRay.start, downRay.end, Color::yellow());
		if (!RayCast::JudgeAllCollision(&downRay, &info) || Vector3::Dot(info.normal, Vector3::up()) < 0.8f)
		{
			actor->rigidbody.lock()->AddForce(Vector3::up() * actor->jumpForce * actor->forceAmount);
			actor->animator.lock()->SetTrigger("JumpTrigger");
		}

		actor->ChangeState(PlayerActor::State::Air);
		return;
	}
	if (Input::Keyboad::IsTrigger('E'))
	{
		actor->ChangeState(PlayerActor::State::Attack);
		return;
	}
	// �]����
	if (Input::Keyboad::IsTrigger('R') && actor->moveAmount > 0.1f)
	{
		actor->ChangeState(PlayerActor::State::Roll);
		return;
	}

	// �R������̂����m�F
	if (!actor->castCliffGroundInfo.collision.expired()
		&& Vector3::Dot(actor->castCliffGroundInfo.normal, Vector3::up()) > 0.7f)	// �͂߂�R�̊p�x���m�F)
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

	if (actor->isRockOn)
	{
		if (Input::Keyboad::IsTrigger('Q'))
		{
			actor->ChangeState(PlayerActor::State::AttackJump);
			return;
		}

		// �ړ�����
		Vector3 force;
		force += actor->moveDir * (actor->moveAmount * actor->moveForce * Time::DeltaTime());
		auto rb = actor->rigidbody.lock();
		rb->AddForce(force);
	}
	else
	{
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

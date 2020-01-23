#include "PlayerIdle.h"
#include "PlayerMove.h"
#include "../PlayerActor.h"

PlayerMove::PlayerMove()
{
	frameCnt = 0;
	lookSpeed = 5.0f;
}

void PlayerMove::OnStart(PlayerActor * actor)
{
	frameCnt = 0;
	actor->animator.lock()->SetFloat("WalkValue", actor->moveAmount);

	actor->animator.lock()->SetAnimationCallBack("Cliff_Jump", 10, [=]()
	{
		Vector3 force = Vector3::up() * actor->cliffJumpForce;
		actor->rigidbody.lock()->AddForce(force);
		actor->ChangeState(PlayerActor::State::Air);
	});
}

void PlayerMove::OnUpdate(PlayerActor * actor)
{
	if (actor->moveAmount < 0.05f)
	{
		actor->ChangeState(PlayerActor::State::Idle);
		return;
	}

	actor->animator.lock()->SetFloat("WalkValue", actor->forceAmount);

	if (!actor->castCliffGroundInfo.collision.expired())
	{
		frameCnt++;
		if (frameCnt > 10)
		{
			frameCnt = 0;
			Vector3 dir = -actor->castCliffWallInfo.normal; dir.y = 0.0;
			Quaternion look = Quaternion::LookRotation(dir).Normalized();
			actor->transform.lock()->SetWorldRotation(look);

			actor->rigidbody.lock()->velocity = Vector3::zero();
			actor->animator.lock()->SetTrigger("Cliff_Jump_Trigger");
			return;
		}
	}
	else
	{
		frameCnt = 0;
	}

	if (!actor->animator.lock()->IsCurrentAnimation("Cliff_Jump"))
	{
		Vector3 forward = actor->transform.lock()->forward();

		Quaternion q1 = actor->transform.lock()->GetWorldRotation();
		if (actor->moveAmount > 0.1f)
		{
			Quaternion look = Quaternion::LookRotation(actor->moveDir).Normalized();
			bool flag = Vector3::Dot(forward, actor->moveDir) >= 0;
			q1 = (flag) ?
				q1.Slerp(look, Time::DeltaTime() * lookSpeed) : look;

			actor->transform.lock()->SetWorldRotation(q1.Normalize());
		}

		forward = actor->transform.lock()->forward();

		Vector3 force;
		force += forward * (actor->moveAmount * actor->moveForce * Time::DeltaTime());

		if (Input::Keyboad::IsTrigger(VK_SPACE))
		{
			force += Vector3::up() * actor->jumpForce;
			actor->animator.lock()->SetTrigger("JumpTrigger");
			actor->ChangeState(PlayerActor::State::Air);
		}

		if (Input::Keyboad::IsTrigger('R') && actor->moveAmount > 0.1f)
		{
			actor->animator.lock()->SetTrigger("RollTrigger");
			force += forward * (50.0f * Time::DeltaTime());
		}

		Rigidbody *rb = actor->rigidbody.lock().get();
		rb->AddForce(force);
	}
}

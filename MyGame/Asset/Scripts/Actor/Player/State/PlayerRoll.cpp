#include "PlayerRoll.h"
#include "../../../../DirectX/Common.h"

void PlayerRoll::OnStart(PlayerActor * actor)
{
	actor->animator.lock()->SetTrigger("RollTrigger");
	Vector3 force;
	actor->rigidbody.lock()->AddForce(actor->transform.lock()->forward() * (80.0f * Time::DeltaTime()));
}

void PlayerRoll::OnUpdate(PlayerActor * actor)
{
	actor->horizontalRegistance = 1.0f;

	bool isRool = actor->animator.lock()->IsCurrentAnimation("Roll");

	if (!isRool)
	{
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
	else if(actor->animator.lock()->GetCurrentPercent() < 0.8f)
	{
		Ray downRay;
		RayCastInfo info;
		Vector3 forward = actor->transform.lock()->forward();

		Vector3 pos = actor->transform.lock()->GetWorldPosition() + forward;
		downRay.Set(pos + Vector3(0.0f, actor->rayStart * 0.2f, 0.0f), forward, actor->cliffRayLength_Front * 1.0f);
		DebugLine::DrawRay(downRay.start, downRay.end, Color::red());

		if (RayCast::JudgeAllCollision(&downRay, &info) && Vector3::Dot(-info.normal, forward) > 0.8f)
		{
			actor->ChangeState(PlayerActor::State::RollStop);
			return;
		}
	}
	else
	{
		actor->horizontalRegistance = 0.8f;
	}

	if (!actor->onGround)
	{
		//ƒWƒƒƒ“ƒvˆ—
		Ray downRay;
		RayCastInfo info;
		Vector3 pos = actor->transform.lock()->GetWorldPosition() + actor->transform.lock()->forward();
		downRay.Set(pos + Vector3(0.0f, actor->rayStart, 0.0f), Vector3::down(), actor->rayLength * 1.3f);
		DebugLine::DrawRay(downRay.start, downRay.end, Color::yellow());
		if (!RayCast::JudgeAllCollision(&downRay, &info) || Vector3::Dot(info.normal, Vector3::up()) < 0.8f)
		{
			actor->rigidbody.lock()->AddForce(Vector3::up() * actor->jumpForce * actor->forceAmount);
			actor->animator.lock()->SetTrigger("JumpTrigger");
			actor->ChangeState(PlayerActor::State::Air);
		}

		actor->ChangeState(PlayerActor::State::Air);
		return;
	}

	// ˆÚ“®ˆ—
	Vector3 force;
	Vector3 forward = actor->transform.lock()->forward() * 1.1f;
	float moveDot = Vector3::Dot(forward, actor->moveDir); 
	if (moveDot < 0.0f) moveDot = 0.0f;

	force += forward * (actor->moveAmount * actor->moveForce * moveDot * Time::DeltaTime());
	Rigidbody *rb = actor->rigidbody.lock().get();
	rb->AddForce(force);
}

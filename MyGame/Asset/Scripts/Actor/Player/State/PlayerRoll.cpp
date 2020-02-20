#include "PlayerRoll.h"
#include "../../../../DirectX/Common.h"

void PlayerRoll::OnStart(PlayerActor * actor)
{
	actor->animator.lock()->SetTrigger("RollTrigger");

	actor->animator.lock()->SetAnimationCallBack("Roll", 1, [=]()
	{
		Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Roll);
		actor->rigidbody.lock()->AddForce(actor->transform.lock()->forward() * (50.0f * Time::DeltaTime()));
	});

	Quaternion look = Quaternion::LookRotation(actor->moveDir);
	actor->transform.lock()->SetWorldRotation(look);

	// 頭のコリジョンをこの時だけトリガーに変更
	if (!actor->headCollision.expired())
	{
		actor->headCollision.lock()->isTrigger = true;
	}
}

void PlayerRoll::OnUpdate(PlayerActor * actor)
{
	actor->horizontalRegistance = 0.9f;

	float parcent = actor->animator.lock()->GetCurrentPercent();
	bool isRoll = actor->animator.lock()->IsCurrentAnimation("Roll");

	if (!isRoll)
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

	if(parcent < 0.7f)
	{
		Ray downRay;
		RayCastInfo info;
		Vector3 forward = actor->transform.lock()->forward();

		Vector3 pos = actor->transform.lock()->GetWorldPosition() + forward;
		downRay.Set(pos + Vector3(0.0f, actor->rayStart * 0.2f, 0.0f), forward, actor->cliffRayLength_Front * 1.0f);
		DebugLine::DrawRay(downRay.start, downRay.end, Color::red());

		if (RayCast::JudgeAllCollision(&downRay, &info, actor->rigidbody.lock()->collisions) && Vector3::Dot(-info.normal, forward) > 0.8f)
		{
			actor->ChangeState(PlayerActor::State::RollStop);
			return;
		}
	}
	else if (parcent < 0.9f)
	{
		actor->horizontalRegistance = 0.6f;
	}
	else 
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
			actor->ChangeState(PlayerActor::State::Air);
			return;
		}

		actor->ChangeState(PlayerActor::State::Air);
		return;
	}

	// 移動処理
	Vector3 force;
	Vector3 forward = actor->transform.lock()->forward() * 1.1f;
	float moveDot = Vector3::Dot(forward, actor->moveDir); 
	if (moveDot < 0.0f) moveDot = 0.0f;

	force += forward * (actor->moveAmount * actor->moveForce * moveDot * Time::DeltaTime());
	Rigidbody *rb = actor->rigidbody.lock().get();
	rb->AddForce(force);
}

void PlayerRoll::OnDestroy(PlayerActor * actor)
{
	// 頭のコリジョンを戻す
	if (!actor->headCollision.expired())
	{
		actor->headCollision.lock()->isTrigger = false;
	}
}

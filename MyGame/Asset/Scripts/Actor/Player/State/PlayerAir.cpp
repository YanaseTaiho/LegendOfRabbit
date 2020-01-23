#include "PlayerAir.h"
#include "PlayerIdle.h"
#include "PlayerMove.h"

#include "../PlayerActor.h"

void PlayerAir::OnStart(PlayerActor * actor)
{
	//actor->rigidbody.lock()->resistance = 0.98f;
}

void PlayerAir::OnUpdate(PlayerActor * actor)
{	
	if (Input::Keyboad::IsTrigger(VK_SPACE))
	{
		Vector3 force = Vector3::up() * actor->jumpForce;
		actor->rigidbody.lock()->AddForce(force);
		actor->animator.lock()->SetTrigger("JumpTrigger");
		//actor->ChangeState(PlayerActor::State::Air);
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

	if (!actor->castCliffGroundInfo.collision.expired()
		&& actor->rigidbody.lock()->velocity.y < 0
		&& actor->castCliffGroundInfo.point.y - actor->castCliffWallInfo.point.y < 5.0f)
	{
		Vector3 dir = -actor->castCliffWallInfo.normal; dir.y = 0.0;
		Quaternion look = Quaternion::LookRotation(dir).Normalized();
		actor->transform.lock()->SetWorldRotation(look);

		actor->transform.lock()->SetWorldPosition(actor->castCliffGroundInfo.point);
		actor->ChangeState(PlayerActor::State::CliffGrap);
		return;
	}

	if (!actor->animator.lock()->IsCurrentAnimation("Cliff_Jump"))
	{
		Vector3 force;
		auto rb = actor->rigidbody.lock();
		force += actor->moveDir * (actor->moveAmount * moveSpeed * Time::DeltaTime());
		rb->AddForce(force);
	}
}

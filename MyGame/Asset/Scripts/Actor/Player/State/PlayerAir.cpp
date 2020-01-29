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
	if (Input::Keyboad::IsTrigger(VK_SPACE))
	{
		Vector3 force = Vector3::up() * actor->jumpForce;
		actor->rigidbody.lock()->AddForce(force);
		actor->animator.lock()->SetTrigger("JumpTrigger");
	}

	// ジャンプ切り
	if (Input::Keyboad::IsTrigger('Q'))
	{
		actor->ChangeState(PlayerActor::State::AttackJump);
		return;
	}

	// 崖掴み判定
	if (!actor->castCliffGroundInfo.collision.expired() && !actor->castCliffWallInfo.collision.expired()
		&& actor->rigidbody.lock()->velocity.y < 0										// 力が下方向に働いている時のみ
		&& actor->castCliffGroundInfo.point.y - actor->castCliffWallInfo.point.y < 5.0f	// 高さを確認
		&& Vector3::Dot(actor->castCliffGroundInfo.normal, Vector3::up()) > 0.7f)		// 掴める崖の角度を確認
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

	if (!actor->animator.lock()->IsCurrentAnimation("Cliff_Jump"))
	{
		Vector3 force;
		auto rb = actor->rigidbody.lock();
		force += actor->moveDir * (actor->moveAmount * moveSpeed * Time::DeltaTime());
		rb->AddForce(force);
	}
}

#include "PlayerAttackJump.h"
#include "../../../LocusController.h"

void PlayerAttackJump::OnStart(PlayerActor * actor)
{
	actor->rigidbody.lock()->velocity = Vector3::zero();
	actor->horizontalRegistance = 1.0f;

	//float jumpForce = (actor->onGround) ? 0.8f : 0.5f;
	Vector3 force = Vector3::up() * actor->jumpForce * 0.5f;
	force += actor->transform.lock()->forward() * 80.0f * Time::DeltaTime();
	actor->rigidbody.lock()->AddForce(force);


	actor->animator.lock()->SetTrigger("Attack_Jump_Trigger");
	actor->animator.lock()->SetTrigger("Attack_Jump_Land_Trigger", false);

	actor->animator.lock()->SetAnimationCallBack("Attack_Jump", 10, [=]()
	{
		// 軌跡スタート
		if (!actor->locusController.expired())
		{
			actor->locusController.lock()->LocusStart();
		}
	});

	actor->animator.lock()->SetAnimationCallBack("Attack_Jump_Land", 20, [=]()
	{
		actor->ChangeState(PlayerActor::State::Idle);
	});

}

void PlayerAttackJump::OnUpdate(PlayerActor * actor)
{
	if (actor->onGround && actor->rigidbody.lock()->velocity.y <= 0.0f)
		actor->animator.lock()->SetTrigger("Attack_Jump_Land_Trigger");	// 着地モーショントリガー
	else
	{
		actor->horizontalRegistance = 1.0f;
		// 軌跡スタート
		if (!actor->locusController.expired()
			&& !actor->locusController.lock()->isStart
			&& actor->animator.lock()->IsCurrentAnimation("Attack_Jump")
			&& actor->animator.lock()->GetCurrentPercent() > 0.5f)
		{
			actor->locusController.lock()->LocusStart();
		}
	}

	if (!actor->animator.lock()->IsCurrentAnimation("Attack_Jump")
		&& !actor->animator.lock()->IsCurrentAnimation("Attack_Jump_Land")
		&& !actor->animator.lock()->IsCurrentAnimation("Weapon_Change"))
	{
		actor->ChangeState(PlayerActor::State::Idle);
	}
}

#include "PlayerStep.h"

void PlayerStep::OnStart(PlayerActor * actor)
{
	actor->horizontalRegistance = 1.0f;
	actor->animator.lock()->SetTrigger("Step_Trigger");
	actor->animator.lock()->SetTrigger("Step_Trigger_Land", false);
}

void PlayerStep::OnUpdate(PlayerActor * actor)
{
	actor->horizontalRegistance = 1.01f;


	// ƒ^[ƒQƒbƒg‚ª‚¢‚ê‚Î‚»‚Á‚¿‚ðŒü‚­
	if (!actor->targetTransform.expired())
	{
		Vector3 dir = actor->targetTransform.lock()->GetWorldPosition() - actor->transform.lock()->GetWorldPosition();
		dir.y = 0.0f;
		Quaternion look = Quaternion::LookRotation(dir);
		Quaternion rot = actor->transform.lock()->GetWorldRotation();
		rot = rot.Slerp(look, Time::DeltaTime() * 20.0f);
		actor->transform.lock()->SetWorldRotation(rot);
	}


	if (!actor->animator.lock()->IsCurrentAnimation("RockOn_Back_Step"))
	{
		if (actor->GetInput(PlayerActor::InputKey::A_Trigger))
		{
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
		}
	}
	
	if (actor->onGround && actor->rigidbody.lock()->velocity.y <= 0.0f)
	{
		actor->animator.lock()->SetTrigger("Step_Trigger_Land");

		if (!actor->animator.lock()->IsCurrentAnimation("RockOn_Left_Step")
			&& !actor->animator.lock()->IsCurrentAnimation("RockOn_Right_Step")
			&& !actor->animator.lock()->IsCurrentAnimation("RockOn_Back_Step"))
		{
			// ÃŽ~
			if (actor->moveAmount < 0.1f)
			{
				Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Landing);
				actor->ChangeState(PlayerActor::State::Idle);
				return;
			}
			if (actor->moveAmount >= 0.1f && actor->moveDir != Vector3::zero())
			{
				Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Landing);
				actor->ChangeState(PlayerActor::State::Move);
				return;
			}
		}
	}
}

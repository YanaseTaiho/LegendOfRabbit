#include "PlayerDamage.h"

void PlayerDamage::OnStart(PlayerActor * actor)
{
	isLand = false;
}

void PlayerDamage::OnUpdate(PlayerActor * actor)
{
	if (!isLand)
	{
		if (actor->rigidbody.lock()->velocity.y <= 0.0f && actor->onGround)
		{
			Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Down);
			isLand = true;
			actor->animator.lock()->SetTrigger("Damage_Land_Trigger");
		}
	}
	else
	{
		if (!actor->animator.lock()->IsCurrentAnimation("Damage_Forward_Land")
			&& !actor->animator.lock()->IsCurrentAnimation("Damage_Back_Land"))
		{
			// 一定時間点滅状態
			actor->damageFlashColor.StartFlash(Color::white(), Color(1.0f, 0.2f, 0.2f, 1.0f), 3.0f, 2.0f);
			actor->ChangeState(PlayerActor::State::Idle);
		}
	}
}

void PlayerDamage::SetDamage(PlayerActor * actor, DamageDirection dir)
{
	actor->animator.lock()->SetTrigger("Damage_Trigger");

	// 前方ダメージ
	if (dir == DamageDirection::Forward)
	{
		actor->animator.lock()->SetInt("Damage_Direction", 0);
		actor->rigidbody.lock()->AddForce((Vector3::up() * 150.0f + actor->transform.lock()->forward() * 800.0f) * Time::DeltaTime());
	}
	// 後方ダメージ
	if (dir == DamageDirection::Back)
	{
		actor->animator.lock()->SetInt("Damage_Direction", 1);
		actor->rigidbody.lock()->AddForce((Vector3::up() * 150.0f - actor->transform.lock()->forward() * 800.0f) * Time::DeltaTime());
	}
}

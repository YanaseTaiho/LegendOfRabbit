#include "SpikeRabbitAttackFinish.h"

void SpikeRabbitAttackFinish::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Attack_Finish");

	/*actor->animator.lock()->SetAnimationCallBack("Attack_Finish", 20, [=]()
	{
		auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect_Deth"));
		if (!effect.expired())
		{
			Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Bomb);
			effect.lock()->transform.lock()->SetWorldPosition(actor->transform.lock()->GetWorldPosition());
		}
		GameObject::Destroy(actor->gameObject);
	});*/

	frameCnt = 100;
}

void SpikeRabbitAttackFinish::OnUpdate(SpikeRabbitActor * actor)
{
	if (frameCnt <= 0)
	{
		actor->ChangeState(SpikeRabbitActor::State::Idle);
		return;
	}
	else if (frameCnt <= 30)
	{
		if(frameCnt == 30) actor->animator.lock()->ChangeAnimation("Idle");
	}
	else
	{
		Vector3 moveDir = actor->transform.lock()->forward() * Time::DeltaTime() * actor->attackMoveSpeed * 0.1f;
		actor->rigidbody.lock()->AddForce(moveDir);
	}
	frameCnt--;
}

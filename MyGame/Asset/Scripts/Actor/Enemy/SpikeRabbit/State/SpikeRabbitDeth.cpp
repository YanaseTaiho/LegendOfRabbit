#include "SpikeRabbitDeth.h"

void SpikeRabbitDeth::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Deth");
	actor->animator.lock()->SetAnimationCallBack("Deth", 20, [=]()
	{
		auto effect = Singleton<GameObjectManager>::Instance()->Instantiate(Singleton<SceneManager>::Instance()->GetCurrentScene()->GetPrefabGameObject("Effect_Deth"));
		if (!effect.expired())
		{
			Singleton<AudioClipManager>::Instance()->Play(AudioData::SE_Bomb);
			effect.lock()->transform.lock()->SetWorldPosition(actor->transform.lock()->GetWorldPosition());
		}
		GameObject::Destroy(actor->gameObject);
	});
}

void SpikeRabbitDeth::OnUpdate(SpikeRabbitActor * actor)
{
}

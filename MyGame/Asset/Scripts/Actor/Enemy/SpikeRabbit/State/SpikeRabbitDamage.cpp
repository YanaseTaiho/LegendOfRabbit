#include "SpikeRabbitDamage.h"

void SpikeRabbitDamage::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Damage");

	actor->animator.lock()->SetAnimationCallBack("Damage", 10, [=]()
	{
		actor->ChangeState(SpikeRabbitActor::State::Idle);
	});
}

void SpikeRabbitDamage::OnUpdate(SpikeRabbitActor * actor)
{
}

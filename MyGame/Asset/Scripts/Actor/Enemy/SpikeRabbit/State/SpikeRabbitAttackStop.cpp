#include "SpikeRabbitAttackStop.h"

void SpikeRabbitAttackStop::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Attack_Stop");

	actor->animator.lock()->SetAnimationCallBack("Attack_Stop", 15, [=]()
	{
		actor->ChangeState(SpikeRabbitActor::State::Idle);
	});
}

void SpikeRabbitAttackStop::OnUpdate(SpikeRabbitActor * actor)
{
	if (!actor->animator.lock()->IsCurrentAnimation("Attack_Stop"))
	{
		actor->ChangeState(SpikeRabbitActor::State::Idle);
	}
}

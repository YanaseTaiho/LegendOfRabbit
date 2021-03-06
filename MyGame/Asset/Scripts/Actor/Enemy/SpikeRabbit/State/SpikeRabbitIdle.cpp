#include "SpikeRabbitIdle.h"

void SpikeRabbitIdle::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Idle");
	frameCnt = 250;
}

void SpikeRabbitIdle::OnUpdate(SpikeRabbitActor * actor)
{
	// プレイヤーを見つけた時
	if (!actor->player.expired())
	{
		actor->ChangeState(SpikeRabbitActor::State::Attack_Ready);
		return;
	}

	if (frameCnt <= 0)
	{
		actor->ChangeState(SpikeRabbitActor::State::Move);
	}
	frameCnt--;
}

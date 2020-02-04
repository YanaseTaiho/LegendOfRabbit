#include "SmallGomaIdle.h"

void SmallGomaIdle::OnStart(SmallGomaActor * actor)
{
	frameCnt = rand() % 200;
}

void SmallGomaIdle::OnUpdate(SmallGomaActor * actor)
{
	if (!actor->player.expired())
	{
		actor->ChangeState(SmallGomaActor::State::Chase);
	}
	if (frameCnt <= 0)
	{
		actor->ChangeState(SmallGomaActor::State::Move);
	}

	frameCnt--;
}

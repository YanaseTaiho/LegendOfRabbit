#include "PlayerGuard.h"

void PlayerGuard::OnStart(PlayerActor * actor)
{
	frameCnt = 10;
	actor->horizontalRegistance = 1.0f;
}

void PlayerGuard::OnUpdate(PlayerActor * actor)
{
	actor->horizontalRegistance = 1.0f;

	if (frameCnt <= 0) actor->ChangeState(PlayerActor::State::Idle);
	frameCnt--;
}

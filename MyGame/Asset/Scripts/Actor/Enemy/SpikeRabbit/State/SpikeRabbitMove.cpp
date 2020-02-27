#include "SpikeRabbitMove.h"

void SpikeRabbitMove::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Move");
	frameCnt = 300;

	actor->animator.lock()->SetAnimationCallBack("Move", 6, [=]()
	{
		Vector3 moveDir = actor->transform.lock()->forward() * Time::DeltaTime() * actor->moveSpeed;
		actor->rigidbody.lock()->AddForce(moveDir);
	});
}

void SpikeRabbitMove::OnUpdate(SpikeRabbitActor * actor)
{
	// ƒvƒŒƒCƒ„[‚ðŒ©‚Â‚¯‚½Žž
	if (!actor->player.expired())
	{
		actor->ChangeState(SpikeRabbitActor::State::Attack_Ready);
		return;
	}

	if (frameCnt <= 0)
	{
		actor->ChangeState(SpikeRabbitActor::State::Idle);
		return;
	}
	frameCnt--;
}

#include "SpikeRabbitAttackReady.h"
#include "../../../Player/PlayerActor.h"

void SpikeRabbitAttackReady::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Attack_Ready");
	frameCnt = 200;
}

void SpikeRabbitAttackReady::OnUpdate(SpikeRabbitActor * actor)
{
	// ƒvƒŒƒCƒ„[‚ðŒ©Ž¸‚Á‚½
	if (actor->player.expired())
	{
		actor->ChangeState(SpikeRabbitActor::State::Idle);
		return;
	}

	Vector3 dir = actor->player.lock()->transform.lock()->GetWorldPosition() - actor->transform.lock()->GetWorldPosition();
	dir.y = 0.0f;
	Quaternion look = Quaternion::LookRotation(dir);
	Quaternion rot = actor->transform.lock()->GetWorldRotation();
	rot = rot.Slerp(look, Time::DeltaTime() * 3.0f);
	actor->transform.lock()->SetWorldRotation(rot);

	if (frameCnt <= 0)
	{
		actor->ChangeState(SpikeRabbitActor::State::Attack);
		return;
	}
	frameCnt--;
}

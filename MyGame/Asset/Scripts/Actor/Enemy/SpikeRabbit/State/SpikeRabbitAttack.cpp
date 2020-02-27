#include "SpikeRabbitAttack.h"

void SpikeRabbitAttack::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Attack");
	frameCnt = 120;
}

void SpikeRabbitAttack::OnUpdate(SpikeRabbitActor * actor)
{
	if (frameCnt <= 0)
	{
		actor->ChangeState(SpikeRabbitActor::State::Attack_Finish);
		return;
	}
	frameCnt--;

	Vector3 moveDir = actor->transform.lock()->forward() * Time::DeltaTime() * actor->attackMoveSpeed;
	actor->rigidbody.lock()->AddForce(moveDir);
}

#include "SpikeRabbitMove.h"

void SpikeRabbitMove::OnStart(SpikeRabbitActor * actor)
{
	actor->animator.lock()->ChangeAnimation("Move");
	frameCnt = rand() % 200 + 100;

	look = Quaternion::AxisAngle(Vector3::up(), (float)(rand() % 360));

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

	Quaternion rot = actor->transform.lock()->GetWorldRotation();
	rot = rot.Slerp(look, 0.5f);
	actor->transform.lock()->SetWorldRotation(rot);

	frameCnt--;
}

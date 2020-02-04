#include "SmallGomaMove.h"

void SmallGomaMove::OnStart(SmallGomaActor * actor)
{
	frameCnt = rand() % 200;

	look = Quaternion::AxisAngle(Vector3::up(), (float)(rand() % 360));
}

void SmallGomaMove::OnUpdate(SmallGomaActor * actor)
{
	if (!actor->player.expired())
	{
		actor->ChangeState(SmallGomaActor::State::Chase);
	}
	if (frameCnt <= 0)
	{
		actor->ChangeState(SmallGomaActor::State::Idle);
	}

	Quaternion rot = actor->transform.lock()->GetWorldRotation();
	rot = rot.Slerp(look, Time::DeltaTime() * 10.0f);
	actor->transform.lock()->SetWorldRotation(rot);

	Vector3 moveDir = actor->transform.lock()->forward() * Time::DeltaTime() * actor->moveSpeed;
	actor->rigidbody.lock()->AddForce(moveDir);

	frameCnt--;
}

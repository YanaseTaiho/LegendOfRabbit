#include "SmallGomaChase.h"
#include "../../../Player/PlayerActor.h"

void SmallGomaChase::OnStart(SmallGomaActor * actor)
{
}

void SmallGomaChase::OnUpdate(SmallGomaActor * actor)
{
	if (actor->player.expired())
	{
		actor->ChangeState(SmallGomaActor::State::Idle);
		return;
	}
	Vector3 dir = actor->player.lock()->transform.lock()->GetWorldPosition() - actor->transform.lock()->GetWorldPosition();
	dir.y = 0.0f;
	Quaternion look = Quaternion::LookRotation(dir);
	Quaternion rot = actor->transform.lock()->GetWorldRotation();
	rot = rot.Slerp(look, Time::DeltaTime() * 3.0f);
	actor->transform.lock()->SetWorldRotation(rot);

	Vector3 moveDir = actor->transform.lock()->forward() * Time::DeltaTime() * actor->moveSpeed;
	actor->rigidbody.lock()->AddForce(moveDir);
}

#include "Vibration.h"

void Vibration::SetTarget(std::weak_ptr<Transform> target)
{
	this->target = target;
}

void Vibration::SetVibration(Vector3 dir)
{
	vibPos = dir;
	vibAmount = 1.0f;
	vibFrameCnt = 0;
	vibOldPos = Vector3::zero();
}

void Vibration::Update()
{
	if (abs(vibAmount) <= 0.01f) return;

	vibFrameCnt++;
	if (vibFrameCnt % 3 == 0) return;

	Vector3 pos = target.lock()->GetWorldPosition();
	pos += vibPos - vibOldPos;
	vibOldPos = vibPos;
	target.lock()->SetWorldPosition(pos);

	vibPos = -(vibPos * vibAmount);
	vibAmount *= 0.90f;
}

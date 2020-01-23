#ifndef _SMOOTHFOLLOW_H_
#define _SMOOTHFOLLOW_H_

#include "FrameWork/Common.h"

class SmoothFollow : public MonoBehaviour
{
public:
	SmoothFollow(std::weak_ptr<Transform> target, float followSpeed) :
		target(target), followSpeed(followSpeed)
	{}



	void Update() override
	{
		if (target.expired()) return;

		Vector3 pos = transform.lock()->GetWorldPosition();
		Vector3 targetPos = target.lock()->GetWorldPosition();
		pos = Vector3::Lerp(pos, targetPos, Time::DeltaTime() * followSpeed);
		transform.lock()->SetWorldPosition(pos);
	}

	void SetTarget(std::weak_ptr<Transform> target)
	{
		this->target = target;
	}

	void SetSpeed(float speed)
	{
		followSpeed = speed;
	}

private:
	std::weak_ptr<Transform> target;
	float followSpeed;
};

#endif // !_SMOOTHFOLLOW_H_


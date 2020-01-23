#ifndef _BULLET_H_
#define _BULLET_H_

#include "FrameWork/Common.h"

class Bullet : public MonoBehaviour
{
public:
	Bullet(Vector3 direction, float speed)
		: direction(direction), speed(speed)
	{}

	void Update() override
	{
		Vector3 pos = transform.lock()->GetWorldPosition();
		transform.lock()->SetWorldPosition(pos + direction * speed * Time::DeltaTime());
	}
private:
	Vector3 direction;
	float speed;
};

#endif // !_BULLET_H_


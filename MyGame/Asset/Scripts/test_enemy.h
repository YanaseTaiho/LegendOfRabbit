#ifndef _TEST_ENEMY_H_
#define _TEST_ENEMY_H_

#include "FrameWork/Component/MonoBehaviour/MonoBehaviour.h"
#include "Scripts/test_move.h"

#include "Main/audio_clip.h"

class TestEnemy : public MonoBehaviour
{
public:
//	TestEnemy(){}
//	~TestEnemy()
//	{
//	}
//
//	void Start() override
//	{
//		std::weak_ptr<GameObject> obj = FindGameObject(Tag::Player);
//		if (!obj.expired())
//		{
//			target = obj.lock()->transform;
//		}
//	}
//
//	void Update() override
//	{
//		if (target.expired()) return;
//
//		Quaternion mineRotation = transform.lock()->GetWorldRotation();
//
//		//Quaternion::AxisAngle(Vector3::right(), 10.0f);
//		Quaternion look = Quaternion::LookRotation(target.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition());
//		mineRotation = mineRotation.Slerp(look, Time::DeltaTime() * 3.0f);
//		
//	/*	Quaternion look = Quaternion::FromToRotation(transform.lock()->forward(),target.lock()->GetWorldPosition() - transform.lock()->GetWorldPosition());
//		look = look * mineRotation;
//		mineRotation = mineRotation.Slerp(look, Time::DeltaTime() * 3.0f);*/
//
//		//Quaternion up = Quaternion::FromToRotation(transform.lock()->up(),Vector3::up());
//		//up = up * mineRotation;
//		//mineRotation = mineRotation.Slerp(up, Time::DeltaTime() * 3.0f);
//
//		transform.lock()->SetWorldRotation(mineRotation);
//
//		hit = false;
//	}
//
//	void LateUpdate() override
//	{
//		
//	}
//
//	void OnCollisionStay(std::weak_ptr<Collision> mine, std::weak_ptr<Collision> other) override
//	{
//		if (other.lock()->gameObject.lock()->tag == Tag::Bullet)
//		{
//			gameObject.lock()->Destroy();
//			other.lock()->gameObject.lock()->Destroy();
//		}
//		hit = true;
//	}
//private:
//	std::weak_ptr<Transform> target;
//
//	bool hit = false;
//	int cnt = 0;
};

#endif // !_TEST_ENEMY_H_

#ifndef ____VIBRATION__H__
#define ____VIBRATION__H__

#include "../FrameWork/Common.h"

class Vibration
{
public:
	void SetTarget(std::weak_ptr<Transform> target);
	void SetVibration(Vector3 dir);
	void Update();
private:
	std::weak_ptr<Transform> target;
	int vibFrameCnt;
	Vector3 vibPos;
	Vector3 vibOldPos;
	float vibAmount;
};

#endif // !____VIBRATION__H__

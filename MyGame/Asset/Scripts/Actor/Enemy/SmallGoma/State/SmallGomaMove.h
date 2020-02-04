#ifndef __SmallGomaMove__H__
#define __SmallGomaMove__H__

#include "../SmallGomaActor.h"

class SmallGomaMove : public BaseFSM<SmallGomaActor>
{
public:
	void OnStart(SmallGomaActor * actor) override;
	void OnUpdate(SmallGomaActor * actor) override;
private:
	int frameCnt;
	Quaternion look;
};

#endif // !__SmallGomaMove__H__
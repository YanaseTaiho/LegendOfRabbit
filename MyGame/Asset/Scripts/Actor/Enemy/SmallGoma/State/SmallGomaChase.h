#ifndef __SmallGomaChase__H__
#define __SmallGomaChase__H__

#include "../SmallGomaActor.h"

class SmallGomaChase : public BaseFSM<SmallGomaActor>
{
public:
	void OnStart(SmallGomaActor * actor) override;
	void OnUpdate(SmallGomaActor * actor) override;
private:
	int frameCnt;
};

#endif // !__SmallGomaChase__H__

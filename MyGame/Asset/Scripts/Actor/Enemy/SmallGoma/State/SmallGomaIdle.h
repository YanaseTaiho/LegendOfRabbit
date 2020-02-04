#ifndef __SmallGomaIdle__H__
#define __SmallGomaIdle__H__

#include "../SmallGomaActor.h"

class SmallGomaIdle : public BaseFSM<SmallGomaActor>
{
public:
	void OnStart(SmallGomaActor * actor) override;
	void OnUpdate(SmallGomaActor * actor) override;
private:
	int frameCnt;
};

#endif // !__SmallGomaIdle__H__

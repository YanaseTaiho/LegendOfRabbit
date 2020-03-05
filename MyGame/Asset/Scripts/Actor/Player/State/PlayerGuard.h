#ifndef __PlayerGuard__H__
#define __PlayerGuard__H__

#include "../PlayerActor.h"

class PlayerGuard : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
	float frameCnt;
};

#endif // !__PlayerGuard__H__

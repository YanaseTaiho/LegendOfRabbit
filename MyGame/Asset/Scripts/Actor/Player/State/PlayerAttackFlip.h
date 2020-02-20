#ifndef __PlayerAttackFlip__H__
#define __PlayerAttackFlip__H__

#include "../PlayerActor.h"

class PlayerAttackFlip : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
	float frameCnt;
};

#endif // !__PlayerAttackFlip__H__

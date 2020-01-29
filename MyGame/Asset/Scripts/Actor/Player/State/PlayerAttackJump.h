#ifndef __PlayerAttackJump__H__
#define __PlayerAttackJump__H__

#include "../PlayerActor.h"

class PlayerAttackJump : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
};

#endif // !__PlayerAttackJump__H__

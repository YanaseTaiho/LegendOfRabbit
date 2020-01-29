#ifndef __PLAYER_CLIFF_JUMP__H__
#define __PLAYER_CLIFF_JUMP__H__

#include "../PlayerActor.h"

class PlayerCliffJump : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
};

#endif // !__PLAYER_CLIFF_JUMP__H__

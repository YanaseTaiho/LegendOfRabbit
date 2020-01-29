#ifndef __PLAYER_ROLL__H__
#define __PLAYER_ROLL__H__

#include "../PlayerActor.h"

class PlayerRoll : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
};

#endif // !__Template_PlayerState__H__

#ifndef _PLAYER_CLIFF_GRAP_H_
#define _PLAYER_CLIFF_GRAP_H_

#include "../PlayerActor.h"

class PlayerCliffGrap : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
};

#endif // !_PLAYER_CLIFF_GRAP_H_

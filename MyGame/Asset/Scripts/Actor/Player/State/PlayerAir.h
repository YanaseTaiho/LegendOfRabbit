#ifndef _PLAYERAIR_H_
#define _PLAYERAIR_H_

#include "../PlayerActor.h"

class PlayerAir : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;

private:
	float moveSpeed = 5.5f;
};

#endif // !_PLAYERIDLE_H_

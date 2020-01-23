#ifndef _PLAYERMOVE_H_
#define _PLAYERMOVE_H_

#include "../PlayerActor.h"

class PlayerMove : public BaseFSM<PlayerActor>
{
public:
	PlayerMove();

	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;

private:
	int frameCnt;
	float lookSpeed;
};

#endif // !_PLAYERMOVE_H_

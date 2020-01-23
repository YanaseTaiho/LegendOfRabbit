#ifndef _PLAYERIDLE_H_
#define _PLAYERIDLE_H_

#include "../PlayerActor.h"

class PlayerIdle : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
};

#endif // !_PLAYERIDLE_H_

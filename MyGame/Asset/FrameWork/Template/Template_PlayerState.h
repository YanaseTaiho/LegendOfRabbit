#ifndef __Template_PlayerState__H__
#define __Template_PlayerState__H__

#include "../PlayerActor.h"

class Template_PlayerState : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
};

#endif // !__Template_PlayerState__H__

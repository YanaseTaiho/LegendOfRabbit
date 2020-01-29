#ifndef __PlayerAttack__H__
#define __PlayerAttack__H__

#include "../PlayerActor.h"

class PlayerAttack : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
};

#endif // !__PlayerAttack__H__

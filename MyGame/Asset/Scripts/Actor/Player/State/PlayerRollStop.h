#ifndef __PlayerRollStop__H__
#define __PlayerRollStop__H__

#include "../PlayerActor.h"

class PlayerRollStop : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
};

#endif // !__PlayerRollStop__H__

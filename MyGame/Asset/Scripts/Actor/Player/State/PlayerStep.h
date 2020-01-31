#ifndef __PlayerStep__H__
#define __PlayerStep__H__

#include "../PlayerActor.h"

class PlayerStep : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
private:
};

#endif // !__PlayerStep__H__

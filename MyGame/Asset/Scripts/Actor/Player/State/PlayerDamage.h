#ifndef __PlayerDamage__H__
#define __PlayerDamage__H__

#include "../PlayerActor.h"

class PlayerDamage : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;


	enum class DamageDirection
	{
		Forward,
		Back
	};

	void SetDamage(PlayerActor * actor, DamageDirection dir);
private:
	bool isLand = false;
//	DamageDirection damageDirection;
};

#endif // !__PlayerDamage__H__

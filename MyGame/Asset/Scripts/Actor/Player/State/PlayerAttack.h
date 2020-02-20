#ifndef __PlayerAttack__H__
#define __PlayerAttack__H__

#include "../PlayerActor.h"

class PlayerAttack : public BaseFSM<PlayerActor>
{
public:
	void OnStart(PlayerActor * actor) override;
	void OnUpdate(PlayerActor * actor) override;
	void OnDestroy(PlayerActor * actor) override;

	void Attack(PlayerActor * actor);

private:
	int frameCnt;
	int combo;
	PlayerActor::AttackType attackType;

	void CheckAttackType(PlayerActor * actor);
};

#endif // !__PlayerAttack__H__

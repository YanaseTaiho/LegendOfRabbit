#ifndef __SpikeRabbitAttack__H__
#define __SpikeRabbitAttack__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitAttack : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
	int frameCnt;
};

#endif // !__SpikeRabbitAttack__H__

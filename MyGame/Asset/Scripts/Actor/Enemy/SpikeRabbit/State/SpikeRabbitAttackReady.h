#ifndef __SpikeRabbitAttackReady__H__
#define __SpikeRabbitAttackReady__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitAttackReady : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
	int frameCnt;
};

#endif // !__SpikeRabbitAttackReady__H__

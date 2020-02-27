#ifndef __SpikeRabbitAttackFinish__H__
#define __SpikeRabbitAttackFinish__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitAttackFinish : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
	int frameCnt;
};

#endif // !__SpikeRabbitAttackFinish__H__

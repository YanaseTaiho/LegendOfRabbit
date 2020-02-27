#ifndef __SpikeRabbitAttackStop__H__
#define __SpikeRabbitAttackStop__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitAttackStop : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
};

#endif // !__SpikeRabbitAttackStop__H__

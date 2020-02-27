#ifndef __SpikeRabbitDamage__H__
#define __SpikeRabbitDamage__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitDamage : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
};

#endif // !__SpikeRabbitDamage__H__

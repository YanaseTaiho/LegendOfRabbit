#ifndef __SpikeRabbitDeth__H__
#define __SpikeRabbitDeth__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitDeth : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
};

#endif // !__SpikeRabbitDeth__H__

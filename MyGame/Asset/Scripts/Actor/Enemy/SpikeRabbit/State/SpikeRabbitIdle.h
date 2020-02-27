#ifndef __SpikeRabbitIdle__H__
#define __SpikeRabbitIdle__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitIdle : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
	int frameCnt;
};

#endif // !__SpikeRabbitIdle__H__

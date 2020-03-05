#ifndef __SpikeRabbitMove__H__
#define __SpikeRabbitMove__H__

#include "../SpikeRabbitActor.h"

class SpikeRabbitMove : public BaseFSM<SpikeRabbitActor>
{
public:
	void OnStart(SpikeRabbitActor * actor) override;
	void OnUpdate(SpikeRabbitActor * actor) override;
private:
	int frameCnt;
	Quaternion look;
};

#endif // !__SpikeRabbitMove__H__

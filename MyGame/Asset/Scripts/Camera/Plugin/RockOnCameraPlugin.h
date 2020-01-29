#ifndef __RockOnCameraPlugin__H__
#define __RockOnCameraPlugin__H__

#include "../CameraPlugin.h"

class RockOnCameraPlugin : public CameraPlugin
{
public:
	//void OnStart(CameraController * controller) override;
	//void OnUpdate(CameraController * controller) override;
	void OnLateUpdate(CameraController * controller) override;
private:
};

#endif // !__RockOnCameraPlugin__H__

#ifndef __RockOnCameraPlugin__H__
#define __RockOnCameraPlugin__H__

#include "../CameraPlugin.h"
#include "../../../FrameWork/Common.h"

class RockOnCameraPlugin : public CameraPlugin
{
public:
	void OnStart(CameraController * controller) override;
	//void OnUpdate(CameraController * controller) override;
	void OnLateUpdate(CameraController * controller) override;
private:

	float defaultDistance = 0.0f;
	Quaternion rotation;
};

#endif // !__RockOnCameraPlugin__H__

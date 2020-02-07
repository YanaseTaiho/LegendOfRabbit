#ifndef _CHARACTERCAMERAPLUGIN_H_
#define _CHARACTERCAMERAPLUGIN_H_

#include "../CameraPlugin.h"

class CharacterCameraPlugin : public CameraPlugin
{
public:
	CharacterCameraPlugin();

	void OnStart(CameraController * controller) override;
	void OnUpdate(CameraController * controller) override;
	void OnLateUpdate(CameraController * controller) override;

private:
	float defaultDistance = 0.0f;
	float lookSpeed;
	float moveSpeed;

	float lookValue;		// �������̉�]�̗�
	float lookHolizontal;	// ���������̉�]�p
	float lookVertical;		// �Ɉʕ����̉�]�p
};

#endif // !_CHARACTERCAMERAPLUGIN_H_


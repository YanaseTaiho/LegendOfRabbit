#ifndef _CAMERACONTROLLER_H_
#define _CAMERACONTROLLER_H_

#include "FrameWork/Common.h"
#include "CameraPlugin.h"

class PlayerActor;

class CameraController : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(cameraTransform, targetTransform, playerActor);
		archive(targetDistance, distanceSpeed, offsetHeight);

		if (version >= 1)
			archive(verticalTransform);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::make_nvp("MonoBehaviour", cereal::base_class<MonoBehaviour>(this)));
		archive(cameraTransform, targetTransform, playerActor);
		archive(targetDistance, distanceSpeed, offsetHeight);

		if(version >= 1) 
			archive(verticalTransform);
	}

	void DrawImGui(int id) override;

public:

	enum class Plugin : int
	{
		Character,
		RockOn
	};

	void OnStart();
	void OnUpdate();
	void OnLateUpdate();

	void AddPlugin(Plugin key, CameraPlugin * plugin);
	void ChangePlugin(Plugin key);
	bool IsPlugin(Plugin key);

	bool CollisionCheck();
	void UpdateDistance(float distance, float speed);

	std::weak_ptr<Transform> cameraTransform;	// �J�����̃g�����X�t�H�[��
	std::weak_ptr<Transform> verticalTransform;	// �J�����̋Ɉʕ����̉�]�p�g�����X�t�H�[��
	std::weak_ptr<Transform> targetTransform;	// �J�����̒����_�̃g�����X�t�H�[��
	std::weak_ptr<PlayerActor> playerActor;		// �v���C���[�R���|�[�l���g

	float targetDistance = 150.0f;
	float distanceSpeed = 1.0f;
	float offsetHeight = 4.0f;
private:

	std::unordered_map<Plugin,std::shared_ptr<CameraPlugin>> pluginMap;
	std::weak_ptr<CameraPlugin> currentPlugin;
};

CEREAL_CLASS_VERSION(CameraController, 1)
CEREAL_REGISTER_TYPE(CameraController)

#endif // !_CAMERACONTROLLER_H_


#ifndef __EntryPoint__H__
#define __EntryPoint__H__

#include "FrameWork/Common.h"

class SceneChangePoint : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(nextSceneName, entryNumber);
		if (version >= 1) archive(sceneStartPoint);
	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
		archive(nextSceneName, entryNumber);
		if (version >= 1) archive(sceneStartPoint);
	}


public:
	void DrawImGui(int id) override;

	std::string nextSceneName;	// ���̃V�[���؂�ւ���̖��O
	int entryNumber;			// ���̃V�[����SceneChangePoint�̔ԍ�
	std::weak_ptr<Transform> sceneStartPoint;	// �V�[���J�n���̃X�^�[�g�n�_
	std::function<void(SceneChangePoint & point)> PlaySceneChange = nullptr;	// �V�[����؂�ւ���^�C�~���O�ŌĂ�
private:
//	void Start() override;
//  void Update() override;

	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
};

CEREAL_CLASS_VERSION(SceneChangePoint, 1)
CEREAL_REGISTER_TYPE(SceneChangePoint)

#endif // !__EntryPoint__H__
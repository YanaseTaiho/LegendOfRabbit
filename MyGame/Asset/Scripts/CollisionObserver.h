#ifndef __CollisionObserver__H__
#define __CollisionObserver__H__

#include "FrameWork/Common.h"

// �Փ˂��Ă���R���W�����̊Ǘ�������N���X

class CollisionObserver : public MonoBehaviour
{
private:
	friend cereal::access;
	template<class Archive>
	void save(Archive & archive, std::uint32_t const version) const
	{
		archive(cereal::base_class<MonoBehaviour>(this));

	}

	template<class Archive>
	void load(Archive & archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}

	void DrawImGui(int id) override;

public:
	CollisionObserver() {}
	~CollisionObserver() {}

	std::list<std::weak_ptr<Collision>> collisionList;			// �������Ă���R���W���������X�g�ɕێ�
	std::list<std::weak_ptr<Collision>> collisionTriggerList;	// �������Ă���g���K�[�R���W���������X�g�ɕێ�

	void SetCollisionEnter(std::function<void(std::weak_ptr<Collision>)> func) { collisionEnter = func; }
	void SetCollisionExit(std::function<void(std::weak_ptr<Collision>)> func) { collisionExit = func; }
	void SetTriggerEnter(std::function<void(std::weak_ptr<Collision>)> func) { triggerEnter = func; }
	void SetTriggerExit(std::function<void(std::weak_ptr<Collision>)> func) { triggerExit = func; }

private:

	std::list<std::weak_ptr<Collision>> addCollisionList;			// �t���[�����̓������Ă���R���W���������X�g�ɕێ�
	std::list<std::weak_ptr<Collision>> addCollisionTriggerList;	// �t���[�����̓������Ă���g���K�[�R���W���������X�g�ɕێ�

	// �����ɉ����Ă��̊֐����Ă�
	std::function<void(std::weak_ptr<Collision>)> collisionEnter;
	std::function<void(std::weak_ptr<Collision>)> collisionExit;
	std::function<void(std::weak_ptr<Collision>)> triggerEnter;
	std::function<void(std::weak_ptr<Collision>)> triggerExit;

	void Update() override;
	void LateUpdate() override;

	// �����I�ɓ������Ă���Ԃ͌Ă΂��
	void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
	void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) override;
};

CEREAL_CLASS_VERSION(CollisionObserver, 0)
CEREAL_REGISTER_TYPE(CollisionObserver)

#endif // !__CollisionObserver__H__
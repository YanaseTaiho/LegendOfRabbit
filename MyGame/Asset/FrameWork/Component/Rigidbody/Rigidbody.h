#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "../Component.h"
#include "../Transform/Transform.h"
#include <list>
#include <memory>

namespace FrameWork
{
	class GameObject;
	class Collision;
	class SceneBase;

	class Rigidbody : public Component
	{
		friend Collision;
		friend Transform;
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(velocity), CEREAL_NVP(gravity), CEREAL_NVP(resistance),CEREAL_NVP(collisions));
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(velocity), CEREAL_NVP(gravity), CEREAL_NVP(resistance), CEREAL_NVP(collisions));
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override
		{
			Component::LoadSerialized(component);
			AddComponentList(rigidbodyList);
		}

	public:
		Rigidbody() {}
		Rigidbody(float gravity, float resistance);
		~Rigidbody();

		static std::list<std::weak_ptr<Rigidbody>> & ComponentList() { return rigidbodyList; }

		void DrawImGui(int id) override;

		void Update();
		void UpdateSet();

		void AddForce(Vector3 force);

		std::list<std::weak_ptr<Collision>> collisions;	// ���g�̕ێ�����R���W�������X�g

		Vector3 velocity;
		float gravity;		// �d��
		float resistance;	// ��R

		// �S�Ă̓����蔻�菈��
		static void UpdateCollisionJudge();
	private:
	
		void AddComponent() override;
		void RemoveComponent() override;
		
		void SetCollision(std::weak_ptr<Collision> & collision);
		// ���̊֐����Ăԃ^�C�~���O
		// �E�I�u�W�F�N�g�ɃZ�b�g���ꂽ��
		// �E�e�q�֌W���ς������
		void ResetCollisions();
		void SetCollisions(std::weak_ptr<Rigidbody> & rigid ,std::weak_ptr<GameObject> & object);
		// �R���W�����Ƃ̊֘A����؂藣��
		void RemoveCollisions();
		void RemoveCollision(Collision * collision);

		// ���W�b�h�{�f�B���Ȃ��R���W�����Ƃ̓����蔻��
		void JudgeNormalCollision();
		void JudgeRigidbody(std::weak_ptr<Rigidbody> rigidbody);

		void HitCollision(std::weak_ptr<Collision> & a, std::weak_ptr<Collision> & b);

		static std::list<std::weak_ptr<Rigidbody>> rigidbodyList;
	};
}

CEREAL_CLASS_VERSION(FrameWork::Rigidbody, 0)
CEREAL_REGISTER_TYPE(FrameWork::Rigidbody)

#endif //!_RIGIDBODY_H_
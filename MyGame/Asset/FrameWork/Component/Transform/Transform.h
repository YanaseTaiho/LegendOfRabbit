#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <memory>
#include <list>
#include <string>
#include <functional>
#include "../Component.h"
#include "../../Quaternion.h"
#include "../../Vector.h"
#include "../../Matrix.h"
#include "../../SetType.h"

class EditorScene;

namespace FrameWork
{
	class AnimationClip;
	class GameObjectManager;
	class Prefab;

	class Transform : public Component
	{
		friend AnimationClip;
		friend EditorScene;
		friend GameObjectManager;
		friend Prefab;

	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			// ���[���h�p����ۑ�
			/*Matrix4 w = (parent.expired()) ? localMatrix : parent.lock()->worldMatrix.Inverse() * localMatrix;
			Vector3 p = w.position();
			Vector3 s = w.scale();
			Quaternion rot = w.rotation();
		 	Translation3f trans = Translation3f(p.x, p.y, p.z);
			Scaling3f scal = Scaling3f(s.x, s.y, s.z);

			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				cereal::make_nvp("translation", trans), cereal::make_nvp("scaling", scal), cereal::make_nvp("rotate", rot),
				CEREAL_NVP(parent), CEREAL_NVP(childs));*/
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(translation), CEREAL_NVP(scaling), CEREAL_NVP(rotate),
				CEREAL_NVP(parent), CEREAL_NVP(childs));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(translation), CEREAL_NVP(scaling), CEREAL_NVP(rotate),
				CEREAL_NVP(parent), CEREAL_NVP(childs));

			SetLocalMatrix();
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override
		{
			Component::LoadSerialized(component);
			LoadTransform();
		}
		void DrawImGui(int id) override;

		static std::function<void(std::weak_ptr<Transform>)> AddTransform;
		static std::function<void(std::weak_ptr<Transform>)> RemoveTransform;

	public:
		Transform();
		Transform(const Transform & trans);
		Transform(Vector3 position, Vector3 scale = Vector3::one(), Quaternion rotation = Quaternion::Identity())
		{
			SetLocalTransform(position, scale, rotation);
		}
		~Transform();

		void SetWorldTransform(const Vector3 & position, const Vector3 & scale, const Quaternion & rotation);
		void SetLocalTransform(const Vector3 & position, const Vector3 & scale, const Quaternion &  rotation);

		void SetWorldPosition(const Vector3 & position);
		void SetLocalPosition(const Vector3 & position);
		void SetWorldScale(const Vector3 & scale);
		void SetLocalScale(const Vector3 & scale);
		void SetWorldRotation(const Quaternion &  rotation);
		void SetLocalRotation(const Quaternion &  rotation);

		void Update();
		void UpdateChilds();

		void SetLocalMatrix(const Matrix4 & matrix);
		void SetLocalMatrix(const Vector3 & position, const Vector3 & scale, const Quaternion &  rotation);
		void SetWorldMatrix(const Matrix4 & matrix);

		Vector3 GetWorldPosition();
		Vector3 GetLocalPosition();
		Vector3 GetWorldScale();
		Vector3 GetLocalScale();
		Quaternion GetWorldRotation();
		Quaternion GetLocalRotation();

		void SetParent(std::weak_ptr<Transform> & parent, SetType type = SetType::World, int number = -1);	// number : ���Ԗڂ̎q�ɒǉ����邩
		void SetChild(std::weak_ptr<Transform> & child, SetType type = SetType::World, int number = -1);	// number : ���Ԗڂ̎q�ɒǉ����邩

		std::weak_ptr<Transform> GetParent();		// ���g�̐e��Ԃ�( ���Ȃ���������Ԃ� )
		std::weak_ptr<Transform> GetBossParent();	// �K�w�̈�ԏ�̐e��Ԃ�( ���g�������Ȃ玩�g��Ԃ� )
		std::weak_ptr<Transform> GetChild(int num); // �q�̃i���o�[�Ɉ�v�����I�u�W�F�N�g���擾
		std::weak_ptr<Transform> GetChild(std::string name); // ���O����v�����q���擾
		std::list<std::weak_ptr<Transform>> & GetChilds() { return childs; };

		void ReleaseParent();	// �e�Ƃ̊֌W�𑊌݂ɒf���؂�
		void ReleaseChild(int num); // num�Ԗڂ̎q�Ƃ̊֌W�𑊌݂ɒf���؂�
		void ReleaseChild(std::weak_ptr<Transform> child);	// �����̎q�Ƃ̊֌W�𑊌݂ɒf���؂�
		void ReleaseChild(std::weak_ptr<GameObject> child);	// �����̎q�Ƃ̊֌W�𑊌݂ɒf���؂�
		void ReleaseChild(std::string name);	// ���O����v�����q�Ƃ̊֌W�𑊌݂ɒf���؂�
		void ReleaseChildAll();	// �S�Ă̎q�Ƃ̊֌W�𑊌݂ɒf���؂�

		Vector3 forward();	// ���g�̑O�����擾
		Vector3 right();	// ���g�̉E�����擾
		Vector3 up();		// ���g�̏�����擾

		Matrix4 GetWorldMatrix();
		Matrix4 GetLocalMatrix();


	private:
		void AddComponent() override final;
		void RemoveComponent() override final;

		void SetLocalMatrix();
		void SetWorldMatrix();
		void DestroyChilds();
		void PrivateReleaseParent();
		void PrivateReleaseChild(std::weak_ptr<Transform> & child);
		void ResetRigidbodyCollision();
		void LossyScale(Vector3 & s);

		void Finalize();

		void LoadTransform();

		Scaling3f ScalingMultiply(const Scaling3f & a, const Scaling3f & b);

		std::weak_ptr<Transform> parent;
		std::list<std::weak_ptr<Transform>> childs;

		// ���s�ړ�(x, y, z)
		Translation3f translation;
		// �X�P�[�����O
		Scaling3f scaling;
		// ��](�N�H�[�^�j�I��)
		Quaternion rotate;

		Matrix4 worldMatrix;
		Matrix4 localMatrix;
	};
}

CEREAL_REGISTER_TYPE(FrameWork::Transform)
//cereal::base_class<>()��������Ăׂ΃}�N���͕K�v�Ȃ�
//CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, FrameWork::Transform)

#endif //!_TRANSFORM_H_
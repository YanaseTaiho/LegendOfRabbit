#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "../Tag.h"
#include "../Layer.h"
#include "../SetType.h"
#include "../Component/Component.h"
#include "../Component/Transform/Transform.h"
#include <memory>
#include <list>
#include <string>

class EditorScene;

namespace FrameWork
{
	class GameObjectManager;
	class SceneManager;
	class Collision;
	class Rigidbody;
	class MonoBehaviour;
	class Prefab;

	class GameObject final
	{
		friend GameObjectManager;
		friend SceneManager;
		friend Component;
		friend Transform;
		friend MonoBehaviour;
		friend Rigidbody;

		friend EditorScene;
		friend Prefab;

	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(CEREAL_NVP(isActive), CEREAL_NVP(name),CEREAL_NVP(tag), CEREAL_NVP(layer), 
				CEREAL_NVP(gameObject), CEREAL_NVP(transform), CEREAL_NVP(components), CEREAL_NVP(behaviours));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(CEREAL_NVP(isActive), CEREAL_NVP(name), CEREAL_NVP(tag), CEREAL_NVP(layer),
				CEREAL_NVP(gameObject), CEREAL_NVP(transform), CEREAL_NVP(components), CEREAL_NVP(behaviours));

			// ��������̏���������

		}

		void DrawImGui();

	public:
		GameObject() = default;
		GameObject(const GameObject & object);
		GameObject(std::string name, Tag tag, Layer layer);
		~GameObject();

		std::weak_ptr<GameObject> gameObject;	// ���g���w���Q�ƃ|�C���^
		std::weak_ptr<Transform> transform; // �g�����X�t�H�[���̎Q�ƃ|�C���^
		
		Tag tag; // �^�O
		std::string name; // ���O

		// �I�u�W�F�N�g���L������������ݒ�
		void SetActive(bool active);
		// �I�u�W�F�N�g���L����������
		bool IsActive();
		Layer GetLayer() { return layer; }
		Layer ChangeLayer(Layer layer);

		// stopFrame : �I�u�W�F�N�g���~����t���[����(0�ȉ��Őݒ肵���ꍇ�͖����ɒ�~����)
		void SetStop(bool isStop);
		bool IsStop() { return isStop; }

		// �e���Z�b�g����
		std::weak_ptr<GameObject> SetParent(std::weak_ptr<GameObject> gameObject, SetType type = SetType::World);
		// �q���Z�b�g����
		std::weak_ptr<GameObject> SetChild(std::weak_ptr<GameObject> gameObject, SetType type = SetType::World);

		// �R���|�[�l���g��ǉ�
		template <typename T>
		void AddComponent(T * component)
		{
			components.emplace_back(Component::AddComponent(component, gameObject, transform));
		}

		// �R���|�[�l���g���擾
		template <typename T>
		std::weak_ptr<T> GetComponent()
		{
			for (auto & component : components)
			{
				// �_�C�i�~�b�N�L���X�g���邱�Ƃł��̃R���|�[�l���g���𔻒f����
				// typeid���ƌp���O�̃N���X�̌^�Ŏ��Ȃ�����
				std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
				if (cast.expired()) continue;

				return cast;
			}

			return std::weak_ptr<T>();
		}

		// �e�̃R���|�[�l���g���擾
		template <typename T>
		std::weak_ptr<T> GetComponentWithParent()
		{
			auto parent = transform.lock()->GetParent();
			if (!parent.expired())
			{
				auto object = parent.lock()->gameObject;
				if (!object.expired())
				{
					for (auto & component : object.lock()->components)
					{
						std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
						if (cast.expired()) continue;

						return cast;
					}

					return object.lock()->GetComponentWithParent<T>();
				}
			}

			return std::weak_ptr<T>();
		}

		// �q�̃R���|�[�l���g���擾
		template <typename T>
		std::weak_ptr<T> GetComponentWithChild()
		{
			for (auto & child : transform.lock()->GetChilds())
			{
				auto obj = child.lock()->gameObject;
				if (obj.expired()) continue;

				for (auto & component : obj.lock()->components)
				{
					std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
					if (cast.expired()) continue;

					return cast;
				}

				return obj.lock()->GetComponentWithChild<T>();
			}

			return std::weak_ptr<T>();
		}

		// �R���|�[�l���g�Q���擾
		template <typename T>
		void GetComponents(std::list<std::weak_ptr<T>> * arrays)
		{
			for (auto & component : components)
			{
				// �_�C�i�~�b�N�L���X�g���邱�Ƃł��̃R���|�[�l���g���𔻒f����
				// typeid���ƌp���O�̃N���X�̌^�Ŏ��Ȃ�����
				std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
				if (cast.expired()) continue;

				arrays->emplace_back(cast);
			}
		}

		// �e�̃R���|�[�l���g�Q���擾
		template <typename T>
		void GetComponentsWithParent(std::list<std::weak_ptr<T>> * arrays)
		{
			auto parent = transform.lock()->GetParent();
			if (!parent.expired())return;

			auto object = parent.lock()->gameObject;
			if (!object.expired()) return;

			for (auto & component : object.lock()->components)
			{
				std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
				if (cast.expired()) continue;

				arrays->emplace_back(cast);
			}

			object.lock()->GetComponentsWithParent<T>(arrays);
		}

		// �q�̃R���|�[�l���g�Q���擾
		template <typename T>
		void GetComponentsWithChild(std::list<std::weak_ptr<T>> * arrays)
		{
			for (auto & child : transform.lock()->GetChilds())
			{
				auto obj = child.lock()->gameObject;
				if (obj.expired()) continue;

				for (auto & component : obj.lock()->components)
				{
					std::weak_ptr<T> cast = std::dynamic_pointer_cast<T>(component.lock());
					if (cast.expired()) continue;

					arrays->emplace_back(cast);
				}

				obj.lock()->GetComponentsWithChild<T>(arrays);
			}
		}
		
		// �R���|�[�l���g�̍폜
		template <typename T>
		void RemoveComponent()
		{
			for (auto itr = components.begin(); itr != components.end(); ++itr)
			{
				if (typeid(T) == itr->lock()->GetType())
				{
					Component::RemoveComponent(*itr);
					return;
				}
			}
		}
		void RemoveComponent(std::weak_ptr<Component> component)
		{
			for (auto itr = components.begin(); itr != components.end(); ++itr)
			{
				if (component.lock() == itr->lock())
				{
					Component::RemoveComponent(*itr);
					return;
				}
			}
		}

		// ���~�b�g( �b�� )�o�ߌ�ɃI�u�W�F�N�g���폜����
		static void Destroy(std::weak_ptr<GameObject> gameObject, float limit = 0);

		// �V�[����ύX���Ă��폜����Ȃ��I�u�W�F�N�g�Ƃ��ēo�^
		static void DontDestroyOnLoad(std::weak_ptr<GameObject> object);

	private:

		static void DontDestroyOnLoadChild(std::weak_ptr<Transform> child);

		// ���g�̃C���X�^���X������
		void Initialize(std::shared_ptr<GameObject> thisObject);
		void Finalize();
		void RemoveComponentList(Component * component);
		void SetTransform(std::weak_ptr<Transform> & transform);
		void SetMonoBehaviour(std::weak_ptr<MonoBehaviour> & behaviour);
		void RemoveMonoBehaviour(MonoBehaviour * behaviour);

		//// �����蔻��œ����������ɌĂ�
		void OnCollision(std::weak_ptr<Collision> mine, std::weak_ptr<Collision> other);
		void OnTrigger(std::weak_ptr<Collision> mine, std::weak_ptr<Collision> other);

		// ���~�b�g( �b�� )�o�ߌ�ɃI�u�W�F�N�g���폜����
		void OnDestroy(float limit = 0);

		// �V���A���������̊֐�
		void LoadSerialized();

		std::list<std::weak_ptr<Component>> components;
		std::list<std::weak_ptr<MonoBehaviour>> behaviours;

		Layer layer;
		bool isActive;	// �I�u�W�F�N�g���L�����ǂ���
		bool isStop;	// �I�u�W�F�N�g���ꎞ��~���邽�߂̃t���O
		bool isDontDestroyOnLoad;	// �V�[����؂�ւ������ɏ�����邩�̃t���O
	};
}

#endif // !_GAMEOBJECT_H_

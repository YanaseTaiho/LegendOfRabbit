#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <typeinfo>
#include <memory>
#include <list>

#include "../Layer.h"

#include "../../Cereal/Common.h"
#include "DirectX/ImGui/imgui.h"

namespace FrameWork
{
#define STRING(var) #var   //�����ɂ����ϐ���ϐ��������������񃊃e�����Ƃ��ĕԂ��}�N���֐�

	class GameObject;
	class Transform;
	class MonoBehaviour;
	class Animator;
	class Rigidbody;
	class Collision;
	class Light;
	class Camera;
	class Renderer;
	
	class Component
	{
		friend GameObject;
		friend Transform;
		friend MonoBehaviour;
		friend Animator;
		friend Rigidbody;
		friend Collision;
		friend Light;
		friend Camera;
		friend Renderer;
	private:
		friend cereal::access;
		template<typename Archive>
		void save(Archive & archive) const
		{
			archive(CEREAL_NVP(enable), CEREAL_NVP(gameObject), CEREAL_NVP(transform));
		}
		template<typename Archive>
		void load(Archive & archive)
		{
			archive(CEREAL_NVP(enable), CEREAL_NVP(gameObject), CEREAL_NVP(transform));
		}

		// �ǂݍ��ݎ��͂��̊֐����I�[�o�[���C�h���čs�����������S
		virtual void LoadSerialized(const std::shared_ptr<Component> & component)
		{
			allComponentList.emplace_back(component);
		}

	public:
		virtual ~Component() 
		{
			gameObject.reset();
			transform.reset();
		}

		virtual void DrawImGui(int id) {};

		static void CleanupDestoryComponent();

		const type_info& GetType() { return typeid(*this); }

		std::string ClassName()
		{
			std::string className = typeid(*this).name();
			size_t pos = className.find_last_of(":");	// ���O��Ԃ̊m�F
			if (pos == std::string::npos)
			{
				pos = className.find_first_of(" ");
			}
			return className.substr(pos + 1, className.size() - 1);
		}

		void SetEnable(bool enable) { this->enable = enable; }
		bool & Enable() { return this->enable; }
		bool IsEnable();
		bool IsStop();
		Layer GetLayer() { return layer; };

		// ���g�̃I�u�W�F�N�g�̎Q�Ƃ𓾂�
		std::weak_ptr<GameObject> gameObject;
		std::weak_ptr<Transform> transform;

		template<class T>
		static std::list<std::weak_ptr<T>> FindComponents()
		{
			std::list<std::weak_ptr<T>> outList;

			for (auto & com : allComponentList)
			{
				if (com->GetType() == typeid(T))
				{
					std::shared_ptr<T> hit = std::static_pointer_cast<T>(com);
					outList.emplace_back(hit);
				}
			}
			return outList;
		}

	private:
		bool enable = true;
		Layer layer;

		static std::list<std::shared_ptr<Component>> allComponentList;
		static std::list<std::weak_ptr<Component>> destroyComponentList;

		static std::weak_ptr<Component> AddComponent(Component * component, std::weak_ptr<GameObject> & object, std::weak_ptr<Transform> & transform);
		static void RemoveComponent(std::weak_ptr<Component> & component);

		void Initialize(std::weak_ptr<GameObject> & object, std::weak_ptr<Transform> & transform);
		virtual void ChangeLayer(Layer layer)
		{
			this->layer = layer;
		}

		// �������Ɉ�x�����Ă΂��
		virtual void Awake() {}
		// �폜����钼�O�ɌĂ΂��
		virtual void OnDestroy() {}

		virtual void AddComponent()
		{
			
		}
		virtual void RemoveComponent();
		

		template <class T>
		void AddComponentList(std::list<std::weak_ptr<T>> & components)
		{
			components.emplace_back(std::static_pointer_cast<T>(allComponentList.back()));
		}
		template <class T>
		void AddComponentList(std::list<std::weak_ptr<T>> & components, std::weak_ptr<T> & com)
		{
			components.emplace_back(com);
		}

		template <class T>
		std::weak_ptr<T> RemoveComponentList(std::list<std::weak_ptr<T>> & components)
		{
			for (auto itr = components.begin(), end = components.end(); itr != end; ++itr)
			{
				if (itr->lock().get() == this)
				{
					std::weak_ptr<T> com = *itr;
					components.erase(itr);
					return com;
				}
			}
			return std::weak_ptr<T>();
		}
	};
}

#endif // !_COMPONENT_H_


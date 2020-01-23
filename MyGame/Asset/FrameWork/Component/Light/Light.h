#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "../Component.h"
#include <list>

namespace FrameWork
{
	class Light : public Component
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override
		{
			Component::LoadSerialized(component);
			AddComponentList(lightList);
		}

	public:
		Light() {};
		virtual ~Light() {};

		virtual void SetLight() {};

		static std::list<std::weak_ptr<Light>> & ComponentList() { return lightList; }
	private:

		void AddComponent() override final
		{
			Component::AddComponent();
			AddComponentList(lightList);
			Awake();
		}

		void RemoveComponent() override final
		{
			RemoveComponentList(lightList);
			Component::RemoveComponent();
		}

		static std::list<std::weak_ptr<Light>> lightList;
	};
}

CEREAL_REGISTER_TYPE(FrameWork::Light)

#endif // !_LIGHT_H_


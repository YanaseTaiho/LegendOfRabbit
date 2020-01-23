#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../Component.h"
#include "../../Vector.h"
#include <list>

namespace FrameWork
{
	class Camera : public Component
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
			AddComponentList(cameraList);
		}

	public:
		Camera() {}
		virtual ~Camera() {}

		static std::list<std::weak_ptr<Camera>> & ComponentList() { return cameraList; }

		virtual bool IsVisiblity(Vector3 position) = 0;
		virtual void Draw() = 0;
		// カメラのスクリーン座標からワールド座標へ変換( depth : 0.0f ～ 1.0f )
		virtual void ScreenToWorldPoint(Vector3 & outPos, const Vector2 & screenPos, float depth) {}

	private:

		void AddComponent() override
		{
			Component::AddComponent();
			AddComponentList(cameraList);
			Awake();
		}

		void RemoveComponent() override
		{
			RemoveComponentList(cameraList);
			Component::RemoveComponent();
		}

		static std::list<std::weak_ptr<Camera>> cameraList;
	};
}

CEREAL_REGISTER_TYPE(FrameWork::Camera)

#endif // !_CAMERA_H_


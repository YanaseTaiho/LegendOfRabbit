#ifndef _LIGHT__H__
#define _LIGHT__H__

#include "FrameWork/Component/Light/Light.h"
#include "FrameWork/Color.h"
#include "FrameWork/Matrix.h"

using namespace FrameWork;

namespace MyDirectX
{
	class DirectionalLight : public Light
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(cereal::make_nvp("Light", cereal::base_class<Light>(this)),
				CEREAL_NVP(diffuse), CEREAL_NVP(ambient), CEREAL_NVP(orthoMatrix));
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::make_nvp("Light", cereal::base_class<Light>(this)),
				CEREAL_NVP(diffuse), CEREAL_NVP(ambient), CEREAL_NVP(orthoMatrix));
		}
	public:
		DirectionalLight(Color diffuse, Color ambient) : diffuse(diffuse), ambient(ambient) {}
		DirectionalLight() { ambient = Color(0.3f, 0.3f, 0.3f, 1.0f); }
		~DirectionalLight() {}

		void Awake() override;
		void DrawImGui(int id) override;
		void SetLight() override;

		Color diffuse;
		Color ambient;
		Matrix4 orthoMatrix;	// ê≥éÀâeçsóÒ

	};
}

CEREAL_CLASS_VERSION(MyDirectX::DirectionalLight, 0)
CEREAL_REGISTER_TYPE(MyDirectX::DirectionalLight)
CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, MyDirectX::DirectionalLight)

#endif // !_LIGHT__H__


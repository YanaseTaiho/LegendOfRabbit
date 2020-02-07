#ifndef _BILLBOARD_RENDERER_H_
#define _BILLBOARD_RENDERER_H_

#include "../../FrameWork/Component/Renderer/Renderer.h"
#include "../MeshData/PlaneMesh/PlaneMesh.h"
#include "../Texture/TextureManager.h"

namespace MyDirectX
{
	class BillboardRenderer : public Renderer
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			std::string texName = (!texture.expired()) ? texture.lock()->GetName() : "";
			archive(cereal::base_class<Renderer>(this));
			archive(texName, rotation, rect, uv1, uv2, isDepthEnable);
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			std::string texName;
			archive(cereal::base_class<Renderer>(this));
			archive(texName, rotation, rect, uv1, uv2, isDepthEnable);
			texture = Singleton<TextureManager>::Instance()->Load(texName);
		}

		void DrawImGui(int id) override;

	public:
		BillboardRenderer() : uv1(0.0f, 0.0f), uv2(1.0f, 1.0f), isDepthEnable(true){};
		~BillboardRenderer() {};

		void Draw() override;

		std::weak_ptr<Texture> texture;
		float rotation;
		Rectf rect;
		Vector2 uv1;
		Vector2 uv2;
		bool isDepthEnable;
	private:

	};
}

CEREAL_CLASS_VERSION(MyDirectX::BillboardRenderer, 0)
CEREAL_REGISTER_TYPE(MyDirectX::BillboardRenderer)

#endif // !_BILLBOARD_RENDERER_H_


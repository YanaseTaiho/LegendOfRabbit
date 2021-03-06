#ifndef _CANVASRENDERER_H_
#define _CANVASRENDERER_H_

#include "../../FrameWork/Component/Renderer/Renderer.h"
#include "../MeshData/PlaneMesh/PlaneMesh.h"
#include "../Texture/TextureManager.h"

namespace MyDirectX
{
	class CanvasRenderer : public Renderer
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			std::string texName = (!texture.expired()) ? texture.lock()->GetName() : "";
			archive(cereal::base_class<Renderer>(this));
			archive(texName, rotation, rect, uv1, uv2);
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			std::string texName;
			archive(cereal::base_class<Renderer>(this));
			archive(texName, rotation, rect, uv1, uv2);
			texture = Singleton<TextureManager>::Instance()->Load(texName);
		}

		void DrawImGui(int id) override;

	public:
		CanvasRenderer();
		~CanvasRenderer();

		void Draw() override;

		std::weak_ptr<Texture> texture;
		float rotation;
		Rectf rect;
		Vector2 uv1;
		Vector2 uv2;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::CanvasRenderer, 0)
CEREAL_REGISTER_TYPE(MyDirectX::CanvasRenderer)

#endif // !_CANVASRENDERER_H_


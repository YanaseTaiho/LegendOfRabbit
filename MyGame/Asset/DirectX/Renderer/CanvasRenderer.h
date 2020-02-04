#ifndef _CANVASRENDERER_H_
#define _CANVASRENDERER_H_

#include "../../FrameWork/Component/Renderer/Renderer.h"
#include "../MeshData/PlaneMesh/PlaneMesh.h"

namespace MyDirectX
{
	class CanvasRenderer : public Renderer
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("DXRenderer", cereal::base_class<Renderer>(this)));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("DXRenderer", cereal::base_class<Renderer>(this)));
		}

		void DrawImGui(int id) override;

	public:
		CanvasRenderer();
		CanvasRenderer(std::string imageName);
		~CanvasRenderer();

		void Draw() override;

		std::shared_ptr<PlaneMesh> model;
	};
}

CEREAL_REGISTER_TYPE(MyDirectX::CanvasRenderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, MyDirectX::CanvasRenderer)

#endif // !_CANVASRENDERER_H_


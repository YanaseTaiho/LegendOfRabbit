#ifndef _LOCUS_RENDERER_H_
#define _LOCUS_RENDERER_H_

#include "../../FrameWork/Component/Renderer/Renderer.h"
#include "../MeshData/Mesh/Mesh.h"
#include "../Material/MaterialManager.h"

// åïÇÃãOê’Çï\åªÇ∑ÇÈÇΩÇﬂÇÃÉNÉâÉX

namespace MyDirectX
{
	class LocusRenderer : public Renderer
	{
	private:
		friend cereal::access;

		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			std::string matName = (material.expired()) ? "" : material.lock()->name;

			archive(cereal::base_class<Renderer>(this));
			archive(meshData, matName);
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			std::string matName;

			archive(cereal::base_class<Renderer>(this));
			archive(meshData, matName);

			material = Singleton<MaterialManager>::Instance()->GetMaterial(matName);
		}

	public:
		LocusRenderer() {}
		~LocusRenderer();

		void DrawImGui(int id) override;

		void CreateMesh(int frameMax);
		void SetMesh(const std::vector<Vector3> & pos1, const  std::vector<Vector3> & pos2, int startFrame, int endFrame);
		void Awake() override;
		void Draw() override;

		MeshData<VTX_MESH> meshData;
		unsigned short startNum;
		unsigned short indexNum;
		std::weak_ptr<Material> material;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::LocusRenderer, 0)
CEREAL_REGISTER_TYPE(MyDirectX::LocusRenderer)

#endif // !_LOCUS_RENDERER_H_


#ifndef __MESH__H__
#define __MESH__H__

#include "FrameWork/Common.h"
#include "../MeshNode.h"

namespace MyDirectX
{
	struct VTX_MESH
	{
		Vector3 pos;
		Vector2 uv;
		Vector3 nor;
		Vector3 tan;		// ê⁄ãÛä‘
		Vector3 binor;		// è]ñ@ê¸

	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(pos), CEREAL_NVP(uv), CEREAL_NVP(nor), CEREAL_NVP(tan), CEREAL_NVP(binor));
		}
	};

	class Mesh
	{
	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(meshNode));
		}

	public:
		Mesh() {}
		~Mesh()
		{
			std::vector<MeshNode<VTX_MESH>>().swap(meshNode);
		}

		std::string name;
		std::vector<MeshNode<VTX_MESH>> meshNode;

		void Initialize();
		void Draw(Transform * transform, std::vector<std::weak_ptr<Material>> & materialArray);
		void DrawShadow(Transform * transform);
		int GetMaterialNum();
	};
}

CEREAL_CLASS_VERSION(MyDirectX::VTX_MESH, 0)
CEREAL_CLASS_VERSION(MyDirectX::Mesh, 0)

#endif // !_MESH_H_

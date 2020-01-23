#ifndef _SKINMESH_H_
#define _SKINMESH_H_

#include "FrameWork/Common.h"
#include "FrameWork/Animation/SkeletonData.h"
#include "../MeshNode.h"

namespace MyDirectX
{
	// 一つの頂点情報を格納する構造体
	struct VTX_SKIN_MESH
	{
		Vector3 pos;
		Vector2 uv;
		Vector3 nor;
		int     idx[4];		// ボーン行列のインデックス番号
		float   weight[4];	// ボーンのウェイト情報
		Vector3 tan;		// 接空間
		Vector3 binor;		// 従法線

	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(pos), CEREAL_NVP(uv), CEREAL_NVP(nor), CEREAL_NVP(idx), CEREAL_NVP(weight), CEREAL_NVP(tan), CEREAL_NVP(binor));
		}
	};

	class SkinMesh
	{
	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(meshNode), CEREAL_NVP(skeleton), CEREAL_NVP(boneData));
		}
	public:
		SkinMesh() {};
		~SkinMesh()
		{
			std::vector<MeshNode<VTX_SKIN_MESH>>().swap(meshNode);
			std::vector<BoneData>().swap(boneData);
			skeleton.reset();
		}

		std::string name;
		std::vector<MeshNode<VTX_SKIN_MESH>> meshNode;
		std::shared_ptr<Skeleton> skeleton;
		std::vector<BoneData> boneData;

		void Initialize();

		void Draw(Transform * transform, std::vector<std::weak_ptr<Material>> & materialArray, const std::vector<std::weak_ptr<Transform>> & bones);
		void DrawShadow(Transform * transform, const std::vector<std::weak_ptr<Transform>> & bones);
		int GetMaterialNum();
	};
}

CEREAL_CLASS_VERSION(MyDirectX::VTX_SKIN_MESH, 0)
CEREAL_CLASS_VERSION(MyDirectX::SkinMesh, 0)

#endif // !_SKINMESH_H_


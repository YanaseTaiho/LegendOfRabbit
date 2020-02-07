#ifndef __PLANE_MESH__H__
#define __PLANE_MESH__H__

#include "FrameWork/Common.h"
#include "../MeshData.h"
#include "../../Material/Material.h"

namespace MyDirectX
{
	struct VTX_PLANE_MESH
	{
		Vector3 pos;
		Vector2 uv;
		Vector3 nor;

	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(pos), CEREAL_NVP(uv), CEREAL_NVP(nor), CEREAL_NVP(tan));
		}
	};

	// Ç†Ç‹ÇËÇÊÇ≠Ç»Ç¢ÇØÇ«éûä‘éûä‘Ç™Ç»Ç¢ÇÃÇ≈íÜêgÇê√ìIÇ…Ç∑ÇÈ
	class PlaneMesh
	{
	public:
		//PlaneMesh();
		//~PlaneMesh();

		static void CreateMesh();
		static void ReleaseMesh();

		static std::weak_ptr<Material> GetMaterial();
		static void SetTexture(std::weak_ptr<Texture> texture);
		static void SetUV(float u1, float v1, float u2, float v2);
		static void Draw(float left, float right, float top, float bottom);

	private:
		static MeshData<VTX_PLANE_MESH> meshData;
		static std::shared_ptr<Material> material;
	};
}

#endif // !__PLANE_MESH__H__

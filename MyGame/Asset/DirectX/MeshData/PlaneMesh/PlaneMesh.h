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

	class PlaneMesh
	{
	public:
		PlaneMesh();
		~PlaneMesh();

		std::string name;
		MeshData<VTX_PLANE_MESH> meshData;
		std::shared_ptr<Material> material;

		void Draw(Transform * transform);
		void CanvasDraw(float left, float right, float top, float bottom);
		void DrawShadow(Transform * transform);
	};
}

#endif // !__PLANE_MESH__H__

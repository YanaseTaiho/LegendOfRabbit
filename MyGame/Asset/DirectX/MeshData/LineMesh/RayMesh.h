#ifndef __RAYMESH__H__
#define __RAYMESH__H__

#include "LineMesh.h"

namespace MyDirectX
{
	class RayMesh
	{
	public:
		RayMesh();
		~RayMesh() {}

		void Draw(const Vector3& start, const Vector3& end, const Color& color, const float width = 0.0f);
		void Draw(const Vector3& start, const Vector3& dir, const float& length, const Color& color, const float width = 0.0f);

		MeshData<VTX_LINE> meshData;
	};
}

#endif // !__RAYMESH__H__
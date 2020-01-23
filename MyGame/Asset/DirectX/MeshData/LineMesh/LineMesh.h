#ifndef __LINE__H__
#define __LINE__H__

#include "../MeshData.h"
#include "FrameWork/Common.h"

namespace MyDirectX
{
	struct VTX_LINE
	{
		Vector3 pos;
	};

	class LineMesh
	{
	public:

		MeshData<VTX_LINE> meshData;

		LineMesh();
		~LineMesh() {}

		void Draw(const Matrix4 & matrix, const Color & color, const float width = 0.0f);
	};
}

#endif // !__LINE__H__
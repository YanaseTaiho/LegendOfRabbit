#ifndef _FROMASSIMP_H_
#define _FROMASSIMP_H_

#include <assimp/matrix4x4.h>
#include "FrameWork/Matrix.h"

namespace MyDirectX
{
	namespace Assimp
	{
		FrameWork::Matrix4 FromAssimpMatrix(const aiMatrix4x4 & aiMatrix)
		{
			FrameWork::Matrix4 matrix;
			matrix.matrix(0, 0) = aiMatrix.a1;
			matrix.matrix(0, 1) = aiMatrix.a2;
			matrix.matrix(0, 2) = aiMatrix.a3;
			matrix.matrix(0, 3) = aiMatrix.a4;

			matrix.matrix(1, 0) = aiMatrix.b1;
			matrix.matrix(1, 1) = aiMatrix.b2;
			matrix.matrix(1, 2) = aiMatrix.b3;
			matrix.matrix(1, 3) = aiMatrix.b4;

			matrix.matrix(2, 0) = aiMatrix.c1;
			matrix.matrix(2, 1) = aiMatrix.c2;
			matrix.matrix(2, 2) = aiMatrix.c3;
			matrix.matrix(2, 3) = aiMatrix.c4;

			matrix.matrix(3, 0) = aiMatrix.d1;
			matrix.matrix(3, 1) = aiMatrix.d2;
			matrix.matrix(3, 2) = aiMatrix.d3;
			matrix.matrix(3, 3) = aiMatrix.d4;

			return matrix;
		}
	}
}

#endif // !_FROMASSIMP_H_


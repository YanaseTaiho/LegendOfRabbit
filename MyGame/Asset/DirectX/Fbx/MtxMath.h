#ifndef _FBXMTXMATH_H_
#define _FBXMTXMATH_H_

//==================================================================================================
// �C���N���[�h
//==================================================================================================
#include "../DirectX.h"
#include <fbxsdk.h>
#include <cmath>

namespace MyDirectX
{
	namespace Fbx
	{
		// �v���g�^�C�v�錾---------------------------------------------------------------------------------
		XMMATRIX BlendMtx(XMMATRIX mtx1, XMMATRIX mtx2, float weight);
		XMMATRIX ConvFbxMtxXmtx(FbxMatrix& pSrc);
		XMMATRIX ConvFbxMtxXmtx(FbxAMatrix& pSrc);
	}
}

#endif // !_FBXMTXMATH_H_
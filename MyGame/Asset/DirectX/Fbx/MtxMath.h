#ifndef _FBXMTXMATH_H_
#define _FBXMTXMATH_H_

//==================================================================================================
// インクルード
//==================================================================================================
#include "../DirectX.h"
#include <fbxsdk.h>
#include <cmath>

namespace MyDirectX
{
	namespace Fbx
	{
		// プロトタイプ宣言---------------------------------------------------------------------------------
		XMMATRIX BlendMtx(XMMATRIX mtx1, XMMATRIX mtx2, float weight);
		XMMATRIX ConvFbxMtxXmtx(FbxMatrix& pSrc);
		XMMATRIX ConvFbxMtxXmtx(FbxAMatrix& pSrc);
	}
}

#endif // !_FBXMTXMATH_H_
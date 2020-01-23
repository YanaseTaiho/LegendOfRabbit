#ifndef __FBX__H__
#define __FBX__H__

//==================================================================================================
// インクルード
//==================================================================================================
#include "FrameWork/Vector.h"
#include "../Material/Material.h"
#include "../MeshInfomation/SkinMesh/SkinMesh.h"
#include <fbxsdk.h>
#include <string>
#include <vector>

namespace MyDirectX
{
	class CLight;
	struct VTX_SKIN_MESH;

	namespace Fbx
	{
		class BoneManager;

		using TmpWeight = std::pair<int, float>;

		// FBXクラス----------------------------------------------------------------------------------------
		class FbxLoader
		{
		public:
			// スキンメッシュを作成
			static SkinMesh * LoadSkinMesh(std::string fileName);

		private:
			FbxLoader() {};
			~FbxLoader();

			static std::string filePath;

			//static float m_maxX, m_maxY, m_maxZ;

			static std::vector<XMFLOAT3> positionArray;
			static std::vector<XMFLOAT3> normalArray;
			static std::vector<XMFLOAT2> uvArray;
			static std::vector<XMFLOAT3> tangentArray;
			static std::vector<XMFLOAT3> binormalArray;

			static Skeleton skeleton;
			static std::vector<std::vector<TmpWeight>> tmpBoneWeightList;

			// 初期化
			static void LoadData(FbxNode *pParentNode, SkinMesh *pSkinMesh, const XMMATRIX *parentMtx);
			static void LoadMaterial(FbxNode *pNode, SkinMesh::MeshNode * pSkinMeshNode);
			static void SetMaterial(Material * material, FbxSurfaceMaterial * fbxMaterial);
			static void LoadBone(FbxMesh* pMesh, SkinMesh::MeshNode* pSkinMeshNode, const XMMATRIX* pMtx);

			// 頂点関連
			static void LoadVtxPos(FbxMesh* pMesh, const XMMATRIX*);
			static void LoadVtxUv(FbxMesh* pMesh,FbxLayer *pLayer);
			static void LoadVtxNormal(FbxMesh* pMesh,FbxLayer *pLayer, const XMMATRIX *);
			static void LoadVtxBinormal(FbxMesh* pMesh,FbxLayer *pLayer, const XMMATRIX *);
			static void LoadVtxTangent(FbxMesh* pMesh,FbxLayer *pLayer, const XMMATRIX *);

			static bool ProcessSkeletonHeirarchyre(FbxNode * node, int depth, int num, int index, int parentindex, std::vector<int> *hierarchy);
			static Joint* GetSkeleton(std::string name);

			static XMMATRIX GetLclMtx(FbxNode *, XMMATRIX *);
			static XMMATRIX GetPreMtx(FbxNode *);

//			VTX_SKIN_MESH* GetNodeVtxToPos(const XMVECTOR *pos, CPolygonNode *pPolygonNode);
//			void DrawTreeShadow(const XMMATRIX *world, CPolygonNode *pPolygonNode);
			bool CheckVtxToPos(const XMVECTOR *pPos, const XMVECTOR *pNormal, VTX_SKIN_MESH *pVtx);

			// 指定したボーンのキーフレーム時のワールド行列を取得
			static Matrix4 GetSkeltonKeyFrameWorldMatrix(Joint * joint, const int takeNum, const int keyFrame);
		};
	}

}

#endif // !__FBX__H__
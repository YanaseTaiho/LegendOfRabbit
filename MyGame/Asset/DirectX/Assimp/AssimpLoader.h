#ifndef _ASSIMPLOADER_H_
#define _ASSIMPLOADER_H_

#define NOMINMAX

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <string>
#include <unordered_map>

#pragma comment (lib, "assimp.lib")

#include "../MeshData/SkinMesh/SkinMesh.h"
#include "../MeshData/Mesh/Mesh.h"

namespace MyDirectX
{
	namespace Assimp
	{
		using TmpWeight = std::pair<int, float>;

		class AssimpLoader
		{
		public:
			// メッシュを作成
			static bool LoadMesh(std::string fileName, Mesh * mesh);
			// スキンメッシュを作成
			static bool LoadSkinMesh(std::string fileName, SkinMesh * skinMesh);
			
		private:
			AssimpLoader() {};
			~AssimpLoader() {};

			static std::string filePath;
			static std::string modelName;
			static std::vector<BoneData> bonesData;	// 名前でノードからボーンであるか判断するために使う
			static Skeleton skeleton;
			static int meshCnt;

			static void LoadMeshNode(Mesh * mesh, const aiScene * scene, aiNode * node, const aiMatrix4x4& parentTransform);
			static void LoadSkinMeshNode(SkinMesh * skinMesh, const aiScene * scene, aiNode * node, const aiMatrix4x4& parentTransform);
			static void LoadMaterial(const aiMaterial * aiMat, Material * mat);
			//D3DXVECTOR4 CalcTangent
			//頂点3つとUV座標３つと法線をもとに接線を計算する。返す4次元ベクトルのwは左か右手系かの判断用フラグ。
			static XMFLOAT4 CalcTangent(Vector3 v1, Vector3 v2, Vector3 v3,
				Vector2 uv1, Vector2 uv2, Vector2 uv3, Vector3 normal);

			static void CalcTangent(Vector3 & tangent0, Vector3 & binormal0, Vector3 const & p0, Vector2 const & uv0, Vector3 const & normal0,
				Vector3 & tangent1, Vector3 & binormal1, Vector3 const & p1, Vector2 const & uv1, Vector3 const & normal1,
				Vector3 & tangent2, Vector3 & binormal2, Vector3 const & p2, Vector2 const & uv2, Vector3 const & normal2);

			// ノードから全てのボーンの名前を取得する
			static void SearchNodeBone(const aiScene * scene, const aiNode * node, std::vector<BoneData> & bones);
			// ノードの名前からボーンであるか判断
			static BoneData * GetBoneName(std::string name, std::vector<BoneData> & bones);
			static UINT GetBoneIndex(std::string name, std::vector<BoneData> & bones);
			// 階層ありのスケルトンを生成するための関数
			static void ProcessSkeletonHeirarchyre(const aiNode * node, std::vector<BoneData> & boneDataArray, aiMatrix4x4 & parentMatrix, int num, int index, int parentindex, std::vector<int> * hierarchy);
			static void WeightListNormalize(std::vector<std::vector<TmpWeight>> & tmpBoneWeightList);
			static void LoadBoneWeightList(const aiMesh * mesh, std::vector<std::vector<TmpWeight>> * tmpBoneWeightList);
			// アニメーションデータ読み込み
			static void LoadAnimation(const aiScene * scene);
			static void LoadNodeAnim(const aiAnimation * anim, const UINT takeNum, const UINT keyFrame, const aiNode * node);

			static aiNodeAnim * FindNodeAnim(const aiAnimation * anim, aiString nodeName);
			static aiQuaternion FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
			static aiVector3D FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim);
			static aiVector3D FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		};
	}
}

#endif // !_ASSIMPLOADER_H_


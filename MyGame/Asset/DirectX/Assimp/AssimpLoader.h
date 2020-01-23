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
			// ���b�V�����쐬
			static bool LoadMesh(std::string fileName, Mesh * mesh);
			// �X�L�����b�V�����쐬
			static bool LoadSkinMesh(std::string fileName, SkinMesh * skinMesh);
			
		private:
			AssimpLoader() {};
			~AssimpLoader() {};

			static std::string filePath;
			static std::string modelName;
			static std::vector<BoneData> bonesData;	// ���O�Ńm�[�h����{�[���ł��邩���f���邽�߂Ɏg��
			static Skeleton skeleton;
			static int meshCnt;

			static void LoadMeshNode(Mesh * mesh, const aiScene * scene, aiNode * node, const aiMatrix4x4& parentTransform);
			static void LoadSkinMeshNode(SkinMesh * skinMesh, const aiScene * scene, aiNode * node, const aiMatrix4x4& parentTransform);
			static void LoadMaterial(const aiMaterial * aiMat, Material * mat);
			//D3DXVECTOR4 CalcTangent
			//���_3��UV���W�R�Ɩ@�������Ƃɐڐ����v�Z����B�Ԃ�4�����x�N�g����w�͍����E��n���̔��f�p�t���O�B
			static XMFLOAT4 CalcTangent(Vector3 v1, Vector3 v2, Vector3 v3,
				Vector2 uv1, Vector2 uv2, Vector2 uv3, Vector3 normal);

			static void CalcTangent(Vector3 & tangent0, Vector3 & binormal0, Vector3 const & p0, Vector2 const & uv0, Vector3 const & normal0,
				Vector3 & tangent1, Vector3 & binormal1, Vector3 const & p1, Vector2 const & uv1, Vector3 const & normal1,
				Vector3 & tangent2, Vector3 & binormal2, Vector3 const & p2, Vector2 const & uv2, Vector3 const & normal2);

			// �m�[�h����S�Ẵ{�[���̖��O���擾����
			static void SearchNodeBone(const aiScene * scene, const aiNode * node, std::vector<BoneData> & bones);
			// �m�[�h�̖��O����{�[���ł��邩���f
			static BoneData * GetBoneName(std::string name, std::vector<BoneData> & bones);
			static UINT GetBoneIndex(std::string name, std::vector<BoneData> & bones);
			// �K�w����̃X�P���g���𐶐����邽�߂̊֐�
			static void ProcessSkeletonHeirarchyre(const aiNode * node, std::vector<BoneData> & boneDataArray, aiMatrix4x4 & parentMatrix, int num, int index, int parentindex, std::vector<int> * hierarchy);
			static void WeightListNormalize(std::vector<std::vector<TmpWeight>> & tmpBoneWeightList);
			static void LoadBoneWeightList(const aiMesh * mesh, std::vector<std::vector<TmpWeight>> * tmpBoneWeightList);
			// �A�j���[�V�����f�[�^�ǂݍ���
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


#include "AssimpLoader.h"
#include "../Texture/TextureManager.h"
#include "FromAssimp.h"

using namespace MyDirectX;
using namespace MyDirectX::Assimp;

std::string AssimpLoader::filePath;
std::string AssimpLoader::modelName;

// 階層なしのボーン情報
std::vector<BoneData> AssimpLoader::bonesData;
// 階層ありのボーン情報
Skeleton AssimpLoader::skeleton;

int AssimpLoader::meshCnt = 0;

bool AssimpLoader::LoadMesh(std::string fileName, Mesh * mesh)
{
	const aiScene * scene;
	scene = aiImportFile(fileName.c_str(),
		aiProcessPreset_TargetRealtime_MaxQuality |		// 三角ポリゴンで読み込んでる
		aiProcess_ConvertToLeftHanded);					// 左手座標系で読みこむ

	if (!scene)
		return false;

	size_t pos = fileName.find_last_of("\\/");
	size_t pos2 = fileName.find_last_of(".");
	filePath = fileName.substr(0, pos + 1);
	modelName = fileName.substr(pos + 1, pos2 - pos - 1);

	mesh->name = modelName;
	
	meshCnt = 0;
	// あらかじめメッシュノードのサイズを作っておく
	mesh->meshNode.resize(scene->mNumMeshes);

	// メッシュ情報を作成
	aiMatrix4x4 rootTransform;
	LoadMeshNode(mesh, scene, scene->mRootNode, rootTransform);

	// データ解放
	filePath.clear();
	modelName.clear();
	meshCnt = 0;

	// モデルの解放
	aiReleaseImport(scene);

	return true;
}

bool AssimpLoader::LoadSkinMesh(std::string fileName, SkinMesh * skinMesh)
{
	const aiScene * scene;
	scene = aiImportFile(fileName.c_str(),
		aiProcessPreset_TargetRealtime_MaxQuality |		// 三角ポリゴンで読み込んでる
		aiProcess_ConvertToLeftHanded);					// 左手座標系で読みこむ

	size_t pos = fileName.find_last_of("\\/");
	size_t pos2 = fileName.find_last_of(".");
	filePath = fileName.substr(0, pos + 1);
	modelName = fileName.substr(pos + 1, pos2 - pos - 1);

	if (!scene)
		return false;

	meshCnt = 0;

	// あらかじめメッシュノードのサイズを作っておく
	skinMesh->meshNode.resize(scene->mNumMeshes);

	skinMesh->name = modelName;
	// ここでノードにある全てのボーンを配列にまとめる
	std::vector<BoneData> boneDataArray;
	SearchNodeBone(scene, scene->mRootNode, boneDataArray);

	// スキンメッシュではないので失敗
	if (boneDataArray.size() == 0)
		return false;

	// 階層の順番になるようにボーン情報を格納
	std::vector<int> hierarchy;
	aiMatrix4x4 rootMat;
	ProcessSkeletonHeirarchyre(scene->mRootNode, boneDataArray, rootMat, 0, 0, -1, &hierarchy);

	// メッシュ情報を作成
	aiMatrix4x4 rootTransform;
	LoadSkinMeshNode(skinMesh, scene, scene->mRootNode, rootTransform);

	// アニメーション読み込み
	LoadAnimation(scene);

	skinMesh->skeleton = std::shared_ptr<Skeleton>(new Skeleton(skeleton));
	skinMesh->boneData = bonesData;

	// データ解放
	filePath.clear();
	modelName.clear();
	meshCnt = 0;
	std::vector<int>().swap(hierarchy);
	std::vector<BoneData>().swap(boneDataArray);
	std::vector<BoneData>().swap(bonesData);
	std::vector<AnimInfo>().swap(skeleton.animInfo);
	std::vector<Joint>().swap(skeleton.mJoints);

	// モデルの解放
	aiReleaseImport(scene);

	return true;
}

void AssimpLoader::LoadMeshNode(Mesh * mesh, const aiScene * scene, aiNode * node, const aiMatrix4x4 & parentTransform)
{
	aiMatrix4x4 rootTransform = parentTransform * node->mTransformation;

	// ノードにメッシュが存在する時
	if (node->mNumMeshes > 0)
	{
		// 平行移動を無効化
		Matrix4 meshMatrix = FromAssimpMatrix(rootTransform);
		Matrix4 meshMatrixNoPos = meshMatrix;
		meshMatrixNoPos.matrix(0, 3) = 0.0f;
		meshMatrixNoPos.matrix(1, 3) = 0.0f;
		meshMatrixNoPos.matrix(2, 3) = 0.0f;

		// メッシュノード追加
		MeshNode<VTX_MESH> & addMeshNode = mesh->meshNode[meshCnt];
		meshCnt++;

		// 頂点の数を割り出す
		unsigned int pointNum = 0;
		for (unsigned int n = 0; n < node->mNumMeshes; n++)
		{
			const aiMesh * ai_mesh = scene->mMeshes[node->mMeshes[n]];
			pointNum += ai_mesh->mNumFaces;
		}
		// 既に三角ポリゴンにしているので3倍
		UINT vertexNum = pointNum * 3;

		// 頂点情報
		std::vector<VTX_MESH> addVertex;
		addVertex.resize(vertexNum);
		// インデックス情報
		std::vector<unsigned short> addIndex;
		addIndex.resize(vertexNum);

		UINT vertexCount = 0;

		for (unsigned int n = 0; n < node->mNumMeshes; n++)
		{
			// サブセット追加
			addMeshNode.subsetArray.emplace_back();
			Subset & addSubset = addMeshNode.subsetArray.back();

			const aiMesh * aiMesh = scene->mMeshes[node->mMeshes[n]];
			const aiMaterial * material = scene->mMaterials[aiMesh->mMaterialIndex];

			// マテリアルを読み込み
			LoadMaterial(material, &addSubset.material);

			addSubset.startIndex = vertexCount;

			for (unsigned int t = 0; t < aiMesh->mNumFaces; t++)
			{
				const aiFace* face = &aiMesh->mFaces[t];
				for (unsigned int i = 0; i < face->mNumIndices; i++)
				{
					// 面の頂点番号取得
					int index = face->mIndices[i];

					// 法線
					if (aiMesh->mNormals != nullptr)
					{
						// 正規化してから格納
						addVertex[vertexCount].nor = Vector3(aiMesh->mNormals[index].x, aiMesh->mNormals[index].y, aiMesh->mNormals[index].z).Normalize();
						// 姿勢を反映
						addVertex[vertexCount].nor = meshMatrixNoPos * addVertex[vertexCount].nor;
					}
					// テクスチャのUVセット
					if (aiMesh->HasTextureCoords(0))
					{
						addVertex[vertexCount].uv = Vector2(aiMesh->mTextureCoords[0][index].x, aiMesh->mTextureCoords[0][index].y);
					}

					// 頂点座標
					addVertex[vertexCount].pos = Vector3(aiMesh->mVertices[index].x, aiMesh->mVertices[index].y, aiMesh->mVertices[index].z);
					// 姿勢を反映
					addVertex[vertexCount].pos = meshMatrix * addVertex[vertexCount].pos;
				
					// インデックスバッファに割り当て
					addIndex[vertexCount] = vertexCount;
					vertexCount += 1;
				}

				// 接空間、従法線計算
				VTX_MESH & v1 = addVertex[vertexCount - 3];
				VTX_MESH & v2 = addVertex[vertexCount - 2];
				VTX_MESH & v3 = addVertex[vertexCount - 1];

				CalcTangent(v1.tan, v1.binor, v1.pos, v1.uv, v1.nor,
							v2.tan, v2.binor, v2.pos, v2.uv, v2.nor,
							v3.tan, v3.binor, v3.pos, v3.uv, v3.nor);
			}
			addSubset.indexNum = vertexCount - addSubset.startIndex;
		}
		
		// 頂点バッファ生成
		addMeshNode.meshData.CreateVertexBuffer(&addVertex);
		// インデックスバッファ生成
		addMeshNode.meshData.CreateIndexBuffer(&addIndex);
	}

	// 子ノードへ
	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		LoadMeshNode(mesh, scene, node->mChildren[n], rootTransform);
	}
}

void AssimpLoader::LoadSkinMeshNode(SkinMesh * skinMesh, const aiScene * scene, aiNode * node, const aiMatrix4x4 & parentTransform)
{
	aiMatrix4x4 rootTransform = parentTransform * node->mTransformation;

	// ノードにメッシュが存在する時
	if (node->mNumMeshes > 0)
	{
		// 平行移動を無効化
		XMMATRIX4 meshMatrix = FromAssimpMatrix(rootTransform);
		meshMatrix.matrix.r[3].m128_f32[0] = 0.0f;
		meshMatrix.matrix.r[3].m128_f32[1] = 0.0f;
		meshMatrix.matrix.r[3].m128_f32[2] = 0.0f;

		// メッシュ情報を追加
		MeshNode<VTX_SKIN_MESH> & addSkinMeshNode = skinMesh->meshNode[meshCnt];
		meshCnt++;
		std::vector<int> offsetMeshVertexID;

		// 頂点の数を割り出す
		unsigned int pointNum = 0;
		for (unsigned int n = 0; n < node->mNumMeshes; n++)
		{
			offsetMeshVertexID.push_back(pointNum);

			const aiMesh * mesh = scene->mMeshes[node->mMeshes[n]];
			pointNum += mesh->mNumFaces * 3;
		}
		// 既に三角ポリゴンにしているので3倍
		UINT vertexNum = pointNum;

		// 頂点情報
		std::vector<VTX_SKIN_MESH> addVertex;
		addVertex.resize(vertexNum);
		// インデックス情報
		std::vector<unsigned short> addIndex;
		addIndex.resize(vertexNum);

		UINT vertexCount = 0;
		for (unsigned int n = 0; n < node->mNumMeshes; n++)
		{
			// サブセット追加
			addSkinMeshNode.subsetArray.emplace_back();
			Subset & addSubset = addSkinMeshNode.subsetArray.back();

			const aiMesh * mesh = scene->mMeshes[node->mMeshes[n]];
			const aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
			// ボーンウェイトデータ
			std::vector<std::vector<TmpWeight>> weightList(mesh->mNumVertices);
			LoadBoneWeightList(mesh, &weightList);
			// リストを正規化
			WeightListNormalize(weightList);

			// マテリアルを読み込み
			LoadMaterial(material, &addSubset.material);

			addSubset.startIndex = vertexCount;

			for (unsigned int t = 0; t < mesh->mNumFaces; t++)
			{
				const aiFace* face = &mesh->mFaces[t];
				
				for (unsigned int i = 0; i < face->mNumIndices; i++)
				{
					// 面の頂点番号取得
					int index = face->mIndices[i];

					// 法線
					if (mesh->mNormals != nullptr)
					{
						// 正規化してから格納
						addVertex[vertexCount].nor = Vector3(mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z).Normalize();
					}
					// テクスチャのUVセット
					if (mesh->HasTextureCoords(0))
					{
						addVertex[vertexCount].uv = Vector2(mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y);
					}

					// ボーンの情報
					for (UINT w = 0;w < weightList[index].size();w++)
					{
						addVertex[vertexCount].idx[w] = weightList[index][w].first;
						addVertex[vertexCount].weight[w] = weightList[index][w].second;
					}

					// 頂点座標
					addVertex[vertexCount].pos = Vector3(mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z);

					// インデックスバッファに割り当て
					addIndex[vertexCount] = vertexCount;
					vertexCount += 1;
				}

				// 接空間、従法線計算
				VTX_SKIN_MESH & v1 = addVertex[vertexCount - 3];
				VTX_SKIN_MESH & v2 = addVertex[vertexCount - 2];
				VTX_SKIN_MESH & v3 = addVertex[vertexCount - 1];

				CalcTangent(v1.tan, v1.binor, v1.pos, v1.uv, v1.nor,
					v2.tan, v2.binor, v2.pos, v2.uv, v2.nor,
					v3.tan, v3.binor, v3.pos, v3.uv, v3.nor);

			}
			addSubset.indexNum = vertexCount - addSubset.startIndex;
		}

		// 頂点バッファ生成
		addSkinMeshNode.meshData.CreateVertexBuffer(&addVertex);
		// インデックスバッファ生成
		addSkinMeshNode.meshData.CreateIndexBuffer(&addIndex);
	}

	// 子ノードへ
	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		LoadSkinMeshNode(skinMesh, scene, node->mChildren[n], rootTransform);
	}
}

void AssimpLoader::LoadMaterial(const aiMaterial * aiMat, Material * mat)
{
	aiString name;
	aiMat->Get(AI_MATKEY_NAME, name);
	mat->name = name.C_Str();

	// マテリアルパラメータ読み込み
	float alpha;
	aiMat->Get(AI_MATKEY_OPACITY, alpha);
	aiColor3D color;
	aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	mat->materialParam.diffuse = Color(color.r, color.g, color.b, 1.0f);
	aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	mat->materialParam.ambient = Color(color.r, color.g, color.b, 1.0f);
	aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
	mat->materialParam.emissive = Color(color.r, color.g, color.b, 1.0f);
	aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	mat->materialParam.specular = Color(color.r, color.g, color.b, 1.0f);

	float value = 0.0f;
	aiMat->Get(AI_MATKEY_SHININESS, value);
	mat->materialParam.shininess = value;
	//mat->materialParam.reflection = 0.0f;
	//mat->materialParam.height = 0.0f;

	/*
	{
		aiString path;
		if (scnen->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			// テクスチャが埋め込みか
			if (path[0] == '*')
			{
				int id = atoi(&path.data[0]);

			}
	}*/

	// テクスチャ読み込み
	auto LoadTexture = [](aiString texureName) -> std::weak_ptr<Texture>
	{
		std::string fileName = texureName.C_Str();

		size_t pos = fileName.find_last_of("\\/");
		std::string texName;
		(pos != std::string::npos) ?	// ファイルパスが含まれていたら取り除く
			texName = fileName.substr(pos + 1, fileName.size() - pos) :
			texName = fileName;

		return Singleton<TextureManager>::Instance()->Load(filePath + texName);
	};

	aiString tex_name;
	if (aiMat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), tex_name) == AI_SUCCESS)
	{
		mat->pTexture = LoadTexture(tex_name);
	}
	if (aiMat->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), tex_name) == AI_SUCCESS)
	{
		mat->pNormalTexture = LoadTexture(tex_name);
	}
	if (aiMat->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), tex_name) == AI_SUCCESS)
	{
		mat->pHeightTexture = LoadTexture(tex_name);
	}
}

XMFLOAT4 AssimpLoader::CalcTangent(Vector3 v1, Vector3 v2, Vector3 v3, Vector2 uv1, Vector2 uv2, Vector2 uv3, Vector3 normal)
{
	Vector3 tangent;
	Vector3 bitangent;

	Vector3 edge1 = v2 - v1;
	Vector3 edge2 = v3 - v1;
	edge1.Normalize();
	edge2.Normalize();
	Vector2 uvEdge1 = uv2 - uv1;
	Vector2 uvEdge2 = uv3 - uv1;
	uvEdge1.Normalize();
	uvEdge2.Normalize();

	float det = (uvEdge1.x*uvEdge2.y) - (uvEdge1.y*uvEdge2.x);
	det = 1.0f / det;

	tangent.x = (uvEdge2.y*edge1.x - uvEdge1.y*edge2.x)*det;
	tangent.y = (uvEdge2.y*edge1.y - uvEdge1.y*edge2.y)*det;
	tangent.z = (uvEdge2.y*edge1.z - uvEdge1.y*edge2.z)*det;

	bitangent.x = (-uvEdge2.x*edge1.x + uvEdge1.x*edge2.x)*det;
	bitangent.y = (-uvEdge2.x*edge1.y + uvEdge1.x*edge2.y)*det;
	bitangent.z = (-uvEdge2.x*edge1.z + uvEdge1.x*edge2.z)*det;

	normal.Normalize();
	tangent.Normalize();
	bitangent.Normalize();

	Vector3 BiNormal;
	BiNormal = Vector3::Cross(normal, tangent);

	float w = (Vector3::Dot(BiNormal, bitangent) >= 0.0f) ? 1.0f : -1.0f;

	return XMFLOAT4(tangent.x, tangent.y, tangent.z, w);
}

void AssimpLoader::CalcTangent(Vector3 & tangent0, Vector3 & binormal0, Vector3 const & p0, Vector2 const & uv0, Vector3 const & normal0,
								   Vector3 & tangent1, Vector3 & binormal1, Vector3 const & p1, Vector2 const & uv1, Vector3 const & normal1,
								   Vector3 & tangent2, Vector3 & binormal2, Vector3 const & p2, Vector2 const & uv2, Vector3 const & normal2)
{
	auto const dp1 = p1 - p0;
	auto const dp2 = p2 - p0;

	auto const duv1 = uv1 - uv0;
	auto const duv2 = uv2 - uv0;
	//接線と従接線を計算
	auto const binor = (dp2 * duv1.x - dp1 * duv2.x);
	//auto const binor = (dp2 * duv1.y - dp1 * duv2.y);

	tangent0 = Vector3::Cross(normal0, binor).Normalized();
	tangent1 = Vector3::Cross(normal1, binor).Normalized();
	tangent2 = Vector3::Cross(normal2, binor).Normalized();
	binormal0 = Vector3::Cross(tangent0, normal0).Normalized();
	binormal1 = Vector3::Cross(tangent1, normal1).Normalized();
	binormal2 = Vector3::Cross(tangent2, normal2).Normalized();
}

void AssimpLoader::SearchNodeBone(const aiScene * scene, const aiNode * node, std::vector<BoneData> & bones)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		for (UINT b = 0; b < mesh->mNumBones; b++)
		{
			aiBone* bone = mesh->mBones[b];
			bool hitBone = false;
			for (auto boneData : bonesData)
			{
				std::string tName = bone->mName.C_Str();
				// 既に格納されているなら入れない
				if (boneData.name == tName)
				{
					hitBone = true;
					break;
				}
			}
			// 既にある場合は中身の情報に追加情報があれば追加
			if (!hitBone)
			{
				// ボーンデータを配列に保持しておく
				BoneData boneData;
				boneData.name = bone->mName.C_Str();
				const aiNode * boneNode = scene->mRootNode->FindNode(bone->mName);
				boneData.offsetMatrix = FromAssimpMatrix(bone->mOffsetMatrix);
				bones.emplace_back(boneData);
			}
		}
	}
	for (UINT c = 0; c < node->mNumChildren; c++)
	{
		SearchNodeBone(scene, node->mChildren[c], bones);
	}
}

BoneData * AssimpLoader::GetBoneName(std::string name, std::vector<BoneData> & bones)
{
	for (UINT i = 0; i < bones.size();i++)
	{
		if (bones[i].name == name)
			return &bones[i];
	}
	return nullptr;
}

UINT AssimpLoader::GetBoneIndex(std::string name, std::vector<BoneData> & bones)
{
	for (UINT i = 0; i < bones.size(); i++)
	{
		if (bones[i].name == name)
			return i;
	}

	return 0;
}

void AssimpLoader::ProcessSkeletonHeirarchyre(const aiNode * node, std::vector<BoneData> & boneDataArray, aiMatrix4x4 & parentMatrix, int num, int index, int parentindex, std::vector<int> * hierarchy)
{
	aiMatrix4x4 rootMatrix = parentMatrix * node->mTransformation;
	Joint joint;

	std::string nodeName = node->mName.C_Str();
	joint.name = nodeName;

	// ボーンデータを階層ごとに並ぶように格納
	BoneData * boneData = GetBoneName(nodeName, boneDataArray);
	if (boneData)
		bonesData.emplace_back(*boneData);
	
	if (hierarchy->size() == 0)
	{
		joint.parentIndex = -1;
	}
	else
	{
		// 自分の親に自身の Id を登録
		joint.parentIndex = parentindex;
		skeleton.mJoints[joint.parentIndex].childsId.emplace_back(index);
	}

	// 現在の階層番号を格納
	hierarchy->emplace_back(num);
	joint.hierarchy = *hierarchy;

	// ローカル初期姿勢
	joint.initLocalMatrix = FromAssimpMatrix(node->mTransformation);

	skeleton.mJoints.emplace_back(joint);

	for (UINT n = 0; n < node->mNumChildren; n++)
	{
		ProcessSkeletonHeirarchyre(node->mChildren[n], boneDataArray, rootMatrix, n, (int)skeleton.mJoints.size(), index, hierarchy);
		hierarchy->pop_back();
	}
}

void AssimpLoader::WeightListNormalize(std::vector<std::vector<TmpWeight>>& tmpBoneWeightList)
{
	// ウェイト情報の正規化
	for (auto& tmpBoneWeight : tmpBoneWeightList)
	{
		// ウェイトの大きさでソート
		std::sort(tmpBoneWeight.begin(), tmpBoneWeight.end(),
			[](const TmpWeight& weightA, const TmpWeight& weightB) { return weightA.second > weightB.second; }
		);
		// 1頂点に4つより多くウェイトが割り振られているなら影響が少ないものは無視する
		while (tmpBoneWeight.size() > 4)
		{
			tmpBoneWeight.pop_back();
		}
		// 4つに満たない場合はダミーを挿入
		while (tmpBoneWeight.size() < 4)
		{
			tmpBoneWeight.push_back({ 0, 0.0f });
		}
		float total = 0.0f;
		for (int i = 0; i < 4; ++i)
		{
			total += tmpBoneWeight[i].second;
		}
		// ウェイトの正規化
		for (int i = 0; i < 4; ++i)
		{
			tmpBoneWeight[i].second /= total;
		}
	}
}

void AssimpLoader::LoadBoneWeightList(const aiMesh * mesh, std::vector<std::vector<TmpWeight>>* tmpBoneWeightList)
{
	for (UINT b = 0; b < mesh->mNumBones; b++)
	{
		aiBone * bone = mesh->mBones[b];
		std::string boneName(bone->mName.data);

		UINT boneIndex = GetBoneIndex(boneName, bonesData);

		for (UINT w = 0; w < bone->mNumWeights; w++)
		{
			TmpWeight weight;
			UINT vertexID = bone->mWeights[w].mVertexId;
			weight.first = boneIndex;
			weight.second = bone->mWeights[w].mWeight;
			(*tmpBoneWeightList)[vertexID].emplace_back(weight);
		}
	}
}

void AssimpLoader::LoadAnimation(const aiScene * scene)
{
	for (UINT a = 0; a < scene->mNumAnimations; a++)
	{
		aiAnimation * anim = scene->mAnimations[a];
		
		// アニメーション名格納
		AnimInfo animInfo;
		std::string animName = anim->mName.C_Str();
		size_t pos = animName.find_last_of("|");

		if (std::string::npos == pos)
		{
			MessageBox(NULL, "アニメーションデータがスケルトンに登録されてない！", "", MB_OK);
			//continue;
			pos = animName.find_last_of(":");
		}

		animName = animName.substr(pos + 1, animName.size() - pos);
		animName = modelName + std::string("|") + animName;

		animInfo.name = animName;
		animInfo.maxFrame = (UINT)anim->mDuration;
		skeleton.animInfo.emplace_back(animInfo);

		for (UINT k = 0; k < animInfo.maxFrame; k++)
		{
			LoadNodeAnim(anim, (UINT)skeleton.animInfo.size() - 1, k, scene->mRootNode);
		}
	}
}

void AssimpLoader::LoadNodeAnim(
	const aiAnimation * anim,
	const UINT takeNum,
	const UINT keyFrame,
	const aiNode * node)
{
	const aiNodeAnim* nodeAnim = FindNodeAnim(anim, node->mName);
	
	if (nodeAnim)
	{
		Joint * joint = skeleton.GetJoint(node->mName.C_Str());

		UINT maxFrame = 0;
		float time1 = (float)nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1].mTime;
		float time2 = (float)nodeAnim->mScalingKeys[nodeAnim->mNumScalingKeys - 1].mTime;
		float time3 = (float)nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1].mTime;
		if (time1 > maxFrame) maxFrame = (UINT)time1;
		if (time2 > maxFrame) maxFrame = (UINT)time2;
		if (time3 > maxFrame) maxFrame = (UINT)time3;

		// 最大キーフレームが超えていたら前のキーフレームの状態を使う
		if (keyFrame < maxFrame + 1)
		{
			// スケーリング変換行列を生成する
			aiVector3D scale = FindScale((float)keyFrame, nodeAnim);

			// 回転変換行列を生成する
			aiQuaternion rotation = FindRotation((float)keyFrame, nodeAnim);

			// 移動変換行列を生成する
			aiVector3D position = FindPosition((float)keyFrame, nodeAnim);

			// これら上記の変換を合成する
			Translation3f translation(position.x, position.y, position.z);
			Scaling3f scaling(scale.x, scale.y, scale.z);
			Quaternion rotate(rotation.x, rotation.y, rotation.z, rotation.w);
			Eigen::Affine3f affine = translation * scaling * rotate.quaternion;
			// このキーフレームでの姿勢を格納
			Matrix4 nodeMatrix; 
			nodeMatrix = affine.matrix();

			Keyframe key;
			key.frameNum = keyFrame;
			key.localMatrix = nodeMatrix;

			// 足りないアニメーションデータを追加
			for (size_t i = joint->animations.size(); i < takeNum + 1; i++)
			{
				joint->animations.emplace_back(Anim());
			}
		
			joint->animations[takeNum].keyFrames.emplace_back(key);
		}
	}

	for (UINT c = 0; c < node->mNumChildren; c++)
	{
		LoadNodeAnim(anim, takeNum, keyFrame, node->mChildren[c]);
	}
}

aiNodeAnim * AssimpLoader::FindNodeAnim(const aiAnimation * anim, aiString nodeName)
{
	for (UINT c = 0; c < anim->mNumChannels; c++)
	{
		if (anim->mChannels[c]->mNodeName == nodeName)
		{
			return anim->mChannels[c];
		}
	}

	return nullptr;
}

aiQuaternion AssimpLoader::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	// 補間には最低でも２つの値が必要…
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		return pNodeAnim->mRotationKeys[0].mValue;
	}

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {

			aiQuaternion out;

			UINT RotationIndex = i;
			UINT NextRotationIndex = (RotationIndex + 1);
			assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
			float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
			float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
			assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
			const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
			aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
			out = out.Normalize();

			return out;
		}
	}

	if (AnimationTime >= (float)pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys - 1].mTime)
	{
		return pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys - 1].mValue;
	}

	assert(0);

	return aiQuaternion();
}

aiVector3D AssimpLoader::FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	// 補間には最低でも２つの値が必要…
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		return pNodeAnim->mScalingKeys[0].mValue;
	}

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {

			UINT ScaleIndex = i;
			UINT NextScaleIndex = (ScaleIndex + 1);
			assert(NextScaleIndex < pNodeAnim->mNumScalingKeys);
			float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScaleIndex].mTime - pNodeAnim->mScalingKeys[ScaleIndex].mTime);
			float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScaleIndex].mTime) / DeltaTime;
			assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiVector3D& StartScaleQ = pNodeAnim->mScalingKeys[ScaleIndex].mValue;
			const aiVector3D& EndScaleQ = pNodeAnim->mScalingKeys[NextScaleIndex].mValue;

			return StartScaleQ * (1.0f - Factor) + EndScaleQ * Factor;
		}
	}

	if (AnimationTime >= (float)pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mTime)
	{
		return pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mValue;
	}

	assert(0);

	return aiVector3D();
}

aiVector3D AssimpLoader::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumPositionKeys > 0);

	// 補間には最低でも２つの値が必要…
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		return pNodeAnim->mPositionKeys[0].mValue;
	}

	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {

			UINT PositionIndex = i;
			UINT NextPositionIndex = (PositionIndex + 1);
			assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
			float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
			float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
			assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiVector3D& StartPositionQ = pNodeAnim->mPositionKeys[PositionIndex].mValue;
			const aiVector3D& EndPositionQ = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;

			return StartPositionQ * (1.0f - Factor) + EndPositionQ * Factor;
		}
	}

	if (AnimationTime >= (float)pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mTime)
	{
		return pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mValue;
	}

	assert(0);

	return aiVector3D();
}


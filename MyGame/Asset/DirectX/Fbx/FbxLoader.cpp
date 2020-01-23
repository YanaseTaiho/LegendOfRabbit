//==================================================================================================
// �C���N���[�h
//==================================================================================================
#include "FbxLoader.h"
#include "../Shader/SkinMesh/SkinMeshShader.h"
#include "MtxMath.h"
#include "FrameWork/debugPrintf.h"

using namespace MyDirectX;
using namespace MyDirectX::Fbx;

std::string FbxLoader::filePath;

std::vector<XMFLOAT3> FbxLoader::positionArray;
std::vector<XMFLOAT3> FbxLoader::normalArray;
std::vector<XMFLOAT2> FbxLoader::uvArray;
std::vector<XMFLOAT3> FbxLoader::tangentArray;
std::vector<XMFLOAT3> FbxLoader::binormalArray;
std::vector<std::vector<TmpWeight>> FbxLoader::tmpBoneWeightList;

Skeleton FbxLoader::skeleton;


// �f�X�g���N�^-------------------------------------------------------------------------------------
FbxLoader ::~FbxLoader()
{
 
}

// ������-------------------------------------------------------------------------------------------
SkinMesh * FbxLoader::LoadSkinMesh(std::string fileName)
{
	size_t pos = fileName.find_last_of("\\/");
	std::string basePath = fileName.substr(0, pos + 1);
	filePath = basePath;

	FbxManager * pFbxManager = FbxManager::Create();
	FbxScene * pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
	FbxString FileName(fileName.c_str());
	FbxImporter *fbxImporter = FbxImporter::Create(pFbxManager, "imp");
	fbxImporter->Initialize(FileName.Buffer(), -1, pFbxManager->GetIOSettings());
	fbxImporter->Import(pFbxScene);

	FbxGeometryConverter geometryConverter(pFbxManager);
	// ���炩���߃|���S����S�ĎO�p�`�����Ă���
	geometryConverter.Triangulate(pFbxScene, true);
	geometryConverter.RemoveBadPolygonsFromMeshes(pFbxScene);

	/*FbxAxisSystem axisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem targetSystem(FbxAxisSystem::DirectX);
	if (axisSystem != targetSystem)
	{
		targetSystem.ConvertScene(pFbxScene);
	}*/
	

	// �}�e���A�����ƂɃ��b�V���𕪗�
//	geometryConverter.SplitMeshesPerMaterial(m_pFbxScene, true);

	fbxImporter->Destroy();

    XMMATRIX rootMtx = XMMatrixIdentity();
	
	SkinMesh * mesh = new SkinMesh();
	// �{�[���̊K�w��T��
	std::vector<int> hierarchy;
	ProcessSkeletonHeirarchyre(pFbxScene->GetRootNode(), 0, 0, 0, -1, &hierarchy);
	// ���f���f�[�^�ǂݍ���
    LoadData(pFbxScene->GetRootNode(), mesh, &rootMtx );

	mesh->skeleton = std::shared_ptr<Skeleton>(new Skeleton(skeleton));

	std::vector<AnimInfo>().swap(skeleton.animInfo);
	std::vector<Joint>().swap(skeleton.mJoints);
	pFbxScene->Destroy();
	pFbxManager->Destroy();

	return mesh;
}

// ���_�f�[�^�̎��o��-----------------------------------------------------------------------------
void FbxLoader::LoadData( FbxNode *pParentNode, SkinMesh *pSkinMesh, const XMMATRIX *parentMtx )
{
    for (int i = 0; i < pParentNode->GetChildCount(); i++)
	{
        FbxNode *pNode = pParentNode->GetChild( i );

        XMMATRIX preMtx = GetPreMtx( pNode );
        XMMATRIX lclMtx = GetLclMtx( pNode, &preMtx );
		XMMATRIX nextMtx = lclMtx * ( *parentMtx );

        if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
			FbxMesh* pMesh = pNode->GetMesh();
			SkinMesh::MeshNode* pAddMeshNode = new SkinMesh::MeshNode(new SkinMeshShader());

			// �S�Ẵ}�e���A���ǂݍ���
			LoadMaterial(pNode, pAddMeshNode);
        
            int layerNum = pMesh->GetLayerCount();

            for (int j = 0; j < layerNum; ++j)
            {
                FbxLayer* pLayer = pMesh->GetLayer(j);
                LoadVtxPos(pMesh, &nextMtx );
                LoadVtxUv(pMesh, pLayer);

                XMMATRIX notTransformMtx = nextMtx;
                notTransformMtx.r[3].m128_f32[0] = 0;
                notTransformMtx.r[3].m128_f32[1] = 0;
                notTransformMtx.r[3].m128_f32[2] = 0;

                LoadVtxNormal(pMesh, pLayer, &notTransformMtx);
                LoadVtxTangent(pMesh, pLayer, &notTransformMtx);
                LoadVtxBinormal(pMesh, pLayer, &notTransformMtx);

				// �{�[�����ǂݍ���
	            LoadBone( pMesh, pAddMeshNode, &nextMtx );
            }

			// �|���S���̐�
			const int polygonNum = pMesh->GetPolygonCount();
			// �C���f�b�N�X�o�b�t�@�̗v�f�����擾( �O�p�|���S���ɕ������������Ă��� )
			int indexesCount = polygonNum * 3;

			// ���_�o�b�t�@�쐬
			int vertexesCount = pMesh->GetPolygonVertexCount();
			SkinMeshPolygon *pPolygon = pAddMeshNode->shader->pPolygon.get();
			pPolygon->Initialize(vertexesCount);
			// �C���f�b�N�X�z�񐶐�
			pAddMeshNode->indexArray = new unsigned short[indexesCount];

			// �}�e���A�����ɕ`�悷�邽�߂̐ݒ�
			FbxLayerElementMaterial* material = pMesh->GetLayer(0)->GetMaterials();//���C���[��1��������z��
			int materialIndexCount = material->GetIndexArray().GetCount();

			// �}�e���A�����������邩�Ȃ���
			if (pAddMeshNode->subset.size() > 1)
			{
				for (int i = 0; i < pAddMeshNode->subset.size(); i++)
				{
					pAddMeshNode->subset[i]->startIndex = (i == 0) ?
						0 :
						pAddMeshNode->subset[i - 1]->indexNum + pAddMeshNode->subset[i - 1]->startIndex;

					// �Y������}�e���A����T��
					for (int m = 0; m < materialIndexCount; m++)
					{
						int materialId = material->GetIndexArray().GetAt(m);

						if (materialId == i)
						{
							pAddMeshNode->subset[i]->indexNum += 3;
						}
					}
				}
			}
			else
			{
				// ��̏ꍇ
				pAddMeshNode->subset[0]->startIndex = 0;
				pAddMeshNode->subset[0]->indexNum = indexesCount;
			}

			// �C���f�b�N�X�o�b�t�@���Z�b�g
			{
				UINT Index = 0;
				int count = 0;

				for (int m = 0; m < pAddMeshNode->subset.size(); m++)
				{
					for (int i = 0; i < polygonNum; i++)
					{
						// �}�e���A���ԍ��ɂ����Ē��_��񊄂蓖��
						int materialId = material->GetIndexArray().GetAt(i);
						// �}�e���A���ԍ��ƈ�v������i�[
						if (materialId == m)
						{
							// 1�|���S�����̒��_�����擾
							int lPolygonSize = pMesh->GetPolygonSize(i);
							// �O�p�|���S��
							for (int j = 0; j < lPolygonSize; j++)
							{
								int polygonIndex = pMesh->GetPolygonVertex(i, j);

								pPolygon->m_pVtx[count].pos = positionArray[(i * 3) + j];
								pPolygon->m_pVtx[count].nor = normalArray[(i * 3) + j];
								pPolygon->m_pVtx[count].uv = uvArray[(i * 3) + j];
								pPolygon->m_pVtx[count].tan = tangentArray[(i * 3) + j];
								pPolygon->m_pVtx[count].biNor = binormalArray[(i * 3) + j];

								//DebugPrintf("%d\n", polygonIndex);

								for (int w = 0; w < 4; w++)
								{
									pPolygon->m_pVtx[count].idx[w] = tmpBoneWeightList[polygonIndex][w].first;
									pPolygon->m_pVtx[count].weight[w] = tmpBoneWeightList[polygonIndex][w].second;
								}

								// �R���g���[���|�C���g�̃C���f�b�N�X���擾
								pAddMeshNode->indexArray[Index] = count;

								count++;
								Index++;
							}
						}
					}
				}
			}
			// �C���f�b�N�X�o�b�t�@����
			pPolygon->SetIndex(pAddMeshNode->indexArray, indexesCount);

			// �q�ɒǉ����čċN
			pSkinMesh->meshNode.emplace_back(std::shared_ptr<SkinMesh::MeshNode>(pAddMeshNode));

			// �v�f�̉��
			std::vector<XMFLOAT3>().swap(positionArray);
			std::vector<XMFLOAT3>().swap(normalArray);
			std::vector<XMFLOAT2>().swap(uvArray);
			std::vector<XMFLOAT3>().swap(tangentArray);
			std::vector<XMFLOAT3>().swap(binormalArray);

			for (auto a : tmpBoneWeightList)
			{
				std::vector<TmpWeight>().swap(a);
			}

			std::vector<std::vector<TmpWeight>>().swap(tmpBoneWeightList);
        }
		// �ċN�֐�
        LoadData(pNode, pSkinMesh, &nextMtx );
    }
}

// ���[�J���ϊ��s��̎擾---------------------------------------------------------------------------
XMMATRIX FbxLoader::GetLclMtx(FbxNode *pNode, XMMATRIX *preMtx)
{
    XMMATRIX mtx;
    FbxDouble3 fbxScale = pNode->LclScaling;
    FbxDouble3 fbxRota = pNode->LclRotation;
    FbxDouble3 fbxPos = pNode->LclTranslation;
    XMMATRIX scale = XMMatrixScaling((float)fbxScale[0], (float)fbxScale[1], (float)fbxScale[2]);
    XMMATRIX rota = 
        XMMatrixRotationX(XMConvertToRadians((float)fbxRota[0])) *
        XMMatrixRotationY(XMConvertToRadians((float)fbxRota[1])) *
        XMMatrixRotationZ(XMConvertToRadians((float)fbxRota[2]));
    XMMATRIX pos = XMMatrixTranslation((float)fbxPos[0], (float)fbxPos[1], (float)fbxPos[2]);

    return mtx = scale * rota * (*preMtx) * pos;
}

// �v���r���[�ϊ��s��̎擾-------------------------------------------------------------------------
XMMATRIX FbxLoader ::GetPreMtx( FbxNode *pNode )
{
    XMMATRIX mtx;
    FbxDouble3 fbxPreRota = pNode->PreRotation;
    XMMATRIX preRota =
        XMMatrixRotationX(XMConvertToRadians((float)fbxPreRota[0])) *
        XMMatrixRotationY(XMConvertToRadians((float)fbxPreRota[1])) *
        XMMatrixRotationZ(XMConvertToRadians((float)fbxPreRota[2]));

    return mtx = preRota;
}

// �e�N�X�`���f�[�^�̎��o��-----------------------------------------------------------------------
void FbxLoader::LoadMaterial(FbxNode *pNode, SkinMesh::MeshNode * pSkinMeshNode)
{
	FbxMesh * pMesh = pNode->GetMesh();

	/*int layerCount = pMesh->GetLayerCount();
	
	for (int l = 0; l < layerCount; l++)
	{
		FbxLayerElementMaterial * pLayerMaterial = pMesh->GetLayer(l)->GetMaterials();
		FbxLayerElementArrayTemplate <int> * tmpArray = &pLayerMaterial->GetIndexArray();
		int materialIndexCount = tmpArray->GetCount();
		for (int i = 0; i < materialIndexCount; i++)
		{
			int index = tmpArray->GetAt(i);
			DebugPrintf(" %d \n", index);
		}
	}*/

	int materialNum = pNode->GetMaterialCount();
	// �}�e���A���̐������T�u�Z�b�g����
	for (int i = 0; i < materialNum; i++)
	{
		FbxSurfaceMaterial *pMaterial = NULL;
		pMaterial = pNode->GetMaterial(i);
		if (pMaterial == NULL) continue;

		Subset * sub = new Subset(pSkinMeshNode->shader);	// �R���X�g���N�^�Ń}�e���A���쐬

		//	�}�e���A���̐ݒ�
		SetMaterial(sub->material.get(), pMaterial);
		// �m�[�h�ɃT�u�Z�b�g�ǉ�
		pSkinMeshNode->subset.emplace_back(std::shared_ptr<Subset>(sub));
	}
}

void FbxLoader::SetMaterial(Material * material, FbxSurfaceMaterial * fbxMaterial)
{
	material->name = fbxMaterial->GetName();

	FbxProperty prop;
	//���e�N�X�`���t�@�C���p�X�@�����ƃ��f����R�Â��邱�ƂŐ؂�ւ��ł��邩��
	std::string strTextureFilePath;
	std::string strNormalTextureFilePath;

	FbxDouble3 ambient;
	FbxDouble3 diffuse;
	FbxDouble3 emissive;
	FbxDouble3 bump;

	FbxDouble ambientFactor;
	FbxDouble diffuseFactor;
	FbxDouble emissiveFactor;
	FbxDouble bumpFactor;

	FbxDouble3 specular;
	FbxDouble shininess;
	FbxDouble reflection;
	FbxDouble specularFactor;

	//�A���r�G���g
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (prop.IsValid()) {
		ambient = prop.Get<FbxDouble3>();
	}
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbientFactor);
	if (prop.IsValid()) {
		ambientFactor = prop.Get<FbxDouble>();
	}

	//�f�B�t���[�Y
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (prop.IsValid()) {
		diffuse = prop.Get<FbxDouble3>();
	}
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	if (prop.IsValid()) {
		diffuseFactor = prop.Get<FbxDouble>();
	}

	//�G�~�b�V�u
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
	if (prop.IsValid()) {
		emissive = prop.Get<FbxDouble3>();
	}
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);
	if (prop.IsValid()) {
		emissiveFactor = prop.Get<FbxDouble>();
	}

	//�o���v
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sBump);
	if (prop.IsValid()) {
		bump = prop.Get<FbxDouble3>();
	}
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sBumpFactor);
	if (prop.IsValid()) {
		bumpFactor = prop.Get<FbxDouble>();
	}

	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	int LayeredTexCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

	if (LayeredTexCount == 0)
	{
		//�ʏ�e�N�X�`��
		int texCount = prop.GetSrcObjectCount<FbxFileTexture>();

		if (texCount > 0)
		{
			for (int j = 0; j < texCount; j++)
			{
				//�e�N�X�`�����擾
				FbxFileTexture *pTexture = prop.GetSrcObject<FbxFileTexture>(j);

				std::string strTextureFilename = pTexture->GetRelativeFileName();
				
				// �p�X����菜���ăe�N�X�`���̖��O�������
				size_t pos = strTextureFilename.find_last_of("\\/");
				std::string texName = strTextureFilename.substr(pos + 1, strTextureFilename.size() - pos);
				
				strTextureFilePath = filePath + texName;
			}
		}
	}

	// �@���e�N�X�`��
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);

	const int LayeredNormalTexCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

	if (LayeredNormalTexCount == 0)
	{
		//�ʏ�e�N�X�`��
		int texCount = prop.GetSrcObjectCount<FbxFileTexture>();

		if (texCount > 0)
		{
			for (int j = 0; j < texCount; j++)
			{
				//�e�N�X�`�����擾
				FbxFileTexture *pTexture = prop.GetSrcObject<FbxFileTexture>(j);

				std::string strTextureFilename = pTexture->GetRelativeFileName();
				// �p�X����菜���ăe�N�X�`���̖��O�������
				size_t pos = strTextureFilename.find_last_of("\\/");
				std::string texName = strTextureFilename.substr(pos + 1, strTextureFilename.size() - pos);

				strNormalTextureFilePath = filePath + texName;
			}
		}
	}
	else
	{
		//���C���[�h�e�N�X�`��
		for (int j = 0; j < LayeredNormalTexCount; j++)
		{
			FbxLayeredTexture *pLayeredTexture = prop.GetSrcObject<FbxLayeredTexture>(j);
			int texCount = pLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

			if (texCount > 0)
			{
				for (int k = 0; k < texCount; k++) {

					//�e�N�X�`�����擾
					FbxFileTexture *pTexture = prop.GetSrcObject<FbxFileTexture>(k);

					std::string strTextureFilename = pTexture->GetRelativeFileName();
					// �p�X����菜���ăe�N�X�`���̖��O�������
					size_t pos = strTextureFilename.find_last_of("\\/");
					std::string texName = strTextureFilename.substr(pos + 1, strTextureFilename.size() - pos);

					strNormalTextureFilePath = filePath + texName;
				}
			}
		}
	}

	// �P��
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
	if (prop.IsValid())
	{
		specular = prop.Get<FbxDouble3>();
	}
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
	if (prop.IsValid()) {
		specularFactor = prop.Get<FbxDouble>();
	}

	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	if (prop.IsValid())
	{
		shininess = prop.Get<FbxDouble>();
	}

	//����
	prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
	if (prop.IsValid())
	{
		reflection = prop.Get<FbxDouble>();
	}

	// �e�N�X�`�������[�h
	if(strTextureFilePath.size() > 0)
		material->SetTexture(strTextureFilePath);
	if (strNormalTextureFilePath.size() > 0)
		material->SetNormalTexture(strNormalTextureFilePath);

	material->materialParam.ambient = Color((float)ambient[0], (float)ambient[1], (float)ambient[2], 0);
	material->materialParam.diffuse = Color((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 0);
	material->materialParam.emissive = Color((float)emissive[0], (float)emissive[1], (float)emissive[2], 0);
	material->materialParam.bump = Color((float)bump[0], (float)bump[1], (float)bump[2], 0);

	material->materialParam.ambientFactor = (float)ambientFactor;
	material->materialParam.diffuseFactor = (float)diffuseFactor;
	material->materialParam.emissiveFactor = (float)emissiveFactor;
	material->materialParam.bumpFactor = (float)bumpFactor;

	material->materialParam.specular = Color((float)specular[0], (float)specular[1], (float)specular[2], 0);
	material->materialParam.shininess = (float)shininess;
	material->materialParam.reflection = (float)reflection;

	material->materialParam.specularFactor = (float)specularFactor;
}

// ���_���W�̓ǂݍ���-------------------------------------------------------------------------------
void FbxLoader::LoadVtxPos(FbxMesh* pMesh, const XMMATRIX* pMtx)
{
	// ���_�����Z�b�g
	{
		// ���_���擾
		//int vertexesCount = pMesh->GetPolygonVertexCount();
		//pPolygon->Initialize(vertexesCount);

		//for (int i = 0; i < vertexesCount; i++)
		//{
		//	// �R���g���[���|�C���g���璸�_�����擾
		//	FbxVector4 lVec4 = pMesh->GetControlPointAt(i);
		//	// ���_�f�[�^���Z�b�g
		//	pPolygon->m_pVtx[i].pos.x = lVec4[0];
		//	pPolygon->m_pVtx[i].pos.y = lVec4[1];
		//	pPolygon->m_pVtx[i].pos.z = lVec4[2];

		//	const float ABS_X = fabsf(pPolygon->m_pVtx[i].pos.x);
		//	const float ABS_Y = fabsf(pPolygon->m_pVtx[i].pos.y);
		//	const float ABS_Z = fabsf(pPolygon->m_pVtx[i].pos.z);
		//	if (ABS_X > m_maxX)
		//	{
		//		m_maxX = ABS_X;
		//	}

		//	if (ABS_Y > m_maxY)
		//	{
		//		m_maxY = ABS_Y;
		//	}

		//	if (ABS_Z > m_maxZ)
		//	{
		//		m_maxZ = ABS_Z;
		//	}
		//}
	}

	//���_���W�ǂݍ���
	{
		/*FbxVector4* pCoord = pMesh->GetControlPoints();
		int pointCount = pMesh->GetControlPointsCount();

		for (int i = 0; i < pointCount; i++)
		{
			XMFLOAT3 pos;
			pos.x = (float)pCoord[i][0];
			pos.y = (float)pCoord[i][1];
			pos.z = (float)pCoord[i][2];

			positionArray.emplace_back(pos);
		}*/
	}

	int polygonNum = pMesh->GetPolygonCount();
	FbxVector4* pCoord = pMesh->GetControlPoints();

	for (int i = 0; i < polygonNum; i++)
	{
		// 1�|���S�����̒��_�����擾
		int lPolygonSize = pMesh->GetPolygonSize(i);
		// �O�p�|���S��
		for (int j = 0; j < lPolygonSize; j++)
		{
			int polygonIndex = pMesh->GetPolygonVertex(i, j);

			XMFLOAT3 pos;
			pos.x = (float)pCoord[polygonIndex][0];
			pos.y = (float)pCoord[polygonIndex][1];
			pos.z = (float)pCoord[polygonIndex][2];

			//DebugPrintf("position : x : %3.5f, y : %3.5f, z : %3.5f\n", pos.x, pos.y, pos.z);
			positionArray.emplace_back(pos);
		}
	}
}

// �@���̓ǂݍ���-----------------------------------------------------------------------------------
void FbxLoader::LoadVtxNormal(FbxMesh* pMesh, FbxLayer *pLayer, const XMMATRIX* pMtx)
{
	// �@��
	FbxLayerElementNormal* normalElem = pLayer->GetNormals();
	if (normalElem == nullptr)return;

	int normalNum = normalElem->GetDirectArray().GetCount();
	int indexNum = normalElem->GetIndexArray().GetCount();
	FbxLayerElement::EMappingMode	mappingMode = normalElem->GetMappingMode();
	FbxLayerElement::EReferenceMode	refMode = normalElem->GetReferenceMode();

	if (mappingMode == FbxLayerElement::eByPolygonVertex)
	{
		if (refMode == FbxLayerElement::eDirect)
		{
			// ���ڎ擾
			for (int i = 0; i < normalNum; ++i)
			{
				XMFLOAT3 nor;
				nor.x = (float)normalElem->GetDirectArray().GetAt(i)[0];
				nor.y = (float)normalElem->GetDirectArray().GetAt(i)[1];
				nor.z = (float)normalElem->GetDirectArray().GetAt(i)[2];

				XMVECTOR vec = XMLoadFloat3(&nor);
				vec = XMVector3Transform(vec, *pMtx);
				vec = XMVector3Normalize(vec);
				XMStoreFloat3(&nor, vec);

				normalArray.emplace_back(nor);
			}
		}
	}
	else if (mappingMode == FbxLayerElement::eByControlPoint)
	{
		if (refMode == FbxLayerElement::eDirect)
		{
			// ���ڎ擾
			for (int i = 0; i < normalNum; ++i)
			{
				for (int j = 0; j < 3; j++)
				{
					if (i == pMesh->GetPolygonVertex(i, j))
					{
						XMFLOAT3 nor;
						nor.x = (float)normalElem->GetDirectArray().GetAt(i)[0];
						nor.y = (float)normalElem->GetDirectArray().GetAt(i)[1];
						nor.z = (float)normalElem->GetDirectArray().GetAt(i)[2];

						XMVECTOR vec = XMLoadFloat3(&nor);
						vec = XMVector3Transform(vec, *pMtx);
						vec = XMVector3Normalize(vec);
						XMStoreFloat3(&nor, vec);

						normalArray.emplace_back(nor);
					}
				}
			}
		}
	}
}

// ����Ԃ̓ǂݍ���---------------------------------------------------------------------------------
void FbxLoader::LoadVtxTangent(FbxMesh* pMesh, FbxLayer *pLayer, const XMMATRIX* pMtx)
{
    FbxLayerElementTangent *pTangentElement = pLayer->GetTangents();
    if (pTangentElement == nullptr)
    {
        for( int i = 0; i < pMesh->GetPolygonVertexCount(); i++ )
        {
			XMFLOAT3 tan;
            tan.x = -1;
            tan.y = -1;
            tan.z = -1;
			tangentArray.emplace_back(tan);
        }
        return;
    }

    int tangentCount = pTangentElement->GetDirectArray().GetCount();

    if (pTangentElement->GetReferenceMode() == FbxLayerElement::eDirect)
    {
        for (int j = 0; j < tangentCount; j++)
        {
			XMFLOAT3 tan;
            tan.x = static_cast<float>(pTangentElement->GetDirectArray().GetAt(j)[0]);
            tan.y = static_cast<float>(pTangentElement->GetDirectArray().GetAt(j)[1]);
            tan.z = static_cast<float>(pTangentElement->GetDirectArray().GetAt(j)[2]);
			tangentArray.emplace_back(tan);
        }

        for (int j = 0; j < tangentCount; j += 3)
        {
            XMVECTOR v[3] = {
                XMLoadFloat3(&positionArray[j + 0]),
				XMLoadFloat3(&positionArray[j + 1]),
                XMLoadFloat3(&positionArray[j + 2])
            };

            XMFLOAT2 uv[3] = {
                uvArray[j + 0],
                uvArray[j + 1],
                uvArray[j + 2]
            };

            for (int i = 0; i < 3; i++)
            {
                XMVectorSetZ(v[i], XMVectorGetZ(v[i]) * -1);
            }

            XMVECTOR deltaPos1 = v[1] - v[0];
            XMVECTOR deltaPos2 = v[2] - v[0];

            XMFLOAT2 deltaUv1 = {
                uv[1].x - uv[0].x,
                uv[1].y - uv[0].y
            };

            XMFLOAT2 deltaUv2 = {
                uv[2].x - uv[0].x,
                uv[2].y - uv[0].y
            };

            float r = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
            XMVECTOR tangent = (deltaPos1 * deltaUv2.y - deltaPos2 * deltaUv1.y)*r;
            XMVECTOR binormal = (deltaPos2 * deltaUv1.x - deltaPos1 * deltaUv2.x)*r;

            tangent = XMVector3Normalize(tangent);
            for (int i = 0; i < 3; i++)
            {
				XMFLOAT3 tan;
                tan.x = XMVectorGetX(tangent);
                tan.y = XMVectorGetY(tangent);
                tan.z = XMVectorGetZ(tangent);

                XMVECTOR vec = XMLoadFloat3(&tan);
                vec = XMVector3Transform(vec, *pMtx);
                vec = XMVector3Normalize(vec);
                XMStoreFloat3(&tan, vec);

				tangentArray[j + i] = tan;


                /*binormal = XMVector3Normalize(binormal);

				XMFLOAT3 biNor;
                biNor.x = XMVectorGetX(binormal);
                biNor.y = XMVectorGetY(binormal);
                biNor.z = XMVectorGetZ(binormal);

                vec = XMLoadFloat3(&biNor);
                vec = XMVector3Transform(vec, *pMtx);
                vec = XMVector3Normalize(vec);
                XMStoreFloat3(&biNor, vec);

				binormalArray.emplace_back(biNor);*/
            }
        }
    }
    else
    {
        MessageBox(NULL, "���ڎw������Ŗ@���}�b�v���g�p���Ă�������", "�@���}�b�v�̃G���[", MB_OK | MB_ICONERROR);
    }
}

// �]�@���̓ǂݍ���---------------------------------------------------------------------------------
void FbxLoader ::LoadVtxBinormal(FbxMesh* pMesh, FbxLayer *pLayer, const XMMATRIX* pMtx )
{
    FbxLayerElementBinormal *pBinormalElement = pLayer->GetBinormals();
    if (pBinormalElement == nullptr)
    {
        for( int i=0; i< pMesh->GetPolygonVertexCount(); i++ )
        {
		   XMFLOAT3 biNor;
           biNor.x = -1;
           biNor.y = -1;
           biNor.z = -1;

		   binormalArray.emplace_back(biNor);
        }
        return;
    }
    int binormalCount = pBinormalElement->GetDirectArray().GetCount();

    if (pBinormalElement->GetReferenceMode() == FbxLayerElement::eDirect)
    {
        for (int j = 0; j < binormalCount; j++)
        {
			XMFLOAT3 biNor;
            Vector3 closs = Vector3::Cross(Vector3(normalArray[j]), Vector3(tangentArray[j]));
            biNor.x = closs.x;
            biNor.y = closs.y;
            biNor.z = closs.z;

            XMVECTOR vec = XMLoadFloat3(&biNor);
            vec = XMVector3Transform(vec, *pMtx);
            vec = XMVector3Normalize(vec);
            XMStoreFloat3(&biNor, vec);

			binormalArray.emplace_back(biNor);
        }
    }
    else
    {
        MessageBox(NULL, "���ڎw������Ŗ@���}�b�v���g�p���Ă�������", "�@���}�b�v�̃G���[", MB_OK | MB_ICONERROR);
    }
}

// UV���W�̓ǂݍ���---------------------------------------------------------------------------------
void FbxLoader::LoadVtxUv(FbxMesh* pMesh, FbxLayer *pLayer)
{
    FbxLayerElementUV* pUVElement;
    pUVElement = pLayer->GetUVs();

	// UV���ݒ肳��Ă��Ȃ�������S��0������
    if (pUVElement == nullptr)
    {
		int vertexesCount = pMesh->GetPolygonVertexCount();

		for (int i = 0; i < vertexesCount; i++)
		{
			XMFLOAT2 uv(0.0f, 0.0f);
			uvArray.emplace_back(uv);
		}

        return;
    }

    FbxLayerElement::EMappingMode mappingMode = pUVElement->GetMappingMode();

    //���t�@�����X���[�h�̎擾
    FbxLayerElement::EReferenceMode referenceMode = pUVElement->GetReferenceMode();

    // uv�̐��̎擾 (�\���p)
    int idxNum = pUVElement->GetIndexArray().GetCount();

    if (mappingMode == FbxLayerElement::eByPolygonVertex) {

        if (referenceMode == FbxLayerElement::eDirect) {

            int size = pUVElement->GetDirectArray().GetCount();

            if (size == idxNum) {
                for (int j = 0; j < size; j++)
                {
					XMFLOAT2 uv;
                    uv.x = (float)(1.0f - pUVElement->GetDirectArray().GetAt(j)[0]);
                    uv.y = (float)(1.0f - pUVElement->GetDirectArray().GetAt(j)[1]);

					uvArray.emplace_back(uv);
                }
            }
        }
        else if (referenceMode == FbxLayerElement::eIndexToDirect) {

            int size = pUVElement->GetIndexArray().GetCount();

            if (size == idxNum) {
                for (int j = 0; j < size; j++) {
                    int index = pUVElement->GetIndexArray().GetAt(j);

					XMFLOAT2 uv;
                    uv.x = (float)(pUVElement->GetDirectArray().GetAt(index)[0]);
                    uv.y = (float)(1.0f - pUVElement->GetDirectArray().GetAt(index)[1]);

					uvArray.emplace_back(uv);
                }
            }
        }
    }
}

// �{�[�����̓ǂݍ���-----------------------------------------------------------------------------
void FbxLoader::LoadBone(FbxMesh * pMesh, SkinMesh::MeshNode* pSkinMeshNode, const XMMATRIX* pMtx )
{
	int skinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount == 0)
		return;

	int controlPointCount = pMesh->GetControlPointsCount();

	FbxSkin* pSkin = (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	int clusterNum = pSkin->GetClusterCount();

	tmpBoneWeightList.resize((size_t)controlPointCount);

	FbxScene *pScene = pMesh->GetScene();
	FbxArray<FbxString*> takeNameAry;
	pScene->FillAnimStackNameArray(takeNameAry);

	int takeCnt = takeNameAry.Size() / 2;
	for (int i = 0; i < takeCnt; i++)
	{
		AnimInfo animInfo;
		animInfo.name = takeNameAry[i]->Buffer();
		skeleton.animInfo.emplace_back(animInfo);
	}

	Matrix4 offset = *pMtx;	// ���b�V���̏����p���s��

	for (int c = 0; c < clusterNum; c++)
	{
		FbxCluster *cluster = pSkin->GetCluster(c);
		int indexCount = cluster->GetControlPointIndicesCount();
		int *indices = cluster->GetControlPointIndices();
		auto weights = cluster->GetControlPointWeights();

		// �{�[���̒��_���i�[
		for (int j = 0; j < indexCount; ++j)
		{
			int controlPointIndex = indices[j];
			tmpBoneWeightList[controlPointIndex].push_back({ c, (float)weights[j] });
		}

		fbxsdk::FbxAMatrix initMat,invMat;
		cluster->GetTransformMatrix(initMat);
		cluster->GetTransformLinkMatrix(invMat);
		Matrix4 initTransform,invLinkTransformMatrix;
		initTransform = ConvFbxMtxXmtx(initMat);	
		invLinkTransformMatrix = ConvFbxMtxXmtx(invMat.Inverse());	// �����p���̋t�s��i�[

		Joint* bone = &skeleton.mJoints[c];

		bone->initWorldMatrix = initTransform;
		bone->initLocalMatrix = bone->initWorldMatrix;

		if (bone->parentIndex != -1)
		{
			// �e�̃��[���h��Ԃ̋t�s����|���Ă��̐e����̃��[�J����Ԃ̍s������߂�
			bone->initLocalMatrix = skeleton.mJoints[bone->parentIndex].initWorldMatrix.Inverse() * bone->initLocalMatrix;
		}

		Vector3 p = bone->initLocalMatrix.position();
		Vector3 s = bone->initLocalMatrix.scale();
		Quaternion q = bone->initLocalMatrix.rotation();
		Vector3 r = q.GetEulerAngles();

		if (bone)
		{			
			if (takeNameAry.Size() <= 0)
				continue;

			// �A�j���[�V�����̃t���[���P�ʂ̍s��i�[
			for (int i = 0; i < takeCnt; i++)
			{
				FbxTime start;
				FbxTime stop;
				FbxTime now;
				FbxTime frameTime;

				FbxAnimStack *animStack = pScene->FindMember<FbxAnimStack>(takeNameAry[i]->Buffer());
				pScene->SetCurrentAnimationStack(animStack);

				FbxTakeInfo *takeInfo = pScene->GetTakeInfo(*(takeNameAry[i]));
				start = takeInfo->mLocalTimeSpan.GetStart();
				stop = takeInfo->mLocalTimeSpan.GetStop();

				FbxTime::EMode timeMode = pScene->GetGlobalSettings().GetTimeMode();
				frameTime.SetTime(0, 0, 0, 1, 0, timeMode);
				now = start;

				UINT maxFrame = 0;

				std::string name = takeNameAry[i]->Buffer();

				FbxNode *pLink = cluster->GetLink();

				Anim * animation = new Anim();

				while (now < stop)
				{
					// ���_�ɖ߂��Ĉړ�
					Matrix4 mtx;
					FbxMatrix currentPos;
					currentPos = pLink->EvaluateGlobalTransform(now);
					mtx = ConvFbxMtxXmtx(currentPos);

					Vector3 p = mtx.position();
					Vector3 s = mtx.scale();
					Quaternion q = mtx.rotation();

					Translation3f pos = Translation3f(p.x, p.y, p.z);
					Scaling3f sca = Scaling3f(s.x, s.y, s.z);
					Eigen::Affine3f affine = pos * sca * q.quaternion;
					mtx.matrix = affine.matrix();

					Keyframe * keyFrame = new Keyframe();
					keyFrame->frameNum = maxFrame;
					keyFrame->localMatrix = mtx * invLinkTransformMatrix * offset;

					// �e�̃{�[�����猩�����[�J����ԂɕύX
					if (bone->parentIndex != -1)
					{
						// �e�̃L�[�t���[���A�j���[�V�����̋t�s����擾
						Matrix4 invParentMatrix = GetSkeltonKeyFrameWorldMatrix(&skeleton.mJoints[bone->parentIndex], i, keyFrame->frameNum).Inverse();
						keyFrame->localMatrix = invParentMatrix * keyFrame->localMatrix;
					}

					// �L�[�t���[���A�j���[�V�����Z�b�g
					animation->keyFrames.emplace_back(std::shared_ptr<Keyframe>(keyFrame));

					maxFrame += 1;
					now += frameTime;
				}

				bone->animations.emplace_back(std::shared_ptr<Anim>(animation));

				if(skeleton.animInfo[i].maxFrame < maxFrame)
					skeleton.animInfo[i].maxFrame = maxFrame;
			}
		}
	}

	for (int i = 0; i < takeNameAry.Size(); i++)
	{
		delete takeNameAry[i];
	}


	// �R���g���[���|�C���g�ɑΉ������E�F�C�g���쐬
	for (auto& tmpBoneWeight : tmpBoneWeightList)
	{
		// �E�F�C�g�̑傫���Ń\�[�g
		std::sort(tmpBoneWeight.begin(), tmpBoneWeight.end(),
			[](const TmpWeight& weightA, const TmpWeight& weightB) { return weightA.second > weightB.second; }
			//[](const TmpWeight& weightA, const TmpWeight& weightB){ return weightA.second < weightB.second; }
		);

		// 1���_��4��葽���E�F�C�g������U���Ă���Ȃ�e�������Ȃ����͖̂�������
		while (tmpBoneWeight.size() > 4)
		{
			tmpBoneWeight.pop_back();
		}

		// 4�ɖ����Ȃ��ꍇ�̓_�~�[��}��
		while (tmpBoneWeight.size() < 4)
		{
			tmpBoneWeight.push_back({ 0, 0.0f });
		}

		float total = 0.0f;
		for (int i = 0; i < 4; ++i)
		{
			total += tmpBoneWeight[i].second;
		}

		// �E�F�C�g�̐��K��
		for (int i = 0; i < 4; ++i)
		{
			tmpBoneWeight[i].second /= total;
		}
	}
}

bool FbxLoader::ProcessSkeletonHeirarchyre(FbxNode * node, int depth, int num, int index, int parentindex, std::vector<int> *hierarchy)
{
	bool flag = false;

	if (node->GetNodeAttribute() &&
		node->GetNodeAttribute()->GetAttributeType() &&
		node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint joint;
		if (depth == 0)
		{
			joint.parentIndex = -1;
		}
		else
		{
			joint.parentIndex = parentindex;
		}
		
		joint.name = node->GetName();

		// ������"_end"������ꍇ�͂���Ȃ��̂ő}�����Ȃ�
		size_t pos = joint.name.rfind("_end");
		if (pos == std::string::npos)
		{
			flag = true;

			if (joint.parentIndex == -1)
			{
				depth += 1;
			}
			depth += 1;
			hierarchy->emplace_back(num);

			joint.hierarchy = *hierarchy;


			// �����̐e�Ɏ��g�� Id ��o�^
			if (joint.parentIndex >= 0)
			{
				skeleton.mJoints[joint.parentIndex].childsId.emplace_back(index);
			}
			
			skeleton.mJoints.emplace_back(joint);
		}
	}

	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; i++)
	{
		bool hit = ProcessSkeletonHeirarchyre(node->GetChild(i), depth, i, (int)skeleton.mJoints.size(), index, hierarchy);

		if (hit && hierarchy->size() > 0)
		{
			hierarchy->pop_back();
			if (childCnt - 1 == i)	// ���̊K�w�̎q�̃J�E���g�̍Ō�Ō��݂̊K�w���폜����
				depth -= 1;
		}
	}

	return flag;
}

Joint* FbxLoader::GetSkeleton(std::string name)
{
	for (auto& bone : skeleton.mJoints)
	{
		if (bone.name == name)
		{
			return &bone;
		}
	}
	return nullptr;
}


// �V���h�E�}�b�s���O�p�̕`��-----------------------------------------------------------------------
//void FbxLoader::DrawShadow()
//{
//    // �p�����[�^�̌v�Z
//   /* const XMMATRIX SCALE = XMMatrixScaling( m_scale.m128_f32[0], m_scale.m128_f32[1], m_scale.m128_f32[2] );
//    const XMMATRIX ROTA = XMMatrixRotationX( m_rota.m128_f32[0] ) * XMMatrixRotationY( m_rota.m128_f32[1] ) * XMMatrixRotationZ( m_rota.m128_f32[2] );
//    const XMMATRIX POS = XMMatrixTranslation( m_pos.m128_f32[0], m_pos.m128_f32[1], m_pos.m128_f32[2] );
//
//    const XMMATRIX WORLD = SCALE * ROTA * POS;
//
//    for ( auto i: m_pRootPolygon->m_children )
//    {
//        this->DrawTreeShadow(&WORLD, i );
//    }*/
//}

// �V���h�E�}�b�s���O�p�̃��f����`��---------------------------------------------------------------
//void FbxLoader ::DrawTreeShadow( const XMMATRIX *world, CPolygonNode *pPolygonNode )
//{
//    for ( auto i: pPolygonNode->m_children )
//    {
//        this->DrawTreeShadow(world, i );
//    }
//
//    if (pPolygonNode->m_Material == nullptr)
//        return;
//
////    m_pLight->UpdateWVP(world);
//
////    m_pLight->SetShadowOption(pPolygonNode->m_pPolygon->GetVtxBuf(), pPolygonNode->m_pPolygon->GetIdxBuf());
////    m_pLight->ShadowMapping(pPolygonNode->m_pPolygon->m_pVtx, pPolygonNode->m_pMesh, pPolygonNode->m_pPolygon->GetVtxBuf());
//}

// �`��---------------------------------------------------------------------------------------------
//void FbxLoader::Draw(XMMATRIX &matrix)
//{
//    // �p�����[�^�̌v�Z
//    for( auto i: m_pRootPolygon->m_children )
//    {
//        this->DrawTree(&matrix, i );
//    }
//}

// �ꏊ���璸�_���擾-------------------------------------------------------------------------------
//VTX_SKIN_MESH* FbxLoader ::GetVtxToPos( const XMVECTOR *pos )
//{
//    VTX_SKIN_MESH *returnVtx = nullptr;
//    for( auto i: m_pRootPolygon->m_children )
//    {
//        returnVtx = this->GetNodeVtxToPos( pos, i );
//        if( returnVtx != nullptr )
//        {
//            break;
//        }
//    }
//
//    return returnVtx;
//}

// �ꏊ����m�[�h�̒��_���擾-----------------------------------------------------------------------
//VTX_SKIN_MESH* FbxLoader ::GetNodeVtxToPos( const XMVECTOR *pPos, CPolygonNode *pPolygonNode )
//{
//    VTX_SKIN_MESH *returnVtx = nullptr;
//    for ( auto i: pPolygonNode->m_children )
//    {
//        returnVtx = this->GetNodeVtxToPos( pPos, i );
//        if( returnVtx != nullptr )
//        {
//            return returnVtx;
//        }
//    }
//
//    for( int i=0; i<pPolygonNode->m_Material->GetShader<SkinMeshShader>().lock()->pPolygon->GetVtxCnt(); i+= 3 )
//    {
//        VTX_SKIN_MESH *pNow = &pPolygonNode->m_Material->GetShader<SkinMeshShader>().lock()->pPolygon->m_pVtx[i];
//        const XMVECTOR A = XMLoadFloat3( &pNow[0].nor );
//        const XMVECTOR B = XMLoadFloat3( &pNow[1].nor );
//        const XMVECTOR C = XMLoadFloat3( &pNow[2].nor );
//        const XMVECTOR NORMAL = XMVector3Normalize( A + B + C );
//
//        if( this->CheckVtxToPos( pPos, &NORMAL, pNow ) )
//        {
//            returnVtx = pNow;
//        }
//    }
//
//    return returnVtx;
//}

// �w�肵���ꏊ�̒��_���m�F-------------------------------------------------------------------------
bool FbxLoader ::CheckVtxToPos( const XMVECTOR *pPos, const XMVECTOR *pNormal, VTX_SKIN_MESH *pVtx )
{
    const XMVECTOR VEC_RAY = { 0.0f, -1.0f, 0.0f, 0.0f };
    const XMVECTOR A = XMLoadFloat3( &pVtx[0].pos );
    const XMVECTOR B = XMLoadFloat3( &pVtx[1].pos );
    const XMVECTOR C = XMLoadFloat3( &pVtx[2].pos );


    //���C�����ʂɕ��s�ȏꍇ�◠�|���S���̏ꍇ�A��_�����Ƃ���
    XMVECTOR vecWork = A - *pPos;
    float numerator = XMVector3Dot( *pNormal, vecWork ).m128_f32[0];
    float denominator = XMVector3Dot( *pNormal, VEC_RAY ).m128_f32[0];

    if ( denominator >= 0 )
    {
        return false;
    }
    float t = numerator / denominator;

    //���C�̋t�����ɂ���|���S���̏ꍇ�A��_�����Ƃ���
    XMVECTOR vP = *pPos + VEC_RAY * t;
    vecWork = vP - *pPos;
    XMVECTOR vecNor = XMVector3Normalize( vecWork );

    if ( XMVector3Dot( VEC_RAY, vecNor ).m128_f32[0] <= 0 )
    {
        return false;
    }

    //��_���|���S�����ɂ��邩�ǂ������ׂ�
    XMVECTOR v1, v2;
    float result;

    v1 = B - A;
    v2 = vP - B;
    vecWork = XMVector3Cross( v1, v2 );
    result = XMVector3Dot( vecWork, *pNormal ).m128_f32[0];
    if ( result < 0 )
    {
        return false;
    }

    v1 = C - B;
    v2 = vP - C;
    vecWork = XMVector3Cross( v1, v2 );
    result = XMVector3Dot( vecWork, *pNormal ).m128_f32[0];
    if ( result < 0 )
    {
        return false;
    }

    v1 = A - C;
    v2 = vP - A;
    vecWork = XMVector3Cross( v1, v2 );
    result = XMVector3Dot( vecWork, *pNormal ).m128_f32[0];
    if ( result < 0 )
    {
        return false;
    }

    return true;
}

Matrix4 FbxLoader::GetSkeltonKeyFrameWorldMatrix(Joint * joint, const int takeNum, const int keyFrame)
{
	if (joint->parentIndex != -1)
	{
		Joint * j = &skeleton.mJoints[joint->parentIndex];
		Matrix4 mat = GetSkeltonKeyFrameWorldMatrix(j, takeNum, keyFrame);

		return mat * joint->animations[takeNum]->keyFrames[keyFrame]->localMatrix;
	}
	else
	{
		// ��e�������ꍇ���̂܂܂̍s���Ԃ�
		return joint->animations[takeNum]->keyFrames[keyFrame]->localMatrix;
	}

}

#include "GameObjectManager.h"
#include "../Common.h"
#include "../../DirectX/Common.h"
#include "../../Scripts/Common.h"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace FrameWork;

void GameObjectManager::RegisterSceneDataGameObjects(const std::shared_ptr<SceneData>& sceneData)
{
	for (auto object : sceneData->gameObjectList)
	{
		RegisterGameObject(object.lock(), true);
	}
}

std::weak_ptr<GameObject> GameObjectManager::CreateGameObject(GameObject * gameObject)
{
	if (!Singleton<SceneManager>::Instance()->GetCurrentScene()->IsSceneData())
	{
		delete gameObject;
		std::weak_ptr<GameObject> dummy;
		return dummy;
	}

	int layer = (int)gameObject->layer;
	std::shared_ptr<GameObject> object = std::shared_ptr<GameObject>(gameObject);
	gameObjectList.emplace_back(object);
	object->Initialize(object);	// ������

	Singleton<SceneManager>::Instance()->GetCurrentScene()->AddSceneGameObject(object);

	return gameObjectList.back();
}

std::weak_ptr<GameObject> GameObjectManager::Instantiate(const std::weak_ptr<GameObject>& instance)
{
	if (instance.expired()) return std::weak_ptr<GameObject>();
	if (!Singleton<SceneManager>::Instance()->GetCurrentScene()->IsSceneData())
		return std::weak_ptr<GameObject>();
	
	auto transform = instance.lock()->transform.lock();
	auto parent = transform->GetParent();
	
	Translation3f translation = transform->translation;
	Scaling3f scaling = transform->scaling;
	Quaternion rotate = transform->rotate;
	// ��U�e��ޔ�
	if (!parent.expired())
	{
		transform->parent.reset();
		Matrix4 localMat = transform->worldMatrix;
		Vector3 pos = localMat.position();
		Vector3 sca = localMat.scale();
		transform->translation = Translation3f(pos.x, pos.y, pos.z);
		transform->scaling = Scaling3f(sca.x, sca.y, sca.z);
		transform->rotate = localMat.rotation();
	}

	// ��U�V���A����
	std::stringstream ss;
	{
		cereal::BinaryOutputArchive o_archive(ss);
		//cereal::JSONOutputArchive o_archive(ss);
		o_archive(instance.lock());
	}

	// �R�s�[��ޔ������e���ēx�Z�b�g
	if (!parent.expired())
	{
		transform->parent = parent;

		transform->translation = translation;
		transform->scaling = scaling;
		transform->rotate = rotate;
	}

	// �V���A�����������̂�V�����C���X�^���X����
	std::shared_ptr<GameObject> object;
	{
		cereal::BinaryInputArchive i_archive(ss);
		//cereal::JSONInputArchive i_archive(ss);
		i_archive(object);

		// �V�������������I�u�W�F�N�g�̃��[���h�}�g���N�X���Z�b�g
		object->transform.lock()->worldMatrix = object->transform.lock()->localMatrix;

		// �q�I�u�W�F�N�g���܂ߑS�Ă����X�g�ɓo�^
		this->RegisterGameObject(object, false);
	}

	return object;
}

std::weak_ptr<GameObject> GameObjectManager::Instantiate(const std::string & fileName)
{
	if (!Singleton<SceneManager>::Instance()->GetCurrentScene()->IsSceneData())
		return std::weak_ptr<GameObject>();

	std::string prefabFile = fileName;
	// �g���q���Ȃ��ꍇ�͕t������
	/*size_t pos = prefabFile.rfind(".prefab");
	if (pos == std::string::npos)
	{
		prefabFile += ".prefab";
	}*/

	std::ifstream ifs(prefabFile, std::ios::binary);
	if (!ifs)
	{
		MessageBox(NULL, (prefabFile + std::string("�ǂݍ��ݎ��s")).c_str(), "�C���X�^���X��", MB_OK);
		return std::weak_ptr<GameObject>();
	}

	std::shared_ptr<GameObject> object;
	{
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(object);
		if (object) this->RegisterGameObject(object, false);
	}

	ifs.close();

	return object;
}

void GameObjectManager::DestroyGameObject(std::weak_ptr<GameObject>& object, float limit)
{
	// ���ɓo�^����Ă����牽�����Ȃ�
	for (auto & obj : destroyObjects)
	{
		if (obj.gameObject.lock() == object.lock()) return;
	}

	DestroyGameObjectData data;
	data.gameObject = object;
	data.dethLimit = limit;

	destroyObjects.emplace_back(data);
}

void GameObjectManager::CleanupDestroyGameObject()
{
	for (auto itr = destroyObjects.begin(), end = destroyObjects.end();itr != end;)
	{
		if (itr->dethLimit > 0.0f)
		{
			itr->dethLimit -= Time::DeltaTime();
			++itr;
			continue;
		}

		EraseGameObject(itr->gameObject);
		itr = destroyObjects.erase(itr);
	}
}

std::weak_ptr<GameObject> GameObjectManager::FindGameObject(std::string name, int num)
{
	if (num < 0) return std::weak_ptr<GameObject>();

	for (auto & object : gameObjectList)
	{
		if (object->name == name)
		{
			if (num == 0) return (std::weak_ptr<GameObject>)object;

			num--;
		}
	}

	return std::weak_ptr<GameObject>();
}

std::weak_ptr<GameObject> GameObjectManager::FindGameObject(Tag tag, int num)
{
	if (num < 0) return std::weak_ptr<GameObject>();

	for (auto & object : gameObjectList)
	{
		if (object->tag == tag)
		{
			if (num == 0) return (std::weak_ptr<GameObject>)object;

			num--;
		}
	}

	return std::weak_ptr<GameObject>();
}

std::list<std::weak_ptr<GameObject>> GameObjectManager::FindGameObjects(std::string name)
{
	std::list<std::weak_ptr<GameObject>> findList;

	for (auto & object : gameObjectList)
	{
		if (object->name == name)
			findList.emplace_back(object);
	}

	return findList;
}

std::list<std::weak_ptr<GameObject>> GameObjectManager::FindGameObjects(Tag tag)
{
	std::list<std::weak_ptr<GameObject>> findList;

	for (auto & object : gameObjectList)
	{
		if (object->tag == tag)
			findList.emplace_back(object);
	}

	return findList;
}

void GameObjectManager::EraseGameObject(std::weak_ptr<GameObject>& object)
{
	for (auto itr = gameObjectList.begin(), end = gameObjectList.end(); itr != end; ++itr)
	{
		if ((*itr) != object.lock()) continue;

		Singleton<SceneManager>::Instance()->RemoveSceneGameObject(*itr);

		(*itr)->Finalize();
		itr = gameObjectList.erase(itr);
		return;
	}
}

void GameObjectManager::RegisterGameObject(const std::shared_ptr<GameObject>& object, bool isSceneData)
{
	// ���X�g�ɓo�^
	gameObjectList.emplace_back(object);

	// �V�[���̃f�[�^����o�^���Ă����ꍇ�͖���
	if(!isSceneData)
		Singleton<SceneManager>::Instance()->GetCurrentScene()->AddSceneGameObject(object);
	// �R���|�[�l���g�̓o�^
	object->LoadSerialized();

	// �q�̃I�u�W�F�N�g�����l�ɓo�^
	for (auto & child : object->transform.lock()->GetChilds())
	{
		if (child.lock()->gameObject.expired())
			continue;

		this->RegisterGameObject(child.lock()->gameObject.lock(), isSceneData);
	}
}

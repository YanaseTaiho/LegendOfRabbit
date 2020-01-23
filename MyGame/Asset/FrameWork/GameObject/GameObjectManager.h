#ifndef _GAMEOBJECTMANAGER_H_
#define _GAMEOBJECTMANAGER_H_

#include "GameObject.h"
#include <memory>
#include <list>

namespace FrameWork
{
	// �V���O���g���N���X
	class GameObjectManager
	{
		/*friend Singleton<GameObjectManager>;
		GameObjectManager() {}
		~GameObjectManager() {}*/

	public:
		// �V�����I�u�W�F�N�g�����
		std::weak_ptr<GameObject> CreateGameObject(GameObject * gameObject);
		// �I�u�W�F�N�g�̃R�s�[�𐶐�
		std::weak_ptr<GameObject> Instantiate(const std::weak_ptr<GameObject> & instance);
		// �t�@�C���f�[�^����I�u�W�F�N�g����
		std::weak_ptr<GameObject> Instantiate(const std::string & fileName);

		void DestroyGameObject(std::weak_ptr<GameObject> & object, float limit = 0);
		void CleanupDestroyGameObject();	// �폜�����I�u�W�F�N�g�Q�ɓo�^���ꂽ�I�u�W�F�N�g���폜���邩���f���ă��X�g����O��

		// �I�u�W�F�N�g���X�g���擾
		const std::list<std::shared_ptr<GameObject>> & GetGameObjectList() { return gameObjectList; }

		// �ŏ��Ɍ��������O�̃I�u�W�F�N�g���擾
		std::weak_ptr<GameObject> FindGameObject(std::string name, int num = 0);
		// �ŏ��Ɍ������^�O�̃I�u�W�F�N�g���擾
		std::weak_ptr<GameObject> FindGameObject(Tag tag, int num = 0);
		// �ŏ��Ɍ������N���X�������Ă���I�u�W�F�N�g���擾
		template <typename T>
		std::weak_ptr<GameObject> FindGameObject(int num = 0);

		// ���������O�̕����̃I�u�W�F�N�g���擾
		std::list<std::weak_ptr<GameObject>> FindGameObjects(std::string name);
		// �������^�O�̕����̃I�u�W�F�N�g���擾
		std::list<std::weak_ptr<GameObject>> FindGameObjects(Tag tag);
		// �������N���X�������Ă��镡���̃I�u�W�F�N�g���擾
		template <typename T>
		std::list<std::weak_ptr<GameObject>> FindGameObjects();

	private:
		
		struct DestroyGameObjectData
		{
			std::weak_ptr<GameObject> gameObject;
			float dethLimit;
		};

		void EraseGameObject(std::weak_ptr<GameObject> & object);
		void RegisterGameObject(const std::shared_ptr<GameObject> & object);

		std::list<std::shared_ptr<GameObject>> gameObjectList;
		std::list<DestroyGameObjectData> destroyObjects;

	};

	template<typename T>
	inline std::weak_ptr<GameObject> GameObjectManager::FindGameObject(int num)
	{
		if (num < 0) return std::weak_ptr<GameObject>();

		for (auto & object : gameObjectList)
		{
			if (!object->GetComponent<T>().expired())
			{
				if (num == 0) return (std::weak_ptr<GameObject>)object;

				num--;
			}
		}

		return std::weak_ptr<GameObject>();
	}

	template<typename T>
	inline std::list<std::weak_ptr<GameObject>> GameObjectManager::FindGameObjects()
	{
		std::list<std::weak_ptr<GameObject>> list;

		for (auto & object : gameObjectList)
		{
			if (!object->GetComponent<T>().expired())
			{
				list.emplace_back(object);
			}
		}

		return list;
	}
}

#endif // !_GAMEOBJECTMANAGER_H_


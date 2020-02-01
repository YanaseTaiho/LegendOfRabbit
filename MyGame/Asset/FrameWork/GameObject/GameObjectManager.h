#ifndef _GAMEOBJECTMANAGER_H_
#define _GAMEOBJECTMANAGER_H_

#include "GameObject.h"
#include <memory>
#include <list>

namespace FrameWork
{
	class SceneData;

	// シングルトンクラス
	class GameObjectManager
	{
		/*friend Singleton<GameObjectManager>;
		GameObjectManager() {}
		~GameObjectManager() {}*/

	public:
		// シーンのデータのオブジェクトを登録
		void RegisterSceneDataGameObjects(const std::shared_ptr<SceneData> & sceneData);

		// 新しくオブジェクトを作る
		std::weak_ptr<GameObject> CreateGameObject(GameObject * gameObject);
		// オブジェクトのコピーを生成
		std::weak_ptr<GameObject> Instantiate(const std::weak_ptr<GameObject> & instance);
		// ファイルデータからオブジェクト生成
		std::weak_ptr<GameObject> Instantiate(const std::string & fileName);

		void DestroyGameObject(std::weak_ptr<GameObject> & object, float limit = 0);
		void CleanupDestroyGameObject();	// 削除されるオブジェクト群に登録されたオブジェクトを削除するか判断してリストから外す

		// オブジェクトリストを取得
		const std::list<std::shared_ptr<GameObject>> & GetGameObjectList() { return gameObjectList; }

		// 最初に見つけた名前のオブジェクトを取得
		std::weak_ptr<GameObject> FindGameObject(std::string name, int num = 0);
		// 最初に見つけたタグのオブジェクトを取得
		std::weak_ptr<GameObject> FindGameObject(Tag tag, int num = 0);
		// 最初に見つけたクラスを持っているオブジェクトを取得
		template <typename T>
		std::weak_ptr<GameObject> FindGameObject(int num = 0);

		// 見つけた名前の複数のオブジェクトを取得
		std::list<std::weak_ptr<GameObject>> FindGameObjects(std::string name);
		// 見つけたタグの複数のオブジェクトを取得
		std::list<std::weak_ptr<GameObject>> FindGameObjects(Tag tag);
		// 見つけたクラスを持っている複数のオブジェクトを取得
		template <typename T>
		std::list<std::weak_ptr<GameObject>> FindGameObjects();

	private:
		
		struct DestroyGameObjectData
		{
			std::weak_ptr<GameObject> gameObject;
			float dethLimit;
		};

		void EraseGameObject(std::weak_ptr<GameObject> & object);
		void RegisterGameObject(const std::shared_ptr<GameObject> & object, bool isSceneData);

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


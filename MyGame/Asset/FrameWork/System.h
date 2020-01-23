#ifndef __SYSTEM__H__
#define __SYSTEM__H__

#include "GameObject/GameObjectManager.h"

namespace FrameWork
{
	namespace System
	{
	//	std::weak_ptr<GameObject> & CreateGameObject(GameObject * gameObject) { return GameObjectManager::Instance()->CreateGameObject(gameObject); }

	//	// 最初に見つけた名前のオブジェクトを取得
	//	std::weak_ptr<GameObject> FindGameObject(std::string name, int num = 0) { return GameObjectManager::Instance()->FindGameObject(name, num); }
	//	// 最初に見つけたタグのオブジェクトを取得
	//	std::weak_ptr<GameObject> FindGameObject(Tag tag, int num = 0) { return GameObjectManager::Instance()->FindGameObject(tag, num); }
	//	// 最初に見つけたクラスを持っているオブジェクトを取得
	//	template <typename T>
	//	std::weak_ptr<GameObject> FindGameObject(int num = 0) { return GameObjectManager::Instance()->FindGameObject<T>(num); }

	//	// 見つけた名前の複数のオブジェクトを取得
	//	std::list<std::weak_ptr<GameObject>> FindGameObjects(std::string name) { return GameObjectManager::Instance()->FindGameObjects(name); }
	//	// 見つけたタグの複数のオブジェクトを取得
	//	std::list<std::weak_ptr<GameObject>> FindGameObjects(Tag tag) { return GameObjectManager::Instance()->FindGameObjects(tag); }
	//	// 見つけたクラスを持っている複数のオブジェクトを取得
	//	template <typename T>
	//	std::list<std::weak_ptr<GameObject>> FindGameObjects() { return GameObjectManager::Instance()->FindGameObjects<T>(); }
	}
}

#endif // !__SYSTEM__H__
#ifndef __SYSTEM__H__
#define __SYSTEM__H__

#include "GameObject/GameObjectManager.h"

namespace FrameWork
{
	namespace System
	{
	//	std::weak_ptr<GameObject> & CreateGameObject(GameObject * gameObject) { return GameObjectManager::Instance()->CreateGameObject(gameObject); }

	//	// �ŏ��Ɍ��������O�̃I�u�W�F�N�g���擾
	//	std::weak_ptr<GameObject> FindGameObject(std::string name, int num = 0) { return GameObjectManager::Instance()->FindGameObject(name, num); }
	//	// �ŏ��Ɍ������^�O�̃I�u�W�F�N�g���擾
	//	std::weak_ptr<GameObject> FindGameObject(Tag tag, int num = 0) { return GameObjectManager::Instance()->FindGameObject(tag, num); }
	//	// �ŏ��Ɍ������N���X�������Ă���I�u�W�F�N�g���擾
	//	template <typename T>
	//	std::weak_ptr<GameObject> FindGameObject(int num = 0) { return GameObjectManager::Instance()->FindGameObject<T>(num); }

	//	// ���������O�̕����̃I�u�W�F�N�g���擾
	//	std::list<std::weak_ptr<GameObject>> FindGameObjects(std::string name) { return GameObjectManager::Instance()->FindGameObjects(name); }
	//	// �������^�O�̕����̃I�u�W�F�N�g���擾
	//	std::list<std::weak_ptr<GameObject>> FindGameObjects(Tag tag) { return GameObjectManager::Instance()->FindGameObjects(tag); }
	//	// �������N���X�������Ă��镡���̃I�u�W�F�N�g���擾
	//	template <typename T>
	//	std::list<std::weak_ptr<GameObject>> FindGameObjects() { return GameObjectManager::Instance()->FindGameObjects<T>(); }
	}
}

#endif // !__SYSTEM__H__
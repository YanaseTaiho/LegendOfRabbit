#ifndef _PREFAB_H_
#define _PREFAB_H_

#include "../GameObject/GameObject.h"

namespace FrameWork
{
	class Prefab
	{
	private:
		std::list<std::shared_ptr<GameObject>> prefabList;				// プレハブのリスト
		std::list<std::shared_ptr<GameObject>> prefabGameObjectList;	// インスタンスオブジェクトリスト
		std::list<std::shared_ptr<Component>> prefabComponentList;		// インスタンスコンポーネントリスト
		std::weak_ptr<GameObject> selectObject;
		std::weak_ptr<Component> selectComponent;
		std::list<std::string> deletePathList;

	public:
		void Load(std::string path);
		void Save(std::string path);

		void RegisterObject(std::weak_ptr<GameObject> gameObject);
		void RegisterInsertObject(std::weak_ptr<GameObject> gameObject, std::list<std::shared_ptr<GameObject>>::iterator & outItr);
		void DeleteObject(std::shared_ptr<GameObject> gameObject);

		void DrawImGui();

		const char * GAMEOBJECT = "GAMEOBJECT";

		std::weak_ptr<GameObject> FindGameObject(std::string name);

		template<class T>
		std::list<std::weak_ptr<T>> FindComponents()
		{
			std::list<std::weak_ptr<T>> outList;

			for (auto & com : prefabComponentList)
			{
				if (com->GetType() == typeid(T))
				{
					std::shared_ptr<T> hit = std::static_pointer_cast<T>(com);
					outList.emplace_back(hit);
				}
			}
			return outList;
		}

	private:

		void RegisterGameObject(const std::shared_ptr<GameObject> & object);
		void DeleteGameObject(std::weak_ptr<GameObject> object);

		void DeletePrefabObjectList(std::weak_ptr<GameObject> object);
		void DeletePrefabComponentList(std::weak_ptr<Component> component);
		void SetOriginalName(std::weak_ptr<GameObject> & original);
		bool CheckName(std::weak_ptr<GameObject> & mine, std::string checkName);
		bool CheckName(std::weak_ptr<GameObject> & mine, std::string checkName, std::list<std::shared_ptr<GameObject>>::iterator & outItr);
	};
}

#endif // !_PREFAB_H_

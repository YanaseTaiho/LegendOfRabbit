#ifndef _SCENEBASE_H_
#define _SCENEBASE_H_

#include "../Common.h"
#include "SceneData.h"
#include <typeinfo.h>
#include "Prefab.h"

namespace FrameWork
{
	class SceneBase
	{
		friend SceneManager;
		friend class SceneDataManager;
	public:
		SceneBase();
		virtual ~SceneBase();

		virtual void Initialize();
		virtual void Start();
		virtual void Update();
		virtual void Draw();

		bool IsSceneData() { return sceneData.operator bool(); }
		void AddSceneGameObject(std::weak_ptr<GameObject> addObject);
		const type_info& GetType() { return typeid(*this); }
		bool IsStart() { return isStart; }

		std::weak_ptr<GameObject> GetPrefabGameObject(std::string name);

		Prefab prefab;

	protected:
		bool isStart = false;
		std::shared_ptr<SceneData> sceneData;

		bool isDebug = false;
	};
}

#endif // !_SCENEBASE_H_

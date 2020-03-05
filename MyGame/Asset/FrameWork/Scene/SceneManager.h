#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#include "../Singleton.h"
#include <string>
#include <vector>
#include <memory>

namespace FrameWork
{
	class SceneBase;
	class SceneData;
	class GameObject;

	class SceneManager
	{
		friend Singleton<SceneManager>;
		SceneManager() {}
		~SceneManager();
	public:

		void Initialize(SceneBase * scene);

		void SceneChange(SceneBase * newScene);
		SceneBase * GetCurrentScene();

		void LoadSceneData(std::string sceneName);
		void LoadSceneData(std::stringstream & sceneData);
		void SaveSceneData();
		void DrawImGui();

		void Update();
		void Draw();
		void SceneCheck();

		void RemoveSceneGameObject(std::weak_ptr<GameObject> remove);

	private:
		void LoadEditorSceneData(std::string sceneName);	// エディタ用

		void LoadFileData(std::string path);
		void SaveSceneData(std::shared_ptr<SceneData> sceneData);
		void DeleteSceneFile(std::string delName);
		void DeleteSceneData(std::shared_ptr<SceneData> & deleteData);
		void CreateSceneData(std::shared_ptr<SceneData> & createData);
		void SetOriginalName(std::string * original);
		bool CheckName(std::string * mine, std::string checkName);

		SceneBase * currentScene;
		SceneBase * nextScene;

		std::shared_ptr<SceneData> nextSceneData;

		std::vector<std::shared_ptr<std::string>> sceneNameArray;
	};
}

#endif // !_SCENEMANAGER_H_


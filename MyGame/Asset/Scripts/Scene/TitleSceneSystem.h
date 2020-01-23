#ifndef _TITLESCENESYSTEM_H_
#define _TITLESCENESYSTEM_H_

#include "FrameWork/Component/MonoBehaviour/MonoBehaviour.h"
#include "Scene/GameScene.h"

#include "Main/audio_clip.h"

class TitleSceneSystem : public MonoBehaviour
{
public:
	TitleSceneSystem() {}
	~TitleSceneSystem()
	{ }

	void Start() override
	{
		clip = new CAudioClip();
		clip->Load("Asset/Art/Sound/Title.wav");
		clip->Play(true);
	}

	void Update() override
	{
		/*if (Input::Keyboad::IsTrigger(VK_RETURN))
		{
			SceneManager::Instance()->SceneChange(new GameScene());
		}*/
	}

	void OnDestroy() override
	{
		clip->Stop();
		clip->Unload();
		delete clip;
	}

private:
	CAudioClip * clip;
};

#endif // !_TITLESCENESYSTEM_H_
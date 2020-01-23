#include "TitleScene.h"
#include "DirectX/Camera/DXCamera.h"

#include "Scripts/Scene/TitleSceneSystem.h"
#include "Scripts/AnimationController/TestAnimationController.h"
#include "Scripts/Camera/ToolCamera.h"

#include "Scripts/test_move.h"

void TitleScene::Initialize()
{
	GameObjectManager * manager = Singleton<GameObjectManager>::Instance();

	std::weak_ptr<GameObject> camera1 = manager->CreateGameObject(new GameObject("MainCamera", Tag::MainCamera, Layer::Camera));
	camera1.lock()->AddComponent(new Transform(Vector3(0.0f, 8.0f, -20.0f), Vector3::one(), Quaternion(30.0f, 0.0f, 0.0f)));
	camera1.lock()->AddComponent(new DXCamera(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0.5f, 1000.0f));
	camera1.lock()->AddComponent(new ToolCamera());

	std::weak_ptr<GameObject> title = manager->CreateGameObject(new GameObject("TitleScene", Tag::None, Layer::Default));
	title.lock()->AddComponent(new Transform());
	title.lock()->AddComponent(new TitleSceneSystem());

	std::weak_ptr<GameObject> dirLight = manager->CreateGameObject(new GameObject("Light", Tag::None, Layer::Default));
	dirLight.lock()->AddComponent(new Transform(Vector3(0.0f, 10.0f, 0.0f), Vector3::one(),Quaternion(45.0f, 0.0f, 45.0f)));
	dirLight.lock()->AddComponent(new DirectionalLight(
		Color(1.0f, 1.0f, 1.0f, 1.0f),		// ディフーズ
		Color(0.1f, 0.1f, 0.1f, 1.0f)));	// アンビエント
	dirLight.lock()->AddComponent(new TestMove());
	{
		std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("chara", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(0.0f, 1.0f, 0.0f), Vector3(3.08f, 3.08f, 3.08f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("SmaphoMan.fbx"), false));
		chara.lock()->AddComponent(new Animator(new TestAnimationController("SmaphoMan|Deth")));
		chara.lock()->AddComponent(new CollisionCapsule(Vector3(0.0f, 5.0f, 0.0f), 3.5f, Vector3(0.0f, 1.5f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), false));
	}
	{
		std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("chara", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(10.0f, 1.0f, 0.0f), Vector3(3.08f, 3.08f, 3.08f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("SmaphoMan.fbx"), false));
		chara.lock()->AddComponent(new Animator(new TestAnimationController("SmaphoMan|Walk")));
		chara.lock()->AddComponent(new CollisionCapsule(Vector3(0.0f, 5.0f, 0.0f), 3.5f, Vector3(0.0f, 1.5f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), false));
	}
	{
		std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("chara", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(-10.0f, 1.0f, 0.0f), Vector3(3.08f, 3.08f, 3.08f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("SmaphoMan.fbx"), false));
		chara.lock()->AddComponent(new Animator(new TestAnimationController("SmaphoMan|Attack")));
		chara.lock()->AddComponent(new CollisionCapsule(Vector3(0.0f, 5.0f, 0.0f), 3.5f, Vector3(0.0f, 1.5f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), false));
	}
	{
		std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("chara", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(15.0f, 1.0f, 0.0f), Vector3(3.08f, 3.08f, 3.08f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("SmaphoMan.fbx"), false));
		chara.lock()->AddComponent(new Animator(new TestAnimationController("SmaphoMan|Idle")));
		chara.lock()->AddComponent(new CollisionCapsule(Vector3(0.0f, 5.0f, 0.0f), 3.5f, Vector3(0.0f, 1.5f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), false));
	}
	{
		std::weak_ptr<GameObject> chara2 = manager->CreateGameObject(new GameObject("chara", Tag::None, Layer::Default));
		chara2.lock()->AddComponent(new Transform(Vector3(-30.0f, 0.0f, 0.0f), Vector3(0.08f, 0.08f, 0.08f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara2.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("SuperRabit2.fbx"), false));
		chara2.lock()->AddComponent(new Animator(new TestAnimationController("SuperRabit2|Walk", 100.0f)));
		//chara2.lock()->AddComponent(new CollisionSphere(Vector3(0.0f, 5.0f, 0.0f), 3.5f, false));
		//chara2.lock()->AddComponent(new Rigidbody(0.1f,0.01f));
	}
	{
		std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("chara", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(30.0f, 3.0f, 0.0f), Vector3(3.08f, 3.08f, 3.08f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("olimar/olimar.fbx"), false));
		chara.lock()->AddComponent(new Animator(new TestAnimationController("olimar|run", 120.0f)));
	}
	{
		std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("Chicken", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(60.0f, 5.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new MeshRenderer(FilePathModel("olimar/olimar.fbx"), true));
	}
	{
		std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("Chicken", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(0.0f, 10.0f, -50.0f), Vector3(13.0f, 13.0f, 13.0f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new MeshRenderer(FilePathModel("Chicken/Chicken.fbx"), true));
	}
	{
		/*std::weak_ptr<GameObject> chara = manager->CreateGameObject(new GameObject("Chicken", Tag::None, Layer::Default));
		chara.lock()->AddComponent(new Transform(Vector3(30.0f, 10.0f, -50.0f), Vector3(13.0f, 13.0f, 13.0f), Quaternion(0.0f, 0.0f, 0.0f)));
		chara.lock()->AddComponent(new SkinMeshRenderer(FilePathModel("Chicken/Chicken_Skin.fbx"), false));
		chara.lock()->AddComponent(new Animator(new TestAnimationController("Chicken_Skin|Music", 30.0f)));*/
	}
	

	std::weak_ptr<GameObject> sea = manager->CreateGameObject(new GameObject("sea", Tag::None, Layer::Default));
	sea.lock()->AddComponent(new Transform(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 0.1f, 10.0f), Quaternion(0.0f, 0.0f, 0.0f)));
	sea.lock()->AddComponent(new MeshRenderer(FilePathModel("sea.fbx"), true));
	sea.lock()->AddComponent(new CollisionMesh(false));

	std::weak_ptr<GameObject> stage = manager->CreateGameObject(new GameObject("stage", Tag::None, Layer::Default));
	stage.lock()->AddComponent(new Transform(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.1f, 0.1f, 0.1f), Quaternion(0.0f, 0.0f, 0.0f)));
	stage.lock()->AddComponent(new MeshRenderer(FilePathModel("Stage.fbx"), true));
	stage.lock()->AddComponent(new CollisionMesh(false));
}

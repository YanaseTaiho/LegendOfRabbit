#include "GameScene.h"
#include "DirectX/Common.h"
#include "Scripts/test_move.h"
#include "Scripts/test_enemy.h"
#include "Scripts/Camera/CameraController.h"

#include "Scripts/AnimationController/RabitAnimationController.h"
#include "Scripts/AnimationController/TestAnimationController.h"

#include "Scripts/Actor/Player/PlayerActor.h"
#include "Scripts/Scene/GameSceneSystem.h"

void GameScene::Initialize()
{
	//std::weak_ptr<GameObject> light =
	//	CreateGameObject(new GameObject("Light", Tag::None,
	//		Vector3(0.0f, 100.0f, 0.0f),
	//		Vector3::one(),
	//		Quaternion(45.0f, 0.0f, 0.0f)))
	//	.lock()->AddLight(new DirectionalLight(
	//		Color(1.0f, 1.0f, 1.0f, 1.0f),		// ディフーズ
	//		Color(0.1f, 0.1f, 0.1f, 1.0f)));	// アンビエント


	//// プレイヤー
	//std::weak_ptr<GameObject> player =
	//	CreateGameObject(new GameObject("Player", Tag::Player,
	//		Vector3(0.0f, 5.0f, -5.0f),
	//		Vector3(0.025f, 0.025f, 0.025f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//	.lock()->SetRenderer(new SkinMeshRenderer(FilePathModel("SuperRabit.fbx"), false))
	//	.lock()->SetAnimator(new RabitAnimationController())
	//	.lock()->AddComponent(new PlayerActor())
	//	.lock()->AddCollision(SetType::World, new CollisionCapsule(
	//		Vector3(0.0f, 2.5f, 0.0f), 1.0f, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), false))
	//	.lock()->SetRigidbody(0.03f, 0.90f);

	//{
	//	/*CreateGameObject(new GameObject("testChara",
	//		Vector3(0.0f, 5.0f, 0.0f),
	//		Vector3(0.03f, 0.03f, 0.03f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new SkinMeshRenderer(FilePathModel("dancle.fbx"), false))
	//		.lock()->SetAnimator(new TestAnimationController("dancle|Attack"));*/
	//	CreateGameObject(new GameObject("testChara",
	//		Vector3(30.0f, 5.0f, -30.0f),
	//		Vector3(0.03f, 0.03f, 0.03f),
	//		Quaternion(0.0f, 90.0f, 0.0f)))
	//		.lock()->SetRenderer(new SkinMeshRenderer(FilePathModel("dancle.fbx"), false))
	//		.lock()->SetAnimator(new TestAnimationController("dancle|Swim"));

	//	CreateGameObject(new GameObject("testChara",
	//		Vector3(-30.0f, 5.0f, -25.0f),
	//		Vector3(2.0f, 2.0f, 2.0f),
	//		Quaternion(0.0f, 180.0f, 0.0f)))
	//		.lock()->SetRenderer(new SkinMeshRenderer(FilePathModel("spino.fbx"), false))
	//		.lock()->SetAnimator(new TestAnimationController("spino|ArmAttack"));

	//	/*CreateGameObject(new GameObject("testChara",
	//		Vector3(30.0f, 0.0f, -20.0f),
	//		Vector3(0.08f, 0.08f, 0.08f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new SkinMeshRenderer("Girl1.fbx", false))
	//		.lock()->SetAnimator(new TestAnimationController("Girl1|Dancing"));*/
	//	/*CreateGameObject(new GameObject("testChara",
	//		Vector3(-30.0f, 0.0f, -50.0f),
	//		Vector3(0.08f, 0.08f, 0.08f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new SkinMeshRenderer("Girl1.fbx", false))
	//		.lock()->SetAnimator(new TestAnimationController("Girl1|Dancing"));
	//	CreateGameObject(new GameObject("testChara",
	//		Vector3(0.0f, 0.0f, -100.0f),
	//		Vector3(0.08f, 0.08f, 0.08f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new SkinMeshRenderer("Girl1.fbx", false))
	//		.lock()->SetAnimator(new TestAnimationController("Girl1|Dancing"));*/
	//}

	//// カメラ
	//std::weak_ptr<GameObject> camera =
	//	CreateGameObject(new GameObject("MainCamera", Tag::MainCamera,
	//		Vector3(0.0f, 0.0f, -30.0f),
	//		Vector3::one(),
	//		Quaternion(0.0f, 0.0f, 0.0f))).lock()
	//	->SetCamera(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0.1f, 5000.0f);

	//std::weak_ptr<GameObject> rootCamera =
	//	CreateGameObject(new GameObject("CharacterCamera", Tag::None,
	//		Vector3(0.0f, 8.0f, -20.0f),
	//		Vector3::one(),
	//		Quaternion(30.0f, 0.0f, 0.0f)))
	//	.lock()->SetChild(camera, SetType::Local);


	//CreateGameObject(new GameObject("GameSystem", Tag::None))
	//	.lock()->AddComponent(
	//		new GameSceneSystem(
	//			player.lock()->GetComponent<PlayerActor>(),
	//			rootCamera.lock()->transform,
	//			camera.lock()->transform));


	//{
	//	// スカイドーム
	//	CreateGameObject(new GameObject("Sky",
	//		Vector3(0.0f, -500.0f, 0.0f),
	//		Vector3(15.0f, 15.0f, 15.0f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new MeshRenderer(FilePathModel("SkyDome.fbx"), false));
	//	// 海
	//	CreateGameObject(new GameObject("Sky",
	//		Vector3(0.0f, -300.0f, 0.0f),
	//		Vector3(15.0f, 15.0f, 15.0f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new MeshRenderer(FilePathModel("Sea.fbx")));
	//	// ステージ
	//	CreateGameObject(new GameObject("Stage",
	//		Vector3(0.0f, 0.0f, 0.0f),
	//		Vector3(0.1f, 0.1f, 0.1f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new MeshRenderer(FilePathModel("Stage.fbx")))
	//		.lock()->AddCollision(new CollisionMesh());

	//	CreateGameObject(new GameObject("Rex",
	//		Vector3(-100.0f, 40.0f, -100.0f),
	//		Vector3(0.3f, 0.3f, 0.3f),
	//		Quaternion(0.0f, 90.0f, 0.0f)))
	//		.lock()->SetRenderer(new MeshRenderer(FilePathModel("Rex.fbx")))
	//		.lock()->AddCollision(new CollisionMesh());

	//	CreateGameObject(new GameObject("Rabit",
	//		Vector3(-20.0f, 30.0f, -70.0f),
	//		Vector3(0.3f, 0.3f, 0.3f),
	//		Quaternion(90.0f, 180.0f, 0.0f)))
	//		.lock()->SetRenderer(new MeshRenderer(FilePathModel("SuperRabit.fbx")))
	//		.lock()->AddCollision(new CollisionMesh());

	//	CreateGameObject(new GameObject("Cube",
	//		Vector3(-50.0f, -40.0f, -20.0f),
	//		Vector3(0.3f, 0.3f, 0.3f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new MeshRenderer(FilePathModel("miku.fbx")))
	//		.lock()->AddCollision(new CollisionMesh());

	//	CreateGameObject(new GameObject("Cube",
	//		Vector3(0.0f, -60.0f, -20.0f),
	//		Vector3(1.5f, 0.3f, 1.5f),
	//		Quaternion(0.0f, 0.0f, 0.0f)))
	//		.lock()->SetRenderer(new MeshRenderer(FilePathModel("Cube.fbx")))
	//		.lock()->AddCollision(new CollisionMesh());
	//}
}

#include "manager.h"
#include "DirectX/Renderer/System/RendererSystem.h"

#include "DirectX/Texture/TextureManager.h"
#include "FrameWork/Animation/AnimationClipManager.h"

#include "FrameWork/Common.h"
#include "Scene/GameScene.h"
#include "Scene/TitleScene.h"
#include "Scene/Editor/EditorScene.h"

#include "DirectX/ImGui/imgui.h"
#include "DirectX/ImGui/imgui_impl_win32.h"
#include "DirectX/ImGui/imgui_impl_dx11.h"

//////////////////////////////////////////////////////////////////////////////
#include "../FrameWork/Component/Collision/Mesh/CollisionMeshInfoManager.h"
//////////////////////////////////////////////////////////////////////////////

#include <direct.h>
#include <time.h>

using namespace MyDirectX;

#if EDITOR_MODE
#define START_SCENE (EditorScene)
#else
#define START_SCENE (GameScene)
#endif

const std::string CManager::dataFolderName = "Data";
const std::string CManager::dataFolderPath = dataFolderName + "/";

void CManager::Init()
{
	// �����ݒ� 
	// rand�֐�������s���邱�Ƃŗ\���ł��Ȃ��l�ɂ���
	srand((unsigned int)time(0));
	rand(); rand(); rand(); rand(); rand();	

	// �f�[�^�p�̃t�H���_�쐬
	_mkdir(dataFolderName.c_str());

	Singleton<AudioClipManager>::Create();
	RendererSystem::Init();

	// �G�t�F�N�V�A�[�̏�����
	Singleton<EffekseerManager>::Create();
	Singleton<EffekseerManager>::Instance()->Initialize(
		RendererSystem::GetDevice()
		, RendererSystem::GetDeviceContext()
		, Singleton<AudioClipManager>::Instance()->GetXAudio2());
	// �G�t�F�N�g��ǂݍ���
	Singleton<EffekseerManager>::Instance()->Load();

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	// Gui���E�B���h�E�O�ɏo�邱�Ƃ�����
	ImGui_ImplWin32_Init(RendererSystem::GetWindow());
	ImGui_ImplDX11_Init(RendererSystem::GetDevice(), RendererSystem::GetDeviceContext());
	ImGui::StyleColorsDark();

	Input::Mouse::Initialize();
	Input::GamePad::Initialize();

	Singleton<TextureManager>::Create();
	Singleton<MaterialManager>::Create();
	Singleton<AnimationClipManager>::Create();
	Singleton<MeshManager>::Create();
	Singleton<SkinMeshManager>::Create();
	Singleton<GameObjectManager>::Create();

	Singleton<AudioClipManager>::Instance()->Load();
	Singleton<TextureManager>::Instance()->LoadTexture(dataFolderPath);
	Singleton<MaterialManager>::Instance()->LoadMaterial(dataFolderPath);
	Singleton<MeshManager>::Instance()->LoadMesh(dataFolderPath);
	Singleton<SkinMeshManager>::Instance()->LoadMesh(dataFolderPath);

	Singleton<SceneManager>::Create();
	Singleton<SceneManager>::Instance()->Initialize(new START_SCENE());
}

void CManager::Update()
{
	Input::Keyboad::Update();
	Input::Mouse::Update();
	Input::GamePad::Update();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Singleton<SceneManager>::Instance()->Update();
}

void CManager::Draw()
{
	RendererSystem::Begin();

	Singleton<SceneManager>::Instance()->Draw();

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// �E�B���h�E�O�ɏo�����߂̍X�V
//	ImGui::UpdatePlatformWindows();
//	ImGui::RenderPlatformWindowsDefault();

	RendererSystem::End();

	Singleton<SceneManager>::Instance()->SceneCheck();
}

void CManager::Uninit()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	//Singleton<SceneManager>::Instance()->SaveSceneData();

	Singleton<SkinMeshManager>::Instance()->SaveMesh(dataFolderPath);
	Singleton<MeshManager>::Instance()->SaveMesh(dataFolderPath);
	//Singleton<MaterialManager>::Instance()->SaveMaterial(dataFolderPath);
	Singleton<TextureManager>::Instance()->SaveTexture(dataFolderPath);

	Singleton<SkinMeshManager>::Release();
	Singleton<TextureManager>::Release();
	Singleton<MaterialManager>::Release();
	Singleton<AnimationClipManager>::Release();
	Singleton<MeshManager>::Release();
	Singleton<SkinMeshManager>::Release();

	Singleton<SceneManager>::Release();
	Singleton<GameObjectManager>::Release();

	/////////////////////////////////////
	CollisionMeshInfoManager::Release();
	///////////////////////////////////////

	// �G�t�F�N�V�A�[�j��
	Singleton<EffekseerManager>::Release();

	RendererSystem::Uninit();

	Singleton<AudioClipManager>::Release();

	Input::GamePad::Finalize();
}
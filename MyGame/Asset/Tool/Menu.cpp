#include "Menu.h"
#include "DirectX/Common.h"
#include "FrameWork/Common.h"
#include "Cereal/Common.h"
#include "Main/manager.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#include <commctrl.h>
#pragma comment (lib, "Comctl32.lib")

using namespace Tool;
using namespace Input;
using namespace FrameWork;
using namespace MyDirectX;

static char gFileName[MAX_PATH];
static std::string projectName;

Menu::Menu()
{
	DragAcceptFiles(RendererSystem::hWnd, TRUE);	// ドラッグ＆ドロップを許可

	///GetFullPathNameA()
	char Path[MAX_PATH + 1];
		// 実行ファイルの完全パスを取得
	if (0 != GetModuleFileName(NULL, Path, MAX_PATH)) 
	{
		char drive[MAX_PATH + 1], dir[MAX_PATH + 1], fname[MAX_PATH + 1], ext[MAX_PATH + 1];
		_splitpath(Path, drive, dir, fname, ext);
		projectName = fname;
		projectName += "\\";
	}
}

Menu::~Menu()
{
	DragAcceptFiles(RendererSystem::hWnd, FALSE);	// ドラッグ＆ドロップを禁止
}

//void Menu::Command(WORD param) const
//{
//	switch (LOWORD(param))
//	{
//	case ID_SAVE:	// 保存
//	{
//		std::shared_ptr<GameObject> object;
//		object = GameObjectManager::Instance()->CreateGameObject(new GameObject("test", Tag::None, Layer::Default)).lock();
//		object->AddComponent(new Transform(Vector3(0.0f,20.0f,50.0f), Vector3(0.1f,0.1f,0.1f)));
//		object->AddComponent(new MeshRenderer(FilePathModel("Baby.fbx"), true));
//
//		GameObject::Destroy(object, 30.0f);
//
//		std::stringstream ss;
//		{
//			cereal::JSONOutputArchive o_archive(ss);
//			o_archive(CEREAL_NVP(object));
//		}
//
//		std::ofstream ofs("test.json", std::ios::out);
//		ofs << ss.str();
//		ofs.close();
//	}
//		break;
//	case ID_OPNE:	// 開く
//
//		OPENFILENAME of;
//		memset(&of, 0, sizeof(of));
//		of.lStructSize = sizeof(of);
//		of.hwndOwner = RendererSystem::hWnd;
//		of.lpstrFilter = "JSONファイル(*.json)\0*.json\0バイナリファイル(*.binary)\0*.binary\0";
//		of.lpstrFile = gFileName;
//		of.nMaxFile = MAX_PATH;
//		of.Flags = OFN_PATHMUSTEXIST;
//		//of.lpstrDefExt = "json";
//		if (GetOpenFileName(&of))
//		{
//			GameObjectManager::Instance()->Instantiate(gFileName);
//		}
//
//		break;
//	case ID_EDIT:	// 終了
//		DestroyWindow(RendererSystem::hWnd);
//		break;
//	default:
//		break;
//	}
//}

void Menu::WinMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DROPFILES:	// ドラッグ＆ドロップ
	{
		UINT fileNum;
		HDROP drop = (HDROP)wParam;
		// ファイルの数を取得
		fileNum = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);

		for (UINT i = 0; i < fileNum; i++)
		{
			char fileName[MAX_PATH] = "";
			DragQueryFileA(drop, i, fileName, MAX_PATH);	// ドロップされたファイルの名前を取得

			// 相対パスに変換
			size_t pos;
			std::string file = fileName;
			pos = file.rfind(projectName);
			file = file.substr(pos + projectName.size(), file.size() - (pos + projectName.size()));

			if (pos != std::string::npos)
			{
				pos = file.find_last_of(".");
				std::string extension = file.substr(pos, file.size() - pos);

				// テクスチャ読み込み
				if (extension == ".jpg" || extension == ".png" || extension == ".jpeg")
				{
					Singleton<TextureManager>::Instance()->Load(file);
				}
				// メッシュ読み込み
				else if (extension == ".fbx")
				{
					Singleton<MeshManager>::Instance()->Load(file);
					Singleton<SkinMeshManager>::Instance()->Load(file);
				}
			}
		}

		DragFinish(drop);

		break;
	}
	}
}

void Menu::ImGui() const
{
	if (ImGui::Begin("Texture"))
	{
		Singleton<TextureManager>::Instance()->DrawImGui();
	}
	ImGui::End();
	if (ImGui::Begin("Material"))
	{
		const std::string materialId = "##MaterialManager";
		float winWidth = ImGui::GetWindowWidth();
		if (ImGui::Button(("Save" + materialId).c_str(), ImVec2(winWidth * 0.4f, 0)))
		{
			ImGui::OpenPopup("Save?");
		}
		ImGui::SameLine();
		if (ImGui::Button(("Load" + materialId).c_str(), ImVec2(winWidth * 0.4f, 0)))
		{
			ImGui::OpenPopup("Load?");
		}
		if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("final confirmation.");
			if (ImGui::Button("OK", ImVec2(120, 0))) 
			{
				Singleton<MaterialManager>::Instance()->SaveMaterial(CManager::dataFolderPath);
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Load?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("final confirmation.");
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				Singleton<MaterialManager>::Instance()->LoadMaterial(CManager::dataFolderPath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::Separator();

		Singleton<MaterialManager>::Instance()->DrawImGui();
	}
	ImGui::End();

	if (ImGui::Begin("MeshData"))
	{
		if (ImGui::CollapsingHeader("Mesh"))
		{
			Singleton<MeshManager>::Instance()->DrawImGui();
		}

		if (ImGui::CollapsingHeader("SkinMesh"))
		{
			Singleton<SkinMeshManager>::Instance()->DrawImGui();
		}
	}
	ImGui::End();

	if (ImGui::Begin("SceneData"))
	{
		Singleton<SceneManager>::Instance()->DrawImGui();
	}
	ImGui::End();

	if (ImGui::Begin("AnimationClip"))
	{
		Singleton<AnimationClipManager>::Instance()->DrawImGui();
	}
	ImGui::End();
}

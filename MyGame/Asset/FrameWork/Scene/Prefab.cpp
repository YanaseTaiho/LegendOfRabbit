#include "Prefab.h"
#include "../../FrameWork/Component/Transform/NodeTransform.h"
#include "../../Main/manager.h"
#include "../Common.h"

#include <cereal/archives/binary.hpp>
#include <fstream>
#include <direct.h>

static const char FileName[] = "Prefab";
static const char PopupPrefab[] = "PopupPrefab";

using namespace Input;

void Prefab::Load(std::string path)
{
	deletePathList.clear();
	prefabComponentList.clear();
	prefabGameObjectList.clear();
	prefabList.clear();

	// データ用のフォルダ作成
	std::string filePathName = path + std::string(FileName);
	_mkdir(filePathName.c_str());

	std::vector<std::string> fileArray;
	// データの名前配列を読み込み
	fileArray = FileLoader::GetFileListInFolder(path + FileName, "prefab");

	if (fileArray.size() == 0)
		return;

	// 名前配列からデータを読み込み
	for (auto & name : fileArray)
	{
		std::ifstream ifs(filePathName + "/" + name, std::ios::binary);
		if (ifs)
		{
			std::shared_ptr<GameObject> inData;
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(inData);
			RegisterObject(inData);
		}
		ifs.close();
	}
}

void Prefab::Save(std::string path)
{
	std::string filePathName = path + std::string(FileName);

	// 削除されたデータのファイルを消す
	for (std::string delPath : deletePathList)
	{
		std::string name = delPath + ".prefab";
		DeleteFile((filePathName + "/" + name).c_str());
	}
	deletePathList.clear();

	// データを一つずつ保存
	for (auto & pre : prefabList)
	{
		std::string name = pre->name + ".prefab";

		std::string fileName = path + FileName + "/" + name;
		std::ofstream ofs(fileName, std::ios::binary);
		{
			cereal::BinaryOutputArchive o_archive(ofs);
			o_archive(cereal::make_nvp(pre->name, pre));
		}
		ofs.close();
	}
}

void Prefab::RegisterObject(std::weak_ptr<GameObject> gameObject)
{
	auto transform = gameObject.lock()->transform.lock();
	auto parent = transform->GetParent();

	Translation3f translation = transform->translation;
	Scaling3f scaling = transform->scaling;
	Quaternion rotate = transform->rotate;
	// 一旦親を退避
	if (!parent.expired())
	{
		transform->parent.reset();
		/*Matrix4 localMat = transform->localMatrix;
		Vector3 pos = localMat.position();
		Vector3 sca = localMat.scale();
		transform->translation = Translation3f(pos.x, pos.y, pos.z);
		transform->scaling = Scaling3f(sca.x, sca.y, sca.z);
		transform->rotate = localMat.rotation();*/
	}

	// 一旦シリアル化
	std::stringstream ss;
	{
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(gameObject.lock());
	}

	// コピー後退避した親を再度セット
	if (!parent.expired())
	{
		transform->parent = parent;

		/*transform->translation = translation;
		transform->scaling = scaling;
		transform->rotate = rotate;*/
	}

	// シリアル化したものを新しくインスタンス生成
	std::shared_ptr<GameObject> object;
	{
		cereal::BinaryInputArchive i_archive(ss);
		i_archive(object);

		// プレハブに登録
		prefabList.emplace_back(object);

		// 子オブジェクトを含め全てをインスタンスリストに登録
		this->RegisterGameObject(object);
	}
}

void Prefab::RegisterInsertObject(std::weak_ptr<GameObject> gameObject, std::list<std::shared_ptr<GameObject>>::iterator & outItr)
{
	auto transform = gameObject.lock()->transform.lock();
	auto parent = transform->GetParent();

	Translation3f translation = transform->translation;
	Scaling3f scaling = transform->scaling;
	Quaternion rotate = transform->rotate;
	// 一旦親を退避
	if (!parent.expired())
	{
		transform->parent.reset();
		//Matrix4 localMat = transform->worldMatrix;
		//Vector3 pos = localMat.position();
		//Vector3 sca = localMat.scale();
		//transform->translation = Translation3f(pos.x, pos.y, pos.z);
		//transform->scaling = Scaling3f(sca.x, sca.y, sca.z);
		//transform->rotate = localMat.rotation();
	}

	// 一旦シリアル化
	std::stringstream ss;
	{
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(gameObject.lock());
	}

	// コピー後退避した親を再度セット
	if (!parent.expired())
	{
		transform->parent = parent;

		//transform->translation = translation;
		//transform->scaling = scaling;
		//transform->rotate = rotate;
	}

	// シリアル化したものを新しくインスタンス生成
	std::shared_ptr<GameObject> object;
	{
		cereal::BinaryInputArchive i_archive(ss);
		i_archive(object);

		// 指定の位置にプレハブに登録
		prefabList.insert(outItr, object);

		// 子オブジェクトを含め全てをインスタンスリストに登録
		this->RegisterGameObject(object);
	}
}

void Prefab::DeleteObject(std::shared_ptr<GameObject> gameObject)
{
	deletePathList.emplace_back(gameObject->name);

	// プレハブリストから削除
	for (auto itr = prefabList.begin(), end = prefabList.end(); itr != end; ++itr)
	{
		if ((*itr) != gameObject) continue;
		prefabList.erase(itr);
		break;
	}

	// インスタンスリストから削除
	DeleteGameObject(gameObject);
}

void Prefab::DrawImGui()
{
	const std::string strId = "##Prefab";
	const char popupName1[] = "Popup##PrefabEdit";
	static std::weak_ptr<GameObject> deleteObject;

	ImVec2 winSize = ImGui::GetWindowSize();
	winSize.x -= ImGui::GetCursorPosX();
	winSize.y -= ImGui::GetCursorPosY();
	ImGui::BeginChild("PrefabChild", winSize);

	for (auto prefab : prefabList)
	{
		std::string & name = prefab->name;
		ImGui::Bullet(); ImGui::SameLine();

		bool isSelect = prefab == selectObject.lock();
		if (ImGui::Selectable((name + strId).c_str(), isSelect))
		{
			if (isSelect)
				selectObject.reset();
			else
				selectObject = prefab;
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload(GAMEOBJECT, &prefab, sizeof(prefab));

			ImGui::Text(name.c_str());

			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered())
		{
			if (Mouse::IsTrigger(Mouse::Button::Right))
			{
				deleteObject = prefab;
				ImGui::OpenPopup(PopupPrefab);
			}
		}
	}

	if (ImGui::BeginPopup(PopupPrefab))
	{
		if (!deleteObject.expired())
		{
			if (ImGui::MenuItem("Delete"))
			{
				DeleteObject(deleteObject.lock());
				deleteObject.reset();
			}
		}

		ImGui::EndPopup();
	}

	ImGui::EndChild();

	// プレハブ登録
	static std::weak_ptr<GameObject> overwriteObject;
	static std::list<std::shared_ptr<GameObject>>::iterator outItr = prefabList.end();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(NodeTransform::NODE_TRANSFORM_LIST))
		{
			auto & payload_n = *(std::list<std::weak_ptr<NodeTransform>>*)payload->Data;
			overwriteObject = payload_n.front().lock()->transform.lock()->gameObject;
			// 同じ名前のオブジェクトが存在しているか確認
			if (CheckName(overwriteObject, overwriteObject.lock()->name, outItr))
			{
				RegisterObject(overwriteObject);
				overwriteObject.reset();
			}
			else
			{
				ImGui::OpenPopup("Override?");
			}
			
		}
	}

	// 同じプレハブの名前があったら上書きするかの確認
	if (outItr != prefabList.end() && ImGui::BeginPopupModal("Override?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text((*outItr)->name.c_str());
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			// 上書きするオブジェクトの位置に挿入
			RegisterInsertObject(overwriteObject, outItr);
			// 上書き前のオブジェクトを削除
			DeleteObject(*outItr);

			overwriteObject.reset();
			outItr = prefabList.end();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	// 編集ウィンドウ
	bool isOpen = !selectObject.expired();
	if (isOpen)
	{
		if (ImGui::Begin("PrefabEdit", &isOpen))
		{
			std::string beforeName = selectObject.lock()->name;
			selectObject.lock()->DrawImGui();
			if (beforeName != selectObject.lock()->name)
			{
				SetOriginalName(selectObject);	// オリジナルの名前か確認する

				// ファイルにも変更名を反映
				std::string filePathName = DATA_FOLDER_PATH + std::string(FileName);
				std::string oldName = filePathName + +"/" + beforeName + ".prefab";
				std::string finalName = filePathName + "/" + selectObject.lock()->name + ".prefab";
				rename(oldName.c_str(), finalName.c_str());
			}

			int cnt = 0;
			std::string componentLabel = "##PrefabComponent";
			std::string strId = componentLabel + std::to_string(cnt);
			for (auto & com : selectObject.lock()->components)
			{
				ImGui::Checkbox(("##" + com.lock()->ClassName() + strId).c_str(), &com.lock()->Enable());

				ImGui::SameLine();
				bool isOnMouse = false;
				if (ImGui::TreeNode((com.lock()->ClassName() + strId).c_str()))
				{
					isOnMouse = ImGui::IsItemHovered();

					if ((com.lock()->ClassName().c_str(), ""))
					{
						com.lock()->DrawImGui(cnt);
					}
					ImGui::TreePop();
				}
				else
				{
					isOnMouse = ImGui::IsItemHovered();
				}

				if (isOnMouse)
				{
					if (Mouse::IsTrigger(Mouse::Button::Right))
					{
						ImGui::OpenPopup(popupName1);
						if (typeid(*com.lock()) != typeid(Transform))
							selectComponent = com;
					}
				}

				cnt++;
			}

			if (!selectComponent.expired() && ImGui::BeginPopup(popupName1))
			{
				if (ImGui::MenuItem("Copy##Inspector"))
				{

				}
				if (ImGui::MenuItem("Paste##Inspector"))
				{

				}
				if (ImGui::MenuItem("Remove##Inspector"))
				{
					for (auto itr = selectObject.lock()->components.begin(), end = selectObject.lock()->components.end(); itr != end; ++itr)
					{
						if (itr->lock() != selectComponent.lock()) continue;
						selectObject.lock()->components.erase(itr);
						break;
					}
					DeletePrefabComponentList(selectComponent);
					selectComponent.reset();
				}
				ImGui::EndPopup();
			}
		}

		ImGui::End();

		if (!isOpen)
			selectObject.reset();
	}
}

std::weak_ptr<GameObject> Prefab::FindGameObject(std::string name)
{
	for (auto obj : prefabList)
	{
		if (obj->name == name)
		{
			return obj;
		}
	}
	return std::weak_ptr<GameObject>();
}

void Prefab::RegisterGameObject(const std::shared_ptr<GameObject>& object)
{
	// リストに登録
	prefabGameObjectList.emplace_back(object);
	// コンポーネントの登録
	for (const std::weak_ptr<Component> & component : object->components)
	{
		prefabComponentList.emplace_back(component);
	}

	// 子のオブジェクトも同様に登録
	for (auto & child : object->transform.lock()->GetChilds())
	{
		if (child.lock()->gameObject.expired())
			continue;

		this->RegisterGameObject(child.lock()->gameObject.lock());
	}
}

void Prefab::DeleteGameObject(std::weak_ptr<GameObject> object)
{
	// 子のオブジェクトも同様に削除
	for (auto & child : object.lock()->transform.lock()->GetChilds())
	{
		if (child.lock()->gameObject.expired())
			continue;

		this->DeleteGameObject(child.lock()->gameObject);
	}

	for (auto component : object.lock()->components)
		DeletePrefabComponentList(component);

	DeletePrefabObjectList(object);
}

void Prefab::DeletePrefabObjectList(std::weak_ptr<GameObject> object)
{
	for (auto itr = prefabGameObjectList.begin(), end = prefabGameObjectList.end(); itr != end; ++itr)
	{
		if ((*itr) != object.lock()) continue;
		prefabGameObjectList.erase(itr);
		return;
	}
}

void Prefab::DeletePrefabComponentList(std::weak_ptr<Component> component)
{
	for (auto itr = prefabComponentList.begin(), end = prefabComponentList.end(); itr != end; ++itr)
	{
		if ((*itr) != component.lock()) continue;
		prefabComponentList.erase(itr);
		return;
	}
}

void Prefab::SetOriginalName(std::weak_ptr<GameObject> & original)
{
	// 同じ名前がいなかったらそのままリターン
	if (CheckName(original, original.lock()->name))
		return;

	// 重複しなくなるまでカウントを増やす
	int cnt = 1;
	while (true)
	{
		if (CheckName(original, original.lock()->name + "(" + std::to_string(cnt) + ")"))
			break;
		cnt++;
	}
	original.lock()->name += "(" + std::to_string(cnt) + ")";
}

bool Prefab::CheckName(std::weak_ptr<GameObject> & mine, std::string checkName)
{
	for (auto & data : prefabList)
	{
		if (data == mine.lock()) continue;

		if (data->name == checkName)
			return false;
	}
	return true;
}

bool Prefab::CheckName(std::weak_ptr<GameObject>& mine, std::string checkName, std::list<std::shared_ptr<GameObject>>::iterator & outItr)
{
	for (auto itr = prefabList.begin(),end = prefabList.end();itr != end;++itr)
	{
		if (*itr == mine.lock()) continue;

		if ((*itr)->name == checkName)
		{
			outItr = itr;
			return false;
		}
	}
	return true;
}

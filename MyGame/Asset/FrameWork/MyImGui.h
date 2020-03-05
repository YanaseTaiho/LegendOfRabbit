#ifndef _MYIMGUI__H__
#define _MYIMGUI__H__

#include "Input/Mouse.h"
#include "Component/Transform/NodeTransform.h"
#include "GameObject/GameObject.h"
#include <functional>

namespace FrameWork
{
	namespace MyImGui
	{
		static bool DropTargetGameObject(std::weak_ptr<GameObject>& target, std::string id)
		{
			static std::weak_ptr<GameObject> * removeData = nullptr;
			const std::string strId = "##DropGameObject" + id;
			const std::string popUpName = "DropGameObject" + strId;

			float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();

			// データがないとき
			if (target.expired())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Button(("None" + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
				ImGui::PopStyleColor(4);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Button((target.lock()->name + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
				ImGui::PopStyleColor(4);

				if (ImGui::IsItemHovered())
				{
					if (Input::Mouse::IsTrigger(Input::Mouse::Button::Right))
					{
						removeData = &target;
						ImGui::OpenPopup(popUpName.c_str());
					}
				}
			}

			if (ImGui::BeginPopup(popUpName.c_str()))
			{
				if (removeData && !removeData->expired())
				{
					if (ImGui::MenuItem("Remove"))
					{
						removeData->reset();
						removeData = nullptr;
					}
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(NodeTransform::NODE_TRANSFORM_LIST))
				{
					auto & payload_n = *(std::list<std::weak_ptr<NodeTransform>>*)payload->Data;
					std::weak_ptr<GameObject> dropData = payload_n.front().lock()->transform.lock()->gameObject;
					if (target.expired() || (!target.expired() && target.lock() != dropData.lock()))
					{
						target = dropData;
						return true;
					}
				}
			}
			return false;
		}

		template<class T>
		static bool DropTargetComponent(std::weak_ptr<T>& target, std::string id)
		{
			static std::weak_ptr<T> * removeData = nullptr;
			const std::string strId = "##DropComponent" + id;
			const std::string popUpName = "DropComponent" + strId;

			float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();

			// データがないとき
			if (target.expired())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Button(("None" + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
				ImGui::PopStyleColor(4);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Button((target.lock()->gameObject.lock()->name + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
				ImGui::PopStyleColor(4);

				if (ImGui::IsItemHovered())
				{
					if (Input::Mouse::IsTrigger(Input::Mouse::Button::Right))
					{
						removeData = &target;
						ImGui::OpenPopup(popUpName.c_str());
					}
				}
			}

			if (ImGui::BeginPopup(popUpName.c_str()))
			{
				if (removeData && !removeData->expired())
				{
					if (ImGui::MenuItem("Remove"))
					{
						removeData->reset();
						removeData = nullptr;
					}
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(NodeTransform::NODE_TRANSFORM_LIST))
				{
					auto & payload_n = *(std::list<std::weak_ptr<NodeTransform>>*)payload->Data;
					std::weak_ptr<GameObject> dropData = payload_n.front().lock()->transform.lock()->gameObject;
					std::weak_ptr<T> dropComponent = dropData.lock()->GetComponent<T>();

					if (!dropComponent.expired())
					{
						if (target.expired() || (!target.expired() && target.lock() != dropComponent.lock()))
						{
							target = dropComponent;
							return true;
						}
					}
				}
			}
			return false;
		}

		// vectorとlistのサイズを変更出来るようにする関数
		template<typename Element>
		static void VectorEdit(std::string name, int id, std::vector<Element>& target, std::function<void(Element&, int)> func)
		{
			int size = (int)target.size();
			std::string strId = "##" + std::to_string(id);
			if (ImGui::TreeNode((name + strId).c_str()))
			{
				if (ImGui::InputInt(("Size" + strId).c_str(), &size))
				{
					if (size >= 0)
					{
						target.resize((size_t)size);
					}
				}

				int cnt = 0;
				for (auto & v : target)
				{
					ImGui::Text(("Element " + std::to_string(cnt)).c_str());
					ImGui::Indent();
					func(v, cnt);
					ImGui::Unindent();
					cnt++;
				}
				ImGui::TreePop();
			}
			
		}

		static void InputString(std::string label, std::string & str, std::uint32_t size)
		{
			std::uint32_t newSize = (size < (std::uint32_t)str.size()) ? (std::uint32_t)str.size() : size;
			char* inName = new char[newSize];
			ZeroMemory(inName, newSize);
			if (!str.empty())
				memcpy(inName, &str.front(), str.size());

			if (ImGui::InputText(label.c_str(), inName, newSize))
				str = inName;

			delete[] inName;
		}
	}
}

#endif // !_MYIMGUI__H__


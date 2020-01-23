#ifndef TOOLCAMERA_H_
#define TOOLCAMERA_H_

#include "FrameWork/Common.h"

#include "DirectX/ImGui/imgui.h"

class ToolCamera : public MonoBehaviour
{
public:
	
	void Update() override
	{
		using namespace Input;

		// ウィンドウの上にマウスがあるか
		bool onWindow = false;
		onWindow = ImGui::IsAnyWindowHovered();
		if (onWindow) return;

		Vector2 mouseMove;
		float mouseHwheel;
		Mouse::GetMove(&mouseMove);
		mouseHwheel = Mouse::GetHwheel();

		Vector3 pos = transform.lock()->GetWorldPosition();

		if (Mouse::IsPress(Mouse::Button::Center))
		{
			pos += (-transform.lock()->right() * mouseMove.x + transform.lock()->up() * mouseMove.y) * moveSpeed;
			transform.lock()->SetWorldPosition(pos);
		}
		else if (Mouse::IsPress(Mouse::Button::Right))
		{
			Quaternion rot = transform.lock()->GetWorldRotation();
			rot = Quaternion::AxisAngle(Vector3::up(), mouseMove.x * rotationSpeed) * rot;
			transform.lock()->SetWorldRotation(rot);
			rot = Quaternion::AxisAngle(transform.lock()->right(), mouseMove.y * rotationSpeed) * rot;
			transform.lock()->SetWorldRotation(rot);
		}

		pos += transform.lock()->forward() * mouseHwheel * wheelSpeed;
		transform.lock()->SetWorldPosition(pos);
	}
private:
	float moveSpeed = 0.3f;
	float rotationSpeed = 0.3f;
	float wheelSpeed = 0.04f;
};


#endif // !TOOLCAMERA_H_


#include "Mouse.h"

#define NOMINMAX
#include <windows.h>

#include "../../DirectX/Renderer/System/RendererSystem.h"
#include "DirectX/ImGui/imgui.h"

using namespace Input;
using namespace FrameWork;
using namespace MyDirectX;

Vector2 Mouse::curPos;
Vector2 Mouse::prePos;
Vector2 Mouse::moveVec;
float Mouse::wheelValue;
bool Mouse::isWheel;

bool Mouse::curState[(int)Button::Max]{};
bool Mouse::mouseState[(int)Button::Max]{};
bool Mouse::mouseStateTrigger[(int)Button::Max]{};
bool Mouse::mouseStateRelease[(int)Button::Max]{};

void Mouse::Initialize()
{
	POINT pos, pos1, pos2;
	// ウィンドウの左上の位置を取得 ＆ ウィンドウのマウス座標取得
	if (ScreenToClient(RendererSystem::hWnd, &pos1) &&
		GetCursorPos(&pos2))
	{
		pos.x = pos1.x + pos2.x + GetSystemMetrics(SM_CXDLGFRAME) * 2;
		pos.y = pos1.y + pos2.y + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
		curPos = Vector2((float)pos.x, (float)pos.y);
		prePos = curPos;
		moveVec = Vector2::zero();
	}
}

void Mouse::Update()
{
	if (!isWheel)
		wheelValue = 0.0f;
	isWheel = false;

	mouseStateTrigger[(int)Button::Left] = (mouseState[(int)Button::Left] ^ curState[(int)Button::Left]) & curState[(int)Button::Left];
	mouseStateTrigger[(int)Button::Right] = (mouseState[(int)Button::Right] ^ curState[(int)Button::Right]) & curState[(int)Button::Right];
	mouseStateTrigger[(int)Button::Center] = (mouseState[(int)Button::Center] ^ curState[(int)Button::Center]) & curState[(int)Button::Center];

	mouseStateRelease[(int)Button::Left] = (mouseState[(int)Button::Left] ^ curState[(int)Button::Left]) & mouseState[(int)Button::Left];
	mouseStateRelease[(int)Button::Right] = (mouseState[(int)Button::Right] ^ curState[(int)Button::Right]) & mouseState[(int)Button::Right];
	mouseStateRelease[(int)Button::Center] = (mouseState[(int)Button::Center] ^ curState[(int)Button::Center]) & mouseState[(int)Button::Center];

	mouseState[(int)Button::Left] = curState[(int)Button::Left];
	mouseState[(int)Button::Right] = curState[(int)Button::Right];
	mouseState[(int)Button::Center] = curState[(int)Button::Center];

	POINT pos;
	// ウィンドウからのマウス座標取得
	if (GetCursorPos(&pos)
		/*&& ScreenToClient(RendererSystem::hWnd, &pos)*/)
	{
		/*int windowFrameW = GetSystemMetrics(SM_CXDLGFRAME) / 2;
		int windowFrameH = windowFrameW + GetSystemMetrics(SM_CYCAPTION);
		pos.x = pos.x + windowFrameW * 4;
		pos.y = pos.y + windowFrameW * 4;*/
		
		// 何か押している時カーソル座標がウィンドウ外に出たらループする処理
		if (IsPressAny())
		{
			RECT wRect;
			//GetClientRect(RendererSystem::hWnd, &wRect);
			// PCのディスプレイサイズ取得
			HWND hDesktop = GetDesktopWindow();
			GetWindowRect(hDesktop, &wRect);
			//WINDOWINFO windowInfo;
			/*wRect.left = windowInfo.rcClient.left;
			wRect.right = windowInfo.rcClient.right;
			wRect.top = windowInfo.rcClient.top;
			wRect.bottom = windowInfo.rcClient.bottom;*/
			/*windowInfo.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(hDesktop, &windowInfo);*/
			//int x = GetSystemMetrics(SM_CXSCREEN);
			//int y = GetSystemMetrics(SM_CYSCREEN);
			/*wRect.left = windowInfo.rcClient.left;
			wRect.right = windowInfo.rcClient.right;
			wRect.top = windowInfo.rcClient.top;
			wRect.bottom = windowInfo.rcClient.bottom;*/

			LONG offset = 5;
			bool isOut = false;

			//wRect.left -= windowFrameW;
			//wRect.right += windowFrameW * 4;
			//wRect.top -= windowFrameH;
			//wRect.bottom += windowFrameW;

			wRect.right -= 1;
			wRect.bottom -= 1;

			if (pos.x <= wRect.left) {
				pos.x = wRect.right - offset; isOut = true;
			}
			if (pos.x >= wRect.right) {
				pos.x = wRect.left + offset; isOut = true;
			}
			if (pos.y <= wRect.top) {
				pos.y = wRect.bottom - offset; isOut = true;
			}
			if (pos.y >= wRect.bottom) {
				pos.y = wRect.top + offset; isOut = true;
			}

			if (isOut) {
				POINT pos2 = pos;
				//ClientToScreen(RendererSystem::hWnd, &pos2);
				SetCursorPos((int)pos2.x, (int)pos2.y);
				curPos = Vector2((float)pos.x, (float)pos.y);

				ImGuiIO & io = ImGui::GetIO();
				io.MouseDelta = ImVec2(0.0f, 0.0f);
				io.MousePos = ImVec2(curPos.x, curPos.y);
				io.MousePosPrev = ImVec2(curPos.x, curPos.y);
			}
		}

		prePos = curPos;
		curPos = Vector2((float)pos.x, (float)pos.y);
		moveVec = curPos - prePos;
	}
}

void Mouse::SetButtonDown(Button button)
{
	curState[(int)button] = true;
}

void Mouse::SetButtonUp(Button button)
{
	curState[(int)button] = false;
}

void Mouse::SetWheel(float value)
{
	wheelValue = value;
	isWheel = true;
}

void Mouse::GetPosition(Vector2 * pos)
{
	*pos = curPos;
}

Vector2 & Mouse::GetPosition()
{
	return curPos;
}

bool Mouse::IsPress(Button button)
{
	return mouseState[(int)button];
}

bool Mouse::IsPressAny()
{
	for (int i = 0; i < (int)Button::Max; i++)
	{
		if (mouseState[i]) return true;
	}
	return false;
}

bool Mouse::IsTriggerAny()
{
	for (int i = 0; i < (int)Button::Max; i++)
	{
		if (mouseStateTrigger[i]) return true;
	}
	return false;
}

bool Mouse::IsTrigger(Button button)
{
	return mouseStateTrigger[(int)button];
}

bool Mouse::IsRelease(Button button)
{
	return mouseStateRelease[(int)button];
}

void Mouse::GetMove(Vector2 * move)
{
	*move = moveVec;
}

Vector2 & Mouse::GetMove()
{
	return moveVec;
}

float Mouse::GetHwheel()
{
	return wheelValue;
}

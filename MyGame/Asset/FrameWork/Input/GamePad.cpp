#include "GamePad.h"

#define THUMB_VALUE (65535/4)

XINPUT_STATE GamePad::g_XInputState;				//XBOXコントローラーの状態を格納する
XINPUT_VIBRATION GamePad::g_Vibration;			//XBOXコントローラーのバイブレーション
bool GamePad::g_bEnable = false;

BYTE GamePad::g_aPadState[(int)Button::MAX];			// コントローラーの入力情報ワーク
BYTE GamePad::g_aPadStateTrigger[(int)Button::MAX];	// コントローラーのトリガー情報ワーク
BYTE GamePad::g_aPadStateRelease[(int)Button::MAX];	// コントローラーのリリース情報ワーク

void GamePad::Initialize(void)
{
	if (XInputGetState(0, &g_XInputState) == ERROR_SUCCESS)
	{
		g_bEnable = true;
	}
}

void GamePad::Update(void)
{
	if (!g_bEnable) { return; }

	XInputGetState(0, &g_XInputState);

	BYTE aPadState[(int)Button::MAX];

	{
		aPadState[(int)Button::DPAD_UP] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1 : 0;
		aPadState[(int)Button::DPAD_DOWN] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1 : 0;
		aPadState[(int)Button::DPAD_LEFT] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1 : 0;
		aPadState[(int)Button::DPAD_RIGHT] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1 : 0;
		aPadState[(int)Button::START] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 1 : 0;
		aPadState[(int)Button::BACK] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 1 : 0;
		aPadState[(int)Button::LEFT_SHOULDER] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 1 : 0;
		aPadState[(int)Button::RIGHT_SHOULDER] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 1 : 0;
		aPadState[(int)Button::A] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 1 : 0;
		aPadState[(int)Button::B] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 1 : 0;
		aPadState[(int)Button::X] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 1 : 0;
		aPadState[(int)Button::Y] = (g_XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 1 : 0;

		aPadState[(int)Button::THUMB_L_UP] = (g_XInputState.Gamepad.sThumbLY >= THUMB_VALUE) ? 1 : 0;
		aPadState[(int)Button::THUMB_L_DOWN] = (g_XInputState.Gamepad.sThumbLY <= -THUMB_VALUE) ? 1 : 0;
		aPadState[(int)Button::THUMB_L_LEFT] = (g_XInputState.Gamepad.sThumbLX <= -THUMB_VALUE) ? 1 : 0;
		aPadState[(int)Button::THUMB_L_RIGHT] = (g_XInputState.Gamepad.sThumbLX >= THUMB_VALUE) ? 1 : 0;
		aPadState[(int)Button::THUMB_R_UP] = (g_XInputState.Gamepad.sThumbRY >= THUMB_VALUE) ? 1 : 0;
		aPadState[(int)Button::THUMB_R_DOWN] = (g_XInputState.Gamepad.sThumbRY <= -THUMB_VALUE) ? 1 : 0;
		aPadState[(int)Button::THUMB_R_LEFT] = (g_XInputState.Gamepad.sThumbRX <= -THUMB_VALUE) ? 1 : 0;
		aPadState[(int)Button::THUMB_R_RIGHT] = (g_XInputState.Gamepad.sThumbRX >= THUMB_VALUE) ? 1 : 0;

		aPadState[(int)Button::LEFT_TRIGGER] = (g_XInputState.Gamepad.bLeftTrigger == 255) ? 1 : 0;
		aPadState[(int)Button::RIGHT_TRIGGER] = (g_XInputState.Gamepad.bRightTrigger == 255) ? 1 : 0;
	}

	for (int nCnKey = 0; nCnKey < (int)Button::MAX; nCnKey++)
	{
		// キートリガー・リリース情報を生成
		g_aPadStateTrigger[nCnKey] = (g_aPadState[nCnKey] ^ aPadState[nCnKey]) & aPadState[nCnKey];
		g_aPadStateRelease[nCnKey] = (g_aPadState[nCnKey] ^ aPadState[nCnKey]) & g_aPadState[nCnKey];


		// キープレス情報を保存
		g_aPadState[nCnKey] = aPadState[nCnKey];
	}
}

void GamePad::Finalize(void)
{
	if (!g_bEnable) { return; }

	XInputEnable(false);
}

bool GamePad::Enable()
{
	return g_bEnable;
}

bool GamePad::IsPress(Button nKey)
{
	if (!g_bEnable) { return false; }

	return (g_aPadState[(int)nKey]) ? true : false;
}

bool GamePad::IsTrigger(Button nKey)
{
	if (!g_bEnable) { return false; }

	return (g_aPadStateTrigger[(int)nKey]) ? true : false;
}

bool GamePad::IsRelease(Button nKey)
{
	if (!g_bEnable) { return false; }

	return (g_aPadStateRelease[(int)nKey]) ? true : false;
}

float GamePad::ThumbLX()
{
	if (!g_bEnable) { return 0.0f; }

	if (g_XInputState.Gamepad.sThumbLX < 0)
	{
		return (float)g_XInputState.Gamepad.sThumbLX / 32768.0f;
	}
	else
	{
		return (float)g_XInputState.Gamepad.sThumbLX / 32767.0f;
	}
}

float GamePad::ThumbLY()
{
	if (!g_bEnable) { return 0.0f; }

	if (g_XInputState.Gamepad.sThumbLY < 0)
	{
		return (float)g_XInputState.Gamepad.sThumbLY / 32768.0f;
	}
	else
	{
		return (float)g_XInputState.Gamepad.sThumbLY / 32767.0f;
	}
}

float GamePad::ThumbRX()
{
	if (!g_bEnable) { return 0.0f; }

	if (g_XInputState.Gamepad.sThumbRX < 0)
	{
		return (float)g_XInputState.Gamepad.sThumbRX / 32768.0f;
	}
	else
	{
		return (float)g_XInputState.Gamepad.sThumbRX / 32767.0f;
	}
}

float GamePad::ThumbRY()
{
	if (!g_bEnable) { return 0.0f; }

	if (g_XInputState.Gamepad.sThumbRY < 0)
	{
		return (float)g_XInputState.Gamepad.sThumbRY / 32768.0f;
	}
	else
	{
		return (float)g_XInputState.Gamepad.sThumbRY / 32767.0f;
	}
}

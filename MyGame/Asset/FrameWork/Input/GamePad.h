//=============================================================================
//
// XBOXコントローラー入力処理 [xinput.h]
//
//=============================================================================
#ifndef ___GAMEPAD__H__
#define ___GAMEPAD__H__

#pragma warning (disable : 4995)

#include <Windows.h>
#include <XInput.h>
#pragma comment (lib, "xinput.lib")

class GamePad
{
	enum class Button : int
	{
		DPAD_UP,
		DPAD_DOWN,
		DPAD_LEFT,
		DPAD_RIGHT,
		START,
		BACK,
		LEFT_SHOULDER,
		RIGHT_SHOULDER,
		A,
		B,
		X,
		Y,
		THUMB_L_UP,
		THUMB_L_DOWN,
		THUMB_L_LEFT,
		THUMB_L_RIGHT,
		THUMB_R_UP,
		THUMB_R_DOWN,
		THUMB_R_LEFT,
		THUMB_R_RIGHT,
		LEFT_TRIGGER,
		RIGHT_TRIGGER,
		MAX
	};

	static XINPUT_STATE g_XInputState;				//XBOXコントローラーの状態を格納する
	static XINPUT_VIBRATION g_Vibration;			//XBOXコントローラーのバイブレーション
	static bool g_bEnable;

	static BYTE g_aPadState[(int)Button::MAX];			// コントローラーの入力情報ワーク
	static BYTE g_aPadStateTrigger[(int)Button::MAX];	// コントローラーのトリガー情報ワーク
	static BYTE g_aPadStateRelease[(int)Button::MAX];	// コントローラーのリリース情報ワーク

	static void Initialize(void);
	static void Update(void);
	static void Finalize(void);

	static bool Enable();

	static bool IsPress(Button nKey);
	static bool IsTrigger(Button nKey);
	static bool IsRelease(Button nKey);

	static float ThumbLX();
	static float ThumbLY();
	static float ThumbRX();
	static float ThumbRY();
};

#endif //!___GAMEPAD__H__

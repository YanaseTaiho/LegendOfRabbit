
#include "main.h"
#include "manager.h"
#include "FrameWork/Time.h"
#include "Tool/Menu.h"

#include "../DirectX/ImGui/imgui_impl_win32.h"


using namespace FrameWork;
using namespace MyDirectX;

const char* CLASS_NAME = "DX11AppClass";
const char* WINDOW_NAME = "DX11";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};

	// ウィンドウメニューの設定
//	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// PCのディスプレイサイズ取得
	HWND hDesktop = GetDesktopWindow();
	WINDOWINFO windowInfo;
	windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hDesktop, &windowInfo);
	int pcWidth = windowInfo.rcWindow.right/* - windowInfo.rcWindow.left*/;
	int pcHeight = windowInfo.rcWindow.bottom /*- windowInfo.rcWindow.top*/;

	int screenWidth = (int)(pcWidth);
	int screenHeight = (int)(pcHeight);
	
//	int screenWidth = (SCREEN_WIDTH/* - GetSystemMetrics(SM_CXDLGFRAME) * 2)*/);
//	int screenHeight = (SCREEN_HEIGHT/* - GetSystemMetrics(SM_CXDLGFRAME) * 2*/ - GetSystemMetrics(SM_CYCAPTION));

	int screenX = (pcWidth - screenWidth) / 2;		// 中心に配置
	int screenY = (pcHeight - screenHeight) / 2;	// 少し上に配置

	// ウィンドウの作成
	RendererSystem::hWnd = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW /*& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX*/,	// ウィンドウサイズ固定＆最大化無効
//		WS_POPUPWINDOW,
		screenX,
		screenY,
		screenWidth,
		screenHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	/*SetWindowLong(RendererSystem::hWnd, GWL_STYLE, WS_POPUP | WS_BORDER);

	SetWindowPos(RendererSystem::hWnd, NULL,
		screenX, screenY,
		screenWidth,
		screenHeight,
		SWP_DRAWFRAME | SWP_NOZORDER);*/


	// 初期化処理(ウィンドウを作成してから行う)
	CManager::Init();

	// ウインドウの表示(初期化処理の後に行う)
	ShowWindow(RendererSystem::hWnd, nCmdShow);
	UpdateWindow(RendererSystem::hWnd);

	Time::Initialize(30);
	Time::SetTargetFPS(60);
	
	// メッセージループ
	MSG msg;
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			if(Time::Update())
			{
				// 更新処理
				CManager::Update();

				// 描画処理
				CManager::Draw();
			}
		}
	}

	Time::Finalize();

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	CManager::Uninit();

	return (int)msg.wParam;
}


//=============================================================================
// ウインドウプロシージャ
//=============================================================================
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	auto menu = Singleton<Tool::Menu>::Instance();
	if (menu) menu->WinMsg(hWnd, uMsg, wParam, lParam);

	switch(uMsg)
	{
	case WM_SIZE:
		if (RendererSystem::GetDevice() != NULL && wParam != SIZE_MINIMIZED)
		{
			RendererSystem::ResetRenderTarget(lParam);
		}
		return 0;

	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const RECT * sug = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL, sug->left, sug->top, sug->right - sug->left, sug->bottom - sug->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}

		break;

	// ウィンドウメニュー 
	/*case WM_COMMAND:

		Tool::Menu::Instance()->Command(LOWORD(wParam));

		break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		SetCapture(hWnd);	// カーソルが外に出ても反応するように設定
		Input::Mouse::SetButtonDown(Input::Mouse::Button::Left);
		break;
	}
	case WM_LBUTTONUP:
	{
		ReleaseCapture();	// カーソルが外に出ても反応しないように設定
		Input::Mouse::SetButtonUp(Input::Mouse::Button::Left);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		SetCapture(hWnd);	// カーソルが外に出ても反応するように設定
		Input::Mouse::SetButtonDown(Input::Mouse::Button::Right);
		break;
	}
	case WM_RBUTTONUP:
	{
		ReleaseCapture();	// カーソルが外に出ても反応しないように設定
		Input::Mouse::SetButtonUp(Input::Mouse::Button::Right);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		SetCapture(hWnd);	// カーソルが外に出ても反応するように設定
		Input::Mouse::SetButtonDown(Input::Mouse::Button::Center);
		break;
	}
	case WM_MBUTTONUP:
	{
		ReleaseCapture();	// カーソルが外に出ても反応しないように設定
		Input::Mouse::SetButtonUp(Input::Mouse::Button::Center);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		//DWORD fwKeys = GET_KEYSTATE_WPARAM(wParam);	// 同時に押されているキー情報
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);	// 回転量
		Input::Mouse::SetWheel((float)zDelta);
		break;
	}

	//case WM_CREATE:	// ファイルをクリック
	//{
	//	DragAcceptFiles(hWnd, TRUE);	// ドラッグ＆ドロップを許可
	//	break;
	//}
	//case WM_DROPFILES:	// ドラッグ＆ドロップ
	//{
	//	HDROP drop = (HDROP)wParam;
	//	char fileName[MAX_PATH];
	//	DragQueryFileA(drop, 0, fileName, MAX_PATH);	// ドロップされたファイルの名前を取得

	//	break;
	//}

	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



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

	// �E�B���h�E���j���[�̐ݒ�
//	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// PC�̃f�B�X�v���C�T�C�Y�擾
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

	int screenX = (pcWidth - screenWidth) / 2;		// ���S�ɔz�u
	int screenY = (pcHeight - screenHeight) / 2;	// ������ɔz�u

	// �E�B���h�E�̍쐬
	RendererSystem::hWnd = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW /*& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX*/,	// �E�B���h�E�T�C�Y�Œ聕�ő剻����
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


	// ����������(�E�B���h�E���쐬���Ă���s��)
	CManager::Init();

	// �E�C���h�E�̕\��(�����������̌�ɍs��)
	ShowWindow(RendererSystem::hWnd, nCmdShow);
	UpdateWindow(RendererSystem::hWnd);

	Time::Initialize(30);
	Time::SetTargetFPS(60);
	
	// ���b�Z�[�W���[�v
	MSG msg;
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			if(Time::Update())
			{
				// �X�V����
				CManager::Update();

				// �`�揈��
				CManager::Draw();
			}
		}
	}

	Time::Finalize();

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	CManager::Uninit();

	return (int)msg.wParam;
}


//=============================================================================
// �E�C���h�E�v���V�[�W��
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

	// �E�B���h�E���j���[ 
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
		SetCapture(hWnd);	// �J�[�\�����O�ɏo�Ă���������悤�ɐݒ�
		Input::Mouse::SetButtonDown(Input::Mouse::Button::Left);
		break;
	}
	case WM_LBUTTONUP:
	{
		ReleaseCapture();	// �J�[�\�����O�ɏo�Ă��������Ȃ��悤�ɐݒ�
		Input::Mouse::SetButtonUp(Input::Mouse::Button::Left);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		SetCapture(hWnd);	// �J�[�\�����O�ɏo�Ă���������悤�ɐݒ�
		Input::Mouse::SetButtonDown(Input::Mouse::Button::Right);
		break;
	}
	case WM_RBUTTONUP:
	{
		ReleaseCapture();	// �J�[�\�����O�ɏo�Ă��������Ȃ��悤�ɐݒ�
		Input::Mouse::SetButtonUp(Input::Mouse::Button::Right);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		SetCapture(hWnd);	// �J�[�\�����O�ɏo�Ă���������悤�ɐݒ�
		Input::Mouse::SetButtonDown(Input::Mouse::Button::Center);
		break;
	}
	case WM_MBUTTONUP:
	{
		ReleaseCapture();	// �J�[�\�����O�ɏo�Ă��������Ȃ��悤�ɐݒ�
		Input::Mouse::SetButtonUp(Input::Mouse::Button::Center);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		//DWORD fwKeys = GET_KEYSTATE_WPARAM(wParam);	// �����ɉ�����Ă���L�[���
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);	// ��]��
		Input::Mouse::SetWheel((float)zDelta);
		break;
	}

	//case WM_CREATE:	// �t�@�C�����N���b�N
	//{
	//	DragAcceptFiles(hWnd, TRUE);	// �h���b�O���h���b�v������
	//	break;
	//}
	//case WM_DROPFILES:	// �h���b�O���h���b�v
	//{
	//	HDROP drop = (HDROP)wParam;
	//	char fileName[MAX_PATH];
	//	DragQueryFileA(drop, 0, fileName, MAX_PATH);	// �h���b�v���ꂽ�t�@�C���̖��O���擾

	//	break;
	//}

	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


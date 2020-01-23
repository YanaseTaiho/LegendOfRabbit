#ifndef __DIRECTX__H__
#define __DIRECTX__H__

#define _CRT_SECURE_NO_WARNINGS

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#include <DirectXMath.h>
#include "d3dx11/Include/D3DX11.h"
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "xaudio2.lib")

#if defined(_DEBUG) || defined(DEBUG)

#pragma comment (lib, "d3dx11d.lib")

#else

#pragma comment (lib, "d3dx11.lib")

#endif

using namespace DirectX;

#endif // !__DIRECTX__H__


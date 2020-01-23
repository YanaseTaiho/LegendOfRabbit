#ifndef _DIRECTX_COMMON_H_
#define _DIRECTX_COMMON_H_

// ImGui
#include "ImGui/imgui.h"

#include "Renderer/System/RendererSystem.h"
#include "Texture/TextureManager.h"
#include "Material/MaterialManager.h"

// シェーダー
#include "Shader/ShaderFormatManager.h"
#include "Shader/MeshShader.h"
#include "Shader/SkinMeshShader.h"
#include "Shader/SkyDomeShader.h"

// レンダラー
#include "Renderer/MeshRenderer.h"
#include "Renderer/SkinMeshRenderer.h"
#include "Renderer/CanvasRenderer.h"

// カメラ
#include "Camera/DXCamera.h"

// ライト
#include "Light/DirectionalLight.h"

// デバッグ
#include "MeshData/LineMesh/DebugLine.h"


using namespace MyDirectX;

#endif //!_DIRECTX_COMMON_H_
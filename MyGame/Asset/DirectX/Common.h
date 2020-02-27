#ifndef _DIRECTX_COMMON_H_
#define _DIRECTX_COMMON_H_

// ImGui
#include "ImGui/imgui.h"

#include "Renderer/System/RendererSystem.h"
#include "Texture/TextureManager.h"
#include "Material/MaterialManager.h"

// シェーダー
#include "Shader/ShaderFormatManager.h"
#include "Shader/StandardShader.h"
#include "Shader/SkyDomeShader.h"
#include "Shader/ToonShader.h"
#include "Shader/EnviromentMappingShader.h"

// レンダラー
#include "Renderer/MeshRenderer.h"
#include "Renderer/SkinMeshRenderer.h"
#include "Renderer/CanvasRenderer.h"
#include "Renderer/LocusRenderer.h"
#include "Renderer/BillboardRenderer.h"

// カメラ
#include "Camera/DXCamera.h"

// ライト
#include "Light/DirectionalLight.h"

// エフェクト
#include "Effekseer/EffekseerSystem.h"

// デバッグ
#include "MeshData/LineMesh/DebugLine.h"


using namespace MyDirectX;

#endif //!_DIRECTX_COMMON_H_
#ifndef _DIRECTX_COMMON_H_
#define _DIRECTX_COMMON_H_

// ImGui
#include "ImGui/imgui.h"

#include "Renderer/System/RendererSystem.h"
#include "Texture/TextureManager.h"
#include "Material/MaterialManager.h"

// �V�F�[�_�[
#include "Shader/ShaderFormatManager.h"
#include "Shader/MeshShader.h"
#include "Shader/SkinMeshShader.h"
#include "Shader/SkyDomeShader.h"
#include "Shader/ToonMeshShader.h"

// �����_���[
#include "Renderer/MeshRenderer.h"
#include "Renderer/SkinMeshRenderer.h"
#include "Renderer/CanvasRenderer.h"

// �J����
#include "Camera/DXCamera.h"

// ���C�g
#include "Light/DirectionalLight.h"

// �f�o�b�O
#include "MeshData/LineMesh/DebugLine.h"


using namespace MyDirectX;

#endif //!_DIRECTX_COMMON_H_
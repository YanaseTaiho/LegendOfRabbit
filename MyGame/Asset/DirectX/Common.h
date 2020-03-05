#ifndef _DIRECTX_COMMON_H_
#define _DIRECTX_COMMON_H_

// ImGui
#include "ImGui/imgui.h"

#include "Renderer/System/RendererSystem.h"
#include "Texture/TextureManager.h"
#include "Material/MaterialManager.h"

// �V�F�[�_�[
#include "Shader/ShaderFormatManager.h"
#include "Shader/StandardShader.h"
#include "Shader/SkyDomeShader.h"
#include "Shader/ToonShader.h"
#include "Shader/EnviromentMappingShader.h"
#include "Shader/FarShader.h"

// �����_���[
#include "Renderer/MeshRenderer.h"
#include "Renderer/SkinMeshRenderer.h"
#include "Renderer/CanvasRenderer.h"
#include "Renderer/LocusRenderer.h"
#include "Renderer/BillboardRenderer.h"

// �J����
#include "Camera/DXCamera.h"

// ���C�g
#include "Light/DirectionalLight.h"

// �G�t�F�N�g
#include "Effekseer/EffekseerSystem.h"

// �f�o�b�O
#include "MeshData/LineMesh/DebugLine.h"


using namespace MyDirectX;

#endif //!_DIRECTX_COMMON_H_
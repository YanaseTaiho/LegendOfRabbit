#ifndef ___EFFEKSEER__MANAGER__H__
#define ___EFFEKSEER__MANAGER__H__

#include "../../FrameWork/Singleton.h"
#include "../../FrameWork/Matrix.h"
#include "../DirectX.h"

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>

#include <vector>
#include <functional>

#if _DEBUG
#pragma comment(lib, "VS2017WIN64/Debug/Effekseer.lib" )
#pragma comment(lib, "VS2017WIN64/Debug/EffekseerRendererDX11.lib" )
#pragma comment(lib, "VS2017WIN64/Debug/EffekseerSoundXAudio2.lib" )
#else
#pragma comment(lib, "VS2017WIN64/Release/Effekseer.lib" )
#pragma comment(lib, "VS2017WIN64/Release/EffekseerRendererDX11.lib" )
#pragma comment(lib, "VS2017WIN64/Release/EffekseerSoundXAudio2.lib" )
#endif

namespace MyDirectX
{
	class EffekseerSystem;

	enum class EffekseerType : int
	{
		Hit,
		MaxNum
	};

	static std::string enum_to_string(EffekseerType type)
	{
		switch (type)
		{
		case EffekseerType::Hit: return "Hit";
		}
		return "";
	}

	class EffekseerManager
	{
		friend FrameWork::Singleton<EffekseerManager>;
		EffekseerManager() {}
		~EffekseerManager() {}
	public:
		void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, IXAudio2 * audio);
		void Release();
		void Load();

		void Update();
		void Draw(const FrameWork::Matrix4 & viewMatrix, float _aspect, float _near, float _far);

		Effekseer::Handle Play(EffekseerType type, std::weak_ptr<EffekseerSystem> system);

		void RemoveHandle(Effekseer::Handle handle);

		Effekseer::Manager*	manager = nullptr;
	private:
		EffekseerRenderer::Renderer* renderer = nullptr;
		EffekseerSound::Sound* sound = nullptr;
		std::vector<Effekseer::Effect*> effects;
	};
}

#endif // !___EFFEKSEER__MANAGER__H__

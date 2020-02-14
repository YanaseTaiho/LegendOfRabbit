#include "EffekseerManager.h"
#include "EffekseerSystem.h"

using namespace MyDirectX;

constexpr int effectMaxCount = 2000;
static auto constexpr EffectPath = L"Asset/Art/Effect/";

static const wchar_t * effectPaths[(int)EffekseerType::MaxNum] =
{
	L"hit01.efk"
};

static std::list<std::weak_ptr<EffekseerSystem>> effekseerList;

void EffekseerManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, IXAudio2 * audio)
{
	// 描画用インスタンスの生成
	renderer = EffekseerRendererDX11::Renderer::Create(device, context, effectMaxCount);

	// エフェクト管理用インスタンスの生成
	manager = Effekseer::Manager::Create(effectMaxCount);

	// 描画用インスタンスから描画機能を設定
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());

	// 音再生用インスタンスの生成
	sound = EffekseerSound::Sound::Create(audio, 16, 16);

	// 音再生用インスタンスから再生機能を指定
	manager->SetSoundPlayer(sound->CreateSoundPlayer());

	// 音再生用インスタンスからサウンドデータの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	manager->SetSoundLoader(sound->CreateSoundLoader());
}

void EffekseerManager::Release()
{
	// エフェクトの解放
	for (int i = 0; i < (int)EffekseerType::MaxNum; i++)
	{
		if (effects[i])
			effects[i]->Release();
	}

	// 先にエフェクト管理用インスタンスを破棄
	manager->Destroy();

	// 次に音再生用インスタンスを破棄
	sound->Destroy();

	// 次に描画用インスタンスを破棄
	renderer->Destroy();
}

void EffekseerManager::Load()
{
	effects.resize((int)EffekseerType::MaxNum);
	for (int i = 0; i < (int)EffekseerType::MaxNum;i++)
	{
		// エフェクトの読込
		std::wstring path = std::wstring(EffectPath) + std::wstring(effectPaths[i]);
		Effekseer::Effect * effect = Effekseer::Effect::Create(manager, (const EFK_CHAR*)path.c_str());
		if(effect)
			effects[i] = effect;
		else
			effects[i] = nullptr;
	}
}

void EffekseerManager::Update()
{
	manager->Update();
}

void EffekseerManager::Draw(const FrameWork::Matrix4 & viewMatrix, float _aspect, float _near, float _far)
{
	// 投影行列を設定
	renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(1.0f, _aspect, _near, _far));

	// カメラ行列を設定
	Vector3 pos = viewMatrix.position();
	Vector3 at = pos + viewMatrix.forward();
	Vector3 up = viewMatrix.up();
	renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(
			::Effekseer::Vector3D(pos.x, pos.y, pos.z),
			::Effekseer::Vector3D(at.x, at.y, at.z),
			::Effekseer::Vector3D(up.x, up.y, up.z)));

	// エフェクトの描画開始処理を行う。
	renderer->BeginRendering();

	// エフェクトの描画を行う。
	manager->Draw();

	// エフェクトの描画終了処理を行う。
	renderer->EndRendering();
}

Effekseer::Handle EffekseerManager::Play(EffekseerType type, std::weak_ptr<EffekseerSystem> system)
{
	if (!effects[(int)type])
	{
		return (Effekseer::Handle)-1;
	}
	else
	{
		Effekseer::Handle handle = manager->Play(effects[(int)type], 0.0f, 0.0f, 0.0f);

		effekseerList.emplace_back(system);

		// エフェクト終了時に呼ばれる
		Effekseer::EffectInstanceRemovingCallback endCall = [](Effekseer::Manager* manager, Effekseer::Handle handle, bool isRemovingManager)->void
		{
			for (auto itr = effekseerList.begin(), end = effekseerList.end(); itr != end;++itr)
			{
				if (itr->lock()->GetHandle() == handle)
				{
					// エフェクト終了時の関数を呼ぶ
					itr->lock()->EndEffect();
					manager->StopEffect(handle);
					effekseerList.erase(itr);
					return;
				}
			}
		};

		manager->SetRemovingCallback(handle, endCall);
		return handle;
	}
}

void EffekseerManager::RemoveHandle(Effekseer::Handle handle)
{
	for (auto itr = effekseerList.begin(), end = effekseerList.end(); itr != end; ++itr)
	{
		if (itr->lock()->GetHandle() == handle)
		{
			manager->StopEffect(handle);
			effekseerList.erase(itr);
			return;
		}
	}
}

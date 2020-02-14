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
	// �`��p�C���X�^���X�̐���
	renderer = EffekseerRendererDX11::Renderer::Create(device, context, effectMaxCount);

	// �G�t�F�N�g�Ǘ��p�C���X�^���X�̐���
	manager = Effekseer::Manager::Create(effectMaxCount);

	// �`��p�C���X�^���X����`��@�\��ݒ�
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	// �`��p�C���X�^���X����e�N�X�`���̓Ǎ��@�\��ݒ�
	// �Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���B
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());

	// ���Đ��p�C���X�^���X�̐���
	sound = EffekseerSound::Sound::Create(audio, 16, 16);

	// ���Đ��p�C���X�^���X����Đ��@�\���w��
	manager->SetSoundPlayer(sound->CreateSoundPlayer());

	// ���Đ��p�C���X�^���X����T�E���h�f�[�^�̓Ǎ��@�\��ݒ�
	// �Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���B
	manager->SetSoundLoader(sound->CreateSoundLoader());
}

void EffekseerManager::Release()
{
	// �G�t�F�N�g�̉��
	for (int i = 0; i < (int)EffekseerType::MaxNum; i++)
	{
		if (effects[i])
			effects[i]->Release();
	}

	// ��ɃG�t�F�N�g�Ǘ��p�C���X�^���X��j��
	manager->Destroy();

	// ���ɉ��Đ��p�C���X�^���X��j��
	sound->Destroy();

	// ���ɕ`��p�C���X�^���X��j��
	renderer->Destroy();
}

void EffekseerManager::Load()
{
	effects.resize((int)EffekseerType::MaxNum);
	for (int i = 0; i < (int)EffekseerType::MaxNum;i++)
	{
		// �G�t�F�N�g�̓Ǎ�
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
	// ���e�s���ݒ�
	renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(1.0f, _aspect, _near, _far));

	// �J�����s���ݒ�
	Vector3 pos = viewMatrix.position();
	Vector3 at = pos + viewMatrix.forward();
	Vector3 up = viewMatrix.up();
	renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(
			::Effekseer::Vector3D(pos.x, pos.y, pos.z),
			::Effekseer::Vector3D(at.x, at.y, at.z),
			::Effekseer::Vector3D(up.x, up.y, up.z)));

	// �G�t�F�N�g�̕`��J�n�������s���B
	renderer->BeginRendering();

	// �G�t�F�N�g�̕`����s���B
	manager->Draw();

	// �G�t�F�N�g�̕`��I���������s���B
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

		// �G�t�F�N�g�I�����ɌĂ΂��
		Effekseer::EffectInstanceRemovingCallback endCall = [](Effekseer::Manager* manager, Effekseer::Handle handle, bool isRemovingManager)->void
		{
			for (auto itr = effekseerList.begin(), end = effekseerList.end(); itr != end;++itr)
			{
				if (itr->lock()->GetHandle() == handle)
				{
					// �G�t�F�N�g�I�����̊֐����Ă�
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

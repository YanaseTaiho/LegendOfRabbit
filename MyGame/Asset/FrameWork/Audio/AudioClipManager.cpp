#include "AudioClipManager.h"

static auto constexpr SoundPath = "Asset/Art/Sound/";

using namespace FrameWork;

struct audio_data
{
	const char * filePath;
	int sorceNum;
};

static audio_data LoadData[(int)AudioData::MaxNum] =
{
	// BGM
	{ "BGM/hateshinaimichi.wav", 1 },
	// SE
	{ "SE/swing1.wav", 5 },
	{ "SE/sword_chaki.wav", 1 },
	{ "SE/sword_pachin.wav", 4 },
	{ "SE/landing1.wav", 2 },
	{ "SE/RockOn.wav", 2 },
	{ "SE/RockCancel.wav", 2 },
	{ "SE/hit70.wav", 5 },
	{ "SE/hit70.wav", 2 },
	{ "SE/hit_wood.wav", 2 },
	{ "SE/hit_stone.wav", 2 },
	{ "SE/hit_iron.wav", 2 },
	{ "SE/foot_kusa.wav", 2 },
	{ "SE/hit70.wav", 2 },
	{ "SE/foot_soil.wav", 2 },
	{ "SE/foot_iron.wav", 2 },
	{ "SE/hyu.wav", 1 },
	{ "SE/down.wav", 1 },
	{ "SE/roll.wav", 1 },
	{ "SE/roll_hit.wav", 1 },
	{ "SE/bomb1.wav", 1 },
};

AudioClipManager::AudioClipManager()
{
	// COM������
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio����
	XAudio2Create(&xAudio, 0);

	// �}�X�^�����O�{�C�X����
	xAudio->CreateMasteringVoice(&masteringVoice);
}

AudioClipManager::~AudioClipManager()
{
	for (auto & clip : audioClipMap)
	{
		clip.second->Unload();
	}
	audioClipMap.clear();

	masteringVoice->DestroyVoice();

	xAudio->Release();

	CoUninitialize();
}

void AudioClipManager::Load()
{
	for (int i = 0; i < (int)AudioData::MaxNum; i++)
	{
		audioClipMap[i].reset(new AudioClip());
		std::string filePath = std::string(SoundPath) + std::string(LoadData[i].filePath);
		audioClipMap[i]->Load(xAudio, filePath, LoadData[i].sorceNum);
	}
}

void AudioClipManager::Play(AudioData data, float volume, bool loop)
{
	audioClipMap[(int)data]->Play(volume, loop);
}

void AudioClipManager::Stop(AudioData data)
{
	audioClipMap[(int)data]->Stop();
}

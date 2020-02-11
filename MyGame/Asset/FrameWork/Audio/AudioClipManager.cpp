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
	{ "SE/step07.wav", 1 },
	{ "SE/sword_pachin.wav", 4 },
	{ "SE/landing1.wav", 2 },
	{ "SE/extend1.wav", 2 },
	{ "SE/shrink1.wav", 2 },
	{ "SE/hit70.wav", 5 },
};

AudioClipManager::AudioClipManager()
{
	// COM初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio生成
	XAudio2Create(&xAudio, 0);

	// マスタリングボイス生成
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

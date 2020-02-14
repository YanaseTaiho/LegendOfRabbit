#ifndef _AUDIOCLIP_MANAGER_H_
#define _AUDIOCLIP_MANAGER_H_

#include "../Singleton.h"
#include "AudioClip.h"
#include <map>

auto constexpr a = "a";

namespace FrameWork
{
	enum class AudioData : int
	{
		// BGM
		BGM_StageBGM01,
		// SE
		SE_SwordSwing01,
		SE_SwordChakin,
		SE_SwordPachin,
		SE_Landing,
		SE_RockOn,
		SE_RockOnChancel,
		SE_Hit01,
		MaxNum
	};

	class AudioClipManager
	{
		friend Singleton<AudioClipManager>;
		AudioClipManager();
		~AudioClipManager();
	public:

		void Load();
		void Play(AudioData data, float volume = 1.0f, bool loop = false);
		void Stop(AudioData data);

		IXAudio2 * GetXAudio2() { return xAudio; }
		IXAudio2MasteringVoice * GetXAudio2MasteringVoice() { return masteringVoice; }
	private:
		IXAudio2 * xAudio;
		IXAudio2MasteringVoice * masteringVoice;

		std::map<int, std::unique_ptr<AudioClip>> audioClipMap;
	};
}

#endif // !_AUDIOCLIP_MANAGER_H_

#ifndef _AUDIOCLIP_H_
#define _AUDIOCLIP_H_

#include <xaudio2.h>
#include <vector>
#include <string>

namespace FrameWork
{
	class AudioClip
	{
	public:
		bool Load(IXAudio2* xAudio, const std::string & fileName, int sorceNum);
		void Unload();
		void Play(float volume = 1.0f, bool Loop = false);
		void Stop();

	private:
		std::vector<IXAudio2SourceVoice*> m_SourceVoice;
		BYTE* m_SoundData;
		int m_CurNum;	// ç≈å„Ç…égÇÌÇÍÇΩî‘çÜÇï€ë∂ÇµÇƒÇ®Ç≠

		int	m_Length;
		int	m_PlayLength;
	};
}

#endif // !_AUDIOCLIP_H_
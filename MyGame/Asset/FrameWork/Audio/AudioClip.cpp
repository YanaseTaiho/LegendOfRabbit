#include "AudioClip.h"
#include "../Mathf.h"
#include <assert.h>

using namespace FrameWork;

bool AudioClip::Load(IXAudio2* xAudio, const std::string & fileName, int sorceNum)
{
	// サウンドデータ読込
	WAVEFORMATEX wfx = { 0 };

	{
		HMMIO hmmio = NULL;
		MMIOINFO mmioinfo = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;


		hmmio = mmioOpen((LPSTR)fileName.c_str(), &mmioinfo, MMIO_READ);
		
		if (hmmio == NULL)
		{
			//MessageBox(NULL, "", "", MB_OK);
		}
			assert(hmmio);

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		buflen = datachunkinfo.cksize;
		m_SoundData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)m_SoundData, buflen);

		m_Length = readlen;
		m_PlayLength = readlen / wfx.nBlockAlign;


		mmioClose(hmmio, 0);
	}


	// サウンドソース生成
	m_SourceVoice.resize(sorceNum);
	for (auto & sorce : m_SourceVoice)
	{
		xAudio->CreateSourceVoice(&sorce, &wfx);
	}

	return true;
}


void AudioClip::Unload()
{
	for (auto & sorce : m_SourceVoice)
	{
		if (sorce)
		{
			sorce->Stop();
			sorce->DestroyVoice();
			sorce = NULL;
		}
	}

	if (m_SoundData)
	{
		delete[] m_SoundData;
		m_SoundData = nullptr;
	}
}

void AudioClip::Play(float volume, bool Loop )
{
	for (int i = 0, size = (int)m_SourceVoice.size();i < size;i++)
	{
		XAUDIO2_VOICE_STATE state;

		m_SourceVoice[i]->GetState( &state );

		if( state.BuffersQueued == 0 )
		{
			// バッファ設定
			XAUDIO2_BUFFER bufinfo;

			memset(&bufinfo,0x00,sizeof(bufinfo));
			bufinfo.AudioBytes = m_Length;
			bufinfo.pAudioData = m_SoundData;
			bufinfo.PlayBegin = 0;
			bufinfo.PlayLength = m_PlayLength;

			// ループ設定
			if( Loop )
			{
				bufinfo.LoopBegin = 0;
				bufinfo.LoopLength = m_PlayLength;
				bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
			}

			m_SourceVoice[i]->SubmitSourceBuffer( &bufinfo, NULL );

			// ボリューム設定
			m_SourceVoice[i]->SetVolume(volume);

			// 再生
			m_SourceVoice[i]->Start();

			m_CurNum = i;

			return;
		}
	}

	// 全部のデータが使われていた場合
	m_CurNum++;
	m_CurNum = Mathf::Loop(m_CurNum, 0, (int)m_SourceVoice.size() - 1);

	// 使われてるデータの中で最も古いのを使う
	m_SourceVoice[m_CurNum]->Stop();
	m_SourceVoice[m_CurNum]->FlushSourceBuffers();

	// バッファ設定
	XAUDIO2_BUFFER bufinfo;
	memset(&bufinfo, 0x00, sizeof(bufinfo));
	bufinfo.AudioBytes = m_Length;
	bufinfo.pAudioData = m_SoundData;
	bufinfo.PlayBegin = 0;
	bufinfo.PlayLength = m_PlayLength;

	// ループ設定
	if (Loop)
	{
		bufinfo.LoopBegin = 0;
		bufinfo.LoopLength = m_PlayLength;
		bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	m_SourceVoice[m_CurNum]->SubmitSourceBuffer(&bufinfo, NULL);
	// ボリューム設定
	m_SourceVoice[m_CurNum]->SetVolume(volume);
	// 再生
	m_SourceVoice[m_CurNum]->Start();
}



void AudioClip::Stop()
{
	for (auto & sorce : m_SourceVoice)
	{
		sorce->Stop();
	}
}



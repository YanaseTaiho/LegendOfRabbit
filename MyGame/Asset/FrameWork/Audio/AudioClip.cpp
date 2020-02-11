#include "AudioClip.h"
#include "../Mathf.h"
#include <assert.h>

using namespace FrameWork;

bool AudioClip::Load(IXAudio2* xAudio, const std::string & fileName, int sorceNum)
{
	// �T�E���h�f�[�^�Ǎ�
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


	// �T�E���h�\�[�X����
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
			// �o�b�t�@�ݒ�
			XAUDIO2_BUFFER bufinfo;

			memset(&bufinfo,0x00,sizeof(bufinfo));
			bufinfo.AudioBytes = m_Length;
			bufinfo.pAudioData = m_SoundData;
			bufinfo.PlayBegin = 0;
			bufinfo.PlayLength = m_PlayLength;

			// ���[�v�ݒ�
			if( Loop )
			{
				bufinfo.LoopBegin = 0;
				bufinfo.LoopLength = m_PlayLength;
				bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
			}

			m_SourceVoice[i]->SubmitSourceBuffer( &bufinfo, NULL );

			// �{�����[���ݒ�
			m_SourceVoice[i]->SetVolume(volume);

			// �Đ�
			m_SourceVoice[i]->Start();

			m_CurNum = i;

			return;
		}
	}

	// �S���̃f�[�^���g���Ă����ꍇ
	m_CurNum++;
	m_CurNum = Mathf::Loop(m_CurNum, 0, (int)m_SourceVoice.size() - 1);

	// �g���Ă�f�[�^�̒��ōł��Â��̂��g��
	m_SourceVoice[m_CurNum]->Stop();
	m_SourceVoice[m_CurNum]->FlushSourceBuffers();

	// �o�b�t�@�ݒ�
	XAUDIO2_BUFFER bufinfo;
	memset(&bufinfo, 0x00, sizeof(bufinfo));
	bufinfo.AudioBytes = m_Length;
	bufinfo.pAudioData = m_SoundData;
	bufinfo.PlayBegin = 0;
	bufinfo.PlayLength = m_PlayLength;

	// ���[�v�ݒ�
	if (Loop)
	{
		bufinfo.LoopBegin = 0;
		bufinfo.LoopLength = m_PlayLength;
		bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	m_SourceVoice[m_CurNum]->SubmitSourceBuffer(&bufinfo, NULL);
	// �{�����[���ݒ�
	m_SourceVoice[m_CurNum]->SetVolume(volume);
	// �Đ�
	m_SourceVoice[m_CurNum]->Start();
}



void AudioClip::Stop()
{
	for (auto & sorce : m_SourceVoice)
	{
		sorce->Stop();
	}
}



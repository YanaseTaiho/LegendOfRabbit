/*================================================================

	DebugPrintf������Ďg����[debugPrintf.h]
												Author : Taiho Yanase
												Date   : 2018/4/25
================================================================*/

#ifndef _DEBUG_PRINTF_H_
#define _DEBUG_PRINTF_H_

#include<windows.h>
#include<stdio.h>


//�f�o�b�N�pprintf...VisualStudio�̏o�̓E�B���h�E�ɏo�͂����
inline void DebugPrintf(const char* pFormat, ...)
{
	//����ȕ��ɏ�����Release�r���h���̓r���h����Ȃ��͈͐ݒ�ł���
#if defined(_DEBUG) || defined(DEBUG)
	va_list argp;
	char buf[256];	//�o�b�t�@�͑傫�߂Ɏ��܂��傤
	va_start(argp, pFormat);
	vsprintf_s(buf, 256, pFormat, argp);	//�o�b�t�@�ʂƑ����������킹�邱��
	va_end(argp);
	OutputDebugStringA(buf);
#endif	// _DEBUG || DEBUG
}

#endif	//DEBUG_PRINTF_H
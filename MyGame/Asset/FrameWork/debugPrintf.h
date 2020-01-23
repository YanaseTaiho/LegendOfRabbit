/*================================================================

	DebugPrintfを作って使おう[debugPrintf.h]
												Author : Taiho Yanase
												Date   : 2018/4/25
================================================================*/

#ifndef _DEBUG_PRINTF_H_
#define _DEBUG_PRINTF_H_

#include<windows.h>
#include<stdio.h>


//デバック用printf...VisualStudioの出力ウィンドウに出力される
inline void DebugPrintf(const char* pFormat, ...)
{
	//こんな風に書くとReleaseビルド時はビルドされない範囲設定できる
#if defined(_DEBUG) || defined(DEBUG)
	va_list argp;
	char buf[256];	//バッファは大きめに取りましょう
	va_start(argp, pFormat);
	vsprintf_s(buf, 256, pFormat, argp);	//バッファ量と第二引数を合わせること
	va_end(argp);
	OutputDebugStringA(buf);
#endif	// _DEBUG || DEBUG
}

#endif	//DEBUG_PRINTF_H
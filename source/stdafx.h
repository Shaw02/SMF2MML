// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <tchar.h>
#include <search.h>

using namespace std;


/****************************************************************/
/*																*/
/*			プロトタイプ宣言									*/
/*																*/
/****************************************************************/
//●外部宣言	アセンブリ言語の関数を呼ぶには、"C"（大文字）だと明言する。
//				C++のままだと、ラベル生成に余計な文字が付く。
extern "C"	void	print_help();
extern "C"	void	endian_b2l_W(unsigned __int16 *iValue);
extern "C"	void	endian_b2l_D(unsigned __int32 *Value);
//extern "C"	int		NoteScaleOut(FILE *stream,int iScale,char cNote);

#include "targetver.h"
#include "FileOp.h"
#include "Option.h"
#include "smfMThd.h"
#include "smfEvent.h"
#include "smfMeta.h"
#include "unMMLcompiler.h"


// TODO: プログラムに必要な追加ヘッダーをここで参照してください。


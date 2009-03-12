// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"


#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>						//ファイル・メモリ操作とか
#include <search.h>

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
extern "C"	int		NoteScaleOut(FILE *stream,int iScale,int iNote);

/****************************************************************/
/*																*/
/*			構造体定義											*/
/*																*/
/****************************************************************/

//ファイル操作用
typedef struct {
				char		name[128];
				FILE *		stream;
				size_t		size;
} FileInfo;

//オプションスイッチ
typedef struct {
				FileInfo	SMF;
				FileInfo	MML;
				char		fHelp;
} OPSW;

//MIDIヘッダー
typedef struct {
	char					strings[4];	//MThd
	unsigned	__int32		size;		//ヘッダーのサイズ
	unsigned	__int16		format;		//Format
	unsigned	__int16		track;		//トラック数
	unsigned	__int16		timebase;	//タイムベース
} SMF_MThd;

//拍子
typedef struct {
	unsigned	__int64		iTicksBT;		//拍子記号がある時間
				char		cKey;			//調（±0＝C）
	unsigned	char		cmi;			//0:Major / 1:Minor
	unsigned	int			iTicksOne;		//１小節のチック数
} Rhythm;

//逆コンパイル用
typedef struct {
				SMF_MThd	SMF_Header;		//MThdヘッダーを入れておく構造体

	//トラック制御用
	unsigned	long		lOffsetTR;		//コンパイル中のトラックの先頭オフセット
	unsigned	__int32		iTrackSize;		//MTrkのサイズ
				int			iTrack;			//逆コンパイル中のトラック
				int			iChannel;		//逆コンパイル中のチャンネル
				char		flagEoT;		//End of Trackのフラグ
				char		flagCh;			//CH命令を出力したか？

	//音長関係＆MML整形用
	unsigned	int			cntBT;			//カウント
	unsigned	int			defBT;			//拍子の定義数
				Rhythm		BT[255];		//拍子定義
	unsigned	__int64		iTotalTicks;	//今までのチック数
	unsigned	__int64		iTotalTicksChk;	//今までのチック数
	unsigned	__int32		iTicks;			//現在のチック数
	unsigned	int			iTicksBar;		//バーを出すまでのカウンター	
	unsigned	int			iTicksCR;		//改行するまでのカウンター	

	//逆コンパイルしているイベント
	unsigned	char		cStatus;		//ステータス
	unsigned	char		cData1;			//データ１
	unsigned	char		cData2;			//データ２
	unsigned	__int32		iSize;			//サイズ

	//逆コンパイル
	unsigned	char		cNote;			//NoteOn中のノート番号（0xFF＝休符）
	unsigned	char		cVelo;			//NoteOnするヴェロシティ
				int			flagNote;		//現在の出力状況	0:休符 / 1:NoteOn
	unsigned	char		cRunningStatus;	//前に出力したステータス
				int			iGate;			//設定したゲートタイム
	unsigned	int			iLength;		//設定した音長
				int			iOctave;		//設定したオクターブ
	unsigned	char		cVelocity;		//設定したヴェロシティー
} unMMLinfo;

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。


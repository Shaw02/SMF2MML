
#pragma once

/****************************************************************/
/*																*/
/*			クラス定義											*/
/*																*/
/****************************************************************/
//MIDI情報
class  SMF_MThd{
public:
	//Mthd
	char					strings[4];	//MThd
	unsigned	__int32		size;		//ヘッダーのサイズ
	unsigned	__int16		format;		//Format
	unsigned	__int16		track;		//トラック数
	unsigned	__int16		timebase;	//タイムベース

				FileOp*		SMF;

		SMF_MThd(FileOp* ptSMF);
};

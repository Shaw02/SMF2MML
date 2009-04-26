
#pragma once

/****************************************************************/
/*																*/
/*			構造体定義											*/
/*																*/
/****************************************************************/


/****************************************************************/
/*																*/
/*			クラス定義											*/
/*																*/
/****************************************************************/
//逆コンパイル用
class unMMLcompiler {
private:
				OPSW*			OptionSW;	//
				FileOp*			MML;		//
				FileOp*			SMF;		//

public:
				SMF_MetaEvent	SMF_Meta;	//メタイベントの情報
				SMF_event		SMF_Event;	//SMFのイベント
				SMF_MThd		SMF_Header;	//MThdヘッダーを入れておく構造体

private:		//トラック制御用
				long		lOffsetTR;		//コンパイル中のトラックの先頭オフセット
	unsigned	__int32		iTrackSize;		//MTrkのサイズ
				int			iTrack;			//逆コンパイル中のトラック
				int			iChannel;		//逆コンパイル中のチャンネル

private:		//音長関係
	unsigned	__int32		iTicks;			//現在のチック数
	unsigned	__int64		iTotalTicks;	//今までのチック数

public:			//MML整形用
	unsigned	__int64		iTotalTicksChk;	//今までのチック数
	unsigned	int			iTicksBar;		//バーを出すまでのカウンター	
	unsigned	int			iTicksCR;		//改行するまでのカウンター	
	unsigned	int			nowBeatTime;	//全音符のticks数

private:		//逆コンパイル
				char		flagCh;			//CH命令を出力したか？
	unsigned	char		cNote;			//NoteOn中のノート番号（0xFF＝休符）
	unsigned	char		cVelo;			//NoteOnするヴェロシティ
				char		flagNote;		//現在の出力状況	0:休符 / 1:NoteOn
				char		cOctave;		//設定したオクターブ
	unsigned	char		cVelocity;		//設定したヴェロシティー
//				int			iGate;			//設定したゲートタイム
//	unsigned	int			iLength;		//設定した音長


public:
		unMMLcompiler(OPSW* Option):
			MML(Option->MML),
			SMF(Option->SMF),
			OptionSW(Option),
			SMF_Event(Option),
			SMF_Header(SMF)
		{
		};

	void	uncompile();

private:

	//General
	void	OutputHex();
	void	OutputStrings();
	void	EncodeVariable(unsigned	__int32	var);

	void	initTR();
	void	initCH();

	//Decode: MIDI Header
	void	MetaSearch();
	void	Header();

	//Decode: Length
	void	OutputBarCr();
	int		Ticks2Length(int iTicksL);
	void	Ticks2LengthEx(int iTicksL);
	void	OutputTicks(/*unsigned	__int32		iTicksL*/);

	//Decode: Note
	char	NoteScas(char *cScale, char caNote);
	void	OutputNoteEx();

	//Decode: Note Length
	void	OutputNote();

	//Decode: MIDI Channel Voice Message
	void	DecodeNote();
	void	DecodeControlChange();

	//Decode: MIDI Event
	void	DecodeChannelVoiceMessage();
	void	DecodeSystemExcusiveMessage();
	void	DecodeSystemCommonMessage();
	void	DecodeMetaEvent();

	//main routine
	void	Decode1Command();

};


#pragma once

/****************************************************************/
/*																*/
/*			クラス定義											*/
/*																*/
/****************************************************************/

class SMF_event{
private:
				OPSW*		OptionSW;		//オプションスイッチ
				FileOp*		SMF;			//SMFファイル

public:
	//逆コンパイルしているイベント
	unsigned	char		cRunningStatus;	//前に出力したステータス
	unsigned	char		cStatus;		//ステータス
	unsigned	char		cData1;			//データ１
	unsigned	char		cData2;			//データ２
	unsigned	__int32		iSize;			//サイズ

				char		flagEoT;		//End of Trackのフラグ


public:
					SMF_event(OPSW* option):	//初期化
						OptionSW(option),
						SMF(option->SMF)
					{};

unsigned	__int32	DecodeVariable();		//可変長の読み込み

unsigned	__int32	EventRead();			//MIDIイベントの読み込み

			void	DataRead(char* ptData){	//サイズ分読み込み
						SMF->read(ptData, iSize);
					};

unsigned	char	Read(){					//1Byte読み込み
						unsigned	char	cData;
						SMF->read((char*)&cData, 1);
						return(cData);
					};

			void	Seek(){					//サイズ分シーク
						SMF->StreamPointerAdd(iSize);
					};
};

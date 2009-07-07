
#include "stdafx.h"

/****************************************************************/
/*																*/
/*			プロトタイプ宣言									*/
/*																*/
/****************************************************************/
extern	void not_smf(char *filename, char *stMsg);
extern	void StreamPointerAdd(FILE *stream,__int32 iSize);
extern	void OutputHex(OPSW *OptionSW,__int32 iSize);
extern	void OutputStrings(OPSW *OptionSW,__int32 iSize);
extern	void Ticks2LengthEx(OPSW *OptionSW,unMMLinfo *unMML, int iTicks);
extern	void OutputNoteEx(OPSW *OptionSW,unMMLinfo *unMML);
extern	void OutputNote(OPSW *OptionSW,unMMLinfo *unMML);

//##############################################################
//		汎用サブルーチン
//##############################################################
//==============================================================
//		可変長デコード
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//	●返値
//			__int32	iDeltaTime	デルタタイム
//==============================================================
unsigned __int32	DecodeVariable(OPSW *OptionSW){

	//----------------------------------
	//■Local 変数
	unsigned	__int32		iDeltaTime;		//読み込んだ可変長数値
	unsigned	char		cFread;			//読み込み用変数
	unsigned	int			count;			//読み込み回数カウント用

	//----------------------------------
	//■デルタタイム読み込み
	iDeltaTime=0;	//初期値は0
	count=4;		//最大4Byte。従って、読み込みは４回まで
	do{
		fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
		iDeltaTime=(iDeltaTime<<7)+((unsigned __int32)cFread&0x7F);
		count--;
	} while((count>0)&&(cFread&0x80));

	return(iDeltaTime);

};
//==============================================================
//		イベントの読み込み
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW		オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML		逆MML用構造体
//	●返値
//	unsigned	__int32	iDeltaTime	デルタタイム
//			unMML->cRunningStatus	今回のステータス・バイト
//			unMML->cStatus			ステータス・バイト
//			unMML->cData1			データ・バイト１
//			unMML->cData2			データ・バイト２
//			unMML->iSize			データのサイズ
//==============================================================
unsigned	__int32	EventRead(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	unsigned	__int32		iDeltaTime;		//デルタタイム

	unMML->iSize=0;	//これは、デフォルト０

	//----------------------------------
	//■デルタタイムの読み込み（関数の返値はこれにする。）
	iDeltaTime=DecodeVariable(OptionSW);

	//----------------------------------
	//■MIDIイベントの処理
	//まず、1Byte読み込み
	fread((void *)&unMML->cStatus,sizeof(char),1,OptionSW->SMF.stream);
	//ステータスだった場合（メタイベント・システムエクスクルーシブの可能性有り）
	if(unMML->cStatus&0x80){
		unMML->cData1=0xFF;	//先ずは、フラグとしてセット
	//データだった場合、ランニングステータスの適用
	} else {
		unMML->cData1=unMML->cStatus;
		unMML->cStatus=unMML->cRunningStatus;
	};

	//----------------------
	//●コマンド別処理
	//◆Channel Voice Message？
	if(unMML->cStatus<0xF0){

		unMML->cRunningStatus=unMML->cStatus;	//ランニングステータスの更新

		//ランニングステータスが適用されてなかったら、最初のビットを読む
		if(unMML->cData1==0xFF){
			fread((void *)&unMML->cData1,sizeof(char),1,OptionSW->SMF.stream);
		};
		//データ2がある場合、データを読む。
		if(((unMML->cStatus>=0x80)&&(unMML->cStatus<=0xBF))||((unMML->cStatus>=0xE0)&&(unMML->cStatus<=0xEF))){
			fread((void *)&unMML->cData2,sizeof(char),1,OptionSW->SMF.stream);
		};

	//--------------
	//◆System Excusive Message
	//◆System Common Message
	//◆Meta Event
	} else {

		//メッセージ別処理
		switch(unMML->cStatus){
			//--------------
			//システム・エクスクルーシブ・メッセージ
			//システム・コモン・メッセージ
			case(0xF0):
			case(0xF7):
				unMML->iSize=DecodeVariable(OptionSW);
				break;
			//--------------
			//メタ・イベント
			case(0xFF):
				fread((void *)&unMML->cData1,sizeof(char),1,OptionSW->SMF.stream);
				unMML->iSize=DecodeVariable(OptionSW);
				if(unMML->cData1==0x2F){
					unMML->flagEoT=1;	//EoTだけは強制チェック
				};
				break;
			//--------------
			//その他
			default:
				not_smf(OptionSW->SMF.name,"無効なMIDIイベントを検出しました。\n");
				break;
		};
	};

	return(iDeltaTime);

};
/****************************************************************/
/*																*/
/*				逆コンパイル　デコード用・サブルーチン			*/
/*																*/
/****************************************************************/
//##############################################################
//		Channel Voice Message 内のイベント
//##############################################################
//==============================================================
//		ノート・ｏｎ／ｏｆｆのデコード
//--------------------------------------------------------------
//	●引数
//					OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//					unMMLinfo	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void NoteDecode(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数

	unsigned	__int32	iLength=0;	//音長
	//ステータスバイト・データバイトを保存
				char	cTempEoT	= unMML->flagEoT;		//End of Trackのフラグ
	unsigned	char	cTempStatus	= unMML->cStatus;
	unsigned	char	cTempData1	= unMML->cData1;
	unsigned	char	cTempData2	= unMML->cData2;

				char	flagKeyOn	= 0;	//他にkey onがあった。	
				char	flagKeyOff	= 0;	//key offが来た。	
	long				fptStream;			//ファイルポインタ

	//--------------
	//ストリームポインタの保存
	fptStream = ftell(OptionSW->SMF.stream);

	//--------------
	//Note Offを検索する
	while((flagKeyOff==0)&&(unMML->flagEoT==0)){
		iLength+=EventRead(OptionSW,unMML);

		//ポインタを進める
		if(unMML->iSize>0){
			StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);
		};

		//他にコマンドが来た？
		if(((unMML->cStatus==cTempStatus)&&(unMML->cData2>0)) ||
			((unMML->cStatus>=0xA0)&&(unMML->cStatus<0xF0)&&((unMML->cStatus&0x0F)==(cTempStatus&0x0F))))
		{
			flagKeyOn=1;
		};
		//Note Offが来た？
		if(((unMML->cStatus==(cTempStatus&0x8F))&&(unMML->cData1==cTempData1)) ||
			((unMML->cStatus==(cTempStatus&0x9F))&&(unMML->cData1==cTempData1)&&(unMML->cData2==0))) {
			flagKeyOff=1;
		};
	};

	//他にkey onがあったら。
	if(flagKeyOn==1){
		fprintf(OptionSW->MML.stream," Sub{");
		unMML->cNote=cTempData1;	//とりあえず出力
		unMML->cVelo=cTempData2;	
		unMML->flagNote=1;			//Key On
		//音符を出力
		OutputNoteEx(OptionSW,unMML);
		//音長を出力
		Ticks2LengthEx(OptionSW,unMML,iLength);
		fprintf(OptionSW->MML.stream,"}");
		unMML->cNote=0xFF;
	}else{
		unMML->cNote=cTempData1;
		unMML->cVelo=cTempData2;	
		unMML->flagNote=1;	//Key On
	}

	//--------------
	//復帰
	unMML->flagEoT	= cTempEoT;		//End of Trackのフラグ
	unMML->cStatus	= cTempStatus;
	unMML->cData1	= cTempData1;
	unMML->cData2	= cTempData2;

	//ポインタを戻す
	fseek(OptionSW->SMF.stream,fptStream,SEEK_SET);
}
//==============================================================
//		コントロールチェンジのデコード
//--------------------------------------------------------------
//	●引数
//					OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//					unMMLinfo	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void DecodeControlChange(OPSW *OptionSW,unMMLinfo *unMML){

	//コントロール別処理
	switch(unMML->cData1){
		//--------------
		//Modulation
		case(1):
			fprintf(OptionSW->MML.stream,"M(%d); ",unMML->cData2);
			break;
		//--------------
		//ポルタメントタイム
		case(5):
			fprintf(OptionSW->MML.stream,"PT(%d); ",unMML->cData2);
			break;
		//--------------
		//DataMSB
		case(6):
			fprintf(OptionSW->MML.stream,"DataMSB(%d); ",unMML->cData2);
			break;
		//--------------
		//Main Volume
		case(7):
			fprintf(OptionSW->MML.stream,"V(%d); ",unMML->cData2);
			break;
		//--------------
		//Panpot
		case(10):
			fprintf(OptionSW->MML.stream,"P(%d); ",unMML->cData2);
			break;
		//--------------
		//Expression
		case(11):
			fprintf(OptionSW->MML.stream,"EP(%d); ",unMML->cData2);
			break;
		//--------------
		//ポルタメントスイッチ
		case(65):
			fprintf(OptionSW->MML.stream,"PS(%d); ",unMML->cData2);
			break;
		//--------------
		//リバーブ
		case(91):
			fprintf(OptionSW->MML.stream,"Reverb(%d); ",unMML->cData2);
			break;
		//--------------
		//コーラス
		case(93):
			fprintf(OptionSW->MML.stream,"Chorus(%d); ",unMML->cData2);
			break;
		//--------------
		//セレステ
		case(94):
			fprintf(OptionSW->MML.stream,"VAR(%d); ",unMML->cData2);
			break;
		//--------------
		//その他
		default:
			fprintf(OptionSW->MML.stream,"y%d,%d; ",unMML->cData1,unMML->cData2);
			break;
	};

};
//##############################################################
//		MIDIのイベント
//##############################################################
//==============================================================
//		チャンネル・ボイス・メッセージのデコード
//--------------------------------------------------------------
//	●引数
//				OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//				unMMLinfo	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void DecodeChannelVoiceMessage(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	unsigned	int			iChannel;		//チャンネル情報
	unsigned	char		cCommand;		//コマンド種類

	//--------------
	//チャンネルとコマンドに分解する
	iChannel = unMML->cStatus & 0x0F;		//チャンネル
	cCommand = unMML->cStatus >> 4;		//コマンド

	//--------------
	//コマンド別処理
	switch(cCommand){
		//--------------
		//Note Off Message
		case(0x08) :
//			fprintf(OptionSW->MML.stream,"NoteOff(%d,%d); ",unMML->cData1,unMML->cData2);
			if(unMML->cData1==unMML->cNote){
				unMML->cNote=0xFF;			//Note Off (休符)
			};
			break;
		//--------------
		//Note On Message
		case(0x09) :
//			fprintf(OptionSW->MML.stream,"NoteOn(%d,%d); ",unMML->cData1,unMML->cData2);
			if((unMML->cData1==unMML->cNote)&&(unMML->cData2==0)){
				unMML->cNote=0xFF;			//Note Off (休符)
			} else {
				NoteDecode(OptionSW,unMML);	//Note On
			};
			break;
		//--------------
		//Polyhonic Key Pressure 
		case(0x0A) :
			fprintf(OptionSW->MML.stream,"DirectSMF($A0+Channel-1,%d,%d); ",unMML->cData1,unMML->cData2);
			break;
		//--------------
		//Control Change Message
		case(0x0B) :
			DecodeControlChange(OptionSW,unMML);
			break;
		//--------------
		//Program Change Message
		case(0x0C) :
			fprintf(OptionSW->MML.stream,"Voice(%d); ",(unMML->cData1)+1);
			break;
		//--------------
		//Channel Pressure 
		case(0x0D) :
			fprintf(OptionSW->MML.stream,"DirectSMF($D0+Channel-1,%d); ",unMML->cData1);
			break;
		//--------------
		//Pitch Bend Change Message
		case(0x0E) :
			fprintf(OptionSW->MML.stream,"PitchBend(%d); ",((((unMML->cData2)<<7)+(unMML->cData1))-8192));
			break;
		//--------------
		//それら以外
		default:
			not_smf(OptionSW->SMF.name,"無効なChannel Voice Message。\n");
			break;
	};


};
//==============================================================
//		システム・エクスクルーシブ・メッセージのデコード
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void DecodeSystemExcusiveMessage(OPSW *OptionSW,unMMLinfo *unMML){

	fprintf(OptionSW->MML.stream,"SysEx = $F0,");
	OutputHex(OptionSW,unMML->iSize);
	fprintf(OptionSW->MML.stream,";\n	");

};
//==============================================================
//		システム・コモン・メッセージのデコード
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void DecodeSystemCommonMessage(OPSW *OptionSW,unMMLinfo *unMML){

	fprintf(OptionSW->MML.stream,"SysEx = ");
	OutputHex(OptionSW,unMML->iSize);
	fprintf(OptionSW->MML.stream,";\n	");

};
//==============================================================
//		メタイベントのデコード
//--------------------------------------------------------------
//	●引数
//				OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//				unMMLinfo	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void DecodeMetaEvent(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	unsigned	char	cData[4];	//データ
	unsigned	int		iData;		//汎用

	//イベント別処理
	switch(unMML->cData1){
		//--------------
		//シーケンス番号
		case(0x00) :
			if(unMML->iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF($FF,$00,$02,");
				OutputHex(OptionSW,unMML->iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"無効なシーケンス番号。\n");
			}
			break;
		//--------------
		//テキスト
		case(0x01) :
			fprintf(OptionSW->MML.stream,"MetaText={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//著作権表示
		case(0x02) :
			fprintf(OptionSW->MML.stream,"Copyright={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//シーケンス名／トラック名
		case(0x03) :
			fprintf(OptionSW->MML.stream,"TrackName={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//楽器名
		case(0x04) :
			fprintf(OptionSW->MML.stream,"InstrumentName={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//歌詞
		case(0x05) :
			fprintf(OptionSW->MML.stream,"Lyric={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//マーカ
		case(0x06) :
			fprintf(OptionSW->MML.stream,"Marker={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//キューポイント
		case(0x07) :
			fprintf(OptionSW->MML.stream,"CuePoint={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//MIDIチャンネルプリフィックス
		case(0x20) :
			if(unMML->iSize==1){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"ChannelPrefix(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"無効なチャンネルプリフィックス。\n");
			}
			break;
		//--------------
		//ポート番号
		case(0x21) :
			if(unMML->iSize==1){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Port(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"無効なポート番号。\n");
			}
			break;
		//--------------
		//エンドオブトラック
		case(0x2F) :
			if(unMML->iSize==0){
				//チャンネルの逆MMLコンパイルを終わる。
				unMML->flagEoT=1;
			} else {
				not_smf(OptionSW->SMF.name,"無効なEnd of Track。\n");
			}
			break;
		//--------------
		//テンポ
		case(0x51) :
			if(unMML->iSize==3){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Tempo(%d); ",(60000000/((cData[0]<<16)+(cData[1]<<8)+(cData[2]))));
			} else {
				not_smf(OptionSW->SMF.name,"無効な。\n");
			}
			break;
		//--------------
		//SMPTEオフセット
		case(0x54) :
			if(unMML->iSize==5){
				fprintf(OptionSW->MML.stream,"DirectSMF($FF,$54,$05,");
				OutputHex(OptionSW,unMML->iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"無効なSMPTEオフセット。\n");
			}
			break;
		//--------------
		//拍子
		case(0x58) :
			if(unMML->iSize==4){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				iData=1<<cData[1];
				//解析は前処理でやるが、表示はここでする。
				fprintf(OptionSW->MML.stream,"TimeSignature(%d,%d);\n	",cData[0],iData);
			} else {
				not_smf(OptionSW->SMF.name,"無効な拍子。\n");
			}
			break;
		//--------------
		//調号	（to do 先ずは、DirectSMF。TimeKeyFlagで対応できる？）
		case(0x59) :
			if(unMML->iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF($FF,$59,$02,");
				OutputHex(OptionSW,unMML->iSize);
				fprintf(OptionSW->MML.stream,");\n	");
			} else {
				not_smf(OptionSW->SMF.name,"無効な調号。\n");
			}
			break;
		//--------------
		//シーケンサ固有のメタイベント（未対応）
		case(0x7F) :
			fprintf(OptionSW->MML.stream,"DirectSMF($FF,$7F,$00); /*");
			OutputHex(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"*/ ");
			break;
		//--------------
		//
		default:
			not_smf(OptionSW->SMF.name,"無効なMeta Event\n");
			break;
	};

};
//==============================================================
//		１コマンドのデコード	（メインデコード）
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//				unMMLinfo	unMML		逆MML用構造体
//	●返値
//			__int32	iDeltaTime	デルタタイム
//==============================================================
void Decode1Command(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	unsigned	__int32		iDeltaTime;		//読み込んだ可変長数値
	unsigned	int			iChannel;		//チャンネル情報
	unsigned	char		cCommand;		//コマンド種類

	//MIDIイベント読み込み
	iDeltaTime = EventRead(OptionSW,unMML);
	unMML->iTicks+=iDeltaTime;				//現デルタタイムに加算
	unMML->iTotalTicks+=iDeltaTime;			//総デルタタイムに加算

	//----------------------
	//●コマンド別処理
	//◆Channel Voice Message？
	if(unMML->cStatus<0xF0){

		//--------------
		//チャンネルとコマンドに分解する
		iChannel = unMML->cStatus & 0x0F;	//チャンネル
		cCommand = unMML->cStatus >> 4;		//コマンド

		//逆コンパイル中のChannelだったら、音長を出力して、イベントを出力
		if(unMML->iChannel==(iChannel+1)){

			//--------------
			//音長の出力
			OutputNote(OptionSW,unMML);

			//--------------
			//チャンネル・ボイス・メッセージ
			DecodeChannelVoiceMessage(OptionSW,unMML);
		};

	//--------------
	//◆System Excusive Message
	//◆System Common Message
	//◆Meta Event
	} else {

		//Channel 0だったら、音長を出力して、イベントを出力
		if(unMML->iChannel==0){

			//--------------
			//音長の出力
			//但し、EoT時はしない。
			if(unMML->flagEoT==0){
				OutputNote(OptionSW,unMML);
			};

			//--------------
			//メッセージ別処理
			switch(unMML->cStatus){
				//--------------
				//システム・エクスクルーシブ・メッセージ
				case(0xF0):
					DecodeSystemExcusiveMessage(OptionSW,unMML);
					break;
				//--------------
				//システム・コモン・メッセージ
				case(0xF7):
					DecodeSystemCommonMessage(OptionSW,unMML);
					break;
				//--------------
				//メタ・イベント
				case(0xFF):
					DecodeMetaEvent(OptionSW,unMML);
					break;
				//--------------
				//その他
				default:
					not_smf(OptionSW->SMF.name,"無効なMIDIイベントを検出しました。\n");
					break;
			};

		//Channel 1以外だったら、
		} else {
			//EOTだったら、これまでのtick数の休符を出力する。
			if((unMML->flagEoT==1)&&(unMML->flagCh==1)){
				OutputNote(OptionSW,unMML);
			};
			//ストリームのポインタを進める
			StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);

		};

	};

};

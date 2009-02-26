/****************************************************************/
/*																*/
/*					定数										*/
/*																*/
/****************************************************************/

#include "stdafx.h"

#define	dwMThd	0x6468544D		//'MThd'を16進数にしてみた。
#define	dwMTrk	0x6B72544D		//'MTrk'を16進数にしてみた。

/****************************************************************/
/*																*/
/*					エラー処理									*/
/*																*/
/****************************************************************/
//==============================================================
//		MIDIファイルじゃありません。
//--------------------------------------------------------------
//	●引数
//			char		*filename	元ファイル名
//	●返値
//			無し
//==============================================================
void not_smf(char *filename, char *stMsg){
	fprintf(stderr,filename);
	fprintf(stderr,": ");
	fprintf(stderr,"データが不正です。\n");
	fprintf(stderr,stMsg);
	exit(EXIT_FAILURE);
}
/****************************************************************/
/*																*/
/*					汎用サブルーチン							*/
/*																*/
/****************************************************************/
//==============================================================
//		ストリーム・ポインタを進める
//--------------------------------------------------------------
//	●引数
//			FileInfo *file	ファイル情報の構造体
//			__int32		iSize		サイズ
//	●返値
//			無し
//==============================================================
void StreamPointerAdd(FileInfo *file,__int32 iSize){

	fseek(file->stream,(long)iSize,SEEK_CUR);

};
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
	unsigned	__int32		iDeltaTime;		//
	unsigned	char		cFread;			//
	unsigned	int			count;			//

	//----------------------------------
	//■デルタタイム読み込み
	iDeltaTime=0;
	count=4;	//最大4Byte。従って、読み込みは４回まで
	do{
		fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
		iDeltaTime=(iDeltaTime<<7)+((unsigned __int32)cFread&0x7F);
		count--;
	} while((count>0)&&(cFread&0x80));

	return(iDeltaTime);

};
//##############################################################
//		出力関係
//##############################################################
//==============================================================
//		16進数連続デコード＆出力	（SysEx命令等用）
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			__int32		iSize		出力サイズ
//	●返値
//			無し
//==============================================================
void OutputHex(OPSW *OptionSW,__int32 iSize){

	//----------------------------------
	//■Local 変数
	unsigned	char		cFread;			//

	while(iSize>0){
		fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
		fprintf(OptionSW->MML.stream,"x%X",cFread);
		if(iSize>1){
			fprintf(OptionSW->MML.stream,",");
		} else {
			fprintf(OptionSW->MML.stream," ");
		};
		iSize--;
	};

};
//==============================================================
//		文字列デコード＆出力	（メタイベント命令等用）	※""も含む。
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			__int32		iSize		出力サイズ
//	●返値
//			無し
//==============================================================
void OutputStrings(OPSW *OptionSW,__int32 iSize){

	//----------------------------------
	//■Local 変数
	unsigned	char		cFread;			//

	fputc('"',OptionSW->MML.stream);

	while(iSize>0){
		fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
		fputc(cFread,OptionSW->MML.stream);
		iSize--;
	};

	fputc('"',OptionSW->MML.stream);

};
//==============================================================
//		音長出力
//--------------------------------------------------------------
//	●引数
//			unMMLinfo	*unMML		逆MML用構造体
//			int			iTicks		チック
//	●返値
//			int			iLength		音長	-1だったら、余り有り
//==============================================================
int Ticks2Length(unMMLinfo *unMML, int iTicks){

	//----------------------------------
	//■Local 変数
	unsigned	int		iZenlenght;			//全音符
				int		iLength;			//音長

	iZenlenght=(unMML->SMF_Header.timebase)*4;	//これは、4固定

	iLength=iZenlenght/iTicks;
	if((iZenlenght % iTicks)!=0){
		iLength=-1;
	};

	return(iLength);
};
//==============================================================
//		音長出力
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	*unMML		逆MML用構造体
//			int			iTicks		チック
//	●返値
//			無し
//==============================================================
void Ticks2LengthEx(OPSW *OptionSW,unMMLinfo *unMML, int iTicks){

	//----------------------------------
	//■Local 変数
	int		iLength;			//音長変換用

	//----------------------------------
	//■音長に変換

	// to do	付点も計算する。

	iLength=Ticks2Length(unMML,iTicks);
	if(iLength==-1){
		fputc('%',OptionSW->MML.stream);
		fprintf(OptionSW->MML.stream,"%d",unMML->iTicks);	// to do
	} else {
		fprintf(OptionSW->MML.stream,"%d",iLength);
	};

};
//==============================================================
//		音長出力
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	*unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void OutputTicks(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	unsigned	int		iZenlenght;			//
				char	flagStart;
				int		iLength;			//音長変換用

	//----------------------------------
	//■初期化
	flagStart=1;
	iZenlenght=(unMML->SMF_Header.timebase)*4;	//	to do	いずれ

	//----------------------------------
	//■音長の出力
	//iTicks が 0 になるまで繰り返す。
	while((unMML->iTicks)>0){

		//最初のループでなければ、タイで繋ぐ
		if(flagStart==1){
			flagStart=0;
		} else {
			fprintf(OptionSW->MML.stream,"^");
		};

		//全音符より大きかったら、まず全音符を出力する。
		if((unMML->iTicks)>=iZenlenght){
			Ticks2LengthEx(OptionSW,unMML,iZenlenght);
			unMML->iTicks-=iZenlenght;
		//全音符より小さくなったら、音長を計算する。
		} else {
			Ticks2LengthEx(OptionSW,unMML,unMML->iTicks);
			unMML->iTicks=0;
		};
	};
};
//==============================================================
//		音符＆音長出力
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	*unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void OutputNote(OPSW *OptionSW,unMMLinfo *unMML){

	if(unMML->flagCh==0){
		unMML->flagCh=1;
		fprintf(OptionSW->MML.stream,"\nTime(1:1:0);\n	");
		if((unMML->iChannel)>0){
			fprintf(OptionSW->MML.stream,"CH(%d) ",unMML->iChannel);
		};
	};

	if(unMML->iTicks>0){

		fprintf(OptionSW->MML.stream,"r");
		OutputTicks(OptionSW,unMML);

		//出力したので、リセットする。
		unMML->flagNote=0;	//今後は、タイを出力
		unMML->iTicks=0;	//音長は０

	};

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
//		コントロールチェンジのデコード
//--------------------------------------------------------------
//	●引数
//					OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//					unMMLinfo	unMML		逆MML用構造体
//		unsigned	char		cData1		レジスタ
//		unsigned	char		cData2		データ
//	●返値
//			無し
//==============================================================
void DecodeControlChange(OPSW *OptionSW,unMMLinfo *unMML, unsigned char cData1, unsigned char cData2){

	//コントロール別処理
	switch(cData1){
		//--------------
		//Modulation
		case(1):
			fprintf(OptionSW->MML.stream,"M(%d) ",cData2);
			break;
		//--------------
		//ポルタメントタイム
		case(5):
			fprintf(OptionSW->MML.stream,"PT(%d) ",cData2);
			break;
		//--------------
		//DataMSB
		case(6):
			fprintf(OptionSW->MML.stream,"DataMSB(%d) ",cData2);
			break;
		//--------------
		//Main Volume
		case(7):
			fprintf(OptionSW->MML.stream,"V(%d) ",cData2);
			break;
		//--------------
		//Panpot
		case(10):
			fprintf(OptionSW->MML.stream,"P(%d) ",cData2);
			break;
		//--------------
		//Expression
		case(11):
			fprintf(OptionSW->MML.stream,"EP(%d) ",cData2);
			break;
		//--------------
		//ポルタメントスイッチ
		case(65):
			fprintf(OptionSW->MML.stream,"PS(%d) ",cData2);
			break;
		//--------------
		//リバーブ
		case(91):
			fprintf(OptionSW->MML.stream,"Reverb(%d) ",cData2);
			break;
		//--------------
		//コーラス
		case(93):
			fprintf(OptionSW->MML.stream,"Chorus(%d) ",cData2);
			break;
		//--------------
		//セレステ
		case(94):
			fprintf(OptionSW->MML.stream,"VAR(%d) ",cData2);
			break;
		//--------------
		//その他
		default:
			fprintf(OptionSW->MML.stream,"y%d,%d ",cData1,cData2);
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
//	unsigned	char		cStatus		ステータスバイト
//	unsigned	char		cData1		データバイト１
//	unsigned	char		cData2		データバイト２　（無い場合は未使用）
//	●返値
//			無し
//==============================================================
void DecodeChannelVoiceMessage(OPSW *OptionSW,unMMLinfo *unMML,unsigned char cStatus,unsigned char cData1,unsigned char cData2){

	//----------------------------------
	//■Local 変数
	unsigned	int			iChannel;		//チャンネル情報
	unsigned	char		cCommand;		//コマンド種類

	//--------------
	//チャンネルとコマンドに分解する
	iChannel = cStatus & 0x0F;		//チャンネル
	cCommand = cStatus >> 4;		//コマンド

	//--------------
	//コマンド別処理
	switch(cCommand){
		//--------------
		//Note Off Message
		case(0x08) :
			fprintf(OptionSW->MML.stream,"NoteOff(%d,%d); ",cData1,cData2);
			break;
		//--------------
		//Note On Message
		case(0x09) :
			fprintf(OptionSW->MML.stream,"NoteOn(%d,%d); ",cData1,cData2);
			break;
		//--------------
		//Polyhonic Key Pressure 
		case(0x0A) :
			fprintf(OptionSW->MML.stream,"DirectSMF(x%X,%d,%d); ",cStatus,cData1,cData2);
			break;
		//--------------
		//Control Change Message
		case(0x0B) :
			DecodeControlChange(OptionSW,unMML,cData1,cData2);
			break;
		//--------------
		//Program Change Message
		case(0x0C) :
			fprintf(OptionSW->MML.stream,"Voice(%d); ",cData1);
			break;
		//--------------
		//Channel Pressure 
		case(0x0D) :
			fprintf(OptionSW->MML.stream,"DirectSMF(x%X,%d); ",cStatus,cData1);
			break;
		//--------------
		//Pitch Bend Change Message
		case(0x0E) :
			fprintf(OptionSW->MML.stream,"PitchBend(%d); ",(((cData2<<7)+cData1)-8192));
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
//			__int32		iSize		データのサイズ
//	●返値
//			無し
//==============================================================
void DecodeSystemExcusiveMessage(OPSW *OptionSW,unMMLinfo *unMML, __int32 iSize){

	fprintf(OptionSW->MML.stream,"SysEx = xF0,");
	OutputHex(OptionSW,iSize);

};
//==============================================================
//		システム・コモン・メッセージのデコード
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML		逆MML用構造体
//			__int32		iSize		データのサイズ
//	●返値
//			無し
//==============================================================
void DecodeSystemCommonMessage(OPSW *OptionSW,unMMLinfo *unMML, __int32 iSize){

	fprintf(OptionSW->MML.stream,"SysEx = ");
	OutputHex(OptionSW,iSize);

};
//==============================================================
//		メタイベントのデコード
//--------------------------------------------------------------
//	●引数
//				OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//				unMMLinfo	unMML		逆MML用構造体
//	unsigned	char		cData1		データバイト１
//				__int32		iSize		データのサイズ
//	●返値
//			無し
//==============================================================
void DecodeMetaEvent(OPSW *OptionSW,unMMLinfo *unMML, unsigned char cData1, __int32 iSize){

	//----------------------------------
	//■Local 変数
	unsigned char	cData[4];	//データ

	//イベント別処理
	switch(cData1){
		//--------------
		//シーケンス番号
		case(0x00) :
			if(iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x00,x02,");
				OutputHex(OptionSW,iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"無効なシーケンス番号。\n");
			}
			break;
		//--------------
		//テキスト
		case(0x01) :
			fprintf(OptionSW->MML.stream,"MetaText={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//著作権表示
		case(0x02) :
			fprintf(OptionSW->MML.stream,"Copyright={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//シーケンス名／トラック名
		case(0x03) :
			fprintf(OptionSW->MML.stream,"TrackName={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//楽器名
		case(0x04) :
			fprintf(OptionSW->MML.stream,"InstrumentName={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//歌詞
		case(0x05) :
			fprintf(OptionSW->MML.stream,"Lyric={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//マーカ
		case(0x06) :
			fprintf(OptionSW->MML.stream,"Marker={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//キューポイント
		case(0x07) :
			fprintf(OptionSW->MML.stream,"CuePoint={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//MIDIチャンネルプリフィックス
		case(0x20) :
			if(iSize==1){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"ChannelPrefix(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"無効なチャンネルプリフィックス。\n");
			}
			break;
		//--------------
		//ポート番号
		case(0x21) :
			if(iSize==1){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Port(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"無効なポート番号。\n");
			}
			break;
		//--------------
		//エンドオブトラック	to do
		case(0x2F) :
			if(iSize==0){
				//チャンネルの逆MMLコンパイルを終わる。
				unMML->flagEoT=1;
			} else {
				not_smf(OptionSW->SMF.name,"無効なEnd of Track。\n");
			}
			break;
		//--------------
		//テンポ
		case(0x51) :
			if(iSize==3){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Tempo(%d); ",(60000000/((cData[0]<<16)+(cData[1]<<8)+(cData[2]))));
			} else {
				not_smf(OptionSW->SMF.name,"無効な。\n");
			}
			break;
		//--------------
		//SMPTEオフセット
		case(0x54) :
			if(iSize==5){
				fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x54,x05,");
				OutputHex(OptionSW,iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"無効なSMPTEオフセット。\n");
			}
			break;
		//--------------
		//拍子
		case(0x58) :
			if(iSize==4){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"TimeSignature(%d,%d);\n	",cData[0],1<<cData[1]);
			} else {
				not_smf(OptionSW->SMF.name,"無効な拍子。\n");
			}
			break;
		//--------------
		//調号	（先ずは、DirectSMF。KeyFlagで対応できる？）
		case(0x59) :
			if(iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x59,x02,");
				OutputHex(OptionSW,iSize);
				fprintf(OptionSW->MML.stream,");\n	");
			} else {
				not_smf(OptionSW->SMF.name,"無効な。\n");
			}
			break;
		//--------------
		//シーケンサ固有のメタイベント（未対応）
		case(0x7F) :
			fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x7F,x00); /*");
			OutputHex(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"*/ ");
			break;
		//--------------
		//
		default:
			not_smf(OptionSW->SMF.name,"無効なMeta Event\n");
			break;
	};

};
/****************************************************************/
/*																*/
/*					逆コンパイル用・サブルーチン				*/
/*																*/
/****************************************************************/
//##############################################################
//		Sub routine
//##############################################################
//==============================================================
//		イニシャライズ
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML	逆MML用構造体
//	●返値
//			無し
//==============================================================
void uncompile_initTR(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	__int32		stName;		//チャンク名

	//ヘッダー読み込み
	fread((void *)&stName,sizeof(char),4,OptionSW->SMF.stream);
	fread((void *)&unMML->iTrackSize,sizeof(char),4,OptionSW->SMF.stream);
	endian_b2l_D(&unMML->iTrackSize);				//チャンクサイズを保存
	unMML->lOffsetTR=ftell(OptionSW->SMF.stream);	//今のファイルポインタを保存
//	printf("offsetTR=%d\n",unMML->lOffsetTR);	//Debug用

	if(stName!=dwMTrk){
		not_smf(OptionSW->SMF.name,"MTrkチャンクがありません。\n");
	};

	fprintf(OptionSW->MML.stream,"\n\n");
	fprintf(OptionSW->MML.stream,"//--------------------------------------\n");
	fprintf(OptionSW->MML.stream,"//MThd Infomation :\n");
	fprintf(OptionSW->MML.stream,"//	Size = %d \n",unMML->iTrackSize);
	fprintf(OptionSW->MML.stream,"//\n");
	fprintf(OptionSW->MML.stream,"TR(%d)\n",(unMML->iTrack+1));


}
//==============================================================
//		イニシャライズ
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML	逆MML用構造体
//	●返値
//			無し
//==============================================================
void uncompile_initCH(OPSW *OptionSW,unMMLinfo *unMML){

	//トラックの先頭にポインタを移す
	fseek(OptionSW->SMF.stream,unMML->lOffsetTR,SEEK_SET);

	//各項目の初期化
	unMML->iTotalTicks=0;
	unMML->iTicks=0;
	unMML->cRunningStatus=0;
	unMML->iLength=4;
	unMML->iOctave=5;
	unMML->iGate=0;

	unMML->cNote=0xFF;	//ノート
	unMML->flagNote=1;	//出力状態
	unMML->flagCh=0;

	unMML->flagEoT=0;

}
//##############################################################
//		MTrkチャンク
//##############################################################
//==============================================================
//		１コマンドのデコード	（メインデコード）
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//	●返値
//			__int32	iDeltaTime	デルタタイム
//==============================================================
void Decode1Command(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	unsigned	char		cFread;			//fread()関数での読み込み用

	unsigned	__int32		iDeltaTime;		//デルタタイム

	unsigned	int			iChannel;		//チャンネル情報
	unsigned	char		cCommand;		//コマンド種類
	unsigned	char		cStatus;		//ステータス
	unsigned	char		cData1;			//データ１
	unsigned	char		cData2;			//データ２


	//----------------------------------
	//■デルタタイムの読み込み
	iDeltaTime=DecodeVariable(OptionSW);
	unMML->iTicks+=iDeltaTime;				//現デルタタイムに加算
	unMML->iTotalTicks+=iDeltaTime;			//総デルタタイムに加算


	//----------------------------------
	//■MIDIイベントの処理

	//まず、1Byte読み込み
	fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
	//最初からデータビットだったら、ランニングステータスを適用する
	if(cFread&0x80){
		cStatus=cFread;
		cData1=0xFF;
	} else {
		cStatus=unMML->cRunningStatus;
		cData1=cFread;
	};

	//----------------------
	//●コマンド別処理
	//◆Channel Voice Message？
	if(cStatus<0xF0){

	unMML->cRunningStatus=cStatus;	//ランニングステータスの更新

		//--------------
		//チャンネルとコマンドに分解する
		iChannel = cStatus & 0x0F;		//チャンネル
		cCommand = cStatus >> 4;		//コマンド
		//ランニングステータスが適用されてなかったら、最初のビットを読む
		if(cData1==0xFF){
			fread((void *)&cData1,sizeof(char),1,OptionSW->SMF.stream);
		};
		//データ2がある場合、データを読む。
		if(((cCommand>=0x08)&&(cCommand<=0x0B))||(cCommand==0x0E)){
			fread((void *)&cData2,sizeof(char),1,OptionSW->SMF.stream);
		};

		//--------------
		//逆コンパイル中のChannelだったら、音長を出力して、イベントを出力
		if(unMML->iChannel==(iChannel+1)){

			//--------------
			//音長の出力
			OutputNote(OptionSW,unMML);

			//--------------
			//チャンネル・ボイス・メッセージ
			DecodeChannelVoiceMessage(OptionSW,unMML,cStatus,cData1,cData2);
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
			OutputNote(OptionSW,unMML);

			//--------------
			//メッセージ別処理
			switch(cStatus){
				//--------------
				//システム・エクスクルーシブ・メッセージ
				case(0xF0):
					DecodeSystemExcusiveMessage(OptionSW,unMML,DecodeVariable(OptionSW));
					break;
				//--------------
				//システム・コモン・メッセージ
				case(0xF7):
					DecodeSystemCommonMessage(OptionSW,unMML,DecodeVariable(OptionSW));
					break;
				//--------------
				//メタ・イベント
				case(0xFF):
					fread((void *)&cData1,sizeof(char),1,OptionSW->SMF.stream);
					DecodeMetaEvent(OptionSW,unMML,cData1,DecodeVariable(OptionSW));
					break;
				//--------------
				//その他
				default:
					not_smf(OptionSW->SMF.name,"無効なMIDIイベントを検出しました。\n");
					break;
			};

		//Channel 1以外だったら、ストリームのポインタだけ進める
		} else {

			//メッセージ別処理
			switch(cStatus){
				//--------------
				//システム・エクスクルーシブ・メッセージ
				case(0xF0):
					StreamPointerAdd(&OptionSW->SMF,DecodeVariable(OptionSW));
					break;
				//--------------
				//システム・コモン・メッセージ
				case(0xF7):
					StreamPointerAdd(&OptionSW->SMF,DecodeVariable(OptionSW));
					break;
				//--------------
				//メタ・イベント
				case(0xFF):
					fread((void *)&cData1,sizeof(char),1,OptionSW->SMF.stream);
					StreamPointerAdd(&OptionSW->SMF,DecodeVariable(OptionSW));
					if(cData1==0x2F){
						unMML->flagEoT=1;	//EoTだけはチェック
					};
					break;
				//--------------
				//その他
				default:
					not_smf(OptionSW->SMF.name,"無効なMIDIイベントを検出しました。\n");
					break;
			};
		};

	};

};
//##############################################################
//		MThdチャンク
//##############################################################
//==============================================================
//		ヘッダー処理
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void uncompile_Header(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	long		*cSMF_Header;	//if文で一括で'MThd'か調べる用

	//----------------------------------
	//■MThdヘッダー処理	with Endianの変換(Big→Little)
	fread((void *)&unMML->SMF_Header,sizeof(char),14,OptionSW->SMF.stream);
	//↓Ｃ言語でどう書けば良いか知らないから、アセンブリ言語な関数。
	//↓char型にしてもいいんだけど、またポインタ変数増やすのめんどいし。
	//↓"smf2mml_sub.asm"を参照。
	endian_b2l_D(&unMML->SMF_Header.size);
	endian_b2l_W(&unMML->SMF_Header.format);
	endian_b2l_W(&unMML->SMF_Header.track);
	endian_b2l_W(&unMML->SMF_Header.timebase);

	//MThdチャンクがあるか？
	cSMF_Header=(long *)unMML->SMF_Header.strings;	//if文で一括で'MThd'か調べる為
	if(*cSMF_Header != dwMThd){						//(SMF_Header.strings == 'MThd')?
		not_smf(OptionSW->SMF.name,"MThdチャンクがありません。\n");
	};

	//フォーマットは0か1？
	if(unMML->SMF_Header.format==2){
		not_smf(OptionSW->SMF.name,"Format 2には対応しておりません。\n");
	} else if(unMML->SMF_Header.format>2){
		not_smf(OptionSW->SMF.name,"未知のフォーマットです。\n");
	};

	//トラック数は0でない？
	if(unMML->SMF_Header.track==0){
		not_smf(OptionSW->SMF.name,"トラック数が0です。\n");
	};

	//フォーマット0且つ、トラック数が1でない？
	if((unMML->SMF_Header.format==0)&&(unMML->SMF_Header.track!=1)){
		not_smf(OptionSW->SMF.name,"Format 0なのに、複数のトラックがあります。\n");
	};
	
	//timebaseのチェック
	if(unMML->SMF_Header.timebase==0){
		not_smf(OptionSW->SMF.name,"timebaseが0tickです。\n");
	}else if(unMML->SMF_Header.timebase&0x8000){
		not_smf(OptionSW->SMF.name,"秒指定の時間単位には対応しておりません。\n");
	};

	// to do	その他ヘッダのチェック		


	//MMLヘッダー出力
	fprintf(OptionSW->MML.stream,"//--------------------------------------\n");
	fprintf(OptionSW->MML.stream,"//		SMF2MML\n");
	fprintf(OptionSW->MML.stream,"//--------------------------------------\n");
	fprintf(OptionSW->MML.stream,"//MThd Infomation :\n");
	fprintf(OptionSW->MML.stream,"//	Size = %d \n",unMML->SMF_Header.size);
	fprintf(OptionSW->MML.stream,"//	Format = %d \n",unMML->SMF_Header.format);
	fprintf(OptionSW->MML.stream,"//	Track = %d \n",unMML->SMF_Header.track);
	fprintf(OptionSW->MML.stream,"//\n");
	fprintf(OptionSW->MML.stream,"System.TimeBase = %d \n",unMML->SMF_Header.timebase);
	fprintf(OptionSW->MML.stream,"\n");

};
/****************************************************************/
/*																*/
/*					逆コンパイル用・メインルーチン				*/
/*																*/
/****************************************************************/
//==============================================================
//		アンコンパイル
//--------------------------------------------------------------
//	●引数
//			OPSW *OptionSW	オプションスイッチ構造体のポインタ
//	●返値
//			MMLファイルにMML文字列を入れてゆく
//==============================================================
void uncompile(OPSW *OptionSW){

	//----------------------------------
	//■Local 変数
	unMMLinfo	unMML;			//逆MMLを処理するための構造体

	//----------------------------------
	//■ヘッダー処理
	uncompile_Header(OptionSW,&unMML);

	//----------------------------------
	//■各MTrkの処理
	unMML.iTrack=0;
	while(unMML.iTrack<unMML.SMF_Header.track){
		uncompile_initTR(OptionSW,&unMML);
		unMML.iChannel=0;
		while(unMML.iChannel<=16){
			uncompile_initCH(OptionSW,&unMML);
			while(unMML.flagEoT==0){
				Decode1Command(OptionSW,&unMML);
			};
			unMML.iChannel++;
		};
		unMML.iTrack++;
	};
};

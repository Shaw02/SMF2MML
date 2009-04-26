
#include "stdafx.h"

//==============================================================
//		可変長デコード
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//			__int32	iDeltaTime	デルタタイム
//==============================================================
unsigned __int32	SMF_event::DecodeVariable(){

	//----------------------------------
	//■Local 変数
	unsigned	__int32		iDeltaTime=0;	//読み込んだ可変長数値
	unsigned	int			count=4;		//読み込み回数カウント用
	unsigned	char		cFread;			//読み込み用変数

	//----------------------------------
	//■デルタタイム読み込み
	do{
		cFread=Read();		//1 Byte Read
		iDeltaTime=(iDeltaTime<<7)+((unsigned int)cFread&0x7F);
		count--;
	} while((count>0)&&(cFread&0x80));

	return(iDeltaTime);

};
//==============================================================
//		イベントの読み込み
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//	unsigned	__int32	iDeltaTime	デルタタイム
//			unMML->cRunningStatus	今回のステータス・バイト
//			unMML->cStatus			ステータス・バイト(01==error)
//			unMML->cData1			データ・バイト１
//			unMML->cData2			データ・バイト２
//			unMML->iSize			データのサイズ
//==============================================================
unsigned	__int32	SMF_event::EventRead(){

	//----------------------------------
	//■Local 変数
							//デルタタイムの読み込み
	const	unsigned	__int32		iDeltaTime=DecodeVariable();
	const	unsigned	int			iMask=OptionSW->iMaskFlag;

	//----------------------------------
	//■MIDIイベントの処理
	//まず、1Byte読み込み
	cStatus=Read();

	//----------------------
	//●コマンド別処理
	//◆Channel Voice Message？
	if(cStatus<0xF0){

		iSize=0;	//これは０

		//ステータスだった場合（メタイベント・システムエクスクルーシブの可能性有り）
		if(cStatus&0x80){
			cData1=Read();
			cRunningStatus=cStatus;	//ランニングステータスの更新
		//データだった場合、ランニングステータスの適用
		} else {
			cData1=cStatus;
			cStatus=cRunningStatus;
		};

		//データ2がある場合、データを読む。
		if(((cStatus>=0x80)&&(cStatus<=0xBF))||((cStatus>=0xE0)&&(cStatus<=0xEF))){
			cData2=Read();
		};

	//--------------
	//◆System Excusive Message
	//◆System Common Message
	//◆Meta Event
	} else {

		//メッセージ別処理
		switch(cStatus){
			//--------------
			//システム・エクスクルーシブ・メッセージ
			//システム・コモン・メッセージ
			case(SEXM):
			case(SCM):
				iSize=DecodeVariable();
				break;
			//--------------
			//メタ・イベント
			case(MEM):
				cData1=Read();			//Event Type
				iSize=DecodeVariable();	//Event Size
				if(cData1==0x2F){
					flagEoT=1;	//EoTだけは強制チェック
				};
				break;
			//--------------
			//その他
			default:
				//not_smf("無効なMIDIイベントを検出しました。\n");
				break;
		};
	};



	//--------------
	//マスクするかチェック
	//この順番でチェックすると、スマートなコードになった。
	if(	(((cStatus)==SEXM				)&&(iMask & MASK_SEXM	))
	||	(((cStatus)==SCM				)&&(iMask & MASK_SCM	))
	||	(((cStatus)==MEM				)&&(iMask & MASK_MEM	))	
	||	(((cStatus & 0xF0)==CVM_PKPM	)&&(iMask & MASK_CVM_PKPM	))
	||	(((cStatus & 0xF0)==CVM_CCM		)&&(iMask & MASK_CVM_CCM	))
	||	(((cStatus & 0xF0)==CVM_PCM		)&&(iMask & MASK_CVM_PCM	))
	||	(((cStatus & 0xF0)==CVM_CPM		)&&(iMask & MASK_CVM_CPM	))
	||	(((cStatus & 0xF0)==CVM_PBCM	)&&(iMask & MASK_CVM_PBCM	))	){
		cStatus=0;
	}

	//--------------
	//終了
	return(iDeltaTime);

};

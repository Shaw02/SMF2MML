
#include "stdafx.h"

/****************************************************************/
/*			外部宣言											*/
/****************************************************************/
extern	void not_smf(const char *stMsg);

/****************************************************************/
/*					定数										*/
/****************************************************************/
#define	dwMThd	0x6468544D		//'MThd'を16進数にしてみた。

//==============================================================
//		イニシャライズ
//--------------------------------------------------------------
//	●引数
//			FileOp*	ptSMF	SMFファイル
//			FileOp*	ptMML	MMLファイル
//	●返値
//			無し
//==============================================================
SMF_MThd::SMF_MThd(FileOp* ptSMF):
	SMF(ptSMF)
{

	//----------------------------------
	//■Local 変数
	long		*cSMF_Header;	//if文で一括で'MThd'か調べる用

	//----------------------------------
	//■MThdヘッダー処理	with Endianの変換(Big→Little)
	SMF->read((char *)this,14);
	//↓Ｃ言語でどう書けば良いか知らないから、アセンブリ言語な関数。
	//↓char型にしてもいいんだけど、またポインタ変数増やすのめんどいし。
	//↓"smf2mml_sub.asm"を参照。
	endian_b2l_D(&size);
	endian_b2l_W(&format);
	endian_b2l_W(&track);
	endian_b2l_W(&timebase);

	//MThdチャンクがあるか？
	cSMF_Header=(long *)strings;					//if文で一括で'MThd'か調べる為
	if(*cSMF_Header != dwMThd){						//(SMF_Header.strings == 'MThd')?
		not_smf("MThdチャンクがありません。\n");
	};

	//フォーマットは0か1？
	if(format==2){
		not_smf("Format 2には対応しておりません。\n");
	} else if(format>2){
		not_smf("未知のフォーマットです。\n");
	};

	//トラック数は0でない？
	if(track==0){
		not_smf("トラック数が0です。\n");
	};

	//フォーマット0且つ、トラック数が1でない？
	if((format==0)&&(track!=1)){
		not_smf("Format 0なのに、複数のトラックがあります。\n");
	};
	
	//timebaseのチェック
	if(timebase==0){
		not_smf("timebaseが0tickです。\n");
	}else if(timebase&0x8000){
		not_smf("秒指定の時間単位には対応しておりません。\n");
	};

	// to do	その他ヘッダのチェック		

};

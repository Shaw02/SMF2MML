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
/*			プロトタイプ宣言									*/
/*																*/
/****************************************************************/
extern	void not_smf(char *filename, char *stMsg);
extern	void StreamPointerAdd(FILE *stream,__int32 iSize);
extern	unsigned __int32 EventRead(OPSW *OptionSW,unMMLinfo *unMML);
extern	void Decode1Command(OPSW *OptionSW,unMMLinfo *unMML);

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
	unMML->iTotalTicksChk=0;
	unMML->iTicks=0;
	unMML->iTicksBar=0;
	unMML->iTicksCR=0;

	unMML->cRunningStatus=0;
	unMML->iLength=4;
	unMML->iOctave=80;
	unMML->iGate=0;
	unMML->cVelocity=255;

	unMML->cNote=0xFF;	//ノート
	unMML->flagNote=0;	//出力状態
	unMML->flagCh=0;

	unMML->flagEoT=0;
	unMML->cntBT=0;

}
//##############################################################
//		MTrkチャンク
//##############################################################
//==============================================================
//		メタ・イベント	時間の比較
//--------------------------------------------------------------
//	●引数
//			Rhythm	setBT		セットする拍子・調
//			Rhythm	cmpBT		セットする拍子・調
//	●返値
//			差
//	●処理
//			時間を比較する
//==============================================================
int BeatCompare(const void *setBT,const void *cmpBT){

				__int64	cmpData;
				int		iResult;
	Rhythm *sBT=(Rhythm *)setBT;
	Rhythm *cBT=(Rhythm *)cmpBT;

	cmpData=(unsigned __int64)(sBT->iTicksBT) - (unsigned __int64)(cBT->iTicksBT);
	if(cmpData<0){
		iResult=-1;
	}else if(cmpData>0){
		iResult=1;
	}else{
		iResult=0;
	};

	return(iResult);
};
//==============================================================
//		メタ・イベント　追加
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML	逆MML用構造体
//			Rhythm	setBT		セットする拍子・調
//	●返値
//			
//	●処理
//			メタイベントを探し、追加する
//			（※調導入の時、注意。）
//==============================================================
void AddBeat(OPSW *OptionSW,unMMLinfo *unMML,Rhythm *setBT){

	Rhythm	*ptBT;

	if((unMML->defBT)>0){
		ptBT = (Rhythm *)_lfind((void *)setBT,(void *)unMML->BT,&(unMML->defBT),sizeof(Rhythm),BeatCompare);
	} else {
		ptBT=NULL;
	};

	if(ptBT==NULL){
		ptBT=&unMML->BT[unMML->defBT];
		unMML->defBT++;
	};

	ptBT->cKey		= setBT->cKey;
	ptBT->cmi		= setBT->cmi;
	ptBT->iTicksBT	= setBT->iTicksBT;
	ptBT->iTicksOne	= setBT->iTicksOne;

};
//==============================================================
//		メタ・イベントの検索
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	unMML	逆MML用構造体
//	●返値
//			
//	●処理
//			メタイベントを探し、イベントに応じた処理
//==============================================================
void MetaSearch(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	unsigned	__int32		iDeltaTime;		//読み込んだ可変長数値
	unsigned char	cData[4];	//データ
	long			fptStream;	//ファイルポインタ
	Rhythm			nowBeat;	//現在の拍子

	nowBeat.iTicksBT=0;
	nowBeat.iTicksOne=0;
	nowBeat.cKey=0;
	nowBeat.cmi=0;

	//ポインタの保存
	fptStream = ftell(OptionSW->SMF.stream);

	//全トラックのサーチ
	unMML->iTrack=0;
	unMML->iChannel=0;
	while((unMML->iTrack)<(unMML->SMF_Header.track)){
		uncompile_initTR(OptionSW,unMML);
		uncompile_initCH(OptionSW,unMML);
		//EOTがあるまでループ
		while(unMML->flagEoT==0){
			iDeltaTime = EventRead(OptionSW,unMML);
			unMML->iTicks+=iDeltaTime;				//現デルタタイムに加算
			unMML->iTotalTicks+=iDeltaTime;			//総デルタタイムに加算
			
			//メタイベント？
			if(unMML->cStatus==0xFF){
				switch(unMML->cData1){
					//--------------
					//拍子
					case(0x58):
						fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
						nowBeat.iTicksBT = unMML->iTotalTicks;
						nowBeat.iTicksOne= (cData[0]*(unMML->SMF_Header.timebase<<2)/(1<<cData[1]));
						AddBeat(OptionSW,unMML,&nowBeat);
						break;
					//--------------
					//調号
					case(0x59) :
						fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
//						nowBeat.iTicksBT = unMML->iTotalTicks;
						nowBeat.cKey	= (char)cData[0];
						nowBeat.cmi		= cData[1];
//						nowBeat.cmi |=	0x80;
//						AddBeat(OptionSW,unMML,&nowBeat);
//						nowBeat.cmi &=	0x7F;
						break;
					//--------------
					//処理しないメタイベントの場合
					default:
						StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);
						break;
				};
			//メタイベント以外は無視
			} else {
				if(unMML->iSize>0){
					StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);
				};
			};
		};
		unMML->iTrack++;
	};

	//ソート
	qsort((void *)unMML->BT,unMML->defBT,sizeof(Rhythm),BeatCompare);

	//ポインタを戻す
	fseek(OptionSW->SMF.stream,fptStream,SEEK_SET);

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
	Rhythm		defaultBT;

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

	//まずは、4/4拍って事にしておく。
	unMML->defBT=0;
	defaultBT.iTicksBT=0;
	defaultBT.iTicksOne=unMML->SMF_Header.timebase<<2;
	defaultBT.cKey=0;
	defaultBT.cmi=0;
	AddBeat(OptionSW,unMML,&defaultBT);

	//メタイベントのサーチ
	MetaSearch(OptionSW,unMML);

	//最後
	defaultBT.iTicksBT=0x7FFFFFFFFFFFFFFF;
	defaultBT.iTicksOne=unMML->SMF_Header.timebase<<2;
	defaultBT.cKey=0;
	defaultBT.cmi=0;
	AddBeat(OptionSW,unMML,&defaultBT);

	//to do	key, Beatの補完
	unsigned	int	i=0;
	while(i<(unMML->defBT)){
		printf("Pointer:%d / Length:%d / Key:%d / mi:%d \n",
				(int)	unMML->BT[i].iTicksBT,
						unMML->BT[i].iTicksOne,
						unMML->BT[i].cKey,
						unMML->BT[i].cmi);
		i++;
	};

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
		//トラック毎の初期化
		uncompile_initTR(OptionSW,&unMML);

		//MMLでの、トラックの初期化
		fprintf(OptionSW->MML.stream,"\n\n");
		fprintf(OptionSW->MML.stream,"//--------------------------------------\n");
		fprintf(OptionSW->MML.stream,"//MThd Infomation :\n");
		fprintf(OptionSW->MML.stream,"//	Size = %d \n",unMML.iTrackSize);
		fprintf(OptionSW->MML.stream,"//\n");
		fprintf(OptionSW->MML.stream,"TR(%d)\n",(unMML.iTrack+1));

		//チャンネル毎の処理
		unMML.iChannel=0;
		while(unMML.iChannel<=16){
			//チャンネル毎の初期化
			uncompile_initCH(OptionSW,&unMML);
			//End of Trackが来るまで処理
			while(unMML.flagEoT==0){
				Decode1Command(OptionSW,&unMML);
			};
			unMML.iChannel++;
		};
		unMML.iTrack++;
	};
};

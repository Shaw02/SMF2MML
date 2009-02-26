// smf2mml.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

/****************************************************************/
/*																*/
/*			プロトタイプ宣言									*/
/*																*/
/****************************************************************/
extern		void uncompile(OPSW *OptionSW);

//●プロトタイプ宣言



/****************************************************************/
/*																*/
/*					エラー処理									*/
/*																*/
/****************************************************************/
//==============================================================
//		エラー処理	（プロセスも終了する）
//--------------------------------------------------------------
//	●引数
//			char *stErrMsg1	エラーメッセージ(前)
//			char *stErrMsg2	エラーメッセージ(次行)
//	●返値
//			無し
//==============================================================
void error(char *stErrMsg1, char *stErrMsg2){

	perror(stErrMsg1);
	if(stErrMsg2[0]!=0){printf(stErrMsg2);};
	exit(EXIT_FAILURE);

};
/****************************************************************/
/*																*/
/*					汎用サブルーチン							*/
/*																*/
/****************************************************************/
//==============================================================
//		ストリームのサイズを得る
//--------------------------------------------------------------
//	●引数
//			FileInfo *file	ファイル情報の構造体
//	●返値
//			file->size		ストリームのサイズ
//==============================================================
size_t getStreamSize(FileInfo *file){

	//----------------------------------
	//■Local 変数
	size_t	ptCUR;

	ptCUR=ftell(file->stream);				//ストリームのサイズを得る
	fseek(file->stream,0,SEEK_END);			//ストリームのポインタを最後に
	file->size=ftell(file->stream);			//ストリームのサイズを得る
	fseek(file->stream,ptCUR,SEEK_SET);		//ストリームのポインタを元の位置に

	return(ptCUR);
};
/****************************************************************/
/*																*/
/*					メイン・サブルーチン						*/
/*																*/
/****************************************************************/
//==============================================================
//		オプション処理
//--------------------------------------------------------------
//	●引数
//			int argc		オプション文字列の数
//			_TCHAR* argv[]	オプション文字列
//			OPSW *OptionSW	オプションスイッチ構造体のポインタ
//	●返値
//			OptionSW->SMF.name[]		変換元のSMFファイル
//			OptionSW->MML.name[]		変換先のMMLファイル
//	●備考
//			オプションにファイル名が指定されない場合は、ヘルプ表示して終了
//==============================================================
void option(int argc, _TCHAR* argv[],OPSW *OptionSW){

	//----------------------------------
	//■Local 変数
	int		iCount;				//whileのカウント用

	//Option処理用
	int		iOptionChk;			//オプションチェック用　ポインタ
	char	cOption;			//オプションチェック用　文字
	char	iFlagFilnameExt;	//拡張子あったかのフラグ



	//----------------------------------
	//■初期設定
	OptionSW->SMF.name[0]=0;	//デフォルトはファイル名無し
	OptionSW->MML.name[0]=0;	//デフォルトはファイル名無し
	OptionSW->fHelp=0;			//ヘルプは、デフォルトは表示しない。
								//　オプション指定 or ファる名が記述されない場合に出力

	//----------------------------------
	//■オプション処理
	iCount=1;	//コマンド名は飛ばす
	while(iCount!=argc)
	{
		//--------------
		//オプションスイッチにスラッシュがあるか確認
		if((argv[iCount][0]=='/')||(argv[iCount][0]=='-')){

			//--------------
			//◆Option Switch	（スラッシュがあった場合の処理）
			cOption=argv[iCount][1];
			switch(cOption){
				//Help表示
				case 'h' :
				case 'H' :
				case '?' :
					OptionSW->fHelp=1;
					break;
				//デフォルト
				default :
					fprintf(stderr,"オプションが不正です。\n");
					exit(EXIT_FAILURE);
					break;
			};

		} else{

			//--------------
			//◆ファイル名	（スラッシュが無かった場合の処理）
			iFlagFilnameExt=0;		//拡張子の有無　Reset
			iOptionChk=0;		
			while((cOption=argv[iCount][iOptionChk])!=0)
			{
				OptionSW->SMF.name[iOptionChk]=cOption;
				if(cOption=='.'){iFlagFilnameExt=1;};
				iOptionChk++;
			};
			if(iFlagFilnameExt==0){
				OptionSW->SMF.name[iOptionChk++]='.';	//拡張子を".mid"にする。
				OptionSW->SMF.name[iOptionChk++]='m';	
				OptionSW->SMF.name[iOptionChk++]='i';	
				OptionSW->SMF.name[iOptionChk++]='d';	
			};
			OptionSW->SMF.name[iOptionChk++]=0;	

			iOptionChk=0;		
			while((cOption=OptionSW->SMF.name[iOptionChk])!='.')
			{
				OptionSW->MML.name[iOptionChk]=cOption;
				iOptionChk++;
			};
			OptionSW->MML.name[iOptionChk++]='.';	//拡張子を".mml"にする	
			OptionSW->MML.name[iOptionChk++]='m';	
			OptionSW->MML.name[iOptionChk++]='m';	
			OptionSW->MML.name[iOptionChk++]='l';	
			OptionSW->MML.name[iOptionChk++]=0;	

		};

		//--------------
		//◆次のオプション
		iCount++;
	};

	//--------------
	//ファイル名が書かれなかった場合、ヘルプを表示する。
	if(OptionSW->SMF.name[0]==0){OptionSW->fHelp=1;};



	//----------------------------------
	//◆オプションで指定された事を処理する。

	//--------------
	//ヘルプ表示
	if(OptionSW->fHelp==1){print_help();};

};
//==============================================================
//		メインルーチン
//--------------------------------------------------------------
//	●引数
//			int argc		オプション文字列の数
//			_TCHAR* argv[]	オプション文字列
//	●返値
//			プロセスに返す値
//==============================================================
int _tmain(int argc, _TCHAR* argv[])
{

	//----------------------------------
	//■Local 変数
//	FileInfo	SMF;			//SMFファイルの情報
//	FileInfo	MML;			//MMLファイルの情報
	OPSW		OptionSW;

	//オプション処理
	option(argc,argv,&OptionSW);

	//----------
	//Debug用 表示
	printf("SMF = ");
	printf(OptionSW.SMF.name);
	printf("\n");
	printf("MML = ");
	printf(OptionSW.MML.name);
	printf("\n");
	//----------



	//SMFファイルの Open (read only)
	if((OptionSW.SMF.stream=fopen(OptionSW.SMF.name,"rb"))==NULL){error(OptionSW.SMF.name,"");};
	getStreamSize(&OptionSW.SMF);	//ファイルサイズの取得

	//----------
	//Debug用
	printf("size=%d\n",OptionSW.SMF.size);	//ストリームサイズの表示
	//----------

	//MMLファイルの Open (read only)
	if((OptionSW.MML.stream=fopen(OptionSW.MML.name,"w"))==NULL){error(OptionSW.MML.name,"");};

	//逆コンパイル
	uncompile(&OptionSW);

	//MMLファイルの Close
	if(fclose(OptionSW.MML.stream)!=NULL){error("","");};

	//SMFファイルの Close
	if(fclose(OptionSW.SMF.stream)!=NULL){error("","");};

};

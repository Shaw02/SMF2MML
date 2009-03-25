/****************************************************************/
/*																*/
/*				SMF tp MML un-compiler							*/
/*																*/
/*								Programmed by.					*/
/*									S.W.	(A.Watanabe)		*/
/*																*/
/****************************************************************/

// smf2mml.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

/****************************************************************/
/*																*/
/*			プロトタイプ宣言									*/
/*																*/
/****************************************************************/
extern		void	uncompile(OPSW *OptionSW);
extern		size_t	getStreamSize(FILE *fStream);

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
//==============================================================
//		エラー処理	（プロセスも終了する）
//--------------------------------------------------------------
//	●引数
//			char *stErrMsg	エラーメッセージ
//	●返値
//			無し
//==============================================================
void opError(char *stErrMsg){

	fprintf(stderr,"オプションが不正です。：");
	fprintf(stderr,stErrMsg);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);

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
	int		iResult;			//汎用

	//Option処理用
	int		iOptionChk;			//オプションチェック用　ポインタ
	char	cOption;			//オプションチェック用　文字
	char	iFlagFilnameExt;	//拡張子あったかのフラグ



	//----------------------------------
	//■初期設定
	OptionSW->SMF.name[0]=0;	//デフォルトはファイル名無し
	OptionSW->MML.name[0]=0;	//デフォルトはファイル名無し
	OptionSW->fHelp=0;			//ヘルプは、デフォルトは表示しない。
	OptionSW->iBar=1;			// | 出力
	OptionSW->iCR=4;			//改行の小節数

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
			switch(argv[iCount][1]){
				//--------
				//Help表示
				case 'h' :
				case 'H' :
				case '?' :
					OptionSW->fHelp=1;
					break;
				//--------
				//小節スイッチ
				case 'B' :
					switch(argv[iCount][2]){
						case '-' :
							OptionSW->iBar=0;
							break;
						case '+' :
							OptionSW->iBar=1;
							break;
						default :
							opError("/B");
							break;
					};
					break;
				//--------
				//改行
				case 'C' :
					iResult=sscanf(argv[iCount],"/C%d",&OptionSW->iCR);
					if((iResult==NULL)||(iResult==EOF)){
						opError("/C");
						break;
					};
					break;
				//--------
				//ファイルの指定
				case 'F' :
					//先に、ファイル名が書いてあるかチェック。
					if(argv[iCount][3]==0){
						opError("/F ファイル名が書いてありません。");
						break;
					};
					switch(argv[iCount][2]){
					//--------
					//MMLファイルの指定
					case 'm' :
						//既に指定されている？
						if(OptionSW->MML.name[0]==0){
							iFlagFilnameExt=0;		//拡張子の有無　Reset
							iOptionChk=0;		
							while((cOption=argv[iCount][iOptionChk+3])!=0)
							{
								OptionSW->MML.name[iOptionChk]=cOption;
								if(cOption=='.'){iFlagFilnameExt=1;};
								iOptionChk++;
							};
							if(iFlagFilnameExt==0){
								OptionSW->MML.name[iOptionChk++]='.';	//拡張子を".mid"にする。
								OptionSW->MML.name[iOptionChk++]='m';	
								OptionSW->MML.name[iOptionChk++]='m';	
								OptionSW->MML.name[iOptionChk++]='l';	
							};
							OptionSW->MML.name[iOptionChk++]=0;	
						} else {
							opError("/F MMLファイルが2回以上指定されました。");
							break;
						};
						break;
					default :
						opError("/F");
						break;
					};
				break;
				//

				//--------
				//デフォルト
				default :
					opError("");
					break;
			};

		} else{

			//--------------
			//◆ファイル名	（スラッシュが無かった場合の処理）
			//既に指定されている？
			if(OptionSW->SMF.name[0]==0){
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
			} else {
				opError("SMFファイルが2回以上指定されました。");
				break;
			};

		};

		//--------------
		//◆次のオプション
		iCount++;
	};

	//----------------------------------
	//◆オプションで指定された事を処理する。

	//--------------
	//ヘルプ表示
	//ファイル名が書かれなかった場合も、ヘルプを表示する。
	if((OptionSW->fHelp==1)||(OptionSW->SMF.name[0]==0)){print_help();};

	//--------------
	//SMFファイルの指定が無かった場合
	if(OptionSW->MML.name[0]==0){
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
	//

	//	to do	その他のオプションを追加したときは、この辺に追記する。


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
	OptionSW.SMF.size=getStreamSize(OptionSW.SMF.stream);	//ファイルサイズの取得
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


#include "stdafx.h"

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
/*					出力										*/
/*																*/
/****************************************************************/
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
		fprintf(OptionSW->MML.stream,"$%X",cFread);
		if(iSize>1){
			fprintf(OptionSW->MML.stream,",");
		} else {
			fprintf(OptionSW->MML.stream,";");
		};
		iSize--;
	};

	fprintf(OptionSW->MML.stream," ");

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
//		整形処理
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	*unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void OutputBarCr(OPSW *OptionSW,unMMLinfo *unMML){

	//拍子が変わる？
	if(unMML->BT[(unMML->cntBT)+1].iTicksBT==unMML->iTotalTicksChk){
		fprintf(OptionSW->MML.stream,"\n	");
		unMML->iTicksBar=0;
		unMML->iTicksCR=0;
		unMML->cntBT++;
	};

	//整形処理
	if((unMML->iTicksCR)>=(unMML->BT[unMML->cntBT].iTicksOne<<2)){
		unMML->iTicksBar-=unMML->BT[unMML->cntBT].iTicksOne;
		unMML->iTicksCR-=(unMML->BT[unMML->cntBT].iTicksOne<<2);
		fprintf(OptionSW->MML.stream,"\n	");
	} else if((unMML->iTicksBar)>=(unMML->BT[unMML->cntBT].iTicksOne)) {
		unMML->iTicksBar-=unMML->BT[unMML->cntBT].iTicksOne;
		fprintf(OptionSW->MML.stream,"	| ");
	};

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
				int		iDot;
				long	lx;	//汎用
				long	ly;	//汎用

	//----------------------------------
	//■音長に変換
	iDot=0;
	while(iDot<14){	//付点は14個未満まで。
		lx =iTicks*(1<<iDot);
		ly =(1<<(iDot+1))-1;	// (lx/ly)で、付点がつかない場合の音長になる。
		if((lx % ly)==0){		// 但し、割り切れる事が前提。
			iLength=Ticks2Length(unMML,(lx/ly));
			if(iLength!=-1){
				break;
			};
		};
		iDot++;
	};

	//付点が14個以上つくようであれば、ticks表記
	if(iDot>=14){
		fputc('%',OptionSW->MML.stream);
		fprintf(OptionSW->MML.stream,"%d",iTicks);
	//音長＋付点の表示。
	} else{
		fprintf(OptionSW->MML.stream,"%d",iLength);
		while(iDot>0){
			fputc('.',OptionSW->MML.stream);
			iDot--;
		};
	};

};
//==============================================================
//		音長出力（デルタタイムのデコード）
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
				char	flagStart;
				int		iLength;			//音長変換用
				__int64	iLengthBT;			//音長変換用（拍子が変わるまで）

	//----------------------------------
	//■初期化
	flagStart=1;		//呼ばれた最初であるかを示すフラグ

	//----------------------------------
	//■音長の出力

	//いまのTicksが、小節を越えるか？
	iLength=(unMML->BT[unMML->cntBT].iTicksOne)-(unMML->iTicksBar);		//あと、何チックあるか？
	if(iLength<(int)unMML->iTicks){
		//最初でなければ、タイで繋ぐ
		if(flagStart==1){
			flagStart=0;
		} else {
			fprintf(OptionSW->MML.stream,"^");
		};
		Ticks2LengthEx(OptionSW,unMML,iLength);
		unMML->iTicks-=iLength;
		unMML->iTicksBar+=iLength;
		unMML->iTicksCR+=iLength;
		unMML->iTotalTicksChk+=iLength;
		OutputBarCr(OptionSW,unMML);	//整形
	};

	//いまのTicksが、拍子記号を越えるか？
	iLengthBT=(__int64)((unMML->BT[(unMML->cntBT)+1].iTicksBT)-(unMML->iTotalTicksChk));		//あと、何チックあるか？
	if(iLengthBT<(__int64)(unMML->iTicks)){
		//最初でなければ、タイで繋ぐ
		if(flagStart==1){
			flagStart=0;
		} else {
			fprintf(OptionSW->MML.stream,"^");
		};
		Ticks2LengthEx(OptionSW,unMML,(int)iLengthBT);
		unMML->iTicks-=(int)iLengthBT;
		unMML->iTicksBar+=(int)iLengthBT;
		unMML->iTicksCR+=(int)iLengthBT;
		unMML->iTotalTicksChk+=iLengthBT;
		OutputBarCr(OptionSW,unMML);	//整形
	};

	//iTicks が 0 になるまで繰り返す。
	while((unMML->iTicks)>0){

		//最初でなければ、タイで繋ぐ
		if(flagStart==1){
			flagStart=0;
		} else {
			fprintf(OptionSW->MML.stream,"^");
		};

		//全音符より大きかったら、まず全音符を出力する。
		if((unMML->iTicks)>=unMML->BT[unMML->cntBT].iTicksOne){
			Ticks2LengthEx(OptionSW,unMML,unMML->BT[unMML->cntBT].iTicksOne);
			unMML->iTicks-=unMML->BT[unMML->cntBT].iTicksOne;
			unMML->iTicksBar+=unMML->BT[unMML->cntBT].iTicksOne;
			unMML->iTicksCR+=unMML->BT[unMML->cntBT].iTicksOne;
			unMML->iTotalTicksChk+=unMML->BT[unMML->cntBT].iTicksOne;
		//全音符より小さくなったら、音長を計算する。
		} else {
			Ticks2LengthEx(OptionSW,unMML,unMML->iTicks);
			unMML->iTicksBar+=unMML->iTicks;
			unMML->iTicksCR+=unMML->iTicks;
			unMML->iTotalTicksChk+=unMML->iTicks;
			unMML->iTicks=0;
		};

		//整形のチェック（音長を出力した後で）
		OutputBarCr(OptionSW,unMML);

	};
};
//==============================================================
//		音符出力
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLinfo	*unMML		逆MML用構造体
//			unsigned char cVelocity	ヴェロシティー
//	●返値
//			無し
//==============================================================
void OutputNoteEx(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//■Local 変数
	int		iOctave;	//オクターブ計算用
	int		iCmpOctave;	//オクターブ差
	int		iNote;		//ノート計算用

	/*
	------------------------------------------------
	※	非常に無駄なコードを吐かれたので、削除。
	※	アセンブリ言語にて記述。
	------------------------------------------------
	//各スケール時の出力設定
	char	*strNoteGs[12]	={"c*","d" ,"d*","e" ,"e*","f" ,"g" ,"g*","a" ,"a*","b" ,"`c"};	//-6
	char	*strNoteDs[12]	={"c" ,"d" ,"d*","e" ,"e*","f" ,"g" ,"g*","a" ,"a*","b" ,"b*"};	//-5
	char	*strNoteAs[12]	={"c" ,"d" ,"d*","e" ,"e*","f" ,"g-","g" ,"a" ,"a*","b" ,"b*"};	//-4
	char	*strNoteEs[12]	={"c" ,"d-","d" ,"e" ,"e*","f" ,"g-","g" ,"a" ,"a*","b" ,"b*"};	//-3
	char	*strNoteB[12]	={"c" ,"d-","d" ,"e" ,"e*","f" ,"g-","g" ,"a-","a" ,"b" ,"b*"};	//-2
	char	*strNoteF[12]	={"c" ,"d-","d" ,"e-","e" ,"f" ,"g-","g" ,"a-","a" ,"b" ,"b*"};	//-1
	char	*strNoteC[12]	={"c" ,"c+","d" ,"d+","e" ,"f" ,"f+","g" ,"g+","a" ,"a+","b"};	//+0
	char	*strNoteG[12]	={"c" ,"c+","d" ,"d+","e" ,"f*","f" ,"g" ,"g+","a" ,"a+","b"};	//+1
	char	*strNoteD[12]	={"c*","c" ,"d" ,"d+","e" ,"f*","f" ,"g" ,"g+","a" ,"a+","b"};	//+2
	char	*strNoteA[12]	={"c*","c" ,"d" ,"d+","e" ,"f*","f" ,"g*","g" ,"a" ,"a+","b"};	//+3
	char	*strNoteE[12]	={"c*","c" ,"d*","d" ,"e" ,"f*","f" ,"g*","g" ,"a" ,"a+","b"};	//+4
	char	*strNoteH[12]	={"c*","c" ,"d*","d" ,"e" ,"f*","f" ,"g*","g" ,"a*","a" ,"b"};	//+5
	char	*strNoteFis[12]	={"c*","c" ,"d*","d" ,"e*","e" ,"f" ,"g*","g" ,"a*","a" ,"b"};	//+6

	char	**ptNote[13]	={
		strNoteGs,	//-6	[0]
		strNoteDs,	//-5	[1]
		strNoteAs,	//-4	[2]
		strNoteEs,	//-3	[3]
		strNoteB,	//-2	[4]
		strNoteF,	//-1	[5]
		strNoteC,	//+0	[6]
		strNoteG,	//+1	[7]
		strNoteD,	//+2	[8]
		strNoteA,	//+3	[9]
		strNoteE,	//+4	[10]
		strNoteH,	//+5	[11]
		strNoteFis,	//+6	[12]
	};
	*/

	//----------------------------------
	//■出力
	//何か音符を出力しているか？
	if(unMML->cNote<0x80){
		if(unMML->flagNote==0){
			fprintf(OptionSW->MML.stream,"^");
		} else if(unMML->flagNote==1){
			iOctave = unMML->cNote / 12;
			iNote	= unMML->cNote % 12;

			//ヴェロシティー
			if(unMML->cVelocity!=unMML->cVelo){
				unMML->cVelocity=unMML->cVelo;
				fprintf(OptionSW->MML.stream,"v%d",unMML->cVelocity);
			}

			//オクターブ
			iCmpOctave=(unMML->iOctave)-iOctave;
			if(iCmpOctave!=0){
				unMML->iOctave=iOctave;
			}
			if(iCmpOctave==+2){
				fprintf(OptionSW->MML.stream,"<<");
			}else if(iCmpOctave==+1){
				fprintf(OptionSW->MML.stream,"<");
			}else if(iCmpOctave==-1){
				fprintf(OptionSW->MML.stream,">");
			}else if(iCmpOctave==-2){
				fprintf(OptionSW->MML.stream,">>");
			}else if(iCmpOctave!=0){
				fprintf(OptionSW->MML.stream,"o%d",iOctave);
			}

			//音程
			//無駄なソースを吐かれたので、アセンブリ言語で処理する。
			//fprintf(OptionSW->MML.stream,ptNote[6][iNote]);
			NoteScaleOut(OptionSW->MML.stream,0,iNote);
		}
	//何もない場合（0xFF）
	} else {
		fprintf(OptionSW->MML.stream,"r");
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

	//----------------------------------
	//■Local 変数

	//----------------------------------
	//■初めての出力であれば、チャンネルを出力する。
	if(unMML->flagCh==0){
		unMML->flagCh=1;
		fprintf(OptionSW->MML.stream,"\nTime(1:1:0);\n");
		if((unMML->iChannel)>0){
			if(unMML->SMF_Header.format==0){
				fprintf(OptionSW->MML.stream,"TR(%d)",unMML->iChannel);	//format 0だったら、トラックも吐く
			};
			fprintf(OptionSW->MML.stream,"	CH(%d);	q100 h-1\n	",unMML->iChannel);
		};
	};

	//----------------------------------
	//■ノート ro 休符 or タイを出力
	if(unMML->iTicks>0){

		//音符を出力
		OutputNoteEx(OptionSW,unMML);

		//音長を出力
		OutputTicks(OptionSW,unMML);

		//出力したので、リセットする。
		unMML->flagNote=0;	//今後は、タイを出力
		unMML->iTicks=0;	//音長は０

	};

};

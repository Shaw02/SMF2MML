
#include "stdafx.h"

/****************************************************************/
/*			外部宣言											*/
/****************************************************************/
extern	void not_smf(const char *stMsg);

/****************************************************************/
/*					定数										*/
/****************************************************************/
#define	dwMTrk	0x6B72544D		//'MTrk'を16進数にしてみた。

//----------------------------------
//■Gloval定数
	const	char*	const	strTrack[2]		={	"TR","トラック"};
	const	char*	const	strChannel[2][2]={	"CH("		,	");	q100 h-1\n	",
												"チャンネル(",	");	ゲート100	h-1\n	"};
	const	char*	const	strTime[2]		={	"\nTime(1:1:0);\n	","\n時間（１：１：０）;\n	"};


	const	char*	const	strRest[2]	=	{	"r",	"ん"};	
	const	char*	const	strNote[2][7]=	{	"c",	"d",	"e",	"f",	"g",	"a",	"b",
																"ド",	"レ",	"ミ",	"ファ",	"ソ",	"ラ",	"シ"};
	const	char*	const	strSharp[2]	=	{	"+",	"＃"};
	const	char*	const	strFlat[2]	=	{	"-",	"♭"};
	const	char*	const	strVelo[2]	=	{	"v",	"音量"};
	const	char*	const	strOctave[2][6]	={	"o",	"<<",	"<",	"",	">",	">>",
												"音階",	"↓↓",	"↓",	"",	"↑",	"↑↑"};

	const	char			cSharp[6]				={11,4,9,2,7,0};	//調合
	const	char			cFlat[6]				={5,0,7,2,9,4};		//調合

	const	char*	const	strMetaText[2]			={"MetaText=",		"コメント"};
	const	char*	const	strMetaCopyright[2]		={"Copyright=",		"作者"};
	const	char*	const	strMetaTrackName[2]		={"TrackName=",		"曲名"};
	const	char*	const	strMetaTempo[2]			={"Tempo",			"テンポ"};
	const	char*	const	strMetaTimeSignature[2]	={"TimeSignature",	"拍子"};


//###############################################################
//#					汎用サブルーチン							#
//###############################################################
/****************************************************************/
/*					初期化										*/
/****************************************************************/
//==============================================================
//		イニシャライズ
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//			無し
//==============================================================
void unMMLcompiler::initTR(){

	//----------------------------------
	//■Local 変数
	__int32		stName;		//チャンク名

	//ヘッダー読み込み
	SMF->read((char *)&stName,4);
	SMF->read((char *)&iTrackSize,4);
	endian_b2l_D(&iTrackSize);			//チャンクサイズを保存
	lOffsetTR=SMF->tellg();				//今のファイルポインタを保存
//	printf("offsetTR=%d\n",unMML->lOffsetTR);	//Debug用

	if(stName!=dwMTrk){
		not_smf("MTrkチャンクがありません。\n");
	};

}
//==============================================================
//		イニシャライズ
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//			無し
//==============================================================
void unMMLcompiler::initCH(){

	//トラックの先頭にポインタを移す
	SMF->seekg(lOffsetTR,ios::beg);

	//各項目の初期化
	SMF_Event.flagEoT=0;
	SMF_Event.cRunningStatus=0;
	SMF_Meta.CounterReset();

	iTicks=0;
	iTotalTicks=0;
	iTotalTicksChk=0;
	iTicksBar=0;
	iTicksCR=0;
	nowBeatTime=SMF_Meta.nowTimeTicks();

//	iLength=4;
//	iGate=0;
	cOctave=80;
	cVelocity=255;

	cNote=0xFF;	//ノート
	flagNote=0;	//出力状態
	flagCh=0;

}
//###############################################################
//#					MML出力部									#
//###############################################################
/****************************************************************/
/*					連続出力									*/
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
void unMMLcompiler::OutputHex(){

	//----------------------------------
	//■Local 変数
	unsigned	int	iSize = SMF_Event.iSize;

	*MML << hex;
	while(iSize>0){
		*MML << "$" << (unsigned int)SMF_Event.Read() ;
		if(iSize>1){
			MML->put(',');
		};
		iSize--;
	};
	*MML << dec;

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
void unMMLcompiler::OutputStrings(){

	//----------------------------------
	//■Local 変数
	unsigned	int	iSize = SMF_Event.iSize;

	MML->put('"');

	while(iSize>0){
		MML->put(SMF_Event.Read());
		iSize--;
	};

	MML->put('"');

};
//==============================================================
//		可変長出力
//--------------------------------------------------------------
//	●引数
//			unsigned	__int32	var
//	●返値
//			無し
//==============================================================
void unMMLcompiler::EncodeVariable(unsigned	__int32	var){

	unsigned	int		count=0;		//読み込み回数カウント用
				char	cData[4];

	//----------------------------------
	//■可変長エンコード
	do{
		cData[count]=var&0x7F;
		if((var>>=7)==0){
			break;
		}
		cData[count]|=0x80;
		count++;
	} while((count<4));

	//■可変長出力
	*MML << hex;
	do{
		*MML << cData[count];
		count--;
		if(count>0){
			MML->put(',');
		}
	} while((count>0));
	*MML << dec;

};
/****************************************************************/
/*				Decode:	Channel Voice Message					*/
/****************************************************************/
//==============================================================
//		整形処理
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//			無し
//==============================================================
void unMMLcompiler::OutputBarCr(){

	//拍子が変わる？
	if(SMF_Meta.nextBeatTime()==iTotalTicksChk){
		*MML << "\n	";
		iTicksBar=0;
		iTicksCR=0;
		SMF_Meta.CounterInc();
		nowBeatTime=SMF_Meta.nowTimeTicks();
		*MML << "//Rhythm = " << (int)SMF_Meta.nowNumerator() << "/" << (int)SMF_Meta.nowDenominator() << "\n	";
		//Debug用：		*MML << "/*" << (int)SMF_Meta.nowTimeTicks() << "*/";
	};

	//整形処理
	//小節処理をしない場合は、機能させない。
	if(OptionSW->iBar!=0){

		//バーの挿入
		if(iTicksBar>=nowBeatTime) {
			iTicksBar-=nowBeatTime;
			*MML << "	|	";		//Debug用： /*" << (int)iTicksBar << "*/";
		};

		//改行するか？
		//オプション /C0 だったら、音符毎に改行。 -1 で改行無し。
		if((iTicksCR)>=(nowBeatTime*OptionSW->iCR)) {
			iTicksCR-=(nowBeatTime*OptionSW->iCR);
			*MML << "\n	";
		};
	};
};
/****************************************************************/
/*				Decode:	Note Length								*/
/****************************************************************/
//==============================================================
//		音長出力
//--------------------------------------------------------------
//	●引数
//			unMMLcompiler	*unMML		逆MML用構造体
//			int			iTicks		チック
//	●返値
//			int			iLength		音長	-1だったら、余り有り
//==============================================================
int unMMLcompiler::Ticks2Length(int iTicksL){

	//----------------------------------
	//■Local 変数
	unsigned	int		iZenlenght	=(SMF_Header.timebase)*4;	//これは、4固定
				int		iLength;								//音長

	iLength=iZenlenght / iTicksL;
	if((iZenlenght % iTicksL)!=0){
		iLength=-1;
	};

	return(iLength);
};
//==============================================================
//		音長出力
//--------------------------------------------------------------
//	●引数
//			int			iTicks		チック
//	●返値
//			無し
//==============================================================
void unMMLcompiler::Ticks2LengthEx(int iTicksL){

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
		lx =iTicksL*(1<<iDot);
		ly =(1<<(iDot+1))-1;	// (lx/ly)で、付点がつかない場合の音長になる。
		if((lx % ly)==0){		// 但し、割り切れる事が前提。
			iLength=Ticks2Length(lx/ly);
			if(iLength!=-1){
				break;
			};
		};
		iDot++;
	};

	//付点が14個以上つくようであれば、ticks表記
	if(iDot>=14){
		MML->put('%');
		*MML << iTicksL;
	//音長＋付点の表示。
	} else{
		*MML << iLength;
		while(iDot>0){
			MML->put('.');
			iDot--;
		};
	};

};
//==============================================================
//		音長出力（デルタタイムのデコード）
//--------------------------------------------------------------
//	●引数
//			unsigned	__int32		iTicksL	音長
//	●返値
//			無し
//==============================================================
	//----------------------------------
	//■Gloval定数

	const	char*	const	strTai[2]	=	{"^","ー"};

void unMMLcompiler::OutputTicks(/*unsigned	__int32		iTicksL*/){

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

	//小節処理しない場合は、整形処理をしない。
	if(OptionSW->iBar!=0){
		//いまのTicksが、小節を越えるか？
		iLength=nowBeatTime-iTicksBar;		//あと、何チックあるか？
		if(iLength<(int)iTicks){
			//最初でなければ、タイで繋ぐ
			if(flagStart==1){
				flagStart=0;
			} else {
				*MML << strTai[OptionSW->cJpn];
			};
			Ticks2LengthEx(	iLength);
			iTicks			-=iLength;
			iTicksBar		+=iLength;
			iTicksCR		+=iLength;
			iTotalTicksChk	+=iLength;
			OutputBarCr();	//整形
		};
	};

		//いまのTicksが、拍子記号を越えるか？
		iLengthBT=(__int64)((SMF_Meta.nextBeatTime())-(iTotalTicksChk));		//あと、何チックあるか？
		if(iLengthBT<(__int64)iTicks){
			//最初でなければ、タイで繋ぐ
			if(flagStart==1){
				flagStart=0;
			} else {
				*MML << strTai[OptionSW->cJpn];
			};
			Ticks2LengthEx(		(int)iLengthBT);
			iTicks			-=	(int)iLengthBT;
			iTicksBar		+=	(int)iLengthBT;
			iTicksCR		+=	(int)iLengthBT;
			iTotalTicksChk	+=		iLengthBT;
			OutputBarCr();	//整形
		};
	//iTicksL が 0 になるまで繰り返す。
	while(iTicks>0){

		//最初でなければ、タイで繋ぐ
		if(flagStart==1){
			flagStart=0;
		} else {
			*MML << strTai[OptionSW->cJpn];
		};

		//全音符より大きかったら、まず全音符を出力する。
		if(iTicks>=nowBeatTime){
			Ticks2LengthEx(		nowBeatTime);
			iTicks			-=	nowBeatTime;
			iTicksBar		+=	nowBeatTime;
			iTicksCR		+=	nowBeatTime;
			iTotalTicksChk	+=	nowBeatTime;
		//全音符より小さくなったら、音長を計算する。
		} else {
			Ticks2LengthEx(iTicks);
			iTicksBar		+=iTicks;
			iTicksCR		+=iTicks;
			iTotalTicksChk	+=iTicks;
			iTicks			=0;
		};

		//整形のチェック（音長を出力した後で）
		OutputBarCr();

	};
};
/****************************************************************/
/*				Decode:	Note & Scale							*/
/****************************************************************/
//==============================================================
//		音符出力	一致の検索
//--------------------------------------------------------------
//	●引数
//			*scale
//			caNote
//	●返値
//			一致した配列番号
//==============================================================
char unMMLcompiler::NoteScas(char *cScale, char caNote){

	int	i=0;

	while(i<7){
		if(cScale[i]==caNote){
			return(i);
		}
		i++;
	}

	return(-1);

//--------------------------------------------------------------
//	以下のように思ったけど、ストリングス命令が早いのは、8086時代らしい。
//	Pentiumは、マイクロコードに分解されるので、普通にCで書く方が早い。
//--------------------------------------------------------------
//	__asm{
//		mov		edi,cScale		;edi = cScale
//		movzx	eax,cNote		;eax = cNote
//		mov		ecx,7			;ecx=7	//(配列の数)
//
//		repne	scasb			;//配列内容の一括全check
//		je		NoteScas_Agree
//
//		;/*一致しない場合*/
//		or		eax,-1			;return(-1);
//		jmp		NoteScas_Lend
//		;/*一致があった。*/
//NoteScas_Agree:	
//		mov		eax,6
//		sub		eax,ecx			;return(6-eax)
//NoteScas_Lend:
//	};
//--------------------------------------------------------------

};
//==============================================================
//		音符出力
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLcompiler	*unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void unMMLcompiler::OutputNoteEx(){

	//----------------------------------
	//■Local 変数
			char	caOct;			//オクターブ計算用
			char	cCmpOctave;		//オクターブ差
			char	caNote;			//ノート計算用
			char	cN;				//汎用 (Note)
			char	cS=0;			//汎用 (スケール)
			char	cAccidental=0;	//臨時記号
			char	cScale[7]		={0,2,4,5,7,9,11};	//ダイアトニック（これは、調によって変える）

	//----------------------------------
	//■出力
	//何か音符を出力しているか？
	if(cNote<0x80){
		if(flagNote==0){
			*MML << strTai[OptionSW->cJpn];
		} else if(flagNote==1){
			caOct	= cNote / 12;
			caNote	= cNote % 12;

			//------------
			//ヴェロシティー
			if(cVelocity!=cVelo){
				cVelocity=cVelo;
				*MML << strVelo[OptionSW->cJpn] << (int)cVelocity;
			}

			//------------
			//オクターブ
			cCmpOctave=caOct-(cOctave)+3;
			cOctave=caOct;

			if((cCmpOctave<=5)&&(cCmpOctave>=1)){
				*MML << strOctave[OptionSW->cJpn][cCmpOctave];
			}else{
				*MML << strOctave[OptionSW->cJpn][0] << (int)caOct;
			}

			//------------
			//音程
//			NoteScaleOut(OptionSW->MML.stream,0,cNote);
			// to do	調合に併せて配列を操作
			//配列中に一致があるか探す
			if((cN=NoteScas(cScale,caNote))==-1){
				// to do	＃か♭系か？
				// to do	cNote+1が、調の半音記号と一致するか？
				//臨時記号付き
				cAccidental=1;
				cN=NoteScas(cScale,caNote-1);
			};
			//出力
			*MML << strNote[OptionSW->cJpn][cN];
			if(cAccidental==1){
				*MML << strSharp[OptionSW->cJpn];
			} else if(cAccidental==-1){
				*MML << strFlat[OptionSW->cJpn];
			};
		};
	//----------------
	//何もない場合（0xFF）
	} else {
		*MML << strRest[OptionSW->cJpn];
	};

};
//==============================================================
//		音符＆音長出力
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLcompiler	*unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void unMMLcompiler::OutputNote(){

	//----------------------------------
	//■ノート ro 休符 or タイを出力
	if(iTicks>0){

		//音符を出力
		OutputNoteEx();

		//音長を出力
		OutputTicks();

		//出力したので、リセットする。
		flagNote=0;				//今後は、タイを出力
	};
};
/****************************************************************/
/*				Decode:	Channel Voice Message					*/
/****************************************************************/
//==============================================================
//		ノート・ｏｎ／ｏｆｆのデコード
//--------------------------------------------------------------
//	●引数
//					OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//					unMMLcompiler	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void unMMLcompiler::DecodeNote(){

	//----------------------------------
	//■Local 変数

			unsigned	__int32	iLength		= 0;				//音長
						char	flagKeyOn	= 0;				//他にkey onがあった。	
						char	flagKeyOff	= 0;				//key offが来た。	
	const	long				fptStream	= SMF->tellg();		//ファイルポインタの保存

	//ステータスバイト・データバイトを保存
	const				char	cTempEoT	= SMF_Event.flagEoT;	//End of Trackのフラグ
	const	unsigned	char	cTempStatus	= SMF_Event.cStatus;	//
	const	unsigned	char	cTempData1	= SMF_Event.cData1;		//
	const	unsigned	char	cTempData2	= SMF_Event.cData2;		//


	//--------------
	//Note Offを検索する
	while((flagKeyOff==0)&&(SMF_Event.flagEoT==0)){
		iLength+=SMF_Event.EventRead();
		SMF_Event.Seek();	//ポインタを進める(0checkは、ルーチン内でしている。)

		//他にコマンドが来た？
		if(((SMF_Event.cStatus==cTempStatus)&&(SMF_Event.cData2!=0)) ||
			((SMF_Event.cStatus>=0xA0)&&(SMF_Event.cStatus<0xF0)&&((SMF_Event.cStatus&0x0F)==(cTempStatus&0x0F))))
		{
			flagKeyOn=1;
		};
		//Note Offが来た？
		if(((SMF_Event.cStatus==(cTempStatus&0x8F))&&(SMF_Event.cData1==cTempData1)) ||
			((SMF_Event.cStatus==(cTempStatus&0x9F))&&(SMF_Event.cData1==cTempData1)&&(SMF_Event.cData2==0))) {
			flagKeyOff=1;
		};
	};

	//他にkey onがあったら。
	if(flagKeyOn==1){
		*MML << " Sub{";
		cNote=cTempData1;	//とりあえず出力
		cVelo=cTempData2;	
		flagNote=1;			//Key On
		//音符を出力
		OutputNoteEx();
		//音長を出力
		Ticks2LengthEx(iLength);
		*MML << "}";
		cNote=0xFF;
	}else{
		cNote=cTempData1;
		cVelo=cTempData2;	
		flagNote=1;	//Key On
	}

	//--------------
	//復帰
	SMF_Event.flagEoT	= cTempEoT;		//End of Trackのフラグ
	SMF_Event.cStatus	= cTempStatus;
	SMF_Event.cData1	= cTempData1;
	SMF_Event.cData2	= cTempData2;

	//ポインタを戻す
	SMF->StreamPointerMove(fptStream);
}
//==============================================================
//		コントロールチェンジのデコード
//--------------------------------------------------------------
//	●引数
//					OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//					unMMLcompiler	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void unMMLcompiler::DecodeControlChange(){

	//コントロール別処理
	switch(SMF_Event.cData1){
		//--------------
		//Modulation
		case(1):
			*MML << "M(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//ポルタメントタイム
		case(5):
			*MML << "PT(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//DataMSB
		case(6):
			*MML << "DataMSB(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//Main Volume
		case(7):
			*MML << "V(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//Panpot
		case(10):
			*MML << "P(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//Expression
		case(11):
			*MML << "EP(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//ポルタメントスイッチ
		case(65):
			*MML << "PS(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//リバーブ
		case(91):
			*MML << "Reverb(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//コーラス
		case(93):
			*MML << "Chorus(" << (int)SMF_Event.cData2 <<"); ";
			break;
		//--------------
		//セレステ
		case(94):
			*MML << "VAR(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//その他
		default:
			*MML << "y" << (int)SMF_Event.cData1 << "," << (int)SMF_Event.cData2 << "; ";
			break;
	};

};
/****************************************************************/
/*				Decode:	Header & Meta Event Search				*/
/****************************************************************/
//==============================================================
//		メタ・イベントの検索
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//			無し
//	●処理
//			メタイベントを探し、イベントに応じた処理
//==============================================================
void unMMLcompiler::MetaSearch(){

	//----------------------------------
	//■Local 変数
			unsigned	__int32	iDeltaTime;					//読み込んだ可変長数値
			unsigned	char	cData[4];					//データ
	const				long	fptStream = SMF->tellg();	//ファイルポインタ

	//全トラックのサーチ
	iTrack=0;
	iChannel=0;
	while((iTrack)<(SMF_Header.track)){
		initTR();
		initCH();
		//EOTがあるまでループ
		while(SMF_Event.flagEoT==0){
			iDeltaTime = SMF_Event.EventRead();
			iTicks+=iDeltaTime;					//現デルタタイムに加算
			iTotalTicks+=iDeltaTime;			//総デルタタイムに加算
			
			//メタイベント？
			if(SMF_Event.cStatus==0xFF){
				switch(SMF_Event.cData1){
					//--------------
					//拍子
					case(0x58):
						SMF_Event.DataRead((char *)&cData);
						SMF_Meta.addRhythm(iTotalTicks,cData[0],cData[1],SMF_Header.timebase);
						//nowBeat.iTicksBT = iTotalTicks;
						//nowBeat.iTicksOne= (cData[0]*(SMF_Header.timebase<<2)/(1<<cData[1]));
						//SMF_Meta.Add(&nowBeat);
						break;
					//--------------
					//調号
					case(0x59) :
						SMF_Event.DataRead((char *)&cData);
						SMF_Meta.addKey(iTotalTicks,(char)cData[0],cData[1]);
//						nowBeat.iTicksBT = unMML->iTotalTicks;
//						nowBeat.cKey	= (char)cData[0];
//						nowBeat.cmi		= cData[1];
//						nowBeat.cmi |=	0x80;
//						AddBeat(OptionSW,unMML,&nowBeat);
//						nowBeat.cmi &=	0x7F;
						break;
					//--------------
					//処理しないメタイベントの場合
					default:
						SMF_Event.Seek();
						break;
				};
			//メタイベント以外は無視
			} else {
				SMF_Event.Seek();
			};
		};
		iTrack++;
	};

	//ソート
	SMF_Meta.sort();

	//ポインタを戻す
	SMF->StreamPointerMove(fptStream);

};
//==============================================================
//		ヘッダー処理
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//			無し
//==============================================================
void unMMLcompiler::Header(){

	//----------------------------------
	//■Local 変数

	*MML	<< "//--------------------------------------" << endl
			<< "//		SMF2MML" << endl
			<< "//--------------------------------------" << endl
			<< "//MThd Infomation :" << endl
			<< "//	Size = " << SMF_Header.size << endl
			<< "//	Format = " << (unsigned int)SMF_Header.format << endl
			<< "//	Track = " << (unsigned int)SMF_Header.track << endl
			<< "//" << endl
			<< "System.ControllerShift = 0;" << endl
			<< "System.TimeBase = "<< SMF_Header.timebase << ";" << endl
			<< endl << endl << endl;

	//まずは、4/4拍って事にしておく。
	SMF_Meta.addRhythm(0,4,2,SMF_Header.timebase);

	//メタイベントのサーチ
	MetaSearch();

	//最後
	SMF_Meta.addRhythm(0x7FFFFFFFFFFFFFFF,4,2,SMF_Header.timebase);

	//to do	key, Beatの補完

	SMF_Meta.CounterReset();
	while(!SMF_Meta.CounterCheck()){
		cout	<<	"Pointer:"	<< (int)	SMF_Meta.BeatTime()		<<	"	"
				<<	"Length:"	<<			SMF_Meta.nowTimeTicks()	<<	"	"
				<<	"Key:"		<<	(int)	SMF_Meta.nowKey()		<<	"	"
				<<	"mi:"		<<	(int)	SMF_Meta.nowMinor()		<<	endl;
		SMF_Meta.CounterInc();
	};
};

/****************************************************************/
/*				Decode:	MIDI Event								*/
/****************************************************************/
//==============================================================
//		チャンネル・ボイス・メッセージのデコード
//--------------------------------------------------------------
//	●引数
//				OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//				unMMLcompiler	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
	//----------------------------------
	//■Gloval定数
	const	char*	const	strVoice[2]={"Voice","音色"};


void unMMLcompiler::DecodeChannelVoiceMessage(){

	//----------------------------------
	//■Local 変数
	unsigned	int			iChannel;		//チャンネル情報
	unsigned	char		cCommand;		//コマンド種類

	//--------------
	//チャンネルとコマンドに分解する
	iChannel = SMF_Event.cStatus & 0x0F;		//チャンネル
	cCommand = SMF_Event.cStatus >> 4;			//コマンド

	//--------------
	//コマンド別処理
	switch(cCommand){
		//--------------
		//Note Off Message
		case(0x08) :
//			OptionSW->MML.writePrintf("NoteOff(%d,%d); ",unMML->SMF_Event.cData1,unMML->SMF_Event.cData2);
			if(SMF_Event.cData1==cNote){
				cNote=0xFF;			//Note Off (休符)
			};
			break;
		//--------------
		//Note On Message
		case(0x09) :
//			OptionSW->MML.writePrintf("NoteOn(%d,%d); ",unMML->SMF_Event.cData1,unMML->SMF_Event.cData2);
			if((SMF_Event.cData1==cNote)&&(SMF_Event.cData2==0)){
				cNote=0xFF;			//Note Off (休符)
			} else {
				DecodeNote();		//Note On
			};
			break;
		//--------------
		//Polyhonic Key Pressure 
		case(0x0A) :
			*MML << "DirectSMF($A0+Channel-1," << (int)SMF_Event.cData1 << "," << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//Control Change Message
		case(0x0B) :
			DecodeControlChange();
			break;
		//--------------
		//Program Change Message
		case(0x0C) :
			*MML << strVoice[OptionSW->cJpn] << "(" << (int)SMF_Event.cData1+1 << "); ";
			break;
		//--------------
		//Channel Pressure 
		case(0x0D) :
			*MML << "DirectSMF($D0+Channel-1," << (int)SMF_Event.cData1 << "); ";
			break;
		//--------------
		//Pitch Bend Change Message
		case(0x0E) :
			*MML << "PitchBend(" << ((((SMF_Event.cData2)<<7)+(SMF_Event.cData1))-8192) << "); ";
			break;
		//--------------
		//それら以外
		default:
			not_smf("無効なChannel Voice Message。\n");
			break;
	};

};
//==============================================================
//		システム・エクスクルーシブ・メッセージのデコード
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLcompiler	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void unMMLcompiler::DecodeSystemExcusiveMessage(){

	*MML << "SysEx = $F0,";
	OutputHex();
	*MML << ";\n	";

};
//==============================================================
//		システム・コモン・メッセージのデコード
//--------------------------------------------------------------
//	●引数
//			OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//			unMMLcompiler	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void unMMLcompiler::DecodeSystemCommonMessage(){

	*MML << "SysEx = ";
	OutputHex();
	*MML << ";\n	";

};
//==============================================================
//		メタイベントのデコード
//--------------------------------------------------------------
//	●引数
//				OPSW		*OptionSW	オプションスイッチ構造体のポインタ
//				unMMLcompiler	unMML		逆MML用構造体
//	●返値
//			無し
//==============================================================
void unMMLcompiler::DecodeMetaEvent(){

	//----------------------------------
	//■Local 変数
	unsigned	char	cData[4];	//データ
	unsigned	int		iData;		//汎用

	//イベント別処理
	switch(SMF_Event.cData1){
		//--------------
		//シーケンス番号
		case(0x00) :
			if(SMF_Event.iSize==2){
				*MML << "DirectSMF($FF,$00,$02,";
				OutputHex();
				*MML << "); ";
			} else {
				not_smf("無効なシーケンス番号。\n");
			}
			break;
		//--------------
		//テキスト
		case(0x01) :
			*MML << strMetaText[OptionSW->cJpn] << "{";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//著作権表示
		case(0x02) :
			*MML << strMetaCopyright[OptionSW->cJpn] << "{";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//シーケンス名／トラック名
		case(0x03) :
			*MML << strMetaTrackName[OptionSW->cJpn] << "{";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//楽器名
		case(0x04) :
			*MML << "InstrumentName={";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//歌詞
		case(0x05) :
			*MML << "Lyric={";
			OutputStrings();
			*MML << "}; ";
			break;
		//--------------
		//マーカ
		case(0x06) :
			*MML << "Marker={";
			OutputStrings();
			*MML << "}; ";
			break;
		//--------------
		//キューポイント
		case(0x07) :
			*MML << "CuePoint={";
			OutputStrings();
			*MML << "}; ";
			break;
		//--------------
		//MIDIチャンネルプリフィックス
		case(0x20) :
			if(SMF_Event.iSize==1){
				SMF_Event.DataRead((char *)&cData);
				*MML << "ChannelPrefix(" << (int)cData[0] << "); ";
			} else {
				not_smf("無効なチャンネルプリフィックス。\n");
			}
			break;
		//--------------
		//ポート番号
		case(0x21) :
			if(SMF_Event.iSize==1){
				SMF_Event.DataRead((char *)&cData);
				*MML << "Port(" << (int)cData[0] << "); ";
			} else {
				not_smf("無効なポート番号。\n");
			}
			break;
		//--------------
		//エンドオブトラック
		case(0x2F) :
			if(SMF_Event.iSize==0){
				//チャンネルの逆MMLコンパイルを終わる。
				SMF_Event.flagEoT=1;
			} else {
				not_smf("無効なEnd of Track。\n");
			}
			break;
		//--------------
		//テンポ
		case(0x51) :
			if(SMF_Event.iSize==3){
				SMF_Event.DataRead((char *)&cData);
				*MML << strMetaTempo[OptionSW->cJpn] << "(" << (60000000/((cData[0]<<16)+(cData[1]<<8)+(cData[2]))) << ");\n	";
			} else {
				not_smf("無効なTempo。\n");
			}
			break;
		//--------------
		//SMPTEオフセット
		case(0x54) :
			if(SMF_Event.iSize==5){
				*MML << "DirectSMF($FF,$54,$05,";
				OutputHex();
				*MML << "); ";
			} else {
				not_smf("無効なSMPTEオフセット。\n");
			}
			break;
		//--------------
		//拍子
		case(0x58) :
			if(SMF_Event.iSize==4){
				SMF_Event.DataRead((char *)&cData);
				iData=1<<cData[1];
				//解析は前処理でやるが、表示はここでする。
				*MML << strMetaTimeSignature[OptionSW->cJpn] << "(" << (int)cData[0] << "," << iData << ");\n	";
			} else {
				not_smf("無効な拍子。\n");
			}
			break;
		//--------------
		//調号	（to do 先ずは、DirectSMF。TimeKeyFlagで対応できる？）
		case(0x59) :
			if(SMF_Event.iSize==2){
				*MML << "DirectSMF($FF,$59,$02,";
				OutputHex();
				*MML << ");\n	";
			} else {
				not_smf("無効な調号。\n");
			}
			break;
		//--------------
		//シーケンサ固有のメタイベント（未対応）
		case(0x7F) :
			*MML << "/*　DirectSMF($FF,$7F,";
			EncodeVariable(SMF_Event.iSize);
			OutputHex();
			*MML << "); */\n	";
			break;
		//--------------
		//
		default:
			not_smf("無効なMeta Event\n");
			break;
	};

};
/****************************************************************/
/*				Decode:	Main Routine							*/
/****************************************************************/
//==============================================================
//		１コマンドのデコード	（メインデコード）
//--------------------------------------------------------------
//	●引数
//			OPSW	*OptionSW	オプションスイッチ構造体のポインタ
//				unMMLcompiler	unMML		逆MML用構造体
//	●返値
//			__int32	iDeltaTime	デルタタイム
//==============================================================
void unMMLcompiler::Decode1Command(){

	//----------------------------------
	//■Local 変数
	unsigned	__int32		iDeltaTime;		//読み込んだ可変長数値
	unsigned	char		cChannel;		//チャンネル情報
	unsigned	char		cCommand;		//コマンド種類

	//MIDIイベント読み込み
	iDeltaTime = SMF_Event.EventRead();
	iTicks+=iDeltaTime;				//現デルタタイムに加算
	iTotalTicks+=iDeltaTime;		//総デルタタイムに加算

	//----------------------
	//●コマンド別処理

	//--------------
	//チャンネルとコマンドに分解する
	cChannel = SMF_Event.cStatus & 0x0F;	//チャンネル
	cCommand = SMF_Event.cStatus >> 4;		//コマンド

	//--------------
	//◆出力しない場合も、マスクする。
	if(	((SMF_Event.cStatus<0xF0)&&(iChannel!=(cChannel+1)))
	||	((SMF_Event.cStatus>=0xF0)&&(iChannel!=0))	)
		{
			SMF_Event.cStatus=0x00;
		};

	//--------------
	//出力がマスクされた？
	if(SMF_Event.cStatus==0x00){
		SMF_Event.Seek();

	//出力がある場合。
	}else{
		//初めての出力であれば、チャンネルを出力する。
		if(flagCh==0){
			flagCh=1;
			*MML << strTime[OptionSW->cJpn];
			//Channel 1～
			if((iChannel)>0){
				if(SMF_Header.format==0){
					*MML << strTrack[OptionSW->cJpn] << iChannel;	//format 0だったら、トラックも吐く
				};
				*MML << strChannel[OptionSW->cJpn][0] << iChannel << strChannel[OptionSW->cJpn][1];
			};
		};

		//--------------
		//◆いままでの音符・休符の出力
		//但し、EoT時はチャンネル出力時のみ行う。
		if(	((SMF_Event.flagEoT==0)&&(SMF_Event.cStatus!=0x00))
		||	((SMF_Event.flagEoT==1)&&(flagCh==1))&&(iChannel>0)	)
		{
			OutputNote();
		};

		//--------------
		//◆Channel Voice Message？
		if(SMF_Event.cStatus<0xF0){

			//--------------
			//チャンネル・ボイス・メッセージ
			DecodeChannelVoiceMessage();

		//--------------
		//◆System Excusive Message
		//◆System Common Message
		//◆Meta Event
		} else {

			//--------------
			//メッセージ別処理
			switch(SMF_Event.cStatus){
				//--------------
				//システム・エクスクルーシブ・メッセージ
				case(SEXM):
					DecodeSystemExcusiveMessage();
					break;
				//--------------
				//システム・コモン・メッセージ
				case(SCM):
					DecodeSystemCommonMessage();
					break;
				//--------------
				//メタ・イベント
				case(MEM):
					DecodeMetaEvent();
					break;
				//--------------
				//その他
				default:
					not_smf("無効なMIDIイベントを検出しました。\n");
					break;
			};	//End Switeh

		};	//EndIf	SysEx, Meta

	};	//EndIf	Mask

};
//==============================================================
//		アンコンパイル
//--------------------------------------------------------------
//	●引数
//			無し
//	●返値
//			MMLファイルにMML文字列を入れてゆく
//==============================================================
void unMMLcompiler::uncompile(){

	//----------------------------------
	//■Local 変数
//	unMMLcompiler	unMML(OptionSW);	//逆MMLを処理するための構造体

	//----------------------------------
	//■ヘッダー処理
	Header();		//MThdの内容を表示
					//初期解析（メタイベントの処理）

	//----------------------------------
	//■各MTrkの処理
	iTrack=0;
	while(iTrack<SMF_Header.track){
		//トラック毎の初期化
		initTR();

		//MMLでの、トラックの初期化
		*MML	<< "//--------------------------------------" << endl
				<< "//MTrk Infomation :" << endl
				<< "//	Size = " << iTrackSize << endl
				<< endl
				<< strTrack[OptionSW->cJpn] << "(" << iTrack << ");" << endl;

		//チャンネル毎の処理
		iChannel=0;
		while(iChannel<=16){
			//チャンネル毎の初期化
			initCH();
			//End of Trackが来るまで処理
			while(SMF_Event.flagEoT==0){
				Decode1Command();
			};
			iChannel++;
		};
		*MML << endl << endl;
		iTrack++;
	};
};


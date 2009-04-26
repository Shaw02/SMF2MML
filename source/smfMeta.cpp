
#include "stdafx.h"

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
int	SMF_MetaEvent::Compare(const void *setBT,const void *cmpBT){

//			int		iResult;
//	const	Rhythm*	sBT=(Rhythm *)setBT;
//	const	Rhythm*	cBT=(Rhythm *)cmpBT;
//	const	__int64	cmpData = (sBT->iTicksBT) - (cBT->iTicksBT);

	// "<" と "==" と ">" の３条件を評価する。
	// こういうのは、アセンブラでさくってやる。
	// if(a<b){} else if(a>b){} else(a==b){} は、無駄なコードだった。
	__asm{
		mov	edx,[setBT]
		mov	ecx,[cmpBT]

		mov	eax,dword ptr [edx].iTicksBT+4	;上位32bitを評価
		sub	eax,dword ptr [ecx].iTicksBT+4	;
		jb	SMF_MetaEvent__Compare_B		;＜？
		ja	SMF_MetaEvent__Compare_A		;＞？

		mov	eax,dword ptr [edx].iTicksBT	;そうでない場合は、下位32bitを評価
		sub	eax,dword ptr [ecx].iTicksBT	;
;		jb	SMF_MetaEvent__Compare_B		;＜？
;		ja	SMF_MetaEvent__Compare_A		;＞？

SMF_MetaEvent__Compare_B:
SMF_MetaEvent__Compare_A:

	}
};
//==============================================================
//		メタ・イベント　追加
//--------------------------------------------------------------
//	●引数
//			Rhythm	setBT		セットする拍子・調
//	●返値
//			
//	●処理
//			メタイベントを探し、追加する
//			（※調導入の時、注意。）
//==============================================================
Rhythm* SMF_MetaEvent::find(unsigned __int64 iTotalTicks){

	Rhythm	*ptBT;
	Rhythm	setBT;

	setBT.iTicksBT = iTotalTicks;

	if((defBT)>0){
		ptBT = (Rhythm *)_lfind(&setBT,(void *)BT,&(defBT),sizeof(Rhythm),Compare);
	} else {
		ptBT=NULL;
	};

	if(ptBT==NULL){
		ptBT=&BT[defBT];
		DefineInc();
	};

	return(ptBT);

};
//==============================================================
//		メタ・イベント　追加
//--------------------------------------------------------------
//	●引数
//		unsigned	__int64	iTotalTicks	セットされる時間
//		unsigned	char	cn			拍子の分子
//		unsigned	char	cd			拍子の分母
//		unsigned	__int16	iTimeBase	timebase	(to do	消す)
//	●返値
//		無し
//==============================================================
void SMF_MetaEvent::addRhythm(	const unsigned	__int64	iTotalTicks,
								const unsigned	char	cn, 
								const unsigned	char	cd,
								const unsigned	__int16	iTimeBase	){

	Rhythm	*setBT = find(iTotalTicks);

	setBT->iTicksBT		= iTotalTicks;
	setBT->cNumerator	= cn;
	setBT->cDenominator	= (1<<cd);

	//to do	これは、ソート後に計算する。
	setBT->iTicksOne	= ((setBT->cNumerator)*(iTimeBase<<2))/(setBT->cDenominator);

};
//==============================================================
//		メタ・イベント　追加
//--------------------------------------------------------------
//	●引数
//		unsigned	__int64	iTotalTicks	セットされる時間
//		unsigned	char	cK		調
//		unsigned	char	cM		Major / Minor
//	●返値
//		無し
//==============================================================
void	SMF_MetaEvent::addKey(	const unsigned	__int64	iTotalTicks, 
								const			char	cK, 
								const unsigned	char	cM	){
	
//	Rhythm	*setBT = find(iTotalTicks);

//	setBT->iTicksBT = iTotalTicks;
//	setBT->cKey		= cK;
//	setBT->cmi		= cM;

}

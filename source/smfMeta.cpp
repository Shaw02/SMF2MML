
#include "stdafx.h"

//==============================================================
//		���^�E�C�x���g	���Ԃ̔�r
//--------------------------------------------------------------
//	������
//			Rhythm	setBT		�Z�b�g���锏�q�E��
//			Rhythm	cmpBT		�Z�b�g���锏�q�E��
//	���Ԓl
//			��
//	������
//			���Ԃ��r����
//==============================================================
int	SMF_MetaEvent::Compare(const void *setBT,const void *cmpBT){

//			int		iResult;
//	const	Rhythm*	sBT=(Rhythm *)setBT;
//	const	Rhythm*	cBT=(Rhythm *)cmpBT;
//	const	__int64	cmpData = (sBT->iTicksBT) - (cBT->iTicksBT);

	// "<" �� "==" �� ">" �̂R������]������B
	// ���������̂́A�A�Z���u���ł������Ă��B
	// if(a<b){} else if(a>b){} else(a==b){} �́A���ʂȃR�[�h�������B
	__asm{
		mov	edx,[setBT]
		mov	ecx,[cmpBT]

		mov	eax,dword ptr [edx].iTicksBT+4	;���32bit��]��
		sub	eax,dword ptr [ecx].iTicksBT+4	;
		jb	SMF_MetaEvent__Compare_B		;���H
		ja	SMF_MetaEvent__Compare_A		;���H

		mov	eax,dword ptr [edx].iTicksBT	;�����łȂ��ꍇ�́A����32bit��]��
		sub	eax,dword ptr [ecx].iTicksBT	;
;		jb	SMF_MetaEvent__Compare_B		;���H
;		ja	SMF_MetaEvent__Compare_A		;���H

SMF_MetaEvent__Compare_B:
SMF_MetaEvent__Compare_A:

	}
};
//==============================================================
//		���^�E�C�x���g�@�ǉ�
//--------------------------------------------------------------
//	������
//			Rhythm	setBT		�Z�b�g���锏�q�E��
//	���Ԓl
//			
//	������
//			���^�C�x���g��T���A�ǉ�����
//			�i���������̎��A���ӁB�j
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
//		���^�E�C�x���g�@�ǉ�
//--------------------------------------------------------------
//	������
//		unsigned	__int64	iTotalTicks	�Z�b�g����鎞��
//		unsigned	char	cn			���q�̕��q
//		unsigned	char	cd			���q�̕���
//		unsigned	__int16	iTimeBase	timebase	(to do	����)
//	���Ԓl
//		����
//==============================================================
void SMF_MetaEvent::addRhythm(	const unsigned	__int64	iTotalTicks,
								const unsigned	char	cn, 
								const unsigned	char	cd,
								const unsigned	__int16	iTimeBase	){

	Rhythm	*setBT = find(iTotalTicks);

	setBT->iTicksBT		= iTotalTicks;
	setBT->cNumerator	= cn;
	setBT->cDenominator	= (1<<cd);

	//to do	����́A�\�[�g��Ɍv�Z����B
	setBT->iTicksOne	= ((setBT->cNumerator)*(iTimeBase<<2))/(setBT->cDenominator);

};
//==============================================================
//		���^�E�C�x���g�@�ǉ�
//--------------------------------------------------------------
//	������
//		unsigned	__int64	iTotalTicks	�Z�b�g����鎞��
//		unsigned	char	cK		��
//		unsigned	char	cM		Major / Minor
//	���Ԓl
//		����
//==============================================================
void	SMF_MetaEvent::addKey(	const unsigned	__int64	iTotalTicks, 
								const			char	cK, 
								const unsigned	char	cM	){
	
//	Rhythm	*setBT = find(iTotalTicks);

//	setBT->iTicksBT = iTotalTicks;
//	setBT->cKey		= cK;
//	setBT->cmi		= cM;

}

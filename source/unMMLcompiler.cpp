
#include "stdafx.h"

/****************************************************************/
/*			�O���錾											*/
/****************************************************************/
extern	void not_smf(const char *stMsg);

/****************************************************************/
/*					�萔										*/
/****************************************************************/
#define	dwMTrk	0x6B72544D		//'MTrk'��16�i���ɂ��Ă݂��B

//----------------------------------
//��Gloval�萔
	const	char*	const	strTrack[2]		={	"TR","�g���b�N"};
	const	char*	const	strChannel[2][2]={	"CH("		,	");	q100 h-1\n	",
												"�`�����l��(",	");	�Q�[�g100	h-1\n	"};
	const	char*	const	strTime[2]		={	"\nTime(1:1:0);\n	","\n���ԁi�P�F�P�F�O�j;\n	"};


	const	char*	const	strRest[2]	=	{	"r",	"��"};	
	const	char*	const	strNote[2][7]=	{	"c",	"d",	"e",	"f",	"g",	"a",	"b",
																"�h",	"��",	"�~",	"�t�@",	"�\",	"��",	"�V"};
	const	char*	const	strSharp[2]	=	{	"+",	"��"};
	const	char*	const	strFlat[2]	=	{	"-",	"��"};
	const	char*	const	strVelo[2]	=	{	"v",	"����"};
	const	char*	const	strOctave[2][6]	={	"o",	"<<",	"<",	"",	">",	">>",
												"���K",	"����",	"��",	"",	"��",	"����"};

	const	char			cSharp[6]				={11,4,9,2,7,0};	//����
	const	char			cFlat[6]				={5,0,7,2,9,4};		//����

	const	char*	const	strMetaText[2]			={"MetaText=",		"�R�����g"};
	const	char*	const	strMetaCopyright[2]		={"Copyright=",		"���"};
	const	char*	const	strMetaTrackName[2]		={"TrackName=",		"�Ȗ�"};
	const	char*	const	strMetaTempo[2]			={"Tempo",			"�e���|"};
	const	char*	const	strMetaTimeSignature[2]	={"TimeSignature",	"���q"};


//###############################################################
//#					�ėp�T�u���[�`��							#
//###############################################################
/****************************************************************/
/*					������										*/
/****************************************************************/
//==============================================================
//		�C�j�V�����C�Y
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::initTR(){

	//----------------------------------
	//��Local �ϐ�
	__int32		stName;		//�`�����N��

	//�w�b�_�[�ǂݍ���
	SMF->read((char *)&stName,4);
	SMF->read((char *)&iTrackSize,4);
	endian_b2l_D(&iTrackSize);			//�`�����N�T�C�Y��ۑ�
	lOffsetTR=SMF->tellg();				//���̃t�@�C���|�C���^��ۑ�
//	printf("offsetTR=%d\n",unMML->lOffsetTR);	//Debug�p

	if(stName!=dwMTrk){
		not_smf("MTrk�`�����N������܂���B\n");
	};

}
//==============================================================
//		�C�j�V�����C�Y
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::initCH(){

	//�g���b�N�̐擪�Ƀ|�C���^���ڂ�
	SMF->seekg(lOffsetTR,ios::beg);

	//�e���ڂ̏�����
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

	cNote=0xFF;	//�m�[�g
	flagNote=0;	//�o�͏��
	flagCh=0;

}
//###############################################################
//#					MML�o�͕�									#
//###############################################################
/****************************************************************/
/*					�A���o��									*/
/****************************************************************/
//==============================================================
//		16�i���A���f�R�[�h���o��	�iSysEx���ߓ��p�j
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			__int32		iSize		�o�̓T�C�Y
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::OutputHex(){

	//----------------------------------
	//��Local �ϐ�
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
//		������f�R�[�h���o��	�i���^�C�x���g���ߓ��p�j	��""���܂ށB
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			__int32		iSize		�o�̓T�C�Y
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::OutputStrings(){

	//----------------------------------
	//��Local �ϐ�
	unsigned	int	iSize = SMF_Event.iSize;

	MML->put('"');

	while(iSize>0){
		MML->put(SMF_Event.Read());
		iSize--;
	};

	MML->put('"');

};
//==============================================================
//		�ϒ��o��
//--------------------------------------------------------------
//	������
//			unsigned	__int32	var
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::EncodeVariable(unsigned	__int32	var){

	unsigned	int		count=0;		//�ǂݍ��݉񐔃J�E���g�p
				char	cData[4];

	//----------------------------------
	//���ϒ��G���R�[�h
	do{
		cData[count]=var&0x7F;
		if((var>>=7)==0){
			break;
		}
		cData[count]|=0x80;
		count++;
	} while((count<4));

	//���ϒ��o��
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
//		���`����
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::OutputBarCr(){

	//���q���ς��H
	if(SMF_Meta.nextBeatTime()==iTotalTicksChk){
		*MML << "\n	";
		iTicksBar=0;
		iTicksCR=0;
		SMF_Meta.CounterInc();
		nowBeatTime=SMF_Meta.nowTimeTicks();
		*MML << "//Rhythm = " << (int)SMF_Meta.nowNumerator() << "/" << (int)SMF_Meta.nowDenominator() << "\n	";
		//Debug�p�F		*MML << "/*" << (int)SMF_Meta.nowTimeTicks() << "*/";
	};

	//���`����
	//���ߏ��������Ȃ��ꍇ�́A�@�\�����Ȃ��B
	if(OptionSW->iBar!=0){

		//�o�[�̑}��
		if(iTicksBar>=nowBeatTime) {
			iTicksBar-=nowBeatTime;
			*MML << "	|	";		//Debug�p�F /*" << (int)iTicksBar << "*/";
		};

		//���s���邩�H
		//�I�v�V���� /C0 ��������A�������ɉ��s�B -1 �ŉ��s�����B
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
//		�����o��
//--------------------------------------------------------------
//	������
//			unMMLcompiler	*unMML		�tMML�p�\����
//			int			iTicks		�`�b�N
//	���Ԓl
//			int			iLength		����	-1��������A�]��L��
//==============================================================
int unMMLcompiler::Ticks2Length(int iTicksL){

	//----------------------------------
	//��Local �ϐ�
	unsigned	int		iZenlenght	=(SMF_Header.timebase)*4;	//����́A4�Œ�
				int		iLength;								//����

	iLength=iZenlenght / iTicksL;
	if((iZenlenght % iTicksL)!=0){
		iLength=-1;
	};

	return(iLength);
};
//==============================================================
//		�����o��
//--------------------------------------------------------------
//	������
//			int			iTicks		�`�b�N
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::Ticks2LengthEx(int iTicksL){

	//----------------------------------
	//��Local �ϐ�
				int		iLength;			//�����ϊ��p
				int		iDot;
				long	lx;	//�ėp
				long	ly;	//�ėp

	//----------------------------------
	//�������ɕϊ�
	iDot=0;
	while(iDot<14){	//�t�_��14�����܂ŁB
		lx =iTicksL*(1<<iDot);
		ly =(1<<(iDot+1))-1;	// (lx/ly)�ŁA�t�_�����Ȃ��ꍇ�̉����ɂȂ�B
		if((lx % ly)==0){		// �A���A����؂�鎖���O��B
			iLength=Ticks2Length(lx/ly);
			if(iLength!=-1){
				break;
			};
		};
		iDot++;
	};

	//�t�_��14�ȏ���悤�ł���΁Aticks�\�L
	if(iDot>=14){
		MML->put('%');
		*MML << iTicksL;
	//�����{�t�_�̕\���B
	} else{
		*MML << iLength;
		while(iDot>0){
			MML->put('.');
			iDot--;
		};
	};

};
//==============================================================
//		�����o�́i�f���^�^�C���̃f�R�[�h�j
//--------------------------------------------------------------
//	������
//			unsigned	__int32		iTicksL	����
//	���Ԓl
//			����
//==============================================================
	//----------------------------------
	//��Gloval�萔

	const	char*	const	strTai[2]	=	{"^","�["};

void unMMLcompiler::OutputTicks(/*unsigned	__int32		iTicksL*/){

	//----------------------------------
	//��Local �ϐ�
				char	flagStart;
				int		iLength;			//�����ϊ��p
				__int64	iLengthBT;			//�����ϊ��p�i���q���ς��܂Łj

	//----------------------------------
	//��������
	flagStart=1;		//�Ă΂ꂽ�ŏ��ł��邩�������t���O

	//----------------------------------
	//�������̏o��

	//���ߏ������Ȃ��ꍇ�́A���`���������Ȃ��B
	if(OptionSW->iBar!=0){
		//���܂�Ticks���A���߂��z���邩�H
		iLength=nowBeatTime-iTicksBar;		//���ƁA���`�b�N���邩�H
		if(iLength<(int)iTicks){
			//�ŏ��łȂ���΁A�^�C�Ōq��
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
			OutputBarCr();	//���`
		};
	};

		//���܂�Ticks���A���q�L�����z���邩�H
		iLengthBT=(__int64)((SMF_Meta.nextBeatTime())-(iTotalTicksChk));		//���ƁA���`�b�N���邩�H
		if(iLengthBT<(__int64)iTicks){
			//�ŏ��łȂ���΁A�^�C�Ōq��
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
			OutputBarCr();	//���`
		};
	//iTicksL �� 0 �ɂȂ�܂ŌJ��Ԃ��B
	while(iTicks>0){

		//�ŏ��łȂ���΁A�^�C�Ōq��
		if(flagStart==1){
			flagStart=0;
		} else {
			*MML << strTai[OptionSW->cJpn];
		};

		//�S�������傫��������A�܂��S�������o�͂���B
		if(iTicks>=nowBeatTime){
			Ticks2LengthEx(		nowBeatTime);
			iTicks			-=	nowBeatTime;
			iTicksBar		+=	nowBeatTime;
			iTicksCR		+=	nowBeatTime;
			iTotalTicksChk	+=	nowBeatTime;
		//�S������菬�����Ȃ�����A�������v�Z����B
		} else {
			Ticks2LengthEx(iTicks);
			iTicksBar		+=iTicks;
			iTicksCR		+=iTicks;
			iTotalTicksChk	+=iTicks;
			iTicks			=0;
		};

		//���`�̃`�F�b�N�i�������o�͂�����Łj
		OutputBarCr();

	};
};
/****************************************************************/
/*				Decode:	Note & Scale							*/
/****************************************************************/
//==============================================================
//		�����o��	��v�̌���
//--------------------------------------------------------------
//	������
//			*scale
//			caNote
//	���Ԓl
//			��v�����z��ԍ�
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
//	�ȉ��̂悤�Ɏv�������ǁA�X�g�����O�X���߂������̂́A8086����炵���B
//	Pentium�́A�}�C�N���R�[�h�ɕ��������̂ŁA���ʂ�C�ŏ������������B
//--------------------------------------------------------------
//	__asm{
//		mov		edi,cScale		;edi = cScale
//		movzx	eax,cNote		;eax = cNote
//		mov		ecx,7			;ecx=7	//(�z��̐�)
//
//		repne	scasb			;//�z����e�̈ꊇ�Scheck
//		je		NoteScas_Agree
//
//		;/*��v���Ȃ��ꍇ*/
//		or		eax,-1			;return(-1);
//		jmp		NoteScas_Lend
//		;/*��v���������B*/
//NoteScas_Agree:	
//		mov		eax,6
//		sub		eax,ecx			;return(6-eax)
//NoteScas_Lend:
//	};
//--------------------------------------------------------------

};
//==============================================================
//		�����o��
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLcompiler	*unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::OutputNoteEx(){

	//----------------------------------
	//��Local �ϐ�
			char	caOct;			//�I�N�^�[�u�v�Z�p
			char	cCmpOctave;		//�I�N�^�[�u��
			char	caNote;			//�m�[�g�v�Z�p
			char	cN;				//�ėp (Note)
			char	cS=0;			//�ėp (�X�P�[��)
			char	cAccidental=0;	//�Վ��L��
			char	cScale[7]		={0,2,4,5,7,9,11};	//�_�C�A�g�j�b�N�i����́A���ɂ���ĕς���j

	//----------------------------------
	//���o��
	//�����������o�͂��Ă��邩�H
	if(cNote<0x80){
		if(flagNote==0){
			*MML << strTai[OptionSW->cJpn];
		} else if(flagNote==1){
			caOct	= cNote / 12;
			caNote	= cNote % 12;

			//------------
			//���F���V�e�B�[
			if(cVelocity!=cVelo){
				cVelocity=cVelo;
				*MML << strVelo[OptionSW->cJpn] << (int)cVelocity;
			}

			//------------
			//�I�N�^�[�u
			cCmpOctave=caOct-(cOctave)+3;
			cOctave=caOct;

			if((cCmpOctave<=5)&&(cCmpOctave>=1)){
				*MML << strOctave[OptionSW->cJpn][cCmpOctave];
			}else{
				*MML << strOctave[OptionSW->cJpn][0] << (int)caOct;
			}

			//------------
			//����
//			NoteScaleOut(OptionSW->MML.stream,0,cNote);
			// to do	�����ɕ����Ĕz��𑀍�
			//�z�񒆂Ɉ�v�����邩�T��
			if((cN=NoteScas(cScale,caNote))==-1){
				// to do	������n���H
				// to do	cNote+1���A���̔����L���ƈ�v���邩�H
				//�Վ��L���t��
				cAccidental=1;
				cN=NoteScas(cScale,caNote-1);
			};
			//�o��
			*MML << strNote[OptionSW->cJpn][cN];
			if(cAccidental==1){
				*MML << strSharp[OptionSW->cJpn];
			} else if(cAccidental==-1){
				*MML << strFlat[OptionSW->cJpn];
			};
		};
	//----------------
	//�����Ȃ��ꍇ�i0xFF�j
	} else {
		*MML << strRest[OptionSW->cJpn];
	};

};
//==============================================================
//		�����������o��
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLcompiler	*unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::OutputNote(){

	//----------------------------------
	//���m�[�g ro �x�� or �^�C���o��
	if(iTicks>0){

		//�������o��
		OutputNoteEx();

		//�������o��
		OutputTicks();

		//�o�͂����̂ŁA���Z�b�g����B
		flagNote=0;				//����́A�^�C���o��
	};
};
/****************************************************************/
/*				Decode:	Channel Voice Message					*/
/****************************************************************/
//==============================================================
//		�m�[�g�E�����^�������̃f�R�[�h
//--------------------------------------------------------------
//	������
//					OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//					unMMLcompiler	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::DecodeNote(){

	//----------------------------------
	//��Local �ϐ�

			unsigned	__int32	iLength		= 0;				//����
						char	flagKeyOn	= 0;				//����key on���������B	
						char	flagKeyOff	= 0;				//key off�������B	
	const	long				fptStream	= SMF->tellg();		//�t�@�C���|�C���^�̕ۑ�

	//�X�e�[�^�X�o�C�g�E�f�[�^�o�C�g��ۑ�
	const				char	cTempEoT	= SMF_Event.flagEoT;	//End of Track�̃t���O
	const	unsigned	char	cTempStatus	= SMF_Event.cStatus;	//
	const	unsigned	char	cTempData1	= SMF_Event.cData1;		//
	const	unsigned	char	cTempData2	= SMF_Event.cData2;		//


	//--------------
	//Note Off����������
	while((flagKeyOff==0)&&(SMF_Event.flagEoT==0)){
		iLength+=SMF_Event.EventRead();
		SMF_Event.Seek();	//�|�C���^��i�߂�(0check�́A���[�`�����ł��Ă���B)

		//���ɃR�}���h�������H
		if(((SMF_Event.cStatus==cTempStatus)&&(SMF_Event.cData2!=0)) ||
			((SMF_Event.cStatus>=0xA0)&&(SMF_Event.cStatus<0xF0)&&((SMF_Event.cStatus&0x0F)==(cTempStatus&0x0F))))
		{
			flagKeyOn=1;
		};
		//Note Off�������H
		if(((SMF_Event.cStatus==(cTempStatus&0x8F))&&(SMF_Event.cData1==cTempData1)) ||
			((SMF_Event.cStatus==(cTempStatus&0x9F))&&(SMF_Event.cData1==cTempData1)&&(SMF_Event.cData2==0))) {
			flagKeyOff=1;
		};
	};

	//����key on����������B
	if(flagKeyOn==1){
		*MML << " Sub{";
		cNote=cTempData1;	//�Ƃ肠�����o��
		cVelo=cTempData2;	
		flagNote=1;			//Key On
		//�������o��
		OutputNoteEx();
		//�������o��
		Ticks2LengthEx(iLength);
		*MML << "}";
		cNote=0xFF;
	}else{
		cNote=cTempData1;
		cVelo=cTempData2;	
		flagNote=1;	//Key On
	}

	//--------------
	//���A
	SMF_Event.flagEoT	= cTempEoT;		//End of Track�̃t���O
	SMF_Event.cStatus	= cTempStatus;
	SMF_Event.cData1	= cTempData1;
	SMF_Event.cData2	= cTempData2;

	//�|�C���^��߂�
	SMF->StreamPointerMove(fptStream);
}
//==============================================================
//		�R���g���[���`�F���W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//					OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//					unMMLcompiler	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::DecodeControlChange(){

	//�R���g���[���ʏ���
	switch(SMF_Event.cData1){
		//--------------
		//Modulation
		case(1):
			*MML << "M(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//�|���^�����g�^�C��
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
		//�|���^�����g�X�C�b�`
		case(65):
			*MML << "PS(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//���o�[�u
		case(91):
			*MML << "Reverb(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//�R�[���X
		case(93):
			*MML << "Chorus(" << (int)SMF_Event.cData2 <<"); ";
			break;
		//--------------
		//�Z���X�e
		case(94):
			*MML << "VAR(" << (int)SMF_Event.cData2 << "); ";
			break;
		//--------------
		//���̑�
		default:
			*MML << "y" << (int)SMF_Event.cData1 << "," << (int)SMF_Event.cData2 << "; ";
			break;
	};

};
/****************************************************************/
/*				Decode:	Header & Meta Event Search				*/
/****************************************************************/
//==============================================================
//		���^�E�C�x���g�̌���
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//			����
//	������
//			���^�C�x���g��T���A�C�x���g�ɉ���������
//==============================================================
void unMMLcompiler::MetaSearch(){

	//----------------------------------
	//��Local �ϐ�
			unsigned	__int32	iDeltaTime;					//�ǂݍ��񂾉ϒ����l
			unsigned	char	cData[4];					//�f�[�^
	const				long	fptStream = SMF->tellg();	//�t�@�C���|�C���^

	//�S�g���b�N�̃T�[�`
	iTrack=0;
	iChannel=0;
	while((iTrack)<(SMF_Header.track)){
		initTR();
		initCH();
		//EOT������܂Ń��[�v
		while(SMF_Event.flagEoT==0){
			iDeltaTime = SMF_Event.EventRead();
			iTicks+=iDeltaTime;					//���f���^�^�C���ɉ��Z
			iTotalTicks+=iDeltaTime;			//���f���^�^�C���ɉ��Z
			
			//���^�C�x���g�H
			if(SMF_Event.cStatus==0xFF){
				switch(SMF_Event.cData1){
					//--------------
					//���q
					case(0x58):
						SMF_Event.DataRead((char *)&cData);
						SMF_Meta.addRhythm(iTotalTicks,cData[0],cData[1],SMF_Header.timebase);
						//nowBeat.iTicksBT = iTotalTicks;
						//nowBeat.iTicksOne= (cData[0]*(SMF_Header.timebase<<2)/(1<<cData[1]));
						//SMF_Meta.Add(&nowBeat);
						break;
					//--------------
					//����
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
					//�������Ȃ����^�C�x���g�̏ꍇ
					default:
						SMF_Event.Seek();
						break;
				};
			//���^�C�x���g�ȊO�͖���
			} else {
				SMF_Event.Seek();
			};
		};
		iTrack++;
	};

	//�\�[�g
	SMF_Meta.sort();

	//�|�C���^��߂�
	SMF->StreamPointerMove(fptStream);

};
//==============================================================
//		�w�b�_�[����
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::Header(){

	//----------------------------------
	//��Local �ϐ�

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

	//�܂��́A4/4�����Ď��ɂ��Ă����B
	SMF_Meta.addRhythm(0,4,2,SMF_Header.timebase);

	//���^�C�x���g�̃T�[�`
	MetaSearch();

	//�Ō�
	SMF_Meta.addRhythm(0x7FFFFFFFFFFFFFFF,4,2,SMF_Header.timebase);

	//to do	key, Beat�̕⊮

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
//		�`�����l���E�{�C�X�E���b�Z�[�W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//				OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//				unMMLcompiler	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
	//----------------------------------
	//��Gloval�萔
	const	char*	const	strVoice[2]={"Voice","���F"};


void unMMLcompiler::DecodeChannelVoiceMessage(){

	//----------------------------------
	//��Local �ϐ�
	unsigned	int			iChannel;		//�`�����l�����
	unsigned	char		cCommand;		//�R�}���h���

	//--------------
	//�`�����l���ƃR�}���h�ɕ�������
	iChannel = SMF_Event.cStatus & 0x0F;		//�`�����l��
	cCommand = SMF_Event.cStatus >> 4;			//�R�}���h

	//--------------
	//�R�}���h�ʏ���
	switch(cCommand){
		//--------------
		//Note Off Message
		case(0x08) :
//			OptionSW->MML.writePrintf("NoteOff(%d,%d); ",unMML->SMF_Event.cData1,unMML->SMF_Event.cData2);
			if(SMF_Event.cData1==cNote){
				cNote=0xFF;			//Note Off (�x��)
			};
			break;
		//--------------
		//Note On Message
		case(0x09) :
//			OptionSW->MML.writePrintf("NoteOn(%d,%d); ",unMML->SMF_Event.cData1,unMML->SMF_Event.cData2);
			if((SMF_Event.cData1==cNote)&&(SMF_Event.cData2==0)){
				cNote=0xFF;			//Note Off (�x��)
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
		//�����ȊO
		default:
			not_smf("������Channel Voice Message�B\n");
			break;
	};

};
//==============================================================
//		�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLcompiler	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::DecodeSystemExcusiveMessage(){

	*MML << "SysEx = $F0,";
	OutputHex();
	*MML << ";\n	";

};
//==============================================================
//		�V�X�e���E�R�����E���b�Z�[�W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLcompiler	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::DecodeSystemCommonMessage(){

	*MML << "SysEx = ";
	OutputHex();
	*MML << ";\n	";

};
//==============================================================
//		���^�C�x���g�̃f�R�[�h
//--------------------------------------------------------------
//	������
//				OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//				unMMLcompiler	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void unMMLcompiler::DecodeMetaEvent(){

	//----------------------------------
	//��Local �ϐ�
	unsigned	char	cData[4];	//�f�[�^
	unsigned	int		iData;		//�ėp

	//�C�x���g�ʏ���
	switch(SMF_Event.cData1){
		//--------------
		//�V�[�P���X�ԍ�
		case(0x00) :
			if(SMF_Event.iSize==2){
				*MML << "DirectSMF($FF,$00,$02,";
				OutputHex();
				*MML << "); ";
			} else {
				not_smf("�����ȃV�[�P���X�ԍ��B\n");
			}
			break;
		//--------------
		//�e�L�X�g
		case(0x01) :
			*MML << strMetaText[OptionSW->cJpn] << "{";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//���쌠�\��
		case(0x02) :
			*MML << strMetaCopyright[OptionSW->cJpn] << "{";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//�V�[�P���X���^�g���b�N��
		case(0x03) :
			*MML << strMetaTrackName[OptionSW->cJpn] << "{";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//�y�햼
		case(0x04) :
			*MML << "InstrumentName={";
			OutputStrings();
			*MML << "};\n	";
			break;
		//--------------
		//�̎�
		case(0x05) :
			*MML << "Lyric={";
			OutputStrings();
			*MML << "}; ";
			break;
		//--------------
		//�}�[�J
		case(0x06) :
			*MML << "Marker={";
			OutputStrings();
			*MML << "}; ";
			break;
		//--------------
		//�L���[�|�C���g
		case(0x07) :
			*MML << "CuePoint={";
			OutputStrings();
			*MML << "}; ";
			break;
		//--------------
		//MIDI�`�����l���v���t�B�b�N�X
		case(0x20) :
			if(SMF_Event.iSize==1){
				SMF_Event.DataRead((char *)&cData);
				*MML << "ChannelPrefix(" << (int)cData[0] << "); ";
			} else {
				not_smf("�����ȃ`�����l���v���t�B�b�N�X�B\n");
			}
			break;
		//--------------
		//�|�[�g�ԍ�
		case(0x21) :
			if(SMF_Event.iSize==1){
				SMF_Event.DataRead((char *)&cData);
				*MML << "Port(" << (int)cData[0] << "); ";
			} else {
				not_smf("�����ȃ|�[�g�ԍ��B\n");
			}
			break;
		//--------------
		//�G���h�I�u�g���b�N
		case(0x2F) :
			if(SMF_Event.iSize==0){
				//�`�����l���̋tMML�R���p�C�����I���B
				SMF_Event.flagEoT=1;
			} else {
				not_smf("������End of Track�B\n");
			}
			break;
		//--------------
		//�e���|
		case(0x51) :
			if(SMF_Event.iSize==3){
				SMF_Event.DataRead((char *)&cData);
				*MML << strMetaTempo[OptionSW->cJpn] << "(" << (60000000/((cData[0]<<16)+(cData[1]<<8)+(cData[2]))) << ");\n	";
			} else {
				not_smf("������Tempo�B\n");
			}
			break;
		//--------------
		//SMPTE�I�t�Z�b�g
		case(0x54) :
			if(SMF_Event.iSize==5){
				*MML << "DirectSMF($FF,$54,$05,";
				OutputHex();
				*MML << "); ";
			} else {
				not_smf("������SMPTE�I�t�Z�b�g�B\n");
			}
			break;
		//--------------
		//���q
		case(0x58) :
			if(SMF_Event.iSize==4){
				SMF_Event.DataRead((char *)&cData);
				iData=1<<cData[1];
				//��͂͑O�����ł�邪�A�\���͂����ł���B
				*MML << strMetaTimeSignature[OptionSW->cJpn] << "(" << (int)cData[0] << "," << iData << ");\n	";
			} else {
				not_smf("�����Ȕ��q�B\n");
			}
			break;
		//--------------
		//����	�ito do �悸�́ADirectSMF�BTimeKeyFlag�őΉ��ł���H�j
		case(0x59) :
			if(SMF_Event.iSize==2){
				*MML << "DirectSMF($FF,$59,$02,";
				OutputHex();
				*MML << ");\n	";
			} else {
				not_smf("�����Ȓ����B\n");
			}
			break;
		//--------------
		//�V�[�P���T�ŗL�̃��^�C�x���g�i���Ή��j
		case(0x7F) :
			*MML << "/*�@DirectSMF($FF,$7F,";
			EncodeVariable(SMF_Event.iSize);
			OutputHex();
			*MML << "); */\n	";
			break;
		//--------------
		//
		default:
			not_smf("������Meta Event\n");
			break;
	};

};
/****************************************************************/
/*				Decode:	Main Routine							*/
/****************************************************************/
//==============================================================
//		�P�R�}���h�̃f�R�[�h	�i���C���f�R�[�h�j
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//				unMMLcompiler	unMML		�tMML�p�\����
//	���Ԓl
//			__int32	iDeltaTime	�f���^�^�C��
//==============================================================
void unMMLcompiler::Decode1Command(){

	//----------------------------------
	//��Local �ϐ�
	unsigned	__int32		iDeltaTime;		//�ǂݍ��񂾉ϒ����l
	unsigned	char		cChannel;		//�`�����l�����
	unsigned	char		cCommand;		//�R�}���h���

	//MIDI�C�x���g�ǂݍ���
	iDeltaTime = SMF_Event.EventRead();
	iTicks+=iDeltaTime;				//���f���^�^�C���ɉ��Z
	iTotalTicks+=iDeltaTime;		//���f���^�^�C���ɉ��Z

	//----------------------
	//���R�}���h�ʏ���

	//--------------
	//�`�����l���ƃR�}���h�ɕ�������
	cChannel = SMF_Event.cStatus & 0x0F;	//�`�����l��
	cCommand = SMF_Event.cStatus >> 4;		//�R�}���h

	//--------------
	//���o�͂��Ȃ��ꍇ���A�}�X�N����B
	if(	((SMF_Event.cStatus<0xF0)&&(iChannel!=(cChannel+1)))
	||	((SMF_Event.cStatus>=0xF0)&&(iChannel!=0))	)
		{
			SMF_Event.cStatus=0x00;
		};

	//--------------
	//�o�͂��}�X�N���ꂽ�H
	if(SMF_Event.cStatus==0x00){
		SMF_Event.Seek();

	//�o�͂�����ꍇ�B
	}else{
		//���߂Ă̏o�͂ł���΁A�`�����l�����o�͂���B
		if(flagCh==0){
			flagCh=1;
			*MML << strTime[OptionSW->cJpn];
			//Channel 1�`
			if((iChannel)>0){
				if(SMF_Header.format==0){
					*MML << strTrack[OptionSW->cJpn] << iChannel;	//format 0��������A�g���b�N���f��
				};
				*MML << strChannel[OptionSW->cJpn][0] << iChannel << strChannel[OptionSW->cJpn][1];
			};
		};

		//--------------
		//�����܂܂ł̉����E�x���̏o��
		//�A���AEoT���̓`�����l���o�͎��̂ݍs���B
		if(	((SMF_Event.flagEoT==0)&&(SMF_Event.cStatus!=0x00))
		||	((SMF_Event.flagEoT==1)&&(flagCh==1))&&(iChannel>0)	)
		{
			OutputNote();
		};

		//--------------
		//��Channel Voice Message�H
		if(SMF_Event.cStatus<0xF0){

			//--------------
			//�`�����l���E�{�C�X�E���b�Z�[�W
			DecodeChannelVoiceMessage();

		//--------------
		//��System Excusive Message
		//��System Common Message
		//��Meta Event
		} else {

			//--------------
			//���b�Z�[�W�ʏ���
			switch(SMF_Event.cStatus){
				//--------------
				//�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W
				case(SEXM):
					DecodeSystemExcusiveMessage();
					break;
				//--------------
				//�V�X�e���E�R�����E���b�Z�[�W
				case(SCM):
					DecodeSystemCommonMessage();
					break;
				//--------------
				//���^�E�C�x���g
				case(MEM):
					DecodeMetaEvent();
					break;
				//--------------
				//���̑�
				default:
					not_smf("������MIDI�C�x���g�����o���܂����B\n");
					break;
			};	//End Switeh

		};	//EndIf	SysEx, Meta

	};	//EndIf	Mask

};
//==============================================================
//		�A���R���p�C��
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//			MML�t�@�C����MML����������Ă䂭
//==============================================================
void unMMLcompiler::uncompile(){

	//----------------------------------
	//��Local �ϐ�
//	unMMLcompiler	unMML(OptionSW);	//�tMML���������邽�߂̍\����

	//----------------------------------
	//���w�b�_�[����
	Header();		//MThd�̓��e��\��
					//������́i���^�C�x���g�̏����j

	//----------------------------------
	//���eMTrk�̏���
	iTrack=0;
	while(iTrack<SMF_Header.track){
		//�g���b�N���̏�����
		initTR();

		//MML�ł́A�g���b�N�̏�����
		*MML	<< "//--------------------------------------" << endl
				<< "//MTrk Infomation :" << endl
				<< "//	Size = " << iTrackSize << endl
				<< endl
				<< strTrack[OptionSW->cJpn] << "(" << iTrack << ");" << endl;

		//�`�����l�����̏���
		iChannel=0;
		while(iChannel<=16){
			//�`�����l�����̏�����
			initCH();
			//End of Track������܂ŏ���
			while(SMF_Event.flagEoT==0){
				Decode1Command();
			};
			iChannel++;
		};
		*MML << endl << endl;
		iTrack++;
	};
};


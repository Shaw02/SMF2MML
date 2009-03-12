
#include "stdafx.h"

/****************************************************************/
/*																*/
/*					�G���[����									*/
/*																*/
/****************************************************************/
//==============================================================
//		MIDI�t�@�C�����Ⴀ��܂���B
//--------------------------------------------------------------
//	������
//			char		*filename	���t�@�C����
//	���Ԓl
//			����
//==============================================================
void not_smf(char *filename, char *stMsg){
	fprintf(stderr,filename);
	fprintf(stderr,": ");
	fprintf(stderr,"�f�[�^���s���ł��B\n");
	fprintf(stderr,stMsg);
	exit(EXIT_FAILURE);
}
/****************************************************************/
/*																*/
/*					�o��										*/
/*																*/
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
void OutputHex(OPSW *OptionSW,__int32 iSize){

	//----------------------------------
	//��Local �ϐ�
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
//		������f�R�[�h���o��	�i���^�C�x���g���ߓ��p�j	��""���܂ށB
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			__int32		iSize		�o�̓T�C�Y
//	���Ԓl
//			����
//==============================================================
void OutputStrings(OPSW *OptionSW,__int32 iSize){

	//----------------------------------
	//��Local �ϐ�
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
//		���`����
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	*unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void OutputBarCr(OPSW *OptionSW,unMMLinfo *unMML){

	//���q���ς��H
	if(unMML->BT[(unMML->cntBT)+1].iTicksBT==unMML->iTotalTicksChk){
		fprintf(OptionSW->MML.stream,"\n	");
		unMML->iTicksBar=0;
		unMML->iTicksCR=0;
		unMML->cntBT++;
	};

	//���`����
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
//		�����o��
//--------------------------------------------------------------
//	������
//			unMMLinfo	*unMML		�tMML�p�\����
//			int			iTicks		�`�b�N
//	���Ԓl
//			int			iLength		����	-1��������A�]��L��
//==============================================================
int Ticks2Length(unMMLinfo *unMML, int iTicks){

	//----------------------------------
	//��Local �ϐ�
	unsigned	int		iZenlenght;			//�S����
				int		iLength;			//����

	iZenlenght=(unMML->SMF_Header.timebase)*4;	//����́A4�Œ�

	iLength=iZenlenght/iTicks;
	if((iZenlenght % iTicks)!=0){
		iLength=-1;
	};

	return(iLength);
};
//==============================================================
//		�����o��
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	*unMML		�tMML�p�\����
//			int			iTicks		�`�b�N
//	���Ԓl
//			����
//==============================================================
void Ticks2LengthEx(OPSW *OptionSW,unMMLinfo *unMML, int iTicks){

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
		lx =iTicks*(1<<iDot);
		ly =(1<<(iDot+1))-1;	// (lx/ly)�ŁA�t�_�����Ȃ��ꍇ�̉����ɂȂ�B
		if((lx % ly)==0){		// �A���A����؂�鎖���O��B
			iLength=Ticks2Length(unMML,(lx/ly));
			if(iLength!=-1){
				break;
			};
		};
		iDot++;
	};

	//�t�_��14�ȏ���悤�ł���΁Aticks�\�L
	if(iDot>=14){
		fputc('%',OptionSW->MML.stream);
		fprintf(OptionSW->MML.stream,"%d",iTicks);
	//�����{�t�_�̕\���B
	} else{
		fprintf(OptionSW->MML.stream,"%d",iLength);
		while(iDot>0){
			fputc('.',OptionSW->MML.stream);
			iDot--;
		};
	};

};
//==============================================================
//		�����o�́i�f���^�^�C���̃f�R�[�h�j
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	*unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void OutputTicks(OPSW *OptionSW,unMMLinfo *unMML){

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

	//���܂�Ticks���A���߂��z���邩�H
	iLength=(unMML->BT[unMML->cntBT].iTicksOne)-(unMML->iTicksBar);		//���ƁA���`�b�N���邩�H
	if(iLength<(int)unMML->iTicks){
		//�ŏ��łȂ���΁A�^�C�Ōq��
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
		OutputBarCr(OptionSW,unMML);	//���`
	};

	//���܂�Ticks���A���q�L�����z���邩�H
	iLengthBT=(__int64)((unMML->BT[(unMML->cntBT)+1].iTicksBT)-(unMML->iTotalTicksChk));		//���ƁA���`�b�N���邩�H
	if(iLengthBT<(__int64)(unMML->iTicks)){
		//�ŏ��łȂ���΁A�^�C�Ōq��
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
		OutputBarCr(OptionSW,unMML);	//���`
	};

	//iTicks �� 0 �ɂȂ�܂ŌJ��Ԃ��B
	while((unMML->iTicks)>0){

		//�ŏ��łȂ���΁A�^�C�Ōq��
		if(flagStart==1){
			flagStart=0;
		} else {
			fprintf(OptionSW->MML.stream,"^");
		};

		//�S�������傫��������A�܂��S�������o�͂���B
		if((unMML->iTicks)>=unMML->BT[unMML->cntBT].iTicksOne){
			Ticks2LengthEx(OptionSW,unMML,unMML->BT[unMML->cntBT].iTicksOne);
			unMML->iTicks-=unMML->BT[unMML->cntBT].iTicksOne;
			unMML->iTicksBar+=unMML->BT[unMML->cntBT].iTicksOne;
			unMML->iTicksCR+=unMML->BT[unMML->cntBT].iTicksOne;
			unMML->iTotalTicksChk+=unMML->BT[unMML->cntBT].iTicksOne;
		//�S������菬�����Ȃ�����A�������v�Z����B
		} else {
			Ticks2LengthEx(OptionSW,unMML,unMML->iTicks);
			unMML->iTicksBar+=unMML->iTicks;
			unMML->iTicksCR+=unMML->iTicks;
			unMML->iTotalTicksChk+=unMML->iTicks;
			unMML->iTicks=0;
		};

		//���`�̃`�F�b�N�i�������o�͂�����Łj
		OutputBarCr(OptionSW,unMML);

	};
};
//==============================================================
//		�����o��
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	*unMML		�tMML�p�\����
//			unsigned char cVelocity	���F���V�e�B�[
//	���Ԓl
//			����
//==============================================================
void OutputNoteEx(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	int		iOctave;	//�I�N�^�[�u�v�Z�p
	int		iCmpOctave;	//�I�N�^�[�u��
	int		iNote;		//�m�[�g�v�Z�p

	/*
	------------------------------------------------
	��	���ɖ��ʂȃR�[�h��f���ꂽ�̂ŁA�폜�B
	��	�A�Z���u������ɂċL�q�B
	------------------------------------------------
	//�e�X�P�[�����̏o�͐ݒ�
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
	//���o��
	//�����������o�͂��Ă��邩�H
	if(unMML->cNote<0x80){
		if(unMML->flagNote==0){
			fprintf(OptionSW->MML.stream,"^");
		} else if(unMML->flagNote==1){
			iOctave = unMML->cNote / 12;
			iNote	= unMML->cNote % 12;

			//���F���V�e�B�[
			if(unMML->cVelocity!=unMML->cVelo){
				unMML->cVelocity=unMML->cVelo;
				fprintf(OptionSW->MML.stream,"v%d",unMML->cVelocity);
			}

			//�I�N�^�[�u
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

			//����
			//���ʂȃ\�[�X��f���ꂽ�̂ŁA�A�Z���u������ŏ�������B
			//fprintf(OptionSW->MML.stream,ptNote[6][iNote]);
			NoteScaleOut(OptionSW->MML.stream,0,iNote);
		}
	//�����Ȃ��ꍇ�i0xFF�j
	} else {
		fprintf(OptionSW->MML.stream,"r");
	};

};
//==============================================================
//		�����������o��
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	*unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void OutputNote(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�

	//----------------------------------
	//�����߂Ă̏o�͂ł���΁A�`�����l�����o�͂���B
	if(unMML->flagCh==0){
		unMML->flagCh=1;
		fprintf(OptionSW->MML.stream,"\nTime(1:1:0);\n");
		if((unMML->iChannel)>0){
			if(unMML->SMF_Header.format==0){
				fprintf(OptionSW->MML.stream,"TR(%d)",unMML->iChannel);	//format 0��������A�g���b�N���f��
			};
			fprintf(OptionSW->MML.stream,"	CH(%d);	q100 h-1\n	",unMML->iChannel);
		};
	};

	//----------------------------------
	//���m�[�g ro �x�� or �^�C���o��
	if(unMML->iTicks>0){

		//�������o��
		OutputNoteEx(OptionSW,unMML);

		//�������o��
		OutputTicks(OptionSW,unMML);

		//�o�͂����̂ŁA���Z�b�g����B
		unMML->flagNote=0;	//����́A�^�C���o��
		unMML->iTicks=0;	//�����͂O

	};

};

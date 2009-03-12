/****************************************************************/
/*																*/
/*					�萔										*/
/*																*/
/****************************************************************/

#include "stdafx.h"

#define	dwMThd	0x6468544D		//'MThd'��16�i���ɂ��Ă݂��B
#define	dwMTrk	0x6B72544D		//'MTrk'��16�i���ɂ��Ă݂��B

/****************************************************************/
/*																*/
/*			�v���g�^�C�v�錾									*/
/*																*/
/****************************************************************/
extern	void not_smf(char *filename, char *stMsg);
extern	void StreamPointerAdd(FILE *stream,__int32 iSize);
extern	unsigned __int32 EventRead(OPSW *OptionSW,unMMLinfo *unMML);
extern	void Decode1Command(OPSW *OptionSW,unMMLinfo *unMML);

/****************************************************************/
/*																*/
/*					�t�R���p�C���p�E�T�u���[�`��				*/
/*																*/
/****************************************************************/
//##############################################################
//		Sub routine
//##############################################################
//==============================================================
//		�C�j�V�����C�Y
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML	�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void uncompile_initTR(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	__int32		stName;		//�`�����N��

	//�w�b�_�[�ǂݍ���
	fread((void *)&stName,sizeof(char),4,OptionSW->SMF.stream);
	fread((void *)&unMML->iTrackSize,sizeof(char),4,OptionSW->SMF.stream);
	endian_b2l_D(&unMML->iTrackSize);				//�`�����N�T�C�Y��ۑ�
	unMML->lOffsetTR=ftell(OptionSW->SMF.stream);	//���̃t�@�C���|�C���^��ۑ�
//	printf("offsetTR=%d\n",unMML->lOffsetTR);	//Debug�p

	if(stName!=dwMTrk){
		not_smf(OptionSW->SMF.name,"MTrk�`�����N������܂���B\n");
	};

}
//==============================================================
//		�C�j�V�����C�Y
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML	�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void uncompile_initCH(OPSW *OptionSW,unMMLinfo *unMML){

	//�g���b�N�̐擪�Ƀ|�C���^���ڂ�
	fseek(OptionSW->SMF.stream,unMML->lOffsetTR,SEEK_SET);

	//�e���ڂ̏�����
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

	unMML->cNote=0xFF;	//�m�[�g
	unMML->flagNote=0;	//�o�͏��
	unMML->flagCh=0;

	unMML->flagEoT=0;
	unMML->cntBT=0;

}
//##############################################################
//		MTrk�`�����N
//##############################################################
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
//		���^�E�C�x���g�@�ǉ�
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML	�tMML�p�\����
//			Rhythm	setBT		�Z�b�g���锏�q�E��
//	���Ԓl
//			
//	������
//			���^�C�x���g��T���A�ǉ�����
//			�i���������̎��A���ӁB�j
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
//		���^�E�C�x���g�̌���
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML	�tMML�p�\����
//	���Ԓl
//			
//	������
//			���^�C�x���g��T���A�C�x���g�ɉ���������
//==============================================================
void MetaSearch(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	unsigned	__int32		iDeltaTime;		//�ǂݍ��񂾉ϒ����l
	unsigned char	cData[4];	//�f�[�^
	long			fptStream;	//�t�@�C���|�C���^
	Rhythm			nowBeat;	//���݂̔��q

	nowBeat.iTicksBT=0;
	nowBeat.iTicksOne=0;
	nowBeat.cKey=0;
	nowBeat.cmi=0;

	//�|�C���^�̕ۑ�
	fptStream = ftell(OptionSW->SMF.stream);

	//�S�g���b�N�̃T�[�`
	unMML->iTrack=0;
	unMML->iChannel=0;
	while((unMML->iTrack)<(unMML->SMF_Header.track)){
		uncompile_initTR(OptionSW,unMML);
		uncompile_initCH(OptionSW,unMML);
		//EOT������܂Ń��[�v
		while(unMML->flagEoT==0){
			iDeltaTime = EventRead(OptionSW,unMML);
			unMML->iTicks+=iDeltaTime;				//���f���^�^�C���ɉ��Z
			unMML->iTotalTicks+=iDeltaTime;			//���f���^�^�C���ɉ��Z
			
			//���^�C�x���g�H
			if(unMML->cStatus==0xFF){
				switch(unMML->cData1){
					//--------------
					//���q
					case(0x58):
						fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
						nowBeat.iTicksBT = unMML->iTotalTicks;
						nowBeat.iTicksOne= (cData[0]*(unMML->SMF_Header.timebase<<2)/(1<<cData[1]));
						AddBeat(OptionSW,unMML,&nowBeat);
						break;
					//--------------
					//����
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
					//�������Ȃ����^�C�x���g�̏ꍇ
					default:
						StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);
						break;
				};
			//���^�C�x���g�ȊO�͖���
			} else {
				if(unMML->iSize>0){
					StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);
				};
			};
		};
		unMML->iTrack++;
	};

	//�\�[�g
	qsort((void *)unMML->BT,unMML->defBT,sizeof(Rhythm),BeatCompare);

	//�|�C���^��߂�
	fseek(OptionSW->SMF.stream,fptStream,SEEK_SET);

};
//##############################################################
//		MThd�`�����N
//##############################################################
//==============================================================
//		�w�b�_�[����
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void uncompile_Header(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	long		*cSMF_Header;	//if���ňꊇ��'MThd'�����ׂ�p
	Rhythm		defaultBT;

	//----------------------------------
	//��MThd�w�b�_�[����	with Endian�̕ϊ�(Big��Little)
	fread((void *)&unMML->SMF_Header,sizeof(char),14,OptionSW->SMF.stream);
	//���b����łǂ������Ηǂ����m��Ȃ�����A�A�Z���u������Ȋ֐��B
	//��char�^�ɂ��Ă������񂾂��ǁA�܂��|�C���^�ϐ����₷�̂߂�ǂ����B
	//��"smf2mml_sub.asm"���Q�ƁB
	endian_b2l_D(&unMML->SMF_Header.size);
	endian_b2l_W(&unMML->SMF_Header.format);
	endian_b2l_W(&unMML->SMF_Header.track);
	endian_b2l_W(&unMML->SMF_Header.timebase);

	//MThd�`�����N�����邩�H
	cSMF_Header=(long *)unMML->SMF_Header.strings;	//if���ňꊇ��'MThd'�����ׂ��
	if(*cSMF_Header != dwMThd){						//(SMF_Header.strings == 'MThd')?
		not_smf(OptionSW->SMF.name,"MThd�`�����N������܂���B\n");
	};

	//�t�H�[�}�b�g��0��1�H
	if(unMML->SMF_Header.format==2){
		not_smf(OptionSW->SMF.name,"Format 2�ɂ͑Ή����Ă���܂���B\n");
	} else if(unMML->SMF_Header.format>2){
		not_smf(OptionSW->SMF.name,"���m�̃t�H�[�}�b�g�ł��B\n");
	};

	//�g���b�N����0�łȂ��H
	if(unMML->SMF_Header.track==0){
		not_smf(OptionSW->SMF.name,"�g���b�N����0�ł��B\n");
	};

	//�t�H�[�}�b�g0���A�g���b�N����1�łȂ��H
	if((unMML->SMF_Header.format==0)&&(unMML->SMF_Header.track!=1)){
		not_smf(OptionSW->SMF.name,"Format 0�Ȃ̂ɁA�����̃g���b�N������܂��B\n");
	};
	
	//timebase�̃`�F�b�N
	if(unMML->SMF_Header.timebase==0){
		not_smf(OptionSW->SMF.name,"timebase��0tick�ł��B\n");
	}else if(unMML->SMF_Header.timebase&0x8000){
		not_smf(OptionSW->SMF.name,"�b�w��̎��ԒP�ʂɂ͑Ή����Ă���܂���B\n");
	};

	// to do	���̑��w�b�_�̃`�F�b�N		


	//MML�w�b�_�[�o��
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

	//�܂��́A4/4�����Ď��ɂ��Ă����B
	unMML->defBT=0;
	defaultBT.iTicksBT=0;
	defaultBT.iTicksOne=unMML->SMF_Header.timebase<<2;
	defaultBT.cKey=0;
	defaultBT.cmi=0;
	AddBeat(OptionSW,unMML,&defaultBT);

	//���^�C�x���g�̃T�[�`
	MetaSearch(OptionSW,unMML);

	//�Ō�
	defaultBT.iTicksBT=0x7FFFFFFFFFFFFFFF;
	defaultBT.iTicksOne=unMML->SMF_Header.timebase<<2;
	defaultBT.cKey=0;
	defaultBT.cmi=0;
	AddBeat(OptionSW,unMML,&defaultBT);

	//to do	key, Beat�̕⊮
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
/*					�t�R���p�C���p�E���C�����[�`��				*/
/*																*/
/****************************************************************/
//==============================================================
//		�A���R���p�C��
//--------------------------------------------------------------
//	������
//			OPSW *OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//	���Ԓl
//			MML�t�@�C����MML����������Ă䂭
//==============================================================
void uncompile(OPSW *OptionSW){

	//----------------------------------
	//��Local �ϐ�
	unMMLinfo	unMML;			//�tMML���������邽�߂̍\����

	//----------------------------------
	//���w�b�_�[����
	uncompile_Header(OptionSW,&unMML);

	//----------------------------------
	//���eMTrk�̏���
	unMML.iTrack=0;
	while(unMML.iTrack<unMML.SMF_Header.track){
		//�g���b�N���̏�����
		uncompile_initTR(OptionSW,&unMML);

		//MML�ł́A�g���b�N�̏�����
		fprintf(OptionSW->MML.stream,"\n\n");
		fprintf(OptionSW->MML.stream,"//--------------------------------------\n");
		fprintf(OptionSW->MML.stream,"//MThd Infomation :\n");
		fprintf(OptionSW->MML.stream,"//	Size = %d \n",unMML.iTrackSize);
		fprintf(OptionSW->MML.stream,"//\n");
		fprintf(OptionSW->MML.stream,"TR(%d)\n",(unMML.iTrack+1));

		//�`�����l�����̏���
		unMML.iChannel=0;
		while(unMML.iChannel<=16){
			//�`�����l�����̏�����
			uncompile_initCH(OptionSW,&unMML);
			//End of Track������܂ŏ���
			while(unMML.flagEoT==0){
				Decode1Command(OptionSW,&unMML);
			};
			unMML.iChannel++;
		};
		unMML.iTrack++;
	};
};

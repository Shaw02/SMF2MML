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
/*					�ėp�T�u���[�`��							*/
/*																*/
/****************************************************************/
//==============================================================
//		�X�g���[���E�|�C���^��i�߂�
//--------------------------------------------------------------
//	������
//			FileInfo *file	�t�@�C�����̍\����
//			__int32		iSize		�T�C�Y
//	���Ԓl
//			����
//==============================================================
void StreamPointerAdd(FileInfo *file,__int32 iSize){

	fseek(file->stream,(long)iSize,SEEK_CUR);

};
//##############################################################
//		�ėp�T�u���[�`��
//##############################################################
//==============================================================
//		�ϒ��f�R�[�h
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//	���Ԓl
//			__int32	iDeltaTime	�f���^�^�C��
//==============================================================
unsigned __int32	DecodeVariable(OPSW *OptionSW){

	//----------------------------------
	//��Local �ϐ�
	unsigned	__int32		iDeltaTime;		//
	unsigned	char		cFread;			//
	unsigned	int			count;			//

	//----------------------------------
	//���f���^�^�C���ǂݍ���
	iDeltaTime=0;
	count=4;	//�ő�4Byte�B�]���āA�ǂݍ��݂͂S��܂�
	do{
		fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
		iDeltaTime=(iDeltaTime<<7)+((unsigned __int32)cFread&0x7F);
		count--;
	} while((count>0)&&(cFread&0x80));

	return(iDeltaTime);

};
//##############################################################
//		�o�͊֌W
//##############################################################
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
		fprintf(OptionSW->MML.stream,"x%X",cFread);
		if(iSize>1){
			fprintf(OptionSW->MML.stream,",");
		} else {
			fprintf(OptionSW->MML.stream," ");
		};
		iSize--;
	};

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

	//----------------------------------
	//�������ɕϊ�

	// to do	�t�_���v�Z����B

	iLength=Ticks2Length(unMML,iTicks);
	if(iLength==-1){
		fputc('%',OptionSW->MML.stream);
		fprintf(OptionSW->MML.stream,"%d",unMML->iTicks);	// to do
	} else {
		fprintf(OptionSW->MML.stream,"%d",iLength);
	};

};
//==============================================================
//		�����o��
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
	unsigned	int		iZenlenght;			//
				char	flagStart;
				int		iLength;			//�����ϊ��p

	//----------------------------------
	//��������
	flagStart=1;
	iZenlenght=(unMML->SMF_Header.timebase)*4;	//	to do	������

	//----------------------------------
	//�������̏o��
	//iTicks �� 0 �ɂȂ�܂ŌJ��Ԃ��B
	while((unMML->iTicks)>0){

		//�ŏ��̃��[�v�łȂ���΁A�^�C�Ōq��
		if(flagStart==1){
			flagStart=0;
		} else {
			fprintf(OptionSW->MML.stream,"^");
		};

		//�S�������傫��������A�܂��S�������o�͂���B
		if((unMML->iTicks)>=iZenlenght){
			Ticks2LengthEx(OptionSW,unMML,iZenlenght);
			unMML->iTicks-=iZenlenght;
		//�S������菬�����Ȃ�����A�������v�Z����B
		} else {
			Ticks2LengthEx(OptionSW,unMML,unMML->iTicks);
			unMML->iTicks=0;
		};
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

	if(unMML->flagCh==0){
		unMML->flagCh=1;
		fprintf(OptionSW->MML.stream,"\nTime(1:1:0);\n	");
		if((unMML->iChannel)>0){
			fprintf(OptionSW->MML.stream,"CH(%d) ",unMML->iChannel);
		};
	};

	if(unMML->iTicks>0){

		fprintf(OptionSW->MML.stream,"r");
		OutputTicks(OptionSW,unMML);

		//�o�͂����̂ŁA���Z�b�g����B
		unMML->flagNote=0;	//����́A�^�C���o��
		unMML->iTicks=0;	//�����͂O

	};

};
/****************************************************************/
/*																*/
/*				�t�R���p�C���@�f�R�[�h�p�E�T�u���[�`��			*/
/*																*/
/****************************************************************/
//##############################################################
//		Channel Voice Message ���̃C�x���g
//##############################################################
//==============================================================
//		�R���g���[���`�F���W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//					OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//					unMMLinfo	unMML		�tMML�p�\����
//		unsigned	char		cData1		���W�X�^
//		unsigned	char		cData2		�f�[�^
//	���Ԓl
//			����
//==============================================================
void DecodeControlChange(OPSW *OptionSW,unMMLinfo *unMML, unsigned char cData1, unsigned char cData2){

	//�R���g���[���ʏ���
	switch(cData1){
		//--------------
		//Modulation
		case(1):
			fprintf(OptionSW->MML.stream,"M(%d) ",cData2);
			break;
		//--------------
		//�|���^�����g�^�C��
		case(5):
			fprintf(OptionSW->MML.stream,"PT(%d) ",cData2);
			break;
		//--------------
		//DataMSB
		case(6):
			fprintf(OptionSW->MML.stream,"DataMSB(%d) ",cData2);
			break;
		//--------------
		//Main Volume
		case(7):
			fprintf(OptionSW->MML.stream,"V(%d) ",cData2);
			break;
		//--------------
		//Panpot
		case(10):
			fprintf(OptionSW->MML.stream,"P(%d) ",cData2);
			break;
		//--------------
		//Expression
		case(11):
			fprintf(OptionSW->MML.stream,"EP(%d) ",cData2);
			break;
		//--------------
		//�|���^�����g�X�C�b�`
		case(65):
			fprintf(OptionSW->MML.stream,"PS(%d) ",cData2);
			break;
		//--------------
		//���o�[�u
		case(91):
			fprintf(OptionSW->MML.stream,"Reverb(%d) ",cData2);
			break;
		//--------------
		//�R�[���X
		case(93):
			fprintf(OptionSW->MML.stream,"Chorus(%d) ",cData2);
			break;
		//--------------
		//�Z���X�e
		case(94):
			fprintf(OptionSW->MML.stream,"VAR(%d) ",cData2);
			break;
		//--------------
		//���̑�
		default:
			fprintf(OptionSW->MML.stream,"y%d,%d ",cData1,cData2);
			break;
	};

};
//##############################################################
//		MIDI�̃C�x���g
//##############################################################
//==============================================================
//		�`�����l���E�{�C�X�E���b�Z�[�W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//				OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//				unMMLinfo	unMML		�tMML�p�\����
//	unsigned	char		cStatus		�X�e�[�^�X�o�C�g
//	unsigned	char		cData1		�f�[�^�o�C�g�P
//	unsigned	char		cData2		�f�[�^�o�C�g�Q�@�i�����ꍇ�͖��g�p�j
//	���Ԓl
//			����
//==============================================================
void DecodeChannelVoiceMessage(OPSW *OptionSW,unMMLinfo *unMML,unsigned char cStatus,unsigned char cData1,unsigned char cData2){

	//----------------------------------
	//��Local �ϐ�
	unsigned	int			iChannel;		//�`�����l�����
	unsigned	char		cCommand;		//�R�}���h���

	//--------------
	//�`�����l���ƃR�}���h�ɕ�������
	iChannel = cStatus & 0x0F;		//�`�����l��
	cCommand = cStatus >> 4;		//�R�}���h

	//--------------
	//�R�}���h�ʏ���
	switch(cCommand){
		//--------------
		//Note Off Message
		case(0x08) :
			fprintf(OptionSW->MML.stream,"NoteOff(%d,%d); ",cData1,cData2);
			break;
		//--------------
		//Note On Message
		case(0x09) :
			fprintf(OptionSW->MML.stream,"NoteOn(%d,%d); ",cData1,cData2);
			break;
		//--------------
		//Polyhonic Key Pressure 
		case(0x0A) :
			fprintf(OptionSW->MML.stream,"DirectSMF(x%X,%d,%d); ",cStatus,cData1,cData2);
			break;
		//--------------
		//Control Change Message
		case(0x0B) :
			DecodeControlChange(OptionSW,unMML,cData1,cData2);
			break;
		//--------------
		//Program Change Message
		case(0x0C) :
			fprintf(OptionSW->MML.stream,"Voice(%d); ",cData1);
			break;
		//--------------
		//Channel Pressure 
		case(0x0D) :
			fprintf(OptionSW->MML.stream,"DirectSMF(x%X,%d); ",cStatus,cData1);
			break;
		//--------------
		//Pitch Bend Change Message
		case(0x0E) :
			fprintf(OptionSW->MML.stream,"PitchBend(%d); ",(((cData2<<7)+cData1)-8192));
			break;
		//--------------
		//�����ȊO
		default:
			not_smf(OptionSW->SMF.name,"������Channel Voice Message�B\n");
			break;
	};


};
//==============================================================
//		�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML		�tMML�p�\����
//			__int32		iSize		�f�[�^�̃T�C�Y
//	���Ԓl
//			����
//==============================================================
void DecodeSystemExcusiveMessage(OPSW *OptionSW,unMMLinfo *unMML, __int32 iSize){

	fprintf(OptionSW->MML.stream,"SysEx = xF0,");
	OutputHex(OptionSW,iSize);

};
//==============================================================
//		�V�X�e���E�R�����E���b�Z�[�W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML		�tMML�p�\����
//			__int32		iSize		�f�[�^�̃T�C�Y
//	���Ԓl
//			����
//==============================================================
void DecodeSystemCommonMessage(OPSW *OptionSW,unMMLinfo *unMML, __int32 iSize){

	fprintf(OptionSW->MML.stream,"SysEx = ");
	OutputHex(OptionSW,iSize);

};
//==============================================================
//		���^�C�x���g�̃f�R�[�h
//--------------------------------------------------------------
//	������
//				OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//				unMMLinfo	unMML		�tMML�p�\����
//	unsigned	char		cData1		�f�[�^�o�C�g�P
//				__int32		iSize		�f�[�^�̃T�C�Y
//	���Ԓl
//			����
//==============================================================
void DecodeMetaEvent(OPSW *OptionSW,unMMLinfo *unMML, unsigned char cData1, __int32 iSize){

	//----------------------------------
	//��Local �ϐ�
	unsigned char	cData[4];	//�f�[�^

	//�C�x���g�ʏ���
	switch(cData1){
		//--------------
		//�V�[�P���X�ԍ�
		case(0x00) :
			if(iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x00,x02,");
				OutputHex(OptionSW,iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"�����ȃV�[�P���X�ԍ��B\n");
			}
			break;
		//--------------
		//�e�L�X�g
		case(0x01) :
			fprintf(OptionSW->MML.stream,"MetaText={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//���쌠�\��
		case(0x02) :
			fprintf(OptionSW->MML.stream,"Copyright={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//�V�[�P���X���^�g���b�N��
		case(0x03) :
			fprintf(OptionSW->MML.stream,"TrackName={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//�y�햼
		case(0x04) :
			fprintf(OptionSW->MML.stream,"InstrumentName={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//�̎�
		case(0x05) :
			fprintf(OptionSW->MML.stream,"Lyric={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//�}�[�J
		case(0x06) :
			fprintf(OptionSW->MML.stream,"Marker={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//�L���[�|�C���g
		case(0x07) :
			fprintf(OptionSW->MML.stream,"CuePoint={");
			OutputStrings(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//MIDI�`�����l���v���t�B�b�N�X
		case(0x20) :
			if(iSize==1){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"ChannelPrefix(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"�����ȃ`�����l���v���t�B�b�N�X�B\n");
			}
			break;
		//--------------
		//�|�[�g�ԍ�
		case(0x21) :
			if(iSize==1){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Port(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"�����ȃ|�[�g�ԍ��B\n");
			}
			break;
		//--------------
		//�G���h�I�u�g���b�N	to do
		case(0x2F) :
			if(iSize==0){
				//�`�����l���̋tMML�R���p�C�����I���B
				unMML->flagEoT=1;
			} else {
				not_smf(OptionSW->SMF.name,"������End of Track�B\n");
			}
			break;
		//--------------
		//�e���|
		case(0x51) :
			if(iSize==3){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Tempo(%d); ",(60000000/((cData[0]<<16)+(cData[1]<<8)+(cData[2]))));
			} else {
				not_smf(OptionSW->SMF.name,"�����ȁB\n");
			}
			break;
		//--------------
		//SMPTE�I�t�Z�b�g
		case(0x54) :
			if(iSize==5){
				fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x54,x05,");
				OutputHex(OptionSW,iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"������SMPTE�I�t�Z�b�g�B\n");
			}
			break;
		//--------------
		//���q
		case(0x58) :
			if(iSize==4){
				fread((void *)&cData,sizeof(char),iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"TimeSignature(%d,%d);\n	",cData[0],1<<cData[1]);
			} else {
				not_smf(OptionSW->SMF.name,"�����Ȕ��q�B\n");
			}
			break;
		//--------------
		//����	�i�悸�́ADirectSMF�BKeyFlag�őΉ��ł���H�j
		case(0x59) :
			if(iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x59,x02,");
				OutputHex(OptionSW,iSize);
				fprintf(OptionSW->MML.stream,");\n	");
			} else {
				not_smf(OptionSW->SMF.name,"�����ȁB\n");
			}
			break;
		//--------------
		//�V�[�P���T�ŗL�̃��^�C�x���g�i���Ή��j
		case(0x7F) :
			fprintf(OptionSW->MML.stream,"DirectSMF(xFF,x7F,x00); /*");
			OutputHex(OptionSW,iSize);
			fprintf(OptionSW->MML.stream,"*/ ");
			break;
		//--------------
		//
		default:
			not_smf(OptionSW->SMF.name,"������Meta Event\n");
			break;
	};

};
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

	fprintf(OptionSW->MML.stream,"\n\n");
	fprintf(OptionSW->MML.stream,"//--------------------------------------\n");
	fprintf(OptionSW->MML.stream,"//MThd Infomation :\n");
	fprintf(OptionSW->MML.stream,"//	Size = %d \n",unMML->iTrackSize);
	fprintf(OptionSW->MML.stream,"//\n");
	fprintf(OptionSW->MML.stream,"TR(%d)\n",(unMML->iTrack+1));


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
	unMML->iTicks=0;
	unMML->cRunningStatus=0;
	unMML->iLength=4;
	unMML->iOctave=5;
	unMML->iGate=0;

	unMML->cNote=0xFF;	//�m�[�g
	unMML->flagNote=1;	//�o�͏��
	unMML->flagCh=0;

	unMML->flagEoT=0;

}
//##############################################################
//		MTrk�`�����N
//##############################################################
//==============================================================
//		�P�R�}���h�̃f�R�[�h	�i���C���f�R�[�h�j
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//	���Ԓl
//			__int32	iDeltaTime	�f���^�^�C��
//==============================================================
void Decode1Command(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	unsigned	char		cFread;			//fread()�֐��ł̓ǂݍ��ݗp

	unsigned	__int32		iDeltaTime;		//�f���^�^�C��

	unsigned	int			iChannel;		//�`�����l�����
	unsigned	char		cCommand;		//�R�}���h���
	unsigned	char		cStatus;		//�X�e�[�^�X
	unsigned	char		cData1;			//�f�[�^�P
	unsigned	char		cData2;			//�f�[�^�Q


	//----------------------------------
	//���f���^�^�C���̓ǂݍ���
	iDeltaTime=DecodeVariable(OptionSW);
	unMML->iTicks+=iDeltaTime;				//���f���^�^�C���ɉ��Z
	unMML->iTotalTicks+=iDeltaTime;			//���f���^�^�C���ɉ��Z


	//----------------------------------
	//��MIDI�C�x���g�̏���

	//�܂��A1Byte�ǂݍ���
	fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
	//�ŏ�����f�[�^�r�b�g��������A�����j���O�X�e�[�^�X��K�p����
	if(cFread&0x80){
		cStatus=cFread;
		cData1=0xFF;
	} else {
		cStatus=unMML->cRunningStatus;
		cData1=cFread;
	};

	//----------------------
	//���R�}���h�ʏ���
	//��Channel Voice Message�H
	if(cStatus<0xF0){

	unMML->cRunningStatus=cStatus;	//�����j���O�X�e�[�^�X�̍X�V

		//--------------
		//�`�����l���ƃR�}���h�ɕ�������
		iChannel = cStatus & 0x0F;		//�`�����l��
		cCommand = cStatus >> 4;		//�R�}���h
		//�����j���O�X�e�[�^�X���K�p����ĂȂ�������A�ŏ��̃r�b�g��ǂ�
		if(cData1==0xFF){
			fread((void *)&cData1,sizeof(char),1,OptionSW->SMF.stream);
		};
		//�f�[�^2������ꍇ�A�f�[�^��ǂށB
		if(((cCommand>=0x08)&&(cCommand<=0x0B))||(cCommand==0x0E)){
			fread((void *)&cData2,sizeof(char),1,OptionSW->SMF.stream);
		};

		//--------------
		//�t�R���p�C������Channel��������A�������o�͂��āA�C�x���g���o��
		if(unMML->iChannel==(iChannel+1)){

			//--------------
			//�����̏o��
			OutputNote(OptionSW,unMML);

			//--------------
			//�`�����l���E�{�C�X�E���b�Z�[�W
			DecodeChannelVoiceMessage(OptionSW,unMML,cStatus,cData1,cData2);
		};

	//--------------
	//��System Excusive Message
	//��System Common Message
	//��Meta Event
	} else {

		//Channel 0��������A�������o�͂��āA�C�x���g���o��
		if(unMML->iChannel==0){

			//--------------
			//�����̏o��
			OutputNote(OptionSW,unMML);

			//--------------
			//���b�Z�[�W�ʏ���
			switch(cStatus){
				//--------------
				//�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W
				case(0xF0):
					DecodeSystemExcusiveMessage(OptionSW,unMML,DecodeVariable(OptionSW));
					break;
				//--------------
				//�V�X�e���E�R�����E���b�Z�[�W
				case(0xF7):
					DecodeSystemCommonMessage(OptionSW,unMML,DecodeVariable(OptionSW));
					break;
				//--------------
				//���^�E�C�x���g
				case(0xFF):
					fread((void *)&cData1,sizeof(char),1,OptionSW->SMF.stream);
					DecodeMetaEvent(OptionSW,unMML,cData1,DecodeVariable(OptionSW));
					break;
				//--------------
				//���̑�
				default:
					not_smf(OptionSW->SMF.name,"������MIDI�C�x���g�����o���܂����B\n");
					break;
			};

		//Channel 1�ȊO��������A�X�g���[���̃|�C���^�����i�߂�
		} else {

			//���b�Z�[�W�ʏ���
			switch(cStatus){
				//--------------
				//�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W
				case(0xF0):
					StreamPointerAdd(&OptionSW->SMF,DecodeVariable(OptionSW));
					break;
				//--------------
				//�V�X�e���E�R�����E���b�Z�[�W
				case(0xF7):
					StreamPointerAdd(&OptionSW->SMF,DecodeVariable(OptionSW));
					break;
				//--------------
				//���^�E�C�x���g
				case(0xFF):
					fread((void *)&cData1,sizeof(char),1,OptionSW->SMF.stream);
					StreamPointerAdd(&OptionSW->SMF,DecodeVariable(OptionSW));
					if(cData1==0x2F){
						unMML->flagEoT=1;	//EoT�����̓`�F�b�N
					};
					break;
				//--------------
				//���̑�
				default:
					not_smf(OptionSW->SMF.name,"������MIDI�C�x���g�����o���܂����B\n");
					break;
			};
		};

	};

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
		uncompile_initTR(OptionSW,&unMML);
		unMML.iChannel=0;
		while(unMML.iChannel<=16){
			uncompile_initCH(OptionSW,&unMML);
			while(unMML.flagEoT==0){
				Decode1Command(OptionSW,&unMML);
			};
			unMML.iChannel++;
		};
		unMML.iTrack++;
	};
};

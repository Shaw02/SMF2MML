
#include "stdafx.h"

/****************************************************************/
/*																*/
/*			�v���g�^�C�v�錾									*/
/*																*/
/****************************************************************/
extern	void not_smf(char *filename, char *stMsg);
extern	void StreamPointerAdd(FILE *stream,__int32 iSize);
extern	void OutputHex(OPSW *OptionSW,__int32 iSize);
extern	void OutputStrings(OPSW *OptionSW,__int32 iSize);
extern	void Ticks2LengthEx(OPSW *OptionSW,unMMLinfo *unMML, int iTicks);
extern	void OutputNoteEx(OPSW *OptionSW,unMMLinfo *unMML);
extern	void OutputNote(OPSW *OptionSW,unMMLinfo *unMML);

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
	unsigned	__int32		iDeltaTime;		//�ǂݍ��񂾉ϒ����l
	unsigned	char		cFread;			//�ǂݍ��ݗp�ϐ�
	unsigned	int			count;			//�ǂݍ��݉񐔃J�E���g�p

	//----------------------------------
	//���f���^�^�C���ǂݍ���
	iDeltaTime=0;	//�����l��0
	count=4;		//�ő�4Byte�B�]���āA�ǂݍ��݂͂S��܂�
	do{
		fread((void *)&cFread,sizeof(char),1,OptionSW->SMF.stream);
		iDeltaTime=(iDeltaTime<<7)+((unsigned __int32)cFread&0x7F);
		count--;
	} while((count>0)&&(cFread&0x80));

	return(iDeltaTime);

};
//==============================================================
//		�C�x���g�̓ǂݍ���
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW		�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML		�tMML�p�\����
//	���Ԓl
//	unsigned	__int32	iDeltaTime	�f���^�^�C��
//			unMML->cRunningStatus	����̃X�e�[�^�X�E�o�C�g
//			unMML->cStatus			�X�e�[�^�X�E�o�C�g
//			unMML->cData1			�f�[�^�E�o�C�g�P
//			unMML->cData2			�f�[�^�E�o�C�g�Q
//			unMML->iSize			�f�[�^�̃T�C�Y
//==============================================================
unsigned	__int32	EventRead(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	unsigned	__int32		iDeltaTime;		//�f���^�^�C��

	unMML->iSize=0;	//����́A�f�t�H���g�O

	//----------------------------------
	//���f���^�^�C���̓ǂݍ��݁i�֐��̕Ԓl�͂���ɂ���B�j
	iDeltaTime=DecodeVariable(OptionSW);

	//----------------------------------
	//��MIDI�C�x���g�̏���
	//�܂��A1Byte�ǂݍ���
	fread((void *)&unMML->cStatus,sizeof(char),1,OptionSW->SMF.stream);
	//�X�e�[�^�X�������ꍇ�i���^�C�x���g�E�V�X�e���G�N�X�N���[�V�u�̉\���L��j
	if(unMML->cStatus&0x80){
		unMML->cData1=0xFF;	//�悸�́A�t���O�Ƃ��ăZ�b�g
	//�f�[�^�������ꍇ�A�����j���O�X�e�[�^�X�̓K�p
	} else {
		unMML->cData1=unMML->cStatus;
		unMML->cStatus=unMML->cRunningStatus;
	};

	//----------------------
	//���R�}���h�ʏ���
	//��Channel Voice Message�H
	if(unMML->cStatus<0xF0){

		unMML->cRunningStatus=unMML->cStatus;	//�����j���O�X�e�[�^�X�̍X�V

		//�����j���O�X�e�[�^�X���K�p����ĂȂ�������A�ŏ��̃r�b�g��ǂ�
		if(unMML->cData1==0xFF){
			fread((void *)&unMML->cData1,sizeof(char),1,OptionSW->SMF.stream);
		};
		//�f�[�^2������ꍇ�A�f�[�^��ǂށB
		if(((unMML->cStatus>=0x80)&&(unMML->cStatus<=0xBF))||((unMML->cStatus>=0xE0)&&(unMML->cStatus<=0xEF))){
			fread((void *)&unMML->cData2,sizeof(char),1,OptionSW->SMF.stream);
		};

	//--------------
	//��System Excusive Message
	//��System Common Message
	//��Meta Event
	} else {

		//���b�Z�[�W�ʏ���
		switch(unMML->cStatus){
			//--------------
			//�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W
			//�V�X�e���E�R�����E���b�Z�[�W
			case(0xF0):
			case(0xF7):
				unMML->iSize=DecodeVariable(OptionSW);
				break;
			//--------------
			//���^�E�C�x���g
			case(0xFF):
				fread((void *)&unMML->cData1,sizeof(char),1,OptionSW->SMF.stream);
				unMML->iSize=DecodeVariable(OptionSW);
				if(unMML->cData1==0x2F){
					unMML->flagEoT=1;	//EoT�����͋����`�F�b�N
				};
				break;
			//--------------
			//���̑�
			default:
				not_smf(OptionSW->SMF.name,"������MIDI�C�x���g�����o���܂����B\n");
				break;
		};
	};

	return(iDeltaTime);

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
//		�m�[�g�E�����^�������̃f�R�[�h
//--------------------------------------------------------------
//	������
//					OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//					unMMLinfo	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void NoteDecode(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�

	unsigned	__int32	iLength=0;	//����
	//�X�e�[�^�X�o�C�g�E�f�[�^�o�C�g��ۑ�
				char	cTempEoT	= unMML->flagEoT;		//End of Track�̃t���O
	unsigned	char	cTempStatus	= unMML->cStatus;
	unsigned	char	cTempData1	= unMML->cData1;
	unsigned	char	cTempData2	= unMML->cData2;

				char	flagKeyOn	= 0;	//����key on���������B	
				char	flagKeyOff	= 0;	//key off�������B	
	long				fptStream;			//�t�@�C���|�C���^

	//--------------
	//�X�g���[���|�C���^�̕ۑ�
	fptStream = ftell(OptionSW->SMF.stream);

	//--------------
	//Note Off����������
	while((flagKeyOff==0)&&(unMML->flagEoT==0)){
		iLength+=EventRead(OptionSW,unMML);

		//�|�C���^��i�߂�
		if(unMML->iSize>0){
			StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);
		};

		//���ɃR�}���h�������H
		if(((unMML->cStatus==cTempStatus)&&(unMML->cData2>0)) ||
			((unMML->cStatus>=0xA0)&&(unMML->cStatus<0xF0)&&((unMML->cStatus&0x0F)==(cTempStatus&0x0F))))
		{
			flagKeyOn=1;
		};
		//Note Off�������H
		if(((unMML->cStatus==(cTempStatus&0x8F))&&(unMML->cData1==cTempData1)) ||
			((unMML->cStatus==(cTempStatus&0x9F))&&(unMML->cData1==cTempData1)&&(unMML->cData2==0))) {
			flagKeyOff=1;
		};
	};

	//����key on����������B
	if(flagKeyOn==1){
		fprintf(OptionSW->MML.stream," Sub{");
		unMML->cNote=cTempData1;	//�Ƃ肠�����o��
		unMML->cVelo=cTempData2;	
		unMML->flagNote=1;			//Key On
		//�������o��
		OutputNoteEx(OptionSW,unMML);
		//�������o��
		Ticks2LengthEx(OptionSW,unMML,iLength);
		fprintf(OptionSW->MML.stream,"}");
		unMML->cNote=0xFF;
	}else{
		unMML->cNote=cTempData1;
		unMML->cVelo=cTempData2;	
		unMML->flagNote=1;	//Key On
	}

	//--------------
	//���A
	unMML->flagEoT	= cTempEoT;		//End of Track�̃t���O
	unMML->cStatus	= cTempStatus;
	unMML->cData1	= cTempData1;
	unMML->cData2	= cTempData2;

	//�|�C���^��߂�
	fseek(OptionSW->SMF.stream,fptStream,SEEK_SET);
}
//==============================================================
//		�R���g���[���`�F���W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//					OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//					unMMLinfo	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void DecodeControlChange(OPSW *OptionSW,unMMLinfo *unMML){

	//�R���g���[���ʏ���
	switch(unMML->cData1){
		//--------------
		//Modulation
		case(1):
			fprintf(OptionSW->MML.stream,"M(%d); ",unMML->cData2);
			break;
		//--------------
		//�|���^�����g�^�C��
		case(5):
			fprintf(OptionSW->MML.stream,"PT(%d); ",unMML->cData2);
			break;
		//--------------
		//DataMSB
		case(6):
			fprintf(OptionSW->MML.stream,"DataMSB(%d); ",unMML->cData2);
			break;
		//--------------
		//Main Volume
		case(7):
			fprintf(OptionSW->MML.stream,"V(%d); ",unMML->cData2);
			break;
		//--------------
		//Panpot
		case(10):
			fprintf(OptionSW->MML.stream,"P(%d); ",unMML->cData2);
			break;
		//--------------
		//Expression
		case(11):
			fprintf(OptionSW->MML.stream,"EP(%d); ",unMML->cData2);
			break;
		//--------------
		//�|���^�����g�X�C�b�`
		case(65):
			fprintf(OptionSW->MML.stream,"PS(%d); ",unMML->cData2);
			break;
		//--------------
		//���o�[�u
		case(91):
			fprintf(OptionSW->MML.stream,"Reverb(%d); ",unMML->cData2);
			break;
		//--------------
		//�R�[���X
		case(93):
			fprintf(OptionSW->MML.stream,"Chorus(%d); ",unMML->cData2);
			break;
		//--------------
		//�Z���X�e
		case(94):
			fprintf(OptionSW->MML.stream,"VAR(%d); ",unMML->cData2);
			break;
		//--------------
		//���̑�
		default:
			fprintf(OptionSW->MML.stream,"y%d,%d; ",unMML->cData1,unMML->cData2);
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
//	���Ԓl
//			����
//==============================================================
void DecodeChannelVoiceMessage(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	unsigned	int			iChannel;		//�`�����l�����
	unsigned	char		cCommand;		//�R�}���h���

	//--------------
	//�`�����l���ƃR�}���h�ɕ�������
	iChannel = unMML->cStatus & 0x0F;		//�`�����l��
	cCommand = unMML->cStatus >> 4;		//�R�}���h

	//--------------
	//�R�}���h�ʏ���
	switch(cCommand){
		//--------------
		//Note Off Message
		case(0x08) :
//			fprintf(OptionSW->MML.stream,"NoteOff(%d,%d); ",unMML->cData1,unMML->cData2);
			if(unMML->cData1==unMML->cNote){
				unMML->cNote=0xFF;			//Note Off (�x��)
			};
			break;
		//--------------
		//Note On Message
		case(0x09) :
//			fprintf(OptionSW->MML.stream,"NoteOn(%d,%d); ",unMML->cData1,unMML->cData2);
			if((unMML->cData1==unMML->cNote)&&(unMML->cData2==0)){
				unMML->cNote=0xFF;			//Note Off (�x��)
			} else {
				NoteDecode(OptionSW,unMML);	//Note On
			};
			break;
		//--------------
		//Polyhonic Key Pressure 
		case(0x0A) :
			fprintf(OptionSW->MML.stream,"DirectSMF($A0+Channel-1,%d,%d); ",unMML->cData1,unMML->cData2);
			break;
		//--------------
		//Control Change Message
		case(0x0B) :
			DecodeControlChange(OptionSW,unMML);
			break;
		//--------------
		//Program Change Message
		case(0x0C) :
			fprintf(OptionSW->MML.stream,"Voice(%d); ",(unMML->cData1)+1);
			break;
		//--------------
		//Channel Pressure 
		case(0x0D) :
			fprintf(OptionSW->MML.stream,"DirectSMF($D0+Channel-1,%d); ",unMML->cData1);
			break;
		//--------------
		//Pitch Bend Change Message
		case(0x0E) :
			fprintf(OptionSW->MML.stream,"PitchBend(%d); ",((((unMML->cData2)<<7)+(unMML->cData1))-8192));
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
//	���Ԓl
//			����
//==============================================================
void DecodeSystemExcusiveMessage(OPSW *OptionSW,unMMLinfo *unMML){

	fprintf(OptionSW->MML.stream,"SysEx = $F0,");
	OutputHex(OptionSW,unMML->iSize);
	fprintf(OptionSW->MML.stream,";\n	");

};
//==============================================================
//		�V�X�e���E�R�����E���b�Z�[�W�̃f�R�[�h
//--------------------------------------------------------------
//	������
//			OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//			unMMLinfo	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void DecodeSystemCommonMessage(OPSW *OptionSW,unMMLinfo *unMML){

	fprintf(OptionSW->MML.stream,"SysEx = ");
	OutputHex(OptionSW,unMML->iSize);
	fprintf(OptionSW->MML.stream,";\n	");

};
//==============================================================
//		���^�C�x���g�̃f�R�[�h
//--------------------------------------------------------------
//	������
//				OPSW		*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//				unMMLinfo	unMML		�tMML�p�\����
//	���Ԓl
//			����
//==============================================================
void DecodeMetaEvent(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	unsigned	char	cData[4];	//�f�[�^
	unsigned	int		iData;		//�ėp

	//�C�x���g�ʏ���
	switch(unMML->cData1){
		//--------------
		//�V�[�P���X�ԍ�
		case(0x00) :
			if(unMML->iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF($FF,$00,$02,");
				OutputHex(OptionSW,unMML->iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"�����ȃV�[�P���X�ԍ��B\n");
			}
			break;
		//--------------
		//�e�L�X�g
		case(0x01) :
			fprintf(OptionSW->MML.stream,"MetaText={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//���쌠�\��
		case(0x02) :
			fprintf(OptionSW->MML.stream,"Copyright={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//�V�[�P���X���^�g���b�N��
		case(0x03) :
			fprintf(OptionSW->MML.stream,"TrackName={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//�y�햼
		case(0x04) :
			fprintf(OptionSW->MML.stream,"InstrumentName={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"};\n	");
			break;
		//--------------
		//�̎�
		case(0x05) :
			fprintf(OptionSW->MML.stream,"Lyric={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//�}�[�J
		case(0x06) :
			fprintf(OptionSW->MML.stream,"Marker={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//�L���[�|�C���g
		case(0x07) :
			fprintf(OptionSW->MML.stream,"CuePoint={");
			OutputStrings(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"}; ");
			break;
		//--------------
		//MIDI�`�����l���v���t�B�b�N�X
		case(0x20) :
			if(unMML->iSize==1){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"ChannelPrefix(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"�����ȃ`�����l���v���t�B�b�N�X�B\n");
			}
			break;
		//--------------
		//�|�[�g�ԍ�
		case(0x21) :
			if(unMML->iSize==1){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Port(%d); ",cData[0]);
			} else {
				not_smf(OptionSW->SMF.name,"�����ȃ|�[�g�ԍ��B\n");
			}
			break;
		//--------------
		//�G���h�I�u�g���b�N
		case(0x2F) :
			if(unMML->iSize==0){
				//�`�����l���̋tMML�R���p�C�����I���B
				unMML->flagEoT=1;
			} else {
				not_smf(OptionSW->SMF.name,"������End of Track�B\n");
			}
			break;
		//--------------
		//�e���|
		case(0x51) :
			if(unMML->iSize==3){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				fprintf(OptionSW->MML.stream,"Tempo(%d); ",(60000000/((cData[0]<<16)+(cData[1]<<8)+(cData[2]))));
			} else {
				not_smf(OptionSW->SMF.name,"�����ȁB\n");
			}
			break;
		//--------------
		//SMPTE�I�t�Z�b�g
		case(0x54) :
			if(unMML->iSize==5){
				fprintf(OptionSW->MML.stream,"DirectSMF($FF,$54,$05,");
				OutputHex(OptionSW,unMML->iSize);
				fprintf(OptionSW->MML.stream,"); ");
			} else {
				not_smf(OptionSW->SMF.name,"������SMPTE�I�t�Z�b�g�B\n");
			}
			break;
		//--------------
		//���q
		case(0x58) :
			if(unMML->iSize==4){
				fread((void *)&cData,sizeof(char),unMML->iSize,OptionSW->SMF.stream);
				iData=1<<cData[1];
				//��͂͑O�����ł�邪�A�\���͂����ł���B
				fprintf(OptionSW->MML.stream,"TimeSignature(%d,%d);\n	",cData[0],iData);
			} else {
				not_smf(OptionSW->SMF.name,"�����Ȕ��q�B\n");
			}
			break;
		//--------------
		//����	�ito do �悸�́ADirectSMF�BTimeKeyFlag�őΉ��ł���H�j
		case(0x59) :
			if(unMML->iSize==2){
				fprintf(OptionSW->MML.stream,"DirectSMF($FF,$59,$02,");
				OutputHex(OptionSW,unMML->iSize);
				fprintf(OptionSW->MML.stream,");\n	");
			} else {
				not_smf(OptionSW->SMF.name,"�����Ȓ����B\n");
			}
			break;
		//--------------
		//�V�[�P���T�ŗL�̃��^�C�x���g�i���Ή��j
		case(0x7F) :
			fprintf(OptionSW->MML.stream,"DirectSMF($FF,$7F,$00); /*");
			OutputHex(OptionSW,unMML->iSize);
			fprintf(OptionSW->MML.stream,"*/ ");
			break;
		//--------------
		//
		default:
			not_smf(OptionSW->SMF.name,"������Meta Event\n");
			break;
	};

};
//==============================================================
//		�P�R�}���h�̃f�R�[�h	�i���C���f�R�[�h�j
//--------------------------------------------------------------
//	������
//			OPSW	*OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//				unMMLinfo	unMML		�tMML�p�\����
//	���Ԓl
//			__int32	iDeltaTime	�f���^�^�C��
//==============================================================
void Decode1Command(OPSW *OptionSW,unMMLinfo *unMML){

	//----------------------------------
	//��Local �ϐ�
	unsigned	__int32		iDeltaTime;		//�ǂݍ��񂾉ϒ����l
	unsigned	int			iChannel;		//�`�����l�����
	unsigned	char		cCommand;		//�R�}���h���

	//MIDI�C�x���g�ǂݍ���
	iDeltaTime = EventRead(OptionSW,unMML);
	unMML->iTicks+=iDeltaTime;				//���f���^�^�C���ɉ��Z
	unMML->iTotalTicks+=iDeltaTime;			//���f���^�^�C���ɉ��Z

	//----------------------
	//���R�}���h�ʏ���
	//��Channel Voice Message�H
	if(unMML->cStatus<0xF0){

		//--------------
		//�`�����l���ƃR�}���h�ɕ�������
		iChannel = unMML->cStatus & 0x0F;	//�`�����l��
		cCommand = unMML->cStatus >> 4;		//�R�}���h

		//�t�R���p�C������Channel��������A�������o�͂��āA�C�x���g���o��
		if(unMML->iChannel==(iChannel+1)){

			//--------------
			//�����̏o��
			OutputNote(OptionSW,unMML);

			//--------------
			//�`�����l���E�{�C�X�E���b�Z�[�W
			DecodeChannelVoiceMessage(OptionSW,unMML);
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
			//�A���AEoT���͂��Ȃ��B
			if(unMML->flagEoT==0){
				OutputNote(OptionSW,unMML);
			};

			//--------------
			//���b�Z�[�W�ʏ���
			switch(unMML->cStatus){
				//--------------
				//�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W
				case(0xF0):
					DecodeSystemExcusiveMessage(OptionSW,unMML);
					break;
				//--------------
				//�V�X�e���E�R�����E���b�Z�[�W
				case(0xF7):
					DecodeSystemCommonMessage(OptionSW,unMML);
					break;
				//--------------
				//���^�E�C�x���g
				case(0xFF):
					DecodeMetaEvent(OptionSW,unMML);
					break;
				//--------------
				//���̑�
				default:
					not_smf(OptionSW->SMF.name,"������MIDI�C�x���g�����o���܂����B\n");
					break;
			};

		//Channel 1�ȊO��������A
		} else {
			//EOT��������A����܂ł�tick���̋x�����o�͂���B
			if((unMML->flagEoT==1)&&(unMML->flagCh==1)){
				OutputNote(OptionSW,unMML);
			};
			//�X�g���[���̃|�C���^��i�߂�
			StreamPointerAdd(OptionSW->SMF.stream,unMML->iSize);

		};

	};

};

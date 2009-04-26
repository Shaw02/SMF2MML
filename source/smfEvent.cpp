
#include "stdafx.h"

//==============================================================
//		�ϒ��f�R�[�h
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//			__int32	iDeltaTime	�f���^�^�C��
//==============================================================
unsigned __int32	SMF_event::DecodeVariable(){

	//----------------------------------
	//��Local �ϐ�
	unsigned	__int32		iDeltaTime=0;	//�ǂݍ��񂾉ϒ����l
	unsigned	int			count=4;		//�ǂݍ��݉񐔃J�E���g�p
	unsigned	char		cFread;			//�ǂݍ��ݗp�ϐ�

	//----------------------------------
	//���f���^�^�C���ǂݍ���
	do{
		cFread=Read();		//1 Byte Read
		iDeltaTime=(iDeltaTime<<7)+((unsigned int)cFread&0x7F);
		count--;
	} while((count>0)&&(cFread&0x80));

	return(iDeltaTime);

};
//==============================================================
//		�C�x���g�̓ǂݍ���
//--------------------------------------------------------------
//	������
//			����
//	���Ԓl
//	unsigned	__int32	iDeltaTime	�f���^�^�C��
//			unMML->cRunningStatus	����̃X�e�[�^�X�E�o�C�g
//			unMML->cStatus			�X�e�[�^�X�E�o�C�g(01==error)
//			unMML->cData1			�f�[�^�E�o�C�g�P
//			unMML->cData2			�f�[�^�E�o�C�g�Q
//			unMML->iSize			�f�[�^�̃T�C�Y
//==============================================================
unsigned	__int32	SMF_event::EventRead(){

	//----------------------------------
	//��Local �ϐ�
							//�f���^�^�C���̓ǂݍ���
	const	unsigned	__int32		iDeltaTime=DecodeVariable();
	const	unsigned	int			iMask=OptionSW->iMaskFlag;

	//----------------------------------
	//��MIDI�C�x���g�̏���
	//�܂��A1Byte�ǂݍ���
	cStatus=Read();

	//----------------------
	//���R�}���h�ʏ���
	//��Channel Voice Message�H
	if(cStatus<0xF0){

		iSize=0;	//����͂O

		//�X�e�[�^�X�������ꍇ�i���^�C�x���g�E�V�X�e���G�N�X�N���[�V�u�̉\���L��j
		if(cStatus&0x80){
			cData1=Read();
			cRunningStatus=cStatus;	//�����j���O�X�e�[�^�X�̍X�V
		//�f�[�^�������ꍇ�A�����j���O�X�e�[�^�X�̓K�p
		} else {
			cData1=cStatus;
			cStatus=cRunningStatus;
		};

		//�f�[�^2������ꍇ�A�f�[�^��ǂށB
		if(((cStatus>=0x80)&&(cStatus<=0xBF))||((cStatus>=0xE0)&&(cStatus<=0xEF))){
			cData2=Read();
		};

	//--------------
	//��System Excusive Message
	//��System Common Message
	//��Meta Event
	} else {

		//���b�Z�[�W�ʏ���
		switch(cStatus){
			//--------------
			//�V�X�e���E�G�N�X�N���[�V�u�E���b�Z�[�W
			//�V�X�e���E�R�����E���b�Z�[�W
			case(SEXM):
			case(SCM):
				iSize=DecodeVariable();
				break;
			//--------------
			//���^�E�C�x���g
			case(MEM):
				cData1=Read();			//Event Type
				iSize=DecodeVariable();	//Event Size
				if(cData1==0x2F){
					flagEoT=1;	//EoT�����͋����`�F�b�N
				};
				break;
			//--------------
			//���̑�
			default:
				//not_smf("������MIDI�C�x���g�����o���܂����B\n");
				break;
		};
	};



	//--------------
	//�}�X�N���邩�`�F�b�N
	//���̏��ԂŃ`�F�b�N����ƁA�X�}�[�g�ȃR�[�h�ɂȂ����B
	if(	(((cStatus)==SEXM				)&&(iMask & MASK_SEXM	))
	||	(((cStatus)==SCM				)&&(iMask & MASK_SCM	))
	||	(((cStatus)==MEM				)&&(iMask & MASK_MEM	))	
	||	(((cStatus & 0xF0)==CVM_PKPM	)&&(iMask & MASK_CVM_PKPM	))
	||	(((cStatus & 0xF0)==CVM_CCM		)&&(iMask & MASK_CVM_CCM	))
	||	(((cStatus & 0xF0)==CVM_PCM		)&&(iMask & MASK_CVM_PCM	))
	||	(((cStatus & 0xF0)==CVM_CPM		)&&(iMask & MASK_CVM_CPM	))
	||	(((cStatus & 0xF0)==CVM_PBCM	)&&(iMask & MASK_CVM_PBCM	))	){
		cStatus=0;
	}

	//--------------
	//�I��
	return(iDeltaTime);

};


#include "stdafx.h"

/****************************************************************/
/*			�O���錾											*/
/****************************************************************/
extern	void not_smf(const char *stMsg);

/****************************************************************/
/*					�萔										*/
/****************************************************************/
#define	dwMThd	0x6468544D		//'MThd'��16�i���ɂ��Ă݂��B

//==============================================================
//		�C�j�V�����C�Y
//--------------------------------------------------------------
//	������
//			FileOp*	ptSMF	SMF�t�@�C��
//			FileOp*	ptMML	MML�t�@�C��
//	���Ԓl
//			����
//==============================================================
SMF_MThd::SMF_MThd(FileOp* ptSMF):
	SMF(ptSMF)
{

	//----------------------------------
	//��Local �ϐ�
	long		*cSMF_Header;	//if���ňꊇ��'MThd'�����ׂ�p

	//----------------------------------
	//��MThd�w�b�_�[����	with Endian�̕ϊ�(Big��Little)
	SMF->read((char *)this,14);
	//���b����łǂ������Ηǂ����m��Ȃ�����A�A�Z���u������Ȋ֐��B
	//��char�^�ɂ��Ă������񂾂��ǁA�܂��|�C���^�ϐ����₷�̂߂�ǂ����B
	//��"smf2mml_sub.asm"���Q�ƁB
	endian_b2l_D(&size);
	endian_b2l_W(&format);
	endian_b2l_W(&track);
	endian_b2l_W(&timebase);

	//MThd�`�����N�����邩�H
	cSMF_Header=(long *)strings;					//if���ňꊇ��'MThd'�����ׂ��
	if(*cSMF_Header != dwMThd){						//(SMF_Header.strings == 'MThd')?
		not_smf("MThd�`�����N������܂���B\n");
	};

	//�t�H�[�}�b�g��0��1�H
	if(format==2){
		not_smf("Format 2�ɂ͑Ή����Ă���܂���B\n");
	} else if(format>2){
		not_smf("���m�̃t�H�[�}�b�g�ł��B\n");
	};

	//�g���b�N����0�łȂ��H
	if(track==0){
		not_smf("�g���b�N����0�ł��B\n");
	};

	//�t�H�[�}�b�g0���A�g���b�N����1�łȂ��H
	if((format==0)&&(track!=1)){
		not_smf("Format 0�Ȃ̂ɁA�����̃g���b�N������܂��B\n");
	};
	
	//timebase�̃`�F�b�N
	if(timebase==0){
		not_smf("timebase��0tick�ł��B\n");
	}else if(timebase&0x8000){
		not_smf("�b�w��̎��ԒP�ʂɂ͑Ή����Ă���܂���B\n");
	};

	// to do	���̑��w�b�_�̃`�F�b�N		

};


#pragma once

/****************************************************************/
/*																*/
/*			�N���X��`											*/
/*																*/
/****************************************************************/
class OPSW {
public:
				FileOp*		SMF;			//�w�肵��SMF�t�@�C��
				FileOp*		MML;			//�w�肵��MML�t�@�C��
				char		fHelp;			//�w���v���w�肵�����H
	unsigned	int			iBar;			// | �̑}��
	unsigned	int			iCR;			//���s��}�����鏬�ߐ�
	unsigned	int			iMaskFlag;		//�o�͂��}�X�N����@�\
	unsigned	char		cJpn;			//���ƂƂ�H

		OPSW();								//�������̂�
		OPSW(int argc, _TCHAR* argv[]);		//�������e����A�N���X�����������t�@�C���I�[�v��
		~OPSW();							//�t�@�C���N���[�Y
void	opError(const char *stErrMsg);		//�I�v�V�����G���[
};



//==============================================================
//�@�\�}�X�N�p
#define	MASK_CVM_PKPM	0x0004	//AT
#define	MASK_CVM_CCM	0x0008	//y
#define	MASK_CVM_PCM	0x0010	//@
#define	MASK_CVM_CPM	0x0020	//AT
#define	MASK_CVM_PBCM	0x0040	//p
#define	MASK_SEXM		0x0100	//
#define	MASK_SCM		0x0200	//
#define	MASK_MEM		0x0400	//

#define	CVM_PKPM	0xA0	//AT
#define	CVM_CCM		0xB0	//y
#define	CVM_PCM		0xC0	//@
#define	CVM_CPM		0xD0	//AT
#define	CVM_PBCM	0xE0	//p
#define	SEXM		0xF0	//
#define	SCM			0xF7	//
#define	MEM			0xFF	//



#pragma once

/****************************************************************/
/*																*/
/*			�N���X��`											*/
/*																*/
/****************************************************************/
//MIDI���
class  SMF_MThd{
public:
	//Mthd
	char					strings[4];	//MThd
	unsigned	__int32		size;		//�w�b�_�[�̃T�C�Y
	unsigned	__int16		format;		//Format
	unsigned	__int16		track;		//�g���b�N��
	unsigned	__int16		timebase;	//�^�C���x�[�X

				FileOp*		SMF;

		SMF_MThd(FileOp* ptSMF);
};

// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include "targetver.h"


#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>						//�t�@�C���E����������Ƃ�
#include <search.h>

/****************************************************************/
/*																*/
/*			�v���g�^�C�v�錾									*/
/*																*/
/****************************************************************/
//���O���錾	�A�Z���u������̊֐����ĂԂɂ́A"C"�i�啶���j���Ɩ�������B
//				C++�̂܂܂��ƁA���x�������ɗ]�v�ȕ������t���B
extern "C"	void	print_help();
extern "C"	void	endian_b2l_W(unsigned __int16 *iValue);
extern "C"	void	endian_b2l_D(unsigned __int32 *Value);
extern "C"	int		NoteScaleOut(FILE *stream,int iScale,int iNote);

/****************************************************************/
/*																*/
/*			�\���̒�`											*/
/*																*/
/****************************************************************/

//�t�@�C������p
typedef struct {
				char		name[128];
				FILE *		stream;
				size_t		size;
} FileInfo;

//�I�v�V�����X�C�b�`
typedef struct {
				FileInfo	SMF;
				FileInfo	MML;
				char		fHelp;
} OPSW;

//MIDI�w�b�_�[
typedef struct {
	char					strings[4];	//MThd
	unsigned	__int32		size;		//�w�b�_�[�̃T�C�Y
	unsigned	__int16		format;		//Format
	unsigned	__int16		track;		//�g���b�N��
	unsigned	__int16		timebase;	//�^�C���x�[�X
} SMF_MThd;

//���q
typedef struct {
	unsigned	__int64		iTicksBT;		//���q�L�������鎞��
				char		cKey;			//���i�}0��C�j
	unsigned	char		cmi;			//0:Major / 1:Minor
	unsigned	int			iTicksOne;		//�P���߂̃`�b�N��
} Rhythm;

//�t�R���p�C���p
typedef struct {
				SMF_MThd	SMF_Header;		//MThd�w�b�_�[�����Ă����\����

	//�g���b�N����p
	unsigned	long		lOffsetTR;		//�R���p�C�����̃g���b�N�̐擪�I�t�Z�b�g
	unsigned	__int32		iTrackSize;		//MTrk�̃T�C�Y
				int			iTrack;			//�t�R���p�C�����̃g���b�N
				int			iChannel;		//�t�R���p�C�����̃`�����l��
				char		flagEoT;		//End of Track�̃t���O
				char		flagCh;			//CH���߂��o�͂������H

	//�����֌W��MML���`�p
	unsigned	int			cntBT;			//�J�E���g
	unsigned	int			defBT;			//���q�̒�`��
				Rhythm		BT[255];		//���q��`
	unsigned	__int64		iTotalTicks;	//���܂ł̃`�b�N��
	unsigned	__int64		iTotalTicksChk;	//���܂ł̃`�b�N��
	unsigned	__int32		iTicks;			//���݂̃`�b�N��
	unsigned	int			iTicksBar;		//�o�[���o���܂ł̃J�E���^�[	
	unsigned	int			iTicksCR;		//���s����܂ł̃J�E���^�[	

	//�t�R���p�C�����Ă���C�x���g
	unsigned	char		cStatus;		//�X�e�[�^�X
	unsigned	char		cData1;			//�f�[�^�P
	unsigned	char		cData2;			//�f�[�^�Q
	unsigned	__int32		iSize;			//�T�C�Y

	//�t�R���p�C��
	unsigned	char		cNote;			//NoteOn���̃m�[�g�ԍ��i0xFF���x���j
	unsigned	char		cVelo;			//NoteOn���郔�F���V�e�B
				int			flagNote;		//���݂̏o�͏�	0:�x�� / 1:NoteOn
	unsigned	char		cRunningStatus;	//�O�ɏo�͂����X�e�[�^�X
				int			iGate;			//�ݒ肵���Q�[�g�^�C��
	unsigned	int			iLength;		//�ݒ肵������
				int			iOctave;		//�ݒ肵���I�N�^�[�u
	unsigned	char		cVelocity;		//�ݒ肵�����F���V�e�B�[
} unMMLinfo;

// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B


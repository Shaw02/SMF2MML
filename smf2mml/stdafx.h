// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>						//�t�@�C���E����������Ƃ�



/****************************************************************/
/*																*/
/*			�v���g�^�C�v�錾									*/
/*																*/
/****************************************************************/
//���O���錾	�A�Z���u������̊֐����ĂԂɂ́A"C"�i�啶���j���Ɩ�������B
//				C++�̂܂܂��ƁA���x�������ɗ]�v�ȕ������t���B
extern "C"	void print_help();
extern "C"	void endian_b2l_W(unsigned __int16 *iValue);
extern "C"	void endian_b2l_D(unsigned __int32 *Value);


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

//�t�R���p�C���p
typedef struct {
				SMF_MThd	SMF_Header;		//MThd�w�b�_�[�����Ă����\����
	unsigned	long		lOffsetTR;		//�R���p�C�����̃g���b�N�̐擪�I�t�Z�b�g
	unsigned	__int32		iTrackSize;		//MTrk�̃T�C�Y
				int			iTrack;			//�t�R���p�C�����̃g���b�N
				int			iChannel;		//�t�R���p�C�����̃`�����l��
				char		flagEoT;		//End of Track�̃t���O
				char		flagCh;			//CH���߂��o�͂������H
	unsigned	__int64		iTotalTicks;	//���܂ł̃`�b�N��
	unsigned	__int32		iTicks;			//���݂̃`�b�N��
	unsigned	char		cNote;			//NoteOn���̃m�[�g�ԍ��i0xFF���x���j
				int			flagNote;		//���݂̏o�͏�	0:^ / 1:r & Note
	unsigned	char		cRunningStatus;	//�O�ɏo�͂����X�e�[�^�X
				int			iGate;			//�ݒ肵���Q�[�g�^�C��
	unsigned	int			iLength;		//�ݒ肵������
				int			iOctave;		//�ݒ肵���I�N�^�[�u
} unMMLinfo;

// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B


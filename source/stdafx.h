// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <tchar.h>
#include <search.h>

using namespace std;


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
//extern "C"	int		NoteScaleOut(FILE *stream,int iScale,char cNote);

#include "targetver.h"
#include "FileOp.h"
#include "Option.h"
#include "smfMThd.h"
#include "smfEvent.h"
#include "smfMeta.h"
#include "unMMLcompiler.h"


// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B


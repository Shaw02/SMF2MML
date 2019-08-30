# SMF2MML

## �T�v
���̃A�v���P�[�V�����́AStandard MIDI File�� Sakura�d�l��MML(Music Macro Language)�ɋt�R���p�C�����܂��B

## �ʓr�C���X�g�[�����K�v�ȃ\�t�g
- Microsoft Visual C++ 2008 SP1 �ĔЕz�\�p�b�P�[�W (x86) 


## �g�p���@
�R�}���h���C���ɁA���̒ʂ�ɑł�����ł��������B
���A�i�j���͏ȗ��\�ł��B

SMF2MML �i/option�j �t�@�C�����i.mid�j

�t�@�C����(.mid)�́A�t�R���p�C�����錳�t�@�C�����w�肵�܂��B
�g���q���ȗ������ꍇ�́A�����I�Ɋg���q��".mid"�Ƃ��܂��B
�g���q���Ȃ��t�@�C�����w�肷��ꍇ�́A"�t�@�C�����P."�ƃs���I�h�܂ŋL�q���ĉ������B
���t�@�C���́ASMF��Format 0�Ⴕ����Format 1�ł���K�v������܂��B
�w�肳�ꂽ�t�@�C�����ŁA�g���q��mml�Ƃ���mml�t�@�C�����o�͂��܂��B 
MML�̕��@�́A�ʓr�ASakura�̃h�L�������g���Q�Ƃ��ĉ������B

---------

## Overview
This application converts Sandard MIDI files into MML (Music Macro Language), for use with [Sakura MML](https://sakuramml.com/).

## Dependencies
- [Microsoft Visual C++ 2008 SP1 Redistributable Package (x86)](https://www.microsoft.com/en-us/download/details.aspx?id=5582)

## Usage
Type the following into the command line:

```SMF2MML /option filename.mid```

`filename.mid` specifies the original file to be converted. It must be either SMF Format 0 or Format 1.

If the extension is omitted here, it will automatically be changed to `.mid`.

When specifying a file with no extension, describe up to "file name 1." and a period.

If no output filename is specified, an `.mml` file with the original filename will be generated.

For more information about the MML syntax, see [Sakura MML's documentation](https://sakuramml.com/).

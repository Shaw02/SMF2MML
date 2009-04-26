
#pragma once

/****************************************************************/
/*																*/
/*			�\���̒�`											*/
/*																*/
/****************************************************************/


/****************************************************************/
/*																*/
/*			�N���X��`											*/
/*																*/
/****************************************************************/
//�t�R���p�C���p
class unMMLcompiler {
private:
				OPSW*			OptionSW;	//
				FileOp*			MML;		//
				FileOp*			SMF;		//

public:
				SMF_MetaEvent	SMF_Meta;	//���^�C�x���g�̏��
				SMF_event		SMF_Event;	//SMF�̃C�x���g
				SMF_MThd		SMF_Header;	//MThd�w�b�_�[�����Ă����\����

private:		//�g���b�N����p
				long		lOffsetTR;		//�R���p�C�����̃g���b�N�̐擪�I�t�Z�b�g
	unsigned	__int32		iTrackSize;		//MTrk�̃T�C�Y
				int			iTrack;			//�t�R���p�C�����̃g���b�N
				int			iChannel;		//�t�R���p�C�����̃`�����l��

private:		//�����֌W
	unsigned	__int32		iTicks;			//���݂̃`�b�N��
	unsigned	__int64		iTotalTicks;	//���܂ł̃`�b�N��

public:			//MML���`�p
	unsigned	__int64		iTotalTicksChk;	//���܂ł̃`�b�N��
	unsigned	int			iTicksBar;		//�o�[���o���܂ł̃J�E���^�[	
	unsigned	int			iTicksCR;		//���s����܂ł̃J�E���^�[	
	unsigned	int			nowBeatTime;	//�S������ticks��

private:		//�t�R���p�C��
				char		flagCh;			//CH���߂��o�͂������H
	unsigned	char		cNote;			//NoteOn���̃m�[�g�ԍ��i0xFF���x���j
	unsigned	char		cVelo;			//NoteOn���郔�F���V�e�B
				char		flagNote;		//���݂̏o�͏�	0:�x�� / 1:NoteOn
				char		cOctave;		//�ݒ肵���I�N�^�[�u
	unsigned	char		cVelocity;		//�ݒ肵�����F���V�e�B�[
//				int			iGate;			//�ݒ肵���Q�[�g�^�C��
//	unsigned	int			iLength;		//�ݒ肵������


public:
		unMMLcompiler(OPSW* Option):
			MML(Option->MML),
			SMF(Option->SMF),
			OptionSW(Option),
			SMF_Event(Option),
			SMF_Header(SMF)
		{
		};

	void	uncompile();

private:

	//General
	void	OutputHex();
	void	OutputStrings();
	void	EncodeVariable(unsigned	__int32	var);

	void	initTR();
	void	initCH();

	//Decode: MIDI Header
	void	MetaSearch();
	void	Header();

	//Decode: Length
	void	OutputBarCr();
	int		Ticks2Length(int iTicksL);
	void	Ticks2LengthEx(int iTicksL);
	void	OutputTicks(/*unsigned	__int32		iTicksL*/);

	//Decode: Note
	char	NoteScas(char *cScale, char caNote);
	void	OutputNoteEx();

	//Decode: Note Length
	void	OutputNote();

	//Decode: MIDI Channel Voice Message
	void	DecodeNote();
	void	DecodeControlChange();

	//Decode: MIDI Event
	void	DecodeChannelVoiceMessage();
	void	DecodeSystemExcusiveMessage();
	void	DecodeSystemCommonMessage();
	void	DecodeMetaEvent();

	//main routine
	void	Decode1Command();

};

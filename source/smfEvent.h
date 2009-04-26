
#pragma once

/****************************************************************/
/*																*/
/*			�N���X��`											*/
/*																*/
/****************************************************************/

class SMF_event{
private:
				OPSW*		OptionSW;		//�I�v�V�����X�C�b�`
				FileOp*		SMF;			//SMF�t�@�C��

public:
	//�t�R���p�C�����Ă���C�x���g
	unsigned	char		cRunningStatus;	//�O�ɏo�͂����X�e�[�^�X
	unsigned	char		cStatus;		//�X�e�[�^�X
	unsigned	char		cData1;			//�f�[�^�P
	unsigned	char		cData2;			//�f�[�^�Q
	unsigned	__int32		iSize;			//�T�C�Y

				char		flagEoT;		//End of Track�̃t���O


public:
					SMF_event(OPSW* option):	//������
						OptionSW(option),
						SMF(option->SMF)
					{};

unsigned	__int32	DecodeVariable();		//�ϒ��̓ǂݍ���

unsigned	__int32	EventRead();			//MIDI�C�x���g�̓ǂݍ���

			void	DataRead(char* ptData){	//�T�C�Y���ǂݍ���
						SMF->read(ptData, iSize);
					};

unsigned	char	Read(){					//1Byte�ǂݍ���
						unsigned	char	cData;
						SMF->read((char*)&cData, 1);
						return(cData);
					};

			void	Seek(){					//�T�C�Y���V�[�N
						SMF->StreamPointerAdd(iSize);
					};
};

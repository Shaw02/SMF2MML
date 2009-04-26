
#pragma once

/****************************************************************/
/*																*/
/*			�\���̒�`											*/
/*																*/
/****************************************************************/
//���q
typedef struct{
	unsigned	__int64		iTicksBT;		//���q�L�������鎞��
	unsigned	char		cNumerator;		//���q
	unsigned	char		cDenominator;	//����
				char		cKey;			//���i�}0��C�j
	unsigned	char		cmi;			//0:Major / 1:Minor
	unsigned	int			iTicksOne;		//�P���߂̃`�b�N��
} Rhythm;

/****************************************************************/
/*																*/
/*			�N���X��`											*/
/*																*/
/****************************************************************/
class	SMF_MetaEvent {
private:
	unsigned	int			cntBT;			//�J�E���g
	unsigned	int			defBT;			//���q�̒�`��
				Rhythm*		BT;				//���q��`

public:
				SMF_MetaEvent():			//������
				cntBT(0),defBT(0)
				{	BT	= new Rhythm[255];	}

				~SMF_MetaEvent()			//�\���̂̔j��
				{	delete	BT;	}

private:

static		int		Compare(const void *setBT,const void *cmpBT);	//��r
			void	DefineInc(){defBT++;};							//
			Rhythm* find(unsigned __int64 iTotalTicks);				//����
public:
			void	sort(){qsort((void *)BT,defBT,sizeof(Rhythm),Compare);};

			void	addRhythm(	const unsigned	__int64 iTotalTicks,
								const unsigned	char cn, 
								const unsigned	char cd,
								const unsigned	__int16 iTimeBase	);

			void	addKey(		const unsigned	__int64 iTotalTicks,
								const			char cK,
								const unsigned	char cM);

			void	CounterReset(){cntBT=0;};							//�J�E���^�[���Z�b�g
			void	CounterInc(){cntBT++;};								//�J�E���^�[�{�{
			int		CounterCheck(){return(defBT<=cntBT);};				//�J�E���^�[�`�F�b�N

unsigned	__int64	nextBeatTime(){return(BT[cntBT+1].iTicksBT);};		//��TimePointer
unsigned	__int64	BeatTime(){return(BT[cntBT].iTicksBT);};			//��TimePointer
unsigned	int		nowTimeTicks(){return(BT[cntBT].iTicksOne);};		//�S������[Ticks]
			char	nowKey(){return(BT[cntBT].cKey);};					//���i�}0��C�j
unsigned	char	nowMinor(){return(BT[cntBT].cmi);};					//0:Major / 1:Minor
unsigned	char	nowNumerator(){return(BT[cntBT].cNumerator);};		//���q���q
unsigned	char	nowDenominator(){return(BT[cntBT].cDenominator);};	//���q����
};

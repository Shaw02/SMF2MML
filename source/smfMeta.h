
#pragma once

/****************************************************************/
/*																*/
/*			構造体定義											*/
/*																*/
/****************************************************************/
//拍子
typedef struct{
	unsigned	__int64		iTicksBT;		//拍子記号がある時間
	unsigned	char		cNumerator;		//分子
	unsigned	char		cDenominator;	//分母
				char		cKey;			//調（±0＝C）
	unsigned	char		cmi;			//0:Major / 1:Minor
	unsigned	int			iTicksOne;		//１小節のチック数
} Rhythm;

/****************************************************************/
/*																*/
/*			クラス定義											*/
/*																*/
/****************************************************************/
class	SMF_MetaEvent {
private:
	unsigned	int			cntBT;			//カウント
	unsigned	int			defBT;			//拍子の定義数
				Rhythm*		BT;				//拍子定義

public:
				SMF_MetaEvent():			//初期化
				cntBT(0),defBT(0)
				{	BT	= new Rhythm[255];	}

				~SMF_MetaEvent()			//構造体の破棄
				{	delete	BT;	}

private:

static		int		Compare(const void *setBT,const void *cmpBT);	//比較
			void	DefineInc(){defBT++;};							//
			Rhythm* find(unsigned __int64 iTotalTicks);				//検索
public:
			void	sort(){qsort((void *)BT,defBT,sizeof(Rhythm),Compare);};

			void	addRhythm(	const unsigned	__int64 iTotalTicks,
								const unsigned	char cn, 
								const unsigned	char cd,
								const unsigned	__int16 iTimeBase	);

			void	addKey(		const unsigned	__int64 iTotalTicks,
								const			char cK,
								const unsigned	char cM);

			void	CounterReset(){cntBT=0;};							//カウンターリセット
			void	CounterInc(){cntBT++;};								//カウンター＋＋
			int		CounterCheck(){return(defBT<=cntBT);};				//カウンターチェック

unsigned	__int64	nextBeatTime(){return(BT[cntBT+1].iTicksBT);};		//次TimePointer
unsigned	__int64	BeatTime(){return(BT[cntBT].iTicksBT);};			//現TimePointer
unsigned	int		nowTimeTicks(){return(BT[cntBT].iTicksOne);};		//全音符長[Ticks]
			char	nowKey(){return(BT[cntBT].cKey);};					//調（±0＝C）
unsigned	char	nowMinor(){return(BT[cntBT].cmi);};					//0:Major / 1:Minor
unsigned	char	nowNumerator(){return(BT[cntBT].cNumerator);};		//拍子分子
unsigned	char	nowDenominator(){return(BT[cntBT].cDenominator);};	//拍子分母
};

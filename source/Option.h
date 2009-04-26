
#pragma once

/****************************************************************/
/*																*/
/*			クラス定義											*/
/*																*/
/****************************************************************/
class OPSW {
public:
				FileOp*		SMF;			//指定したSMFファイル
				FileOp*		MML;			//指定したMMLファイル
				char		fHelp;			//ヘルプを指定したか？
	unsigned	int			iBar;			// | の挿入
	unsigned	int			iCR;			//改行を挿入する小節数
	unsigned	int			iMaskFlag;		//出力をマスクする機能
	unsigned	char		cJpn;			//すととん？

		OPSW();								//初期化のみ
		OPSW(int argc, _TCHAR* argv[]);		//引数内容から、クラスを初期化＆ファイルオープン
		~OPSW();							//ファイルクローズ
void	opError(const char *stErrMsg);		//オプションエラー
};



//==============================================================
//機能マスク用
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


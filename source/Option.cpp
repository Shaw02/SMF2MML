
#include "stdafx.h"

//==============================================================
//		オプション処理
//--------------------------------------------------------------
//	●引数
//			int argc		オプション文字列の数
//			_TCHAR* argv[]	オプション文字列
//	●返値
//			SMF.name[]		変換元のSMFファイル
//			MML.name[]		変換先のMMLファイル
//	●備考
//			オプションにファイル名が指定されない場合は、ヘルプ表示して終了
//==============================================================
OPSW::OPSW(int argc, _TCHAR* argv[]):
	//初期化設定
	fHelp(0),		//ヘルプは、デフォルトは表示しない。
	iBar(1),		// | 出力
	iCR(4),			//改行の小節数
	iMaskFlag(0),	//マスクは、デフォルト無し
	cJpn(0),		//MML
	iAuftakt(0)		//アウフタクト
	{

	//----------------------------------
	//■Local 変数
	int		iCount;				//whileのカウント用
	int		iResult;			//汎用

	//Option処理用
	int		iOptionChk;			//オプションチェック用　ポインタ
	char	cOption;			//オプションチェック用　文字
	char	iFlagFilnameExt;	//拡張子あったかのフラグ

	MML = new FileOp;			//オブジェクトの作成
	SMF = new FileOp;			//オブジェクトの作成

	//----------------------------------
	//■オプション処理
	iCount=1;	//コマンド名は飛ばす
	while(iCount!=argc)
	{
		//--------------
		//オプションスイッチにスラッシュがあるか確認
		if((argv[iCount][0]=='/')||(argv[iCount][0]=='-')){

			//--------------
			//◆Option Switch	（スラッシュがあった場合の処理）
			switch(argv[iCount][1]){
				//--------
				//Help表示
				case 'h' :
				case 'H' :
				case '?' :
					fHelp=1;
					break;
				//--------
				//改行
				case 'A' :
					iResult=sscanf(argv[iCount],"/A%d",&iAuftakt);
					if((iResult==NULL)||(iResult==EOF)){
						opError("/A");
						break;
					};
					break;
				//--------
				//小節スイッチ
				case 'B' :
					switch(argv[iCount][2]){
						case '-' :
							iBar=0;
							break;
						case '+' :
							iBar=1;
							break;
						default :
							opError("/B");
							break;
					};
					break;
				//--------
				//改行
				case 'C' :
					iResult=sscanf(argv[iCount],"/C%d",&iCR);
					if((iResult==NULL)||(iResult==EOF)){
						opError("/C");
						break;
					};
					break;
				//--------
				//マスクスイッチ
				case 'M' :
					switch(argv[iCount][2]){
						case 'A' :
							iMaskFlag = 0xFFFF;
							break;
						case 'a' :
							iMaskFlag |= (MASK_CVM_PKPM | MASK_CVM_CPM);
							break;
						case 'p' :
							iMaskFlag |= MASK_CVM_PCM;
							break;
						case 'c' :
							iMaskFlag |= MASK_CVM_CCM;
							break;
						case 'b' :
							iMaskFlag |= MASK_CVM_PBCM;
							break;
						case 'x' :
							iMaskFlag |= (MASK_SEXM | MASK_SCM);
							break;
						case 'm' :
							iMaskFlag |= MASK_MEM;
							break;
						default :
							opError("/M");
							break;
					};
					break;
				//--------
				//ファイルの指定
				case 'F' :
					//先に、ファイル名が書いてあるかチェック。
					if(argv[iCount][3]==0){
						opError("/F ファイル名が書いてありません。");
						break;
					};
					switch(argv[iCount][2]){
					//--------
					//MMLファイルの指定
					case 'm' :
						//既に指定されている？
						if(MML->name.empty()){
							iFlagFilnameExt=0;		//拡張子の有無　Reset
							iOptionChk=0;
							while((cOption=argv[iCount][iOptionChk+3])!=0)
							{
								MML->name+=cOption;
								if(cOption=='.'){iFlagFilnameExt=1;};
								iOptionChk++;
							};
							if(iFlagFilnameExt==0){
								MML->name+=".mml";
							};
						} else {
							opError("/F MMLファイルが2回以上指定されました。");
							break;
						};
						break;
					default :
						opError("/F");
						break;
					};
				break;
				//

				//--------
				//ストトンの指定
				case 'J' :
					cJpn=1;
					break;

				//--------
				//デフォルト
				default :
					opError("");
					break;
			};

		} else{

			//--------------
			//◆ファイル名	（スラッシュが無かった場合の処理）
			//既に指定されている？
			if(SMF->name.empty()){
				iFlagFilnameExt=0;		//拡張子の有無　Reset
				iOptionChk=0;		
				while((cOption=argv[iCount][iOptionChk])!=0)
				{
					SMF->name+=cOption;
					if(cOption=='.'){iFlagFilnameExt=1;};
					iOptionChk++;
				};
				if(iFlagFilnameExt==0){
					SMF->name+=".mid";
				};
			} else {
				opError("SMFファイルが2回以上指定されました。");
				break;
			};

		};

		//--------------
		//◆次のオプション
		iCount++;
	};

	//----------------------------------
	//◆オプションで指定された事を処理する。

	//--------------
	//ヘルプ表示
	//ファイル名が書かれなかった場合も、ヘルプを表示する。
	if((fHelp==1)||(SMF->name.empty())){print_help();};

	//--------------
	//MMFファイルの指定が無かった場合
	if(MML->name.empty()){
		iOptionChk=0;		
		while((cOption=SMF->name[iOptionChk])!='.')
		{
			MML->name+=cOption;
			iOptionChk++;
		};
		MML->name+=".mml";
	};

	//--------------
	//

	//	to do	その他のオプションを追加したときは、この辺に追記する。

	//----------
	//Debug用 表示
	cout << "SMF = " << SMF->name << endl;
	cout << "MML = " << MML->name << endl;

	//----------

	//SMFファイルの Open (read only)
	SMF->fileopen(ios_base::in | ios_base::binary);

	//MMLファイルの Open (write)
	MML->fileopen(ios_base::out);

};
//==============================================================
//		デストラクト
//--------------------------------------------------------------
//	●引数
//			なし
//	●返値
//			無し
//==============================================================
OPSW::~OPSW(){

	delete	MML;	//MMLファイルの Close
	delete	SMF;	//SMFファイルの Close

};
//==============================================================
//		エラー処理	（プロセスも終了する）
//--------------------------------------------------------------
//	●引数
//			char *stErrMsg	エラーメッセージ
//	●返値
//			無し
//==============================================================
void OPSW::opError(const char *stErrMsg){

	cerr << "オプションが不正です。：" << stErrMsg << endl;
	exit(EXIT_FAILURE);

};

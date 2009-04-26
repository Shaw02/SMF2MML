
#pragma once

/****************************************************************/
/*																*/
/*			クラス定義											*/
/*																*/
/****************************************************************/
class FileOp : public fstream{
public:
		string		name;	//ファイル名

		//ファイルを開く　エラー処理付き
void	fileopen(std::ios_base::open_mode mode){
			open(name.c_str(),mode);
			if(good()==false){
				perror(name.c_str());
				exit(EXIT_FAILURE);
			};
		};

		//相対シーク
void	StreamPointerAdd(__int32 iSize){
			if(iSize>0){
				seekg((long)iSize,ios::cur);
			};
		};

void	StreamPointerMove(__int32 iSize){
			if(iSize>0){
				seekg((long)iSize,ios::beg);
			};
		};

};

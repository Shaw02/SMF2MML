
#pragma once

/****************************************************************/
/*																*/
/*			�N���X��`											*/
/*																*/
/****************************************************************/
class FileOp : public fstream{
public:
		string		name;	//�t�@�C����

		//�t�@�C�����J���@�G���[�����t��
void	fileopen(std::ios_base::open_mode mode){
			open(name.c_str(),mode);
			if(good()==false){
				perror(name.c_str());
				exit(EXIT_FAILURE);
			};
		};

		//���΃V�[�N
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

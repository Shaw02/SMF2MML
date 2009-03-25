/****************************************************************/
/*																*/
/*				SMF tp MML un-compiler							*/
/*																*/
/*								Programmed by.					*/
/*									S.W.	(A.Watanabe)		*/
/*																*/
/****************************************************************/

// smf2mml.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

/****************************************************************/
/*																*/
/*			�v���g�^�C�v�錾									*/
/*																*/
/****************************************************************/
extern		void	uncompile(OPSW *OptionSW);
extern		size_t	getStreamSize(FILE *fStream);

//���v���g�^�C�v�錾



/****************************************************************/
/*																*/
/*					�G���[����									*/
/*																*/
/****************************************************************/
//==============================================================
//		�G���[����	�i�v���Z�X���I������j
//--------------------------------------------------------------
//	������
//			char *stErrMsg1	�G���[���b�Z�[�W(�O)
//			char *stErrMsg2	�G���[���b�Z�[�W(���s)
//	���Ԓl
//			����
//==============================================================
void error(char *stErrMsg1, char *stErrMsg2){

	perror(stErrMsg1);
	if(stErrMsg2[0]!=0){printf(stErrMsg2);};
	exit(EXIT_FAILURE);

};
//==============================================================
//		�G���[����	�i�v���Z�X���I������j
//--------------------------------------------------------------
//	������
//			char *stErrMsg	�G���[���b�Z�[�W
//	���Ԓl
//			����
//==============================================================
void opError(char *stErrMsg){

	fprintf(stderr,"�I�v�V�������s���ł��B�F");
	fprintf(stderr,stErrMsg);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);

};
/****************************************************************/
/*																*/
/*					���C���E�T�u���[�`��						*/
/*																*/
/****************************************************************/
//==============================================================
//		�I�v�V��������
//--------------------------------------------------------------
//	������
//			int argc		�I�v�V����������̐�
//			_TCHAR* argv[]	�I�v�V����������
//			OPSW *OptionSW	�I�v�V�����X�C�b�`�\���̂̃|�C���^
//	���Ԓl
//			OptionSW->SMF.name[]		�ϊ�����SMF�t�@�C��
//			OptionSW->MML.name[]		�ϊ����MML�t�@�C��
//	�����l
//			�I�v�V�����Ƀt�@�C�������w�肳��Ȃ��ꍇ�́A�w���v�\�����ďI��
//==============================================================
void option(int argc, _TCHAR* argv[],OPSW *OptionSW){

	//----------------------------------
	//��Local �ϐ�
	int		iCount;				//while�̃J�E���g�p
	int		iResult;			//�ėp

	//Option�����p
	int		iOptionChk;			//�I�v�V�����`�F�b�N�p�@�|�C���^
	char	cOption;			//�I�v�V�����`�F�b�N�p�@����
	char	iFlagFilnameExt;	//�g���q���������̃t���O



	//----------------------------------
	//�������ݒ�
	OptionSW->SMF.name[0]=0;	//�f�t�H���g�̓t�@�C��������
	OptionSW->MML.name[0]=0;	//�f�t�H���g�̓t�@�C��������
	OptionSW->fHelp=0;			//�w���v�́A�f�t�H���g�͕\�����Ȃ��B
	OptionSW->iBar=1;			// | �o��
	OptionSW->iCR=4;			//���s�̏��ߐ�

	//----------------------------------
	//���I�v�V��������
	iCount=1;	//�R�}���h���͔�΂�
	while(iCount!=argc)
	{
		//--------------
		//�I�v�V�����X�C�b�`�ɃX���b�V�������邩�m�F
		if((argv[iCount][0]=='/')||(argv[iCount][0]=='-')){

			//--------------
			//��Option Switch	�i�X���b�V�����������ꍇ�̏����j
			switch(argv[iCount][1]){
				//--------
				//Help�\��
				case 'h' :
				case 'H' :
				case '?' :
					OptionSW->fHelp=1;
					break;
				//--------
				//���߃X�C�b�`
				case 'B' :
					switch(argv[iCount][2]){
						case '-' :
							OptionSW->iBar=0;
							break;
						case '+' :
							OptionSW->iBar=1;
							break;
						default :
							opError("/B");
							break;
					};
					break;
				//--------
				//���s
				case 'C' :
					iResult=sscanf(argv[iCount],"/C%d",&OptionSW->iCR);
					if((iResult==NULL)||(iResult==EOF)){
						opError("/C");
						break;
					};
					break;
				//--------
				//�t�@�C���̎w��
				case 'F' :
					//��ɁA�t�@�C�����������Ă��邩�`�F�b�N�B
					if(argv[iCount][3]==0){
						opError("/F �t�@�C�����������Ă���܂���B");
						break;
					};
					switch(argv[iCount][2]){
					//--------
					//MML�t�@�C���̎w��
					case 'm' :
						//���Ɏw�肳��Ă���H
						if(OptionSW->MML.name[0]==0){
							iFlagFilnameExt=0;		//�g���q�̗L���@Reset
							iOptionChk=0;		
							while((cOption=argv[iCount][iOptionChk+3])!=0)
							{
								OptionSW->MML.name[iOptionChk]=cOption;
								if(cOption=='.'){iFlagFilnameExt=1;};
								iOptionChk++;
							};
							if(iFlagFilnameExt==0){
								OptionSW->MML.name[iOptionChk++]='.';	//�g���q��".mid"�ɂ���B
								OptionSW->MML.name[iOptionChk++]='m';	
								OptionSW->MML.name[iOptionChk++]='m';	
								OptionSW->MML.name[iOptionChk++]='l';	
							};
							OptionSW->MML.name[iOptionChk++]=0;	
						} else {
							opError("/F MML�t�@�C����2��ȏ�w�肳��܂����B");
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
				//�f�t�H���g
				default :
					opError("");
					break;
			};

		} else{

			//--------------
			//���t�@�C����	�i�X���b�V�������������ꍇ�̏����j
			//���Ɏw�肳��Ă���H
			if(OptionSW->SMF.name[0]==0){
				iFlagFilnameExt=0;		//�g���q�̗L���@Reset
				iOptionChk=0;		
				while((cOption=argv[iCount][iOptionChk])!=0)
				{
					OptionSW->SMF.name[iOptionChk]=cOption;
					if(cOption=='.'){iFlagFilnameExt=1;};
					iOptionChk++;
				};
				if(iFlagFilnameExt==0){
					OptionSW->SMF.name[iOptionChk++]='.';	//�g���q��".mid"�ɂ���B
					OptionSW->SMF.name[iOptionChk++]='m';	
					OptionSW->SMF.name[iOptionChk++]='i';	
					OptionSW->SMF.name[iOptionChk++]='d';	
				};
				OptionSW->SMF.name[iOptionChk++]=0;	
			} else {
				opError("SMF�t�@�C����2��ȏ�w�肳��܂����B");
				break;
			};

		};

		//--------------
		//�����̃I�v�V����
		iCount++;
	};

	//----------------------------------
	//���I�v�V�����Ŏw�肳�ꂽ������������B

	//--------------
	//�w���v�\��
	//�t�@�C������������Ȃ������ꍇ���A�w���v��\������B
	if((OptionSW->fHelp==1)||(OptionSW->SMF.name[0]==0)){print_help();};

	//--------------
	//SMF�t�@�C���̎w�肪���������ꍇ
	if(OptionSW->MML.name[0]==0){
		iOptionChk=0;		
		while((cOption=OptionSW->SMF.name[iOptionChk])!='.')
		{
			OptionSW->MML.name[iOptionChk]=cOption;
			iOptionChk++;
		};
		OptionSW->MML.name[iOptionChk++]='.';	//�g���q��".mml"�ɂ���	
		OptionSW->MML.name[iOptionChk++]='m';	
		OptionSW->MML.name[iOptionChk++]='m';	
		OptionSW->MML.name[iOptionChk++]='l';	
		OptionSW->MML.name[iOptionChk++]=0;	
	};

	//--------------
	//

	//	to do	���̑��̃I�v�V������ǉ������Ƃ��́A���̕ӂɒǋL����B


};
//==============================================================
//		���C�����[�`��
//--------------------------------------------------------------
//	������
//			int argc		�I�v�V����������̐�
//			_TCHAR* argv[]	�I�v�V����������
//	���Ԓl
//			�v���Z�X�ɕԂ��l
//==============================================================
int _tmain(int argc, _TCHAR* argv[])
{

	//----------------------------------
	//��Local �ϐ�
//	FileInfo	SMF;			//SMF�t�@�C���̏��
//	FileInfo	MML;			//MML�t�@�C���̏��
	OPSW		OptionSW;

	//�I�v�V��������
	option(argc,argv,&OptionSW);

	//----------
	//Debug�p �\��
	printf("SMF = ");
	printf(OptionSW.SMF.name);
	printf("\n");
	printf("MML = ");
	printf(OptionSW.MML.name);
	printf("\n");
	//----------



	//SMF�t�@�C���� Open (read only)
	if((OptionSW.SMF.stream=fopen(OptionSW.SMF.name,"rb"))==NULL){error(OptionSW.SMF.name,"");};
	OptionSW.SMF.size=getStreamSize(OptionSW.SMF.stream);	//�t�@�C���T�C�Y�̎擾
	//----------

	//MML�t�@�C���� Open (read only)
	if((OptionSW.MML.stream=fopen(OptionSW.MML.name,"w"))==NULL){error(OptionSW.MML.name,"");};

	//�t�R���p�C��
	uncompile(&OptionSW);

	//MML�t�@�C���� Close
	if(fclose(OptionSW.MML.stream)!=NULL){error("","");};

	//SMF�t�@�C���� Close
	if(fclose(OptionSW.SMF.stream)!=NULL){error("","");};

};

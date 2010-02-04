
#include "stdafx.h"

//==============================================================
//		�I�v�V��������
//--------------------------------------------------------------
//	������
//			int argc		�I�v�V����������̐�
//			_TCHAR* argv[]	�I�v�V����������
//	���Ԓl
//			SMF.name[]		�ϊ�����SMF�t�@�C��
//			MML.name[]		�ϊ����MML�t�@�C��
//	�����l
//			�I�v�V�����Ƀt�@�C�������w�肳��Ȃ��ꍇ�́A�w���v�\�����ďI��
//==============================================================
OPSW::OPSW(int argc, _TCHAR* argv[]):
	//�������ݒ�
	fHelp(0),		//�w���v�́A�f�t�H���g�͕\�����Ȃ��B
	iBar(1),		// | �o��
	iCR(4),			//���s�̏��ߐ�
	iMaskFlag(0),	//�}�X�N�́A�f�t�H���g����
	cJpn(0),		//MML
	iAuftakt(0)		//�A�E�t�^�N�g
	{

	//----------------------------------
	//��Local �ϐ�
	int		iCount;				//while�̃J�E���g�p
	int		iResult;			//�ėp

	//Option�����p
	int		iOptionChk;			//�I�v�V�����`�F�b�N�p�@�|�C���^
	char	cOption;			//�I�v�V�����`�F�b�N�p�@����
	char	iFlagFilnameExt;	//�g���q���������̃t���O

	MML = new FileOp;			//�I�u�W�F�N�g�̍쐬
	SMF = new FileOp;			//�I�u�W�F�N�g�̍쐬

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
					fHelp=1;
					break;
				//--------
				//���s
				case 'A' :
					iResult=sscanf(argv[iCount],"/A%d",&iAuftakt);
					if((iResult==NULL)||(iResult==EOF)){
						opError("/A");
						break;
					};
					break;
				//--------
				//���߃X�C�b�`
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
				//���s
				case 'C' :
					iResult=sscanf(argv[iCount],"/C%d",&iCR);
					if((iResult==NULL)||(iResult==EOF)){
						opError("/C");
						break;
					};
					break;
				//--------
				//�}�X�N�X�C�b�`
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
						if(MML->name.empty()){
							iFlagFilnameExt=0;		//�g���q�̗L���@Reset
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
				//�X�g�g���̎w��
				case 'J' :
					cJpn=1;
					break;

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
			if(SMF->name.empty()){
				iFlagFilnameExt=0;		//�g���q�̗L���@Reset
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
	if((fHelp==1)||(SMF->name.empty())){print_help();};

	//--------------
	//MMF�t�@�C���̎w�肪���������ꍇ
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

	//	to do	���̑��̃I�v�V������ǉ������Ƃ��́A���̕ӂɒǋL����B

	//----------
	//Debug�p �\��
	cout << "SMF = " << SMF->name << endl;
	cout << "MML = " << MML->name << endl;

	//----------

	//SMF�t�@�C���� Open (read only)
	SMF->fileopen(ios_base::in | ios_base::binary);

	//MML�t�@�C���� Open (write)
	MML->fileopen(ios_base::out);

};
//==============================================================
//		�f�X�g���N�g
//--------------------------------------------------------------
//	������
//			�Ȃ�
//	���Ԓl
//			����
//==============================================================
OPSW::~OPSW(){

	delete	MML;	//MML�t�@�C���� Close
	delete	SMF;	//SMF�t�@�C���� Close

};
//==============================================================
//		�G���[����	�i�v���Z�X���I������j
//--------------------------------------------------------------
//	������
//			char *stErrMsg	�G���[���b�Z�[�W
//	���Ԓl
//			����
//==============================================================
void OPSW::opError(const char *stErrMsg){

	cerr << "�I�v�V�������s���ł��B�F" << stErrMsg << endl;
	exit(EXIT_FAILURE);

};

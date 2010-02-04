;===============================================================|
;								|
;		SMF 2 MML	un-compiler			|
;								|
;				Programmed by			|
;					(S.W.)	( A.Watanabe )	|
;								|
;===============================================================|

.386
.model	flat,c

;****************************************************************
;*		�v���g�^�C�v�錾				*
;****************************************************************
;���O���錾
printf		proto	stPrint:near32 ptr
fprintf		proto	ptStream:near32 ptr,stPrint:near32 ptr
exit		proto	iCode:dword
fread		proto	

;���v���g�^�C�v�錾
print_help	proto
endian_b2l_W	proto	iValue:near32 ptr
endian_b2l_D	proto	iValue:near32 ptr
;NoteScaleOut	proto	Stream:near32 ptr byte,
;			iScale:DWORD,
;			iNote:BYTE

;===============================================================|
;		Big Endian -> Little Endian			|
;---------------------------------------------------------------|
;	������							|
;		iValue:near32 ptr	�ϐ��̃|�C���^		|
;	������							|
;		����						|
;===============================================================|
.code
endian_b2l_W	proc	uses eax esi,
	iValue:near32 ptr

	mov	esi,iValue		;esi��WORD�^�ϐ��̃|�C���^
	mov	ax,[esi]
	xchg	al,ah			;���g���G���f�B�A����
	mov	[esi],ax
	ret
endian_b2l_W	endp
;===============================================================|
;		Big Endian -> Little Endian			|
;---------------------------------------------------------------|
;	������							|
;		iValue:near32 ptr	�ϐ��̃|�C���^		|
;	������							|
;		����						|
;===============================================================|
endian_b2l_D	proc	uses eax edx esi,
	iValue:near32 ptr

	mov	esi,iValue		;esi��WORD�^�ϐ��̃|�C���^
	mov	eax,[esi]
	mov	edx,eax
	and	eax,0000ffffh
	shr	edx,16
	xchg	ah,al
	xchg	dh,dl
	shl	eax,16
	or	eax,edx	
	mov	[esi],eax
	ret
endian_b2l_D	endp
;===============================================================|
;		�m�[�g�̕\��					|
;---------------------------------------------------------------|
;	������							|
;		FILE	*Stream	�o�͐�				|
;		int	iScale	�X�P�[���̔ԍ�	(0 =in C)	|
;		int	iNote	�m�[�g(0:C�`11:b)		|
;	������							|
;		eax	fprintf�֐��̖߂�l			|
;===============================================================|
;.const
;NoteScaleOut_Mes	db	"c*",0,0,	"d",0,0,0,	"d*",0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g",0,0,0,	"g*",0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"`c",0,0	;in Gs	-6	0
;			db	"c",0,0,0,	"d",0,0,0,	"d*",0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g",0,0,0,	"g*",0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"b*",0,0	;in Ds	-5	1
;			db	"c",0,0,0,	"d",0,0,0,	"d*",0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"b*",0,0	;in As	-4	2
;			db	"c",0,0,0,	"d-",0,0,	"d",0,0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"b*",0,0	;in Es	-3	3
;			db	"c",0,0,0,	"d-",0,0,	"d",0,0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a-",0,0,	"a",0,0,0,	"b",0,0,0,	"b*",0,0	;in B	-2	4
;			db	"c",0,0,0,	"d-",0,0,	"d",0,0,0,	"e-",0,0,	"e",0,0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a-",0,0,	"a",0,0,0,	"b",0,0,0,	"b*",0,0	;in F	-1	5
;			db	"c",0,0,0,	"c+",0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f",0,0,0,	"f+",0,0,	"g",0,0,0,	"g+",0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in C	 0	6
;			db	"c",0,0,0,	"c+",0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g",0,0,0,	"g+",0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in G	+1	7
;			db	"c*",0,0,	"c",0,0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g",0,0,0,	"g+",0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in D	+2	8
;			db	"c*",0,0,	"c",0,0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in A	+3	9
;			db	"c*",0,0,	"c",0,0,0,	"d*",0,0,	"d",0,0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in E	+4	10
;			db	"c*",0,0,	"c",0,0,0,	"d*",0,0,	"d",0,0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a*",0,0,	"a",0,0,0,	"b",0,0,0	;in H	+5	11
;			db	"c*",0,0,	"c",0,0,0,	"d*",0,0,	"d",0,0,0,	"e*",0,0,	"e",0,0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a*",0,0,	"a",0,0,0,	"b",0,0,0	;in Fis	+6	12
;
;.code
;NoteScaleOut	proc	uses ebx,
;		Stream:near32 ptr byte,
;		iScale:DWORD,
;		cNote:BYTE
;
;		mov	ebx,iScale
;		movzx	eax,cNote
;
;		lea	ebx,[ebx*4 + 6*4]	; ebx = ( iScale + 6 ) *4
;
;		.if	(ebx<13*4)
;			lea	ebx,[ebx*2 + ebx]
;			lea	ebx,[ebx*4]	; ebx = ( iScale + 6 ) *48
;		.else
;			mov	ebx,48 * 6
;		.endif
;		lea	eax,[ebx + eax*4 + offset NoteScaleOut_Mes]
;
;		invoke	fprintf,	Stream,eax
;
;		ret
;NoteScaleOut	endp
;===============================================================|
;		�\��						|
;---------------------------------------------------------------|
;	������							|
;		stPrint	�o�͕�����				|
;	������							|
;		eax	printf�֐��̖߂�l			|
;===============================================================|
.const
print_help_mes	db	"Sakura Music Macro Language un-compiler for Standard MIDI File",0dh,0ah
		db	"Copyright (C) S.W. (A.Watanabe) 2009-2010",0dh,0ah
		db	0ah,0dh
		db	"smf2mml [ /options ] [file(.mid)]",0dh,0ah
		db	0ah,0dh
		db	"  /B[+/-]		���ߐ��Ɖ��s�̏o�͗L�����w�肷��B(Defailt=+)",0dh,0ah
		db	"			 +:Enable",0dh,0ah
		db	"			 -:Disable",0dh,0ah
		db	"  /C<number>		���s�̐ݒ���w�肷��B(Default=4)",0dh,0ah
		db	"			 number>= 1 : ���s���鏬�ߐ����w�肷��B",0dh,0ah
		db	"			 number = 0 : �������ɉ��s����B",0dh,0ah
		db	"			 number =-1 : ���s���Ȃ��B",0dh,0ah
		db	"  /J			���ƂƂ�ŏo�͂���B",0dh,0ah
		db	"			�@�����ƂƂ�ŋL�q�s�\�Ȗ��߂́AMML�ɂȂ�܂��B",0dh,0ah
		db	"  /MA			Note on/off�ȊO�A�S�ďo�͂��Ȃ��B",0dh,0ah
		db	"  /Ma			After Touch message���o�͂��Ȃ��B",0dh,0ah
		db	"  /Mc			Control Change message���o�͂��Ȃ��B",0dh,0ah
		db	"  /Mp			Program Change message���o�͂��Ȃ��B",0dh,0ah
		db	"  /Mb			Pitch Bend Change message���o�͂��Ȃ��B",0dh,0ah
		db	"  /Mx			System Exclusive message���o�͂��Ȃ��B",0dh,0ah
		db	"  /Mm			Meta Event message���o�͂��Ȃ��B",0dh,0ah
		db	"  /Fm[file(.mml)]	.mml�t�@�C�������w�肷��B",0dh,0ah
		db	"  /H			�w���v��\������B",0dh,0ah
		db	0

.code
print_help	proc	uses	eax
	invoke	printf,
		offset print_help_mes
	invoke	exit,
		0
	ret
print_help	endp
;****************************************************************
	end

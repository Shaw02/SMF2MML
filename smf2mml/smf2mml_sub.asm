;===============================================================|
;								|
;		
;								|
;					Programmed by		|
;						------		|
;								|
;===============================================================|

	.586
	.model	flat,c

;****************************************************************
;*		�v���g�^�C�v�錾				*
;****************************************************************
;���O���錾
printf		proto	stPrint:near32 ptr
exit		proto	iCode:dword
fread		proto	

;���v���g�^�C�v�錾
print_help	proto
endian_b2l_W	proto	iValue:near32 ptr
endian_b2l_D	proto	iValue:near32 ptr

.code

;===============================================================|
;		Big Endian -> Little Endian			|
;---------------------------------------------------------------|
;	������							|
;		iValue:near32 ptr	�ϐ��̃|�C���^		|
;	������							|
;		����						|
;===============================================================|
endian_b2l_W	proc	uses eax esi,		iValue:near32 ptr
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
endian_b2l_D	proc	uses eax edx esi,	iValue:near32 ptr
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
;		�\��						|
;---------------------------------------------------------------|
;	������							|
;		stPrint	�o�͕�����				|
;	������							|
;		eax	printf�֐��̖߂�l			|
;===============================================================|
	.const
print_help_mes	db	"smf2mml [�t�@�C����(.mid)] [/H]",0ah,0dh
		db	0ah,0dh
		db	"  /H		�w���v��\�����܂��B",0ah,0dh
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

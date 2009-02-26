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
;*		プロトタイプ宣言				*
;****************************************************************
;●外部宣言
printf		proto	stPrint:near32 ptr
exit		proto	iCode:dword
fread		proto	

;●プロトタイプ宣言
print_help	proto
endian_b2l_W	proto	iValue:near32 ptr
endian_b2l_D	proto	iValue:near32 ptr

.code

;===============================================================|
;		Big Endian -> Little Endian			|
;---------------------------------------------------------------|
;	●引数							|
;		iValue:near32 ptr	変数のポインタ		|
;	●引数							|
;		無し						|
;===============================================================|
endian_b2l_W	proc	uses eax esi,		iValue:near32 ptr
	mov	esi,iValue		;esi←WORD型変数のポインタ
	mov	ax,[esi]
	xchg	al,ah			;リトルエンディアンへ
	mov	[esi],ax
	ret
endian_b2l_W	endp
;===============================================================|
;		Big Endian -> Little Endian			|
;---------------------------------------------------------------|
;	●引数							|
;		iValue:near32 ptr	変数のポインタ		|
;	●引数							|
;		無し						|
;===============================================================|
endian_b2l_D	proc	uses eax edx esi,	iValue:near32 ptr
	mov	esi,iValue		;esi←WORD型変数のポインタ
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
;		表示						|
;---------------------------------------------------------------|
;	●引数							|
;		stPrint	出力文字列				|
;	●引数							|
;		eax	printf関数の戻り値			|
;===============================================================|
	.const
print_help_mes	db	"smf2mml [ファイル名(.mid)] [/H]",0ah,0dh
		db	0ah,0dh
		db	"  /H		ヘルプを表示します。",0ah,0dh
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

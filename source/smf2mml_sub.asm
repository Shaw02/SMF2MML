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
;*		プロトタイプ宣言				*
;****************************************************************
;●外部宣言
printf		proto	stPrint:near32 ptr
fprintf		proto	ptStream:near32 ptr,stPrint:near32 ptr
exit		proto	iCode:dword
fread		proto	

;●プロトタイプ宣言
print_help	proto
endian_b2l_W	proto	iValue:near32 ptr
endian_b2l_D	proto	iValue:near32 ptr
NoteScaleOut	proto	Stream:near32 ptr byte,
			iScale:DWORD,
			iNote:DWORD

;===============================================================|
;		Big Endian -> Little Endian			|
;---------------------------------------------------------------|
;	●引数							|
;		iValue:near32 ptr	変数のポインタ		|
;	●引数							|
;		無し						|
;===============================================================|
.code
endian_b2l_W	proc	uses eax esi,
	iValue:near32 ptr

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
endian_b2l_D	proc	uses eax edx esi,
	iValue:near32 ptr

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
;		ノートの表示					|
;---------------------------------------------------------------|
;	●引数							|
;		FILE	*Stream	出力先				|
;		int	iScale	スケールの番号	(0 =in C)	|
;		int	iNote	ノート(0:C～11:b)		|
;	●引数							|
;		eax	fprintf関数の戻り値			|
;===============================================================|
.const
NoteScaleOut_Mes	db	"c*",0,0,	"d",0,0,0,	"d*",0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g",0,0,0,	"g*",0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"`c",0,0	;in Gs	-6	0
			db	"c",0,0,0,	"d",0,0,0,	"d*",0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g",0,0,0,	"g*",0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"b*",0,0	;in Ds	-5	1
			db	"c",0,0,0,	"d",0,0,0,	"d*",0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"b*",0,0	;in As	-4	2
			db	"c",0,0,0,	"d-",0,0,	"d",0,0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a",0,0,0,	"a*",0,0,	"b",0,0,0,	"b*",0,0	;in Es	-3	3
			db	"c",0,0,0,	"d-",0,0,	"d",0,0,0,	"e",0,0,0,	"e*",0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a-",0,0,	"a",0,0,0,	"b",0,0,0,	"b*",0,0	;in B	-2	4
			db	"c",0,0,0,	"d-",0,0,	"d",0,0,0,	"e-",0,0,	"e",0,0,0,	"f",0,0,0,	"g-",0,0,	"g",0,0,0,	"a-",0,0,	"a",0,0,0,	"b",0,0,0,	"b*",0,0	;in F	-1	5
			db	"c",0,0,0,	"c+",0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f",0,0,0,	"f+",0,0,	"g",0,0,0,	"g+",0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in C	 0	6
			db	"c",0,0,0,	"c+",0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g",0,0,0,	"g+",0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in G	+1	7
			db	"c*",0,0,	"c",0,0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g",0,0,0,	"g+",0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in D	+2	8
			db	"c*",0,0,	"c",0,0,0,	"d",0,0,0,	"d+",0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in A	+3	9
			db	"c*",0,0,	"c",0,0,0,	"d*",0,0,	"d",0,0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a",0,0,0,	"a+",0,0,	"b",0,0,0	;in E	+4	10
			db	"c*",0,0,	"c",0,0,0,	"d*",0,0,	"d",0,0,0,	"e",0,0,0,	"f*",0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a*",0,0,	"a",0,0,0,	"b",0,0,0	;in H	+5	11
			db	"c*",0,0,	"c",0,0,0,	"d*",0,0,	"d",0,0,0,	"e*",0,0,	"e",0,0,0,	"f",0,0,0,	"g*",0,0,	"g",0,0,0,	"a*",0,0,	"a",0,0,0,	"b",0,0,0	;in Fis	+6	12

.code
NoteScaleOut	proc	uses ebx esi,
		Stream:near32 ptr byte,
		iScale:DWORD,
		iNote:DWORD

		mov	esi,iScale
		mov	ebx,iNote

		lea	eax,[esi+6]		; eax = ( iScale + 6 )

		.if	(eax<=12)
			shl	eax,4		; eax = ( iScale + 6 ) * 16
			mov	esi,eax
			add	esi,eax
			add	esi,eax		; esi = ( iScale + 6 ) * 48
		.else
			mov	esi,48 * 6
		.endif
		lea	esi,[esi + ebx*4 + offset NoteScaleOut_Mes]

		invoke	fprintf,	Stream,esi

		ret
NoteScaleOut	endp
;===============================================================|
;		表示						|
;---------------------------------------------------------------|
;	●引数							|
;		stPrint	出力文字列				|
;	●引数							|
;		eax	printf関数の戻り値			|
;===============================================================|
.const
print_help_mes	db	"MML un-compiler for Standard MIDI File",0dh,0ah
		db	"Copyright (C) S.W. (A.watanabe)",0dh,0ah
		db	0ah,0dh
		db	"smf2mml [ /options ] [file(.mid)]",0dh,0ah
		db	0ah,0dh
		db	"  /B[+/-]		小節線と改行の出力有無を指定する。(Defailt=+)",0dh,0ah
		db	"			 +:Enable",0dh,0ah
		db	"			 -:Disable",0dh,0ah
		db	"  /C<number>		改行の設定を指定する。(Default=4)",0dh,0ah
		db	"			 number>= 1 : 改行する小節数を指定する。",0dh,0ah
		db	"			 number = 0 : 音符毎に改行する。",0dh,0ah
		db	"			 number =-1 : 改行しない。",0dh,0ah
		db	"  /Fm[file(.mml)]	.mmlファイル名を指定する。",0dh,0ah
		db	"  /H			ヘルプを表示する。",0dh,0ah
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

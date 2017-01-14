;	Static Name Aliases
;
	TITLE   mmap.c
	.286p
	.287
_TEXT	SEGMENT  WORD PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME DS: DGROUP
	ASSUME  SS: NOTHING
PUBLIC  _cntr
PUBLIC  _bus
PUBLIC  _matches
PUBLIC  _arguments
PUBLIC  _NoMatchMsg
PUBLIC  _MainMsgMCA
PUBLIC  _MainMsgISA
PUBLIC  _devhdr
PUBLIC  _DevHlp
PUBLIC  _appl_buffer
PUBLIC  _p
PUBLIC  _err
EXTRN	__acrtused:ABS
EXTRN	GETDOSVAR:NEAR
EXTRN	VIRTTOLIN:NEAR
EXTRN	VERIFYACCESS:NEAR
EXTRN	LOCKSEG:NEAR
EXTRN	UNLOCKSEG:NEAR
EXTRN	PHYSTOUVIRT:NEAR
EXTRN	DOSPUTMESSAGE:FAR
EXTRN	GETLIDENTRY:NEAR
EXTRN	FREELIDENTRY:NEAR
EXTRN	ABIOSCALL:NEAR
EXTRN	_STRATEGY:NEAR
EXTRN	BEEP:NEAR
EXTRN	ABORT:NEAR
EXTRN	SEGLIMIT:NEAR
EXTRN	MOVEBYTES:NEAR
_BSS      SEGMENT
COMM NEAR	_j:	BYTE:	 2
COMM NEAR	_board_address:	BYTE:	 4
COMM NEAR	_opencount:	BYTE:	 2
COMM NEAR	_savepid:	BYTE:	 2
COMM NEAR	_ABIOS_r_blk:	BYTE:	 70
COMM NEAR	_ABIOS_l_blk:	BYTE:	 32
COMM NEAR	_lid_blk_size:	BYTE:	 2
COMM NEAR	_card:	 66:	 9
COMM NEAR	_pcard:	BYTE:	 2
COMM NEAR	_pos_struct:	BYTE:	 6
COMM NEAR	_addr_struct:	BYTE:	 8
COMM NEAR	_chunk1:	BYTE:	 2
COMM NEAR	_chunk2:	BYTE:	 2
COMM NEAR	_lock_seg_han:	BYTE:	 4
COMM NEAR	_ptr:	BYTE:	 4
COMM NEAR	_i:	BYTE:	 2
_BSS      ENDS
_DATA      SEGMENT
_devhdr	DD	0ffffffffH
	DW	08880H
	DW	_STRATEGY
	DW	00H
	DB	'MMAP$   '
 	DB	12 DUP(0)

_DevHlp	DD	00H
_appl_buffer	DD	00H
_p	DD	00H
_err	DW	00H
_cntr	DW	00H
_bus	DW	00H
_matches	DW	00H
_arguments	DB	00H
 	DB	63 DUP(0)

_NoMatchMsg	DB	' no match for selected Micro Channel card ID found.',  0dH,  0aH
	DB	00H
_MainMsgMCA	DB	0dH,  0aH, 'OS/2 Micro Channel memory-mapped driver installe'
	DB	'd.',  0dH,  0aH,  00H
	ORG	$+1
_MainMsgISA	DB	0dH,  0aH, 'OS/2 ISA bus memory-mapped driver installed.',  0dH
	DB	0aH,  00H
_DATA      ENDS
_TEXT      SEGMENT
	ASSUME	CS: _TEXT
; Line 1
; Line 39
; Line 40
; Line 89
	PUBLIC	_main
_main	PROC NEAR
	enter	8,0
	push	si
;	rp = 4
;	ptr = -8
;	pptr = -6
;	liptr = -4
;	i = -12
;	addr = -16
;	in_data = -2
	les	si,DWORD PTR [bp+4]	;rp
; Line 97
	mov	al,BYTE PTR es:[si+2]
	sub	ah,ah
; Line 336
	cmp	ax,20
	ja	$SD462
	shl	ax,1
	xchg	ax,bx
	jmp	WORD PTR cs:$L20000[bx]
	nop	
$L20000:
		DW	$SC422
		DW	$SD462
		DW	$SD462
		DW	$SD462
		DW	$L20020
		DW	$SD462
		DW	$SD462
		DW	$SD462
		DW	$L20020
		DW	$SD462
		DW	$SD462
		DW	$SD462
		DW	$SD462
		DW	$SC424
		DW	$SC429
		DW	$SD462
		DW	$SC435
		DW	$SD462
		DW	$SD462
		DW	$SD462
		DW	$SC461
; Line 333
$SD462:
; Line 334
$L20020:
	mov	ax,256
	pop	si
	leave	
	ret	
; Line 99
$SC422:
; Line 103
	mov	ax,es
	push	ax
	push	si
	call	_Init
	add	sp,4
	pop	si
	leave	
	ret	
	nop	
$SC424:
; Line 109
	push	2
	lea	ax,WORD PTR [bp-8]	;ptr
	push	ss
	push	ax
	call	GETDOSVAR
	or	ax,ax
	je	$JCC102
	jmp	$SC461
$JCC102:
; Line 114
	les	si,DWORD PTR [bp-8]	;ptr
	mov	ax,WORD PTR es:[si]
	mov	dx,WORD PTR es:[si+2]
	mov	bx,ax
	mov	WORD PTR [bp-2],dx	;in_data
; Line 118
	cmp	WORD PTR _opencount,0
	jne	$I426
; Line 120
	mov	WORD PTR _opencount,1
; Line 121
	mov	es,dx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR _savepid,ax
; Line 123
	jmp	SHORT $L20020
	nop	
$I426:
; Line 125
	mov	ax,WORD PTR _savepid
	mov	es,dx
	cmp	WORD PTR es:[bx],ax
	je	$I428
; Line 126
	mov	ax,-32000
	pop	si
	leave	
	ret	
; Line 127
$I428:
	inc	WORD PTR _opencount
; Line 128
	jmp	SHORT $L20020
; Line 131
$SC429:
; Line 135
	push	2
	lea	ax,WORD PTR [bp-8]	;ptr
	push	ss
	push	ax
	call	GETDOSVAR
	or	ax,ax
	je	$JCC180
	jmp	$SC461
$JCC180:
; Line 148
	mov	ax,WORD PTR _savepid
	les	bx,DWORD PTR [bp-8]	;ptr
	les	bx,DWORD PTR es:[bx]
	cmp	WORD PTR es:[bx],ax
	je	$JCC197
	jmp	$SC461
$JCC197:
	cmp	WORD PTR _opencount,0
	jne	$JCC207
	jmp	$SC461
$JCC207:
; Line 153
	push	WORD PTR _board_address+2
	push	WORD PTR _board_address
	push	-32768
	push	2
	push	ds
	push	OFFSET DGROUP:_addr_struct
	call	PHYSTOUVIRT
; Line 155
	dec	WORD PTR _opencount
	jmp	$L20020
	nop	
$SC435:
; Line 168
	cmp	BYTE PTR es:[si+13],145
	je	$JCC245
	jmp	$L20020
$JCC245:
; Line 171
	mov	al,BYTE PTR es:[si+14]
	sub	ah,ah
; Line 325
	dec	ax
	je	$SC441
	dec	ax
	je	$SC442
	dec	ax
	je	$SC448
	dec	ax
	jne	$JCC266
	jmp	$SC454
$JCC266:
	jmp	$SC461
; Line 177
$SC441:
; Line 178
	mov	ax,WORD PTR _bus
	or	ah,129
	pop	si
	leave	
	ret	
	nop	
; Line 184
$SC442:
; Line 192
	push	WORD PTR es:[si+21]
	push	WORD PTR es:[si+19]
	push	8
	push	1
	call	VERIFYACCESS
	or	ax,ax
	je	$I443
; Line 193
$L20022:
	mov	ax,-32500
	pop	si
	leave	
	ret	
	nop	
; Line 197
$I443:
; Line 201
	mov	es,WORD PTR [bp+6]
	push	WORD PTR es:[si+21]
	push	0
	push	0
	push	ds
	push	OFFSET DGROUP:_lock_seg_han
	call	LOCKSEG
	or	ax,ax
	jne	$L20022
; Line 206
	push	WORD PTR _board_address+2
	push	WORD PTR _board_address
	push	-32768
	push	1
	push	ds
	push	OFFSET DGROUP:_addr_struct
	call	PHYSTOUVIRT
	jmp	$L20023
	nop	
$SC448:
; Line 235
	push	WORD PTR es:[si+21]
	push	WORD PTR es:[si+19]
	push	6
	push	1
	call	VERIFYACCESS
	or	ax,ax
	jne	$L20022
; Line 244
	mov	es,WORD PTR [bp+6]
	push	WORD PTR es:[si+21]
	push	ax
	push	ax
	push	ds
	push	OFFSET DGROUP:_lock_seg_han
	call	LOCKSEG
	or	ax,ax
	jne	$L20022
; Line 252
	push	WORD PTR _appl_buffer+2
	push	WORD PTR _appl_buffer
	push	ds
	push	OFFSET DGROUP:_pos_struct
	push	6
	call	MOVEBYTES
	or	ax,ax
	jne	$L20022
; Line 255
	push	WORD PTR _pos_struct+2
	push	WORD PTR _pos_struct
	call	_get_pos_data
	add	sp,4
	sub	ah,ah
	mov	WORD PTR _pos_struct+4,ax
; Line 262
	push	ds
	push	OFFSET DGROUP:_pos_struct
	push	WORD PTR _appl_buffer+2
	push	WORD PTR _appl_buffer
	push	6
	jmp	SHORT $L20024
	nop	
$SC454:
; Line 287
	push	WORD PTR es:[si+21]
	push	WORD PTR es:[si+19]
	push	8
	push	1
	call	VERIFYACCESS
	or	ax,ax
	je	$JCC469
	jmp	$L20022
$JCC469:
; Line 296
	mov	es,WORD PTR [bp+6]
	push	WORD PTR es:[si+21]
	push	ax
	push	ax
	push	ds
	push	OFFSET DGROUP:_lock_seg_han
	call	LOCKSEG
	or	ax,ax
	je	$JCC492
	jmp	$L20022
$JCC492:
; Line 301
	push	WORD PTR _board_address+2
	push	WORD PTR _board_address
	push	-32768
	push	1
	push	ds
	push	OFFSET DGROUP:_addr_struct
	call	PHYSTOUVIRT
	or	ax,ax
	je	$JCC519
	jmp	$L20022
$JCC519:
; Line 307
	push	WORD PTR _addr_struct+2
	push	WORD PTR _addr_struct
	push	ds
	push	OFFSET DGROUP:_addr_struct
	call	VIRTTOLIN
$L20023:
	or	ax,ax
	je	$JCC541
	jmp	$L20022
$JCC541:
; Line 315
	push	ds
	push	OFFSET DGROUP:_addr_struct
	mov	es,WORD PTR [bp+6]
	push	WORD PTR es:[si+21]
	push	WORD PTR es:[si+19]
	push	8
$L20024:
	call	MOVEBYTES
	or	ax,ax
	je	$JCC568
	jmp	$L20022
$JCC568:
; Line 320
	push	WORD PTR _lock_seg_han+2
	push	WORD PTR _lock_seg_han
	call	UNLOCKSEG
	or	ax,ax
	jne	$JCC586
	jmp	$L20020
$JCC586:
	jmp	$L20022
$SC461:
; Line 329
	mov	ax,-32509
; Line 337
	pop	si
	leave	
	ret	

_main	ENDP
; Line 340
	PUBLIC	_hex2bin
_hex2bin	PROC NEAR
	push	bp
	mov	bp,sp
;	c = 4
; Line 341
	cmp	BYTE PTR [bp+4],58	;c
	jge	$I465
; Line 342
	mov	al,BYTE PTR [bp+4]	;c
	cbw	
	sub	ax,48
	leave	
	ret	
; Line 343
$I465:
; Line 344
	mov	al,BYTE PTR [bp+4]	;c
	and	ax,223
	sub	ax,55
; Line 345
	leave	
	ret	
	nop	

_hex2bin	ENDP
; Line 348
	PUBLIC	_get_POS
_get_POS	PROC NEAR
	enter	2,0
	push	di
	push	si
;	slot_num = 4
;	card_ID = 6
;	pos_regs = 10
;	register si = rc
;	register bx = i
;	lid = -2
; Line 351
	push	16
	push	0
	push	1
	lea	ax,WORD PTR [bp-2]	;lid
	push	ss
	push	ax
	call	GETLIDENTRY
	or	ax,ax
	je	$I474
; Line 352
$L20025:
	mov	ax,1
	pop	si
	pop	di
	leave	
	ret	
	nop	
; Line 356
$I474:
	mov	WORD PTR _ABIOS_l_blk,32
; Line 357
	mov	ax,WORD PTR [bp-2]	;lid
	mov	WORD PTR _ABIOS_l_blk+2,ax
; Line 359
	mov	WORD PTR _ABIOS_l_blk+6,1
; Line 360
	mov	WORD PTR _ABIOS_l_blk+12,23130
; Line 363
	push	ax
	sub	ax,ax
	mov	WORD PTR _ABIOS_l_blk+4,ax
	mov	WORD PTR _ABIOS_l_blk+14,ax
	push	ax
	push	ds
	push	OFFSET DGROUP:_ABIOS_l_blk
	call	ABIOSCALL
	or	ax,ax
	jne	$L20025
; Line 366
	mov	ax,WORD PTR _ABIOS_l_blk+24
	mov	WORD PTR _lid_blk_size,ax
; Line 370
	les	bx,DWORD PTR [bp+6]	;card_ID
	mov	WORD PTR es:[bx],-1
; Line 371
	sub	bx,bx
	mov	di,WORD PTR [bp+10]	;pos_regs
	mov	es,WORD PTR [bp+12]
$F476:
	mov	si,di
	add	si,bx
	mov	BYTE PTR es:[si],0
	inc	bx
	cmp	bx,64
	jb	$F476
; Line 375
	mov	ax,WORD PTR _lid_blk_size
	mov	WORD PTR _ABIOS_r_blk,ax
; Line 376
	mov	ax,WORD PTR [bp-2]	;lid
	mov	WORD PTR _ABIOS_r_blk+2,ax
; Line 378
	mov	WORD PTR _ABIOS_r_blk+6,13
; Line 379
	mov	WORD PTR _ABIOS_r_blk+12,23130
; Line 382
	mov	cl,BYTE PTR [bp+4]	;slot_num
	and	cl,15
	mov	BYTE PTR _ABIOS_r_blk+16,cl
; Line 383
	mov	cx,WORD PTR [bp+12]
	mov	WORD PTR _ABIOS_r_blk+22,di
	mov	WORD PTR _ABIOS_r_blk+24,cx
; Line 384
	mov	WORD PTR _ABIOS_r_blk+18,-1
; Line 386
	push	ax
	sub	ax,ax
	mov	WORD PTR _ABIOS_r_blk+4,ax
	mov	WORD PTR _ABIOS_r_blk+14,ax
	push	ax
	push	ds
	push	OFFSET DGROUP:_ABIOS_r_blk
	call	ABIOSCALL
	or	ax,ax
	je	$I479
; Line 387
	mov	si,1
; Line 388
	jmp	SHORT $I480
$I479:
; Line 389
	mov	ax,WORD PTR _ABIOS_r_blk+18
	les	bx,DWORD PTR [bp+6]	;card_ID
	mov	WORD PTR es:[bx],ax
; Line 390
	sub	si,si
; Line 391
$I480:
; Line 392
	push	WORD PTR [bp-2]	;lid
	call	FREELIDENTRY
; Line 393
	mov	ax,si
; Line 395
	pop	si
	pop	di
	leave	
	ret	
	nop	

_get_POS	ENDP
; Line 398
	PUBLIC	_get_pos_data
_get_pos_data	PROC NEAR
	enter	2,0
	push	di
	push	si
;	slot = 4
;	reg = 6
;	pos = -1
;	register si = cptr
	mov	di,WORD PTR [bp+6]	;reg
; Line 402
	imul	si,WORD PTR [bp+4],66	;slot
	add	si,OFFSET DGROUP:_card-66
; Line 403
	or	di,di
	je	$L20026
; Line 406
	cmp	di,1
	jne	$I488
; Line 407
	mov	ax,WORD PTR [si]
	shr	ax,16
	pop	si
	pop	di
	leave	
	ret	
	nop	
$I488:
; Line 409
	add	si,di
$L20026:
	mov	al,BYTE PTR [si]
; Line 410
; Line 411
	pop	si
	pop	di
	leave	
	ret	

_get_pos_data	ENDP
; Line 416
	PUBLIC	_Init
_Init	PROC NEAR
	enter	12,0
	push	di
	push	si
;	rp = 4
;	lid = -10
;	p = -8
	mov	si,WORD PTR [bp+4]	;rp
; Line 423
	mov	es,WORD PTR [bp+6]
	mov	ax,WORD PTR es:[si+14]
	mov	dx,WORD PTR es:[si+16]
	mov	WORD PTR _DevHlp,ax
	mov	WORD PTR _DevHlp+2,dx
; Line 425
	push	16
	push	0
	push	1
	lea	ax,WORD PTR [bp-10]	;lid
	push	ss
	push	ax
	call	GETLIDENTRY
	or	ax,ax
	je	$JCC931
	jmp	$I494
$JCC931:
; Line 426
	push	WORD PTR [bp-10]	;lid
	call	FREELIDENTRY
; Line 430
	mov	WORD PTR _bus,1
; Line 434
	mov	WORD PTR _i,0
$F495:
; Line 435
	imul	ax,WORD PTR _i,66
	mov	cx,ax
	add	ax,OFFSET DGROUP:_card+2
	push	ds
	push	ax
	add	cx,OFFSET DGROUP:_card
	push	ds
	push	cx
	mov	ax,WORD PTR _i
	inc	ax
	push	ax
	call	_get_POS
	add	sp,10
	inc	WORD PTR _i
	cmp	WORD PTR _i,8
	jbe	$F495
; Line 438
	sub	ax,ax
	mov	WORD PTR _matches,ax
	mov	WORD PTR _i,ax
	mov	WORD PTR _pcard,OFFSET DGROUP:_card
$F498:
; Line 439
	mov	bx,WORD PTR _pcard
	cmp	WORD PTR [bx],27901
	je	$L20009
; Line 438
	add	WORD PTR _pcard,66
	inc	WORD PTR _i
	cmp	WORD PTR _i,8
	jbe	$F498
	jmp	SHORT $FB500
$L20009:
; Line 440
	mov	WORD PTR _matches,1
; Line 443
$FB500:
; Line 445
	cmp	WORD PTR _matches,0
	jne	$I502
; Line 446
	push	1
	push	8
	push	ds
	push	OFFSET DGROUP:_devhdr+10
	call	FAR PTR DOSPUTMESSAGE
; Line 447
	push	1
	mov	ax,OFFSET DGROUP:_NoMatchMsg
	mov	dx,ax
	mov	di,ax
	push	ds
	pop	es
	mov	cx,-1
	xor	ax,ax
	repnz	scasb
	not	cx
	dec	cx
	push	cx
	push	ds
	push	dx
	call	FAR PTR DOSPUTMESSAGE
; Line 449
	sub	ax,ax
	mov	es,WORD PTR [bp+6]
	mov	WORD PTR es:[si+14],ax
	mov	WORD PTR es:[si+16],ax
; Line 450
	mov	ax,-32509
	pop	si
	pop	di
	leave	
	ret	
	nop	
; Line 455
$I502:
; Line 456
	push	4
	mov	ax,WORD PTR _i
	inc	ax
	push	ax
	mov	si,ax
	call	_get_pos_data
	add	sp,4
	push	3
	push	si
	mov	WORD PTR [bp-12],ax
	call	_get_pos_data
	add	sp,4
	and	ax,1
	sub	dx,dx
	mov	dh,dl
	mov	dl,ah
	mov	ah,al
	sub	al,al
	shl	ax,1
	rcl	dx,1
	shl	ax,1
	rcl	dx,1
	shl	ax,1
	rcl	dx,1
	shl	ax,1
	rcl	dx,1
	shl	ax,1
	rcl	dx,1
	shl	ax,1
	rcl	dx,1
	shl	ax,1
	rcl	dx,1
	mov	cl,BYTE PTR [bp-12]
	sub	ch,ch
	sub	bx,bx
	or	dx,cx
	mov	WORD PTR _board_address,ax
	mov	WORD PTR _board_address+2,dx
; Line 459
	jmp	$I503
	nop	
$I494:
; Line 464
	mov	WORD PTR _bus,0
; Line 468
	mov	es,WORD PTR [bp+6]
	mov	ax,WORD PTR es:[si+18]
	mov	dx,WORD PTR es:[si+20]
	mov	cx,ax
	mov	di,ax
	mov	WORD PTR [bp-6],dx
	mov	WORD PTR [bp-8],di	;p
	mov	es,dx
	mov	bx,ax
	cmp	BYTE PTR es:[bx],0
	jne	$L20017
	mov	si,ax
	jmp	SHORT $L20007
	nop	
$L20017:
	les	si,DWORD PTR [bp-8]	;p
$F504:
	cmp	BYTE PTR es:[si],32
	je	$L20007
	inc	si
	cmp	BYTE PTR es:[si],0
	jne	$F504
$L20007:
; Line 469
	cmp	BYTE PTR es:[si],32
	jne	$L20003
$F507:
	inc	si
	cmp	BYTE PTR es:[si],32
	je	$F507
$L20003:
; Line 470
	cmp	BYTE PTR es:[si],0
	je	$I503
; Line 472
	sub	ax,ax
	mov	WORD PTR _board_address+2,ax
	mov	WORD PTR _board_address,ax
; Line 473
	cmp	BYTE PTR es:[si],0
	je	$FB513
$F511:
; Line 474
	mov	es,WORD PTR [bp-6]
	mov	al,BYTE PTR es:[si]
	push	ax
	call	_hex2bin
	add	sp,2
	cwd	
	mov	cx,WORD PTR _board_address
	mov	bx,WORD PTR _board_address+2
	shl	cx,1
	rcl	bx,1
	shl	cx,1
	rcl	bx,1
	shl	cx,1
	rcl	bx,1
	shl	cx,1
	rcl	bx,1
	add	ax,cx
	adc	dx,bx
	mov	WORD PTR _board_address,ax
	mov	WORD PTR _board_address+2,dx
	mov	es,WORD PTR [bp-6]
	inc	si
	cmp	BYTE PTR es:[si],0
	jne	$F511
$FB513:
; Line 475
	mov	ax,WORD PTR _board_address
	mov	dx,WORD PTR _board_address+2
	mov	WORD PTR _addr_struct+4,ax
	mov	WORD PTR _addr_struct+6,dx
; Line 477
$I503:
; Line 479
	cmp	WORD PTR _bus,0
	je	$I514
; Line 480
	push	1
	mov	ax,OFFSET DGROUP:_MainMsgMCA
	jmp	SHORT $L20028
$I514:
; Line 482
	push	1
	mov	ax,OFFSET DGROUP:_MainMsgISA
$L20028:
	mov	dx,ax
	mov	di,ax
	push	ds
	pop	es
	mov	cx,-1
	xor	ax,ax
	repnz	scasb
	not	cx
	dec	cx
	push	cx
	push	ds
	push	dx
	call	FAR PTR DOSPUTMESSAGE
; Line 487
	push	cs
	mov	ax,WORD PTR [bp+4]	;rp
	mov	dx,WORD PTR [bp+6]
	add	ax,14
	push	dx
	push	ax
	call	SEGLIMIT
	or	ax,ax
	jne	$I517
	push	ds
	mov	ax,WORD PTR [bp+4]	;rp
	mov	dx,WORD PTR [bp+6]
	add	ax,16
	push	dx
	push	ax
	call	SEGLIMIT
	or	ax,ax
	je	$I516
$I517:
; Line 488
	call	ABORT
; Line 490
$I516:
	push	200
	push	500
	call	BEEP
; Line 491
	push	200
	push	500
	call	BEEP
; Line 492
	push	250
	push	500
	call	BEEP
; Line 493
	push	300
	push	500
	call	BEEP
; Line 494
	push	250
	push	500
	call	BEEP
; Line 495
	push	300
	push	500
	call	BEEP
; Line 497
	mov	ax,256
; Line 499
	pop	si
	pop	di
	leave	
	ret	
	nop	

_Init	ENDP
_TEXT	ENDS
END

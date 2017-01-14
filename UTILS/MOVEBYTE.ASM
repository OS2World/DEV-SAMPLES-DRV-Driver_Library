;  movebyte.asm  OS/2 Version 2.0
;
;  this routine transfers bytes data to and from the device driver
;
;  C Calling Sequence:
;  if (MoveBytes((FARPOINTER)&From,(FARPOINTER)&To,USHORT Lenth))  err
;
        .386
        include	drvlib.inc

	public	MOVEBYTES
	extrn	_DevHlp:dword
	assume  CS: _TEXT
_TEXT	segment word public use16 'CODE'
MOVEBYTES proc near

	push	bp
	mov	bp,sp
	push    es
	push	ds
	pusha
	pushf			; save everything
	mov	cx,[bp+4]	; length
        xor     ax,ax
	or	cx,cx		; exit if zero
        jz      error           ; bail out
	lds	si,[bp+10]	; from
	les	di,[bp+6]	; to
        cld
	or      cx,3		; if 4 byte boundary, optimize
	jz	double_move	; do movsd
        test    cx,1            ; if even number of bytes, save a
        jz      wordmove        ; little time by doing a word move
	rep     movsb
        jmp     short finish    ; done

double_move:
	shr	cx,1
	shr	cx,1
	rep	movsd
	jmp	short finish	;done
error:  mov     ax,1
        jmp     short get_out   ; bail

wordmove:
        shr     cx,1            ; half the number of bytes
        rep     movsw
finish:
        sub     ah,ah
get_out:
	popf			; restore flags
	popa	                ; restore regs
	pop	ds
	pop     es
	pop	bp
	ret	10              ; fix up stack

MOVEBYTES endp
_TEXT	ends
	end

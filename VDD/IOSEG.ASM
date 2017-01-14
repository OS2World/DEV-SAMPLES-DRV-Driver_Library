; Version 1.0 01 January 1993
; IO segment for VDD I/O
;
; by: Steve Mastrianni
;
        PUBLIC	outp
        PUBLIC  inp

        .386P

_IOSEG	segment	dword public use32 'CODE'

	assume	CS: _IOSEG
;
;  inp( ULONG port)
;
inp    proc	far
	;
        push    bp             ;set up stack frame
        mov     bp,sp          ;save bp
        push    dx             ;save dx
	mov     dx,[bp+10]     ;get port address
        in      al,dx          ;do input
        pop     dx             ;restore regs
        pop     bp             ;return in ax
	ret     4              ;remove from IOPL stack
;
inp    endp

; outp (ULONG port, USHORT data)

outp   proc	far
;
        push    bp             ;set up stack frame
        mov     bp,sp          ;save it
        push    ax             ;save ax
        push    dx             ;and dx
	mov     ax,word ptr[bp+10] ;get data
        mov     dx,[bp+14]     ;get port
        out     dx,al          ;do output
        pop     dx             ;restore regs
        pop     ax
        pop     bp
	ret     8                ;remove off local stack
;
outp   endp

_IOSEG   ends
	 end 

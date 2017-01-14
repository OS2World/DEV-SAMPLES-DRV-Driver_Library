;
; Sample IOPL segment 
;
        PUBLIC		IN_PORT
        PUBLIC		OUT_PORT

        .model large
        .286P

_IOSEG  segment	word public USE16 'CODE'

	assume	  CS: _IOSEG, DS: DGROUP, SS: DGROUP
	.286P
;
IN_PORT  proc	   far
;
        push    bp         ;set up stack frame
        mov     bp,sp      ;save bp
        push    dx         ;save dx
	mov     dx,[bp+6]  ;get port address
        in      ax,dx      ;do input
        pop     dx         ;restore regs
        pop     bp         ;return in ax
        ret     2          ;remove from IOPL stack
;
IN_PORT  endp

OUT_PORT proc	   far
;
        push    bp         ;set up stack frame
        mov     bp,sp      ;save it
        push    ax         ;save ax
        push    dx         ;and dx
	mov     ax,[bp+6]  ;get data
        mov     dx,[bp+8]  ;get port
        out     dx,al      ;do output
        pop     dx         ;restore regs
        pop     ax
        pop     bp
	ret     4          ;remove off local stack
;
OUT_PORT endp

_IOSEG  ends
	  end 



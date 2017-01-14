; out8reg(port,byte)
;
; write a simple 8 bit register with interrupts off

      .286

include mmap.inc

_TEXT	SEGMENT BYTE PUBLIC 'CODE'
_TEXT	ENDS

       assume CS: _TEXT

_TEXT	SEGMENT

_out8reg proc near

public     _out8reg

    cli
    push   bp
    mov    bp,sp		;set up base pointer
    pusha 	 	;save regs
    pushf         	;and flags
    push   es
    push   ds
    mov    dx,word ptr [bp+4] ;output register address
    mov    al,byte ptr [bp+6] ;byte to be output
    out    dx,al	;output low byte
    jmp    $+2
    jmp    $+2
    pop    ds		 ;restore registers
    pop    es 
    popf
    popa
    pop    bp
    sti
    ret

_out8reg endp

_text	ends
	end




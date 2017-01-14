; out16reg(port,word);
;
; write a 16-bit value to a DMA register by issuing two
; consecutive writes to an 8-bit register
;
      .286

include mmap.inc

_TEXT	SEGMENT BYTE PUBLIC 'CODE'
_TEXT	ENDS

    assume CS: _TEXT

_TEXT	SEGMENT

_out16reg proc near

public     _out16reg

    cli
    push   bp
    mov    bp,sp		;set up base pointer
    pusha 	 		;save regs
    pushf         		;and flags
    push   es
    push   ds

;make sure that first write goes to low byte of register

    mov    dx,DMA_BYTE_POINTER_FLIPFLOP
    mov    al,0		     	;reset byte pointer
    out    dx,al
    jmp    $+2     ;register delay
    jmp    $+2
    mov    dx,word ptr [bp+4] 	;output port address
    mov    al,byte ptr [bp+6] 	;byte to be output
    out    dx,al		;output low byte
    jmp    $+2
    jmp    $+2
    mov    al,byte ptr [bp+7];byte to be output
    out    dx,al	   	;output high byte
    jmp    $+2
    jmp    $+2
    pop    ds		   	;restore registers
    pop    es 
    popf
    popa
    pop    bp
    sti
    ret

_out16reg endp

_text	ends
	end



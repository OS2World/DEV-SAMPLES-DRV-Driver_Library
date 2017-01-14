;    C startup routine, one device
;
		EXTRN   _main:near
		EXTRN   _interrupt_handler:near
		EXTRN   _timer_handler:near
		PUBLIC	_STRAT
		PUBLIC  _TIM_HNDLR
		PUBLIC  _INT_HNDLR
		PUBLIC	__acrtused

_DATA	segment word public 'DATA'
_DATA	ends

CONST	segment word public 'CONST'
CONST	ends

_BSS	segment word public 'BSS'
_BSS	ends

DGROUP	group CONST,_BSS,_DATA

_TEXT	segment word public 'CODE'
 	assume cs:_TEXT,ds:DGROUP,es:NOTHING,ss:NOTHING
	.286P
;
_STRAT	proc	far
__acrtused:			;no startup code
;
	push	0
	jmp	start		;signal device 0
;
start:
	push	es		;send Request Packet address
	push	bx
	call	_main		;call driver mainline
	pop	bx		;restore es:bx
	pop	es
	add	sp,2		;clean up stack
	mov	word ptr es:[bx+3],ax ;send completion status
	ret
;
_STRAT	endp
;
_INT_HNDLR proc    far
;
	call	_interrupt_handler ;handle rupts
	ret			;bail out
;
_INT_HNDLR	endp
;
_TIM_HNDLR	proc    far
;
	pusha
	push	es
	push	ds
	call	_timer_handler
	pop	ds
	pop	es
	popa
	ret
;
_TIM_HNDLR 	endp

_TEXT	ends
	end



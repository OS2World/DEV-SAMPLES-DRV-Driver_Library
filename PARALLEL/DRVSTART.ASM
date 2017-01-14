;
;	C Startup routine for parallel device driver
;
	EXTRN	  _main:near
      EXTRN   _timr_handler:near
      PUBLIC  _STRATEGY
	PUBLIC  __acrtused
      PUBLIC  _TIMER_HANDLER

_DATA	segment 	word public 'DATA'
_DATA	ends

CONST	segment	 word public 'CONST'
CONST	ends

_BSS	segment 	word public 'BSS'
_BSS	ends

DGROUP	 group	 CONST, _BSS, _DATA

_TEXT	segment 	word public 'CODE'

	  assume		cs:_TEXT, ds:DGROUP, es:NOTHING, ss:NOTHING
	  .286

_STRATEGY proc    far
__acrtused:			;to satisfy C 

start:
	  push		es	; &reqpacket high part
	  push		bx	; &reqpacket low part
	  call		_main
	  pop	      bx
	  pop	      es
	  mov	      word ptr es:[bx+3],ax  ; plug in status word
	  ret
_STRATEGY endp
;
_TIMER_HANDLER proc 	far
;
	  pusha			;save flags, regs
        push      ds
        push		es	;make up for the 'almost all' push
	  call		_timr_handler ;handle interrupts
        pop	      es
        pop       ds
	  popa			;restore everything and
        ret			;bail out
;
_TIMER_HANDLER endp


_TEXT	  ends
	  end



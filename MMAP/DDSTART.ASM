;	C Startup routine for OS/2 Device Drivers
;
;  1.	Provides the user's "main" routine with the address of
;	the request packet.
;	
;  2.   Defines the segments in the proper order (data, then code)
;	so if ddstart.obj is included first in the link line, the
;	segments for the device driver will be ordered correctly.
;
;  3.	Satisfies the C EXTRN for __acrtused, which prevents the startup
;	logic from being included from the standard C library.

	EXTRN	_main:near
        PUBLIC	_STRATEGY
	PUBLIC  __acrtused

_DATA	segment	word public 'DATA'
_DATA	ends

CONST	segment	word public 'CONST'
CONST	ends

_BSS	segment word public 'BSS'
_BSS	ends

DGROUP	group 	CONST, _BSS, _DATA

_TEXT	segment	word public 'CODE'

	  assume	cs:_TEXT, ds:DGROUP, es:NOTHING, ss:NOTHING
	  .286P
;
_STRATEGY proc	far
__acrtused:		; to satisfy EXTRN in C-generated modules
;
	push	es		;send request packet address
	push	bx
	call	_main		;call driver mailine
	pop	bx		;restore es:bx
	pop	es
	mov	word ptr es:[bx+3],ax ;send completion status
	ret
;
_STRATEGY	endp

_TEXT	  ends
	  end

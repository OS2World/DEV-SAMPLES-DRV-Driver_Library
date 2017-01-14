	title _word_dma
	.286P
	.model small
	include	bsedos.inc
;
; dma set up and execute routine 
; written by: Steve Mastrianni
;
; word_dma(USHORT operation, 	 1=write, 2=read	     [bp+4]
;	   USHORT channel, 	 5, 6 or 7		     [bp+6]
;	   USHORT count, 	 0-65535 (0=1 word)	     [bp+8]
;	   ULONG address,	 far to/from address	     [bp+10,12]
;	   USHORT auto,		 0 for single, 1 for auto    [bp+14]
;	   USHORT init) 	 0 no auto init, 1 auto init [bp+16]
;
_text	segment public 'CODE'
	assume	cs:_text,ds:NOTHING
	public	_word_dma

_word_dma proc	near
	push	bp		;
	mov	bp,sp		;current frame pointer
	cli			;disable rupts during dma setup
	push	bx
	push	dx
	mov	ax,[bp+6]	;get channel number
	sub	ax,4		;minus 4 for second controller
	mov	bx,[bp+4]	;get mode byte and make command
	shl	bx,2		;make valid mode bits
	or	ax,bx
	mov	bx,[bp+14]	;or in initialize bit
	cmp	bx,0		;autoinitialize selected?
	jz	output		;no
	or	ax,010h		;yes, add in autoinitialize bit
output:
	mov	bx,[bp+16]	;block or single mode?
	or	ax,40h		;default single
	cmp	bx,0
	jz	single		;single mode
	and	ax,0bfh		;make block mode
	or	ax,080h
single:
	out	0d8h,al		;set the first/last flip flop
	jmp	short $+2	;small delay
	out	0d6h,al		;output the mode byte
	mov	dx,[bp+6]	;get channel number
	sub	dx,4		;minus 4 for second controller
	mov	ax,08ah		;set page register
	add	ax,dx		;
	push	dx		;save port temp
	mov	dx,ax		;put page register address in dx
	mov	ax,ds		;high page address
	out	dx,al		;do it
	pop	dx
	rol	dx,2		;times 4 for proper address
	add	dx,0c0h		;this is port address
	mov	ax,[bp+10]	;low offset address
	out	dx,al
	jmp	short $+2
	mov	al,ah		;now high part
	out	dx,al		;do it
	jmp	short $+2
	add	dx,2		;formulate count address
	mov	ax,[bp+8]	;put low and
	out	dx,al		;high count to controller
	jmp	short $+2
	mov	al,ah
	out	dx,al
	jmp	short $+2
	sti			;re-enable interrupts
	mov	ax,4		;request dma transfer 
	or	ax,[bp+6]	;add in channel number
	out	0d2h,al		;request dma transfer
	jmp	short $+2
	pop	dx
	pop	bx
	pop	bp
	ret	
;
_word_dma endp

_text	ends

	end

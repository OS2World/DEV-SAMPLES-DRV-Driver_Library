;  int3.asm
;
;  this is NOT a DevHlp, but merely a simple way to break the
;  KDB at a specified point
;
;  C calling sequence: 
;  INT3();
;
        .286
	public	 INT3
	assume   CS: _TEXT
_TEXT	segment word public 'CODE'
INT3	proc near

	int	   3
	ret

INT3	endp
_TEXT	ends
	end



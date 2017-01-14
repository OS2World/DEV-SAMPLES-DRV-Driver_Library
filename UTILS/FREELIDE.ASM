;  DevHlp 0x35
;  this routine releases the logical ID (LID) 
;
;  C Calling Sequence:
;  if (FreeLIDEntry (USHORT id) ) err
;
     include drvlib.inc
;
     public  FREELIDENTRY

     extrn   _DevHlp:dword
     assume  CS: _TEXT
_TEXT        segment word public 'CODE'

FREELIDENTRY  proc near

     push    bp
     mov     bp,sp
     mov     ax,[bp+4] ; logical ID
     mov     dl,DevHlp_FreeLIDEntry
     call    [_DevHlp]
     jc      error           ; error from device help
     xor     ax,ax           ; no errors
     pop     bp
     ret     2               ; fix up the stack
error:    
     mov     ax,1            ; return error for C
     pop     bp
     ret     2               ; fix up stack and return

FREELIDENTRY  endp
_TEXT     ends
     end



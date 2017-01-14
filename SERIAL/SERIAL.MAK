serial.sys: drvstart.obj serial.obj 
	link /nod /noi /map drvstart+serial,serial.sys,serial,\
c:\os2\doscalls+slibcep+\drvlib\drvlib,serial.def
	mapsym serial

drvstart.obj: drvstart.asm
	masm -Mx -t -L -N drvstart;

serial.obj: serial.c drvlib.h serial.h uart.h
	cl -c -Asnw -Gs -G2 -Fc -Zl -Zp -Ox serial.c




digio.sys: drvstart.obj digio.obj 
     link /nod /noi /map drvstart+digio,digio.sys,digio,\
c:\os2\doscalls+slibcep+drvlib,digio.def
     mapsym digio

drvstart.obj: drvstart.asm
	masm -Mx -e -t -L -N drvstart;

digio.obj: digio.c drvlib.h digio.h
	cl -c -Asnw -Gs -G2 -Zl -Zp -Ox digio.c




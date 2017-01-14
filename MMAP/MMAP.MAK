#  makefile for memory mapped driver

mmap.sys: ddstart.obj mmap.obj 
	link /nod /noi /map ddstart+mmap,mmap.sys,mmap,c:\os2\doscalls+d:\lib\slibcep+\
d:\drvlib\drvlib\drvlib,mmap.def
        mapsym mmap

ddstart.obj: ddstart.asm
	masm -Mx -t -L -N ddstart;

mmap.obj: mmap.c drvlib.h mmap.h 
	cl -Fa -c -Asnw -Gs -G2 -Zl -Zp -Ox mmap.c


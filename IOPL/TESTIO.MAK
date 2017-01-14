testio.exe: testio.obj ioseg.obj 
	link16 /CO /nod /noe /noi /map testio+ioseg,testio.exe,testio,c:\os2\doscalls+llibcep,testio.def

testio.obj: testio.c  
	cl -c -AL -G2 testio.c

ioseg.obj: ioseg.asm
        masm /MX /T ioseg.asm;


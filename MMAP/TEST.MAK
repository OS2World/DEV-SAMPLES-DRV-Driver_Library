test.exe: test.obj
     link16 test,test,test,+c:\os2\doscalls+d:\lib\llibcep,,test.def

test.obj: test.c
     cl -AL -G2 -c test.c


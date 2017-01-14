all: ioseg.lib testio32.exe

ioseg.lib: ioseg.def
  implib /nologo ioseg.lib ioseg.def

testio32.exe: testio32.obj ioseg.obj 
	link386 /noi /map /pm:vio testio32,,testio32,ioseg,testio32

testio32.obj: testio32.c  
	icc -c -Q -Gd testio32.c


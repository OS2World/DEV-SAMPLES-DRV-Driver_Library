vpio.sys: pioinit.obj piouser.obj pioin.obj pioout.obj piodata.obj ioseg.obj
	link386 /A:16 /M:FULL /NOL pioinit+piouser+pioin+pioout+piodata+ioseg,vpio.sys,vpio.map,vdh,pio.def
	mapsym vpio

pioinit.obj: pioinit.c mvdm.h pio.h
	icc /Sm /Ss /O /Q /W2 /Rn /Gr /C pioinit.c

pioin.obj: pioin.c pio.h mvdm.h
	icc /Sm /Ss /Q /O /W2 /Rn /Gr /C pioin.c

pioout.obj: pioout.c pio.h mvdm.h
	icc /Sm /Ss /Q /O /W2 /Rn /Gr /C pioout.c

piouser.obj: piouser.c pio.h mvdm.h
	icc /Sm /Ss /Q /O /W2 /Rn /Gr /C piouser.c

piodata.obj: piodata.c pio.h mvdm.h
	icc /Sm /Ss /Q /O /W2 /Rn /Gr /C piodata.c

ioseg.obj: ioseg.asm
        masm /Mx /x ioseg.asm;

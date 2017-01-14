ioseg.dll: ioseg.obj
     link16 /MAP /NOI /NOD ioseg,ioseg.dll,ioseg,d:\lib\llibcdll+os2286,ioseg.def

ioseg.obj: ioseg.asm
     masm ioseg.asm;

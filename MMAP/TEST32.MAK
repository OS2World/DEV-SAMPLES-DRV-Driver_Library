test32.exe: test32.obj
     link386 /MAP /NOI /PM:vio test32,test32,test32,,,test32.def

test32.obj: test32.c
     icc /c /Gt+ test32.c


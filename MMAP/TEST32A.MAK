test32a.exe: test32a.obj
     link386 /MAP /NOI /PM:vio test32a,test32a,test32a,,,test32a.def

test32a.obj: test32a.c
     icc /c /Gt+ test32a.c


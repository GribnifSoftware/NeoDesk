cd h:\source\neodesk
rm neodesk.exe
cp neodesk1.prg neodesk.exe
compres5 a neodesk.exe nd_vdi.exe neodes22.exe -neodesk2.exe gui.exe help.exe iconedit.exe settings.exe -data.exe
call test

exit

test;
  if $errnum != 0
    echo sizes: %size compress.cmp %size neodesk.exe
    getch i
    mv compress.cmp neodesk.exe o-
  endif
return




cd h:\source\neodesk
set d %mid %sysdate 4 2 ~ %right %sysdate 2
set f "c:\\neo4$d".lzh
if %exists $f
  set cmd 'u'
else
  set cmd 'a'
endif
newlzh $cmd $f @zip.dat

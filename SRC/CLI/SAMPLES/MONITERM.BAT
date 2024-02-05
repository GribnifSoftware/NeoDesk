; Moniterm management batch file, allow the user to switch between a
; SM124 Monochrome Monitor and the Moniterm display

unset *

if %alert "[2][|Switch Monitor?][Ok|Cancel]" == 1
  goto switch
endif
exit

switch;
if "$max_wind" == "0 19 1280 941"
  if %exists c:\neodesk\moniterm.inf
    rm c:\neodesk\moniterm.inf
  endif
  if %exists c:\neodesk\neodeskm.inf
    cp c:\neodesk\neodeskm.inf c:\neodesk\moniterm.inf
    rm c:\neodesk\neodeskm.inf
  endif
  if %exists c:\neodesk\sm124.inf
    cp c:\neodesk\sm124.inf c:\neodesk\neodeskm.inf
  endif
  rn c:\auto\bigmontr.prg c:\auto\bigmontr.prx
  if %exists c:\auto\physbase.prg
    rn c:\auto\physbase.prg c:\auto\physbase.prx
  endif
  if %exists c:\auto\maccel2.prx
    rn c:\auto\maccel2.prx c:\auto\maccel2.prg
  endif
  warmboot
elseif "$max_wind" == "0 19 640 381"
  if %exists c:\neodesk\sm124.inf
    rm c:\neodesk\sm124.inf
  endif
  if %exists c:\neodesk\neodeskm.inf
    cp c:\neodesk\neodeskm.inf c:\neodesk\sm124.inf
    rm c:\neodesk\neodeskm.inf
  endif
  if %exists c:\neodesk\moniterm.inf
    cp c:\neodesk\moniterm.inf c:\neodesk\neodeskm.inf
  endif
  rn c:\auto\bigmontr.prx c:\auto\bigmontr.prg
  if %exists c:\auto\physbase.prx
    rn c:\auto\physbase.prx c:\auto\physbase.prg
  endif
  if %exists c:\auto\maccel2.prg
    rn c:\auto\maccel2.prg c:\auto\maccel2.prx
  endif
  warmboot
else
  null %alert "[3][|Unknown resolution!][Oops]"
  pause
endif
exit

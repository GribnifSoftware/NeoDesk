set .input lzh2.dat
cp fontconv\fontconv.ttp fontconv\fontconv.npg o-
cp inf_conv.prg inf_conv.npg o-
while %hasch
  getstr s
  if "$s" == ""
    exit
  endif
  newlzh a e:\beta_neo.lzh $s
  if $errnum != 0
    echo Error $errnum
    exit
  endif
endwhile

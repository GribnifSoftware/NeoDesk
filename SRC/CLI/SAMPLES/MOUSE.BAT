echo "\033E"
while $mouse_but != 1
  echo nl- ' '
  set col $mouse_col
  set row $mouse_row
  echo nl- '\033j*\033k'
endwhile
echo "\033E"


; Batch file to demonstrate %GETSCR, %MOUSE_ROW, and %MOUSE_COL

echo "\033E  <= character under the mouse"
echo "(Press either mouse button to exit)\n"

while $row < $max_row
  echo "qwertyuiopasdfghjklzxcvbnm,./1234567890" nl-
endwhile

while $mouse_but == 0
  echo nl- "\033H" ~ %getscr $mouse_row $mouse_col
endwhile
echo '\f' nl-

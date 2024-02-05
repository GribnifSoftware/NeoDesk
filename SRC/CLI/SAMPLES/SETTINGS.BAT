echo "This batch file will create a file called NEOAUTO.BAT"
echo "which contains all of the current settings. First,"
echo "however, you can easily change all of your CLI settings"
echo "by using the following menu:\n"

set cursmode OVERWRITE          /* Default cursor state */
set cursblnk b+

top;
menu
  "1. Font size"
  "2. Window size and position"
  "3. Aliases"
  "4. Prompt"
  "5. Cursor"
  "6. Misc. variables"
  " "
  "7. Quit"
  1 font
  2 wind
  3 aliases
  4 prompt
  5 curs
  6 misc
  7 end
menu
cls; echo '\f'
     goto top

font;
  echo '\f'
  for
    echo "This is font #$font"'.\n'
    echo "Press any key to go to the next font,"
    echo "or Esc to return to the main menu."
    getch c
    if "$c" == "\033"
      goto cls;
    endif
    if $font == 2
      font 0
    else
      font $font + 1
    endif
  endfor

wind;
  echo '\f'
  for
    echo "\fWindow size and position: $window \n"
    echo "Use Arrow keys to move window."
    echo "Use Shift-Arrow keys to increase/decrease window size."
    echo "Press Esc to return to the main menu."
    getch c scan
    set i 1             /* extract window elements into w1...w4 */
    for j in "$window"
      set w$i $j
      set i $i + 1
    endfor
    if "$scan" == "\x48"        /* up arrow */
      if "$c" == "\x38"         /* Shift-up */
        set w4 $w4 - $char_h
      else
        set w2 $w2 - $char_h
      endif
    elseif "$scan" == "\x50"    /* down arrow */
      if "$c" == "\x32"         /* Shift-down */
        set w4 $w4 + $char_h
      else
        set w2 $w2 + $char_h
      endif
    elseif "$scan" == "\x4b"    /* left arrow */
      if "$c" == "\x34"         /* Shift-left */
        set w3 $w3 - $char_w
      else
        set w1 $w1 - $char_w
      endif
    elseif "$scan" == "\x4d"    /* right arrow */
      if "$c" == "\x36"         /* Shift-right */
        set w3 $w3 + $char_w
      else
        set w1 $w1 + $char_w
      endif
    elseif "$c" == "\033"
      goto cls;
    endif
    window $w1 $w2 $w3 $w4
  endfor

aliases;
  for
    echo "\fCurrent aliases:"
    set i ""
    alias $> i
    if "$i" == ""
      echo <none>
    else
      alias
    endif
    echo "\nPress A to Add, D to Delete, or Q to Quit" nl-
    getch i
    set i %upcase "$i"
    if "$i" == "A"
      echo "\nAlias name: " nl-
      getstr i
      if "$i" != ""
        echo "Value:      " nl-
        getstr j
        if "$j" != ""
          alias $i "$j" i+
        endif
      endif
    elseif "$i" == "D"
      echo "\nName of alias to delete or * for all: " nl-
      getstr i
      if "$i" != ""
        unalias $i
      endif
    elseif "$i" == "Q"
      goto cls;
    endif
  endfor

prompt;
  echo "\fCurrent prompt string:  $prompt\n"
  echo "New prompt string (or <cr> to cancel): " nl-
  getstr i
  if "$i" != ""
    set prompt "$i"
  endif
  goto cls;

curs;
  echo "\fThe cursor is currently set to $cursmode mode."
  echo "It will " nl-
  if $cursblnk == b-
    echo "not " nl-
  endif
  echo "blink.\n"
  echo "Set the cursor to Insert or Overwrite mode? (I/o) " nl-
  getch i
  set cursmode INSERT
  if %upcase "$i" == O
    set cursmode OVERWRITE
  endif
  echo "\nSet the cursor to Blink? (Y/n) " nl-
  getch i
  set cursblnk b+
  if %upcase "$i" == N
    set cursblnk b-
  endif
  goto cls;

misc;
  menu
    "\fSelect the variable to change:"
    "1. \$PATH =     $path"
    "2. \$SUFF =     $suff"
    "3. \$UPDT_NEO = $updt_neo"
    "4. \$USE_ARGV = $use_argv"
    "5. \$WC_EXPAND= $wc_expand"
    "6. \$TOS_WIND = $tos_wind"
    " "
    "7. Quit"
    1 mpth
    2 msuff
    3 mupdt
    4 margv
    5 mexp
    6 mtos
    7 cls
  menu

  mpth;  set i path
         call gets
         goto misc
  msuff; set i suff
         call gets
         goto misc
  mupdt; set i updt_neo
         call get_tf
         goto misc
  margv; set i use_argv
         call get_tf
         goto misc
  mexp;  set i wc_expand
         call get_tf
         goto misc
  mtos;  set i tos_wind
         call get_tf
         goto misc

end;
  echo "\fWrite settings to NEOAUTO.BAT? (Y/n) " nl-
  getch i
  if %upcase "$i" != "N"
    echo "\nSelect the batch file to write:"
    select "\\*.BAT" "NEOAUTO.BAT" "Batch file name:"
    if $item != "FALSE"
      if %exists $item
        echo "$item exists. Overwrite? (Y/n) " nl-
        getch i
        if %upcase "$i" == "N"
          goto top
        endif
      endif
      mouse shape bee
      set .output $item
      echo ";* NeoDesk CLI settings batch file created by settings.bat"
      echo ";*"
      echo ";* Font number"
      echo "font $font"
      echo ";* Window size/position"
      echo "window $window"
      echo ";* Aliases"
      echo "unalias *"
      alias -> settings.tmp
      set .input settings.tmp
      while %hasch
        getstr i
        echo "alias" %left "$i" 8 ' ' nl-  /* send command and name */
        if %mid "$i" 10 1 != ' '           /* is there a key code? */
          set j "k+" %mid "$i" 10 13
          call outj                        /* send it */
          echo nl- " "
        endif
        echo %mid "$i" 24 999              /* send value */
      endwhile
      set .input con:
      rm settings.tmp
      echo ";* Cursor"
      echo cursor $cursmode $cursblnk
      echo ";* Prompt string"
      set i prompt
      call outstr
      echo ";* Miscellaneous variables"
      set i path
      call outstr
      set i suff
      call outstr
      echo set wc_expand $wc_expand
      echo set use_argv  $use_argv
      echo set updt_neo  $updt_neo
      echo set tos_wind  $tos_wind
      echo exit
      set .output con:                  /* close file */
      mouse shape arrow
    endif
  endif
exit

outstr;         /* Take variable named $i and expand its value */
  echo set $i "'" nl-
  set j %expand $i      /* get variable contents */
  call outj2            /* output it */
  echo "'"
  return;

outj;           /* output the contents of $j, expanding \'s */
  while %hasch $< j
    getch c $< j        /* get first char in contents */
    if "$c" == "\\"     /* make sure \'s won't cause problems */
      echo nl- "\\"     /* this becomes \ in the string */
    else
      echo nl- "$c"
    endif
  endwhile
  return

outj2;          /* output the contents of $j, expanding \'s to \\'s */
  while %hasch $< j
    getch c $< j        /* get first char in contents */
    if "$c" == "\\"     /* make sure \'s won't cause problems */
      echo nl- "\\\\"   /* this becomes \\ in the string */
    else
      echo nl- "$c"
    endif
  endwhile
  return

gets;           /* get a string response */
  echo "New value for \$$i"":"
  set j %expand $i
  getstr new "$j"
  set $i "$new"
  return

get_tf;         /* get a true/false value */
  echo "Press T to set \$$i to TRUE or F for FALSE: " nl-
  getch j
  if %upcase "$j" == "T"
    set $i TRUE
  else
    set $i FALSE
  endif
  return


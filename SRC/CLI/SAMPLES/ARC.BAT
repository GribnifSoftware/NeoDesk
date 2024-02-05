; Extract ARC and LZH files, creating a folder for the whole archive.
;
; Change "arc" to point to your arc.ttp, "lzh" for your lharc.ttp, and
; "arcdir" for the place to look for arc files. "arcdir" must end with \
;
set arc e:\arc\arc.ttp
set lzh e:\arc\lharc.ttp
set arcdir e:\arc\

mouse off
; Generate a list of all ARC and LZH files
ls $arcdir ~ *.{arc,lzh} v- -> arc.tmp
echo nl- '\033E'

; Copy a screen-full of data from the file
dup $max_row + 1 <- arc.tmp
mouse on

; Now wait for a mouse press
; Use SET in case the mouse button state changes before the IF is reached
repeat
  set i $mouse_but
until $i != 0

; Only if the left mouse button is used
if $i == 1
  set i $mouse_row
  set .input arc.tmp
  ; Skip some lines in the file
  dup $i -> null:
  ; And get the next line, which is the file that was selected
  getstr i
  set .input con:
  if "$i" != ""
    ; The name of the folder to create
    set dir $arcdir ~ %name $i
    ; Extract to the folder by default
    set s "$dir\\*.*"
    if %not %exists $dir
      ; Create it if it does not already exist
      mkdir $dir
    elseif %not %isfold $dir
      ; If the name is already used by a file, extract to arcdir
      set s $arcdir"*.*"
    endif
    set i $arcdir$i
    if %extn $i == .ARC
      $arc x $i $s
    else
      $lzh x $i $s
    endif
    echo "Delete archive?"
    getch s
    if %upcase "$s" == 'Y'
      rm $i
    endif
  endif
endif
rm arc.tmp


; Pop-Up Menu creating batch file
; This batch file creates nifty pop-up menus which will run programs.
; ver 1.0

unset *
set old_wind $window
set old_path $cwd
window $max_wind

; welcome user
echo "This batch file will create automatic 'Pop-Up Menu' batch files"
echo "which you can use to select and run any program.  Feel free to"
echo "edit and customize this batch file.\n"
echo "Enter up to 7 programs to be entered in the menu.  You can then"
echo "give each program a special 20 character name.\n"
pause
; ask for menu filename and path
selmenu;
echo "\fAt the Item Selector enter the filename and path of the batch file"
echo "menu you wish to create:\n"
pause
select "\*.*" "" "Enter batch file name & path:"
; check for CANCEL button and if the user bothered to return anything
if $item == 'FALSE'
    set junk %alert "[3][|You selected CANCEL|do you want to quit?][Try Again|Quit]"
    if $junk == '1'
        goto selmenu
    endif
    goto quit
elseif %fname $item == ""
    set junk %alert "[3][|You didn't choose anything!][Try Again|Quit]"
    if $junk == '1'
        goto selmenu
    endif
    goto quit
endif
; these lines make sure we are dealing with a file ending in ".bat" even if
; the user tried choosing another file ending
set usemenu %drive $item
set usemenu $usemenu ~ %path $item
set usemenu $usemenu ~ %name $item
set usemenu $usemenu ~ ".BAT"
; see if file already exists
if %exists $usemenu
    set junk %alert "[3][|That file already exists!|Overwrite it?][Overwrite|Try Again]"
    if $junk == '2'
        goto selmenu
    endif
endif
; create file header
echo "\f\033p Please wait, creating file... \033q"
set .output $usemenu
echo "; " %fname $usemenu " pop-up menu file,"
echo "; created by the 'Pop-Up Menu Batch File Creator' version 1.0"
echo "; created on" %sysdate %systime "\r\n"
echo "; first clear all variables"
echo "unset *\r\nset max_len 25\r\nset max_height 12\r\n; get old window size"
echo "set old_wind \$window\r\n; get screen coordinates\r\nset junk 1"
echo 'set old_path $cwd'
echo 'for element in "$max_wind"'
echo '   set wind$junk $element'
echo '   set junk $junk + 1'
echo "endfor\r\nunset element"
echo "; center menu in screen (ack!)"
echo 'set wind1 $wind3 DIV 2'
echo 'set wind3 $wind1'
echo 'set junk $char_w MULT $max_len'
echo 'set wind3 $junk'
echo 'set junk $junk DIV 2'
echo 'set wind1 $wind1 - $junk'
echo 'set wind2 $wind4 DIV 2'
echo 'set wind4 $wind2'
echo 'set junk $char_h MULT $max_height'
echo 'set wind4 $junk'
echo 'set junk $junk DIV 2'
echo 'set wind2 $wind2 - $junk'
echo "if \$wind3 < 184\r\n   set wind3 184\r\nendif"
echo "if \$wind4 < 48\r\n    set wind4 48\r\nendif"
echo 'window $wind1 $wind2 $wind3 $wind4'
echo "\r\n; main menu (finally!)\r\ntop;\r\nmenu"
set .output con:
echo "\r\nEnter the name this Pop-Up Menu, up to 20 characters:\r\n"
echo nl- "\033j________________]\033k"
getstr junk
echo '   "\\f   ' $junk ' \\n"' ->> $usemenu
echo "; subroutines to run programs\r\n" -> battemp.000
set files 1
loop;
set choice %alert "[1][|Choose a program for|the Pop-Up Menu!][Ok|No More]"
while $choice == '1' && $files < 8
    echo "\fChoose a program for the Pop-Up menu:"
    select "\*.*" "" "Choose a program:"
    if $item != 'FALSE' && %exists $item
        set junk %drive $item
        set junk $junk ~ %path $item
        set temp %name $item
        echo $temp";" ->> battemp.000
        echo "   cd $junk" ->> battemp.000
        echo "   mouse reset" ->> battemp.000
        echo "  " %fname $item ->> battemp.000
        echo "   goto top\r\n" ->> battemp.000
        echo "\fEnter the menu name for $item , up to 20 characters:\n"
        echo nl- "\033j__________________]\033k"
        getstr junk
        echo "   \" " $files": $junk \" " ->> $usemenu
        echo "   $files" %name $item ->> $usemenu
    else
        goto continue
    endif
    set files $files + 1
endwhile
continue;
if $files == '1'
    null %alert "[3][|You didn't choose any|files! Oh well...][Quit]"
    rm battemp.000
    rm $usemenu
    goto quit
endif
echo '   "\n[Esc] or [Q] to quit"' ->> $usemenu
echo "   '\\033' quit" ->> $usemenu
echo "   'q' quit" ->> $usemenu
echo "   'Q' quit" ->> $usemenu
echo "menu\r\n" ->> $usemenu
echo "quit;" ->> $usemenu
echo '   window $old_wind' ->> $usemenu
echo '   cd $old_path' ->> $usemenu
echo "   exit\r\n" ->> $usemenu
cat battemp.000 ->> $usemenu
rm battemp.000
kick %drive $usemenu
null %alert "[1][|All done!|Your new menu is|finished.][Great!]"
quit;
window $old_wind
cd $old_path
exit

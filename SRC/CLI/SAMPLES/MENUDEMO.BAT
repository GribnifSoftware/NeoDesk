;                      NeoDesk CLI Menu demo batch file
;
; Clear out all user-defined variables so there will be room
        unset *
        
; Get rid of the mouse so it doesn't get in the way
        mouse off

; Get the current window position for later.
        set old_wind $window
; And set the window size according to the font dimensions
        window 0 0 $char_w MULT 46 $char_h MULT 9

top;    menu
;
; Note how the second line of the menu uses ' to start and end the line
; so that " can be used internally. Also, notice the way a comma can be
; used as a separator, just like a space (like in "1,l1".) The first \f
; does a formfeed, which clears the window.
;
          "\f\n"
          '       NeoDesk CLI \033pMenu\033q Command Demo'
          ""
          "    Press 1:  Get a disk directory"
          '          2:  "Touch" one or more files'
          "          3:  Execute any command"
          "   Esc or Q:  Quit this menu"
          "      Space:  Re-display this list"
          1,list
          2 touch 
          3 run
; Three equivalents for Quit
          '\033' quit
          'q' quit
          'Q' quit
          ' ' top
        menu

list;   echo "\fEnter the location to get the disk directory:"
        getstr pth
        ls $pth
        goto bottom

touch;  echo "\fEnter the name of the file(s) to touch:"
        getstr pth
        touch $pth
        goto bottom

run;    echo "\fEnter the command to run: \033Z"
        getstr cmd
        $cmd

bottom; pause
        goto top

quit;   exit

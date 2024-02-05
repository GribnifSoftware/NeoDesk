; A digital clock that demonstrates some of the cursor controls.
; \033j saves the cursor position so it can be restored later by \033k.
; \033Z prevents ECHO from putting a carriage return at the end of the line.
;
        mouse off
        echo "Press any key to stop the clock\n\nThe time is: \033j\033Z"
        while %not %hasch
          echo "\033Z\033k" %systime
        endwhile
        
        getch a
        unset a
        mouse on
        

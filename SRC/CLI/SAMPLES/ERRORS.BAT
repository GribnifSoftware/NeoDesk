; Demonstrate ONERR and ONQUIT

    onerr errsub
    onquit quitsub
    for                                         /* start an infinite loop */
      echo "This is an infinite loop! Press Control-C!"
    endfor
                                                /* never leaves REPEAT loop */
    quitsub;
      echo "Control-C pressed. Continue batch file?"
      getch ch
      if "$ch" == "y" || "$ch" == "Y"
        return                                  /* return to infinite loop */
      else
        echo "Let's generate an error..."
        echo 1 DIV 0                            /* can't DIV by zero */
        exit                                    /* what we'll do when we get */
      endif                                     /* control back from errsub */
    return
    
    errsub;
      echo "Encountered error $errmsg (Error number $errnum""). Continue?"
      getch ch
      if "$ch" == "y"
        pop
        return
      endif
    return


mouse off
echo "\033E"
echo "      \033pA tiny calculator\033q"
echo "Use numbers, +, -, *, /, and ="
echo "   Clr/Home clears display"
echo "     Press <Esc> to quit"
unset *

;current value being entered
set display 0
;if TRUE, then overwrite what was last displayed
set final FALSE
;if TRUE, then it's ok for the user to enter an operator (+-*/)
set op_ok TRUE

;display the initial value (0)
echo "\n\t\033j$display"
for
  ;stuff within this loop is executed most often, so keep it brief using CALL
  getch key scan
  ;if the last button was an operator or =
  if $final
    set display 0
    set final FALSE
    ;you can't enter two ops in a row
    set op_ok FALSE
  endif
  if "$scan" == "\x47"
    call clrhome
  elseif "$key" >= '0' && "$key" <= '9'
    ;digit keys
    ;multiply the current display value by ten and add the new key
    set display $display MULT 10 + $key
    echo "\033k\033K$display  $op"
    set op_ok TRUE
  elseif "$key" == '\033'
    ;process the Esc key
    exit
  elseif $op_ok
    call operate
  endif
endfor

clrhome;
  ;process the Clr/Home key
  set display 0
  ;you can't unset a variable that hasn't been set, so check
  if "$op" != ""
    unset op
    unset accum
  endif
  set op_ok FALSE
  ;restore cursor position, clear to end of line, display new result (0)
  echo "\033k\033K$display"
  return

operate;
  ;only do this next bit if an operator is expected
  if "$key" == '*'
    set newop "MULT"
    goto operator
  elseif "$key" == '/'
    set newop "DIV"
    goto operator
  elseif "$key" == '+' || "$key" == '-'
    set newop "$key"
operator;
    if "$op" != ""
      ;if there is something in the accumulator, combine it using last op
      set accum $accum $op $display
    else
      ;otherwise simply copy the display into the accumulator
      set accum $display
    endif
    ;display the new value, with the operator type
    echo "\033k\033K$accum  $newop"
    set op "$newop"
    set final TRUE
  elseif "$key" == '=' && "$op" != ""
    ;calculate new value, unset op (for next op pressed), and display
    set display $accum $op $display
    unset op
    echo "\033k\033K$display"
    unset accum
    set final TRUE
  endif
  return


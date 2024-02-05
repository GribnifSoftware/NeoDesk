; A guessing game to demonstrate functions

; Set the random number generator seed to the number of seconds multiplied
; by the number of minutes in the time. 
set seed %right %systime 2 + %mid %systime 4 2

top;

echo "\nI'm thinking of a number from 0 to 99. Guess what it is."
call random

; number from 0 to 100  =  seed - ((seed/100)*100)
set i $seed DIV 100 MULT 100
set number $seed - $i

for
  echo nl- "Your guess? "
  getstr guess
  if "$guess" < $number
    echo "\nToo low. Try again."
  elseif "$guess" > $number
    echo "\nToo high. Try again."
  else
    echo "\n\aYou got it!"
    echo nl- "\nPlay again? "
    getstr i
    if %upcase "$i" == 'N'
      exit
    endif
    goto top
  endif
endfor

; Pseudo-random number generator. This works by taking advantage of the
; fact that multiplying large numbers produces an overflow. It will be
; a long time before this value ever repeats.
random;
  set seed $seed MULT 826354021 + 1
return


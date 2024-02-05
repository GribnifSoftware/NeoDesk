/* NeoDesk 3.03 by Dan Wilga
   Copyright ½ 1992, Gribnif Software.
   All Rights Reserved.
*/

; gtext( x, y, string, fontptr, width, height )
;        8 10    12       16      20     22
;
;  d0  scratch, mask                    a0  string pointer
;  d1  low x clipping offset            a1  font pointer
;  d2  font data                        a2  screen offset
;  d3  low y clipping offset            a3  temporary screen offset
;  d4  height of line
;  d5  width of data; height counter for current char
;  d6  starting x
;  d7  scratch
;
;  change scan_off to int
;
physbase equ 0x44E
          .export fasttext
          .export ft_clip
          .export scan_off
          .export ft_scr_ptr
          .export ft_planes

ft_clip:  ds.w  4
scan_off: ds.w  1
ft_scr_ptr:
          ds.l  1
ft_planes:
          ds.w  1

ftregs reg d1-d7/a2-a3

fasttext:
          link      a6, #0
          movem.l   #ftregs, -(a7); save old regs
          movea.l   12(a6), a0          ; string pointer into a0
          move.w    8(a6), d0
          cmp.w     ft_clip+4(pc), d0   ; is the starting x > max x-clip?
          bgt.b     end                 ; if so, quit
          move.w    10(a6), d0
          cmp.w     ft_clip+6(pc), d0   ; is the y greater than the max clip-y?
          bgt.b     end                 ; if so, don't bother showing any text
          move.w    10(a6), d7
          move.w    22(a6), d4          ; calculate the true height
          subq.w    #1, d4
          add.w     d4, d7              ; add to d7 to find the lowest scan line
          sub.w     ft_clip+6(pc), d7   ; subtract max y-clip
          ble.b     ft2
          sub.w     d7, d4              ; subtract that amount from the counter
ft2:      move.w    ft_clip+2(pc), d3   ; minimum clipping y
          sub.w     10(a6), d3          ; subtract the starting y-coord
          bge.b     ft0                 ; does the whole character fit?
          clr.w     d3                  ; if so, clear the vertical offset
          bra.b     ft1
ft0:      cmp.w     22(a6), d3          ; is this >= the char height?
          bge.b     end                 ; if so, quit
          move.w    ft_clip+2(pc), 10(a6) ; otherwise set y-coord to the min clip
ft1:      sub.w     d3, d4              ; subtract the vertical offset
          move.w    10(a6), d7          ; calculate intitial offset to screen
          mulu      scan_off(pc), d7
          movea.l   ft_scr_ptr(pc), a2
          adda.l    d7, a2              ; screen offset in a2--should be long
next_chr: clr.w     d0
          moveq     #0, d6
          move.b    (a0)+, d0           ; next character to be printed
          beq.b     end                 ; is this a NUL?
          move.w    8(a6), d7           ; offset into the scan line (x)
          move.w    d7, d6              ;   and also in d6 for later
          cmp.w     ft_clip+4(pc), d7   ; compare with max x-clip
          ble.b     noend               ; get out if it's too far
end:      movem.l   (a7)+, #ftregs      ; restore old regs
          unlk      a6
          rts
noend:    movea.l   a2, a3              ; temporary offset in a3
          mulu      22(a6), d0          ; multiply by the font height
          movea.l   16(a6), a1          ; font pointer into a1
          adda.w    d0, a1              ; a1 points to the font data for char
          adda.w    d3, a1              ; add the y-clipping offset
          move.w    ft_clip(pc), d1     ; minimum clipping x
          move.w    20(a6), d5          ; move width into d5
          sub.w     d7, d1              ; subtract the starting x-coord
          bge.b     ft4                 ; does the whole character fit?
          clr.w     d1                  ; if so, clear the horizontal offset
          bra.b     ft5
ft4:      sub.w     d1, d5              ; sub low x; is this number > width?
          blt       inc_x               ; if so, get next char
ft5:    ;  sub      d1, d7
          asr.w     #4, d7              ; figure out what byte starts the word
          muls      ft_planes(pc), d7   ; adjust according to # of bitplanes
          add       d7, d7
          adda.w    d7, a3              ; word to be modified
;          sub      d1, d6
          move.w    d6, d7              ; starting x into d7
          andi.w    #$F, d6             ; bits to shift the data in the word
          add.w     20(a6), d7          ; add the width of the character - 1
          subq.w    #1, d7
          sub.w     ft_clip+4(pc), d7   ; subtract the max x-clip
          blt.b     ft8
          sub.w     d7, d5              ; positive number: subtract high offset
ft8:      moveq.l   #-1, d0             ; create a mask for the existing data
          lsr.l     d5, d0              ;   make a gap the width of the data
          ror.l     d6, d0              ;   carry is always set, fill with 1's
          ror.l     d1, d0              ;   rotate by the clipping offset
          move.w    d4, d5              ; temporary height counter in d5
;          btst      #15, d0             ; does the data cross the word bounds?
;          beq.b     loop                ; yes, modify adjacent data also
          cmpi      #$FFFF, d0
          bne.b     loop1
          swap      d0                  ; no, swap the mask halves
          move      d0, d1
          not       d1
          
loop0:    move.b    (a1)+, d2           ; get font data
          lsl.w     #8, d2              ; shift data into MSB
          lsr.w     d6, d2              ; shift back by x-offset
          and.w     d1, d2              ; and mask-out unneeded data
          
doand:    and.w     d0, (a3)            ; mask first word
          or.w      d2, (a3)            ; and or-in the data
;         cmpi.w    #1, ft_planes       ; is there only 1 bitplane?
;         beq.b     inc_scan0           ; yes, branch
          
          move.w    ft_planes(pc), d7   ; get # of planes
          subq.w    #2, d7              ; -2
          blt.b     inc_scan0
          move.l    a3, -(a7)           ; save a3
          addq      #2, a3
nexpl0:   and.w     d0, (a3)            ; mask the next plane
          or.w      d2, (a3)+           ; and or-in the data
          dbra      d7, nexpl0
          movea.l   (a7)+, a3
          
inc_scan0:adda.w    scan_off(pc), a3    ; increment scan line
          dbra      d5, loop0           ; and loop
          bra       inc_x               ; go and increment the x-offset

loop1:    move.l    d0, d1
          not.l     d1
loop:     move.b    (a1)+, d2           ; d2 holds the font data
          swap      d2                  ; shift the font data into the MSW
          lsl.l     #8, d2              ;   and then into the MSB
          lsr.l     d6, d2              ; shift the font data appropriately
          and.l     d1, d2              ; mask-out unneeded data when clipping
;         cmpi.w    #1, ft_planes       ; test the # of bitplanes
;         beq       mono                ; one means mono monitor
;         cmpi.w    #2, ft_planes
;         bne.b     other               ; some other type of screen
          move.w    ft_planes(pc), d7
          subq      #1, d7
          beq.b     mono
          subq      #1, d7
          bne.b     other

          swap      d0                  ; medium rez: swap d0's halves
          swap      d2                  ; and swap d2's halves
          and.w     d0, (a3)            ; mask first bitplane
          or.w      d2, (a3)+           ; or-in data
          and.w     d0, (a3)            ; mask second bitplane
          or.w      d2, (a3)+           ; or-in data
          swap      d0                  ; unswap d0
          swap      d2                  ; unswap d2
          and.w     d0, (a3)            ; mask first bitplane, adjacent
          or.w      d2, (a3)+           ; or-in data
          and.w     d0, (a3)            ; mask second bitplane, adjacent
          or.w      d2, (a3)            ; or-in data
          subq      #6, a3              ; subtract 6 so that count is right
          bra.b     inc_scan            ; and go and add the right number
          
other: ;  move.w    ft_planes(pc), d7   ; get # of planes
          addq.w    #1, d7              ; -2+1
          move.l    a3, -(a7)           ; save a3
          move.w    d7, -(a7)           ; save d7
          swap      d2                  ; swap d2's halves
          swap      d0                  ; and swap d0's halves
next_pl:  and.w     d0, (a3)            ; mask a bitplane of first data
          or.w      d2, (a3)+           ; or-in data
          dbra      d7, next_pl         ; next plane
          move.w    (a7)+, d7           ; restore d7
          swap      d0                  ; swap d0's halves
          swap      d2                  ; and swap d2's halves
next_pl2: and.w     d0, (a3)            ; mask a bitplane of adjacent data
          or.w      d2, (a3)+           ; or-in data
          dbra      d7, next_pl2        ; next plane
other_ex: movea.l   (a7)+, a3           ; restore a3
          bra.b     inc_scan            ; and go and add the right number

mono:     and.l     d0, (a3)            ; mask the stuff we don't want changed
          or.l      d2, (a3)            ; or-in the new character data

inc_scan: adda.w    scan_off(pc), a3    ; increment scan line

          dbra      d5, loop            ; continue until we reach the y-limit
inc_x:    move.w    20(a6), d7          ; increment x-offset by font width
          add.w     d7, 8(a6)
          bra       next_chr            ; get the next character in the string

/***************************************************************/
.export la_init
.export linea0
.export lineaa
.export linea9
.export linea5
.export lineae
.export linea3
.export linea8
.export linea4
.export linea1
.export _reset_mouse

ra2d2 reg a2/d2
  
la_init:        .ds.l   4

regs reg d0/a0-a2
linea0:         move.l  a2, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                move.l  (sp)+, a2
                rts

linea9:		moveq	#1, d0                
mouse_on:       move.l  a2, -(sp)
		move.l	la_init+4(pc), a0
		move.l	8(a0), a0
		move	d0, (a0)
                .dc.w   $a009
                move.l  (sp)+, a2
                rts
                
;_reset_mouse:	moveq	#0, d0
;		bra.b	mouse_on

lineaa:		move.l  a2, -(sp)
                .dc.w   $a00a
                move.l  (sp)+, a2
                rts

linea5:         move.l  a2, -(sp)
                .dc.w   $a005
                move.l  (sp)+, a2
                rts
                
lineae:         move.l  a2, -(sp)
                .dc.w   $a00e
                move.l  (sp)+, a2
                rts
                
linea3:         move.l  a2, -(sp)
                .dc.w   $a003
                move.l  (sp)+, a2
                rts
                
linea1:         move.l  a2, -(sp)
                .dc.w   $a001
                move.l  (sp)+, a2
                rts
                
linea8:         move.l  a2, -(sp)
                .dc.w   $a008
                move.l  (sp)+, a2
                rts
                
linea4:         move.l  a2, -(sp)
                .dc.w   $a004
                move.l  (sp)+, a2
                rts


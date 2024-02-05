/* NeoDesk 3.01 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/
;
;   gtext( x, string, fontptr, font_height, planes, next_pl, pl_mul )
;          8    10       14         18        20      22       24
;

rest_CD:  lea       TSsave+4(pc), a0
          move.l    #$febf03ce, a1
          move.b    #5, (a1)+
          move.b    (a0), (a1)
          move.b    -(a0), -(a1)
          lea       $3c4-$3ce(a1), a1
          move.b    #4, (a1)+
          move.b    -(a0), (a1)
          move.b    #2, -(a1)
          move.b    -(a0), 1(a1)
          move.b    -(a0), (a1)
          rts

.import is_CD, falc_vid
CD:	  move      20(a6), d0
	  cmpi	    #8, d0
	  blt.b     tcd3
	  bgt.b     tcd4
	  lea	    Etext(pc), a0
	  bra.b	    tcd2
tcd4:  	  lea	    Tmod(pc), a5	;; 003:
	  lea	    Ttext(pc), a0	;;
	  cmpi	    #16, d0		;;
	  ble.b	    tcd8		;; 006: was blt
	  move      -2(a5), (a5)	;; 24-bit
          move.w    8(a6), d7           ;;
	  move	    d7, d5		;; d5=d7*3
	  add	    d5, d5		;;
	  add	    d7, d5		;;
	  bra.b	    tcd3		;;
tcd8:	  move      #$4e71, (a5)	;; 16-bit
tcd2:	  move	    24(a6), d1		;;
	  subq      #1, d1		;;
	  asl	    d1, d5		;; offset *= planes
tcd3:	  move.l    #$febf03c4, a2
	  lea       TSsave(pc), a1
	  move.b    (a2), (a1)+
	  move.b    #2, (a2)+
	  move.b    (a2), (a1)+
	  move.b    #$f, (a2)
	  move.b    #4, -1(a2)
	  move.b    (a2), (a1)+
	  cmpi	    #8, d0
	  bge.b     tcd7
	  andi.b    #$F7, (a2)
	  bra.b     tcd6
tcd7:	  ori.b     #8, (a2)
tcd6:	  lea       $3ce-$3c5(a2), a2
	  move.b    (a2), (a1)+
	  move.b    #5, (a2)+
	  move.b    (a2), (a1)
	  andi.b    #$fc, (a2)
	  lea       rest_CD(pc), a5
no_CD:	  rts

test_cd:
	  lea	    Ntext(pc), a0
          lea	    no_CD(pc), a5
	  move      20(a6), d0
          move.b    is_CD(pc), d1
	  bne       CD
	  move.b    falc_vid(pc), d1
	  beq.b	    no_CD
	  cmpi	    #15, d0
	  blt.b	    no_CD
	  asl	    #4, d5		; 003
	  move      #$4e71, Tmod	; 003 16-bit Falcon: nop
	  lea	    Ttext(pc), a0
	  rts

logbase equ $44E

greg reg d2-d7/a2-a5

          .export gtext
          .import scan_off
          .import strlen
          
gtext:    link      a6, #0
          movem.l   #greg, -(a7)        ; save old regs
          movea.l   10(a6), a0          ; push the string pointer
          jsr       strlen(pc)          ; get the string length
          move      d0, d6              ; string length-1 in d6
          beq       end                 ; empty string
          subq      #1, d6
          
          moveq     #0, d5              ; d5 designates the byte in the word
          btst      #3, 9(a6)           ; check the third bit of the x-coord
          beq.b     gt1
          moveq.l   #1, d5              ; modify the LSB in the screen word
          
gt1:      bsr.b	    test_cd
          movea.l   logbase.w, a2       ; logbase in a2
          cmpi	    #16, 20(a6)		; 003
          bgt.b	    gt24		; 003
          move.w    8(a6), d7           ; offset into the scan line (x)
          lsr.w     #4, d7              ; figure out what byte starts the word
          move      24(a6), d0
          lsl.w     d0, d7              ; * planes * 2
          adda      d7, a2              ; first word to be modified
gt24:     adda      d5, a2              ; add the byte offset into the word
          move.w    18(a6), d2          ; move char height-1 into d2
          subq      #1, d2
          subq      #1, 20(a6)          ; subtract 1 from the # of bitplanes
          movea.l   14(a6), a1          ; temp font pointer in a1
          jsr	    (a0)
end:      jsr       (a5)
          movem.l   (a7)+, #greg        ; restore old regs
          unlk      a6
          rts

; Arbitrary number of bitplanes
Ntext:    movea.l   10(a6), a0          ; string pointer into a0
          move.w    d6, d4              ; string length-1 into d4
          move.w    d5, d3              ; temp byte parity in d3
          movea.l   a2, a3              ; temp screen pointer in a3
          moveq     #0, d0
Nnext_chr:move.b    (a0)+, d0           ; next character to be printed
          movea.l   a3, a4              ; another temp screen pointer
          move.w    20(a6), d1          ; number of bitplanes-1
Nloop:    move.b    $0(a1,d0), (a4)     ; move the font data onto the screen
          addq      #2, a4              ; next bitplane
          dbra      d1, Nloop           ; continue for all the bitplanes
          addq      #1, a3              ; increment temp screen pointer
          bchg      #0, d3              ; change the parity register
          beq.b     Ngt0                ; next byte is odd
          adda      22(a6), a3          ; skip over the other bitplanes
Ngt0:     dbra      d4, Nnext_chr       ; get the next character
          adda      #$100, a1           ; increment font pointer by 256
          adda.l    scan_off(pc), a2    ; add the screen width
          dbra      d2, Ntext           ; copy the whole char height
          rts
          
; 8 bitplanes
Etext:    movea.l   10(a6), a0          ; string pointer into a0
          move.w    d6, d4              ; string length-1 into d4
          movea.l   a2, a3              ; temp screen pointer in a3
          moveq     #0, d0
Enext_chr:move.b    (a0)+, d0           ; next character to be printed
Eloop:    move.b    $0(a1,d0), d3       ; get font data
          moveq     #7, d1              ; number of bits
Ebit:	  add.b     d3, d3		; double it
	  scs	    (a3)+		; set color to 255 if bit was set
	  dbra      d1, Ebit		; repeat 8 times
Egt0:     dbra      d4, Enext_chr       ; get the next character
          adda      #$100, a1           ; increment font pointer by 256
          adda.l    scan_off(pc), a2    ; add the screen width
          dbra      d2, Etext           ; copy the whole char height
          rts
          
; 15/16/24 bit true color
Ttext:    movea.l   10(a6), a0          ; string pointer into a0
          move.w    d6, d4              ; string length-1 into d4
          movea.l   a2, a3              ; temp screen pointer in a3
          moveq     #0, d0
Tnext_chr:move.b    (a0)+, d0           ; next character to be printed
Tloop:    move.b    $0(a1,d0), d3       ; get font data
          moveq     #7, d1              ; number of bits
Tbit:	  add.b     d3, d3		; double it
	  scc	    (a3)+		; set color to black if bit was set
	  scc	    (a3)+		; set color to black if bit was set
Tmod:	  nop				; nop or scc (a3)+ for 24-bit
	  dbra      d1, Tbit		; repeat 8 times
Tgt0:     dbra      d4, Tnext_chr       ; get the next character
          adda      #$100, a1           ; increment font pointer by 256
          adda.l    scan_off(pc), a2    ; add the screen width
          dbra      d2, Ttext           ; copy the whole char height
          rts
          
.export clock_tst
clock_tst:
        move.l  d2, -(sp)
        moveq   #-1, d0
        movea.w #$FC20,a0
        move.b  #9,$1B(a0)
        move.w  #$a05,d2
        dc.w    $588, $5              ; movep.w d2, 5(a0)
        movep.w 5(a0),d1
        and.w   #$F0F,d1
        cmp.w   d2,d1
        bne.b   ct0
        move.b  #1,$01(a0)
        move.b  #8,$1B(a0)
        move.b  #0,$1D(a0)
        clr.l   d0
ct0:    move.l  (sp)+, d2
        rts

/*      
.globl  xor_block_
xor_block_:
        movea.l logbase:s,, a0
        move    4(sp), d0
        moveq   #-1, d1
        subq    #1, d0
0:      eor.l   d1, (a0)+
        dbra    d0, 0b
        rts
*/
.export la_init
.export linea0
  
la_init:        .ds.l   4

regs reg d0/a0-a2
linea0:         movem.l #regs, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                movem.l (sp)+, #regs
                rts

.export set_MSTe
.export set_030
.export cache

cache:	ds.w	1

set_MSTe:
	movea.w	#$8e21, a0
	move.b	(a0), d0
	move.b	d0, d2
	move.w	cache(pc), d1
	bmi.b	sm1
	andi	#$FFFC, d2
	asr	#2, d1
	andi	#3, d1
	or.b	d1, d2
	move.b	d2, (a0)
sm1:	andi	#3, d0
	add	d0, d0
	add	d0, d0
	rts

set_030:
	clr	d0
	movec.l	cacr, d1
	andi	#$101, d1
	cmpi	#$101, d1
	bne.b	s02
	moveq	#2, d0
s02:	move.w	cache(pc), d1
	bmi.b	sx
	btst	#1, d1
	beq.b	s03
	tst	d0
	bne.b	sx
	move.l	#$3919, d2
	bra.b	s01
s03:	tst	d0
	beq.b	sx
	move.l	#$808, d2
s01:	movec.l	d2, cacr
sx:	rts

	.export set_adspd
set_adspd:
	bsr	gcount
sa2:	moveq	#0, d0
	move.b	tspeed(pc), d0
	beq.b	sa1
	moveq	#2, d0
sa1:	move.w	cache(pc), d1
	bmi.b	sax
	btst	#1, d1
	bne.b	sa3
	move.b	has_adspd(pc), d2
	bmi.b	s8t25
	bsr.b	SET8
	rts
s8t25:	bsr.b	SET8T
	rts
sa3:	move.b	has_adspd(pc), d2
	bmi.b	s16t25
	bsr.b	SET16
	rts
s16t25:	bsr.b	SET16T
sax:	rts
	
* set AdSpeed ST to 16 MHz
SET16:
        move.w  #0,$f00000
        rts

* set AdSpeed ST to 8 MHz
SET8:
        move.w  #0,$f10000
        rts

* set T25 to 25 MHz
SET16T:
        move.w  #0,$D00000
        rts

* set T25 to 8 MHz
SET8T:
        move.w  #0,$D10000
        rts

* test for existence of AdSpeed
* out: d0= 1 if found, 0 if not
        .globl is_ads
        .even
is_ads:
;        bsr     super                   ;must be in supervisor mode
        bsr     gcount                  ;get count of divides in tick of

        move.w  sr,d5                  ;save some stuff
        ori	#$700, sr
        move.l  a7,a5
        move.l  8.w,old8
        move.l  #unpatcha,8.w             ;patch address error vector

        move.b  tspeed(pc), d0
        bne.b   t16

        bsr.b   SET8                   ;to see if AdSpeed ST exists, try 
        bra.b   setexa                 ;setting to current CPU speed

t16:
        bsr.b   SET16

setexa:
        move.b  #1, has_adspd          ;if no error, got here and set this flag
        bra.b	unpatcht

unpatcha:				; definitely no AdSpeed
	move.l	#unpatcht,8.w		; try for T25
        move.b  tspeed(pc), d0
        bne.b   t16t

        bsr.b   SET8T                  ;to see if T25 exists, try 
        bra.b   setext                 ;setting to current CPU speed

t16t:
        bsr.b   SET16T

setext:
        move.b  #-1, has_adspd         ;if no error, got here and set this flag

unpatcht:
        move.l  a5,a7                  ;if an error, returned directly to
        move.w  d5,sr                  ;here
        move.l  old8(pc),8.w 
;        bsr     xsuper                 ;leave supervisor mode
        moveq   #0, d0
        move.b  has_adspd(pc),d0              ;exit with value in d0
        rts

	.export has_adspd
has_adspd:
	ds.b	1
tspeed:	ds.b	1
old8:	ds.l	1
junk:   dc.l  $80000000
threshold  equ 550

gcount:
        moveq.l #0, d0                  ;d0 = counter
        move.l  $4ba.w,d2             ;d2 = start tick

tick:
        cmp.l  $4ba.w,d2              ;wait for a new tick
        beq.b  tick

        move.l $4ba.w,d2

gloop:
        move.l junk,d1              ;do divides until start of next tick
        divu   #2,d1
        addq.l #1,d0
        move.l $4ba,d1
        cmp.l  d2,d1
        beq.b  gloop

        cmp.l   #threshold,d0           ;200 Hz counter.  Determine present
        blt.b   is8                     ;system CPU speed

        st      tspeed

is8:	rts        

.bss
TSsave:	ds.b	5
.text

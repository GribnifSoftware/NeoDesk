/* NeoDesk 3.01 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/

.export magx_single, magx_unsingle

.bss
singled:ds.w	1
.text

magx_single:
	move	#1, (a0)+
	movem.l	d3-d7/a2-a7, (a0)
	tas	singled
	bne.b	ms1
	move	#0, -(sp)
	pea	-1.w
	move	#49, -(sp)		; Ptermres
	trap	#1
ms1:	clr	-(sp)			; Pterm0 second copy
	trap	#1
	
magx_unsingle:
	clr	(a0)+
	movem.l	(a0), d3-d7/a2-a7
	move.l	#256, -(sp)
	pea	(a1)
	clr	-(sp)
	move	#74, -(sp)		; Mshrink basepage
;;	move.l	44(a1), -(sp)
;;	move	#73, -(sp)		; Mfree env
;;	trap	#1
;;	addq	#6, sp
	trap	#1			; now Mshrink
	lea	12(sp), sp
	rts

.export set_CaTT
.export set_MSTe
.export set_030
.export cache
.import CaTT_GIOS, cattmodes, cattnum

_CaTT_g_cmode   equ         8

.bss
cache:	ds.w	1
.text

set_CaTT:
        move.l  #'CaTT',d0
        moveq   #_CaTT_g_cmode,d1
        move.l  CaTT_GIOS(pc),a0
        jsr     (a0)
	lea	cattmodes(pc), a0
        moveq	#4, d1
sc2:    cmp.b	0(a0,d1), d0
        dbeq	d1, sc2
        move	d1, d0
        addq	#1, d0		; current mode= (array_index+1)<<4
        lsl	#4, d0
	move.w	cache(pc), d1
	bmi.b	sc1
	rol	#4, d1
	andi	#7, d1
	beq.b	sc1
	tst.b	-1(a0,d1)
	beq.b	sc1
	move	d0, -(sp)
	lea	cattnum(pc), a0
	move.b	-1(a0,d1), d1
        move.l  #'CaTT',d0
        move.l  CaTT_GIOS(pc),a0
        jsr     (a0)
        move	(sp)+, d0
sc1:	rts

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
.bss
has_adspd:
	ds.b	1
tspeed:	ds.b	1
old8:	ds.l	1
.text
junk:   dc.l  $80000000
threshold  equ 550

gcount:
        moveq.l #0, d0                  ;d0 = counter
        move.l  $4ba.w,d2             ;d2 = start tick

tick:
        cmp.l  $4ba.w,d2              ;wait for a new tick
        beq.b  tick

        move.l $4ba.w,d2

loop:
        move.l junk,d1              ;do divides until start of next tick
        divu   #2,d1
        addq.l #1,d0
        move.l $4ba,d1
        cmp.l  d2,d1
        beq.b  loop

        cmp.l   #threshold,d0           ;200 Hz counter.  Determine present
        blt.b   is8                     ;system CPU speed

        st      tspeed

is8:	rts        

/*        .export linea0

la_init:        .ds.l   4
        .export la_init

regs reg d0/a0-a2
linea0:         movem.l #regs, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                movem.l (sp)+, #regs
                rts
  */              

/*        .export new_but
        .export old_but
        .export lineaa
        .export linea9
        .export linea5
        .export mouse_but

new_but:
        move.b  d0, mouse_but
;       beq.b   b_end
;       moveq   #1, d0
b_end:  move.l  old_but(pc), -(sp)
        rts
old_but: 
        ds.l    1
mouse_but:
        ds.b    1
.align 16  */

/******        
linea9:         movem.l #regs, -(sp)
                .dc.w   $a009
                movem.l (sp)+, #regs
                rts
                
lineaa:         movem.l #regs, -(sp)
                .dc.w   $a00a
                movem.l (sp)+, #regs
                rts
*******/
        .export memclr

memclr: moveq.l #4, d1
mc2:    cmp.l   d1, d0
        blo.b   mc1
        clr.l   (a0)+
        subq.l  #4, d0
        bra.b   mc2
mc1:    tst     d0
mc3:    beq.b   mc4
        clr.b   (a0)+
        subq    #1, d0
        bra.b   mc3
mc4:    rts

/********
.export Pexec
.import save_a7
Pexec:	move.l	a2, -(a7)
        move.l  8(a7), -(a7)
        move.l  a1, -(a7)
        move.l  a0, -(a7)
        move.w  d0, -(a7)
        move.w  #$4b, -(a7)
;       move.l  a7, d0
;       sub.l   #100, d0
;       bsr     save_a7
        trap    #1
        lea     $10(a7), a7
        move.l	(sp)+, a2
        rts
*******/

.export get_bp_addr, Malloc, Mfree, Mshrink
.import _BasPag, has_mint
_sysbase equ $4F2
get_bp_addr:
	move.b	has_mint(pc), d0
	bne.b	l3
	lea	bp_addr(pc), a1
	move.l	$602c, (a1)
        movea.l _sysbase.w, a0
        move.w  2(a0), d0
        cmpi    #$100, d0
        bls.b   l2              ; don't set it again if TOS 1.0 or older
        move.l  $28(a0), (a1)
        rts
l2:     move    $1c(a0), d0
        lsr     #1, d0
        cmpi    #4, d0
        bne.b   l3
        move.l  #$873C, (a1)
l3:     rts

mf1:	trap	#1
	bra.b	mf2

Malloc:
	pea	(a2)
	move.l	a7, old_sp
	MOVE.L  D0,-(A7)
	MOVE.W  #$0048,-(A7)
mf:	move.l	bp_addr(pc), d0
	beq.b	mf1
	move.l	d0, a0
	move.l	(a0), old_bp
	move.l	_BasPag(pc), (a0)
	TRAP    #1
	move.l	bp_addr(pc), a0
	move.l	old_bp(pc), (a0)
mf2:	move.l	old_sp(pc), sp
	MOVEA.L D0,A0
	MOVE.L  (A7)+,A2
	RTS

.export Mfree
Mfree:
	pea	(a2)
	move.l	a7, old_sp
	pea	(a0)
	MOVE.W  #$0049,-(A7)
	bra.b	mf

.export Mshrink
Mshrink:
	pea	(a2)
	move.l	a7, old_sp
	move.l	d1, -(a7)
	pea	(a0)
	move	d0, -(a7)
	MOVE.W  #$004A,-(A7)
	bra.b	mf

.export la_init, linea0
regs reg d0/a0-a2
.bss
la_init:        ds.l    4
.text
linea0:         movem.l #regs, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                movem.l (sp)+, #regs
                rts

.bss
bp_addr:  ds.l  1
old_sp:	  ds.l	1
old_bp:	  ds.l	1

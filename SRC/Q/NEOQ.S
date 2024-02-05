/* NeoDesk 3.01 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/
        .export bFopen
        .export bFclose
        .export bFread
        .import bp_ptr
        .import _BasPag, multitask
bf1:	move.l	10(sp), -(sp)
	move.l	10(sp), -(sp)
	move	12(sp), -(sp)
	bra.b	bf3
	
bFopen:
        moveq   #$3d, d0
op:	move.b	multitask(pc), d1
	bne.b	bf1
        movea.l bp_ptr(pc), a1
        move.l  (a1), -(sp)
        move.l  _BasPag(pc), (a1)
        move.l  14(sp), -(sp)
        move.l  14(sp), -(sp)
        move    16(sp), -(sp)
bf3:    move    d0, -(sp)
        trap    #1
        lea     12(sp), sp
	move.b	multitask(pc), d1
	bne.b	bf2
        movea.l bp_ptr(pc), a1
        move.l  (sp)+, (a1)
bf2:    rts
        
bFclose:
        moveq   #$3E, d0
        bra.b   op
        
bFread:
        moveq   #$3F, d0
        bra.b   op
        rts

/*.export la_init
.export linea0
  
la_init:        ds.l   4

regs reg d0/a0-a2
linea0:         movem.l #regs, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                movem.l (sp)+, #regs
                rts
*/

/* NeoDesk 3.03 by Dan Wilga
   Copyright ½ 1992, Gribnif Software.
   All Rights Reserved.
*/
.IFEQ DEBUG
        .import main
        .export StkLim
        .export exit

mostregs reg d3-d7/a2-a6
        
        move.l  d1, StkLim              ; THIS MUST BE FIRST ROUTINE!!
        movem.l #mostregs, -(sp)
        move.l  a7, lastprg
        jmp     main

exit:   movea.l lastprg(pc), a7
        movem.l (sp)+, #mostregs
        ext.l   d0
        rts

StkLim: ds.l    1
lastprg:ds.l    1
.ENDIF

        .export memclr

memclr: tst.l   d0
        beq.b   mc4
        move.l  a0, d2
        btst    #0, d2
        beq.b   mc0
        clr.b   (a0)+
        subq.l  #1, d0
mc0:    moveq.l #4, d1
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

/********************************************************************
/*******
        .export _bytecpy
_bytecpy:
	tst.l	d0
        beq.b   L44
        move    a1, d1
        add     a0, d1
        btst    #0, d1
        beq.b   L46c
        subq.l  #1, d0
L46d:
        move.b  (a1)+, (a0)+
        subq.l	#1, d0
        bcc.b	L46d
        bra.b   L44
L46c:   
        move    a0, d1
        btst    #0, d1
        beq.b   L46a
L46:
        subq.l  #1, d0
        bcs.b   L44
        move.b  (a1)+, (a0)+
L46a:
        cmpi.l  #4, d0
        bcs.b   L46
        subq.l  #4, d0
        move.l  (a1)+, (a0)+
        bra.b   L46a
L44:
        rts ******/
/********************************************************************/
        .export byteswap
byteswap:
        move    12(a7), d1
        beq.b   L47
        movea.l 4(a7), a1
        movea.l 8(a7), a0
        move    a1, d0
        add     a0, d0
        btst    #0, d0
        beq.b   bs2
        subq    #1, d1
bs4:
        move.b  (a0), d0
        move.b  (a1), (a0)+
        move.b  d0, (a1)+
        dbf     d1, bs4
        bra.b   L47
bs2: 
        move    a1, d0
        btst    #0, d0
        beq.b   bs1
bs0:
        subq    #1, d1
        blt.b   L47
        move.b  (a0), d0
        move.b  (a1), (a0)+
        move.b  d0, (a1)+
bs1:
        cmpi    #4, d1
        blt.b   bs0
        subq    #4, d1
        move.l  (a0), d0
        move.l  (a1), (a0)+
        move.l  d0, (a1)+
        bra.b   bs1
L47:
        rts
/********************************************************************/
mregs reg d5-d6/a2-a3

        .export match
match:
        movem.l   #mregs, -(sp)
        movea.l   a0, a2
        movea.l   a1, a3
        moveq     #0, d5
m5:     move.b    (a3)+, d6     ; end of pat?
        beq.b     m0
        move.b    (a2)+, d1
        cmpi.b    #$2A, d6
        bne.b     m1
        tst.b     d1            ; end of string?
        beq.b     m2
        subq.l    #$1, a2      ; string--
m30:    cmpi.b    #$2e, (a2)
        bne.b     m9
        moveq     #1, d5
m9:     movea.l   a2, a0
        movea.l   a3, a1
        bsr.b     match
        tst       d0
        bne.b     m2
        tst.b     (a2)+         ; end of string?
        bne.b     m30           ; nope, try again
        cmpi.b    #$2e, (a3)     ; is next pat char .?
        bne.b     m6
        cmpi.b    #$2a, 1(a3)    ; is the next char *?
        beq.b     m8
        tst.b     1(a3)         ; it was "whatever." so return 1 if no . in str
        bne.b     m6
m7:     tst       d5
        beq.b     m2
        bra.b     m6
m8:     tst.b     2(a3)         ; and is the next char a NUL?
        beq.b     m2            ; if so, return 1 (string had no .)
m6:     moveq     #0, d0
        bra       m31
m1:     cmpi.b    #$3F, d6
        bne.b     m4
        tst.b     d1
        bne.b     m5
        bra.b     m6
m4:     cmp.b     d1, d6
        beq.b     m5
        bra.b     m6
m0:     tst.b     (a2)
        bne.b     m6
m2:     moveq.l   #$1, d0
m31:    movem.l   (sp)+, #mregs
        rts

        .export match2
match2:
        movem.l   #mregs, -(sp)
        movea.l   a0, a2
        movea.l   a1, a3
        moveq     #0, d5
m_5:    move.b    (a3)+, d6     ; end of pat?
        beq.b     m_0
        move.b    (a2)+, d1
        cmpi.b	  #'a', d1
        blt.b	  m_nl
        cmpi.b	  #'z', d1
        bgt.b     m_nl
        sub.b     #'a'-'A', d1
m_nl:   cmpi.b    #$2A, d6
        bne.b     m_1
        tst.b     d1            ; end of string?
        beq.b     m_2
        subq.l    #$1, a2      ; string--
m_30:   cmpi.b    #$2e, (a2)
        bne.b     m_9
        moveq     #1, d5
m_9:    movea.l   a2, a0
        movea.l   a3, a1
        bsr.b     match2
        tst       d0
        bne.b     m_2
        tst.b     (a2)+         ; end of string?
        bne.b     m_30           ; nope, try again
        cmpi.b    #$2e, (a3)     ; is next pat char .?
        bne.b     m_6
        cmpi.b    #$2a, 1(a3)    ; is the next char *?
        beq.b     m_8
        tst.b     1(a3)         ; it was "whatever." so return 1 if no . in str
        bne.b     m_6
m_7:    tst       d5
        beq.b     m_2
        bra.b     m_6
m_8:    tst.b     2(a3)         ; and is the next char a NUL?
        beq.b     m_2            ; if so, return 1 (string had no .)
m_6:    moveq     #0, d0
        bra       m_31
m_1:    cmpi.b    #$3F, d6
        bne.b     m_4
        tst.b     d1
        bne.b     m_5
        bra.b     m_6
m_4:    cmp.b     d1, d6
        beq.b     m_5
        bra.b     m_6
m_0:    tst.b     (a2)
        bne.b     m_6
m_2:    moveq.l   #$1, d0
m_31:   movem.l   (sp)+, #mregs
        rts

.export CDROM
CDROM:
        move.l  (sp)+, savr
        move.l  a2, sava2
        trap    #14
        movea.l savr(pc), a0
        movea.l sava2(pc), a2
        jmp     (a0)
savr:   ds.l    1
sava2:  ds.l    1

.IFNE DEBUG
.export old_crit, new_crit
.import crit_error
cregs reg a1-a2/d1-d2
	dc.b	'XBRA'
	dc.b	'Neo4'
old_crit:
	ds.l	1
new_crit:
        movem.l #cregs, -(sp)
        move    4+16(sp), d0
        move    6+16(sp), d1
        jsr     crit_error
        subq    #1, d0
        bne.b   crit3
        move.l  #$10000, d0
        bra.b   crit4
crit3:  move    4+16(sp), d0
        ext.l   d0
crit4:  movem.l (sp)+, #cregs
        rts
	
.ENDIF
/*******
.import old_mouse, click_time, ob_clicks
.export new_but
new_but:
	move.l	d1, -(sp)
	move	d0, d1
	andi	#1, d1
	bne.b	nbu3
	move.l	$4ba.w, d1
	sub.l	click_time, d1
	cmpi.l	#39, d1
	blt.b	nbu2
;;	move.l	$4ba.w, click_time
	clr	ob_clicks
	bra.b	nbu3
nbu2:	addq	#1, ob_clicks
nbu3:	move.l	(sp), d1
	move.l	old_mouse, (sp)
	rts
*********/

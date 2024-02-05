CJarID		equ	'CJar'		; $434A6172
CJ_xbios	equ	'CJ'
CJ_OK		equ	'ar'

.import message, install
.export country, cookie, Cconws

fdst:   movea.l 4(a7), a6
        movea.l 24(a6), a3
        adda.l  28(a6), a3	; find end of memory usage
        pea	0.w
        move	#32, -(sp)
        trap	#1		; Super(0L)
        move.l	d0, 2(sp)
        move.l	$4f2.w, a0
        move	$1c(a0), d0
        asr.w	#1, d0
        cmpi	#127, d0
        bne.b	got_c
        pea	cookie(pc)
        move.l	#'_AKP', -(sp)
        clr	-(sp)
        move	#CJ_xbios, -(sp)
        trap	#14		; get _AKP cookie
        lea	12(sp), sp
        cmpi	#CJ_OK, d0
        beq.b	got_akp
        moveq	#0, d0
        bra.b	got_c
got_akp:move.w	cookie+2(pc), d0
got_c:  move.b	d0, country
	move	#32, (sp)
	trap	#1		; Exit Super
	addq	#6, sp
	moveq	#0, d0
	bsr	message
        pea	cookie(pc)
	move.l	#'NLdr', -(sp)
        clr	-(sp)
        move	#CJ_xbios, -(sp)
	trap	#14		; get NEOLOAD cookie
	lea	12(sp), sp
        cmpi	#CJ_OK, d0
        bne	no_neo
        move	#$27, -(sp)
        pea	search(pc)
        move	#$4E, -(sp)
        trap	#1		; Fsfirst, search AUTO folder
        tst	d0
        beq.b	fsok
        lea	search+6(pc), a0
        move.l	a0, 2(sp)	; redo it, searching current folder
        trap	#1
        tst	d0
        bne.b 	fserr
fsok:   addq	#8, sp
        move	#$2f, -(sp)
        trap	#1		; Fgetdta
        addq	#2, sp
        move.l	d0, a0
        lea	30+5(a0), a0
        moveq	#2, d1
        clr	d7
        clr	d2
getnum: move.b	(a0)+, d2
        beq.b	getnx
        cmpi.b	#'.', d2
        beq.b	getnx
        cmpi.b	#'0', d2
        blt.b	numerr
        cmpi.b	#'9', d2
        bgt.b	numerr
        mulu	#10, d7
        sub 	#'0', d2
        add	d2, d7
        dbra	d1, getnum
getnx:	cmpi	#999, d7
	bhi.b 	numerr
	cmpi	#9, d7
	blo.b	numerr
	
	move	d7, d0
	move.l	a3, a0
	bsr	install
	move.l	d0, d0
	beq.b	bad_neo
	add.l	d0, a3
	suba.l  (a6), a3
        clr	-(sp)
        move.l  a3, -(sp)
        move.w  #49, -(sp)	; Ptermres
        trap    #1

bad_neo:moveq	#4, d0
	bra.b	error
no_neo:	moveq	#3, d0
	bra.b	error
numerr:	moveq   #2, d0
	bra.b	error
fserr:	moveq	#1, d0
error:	bsr	message
	move	#1, -(sp)
	trap	#1		; Cconin
	clr	(sp)
	trap	#1		; Pterm0

Cconws:	pea	(a2)
	pea	(a0)
	move	#9, -(sp)
	trap	#1
	addq	#6, sp
	move.l	(sp)+, a2
	rts

.data
search:		dc.b	"\AUTO\SPOOL*.PRG", 0

.bss
cookie:		ds.l	1
country:	ds.b	1
.text

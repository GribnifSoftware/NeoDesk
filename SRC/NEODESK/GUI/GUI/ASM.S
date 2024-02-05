.export userdef
usrreg reg d3-d7/a2-a6
userdef:move.l	a1, uddat+4
dousr:	move.l	a0, uddat
	pea	super(pc)
	move	#38, -(sp)
	trap	#14
	addq	#6, sp
	rts

super:	movem.l #usrreg, -(sp)
	move.l	uddat+4(pc), -(sp)
	move.l	uddat(pc), a0
        jsr	(a0)
	addq	#4, sp
        movem.l (sp)+, #usrreg
        rts

uddat:	ds.l	2

.export gobjhand
gobjhand:
	movem.w	d0/d1, uddat+4
	bra.b	dousr

/*
userdef:movem.l #usrreg, -(sp)
	pea	0.w		; clear a space for old stack
        pea     (a1)            ; push PARMBLK ptr
objh:   pea     usrret(pc)      ; push return address
        pea     (a0)            ; push function addr
        pea	0.w
        move	#$20, -(sp)	; Super
        trap	#1
        addq	#6, sp
        move.l	d0, 12(sp)
        rts                     ; call function
usrret:	move	d0, d7
	addq    #4, sp          ; skip PARMBLK ptr
	move.w	#$20, -(sp)	; Super to old stack
	trap	#1
	lea	6+20(sp), sp
	move	d7, d0
        movem.l (sp)+, #usrreg
        rts */

/*
.export gobjhand
gobjhand:
	movem.l #usrreg, -(sp)
;;	pea	0.w		; clear a space for old stack
	move	d1, -(sp)
	move	d0, -(sp)
;;	bra.b	objh
	jsr	(a0)
	addq	#4, sp
        movem.l (sp)+, #usrreg
	rts */

.export butv
.import old_mbutv, g_mb, did_mclick
butv:	move.b	d0, g_mb
	beq.b	bv1
	st	did_mclick
bv1:	move.l	old_mbutv, -(sp)
	rts

.export u_ptext, u_tedinfo, u_object

.module u_ptext
	ext.l	d0
	lsl.l	#3, d0
	move.l	d0, d1
	add.l	d0, d0
	add.l	d1, d0
	move.l	12(a0,d0.l), a0
	move.l	(a0), a0
	rts

.module u_tedinfo
	ext.l	d0
	lsl.l	#3, d0
	move.l	d0, d1
	add.l	d0, d0
	add.l	d1, d0
	move.l	12(a0,d0.l), a1
	btst	#0, 8(a0,d0.l)		; INDIRECT ob_flag
	bne.b	ut1
	move.l	a1, a0
	rts
ut1:	move.l	(a1), a0
	rts

.module u_obspec
	ext.l	d0
	lsl.l	#3, d0
	move.l	d0, d1
	add.l	d0, d0
	add.l	d1, d0
	lea	12(a0,d0.l), a0
	rts

.module u_object
	ext.l	d0
	lsl.l	#3, d0
	adda.l	d0, a0
	add.l	d0, d0
	adda.l	d0, a0
	rts

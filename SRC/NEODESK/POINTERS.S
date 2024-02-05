;
; void put_pointers( char *result, char *label, char *fmt, void **list )
;			A0		A1	4(sp) A2    8(sp) A3
;
.import spf
.import sscnf_
.export put_pointers
.export get_pointers

.module put_pointers
	movem.l	d3/a2-a6, -(sp)
	movem.l	4+24(sp), a2/a3
	lea	ptrs, a5
	lea	temp, a4
	moveq	#-1, d2		; count of pushed words
	move.b	#'%', (a4)+
	move.b	#'s', (a4)+
pp9:	moveq	#0, d1
	move.b	(a2)+, d0
	beq.b	pp1
	move.l	(a3)+, a6	; get next pointer
	cmpi.b	#'A', d0
	bge.b	pp2
	subq.b	#1, d0
	move.b	d0, d1
	move.b	(a2)+, d0
	beq.b	pp1
pp2:	move.b	#' ', (a4)+
	move.b	#'%', (a4)+
	move.b	d0, (a4)+
	cmpi.b	#'S', d0
	beq.b	ppS
	cmpi.b	#'s', d0
	beq.b	ppS
	cmpi.b	#'k', d0
	bne.b	pp4
	move.l	a6, (a5)+
	addq	#4, a6
	bra.b	pp3
ppS:	move.l	a6, (a5)+	; string pointer
	bra.b	pp3
pp4:	cmpi.b	#'X', d0
	beq.b	ppL
	cmpi.b	#'D', d0
	bne.b	pp5
ppL:	move.l	(a6)+, (a5)+	; it's a long address
pp3:	addq	#2, d2
	bra.b	pp6
pp5:	cmpi.b	#'h', d0
	beq.b	ppB
	cmpi.b	#'b', d0
	beq.b	ppB
	cmpi	#'c', d0
	bne.b	ppW
ppB:	clr	d3
	move.b	(a6)+, d3
	move	d3, (a5)+
	bra.b	ppW1
ppW:	move.w	(a6)+, (a5)+
ppW1:	addq	#1, d2		; inc counter
pp6:	dbra	d1, pp2
	bra.b	pp9
pp1:	move.b	#$d, (a4)+
	move.b	#$a, (a4)+
	clr.b	(a4)
	move.l	sp, a6		; save stack for later
	tst	d2
	blt.b	pp8
pp7:	move.w	-(a5), -(sp)
	dbra	d2, pp7
pp8:	pea	(a1)		; push type string
	lea	temp, a1
	jsr	spf
	move.l	a6, sp
	movem.l	(sp)+, d3/a2-a6
	rts

;
; void get_pointers( char *buf, char *fmt, void **list )
;			A0	   A1	      4(sp) A3
;

.module get_pointers
	movem.l	a3-a5, -(sp)
	move.l	4+12(sp), a3
	lea	ptrs, a5
	lea	temp, a4
gp9:	moveq	#0, d1
	move.b	(a1)+, d0
	beq.b	gp1
	move.l	(a3)+, a6	; get next pointer
	cmpi.b	#'A', d0
	bge.b	gp2
	subq.b	#1, d0
	move.b	d0, d1
	move.b	(a1)+, d0
	beq.b	gp1
gp2:	move.b	#'%', (a4)+
	move.b	d0, (a4)+
	move.b	#' ', (a4)+
	move.l	a6, (a5)+
	addq	#1, d2
	addq	#1, a6		; add at least 1 to a6
	cmpi.b	#'s', d0
	beq.b	gpL
	cmpi.b	#'S', d0
	beq.b	gpL
	cmpi.b	#'X', d0
	beq.b	gpL
	cmpi.b	#'k', d0
	beq.b	gpL
	cmpi.b	#'D', d0
	bne.b	gp5
gpL:	addq	#3, a6		; it's a long address
	bra.b	gp6
gp5:	cmpi.b	#'h', d0
	beq.b	gp6		; it's a byte: do nothing more
	cmpi.b	#'b', d0
	beq.b	gp6
	cmpi	#'c', d0
	beq.b	gp6
	addq	#1, a6		; it's a word
gp6:	dbra	d1, gp2
	bra.b	gp9
gp1:	clr.b	(a4)
	pea	ptrs		; push pointer to output pointers
	lea	temp, a1
	jsr	sscnf_
	addq	#4, sp
	movem.l	(sp)+, a3-a5
	rts


.bss
.module temp
	ds.b	200
ptrs:	ds.l	30

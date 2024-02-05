.offset -24
readlen:        ds.l    1	; d7
buflen:         ds.l    1
maxread:        ds.l    1	; d6
readerr:        ds.w    1
readhand:       ds.w    1
readbuf:        ds.l    1
iptr:           ds.l    1	; a5
.text

.import text_buf, diskbuf, Malloc, memclr, Mfree, Fread
.export decomp, _getc

.MACRO _getc out
  .LOCAL gc1
        subq.l     #1, d6
        bmi.s      eof		; getc1
        subq.l     #1, d7             
        bpl.s      gc1
        bsr.b	   getc2
gc1:    move.b     (a5)+, out
.ENDM

decomp:
        movem.l    d3-d7/a2-a5,-(a7)   
        move.l     d0,d3               
        move.w     d1,d4               
        move.l     a0,a3
        lea        text_buf(pc),a2
        move.l     (a2), a4
        moveq      #-1,d0             
        bsr        Malloc
        cmpa.l	   d3, a0
        blt.b	   .0
        move.l	   d3, a0
.0:     move.l     a0,buflen(a2)        
        cmp.l      #$00000518,a0       
        bls.s      .1
        move.l     a0,d0        
        bsr        Malloc
        move.l     a0,readbuf(a2)        
        bgt.s      .2             
.1:     move.l     #diskbuf,readbuf(a2)
        move.l     #$00000518,buflen(a2)
.2:     moveq.l    #0, d7           
        clr.w      readerr(a2)           
        move.w     d4,readhand(a2)        
        move.l     d3,d6        
        move.l     #$00000fee,d0       
        move	   d0, d3
        move.l     a4,a0             
        bsr        memclr             
        bra.b	   next8
loop:
        _getc      d5
        lsr	   #1, d4
        bcc.b	   .5
        move.b     d5,(a3)+            
        move.b     d5,$00(a4,d3.w)     
        addq.w     #1,d3               
        and.w      #$0fff,d3           
        bra.s      bot
.5:
	_getc      d1
	clr	   d0
	move.b	   d1, d0
        lsl.w      #4,d0               
        move.b     d5,d0
        and.w      #$000f,d1           
        addq.w     #3-1,d1               
.8:     and.w      #$0fff,d0           
        move.b     $00(a4,d0.w),(a3)     
        move.b     (a3)+,$00(a4,d3.w)     
        addq.w     #1,d3               
        and.w      #$0fff,d3           
        addq.w     #1,d0               
	dbra	   d1, .8
bot:	dbra	   d2, loop

next8:
        _getc	   d4
        moveq	   #7, d2
        bra.b	   loop

eof:    move.l     readbuf(a2),a0        
        cmpa.l     #diskbuf,a0       
        beq.s      .9             
        bsr        Mfree             
.9:     move.w     readerr(a2),d0        
        movem.l    (a7)+,d3-d7/a2-a5
        rts                            

getc2:  move.l     readbuf(a2),a0        
        move.l     a0,a5        
        move.l     d6, d0
        addq.l     #1, d0            
        move.l     buflen(a2),d1        
        cmp.l	   d0, d1
        blt.s      getc3
        move.l     d0, d1        
getc3:  move.w     readhand(a2),d0        
	move	   d2, -(sp)
        bsr        Fread
	move	   (sp)+, d2
        move.l     d0,d7             
        bpl.s      getc6
        addq	   #4, sp
        beq.s	   eof			; act on previous move.l
getc5:  move.w     d7,readerr(a2) 
        bra.b	   eof
getc6:  subq.l     #1, d7             
        bmi.s      getc2
        rts

/* void fixup( unsigned char *uptr, long *l, long start );  */
.export fixup

fixup:	moveq	#0, d2
	bra.b	fu3
fu2:	adda	d2, a1
	add.l	d0, (a1)
fu3:	move.b	(a0)+, d2
	beq.b	fu1
	cmpi.b	#1, d2
	bne.b	fu2
	lea	254(a1), a1
	bra.b	fu3
fu1:	rts

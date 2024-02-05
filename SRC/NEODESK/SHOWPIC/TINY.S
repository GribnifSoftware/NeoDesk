*	Tiny decompressor.
*	Version 2.0, by dmj
*	Copyright ½ 1994 Damien M. Jones
*
*	This code is Freeware.  You may include this code in your own
*	copyrighted code, and you don't need to give me credit; you
*	can even modify the code as much as you like.  But please leave
*	these comments in.

*	On the stack:
*	12(a7)	Address of 32K buffer to decompress image to.
*	8(a7)	Address of data words.
*	4(a7)	Address of control bytes.
*
*	d0-d2 and a0-a1 should be considered trashed; the calling
*	function should clean up the stack.  (Standard C.)  The
*	return value (in d0) should be considered garbage.

*	DW: modified slightly for Pure C calling conventions and to
*           allow reading of data words from odd address

.export tiny_decompress
tiny_decompress:
	pea (a2)		; Save registers.
	pea (a3)

	movem.l 12(a7),a1-a3	; Read parameters.
	suba.l a0,a0		; Offset in image (starts at 0).
	move.l #32000,d2	; Constant used in several places.

*	Main loop.  Get control byte and figure out what to do.
	
.loop:	moveq #0,d0		; For the moment, move nothing.
	move.b (a1)+,d0		; Read next control byte.

	beq.s .repeat2		; It was zero, repeat word.
	bmi.s .copy1		; < 0, copy words.

	cmp.b #1,d0		; Is it 1?
	beq.s .copy2		; Yes, copy words
	bra.s .repeat1		; Else repeat word (count already in d0).

*	Copy words; single-byte count.

.copy1:	ext.w d0		; Extend the sign.
	neg.w d0		; Make it positive.
	bra.s .copy		; Do the copy thing.

*	Copy words; two-byte count.  Note that the word may not be on a
*	word boundary, hence the read-shift-read method.

.copy2:	move.b (a1)+,d0		; Read next control word (high byte).
	lsl.w #8,d0		; Shift eight bits left.
	move.b (a1)+,d0		; Read in lower byte of word.

*	Copy words, count in d0.

.copy:	subq.w #1,d0		; One less for looping purposes, please.

.copy_loop:
	move.b (a2)+,d1
	asl.w #8, d1
	move.b (a2)+,d1		; Read next data word.
	bsr.s .poke		; Store word on screen and update screen pointer
	dbra d0,.copy_loop	; Loop until done.
	bra.s .next		; And check out the next control byte.

*	Repeat a word; two-byte count.  As with copy, the word may not
*	be word-aligned, so a read-shift-read is used to get it.

.repeat2:
	move.b (a1)+,d0		; Read next control word
	lsl.w #8,d0		; Shift eight bits left.
	move.b (a1)+,d0		; Read in lower byte of word

*	Repeat a word, count in d0.

.repeat1:
	subq.w #1,d0		; One less for looping purposes, please
	move.b (a2)+,d1
	asl.w #8, d1
	move.b (a2)+,d1		; Read next data word.

.repeat_loop:
	bsr.s .poke		; Store word on screen and update screen pointer
	dbra d0,.repeat_loop	; Loop until done.

*	Check if we're done.

.next:	cmpa.w d2,a0		; Off the edge of the screen (past 32000)?
	blt.s .loop		; Nope; let's figure some more control.

	move.l (a7)+,a3		; Restore registers.
	move.l (a7)+,a2
	rts			; Return.

*	Store a word on the screen and update the position.

.poke:	move.w d1,0(a3,a0.w)	; Store word.

	adda.w #160,a0		; Move down a line.
	cmpa.w d2,a0		; Too far (past 32000)?
	blt.s .done		; Nope.

	suba.w #31992,a0	; Else move to top of next column.
	cmpa.w #160,a0		; End of this column set?
	blt.s .done		; Nope.

	suba.w #158,a0		; Yes; move to top of next column set.
	cmpa.w #8,a0		; End of decompression?
	blt.s .done		; Nope.

	move.l d2,a0		; Yes, set the end-of-image value.
	moveq #0,d0		; And we're not moving any more words.

.done:	rts			; Offset's good; return!

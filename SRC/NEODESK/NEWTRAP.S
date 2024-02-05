/* NeoDesk 3.02 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/
; define this constant to 1 if making shel_fix
SHEL_FIX        equ     0
TEST		equ	0

PRN_TICS	equ	25	; 200 Hz tics spooler has control
WAIT_TICS	equ	75	; tics to give control back to system for

proc_mag equ    $380
proc_d0  equ    $384
proc_a0  equ    $3A4
proc_a7  equ    $3C0
proc_flt equ    $3C4
proc_usp equ    $3C8
proc_ir  equ    $3D2
proc_sr2 equ    $3cc
proc_stk equ    $3cc
proc_pc2 equ    $3ce
_longframe equ $59e


.IFEQ   SHEL_FIX
        .import bad_media, lock_drive
        .import _BasPag
        .import _PgmSize
        .import old_hitpa
        .import run_master, has_magcmac
        .export shrink
        .export second_run

shrink:
        move.l  old_usp(pc), a1
        move.l  a1, usp
        move.l  a0, olds_a0
        move.l  d0, -(sp)
        move    #$20, -(sp)
        trap    #1
        pea     $100.w
        move.l  olds_a0(pc), -(sp)
        move.l  #$4a0000, -(sp)
        trap    #1
        st      second_run
        bsr	appl_init
        move.l  8(sp), d0
        move.l  d0, old_hitpa
        move.l  d0, _PgmSize
        bsr     run_master
        clr     -(sp)
        trap    #1

        dc.b    'XBRA'
        dc.b    'NLdr'
old_mediach:
        ds.l    1
        .export new_mediach
new_mediach:
        move.w  4(sp), d0
        cmp.w   bad_media(pc), d0
        beq.b   nmbad
	lea	lock_drive(pc), a0
	moveq	#3, d1
nmed1:	cmp.b	(a0)+, d0
	beq.b	nmbad2
	dbra	d1, nmed1
        move.l  old_mediach(pc), a0
        jmp     (a0)
nmbad:  move    #-1, bad_media  ;       removed for 3.00-3.01
nmbad2: moveq   #2, d0
        rts
.export  old_mediach

        dc.b    'XBRA'
        dc.b    'NLdr'
old_bpb:
        ds.l    1
        .export new_bpb
new_bpb:	; used to also set bad_media to -1 if matched
        move.w  4(sp), d0
	lea	lock_drive(pc), a0
	moveq	#3, d1
nbpb1:	cmp.b	(a0)+, d0
	beq.b	nbbad
	dbra	d1, nbpb1
        move.l  old_bpb(pc), a0
        jmp     (a0)
nbbad:	moveq	#0, d0
	rts
.export  old_bpb
        
        dc.b    'XBRA'
        dc.b    'NLdr'
old_rw: ds.l    1
        .export new_rw
new_rw:
        move.w  14(sp), d0
        cmp.w   bad_media(pc), d0
        beq.b   rwbad1
	lea	lock_drive(pc), a0
	moveq	#3, d1
nrw1:	cmp.b	(a0)+, d0
	beq.b	rwbad2
	dbra	d1, nrw1
        move.l  old_rw(pc), a0
        jmp     (a0)
rwbad1: moveq   #-14, d0
        move.w  #-1, bad_media
        rts
rwbad2:	moveq	#-2, d0
	rts
.export  old_rw

.ENDIF

        .export shel_fix
        
tr1    equ $84
tr2    equ $88
t13    equ $b4
critic equ $404
_bootdev equ $446

	.export new_aes, test_magic
new_aes:lea	t2tabl(pc), a0
	st	10-10(a0)	; turn appl_init off
;;	st	13-10(a0)	; appl_find	;; 003
	st	101-10(a0)	; wind_open
	st	102-10(a0)	; wind_close
	st	105-10(a0)	; wind_set
	st	120-10(a0)	; shel_read
	st	124-10(a0)	; shel_find
	st	125-10(a0)	; shel_envrn
	st	t1tabl+75-$31	; Pexec: 004 was just 75
	st	ge_40
	rts

test_magic:
	tst.b	has_magcmac	;; 004
	beq.b	na1		;;
	st	t2tabl+110-10	;; rsrc_load
	st	t1tabl+72-$31	;; Malloc
na1:	rts
		
.IFEQ   SHEL_FIX
        .export shel_write
shel_write:
        pea.l	(a2)
        lea     intin(pc), a2
        movem.w d0-d2, (a2)
        lea     addrin(pc), a2
        move.l  a0, (a2)+
        move.l  a1, (a2)
        movea.l (sp)+, a2
        lea     control(pc), a1
        move.l  #$790003, (a1)+
        move.l  #$10002, (a1)+
        clr     (a1)
        bra.b   gem

        .export no_exe
no_exe:
        lea     no_exem(pc), a0
        bra.b   alrt
.ENDIF

        .export bad_message
        .export bad_version
bad_version:
        lea	bad_ldr(pc), a0
        bra.b   alrt
bad_message:
        lea     bad_run(pc), a0   ; set-up an alert message
alrt:   move.l  a0, addrin
        lea     control(pc), a0
        move    #52, (a0)
        moveq   #1, d0
        move    d0, 2(a0)
        move    d0, 6(a0)
        move    d0, intin
        bra.b   gem
        
ap_ver_test:
	bsr	test_magic
	move	global(pc), d0
        cmpi    #$104, d0
        blo.b   apv1
        st      ge_14
        cmpi    #$200, d0
        blo.b   apv1
        move    #3, bp_level
        cmpi	#$400, d0
        blo.b	apv1
        bsr	new_aes
        moveq	#0, d0	; clear carry
apv1:   rts

        .export appl_init
appl_init:
        move.w  #10, control
        move.w  #1, control+4
        bsr.b   gem
        move	d0, -(sp)
        bsr.b	ap_ver_test
        move	(sp)+, d0
        rts

gregs   reg     a0-a2/d0-d2
gem:    movem.l #gregs, -(sp)
        lea     aespb(pc), a0
        move.l  a0, d1
        move.w  #$C8, d0
        trap    #2
        movem.l (sp)+, #gregs
        rts

.IFEQ   SHEL_FIX
        .export graf_mkstate
graf_mkstate:
        lea     control(pc), a0
        move.l  #$4f0000, (a0)+
        move.l  #$50000, (a0)+
        clr     (a0)
        bra.b   gem
.ENDIF

shel_fix:
.IFNE   SHEL_FIX
        pea     0.w
        move.w  #$20, -(sp)
        trap    #1
        addq    #6, sp
        move.l  d0, d7
.ELSE
        move.l  usp, a0
        move.l  a0, old_usp
.ENDIF
        tst.w   _longframe.w
        beq.b   sfx1
        move    #2, frame
sfx1:   bsr     linea0
        bsr     get_bp_addr
        move    global(pc), d0
        beq.b   start1

	cmpi	#$3ff, d0
	bhi.b	l1		; skip if AES >= 4.00
	move.b	has_magcmac(pc), d0	; 004
	bne.b	l1
        subq    #2, sp
        bsr     get_bp          ; get basepage of parent in a0
        tst     (sp)+           ; is it desktop?
        beq.b   l1              ; yes, continue
.IFNE   SHEL_FIX
        bsr.b   unsuper
.ENDIF
        moveq   #0, d0
        rts                     ; return to main()
        
l1:
        bsr     prg_start       ; Steal trap 2 right away if run from desktop
        st      prgflg          ; Set flag indicating desktop load

.IFNE   SHEL_FIX
        bsr     set_bios
        bsr.b   unsuper
        lea     desk_msg(pc), a0
        bsr     alrt
        bra.b   start2
start1:
        pea     auto_msg(pc)
        move.w  #9, -(sp)
        trap    #1
        addq    #6, sp
.ELSE
start1:
.ENDIF
        bsr     set_bios        ; Appropriate the Trap 13 vector
.IFNE   SHEL_FIX
        bsr.b   unsuper
.ENDIF  
start2: moveq   #1, d0
        rts                     ; return to main()

.IFNE   SHEL_FIX
unsuper:
        move.l  d7, -(sp)
        move.w  #$20, -(sp)
        trap    #1
        addq    #6, sp
        rts
.ENDIF  

_sysbase equ $4F2

get_bp_addr:
        movea.l _sysbase.w, a0
        move.l  8(a0), sysb
        move.l  $14(a0), sysend
        move.w  2(a0), d0
        cmpi    #$100, d0
        bls.b   l2              ; don't set it again if TOS 1.0 or older
        move.l  $28(a0), bp_addr
        rts
l2:     move    $1c(a0), d0
        lsr     #1, d0
        cmpi    #4, d0
        bne.b   l3
        move.l  #$873C, bp_addr
l3:     rts

ra0d0   reg     a0/d0
in_ROM:                         ; is the value passed within the OS?
        movem.l #ra0d0, -(sp)
        move.l  12(a7), d0
        clr.l   12(a7)          ; clear return flag
        movea.l sysb(pc), a0        ; get sysbase
        cmp.l   a0, d0          ; is value less than sysbase?
        blt.b   l4
        cmp.l   sysend(pc), d0  ; is value >= sysend?
        bge.b   l4
        st      15(a7)
l4:     movem.l (sp)+, #ra0d0
        rts
        
 ; -------------------------------
 ;  Desktop vector initialization
 ; -------------------------------

 prg_start:
        move.l  tr2.w,aesvec
        move.l  #my_trap2, tr2.w
        rts

 ; -----------------------
 ;  Steal the BIOS vector
 ; -----------------------

 set_bios:
        move.l  tr1.w,dosvec
        move.l  #my_gemdos, tr1.w  ; GEMDOS
        move    _bootdev.w, d0
        add     #65, d0
        move.b  d0, nopath+3
        movea.l t13.w, a0          ; Set BIOS fall through address
        cmpi.l  #$bea0babe, -4(a0)
        bne.b   l6
        lea     6(a0), a0
        lea     slm(pc), a1
        moveq   #3, d0
l5:     cmpm.l  (a0)+, (a1)+
        bne.b   l6
        dbra    d0, l5
        move.l  $88(a0), t13adr
        move.l  #my_t13, $88(a0)
        rts
l6:     move.l  t13.w, t13adr
        move.l  #my_t13, t13.w     ; Steal trap #13 (BIOS)
        rts

	.import mdwa
mdw:    st      mdwa+1
        bra     t13_ex
mdg:    sf      mdwa+1
        bra     t13_ex
t13u:	move.l	usp, a0
t13wa:  cmpi.l  #'MDWA', (a0)   ; MultiDesk wait?
        beq.b   mdw
        cmpi.l  #'MDGO', (a0)   ; MultiDesk go?
        beq.b   mdg
        bra.b	t13_ex
 ; ------------------------
 ;  Trap #13 wedge routine
 ; ------------------------
        dc.b    'XBRA'
        dc.b    'NLdr'
t13adr: ds.l    1
my_t13:
        jmp     my_t132

my_t132:btst    #5,(sp)         ; Super or user mode?
       ; beq.b   mt134          ; If user mode, let's forget the whole thing
        beq.b   t13u
        lea     6(sp), a0
        add	frame(pc), a0
mt131: ; cmpi.l  #$050045,(a0)   ; Setexc for Timer C?
       ; beq.b   mt133
        cmpi.l  #$050101, (a0)  ; Setexc for critical error handler?
        bne.b   t13wa           ; Nope, let the call fall through

        move.b  prgflg(pc), d0  ; Run from the desktop? Vectors set already?
        beq.b   first_time      ; No, skip ahead

do_crit:
        move.l  #my_trap2, tr2.w
        move.l  critic.w,d0     ; Get current critical error vector
        move.l  4(a0),d1        ; Get address we're setting it to
        bmi.b   t13_x1          ; If minus, return old vector in d0
        move.l  d1, critic.w    ; Set that vector
t13_x1:
        rte                     ; Not a through street

first_time:
        move.l  4(a0), d0       ; Reading the vector?
        bmi.b   t13_ex          ; Yes, let the system take care of it
        move.l  d0, -(sp)
        bsr     in_ROM          ; is it in ROM?
        tst.l   (sp)+
        beq.b   t13_ex

        st      prgflg          ; Set the 'first-time'/'desktop' flag
        move.l  2(sp), retsav   ; Save return address
        move.l  #t13_2, 2(sp)   ; Replace it with my own
t13_ex:
        move.l  t13adr(pc), a0  ; Go to the Bios
        jmp     (a0)

t13_2:
        bsr     prg_start       ; Grab the trap #2 vector
rtsv:   move.l  retsav(pc), a0
        jmp     (a0)

_bconws:pea	(a6)
	move.l	8(sp), a6
	clr	d0
_bcn:	move.b	(a6)+, d0
	beq.b	_bcx
	move	d0, -(sp)
	move	bcdev(pc), -(sp)
	move	#3, -(sp)
	trap	#13
	addq	#6, sp
	bra.b	_bcn
_bcx:	move.l	(sp)+, a6
	rts

pusherr:pea	$8.w
        pea     $c.w
        lea	errvec(pc), a4
        move.l  a4, $8.w
        move.l  a4, $c.w
        move	d0, d4
        lea	12(sp), a4
pe1:    clr.l	(a4)+		; default to all 0's
        dbra	d4, pe1
        move	sr, d5
        move.l	sp, a5
        lea	12(sp), a4
pe2:    move.l	(a0)+, (a4)+	; might cause exception
        dbra	d0, pe2
errvec:	move	d5, sr
	move.l	a5, sp
	move.l	(sp)+, $c.w
	move.l	(sp)+, $8.w
	rts

crash:    clr.l     proc_mag.w
;;;;          clr.l     open_wind
          movem.l   #mgreg, -(a7)
          dc.w      $a00a                  ; mouse off
          moveq     #0, d0
          move.b    proc_flt.w, d0
          move.w    d0, -(a7)                ; push the number of bombs
          cmp.b     #9, d0                   ; greater than Trace?
          bgt.b     l10
          subq.l    #2, d0
          bra.b     l11
l10:      cmp.b     #15, d0                  ; uninitialized interrupt?
          bne.b     mg3
          moveq.l   #10, d0
          bra.b     l11
mg3:      moveq.l   #8, d0                   ; must be reserved, then
l11:      lea       errcodes(pc), a0
          lsl       #2, d0
          move.l    $0(a0,d0), -(a7)        ; push the exception type
          pea       errmsg(pc)              ; push the error message
          pea       space(pc)               ; push the output buffer
          jsr       spf(pc)
          move      #2, bcdev
          jsr       _bconws(pc)              ; Cconws() the error message
          move.l    #$00020002, (a7)
          trap      #13                      ; Bconin(2)
          lea       14(a7), a7
          bclr      #5, d0
          subi.b    #$52, d0                ; is it 'r' or 'R'?
          beq.b	    mg1
          addq.b    #$2, d0                 ; is it 'p' or 'P'?
          bne       mg8
          clr	    bcdev
mg1:      pea       regstr(pc)
          jsr       _bconws(pc)              ; Cconws(): clear the last line
          addq      #4, a7
          
          lea       proc_d0.w, a4           ; a4 points to dn
          lea       proc_a0.w, a5           ; a5 points to an
          moveq.l   #7, d4                   ; loop eight times
          moveq.l   #48, d5                  ; zero character
mg4:      move.l    (a5)+, -(a7)             ; push an
          move.w    d5, -(a7)                ; push the register number
          move.l    (a4)+, -(a7)             ; push dn
          move.w    d5, -(a7)                ; push the register number
          pea       regfmt(pc)
          pea       space(pc)
          jsr       spf(pc)
          jsr       _bconws(pc)              ; Cconws() the two registers
          lea       20(a7), a7
          addq      #1, d5                   ; increment the register number
          dbra      d4, mg4
          
          moveq.l   #15, d4                  ; loop 16 times
          move.w    #$A71F, d5
          move.w    proc_sr2.w, d0           ; sr_2 into d0
mg5:      ror.w     #1, d0
          bcc.b     mg6
          move.w    #49, -(a7)
          bra.b     mg7
mg6:      move.w    #48, -(a7)
mg7:      ror.w     #1, d5                   ; rotate the mask
          bcs.b     mg0                      ; branch if we do want this bit
          addq      #2, a7                   ; discard the character
mg0:      dbra      d4, mg5
          move.w    proc_sr2.w, -(a7)        ; push sr_2
;;; 003          move.w    proc_ir.w, -(a7)         ; push the last instruction
          move.l    proc_pc2.w, a0           ; dump starting at pc_2
          moveq	    #1, d0		     ; dbra 2 longs
          subq	    #8, sp		     ; make some space
          bsr       pusherr
          move.l    proc_pc2.w, -(sp)        ; push pc_2
          move.l    proc_usp.w, a0           ; dump starting at usp
          moveq	    #4, d0		     ; dbra 5 longs
          lea	    -20(sp), sp		     ; make some space
          bsr       pusherr
          move.l    proc_usp.w, -(a7)        ; push the usp
          lea	    proc_stk.w, a0           ; dump starting at ssp
          moveq	    #4, d0		     ; dbra 5 longs
          lea	    -20(sp), sp		     ; make some space
          bsr       pusherr
          move.l    proc_a7.w, -(a7)         ; push the ssp
          pea       other(pc)
          pea       space(pc)
          jsr       spf(pc)
          jsr       _bconws(pc)              ; Cconws() the other registers
          move.w    bcdev(pc), d0	     ; skip keypress if printer
          beq.b	    mg2
          move.l    #$00020002, (a7)
          trap      #13                       ; Bconin(2)
mg2:      lea       44+8-2+20+20(a7), a7
mg8:      movem.l   (a7)+, #mgreg
	  bra.b	    next_gemdos
crash1:	  bra	    crash

t1reg reg d0/a0-a1              ; critical for pexec
        .import new_bp


.IFEQ   SHEL_FIX
        .import open_wind
        .import errcodes
        .import errmsg
        .import space
        .import spf
        .import regstr
        .import regfmt
        .import other
        .import do_malloc
.ENDIF        
        .import path
        dc.b    'XBRA'
        dc.b    'NLdr'
dosvec: ds.l    1
mgreg reg d4-d5/a0/a4-a5
my_gemdos:
          cmpi.l    #$12345678, proc_mag.w
          beq.b     crash1
next_gemdos:
        movem.l #t1reg, t1rsav  ; save regs
;.IFNE TEST
;	moveq	#1, d1
;        move.b	scrsav(pc), d0
;        bne.b	ill
;	moveq	#2, d1
;        move.b	unsave(pc), d0
;        bne.b	ill
;.ENDIF
        move.l  usp, a0         ; set a0 to the usp
        btst    #$5, (sp)       ; is caller in supervisor mode?
        beq.b   ng02            ; no, use the usp for a0
        lea     6(sp), a0       ; yes, set a0 to sp+6
        add	frame(pc), a0
ng02:   move    (a0), d0
        beq.b   pterm
        sub     #$31, d0        ; lower than least value in table?
        bcs.b   ng9
        cmpi    #$1d, d0        ; past end of table? ($4e-$31)
        bhi.b   ng9
        lea     t1tabl(pc), a1
        move.b  0(a1,d0), d0
        blt.b   ng9             ; branch if no entry
        lea     t1jtbl(pc), a1
        move    0(a1,d0), d0
        jmp     2(pc,d0)
t1pc:

pterm:
.IFEQ   SHEL_FIX
;;;;        clr.l   open_wind
.ENDIF  
        bsr     t13_test
/***    tst.b   writflg         ; did this program do a shel_write()?
        beq.b   ng8             ; nope, zero path and get out
        subq    #2, sp
        bsr     get_bp          ; get basepage of parent
        move    (sp)+, d0
        tst     parms           ; does mode==0?
        bne.b   ng4             ; no, branch
        tst     d0              ; is it the desktop?
        bne.b   ng5
        clr.b   writflg         ; yes, clear flag
ng8:    clr.b   path            ; zero-out the path (for appl_find)
        bra.b   ng9             ; get out and let desktop handle it from here
ng4:    clr.b   writflg         ; clear flag
ng5:    movea.l $7c(a0), a0     ; get usp of program
        cmpi.l  #exec_ret, 6(a0) ; is the return address already at our routine?
        beq.b   ng3             ; if so, don't store old return address
        move.l  6(a0), oldterm  ; save old return address
ng3:    move.l  #new_rte, 6(a0) ; set the return address to my routine
        addq.b  #1, nestcnt     ; increment the nesting level counter
;       bra.b   ng9             ; get out  *********/

ng9:    movem.l t1rsav(pc), #t1reg
        move.l  dosvec(pc), -(sp)
        rts

fsfir:
        tst.b   rsrc_ld         ; rsrc_load flag set?
        beq.b   ng9
        clr.b   rsrc_ld         ; clear it
        move.l  2(a0), -(sp)
        bsr     rpathend        ; (sp) now has end of path
        move.w  #$4e, -(sp)     ; Fsfirst
        tst     _longframe.w
        beq.b   ng09
        move    -2(a0), -(sp)   ; push format word
ng09:   pea     fsret(pc)
        move    sr, -(sp)
        bra.b   ng9             ; go do Fsfirst and return to fsret

.IFEQ   SHEL_FIX
mall:
	moveq	#-1, d0
	cmp.l	2(a0), d0
	beq.b	ng9
        lea     bp_mall(pc), a1
        bra.b   mf
mfms:   lea     bp_rest(pc), a1
mf:     tst.b   do_malloc
        beq.b   ng9
        move.l  2(sp), bp_save
        move.l  a1, 2(sp)
        movea.l bp_addr(pc), a1
        move.l  (a1), old_bp
        move.l  #new_bp, (a1)
        move.l  2(a0), mall_siz
        bra.b   ng9
.ENDIF

ffopen:
        tst.b   fopen           ; do we want to trap it?
        beq.b   ng9
        clr.b   fopen
        move.l  2(a0), -(sp)
        bsr.b   rpathend        ; get just the filename
        move.l  (sp)+, 2(a0)    ; and set for Fopen
        bra     ng9

pexc:
        tst.w   2(a0)
        bne     ng9
        move.l  d2, -(sp)       ; save d2
        move.l  4(a0), -(sp)    ; push address of path
        pea     path(pc)        ; push address of path save area
        bsr     strcpy          ; and copy it for any shel_read() later
        addq    #8, sp
        clr.b   writflg		; 004: assume that if any child is Pexec'd, the last shel_write was for setup only
        move.l  12(a0), lastenv ; save environ pointer for shel_write
        move.l  exe_stk(pc), a0
        cmpa.l  #exe_buf, a0
        beq.b   pex1
        move.l  6(sp), -(a0)
        move.l  a0, exe_stk
        move.l  #new_rte, 6(sp)
pex1:   move.l  (sp)+, d2       ; restore d2
        bra     ng9

rpathend:
        move.l  a1, -(sp)
        move.l  a2, -(sp)
        movea.l 12(sp), a2
        movea.l a2, a1
ngx:    tst.b   (a1)
        beq.b   ngy
        cmpi.b  #$5c, (a1)+             ; is it a slash?
        bne.b   ngx
        movea.l a1, a2                  ; yes, set ptr to next in a2
        bra.b   ngx
ngy:    move.l  a2, 12(sp)              ; set for just the filename
        movea.l (sp)+, a2
        movea.l (sp)+, a1
        rts

fsret:  tst     d0
        seq     fopen
        addq    #6, sp
        rte

.IFNE   SHEL_FIX
t1tabl: dc.b    $0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, $2, -1, -1
        dc.b    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, $4, $0, -1, $6
.align  2
t1jtbl: dc.w    pterm-t1pc, ffopen-t1pc, pexc-t1pc, fsfir-t1pc
.ELSE
t1tabl: dc.b    $0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, $2, -1, -1
        dc.b    -1, -1, -1, -1, -1, -1, -1, -1, $4, $6, $6, $8, $0, -1, $a
.align  2
t1jtbl: dc.w    pterm-t1pc, ffopen-t1pc, mall-t1pc, mfms-t1pc, pexc-t1pc
        dc.w    fsfir-t1pc

        .import pgmsize
        .import baspag
bpregs reg d1-d3
bp_mall:tst.l   d0
        beq.b   bp_rest
        movea.l pgmsize(pc), a0
        movea.l (a0), a0
        adda.l  baspag(pc), a0
        cmp.l   a0, d0
        blo.b   bp_rest                 ; get out if below program's area
        movem.l #bpregs, bprsav
        move.l  baspag(pc), a0
        move.l  4(a0), d1               ; get hi_tpa
        cmp.l   d1, d0
        bcc.b   bp3                     ; get out if past hi_tpa
        move.l  d0, d1                  ; get address of block
        sub.l   baspag(pc), d1          ; subtract program's basepage
        add.l   mall_siz(pc), d1        ; add size of block
        movea.l pgmsize(pc), a0
        move.l  d1, (a0)                ; and stuff into pgmsize
        move.l  d0, a0
        move.l  mall_siz(pc), d1
        moveq   #0, d3
        btst    #0, d0
        beq.b   bp0                     ; if odd, clear one byte
        move.b  d3, (a0)+
        subq.l  #1, d1
bp0:    move    d1, d2                  ; d2 = length%4
        andi    #$3, d2
bp2:    subq.l  #4, d1                  ; more longs left to clear?
        blt.b   bp1
        move.l  d3, (a0)+               ; clear one long
        bra.b   bp2
bp1:    subq    #1, d2                  ; more bytes left to clear?
        blt.b   bp3
        move.b  d3, (a0)+               ; clear one byte
        bra.b   bp1
bp3:    movem.l bprsav(pc), #bpregs
bp_rest:movea.l bp_addr(pc), a0
        move.l  old_bp(pc), (a0)        ; restore old basepage
        move.l  bp_save(pc), a0         ; and get out
        jmp     (a0)
.ENDIF

t13_test:
        movea.l t13.w, a0
        cmpa.l  #my_t132, a0
        beq.b   tt3
        move.l  a1, -(sp)
        lea     uis(pc), a1
        moveq   #3, d0
        addq    #4, a0
tt4:    cmpm.l  (a0)+, (a1)+
        dbne    d0, tt4
        bne.b   tt2
tt5:    move.l  t13adr(pc), my_t132-4
        move.l  t13.w, t13adr
        move.l  #my_t132, t13.w
tt2:    movea.l (sp)+, a1
tt3:    rts

        .export    graf_mouse
graf_mouse:
        move.w    4(sp), intin
        move.l    6(sp), addrin
        lea       control(pc), a0
        move.l    #$4e0001, (a0)
        move.w    #1, 6(a0)
        bra       gem
        
mouse_on:
        move.w    #$101, d0
        bra.b     grf
        
mouse_off:
        move      #$100, d0
grf:
        pea       dum(pc)
        move.w    d0, -(sp)
        bsr.b     graf_mouse
        addq.w    #$6, a7
        rts
        
clear_mouse:
        move.l    d7, -(sp)  
        moveq     #0, d7
cm1:    cmpi.w    #$A, d7
        bge.b     cm0
        bsr.b     mouse_off
        addq.w    #$1, d7
        bra.b     cm1
cm0:    movea.l   la_init+4(pc), a0
        move.w    #1, $FFFFFDAA(a0)
cm3:    movea.l   la_init+4(pc), a0
        move.w    $FFFFFDAA(a0), d0
        beq.b     cm2
        bsr.b     mouse_on
        bra.b     cm3
cm2:    move.l    (sp)+, d7
        rts

get_bp:
        move.w  d1, -(sp)
        clr.w   6(sp)
        movea.l bp_addr(pc), a0
        movea.l (a0), a0        ; get the basepage pointer of current process
        movea.l $24(a0), a0    ; get the pointer of previous process
        move.l  a0, -(sp)
        beq.b	gb2
        move.w  bp_level(pc), d1
gb1:    tst.l   (a0)
        movea.l $24(a0), a0
        dbeq    d1, gb1
        sne     10(sp)
gb2:    movea.l (sp)+, a0
        move.w  (sp)+, d1
        rts
bp_addr:  dc.l  $602c
bp_level: dc.w  2               ; gets 3 if TOS 2.00 or newer

old_ret:                        ; set 4(sp) to last Pexec caller
        move.l  a0, -(sp)
        move.l  exe_stk(pc), a0
        move.l  (a0)+, 8(sp)
        move.l  a0, exe_stk
        move.l  (sp)+, a0
        rts

nrt1:	;tst.b	nestcnt
	;beq.b	nrt2
	cmpi.l	#path, 4(sp)
	bne.b	nrt2
	lea	12(sp), sp
	bra.b	nrt3
nrt2:   subq    #4, sp
nrt3:	move.l	a0, temp_a0
	move.l	exe_stk(pc), a0
	move.l	(a0)+, (sp)
	move.l	a0, exe_stk
	move.l	temp_a0(pc), a0
;   bsr.b   old_ret
        clr.b   path            ; zero-out the path (for appl_find)
        rts
new_rte:
        tst.b   writflg                 ; was there a shel_write?
        beq.b   nrt1
        clr.b   writflg
        tst     parms                   ; is mode==0?
        bne.b   do_graphs               ; no, branch
        clr.w   -(sp)                   ; do a Pterm0()
        trap    #1                      ; never returns from the trap

        .import blank
do_graphs:
        bsr     clear_mouse             ; reset the mouse
        bsr     mouse_off               ; turn it off
        pea     cursoff(pc)             ; push string to turn cursor off
        move.w  #9, -(sp)
        trap    #1
        move    parms+2(pc), d0         ; is the "graphic" flag set?
        bne.b   dg2                     ; yes, branch
        move.l  #clear, 2(sp)
        trap    #1                      ; clear the screen
        move.l  #curson, 2(sp)          ; push string to turn cursor on
        trap    #1
        bra.b   dg0
dg2:    lea     intin(pc), a0
        move.l  a1, (sp)                ; space left over from Cconws
        move.l  #$0001000E, (a0)+       ; wind_set( 1, 14, &blank, ...
        move.l  #blank, (a0)+           ; with blank object
        clr.l   (a0)                    ; and root=0 (with another null)
        lea     control(pc), a0
        move.l  #$690006, (a0)          ; wind_set function number, 6 intins
        clr.w   6(a0)                   ; no addrins
        bsr     gem                     ; go to it
        lea     blank(pc), a1
        move.l  a1, addrin              ; objc_draw( &blank, ...
        lea     intin(pc), a0
        moveq   #8, d0
        move.l  d0, (a0)+               ; root=0, depth=8
        lea     16(a1), a1
        move.l  (a1)+, (a0)+            ; clipx, y
        move.l  (a1), (a0)              ; clipw, h
        lea     control(pc), a0
        move.l  #$2a0006, (a0)          ; objc_draw function number, 6 intins
        move.w  #1, 6(a0)               ; 1 addrin
        bsr     gem                     ; go to it
        bsr     mouse_on                ; turn mouse back on
        move.l  (sp), a1
dg0:    addq    #6, sp                  ; clean-up after Cconws calls
;	tst.b	nestcnt
;	bne.b	dg1
	cmpi.l	#path, 4(sp)
	beq.b	dg1
	subq	#4, sp
	bra.b	dg4
dg1:	lea	12(sp), sp		; skip past my params
dg4:    bsr     old_ret			; push old return onto stack

do_pexec:
        move.l  lastenv(pc), -(sp)      ; previous environment
        pea     tail(pc)                ; push the tail
        pea     path(pc)                ; push the filename
        move.l  #0x4b0000, -(sp)        ; function # and mode
        addq.b	#1, nestcnt
        trap    #1                      ; Pexec() the program

        subq.b	#1, nestcnt
        bne.b	er2
        tst.b   ge_14                ;;;; leave parms on stack for user
        bne.b   er2                     ; ignore "gem" flag if >= TOS 1.4
        tst     parms+4                 ; does "gem" == 0?
        beq.b   er1                     ; yes, set the error return value
er2:    rts                             ; result of Pexec() in d0

er1:    move.l  a0, -(sp)               ; save a0
        movea.l retaddr(pc), a0         ; get address of intout array
        tst     d0                      ; was there an error?
        seq     1(a0)                   ; yes, (char) (a0)=0, otherwise 255
        movea.l (sp)+, a0               ; restore a0
        rts
        
find_var:
        bsr.b   getset
        movea.l 44(a0), a0      ; get pointer to start of env
fv2:    movea.l nameptr(pc), a1  ; reset pointer to name for search
        clr.b   d0              ; this is the first time for this var
fv5:    tst.b   (a0)            ; is it null?
        bne.b   fv3             ; no, continue search
        addq    #1, a0          ; increment env pointer
        tst.b   (a0)            ; are we at the end of the env?
        bne.b   fv2             ; no, search next variable
        suba.l  a0, a0          ; clear a0 for result
        rts                     ; get out
fv3:    cmpm.b  (a0)+, (a1)+    ; compare the two bytes
        beq.b   fv4             ; they are equal
        addq.b  #1, d0          ; set the "no match" flag
        bra.b   fv5             ; and test the next byte
fv4:    tst.b   d0              ; have we always matched so far?
        bne.b   fv5             ; no, forget it
        tst.b   (a1)            ; ok, is the next byte in name a NUL?
        bne.b   fv5             ; no, get next byte in env
        rts

getset:
        movea.l 12(a1), a2
        move    #1, (a2)        ; set return value to 1
        movea.l 16(a1), a2      ; get addrin
        movea.l bp_addr(pc), a0 ; get pointer to current bp
        movea.l (a0), a0        ; and get basepage of current process
        rts
        
.IFEQ   SHEL_FIX
        .export t2tabl
t2tabl: dc.b    $0, -1, -1, $1, -1, -1, -1, -1, -1, -1, -1, -1, -1, $2, $2  ; 10-24
        dc.b    $2, -1, -1, -1, -1, $D, -1, -1, -1, -1, -1, -1, -1, -1, -1  ; 25-39
        dc.b    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  ; 40-54
        dc.b    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  ; 55-69
        dc.b    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  ; 70-84
        dc.b    -1, -1, -1, -1, -1, $3, $4, -1, -1, -1, -1, -1, -1, -1, -1  ; 85-99
        dc.b    -1, $5, $E, $6, -1, $7, -1, -1, -1, -1, $8, -1, -1, -1, -1  ; 100-114
        dc.b    -1, -1, -1, -1, -1, $9, $A, -1, -1, $B, $C
.align  2
t2jtbl: dc.w    apinit-t2j, apfind-t2j, evnt-t2j, fsel-t2j, fselx-t2j
        dc.w    wopen-t2j, wdel-t2j, wset-t2j, rsload-t2j, shread-t2j
        dc.w    shwrit-t2j, shfind-t2j, shenv-t2j, mnubar-t2j, wclose-t2j
.ELSE
t2tabl: dc.b    $0, -1, -1, -1, -1, -1, -1, -1, -1, -1, $1, $2, -1, -1, $3, $4
.align  2
t2jtbl: dc.w    rsload-t2j, shread-t2j, shwrit-t2j, shfind-t2j, shenv-t2j
.ENDIF

        dc.b    'XBRA'
        dc.b    'NLdr'
aesvec: ds.l    1
t2reg   reg     d1-d2/a0-a3        ; apinit & ma51 depend on this; 003 push d1
t2ssr	equ	24
my_trap2:
        cmpi.w  #$C8, d0
        beq.b   isaes          ; an AES function
        move.l  aesvec(pc), -(sp)
        rts
        
isaes:  movem.l #t2reg, -(sp)
        movea.l d1, a1
        movea.l (a1), a2
        move    (a2), d2
.IFEQ   SHEL_FIX
        sub     #$0A, d2        ; lower than least value in table?
        bcs.b   t2j
        cmpi    #$73, d2        ; past end of table? ($7D-$0A)
.ELSE
        sub     #$6E, d2        ; lower than least value in table?
        bcs.b   t2j
        cmpi    #$0F, d2        ; past end of table? ($7D-$6E)
.ENDIF
        bhi.b   endaes
        lea     t2tabl(pc), a3
        move.b  0(a3,d2), d2
        blt.b   endaes          ; branch if no entry
        lea     t2jtbl(pc), a3
        add     d2, d2
        move    0(a3,d2), d2
        jmp     t2j(pc,d2)
.IFNE   SHEL_FIX
t2j:    cmpi    #$FF9F, d2      ; $0D-$6E
        beq.b   apfind
.ELSE
t2j:
.ENDIF
endaes: movem.l (sp)+, #t2reg
ma16:   move.l  aesvec(pc), -(sp)
        rts

.IFEQ   SHEL_FIX
apinit: btst    #5, t2ssr(sp)	; 003
        bne.b   endaes
        move.l  usp, a2
        move.l  t2ssr+2(sp), -(a2)
        move.l  a2, usp
        move.l  #apiret, t2ssr+2(sp)
        bra.b   endaes
.ENDIF
apfind:	move.b	ge_40(pc), d2		; 003
	bne.b	apf40
	lea     path(pc), a2            ; search path
        movea.l a2, a0
mab:    tst.b   (a2)
        beq.b   maa
        cmpi.b  #$5c, (a2)+             ; is it a slash?
        bne.b   mab
        movea.l a2, a0                  ; yes, set ptr to next in a0
        bra.b   mab
maa:    movea.l 16(a1), a2
        movea.l (a2), a2                ; addrin[0]
maa5:   tst.b   (a0)                    ; is it null?
        beq.b   maa7                    ; yes, check other string
        cmpi.b  #$2e, (a0)              ; or period?
        beq.b   maa7                    ; check other string
maa3:   cmpm.b  (a0)+, (a2)+            ; compare the two bytes
        beq.b   maa5                    ; they are equal, get next
        bra.b   apf40                   ; nope, test for avserver
maa7:   tst.b   (a2)                    ; ok, is the next byte in name a NUL?
        beq.b   maa6                    ; yes, get out if at end of path
        cmpi.b  #$20, (a2)              ; is it a space?
        bne.b   apf40                   ; nope, test for avserver
        moveq	#0, d1			; ID=0
maa6:   movea.l 12(a1), a0              ; intout[0]
        move.w  d1, (a0)                ; = ID
        bra.b   ma9                     ; get out w/o chaining

.import avserver
apf40:	move	avserver(pc), d1
	blt.b	endaes
	movea.l 16(a1), a2
        movea.l (a2), a2                ; addrin[0]
	lea	avser(pc), a0
	moveq	#8, d2
apf1:	cmpm.b	(a0)+, (a2)+
	dbne	d2, apf1
	beq.b	maa6			; it was OK, so set to d1 (ID)
        bra     endaes                  ; get out

shenv:  subq    #2, sp
        bsr     get_bp          ; get basepage of parent in a0
        tst     (sp)+           ; is it desktop?
        beq     endaes          ; yes, get out
        bsr     getset
        move.l  4(a2), nameptr  ; set pointer to name of var to find
        bsr     find_var        ; and try to find it
        movea.l (a2), a1        ; get pointer to result
        move.l  a0, (a1)        ; and set to next character after name
        bra.b   ma9             ; get out

shread: subq    #2, sp
        bsr     get_bp          ; get basepage of parent in a0
        tst     (sp)+           ; is it desktop?
        beq     endaes          ; yes, get out
        bsr     getset
        movea.l (a2), a1        ; get the address of cmd buffer
        clr.b   (a1)            ; and set it to null
        pea     128(a0)         ; push tail's addr
        move.l  4(a2), -(sp)    ; push the location of place to write tail
        bsr     strcpy          ; and copy it
        move.l  #path, 4(sp)    ; push the address of the pointer to last path
        move.l  (a2), (sp)      ; push location of place to write path
        bsr     strcpy          ; and copy it
        addq    #8, sp
ma9:    movem.l (sp)+, #t2reg
        rte                     ; get out without chaining
        
shfind: subq    #2, sp
        bsr     get_bp          ; get basepage of parent in a0
        tst     (sp)+           ; is it desktop?
        beq     endaes          ; yes, get out
        move.l  d0, -(sp)       ; save d0
        pea     new_dta(pc)     ; push addr of *my* dta for Fset later
        move.w  #47, -(sp)      ; Fgetdta()
        trap    #1
        move.l  d0, old_dta     ; save the old dta
        move.w  #26, (sp)       ; Fsetdta()
        trap    #1
        addq    #6, sp
        move.l  #pathstr, nameptr ; set nameptr for env search
        bsr     find_var        ; and try to find it
        move.l  a0, d0          ; test a0
        bne.b   ma3             ; var not found, set to default
        lea     nopath(pc), a0
ma3:    lea     tail(pc), a1    ; a1 is pointer to save area for path
ma4:    tst.b   (a0)            ; are we at end of env string?
        beq.b   ma5             ; yes, try this path
        cmpi.b  #44, (a0)       ; is the byte a comma?
        beq.b   ma5             ; yes, try it
        cmpi.b  #59, (a0)       ; is the byte a semi-colon?
        beq.b   ma5             ; yes, try it
        move.b  (a0)+, (a1)+    ; no, copy it into save area
        bra.b   ma4             ; and get next byte
ma5:    cmpa.l  #tail, a1       ; is a1 where it started?
        beq.b   ma6             ; yes, don't bother with slash
        cmpi.b  #92, -1(a0)     ; is the last char a slash?
        beq.b   ma6             ; yes, don't add one
        move.b  #92, (a1)+      ; add a slash
ma6:    movea.l (a2), a3        ; get pointer to name user entered
ma41:   move.b  (a3)+, (a1)+    ; copy one byte in name user passed
        bne.b   ma41            ; and continue until end of string
        pea	(a0)
        move.w  #$27, -(sp)     ; search mask = everything but folders, vols
        pea     tail(pc)        ; push full path
        move.w  #78, -(sp)      ; Fsfirst()
        trap    #1
        addq    #8, sp
        move.l  (sp)+, a0
        tst     d0              ; was the file found?
        beq.b   ma31            ; yes, copy the name
        cmpi.w  #-33, d0        ; file not found?
        bne.b   ma21            ; no, some other error so get out
        tst.b   (a0)+           ; are we at end of env var?
        bne.b   ma3             ; no, get next portion of var
        bra.b   ma21            ; get out
ma31:   pea     tail(pc)        ; yes, push pointer to the whole name
        move.l  (a2), -(sp)     ; push pointer to name user supplied
        bsr     strcpy          ; and copy it
        addq    #8, sp
ma8:    moveq   #1, d0          ; return=1 (no error)
        bra.b   ma51            ; get out
ma21:   clr.w   d0              ; return=0 (error)
;; 003 ma51:   movea.l d1, a1          ; get pointer to parmblk
ma51:   movea.l 4(sp), a1       ; get pointer to parmblk
        movea.l 12(a1), a2      ; get pointer to addr_out
        move.w  d0, (a2)        ; set return value
        move.l  old_dta(pc), -(sp) ; push addr of old dta
        move.w  #26, -(sp)      ; Fsetdta()
        trap    #1
        addq    #6, sp
        move.l  (sp)+, d0       ; restore d0
        bra     ma9             ; get out
shwrit: cmpi	#1, global+2	; 004: was cmpi -1, beq
	bhi	endaes		; get out if multitasking
	subq    #2, sp
        bsr     get_bp          ; get basepage of parent in a0
        tst     (sp)+           ; is it desktop?
        beq     endaes          ; yes, get out
        moveq   #1, d2
        movea.l 12(a1), a0
        move    d2, (a0)        ; set intout[0] to "no error" as a default
        move.l  a0, retaddr     ; save the pointer to intout[0]
        movea.l 8(a1), a0
        lea     parms(pc), a2
        move.l  (a0)+, (a2)+    ; save the shel_write() flags
        move.w  (a0), (a2)
        move.b  -3(a2), d2
        beq.b	maclr		; 004: clear if mode==0
        cmpi.b	#1, d2      	; is mode > 1 ?
        bhi     endaes          ; yes, let the desktop know about the shel_write
        st      writflg         ; set the flag saying a shel_write has occurred
ma19:   movea.l 16(a1), a0
        move.l  (a0)+, -(sp)
        pea     path(pc)
        bsr     strcpy          ; copy the path
        addq.l  #8, sp
        move.b  d2, pthpos      ; save the length of the path
        bne.b   go              ; if length > 0, go ahead with it
maclr:  clr.b   writflg
regrte: movem.l (sp)+, #t2reg   ; restore regs
        rte                     ; get out
go:     move.l  (a0), -(sp)
        pea     tail(pc)
        bsr     strcpy                  ; save the tail
        addq.l  #8, sp                  ; next, get the screen dimensions
        moveq   #5, d2
        move.l  d2, intin               ; wind_get( 0, 5, ...
        lea     control(pc), a0
        move.l  #$00680002, (a0)+       ; set function number, 2 intin's
        move    d2, (a0)+               ; 5 intout's
        clr     (a0)                    ; no addrin's
        bsr     gem                     ; go to it
        lea     blank+16(pc), a0
        move.l  intout+2(pc), (a0)+     ; save the max x and y
        move.l  intout+6(pc), (a0)      ; save the width and height
        move    (a0), 24(a0)            ; blank[1].ob_width
        movea.l la_init+4(pc), a1
        move    -46(a1), d2             ; V_CEL_HT
        addq    #2, d2
        move    d2, 26(a0)              ; blank[1].ob_height
out:    movem.l (sp)+, #t2reg           ; restore regs
        tst.b   ge_14
        bne.b   ma11                    ; ignore "gem" flag if >= TOS 1.4
        tst     parms+4                 ; is the "gem" flag set?
        beq.b   ma18                    ; no, do a Pexec()
ma11:   rte                             ; yes, get out
ma18:   move.w  (sp)+, d0
        move.l  (sp)+, jump_adr
        add	frame(pc), sp
	clr.b   writflg
;        move.b  #1, nestcnt
        move    d0, sr
        move.l  jump_adr(pc), -(sp)     ; save for later
        bsr     do_pexec
        rts

.IFEQ   SHEL_FIX
.import fselect
fsel:   tst.l   fselect
        beq     endaes                  ; fall through if no item sel
        suba.l  a0, a0
        bra.b   fsx1

fsreg   reg a0-a3/d0-d2
fselx:  tst.l   fselect
        beq     endaes                  ; fall through if no item sel
        movea.l 16(a1), a2
        movea.l 8(a2), a0               ; addrin[2]
fsx1:   move.l  a0, fsa0
        movem.l (sp)+, #t2reg
        move.l  2(sp), fsxret
        move.l  #fsx2, 2(sp)
        rte
fsx2:   movem.l #fsreg, -(sp)
        move.l  d1, a1
        movea.l 4(a1), a0
        move.w  4(a0), d0               ; global[2] (apid)
        movea.l 12(a1), a3
        move.l  fsa0(pc), -(sp)         ; title
        pea     2(a3)                   ; &intout[1]
        movea.l 16(a1), a2
        movea.l 4(a2), a1               ; file
        movea.l (a2), a0                ; path
        movea.l fselect(pc), a2
        jsr     (a2)
        addq    #8, sp
        move    d0, (a3)                ; set intout[0]
        movem.l (sp)+, #fsreg
        move.l  fsxret(pc), -(sp)
        rts

	.import w_colors
dcolor:	move	4(a0), d2
	cmpi	#18, d2			; 004
	bhi	endaes			; 004
	add	d2, d2
	add	d2, d2
	lea	w_colors(pc), a1
	cmpi	#-1, 6(a0)
	beq.b	dc1
	move	6(a0), 2(a1,d2)
dc1:	cmpi	#-1, 8(a0)
	beq	endaes
	move	8(a0), 0(a1,d2)
	bra	endaes

wset:   movea.l 8(a1), a0               ; get intin
	cmpi	#19, 2(a0)		; WF_DCOLOR
	beq.b	dcolor
        cmpi    #10, 2(a0)              ; WF_TOP
        bne     endaes                  ; fall through

wopen:	/**move.l	open_wind(pc), d2
	beq	endaes *****/
	move	j2dat(pc), d2
	bne	endaes
	move	#30, j2dat		; WM_UNTOPPED
/****	lea	intin(pc), a0
	move.l	#$0000000A, (a0)	; wind_get( 0, WF_TOP
        lea     control(pc), a0
        move.l  #$00680002, (a0)+       ; set function number, 2 intin's
        move.l  #$00050000, (a0)        ; 5 intout's, no addrin's
        lea	wopstk(pc), a0
        move.l	sp, -(a0)
        move.l	a0, sp
        move.l	d1, -(sp)
        lea     aespb(pc), a0
        move.l  a0, d1
        move.w  #$C8, d0
        tst.w   _longframe.w
        beq.b	wopn1
        move.w	#$88, -(sp)
wopn1:	pea	wopn2(pc)
	move	sr, -(sp)
	bra	ma16
wopn2:  move	intout+2(pc), j2dat+2	; save handle
        move.w  #$C8, d0
        move.l	(sp)+, d1
        move.l	(sp), sp *******/
opcl:	movem.l (sp)+, #t2reg
        move.l  2(sp), jump_adr2
        move.l  #new_rte3, 2(sp)
        bra     ma16                    ; do the wind_open and return to me

wclose:	movea.l 8(a1), a0               ; is intin[0]==0?
        tst     (a0)
        beq.b   cldel                   ; yes, it's bad
	move	j2dat(pc), d2
	bne	endaes
        move	#31, j2dat		; WM_ONTOP
        bra.b	opcl
        
wdel:   movea.l 8(a1), a0               ; is intin[0]==0?
        tst     (a0)
        bne     endaes                  ; nope, continue
cldel:  movea.l 12(a1), a0
        move    #1, (a0)                ; set intout[0] to "no error"
        bra     regrte                  ; return, no fallthrough
        
mnubar: movea.l 8(a1), a0
        tst     (a0)
        beq     endaes                  ; get out if turning it off
        movea.l 16(a1), a0
        movea.l (a0), a0                ; get tree
        movea.l la_init(pc), a1
        move.w  -12(a1), d2
        move.w  d2, 20(a0)              ; set root obj to be very wide
        move.w  d2, 44(a0)              ; and also set obj[1].ob_width
        bra     endaes

.ENDIF

rsload: tst.b   ge_14
        beq     endaes
        movea.l 16(a1), a0
        movea.l (a0), a0        ; addrin[0]
        tst.b   (a0)
        beq     endaes          ; empty
        cmpi.b  #$3a, 1(a0)     ; is it drive:?
        beq     endaes          ; yes, get out
ma04:   cmpi.b  #$5c, (a0)      ; does name contain any slashes?
        beq     endaes		; yes, get out
        tst.b   (a0)+
        bne.b   ma04
        st      rsrc_ld
        bra     endaes

.IFEQ   SHEL_FIX
        .export inittab
apiret: move.l  a0, -(sp)
        move    d0, -(sp)
        movea.l d1, a0
        movea.l 4(a0), a0
        move	(a0), d0
        beq.b	ap1
        move.l	(a0), global	; copy ver | mtask flag
        bsr	ap_ver_test
        bcc.b	ap1
apr1:   move    4(a0), d0
        subq    #2, d0
        bcs.b   ap1
        cmpi    #5, d0
        bhi.b   ap1
        lea     inittab(pc), a0
        st      0(a0,d0)
ap1:    move    (sp)+, d0
        movea.l (sp)+, a0
        rts                     ; return address is already on usp

evnt:   movea.l 4(a1), a2
        move    4(a2), d2
        subq    #2, d2
        bcs     endaes
        cmpi    #5, d2
        bhi     endaes
        lea     inittab(pc), a2
        sf      0(a2,d2)
        bra     endaes
.ENDIF

.IFEQ   SHEL_FIX
r3reg   reg d0-d2/a0-a3
new_rte3:
        tst.l   open_wind
        beq.b   jump2
        movem.l #r3reg, -(sp)
        lea	wopstk(pc), a0
        move.l	sp, -(a0)
        move.l	a0, sp
        movea.l open_wind(pc), a0
        movem.w	j2dat(pc), d0/d1
        jsr     (a0)
        move.l	(sp), sp
        movem.l (sp)+, #r3reg
jump2:  move.l  jump_adr2(pc), -(sp)
	clr	j2dat
        rts
.bss
tail:   ds.b    140		; 003: into bss
pthpos: ds.b    1		;
	ds.b	1		; even-out
second_run:
        ds.w    1
old_usp:
        ds.l    1
olds_a0:
        ds.l    1
bprsav: ds.l    3
mall_siz:
        ds.l    1
bp_save:ds.l    1
old_bp: ds.l    1
temp_a0:ds.l	1
fsa0:   ds.l    1
fsxret: ds.l    1
inittab:ds.b    6
j2dat:	ds.w	2
.text
.ENDIF    

ra0a1 reg a0-a1        
strcpy: movem.l #ra0a1, -(sp)
        movea.l 12(sp), a0
        movea.l 16(sp), a1
        moveq   #-1, d2
sc0:    addq.w  #1, d2
        move.b  (a1)+, (a0)+
        bne.b   sc0
        movem.l (sp)+, #ra0a1
        rts
        
        .export  global
aespb:  dc.l   control, global, intin, intout, addrin, addrout
exe_buf:ds.l    5		; must come before exe_stk!!
exe_stk:dc.l    exe_stk
pathstr:dc.b    'PATH=', 0
nopath: dc.b    ',\,A:\', 0
.IFEQ   SHEL_FIX
bad_run:dc.b    '[1][Sorry, but you must run|NEOLOAD.PRG at least once|from '
        dc.b    'either the built-in|desktop or the AUTO folder][Abort]', 0
bad_ldr:dc.b    '[1][The NEOLOAD.PRG that is|resident is not the same|'
        dc.b    'version as the one|currently running][Abort]',0
no_exem:dc.b    '[1][NEODESK.EXE must be in the|same folder as NEOLOAD.PRG|in '
        dc.b    'order to run NeoDesk][Abort]', 0
.ELSE
bad_run:dc.b    '[1][This program must be run|either from the AUTO '
        dc.b    'folder|or from the GEM desktop][Abort]', 0
desk_msg:
        dc.b    '[1][     Shel_Fix 1.4| Freeware by Dan Wilga|'
        dc.b    ' |', 189,' 1990 Gribnif Software][Ok]', 0
auto_msg:
        dc.b    $d, $a, '     Shel_Fix 1.4', $d, $a
        dc.b            ' Freeware by Dan Wilga', $d, $a
        dc.b        189, ' 1990 Gribnif Software', $d, $a, 0
.ENDIF
clear:  dc.b    27, 'E', 0
curson: dc.b    27, 'e', 0
cursoff:dc.b    27, 'f', 0
avser:	dc.b	'AVSERVER', 0
        .even
        .export prgflg
slm:    dc.l    $66000094, $08170005, $67082f08, $41ef000a
uis:    dc.l    $4e680817, $00056704, $41ef0006, $4a290005

.IFEQ   SHEL_FIX
        .even
rss_vec equ $50E
rso_vec equ $512
memvalid equ $420
memval2  equ $43A
resvalid equ $426
timerc   equ $114

        dc.b    'XBRA'
        dc.b    'NLdr'
old_vbl:ds.l    1

prn_vbl:move.l	old_vbl(pc), -(sp)
	btst	#2, 4(sp)		; is ir >= 4?	004
	bne.b	prvb1			; 004
	move.l	(sp), $70.w
	move	4(sp), pret_sr
	ori	#$2000, 4(sp)		; prn_ret in Super mode
	move.l	2+4(sp), pret_old
	move.l	#prn_ret, 2+4(sp)
prvb1:	rts

dpreg2  reg     a0-a7/d0-d7
	
prn_ret:movem.l	#dpreg2, dprsav2
	lea	prnstack(pc), sp
        tst     outhand                 ; is output going to printer?
        bne.b   dp0                     ; no
        lea	$506.w, a0
        bra.b	dp1
dp0:    lea	$50e.w, a0
dp1:	pea	(a0)
	move.l	$4ba.w, d0
	add.l	#PRN_TICS, d0
	move.l	d0, -(sp)
dpl:	move.l	$4ba.w, d0
	cmp.l	(sp), d0
	bhi.b	dp00
	move.l	pr_count(pc), d0	; in case changed somewhere else
	beq.b	dp00
	move.l	4(sp), a0
	move.l	(a0), a0
	suba.l	a5, a5
	jsr	(a0)
	tst	d0
	beq.b	dpl
        move    sr, -(sp)                   /* save status          */
        ori     #$700, sr              /* no interrupts        */
        movea.l bufptr(pc), a1
        movea.l (a1), a2
        move.b  (a2), d0
        ext     d0
        move.w  d0, -(sp)
        move.w  d0, -(sp)
        movea.l 2+4+4(sp), a0
        move.l	4(a0), a0
        jsr     (a0)
        addq    #4, sp
        movea.l bufptr(pc), a1
        addq.l  #1, (a1)                 /* save new head        */
        move.l	pr_bufmax(pc), d0
        cmp.l	(a1), d0
        bne.b	dp09
        move.l	pr_bufstart(pc), (a1)
dp09:   clr.b	timeout
        move.w  (sp)+, sr
        subq.l  #1, pr_count             /* decrement count      */
        bne.b	dpl
dp00:   addq	#8, sp			; remove vector pointer/timer
	move.l	$4ba.w, d0
	add.l	#WAIT_TICS, d0
	move.l	d0, last_prn
dpx:	movem.l	dprsav2(pc), #dpreg2
	tst.w	_longframe.w		; 004: preserve sr
	beq.b	dpx1
	clr	-(sp)
dpx1:   move.l	pret_old(pc), -(sp)
	move	pret_sr(pc), -(sp)
        clr.b	in_vbl
	rte				; restores sr

prt:
	move.l	last_prn(pc), d0
	cmp.l	$4ba.w, d0
	bhi.b	oldtimx
	btst	#2, (a0)		; is ir >= 4?
	bne.b	oldtimx
	tas	in_vbl
	bne.b	oldtimx
	move.l	$70.w, old_vbl
	move.l	#prn_vbl, $70.w
	bra.b	oldtimx

oldtimt:tst.b   top
        bne.b   oldtimx
        move.l  pr_bufmax(pc), d0
        bne	prt
        bra.b	oldtimx

        .import outhand
        .import pr_bufsiz, pr_count, pr_bufstart, pr_bufmax, pr_buftail, no_bcon
        .import bufptr
        .import top
        .import saver
        .import clock_cnt
        .export do_prt, falc_vid
dpreg   reg     a0-a2/a5/a7/d0-d2

        dc.b    'XBRA'
        dc.b    'NLdr'
timvec: ds.l    1

do_prt:
        tas     in_200
        bne     intim
do_prt2:
        movem.l #dpreg, dprsav
        move.l	sp, a0			; 003: for prt
        lea     newstack(pc), sp
        move.l  pr_count(pc), d0
        bgt.b   oldtimt
	sf	timeout
oldtimx:move.b	has_magcmac(pc), d0	; 004 also swapped kb check w/ pall setting
	bne	nk0
        movea.l iorec(pc), a0
        move    8(a0), d0
        cmp     last(pc), d0
        beq.b   nokey
        move    d0, last
        addq    #1, d0
        cmp     4(a0), d0
        blo.b   ot0
        moveq   #1, d0
ot0:    movea.l kbshift(pc), a1
        move.b  (a1), d1
        andi.b  #$f, d1
        cmpi.b  #$c, d1
        bne.b   nokey
        movea.l (a0), a0
        move.b  0(a0,d0), d0
        cmpi.b  #$61, d0
        bne.b   ot1
        clr.l   memvalid.w
        clr.l   memval2.w
        clr.l   resvalid.w
ot2:    movea.l _sysbase.w, a0
        movea.l $10(a0), a0
        jmp     (a0)
ot1:    cmpi.b  #$53, d0
        beq.b   ot2

nokey:	tst.b   unsave
        beq.b   nouns
.IFEQ TEST
        move.l  saver(pc), d0
        beq.b   xp0
        move.l  d0, a0
        moveq   #0, d0
        jsr     (a0)
        bra.b   xp1
xp0:	lea     pall(pc), a0
        move.w  #$8240.w, a1
        move.b	falc_vid(pc), d0
        beq.b	xpnf
        btst	#7, $ffff8267.w
        bne.b	xpnf
        move.w  #$9800.w, a1
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
xpnf:   move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0)+, (a1)+
        move.l	(a0), (a1)
        clr.b   $ffff827E.w
xp1:    sf      scrsav
        sf      unsave
.ENDIF
nouns:  move.l  saver(pc), d0
        beq.b   nk0
        move.l  d0, a0
        jsr     (a0)
nk0:    lea     clock_tic(pc), a0
        addq.b  #1, (a0)
        move.b  (a0), d0
        cmpi.b  #200, d0
        bne.b   noclk
        addq.b  #1, clock_cnt
        clr.b   (a0)
noclk:;;  move    sr, -(sp)
      ;;  ori.w   #$700, sr
dpret:;;  subq    #1, count2
      ;;  bge.b   doprret
prout:;;  clr     count2
      ;;  move    (sp)+, sr
        movem.l dprsav(pc), #dpreg
        clr.b   in_200
intim:  move.l  timvec(pc), -(sp)
        rts

        .export timvec
        .export iorec
        .export kbshift
last:   dc.w    -1

.bss
control:ds.w    5	; 003: lots of stuff into bss
global: ds.w    15
intin:  ds.w    5
intout: ds.w    5
addrin: ds.l    5
addrout:ds.l    5
lastenv:ds.l    1
old_d0: ds.l    1
jump_adr:ds.l   1
jump_adr2:ds.l  1
retsav: ds.l    1
nameptr:ds.l    1
old_dta:ds.l    1
new_dta:ds.l    11
t1rsav: ds.l    3
sysb:   ds.l    1
sysend: ds.l    1
parms:  ds.w    3
retaddr:ds.l    1
dum:    ds.w    1
last_prn:
	ds.l	1
pret_old:
	ds.l	1
pret_sr:ds.w	1
oldtim2:ds.l    1
iorec:  ds.l    1
kbshift:ds.l    1
in_200: ds.b    1
unsave: ds.b    1
clock_tic: ds.b 1
prgflg: ds.b    1
writflg:ds.b    1
nestcnt:ds.b    1
ge_14:  ds.b    1
rsrc_ld:ds.b    1
fopen:  ds.b    1
ge_40:	ds.b	1
.align  2
dprsav: ds.l    8
dprsav2:ds.l    16	; 003
        ds.l    500     ; 004: was 50
newstack:
	ds.l	100	; 004: was 50
prnstack:
	ds.l	50		; 004: was 25, increase for MagiCMac, moved here
wopstk:
la_init:        ds.l    4
.text

/******	ds.l	1000	; extra space
	dc.b	'extr'
dprsav: ds.l    8
dprsav2:ds.l    16	; 003
	dc.b	'dprs'
        ds.l    500      ; was 100
newstack:
	dc.b	'newsta'
	ds.l	1000	; 004: was 50
prnstack:
	dc.b	'prnsta'
	ds.l	500		; 004: was 25, increase for MagiCMac, moved here
wopstk:
        dc.b 'wopstk'
******/

vblsem equ $452
pallet equ $45A

       dc.b     'XBRA'
       dc.b     'NLdr'
key_orig:
       ds.l     1
       .export kbdv
       .import savcnt
       .import scrsav
       .import pall
kbdv:
       clr.w    savcnt
       tst.b    scrsav
       bne.b    kbt
kexit: move.l   key_orig(pc), -(sp)
       rts
kbt:   st       unsave
       bra.b    kexit

.export key_orig
.export clock_on
.export clock_off
.import vclock

clock_on:
        move.l  a0, vclock
        move.l  (a0)+, clocksv1
        move.l  (a0), clocksv2
        move.l  a1, clockad
        move.l  #my_vbl, (a1)
        rts

clock_off:
        movea.l clockad(pc), a0
        clr.l   (a0)
        clr.l   vclock
cx:     rts

my_vbl:
        lea     clocksv1(pc), a0
        movea.l vclock(pc), a1
        movea.l a1, a2
        cmpm.l  (a0)+, (a1)+
        bne.b   clock_off
        cmpm.l  (a0)+, (a1)+
        bne.b   clock_off
        jmp     (a2)

.ENDIF

regs reg d0/a0-a2

.align  2
.export la_init
linea0:         movem.l #regs, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                movem.l (sp)+, #regs
                rts

        dc.b    'XBRA'
        dc.b    'NLdr'
old_pri:ds.l	1
new_pri:bsr.b	pr_test
	beq.b	np1
	move.b	timeout(pc), d0
	or.b	no_bcon(pc), d0
	bne.b	np2
	move.l	pr_bufsiz(pc), d0
	cmp.l	pr_count(pc), d0
;;002	bne.b	np1			; do a normal Bcostat
	bne.b	np3			; is it full?
np2:	moveq	#0, d0
	rts
np3:	moveq	#-1, d0			; not full
	rts
np1:	move.l	old_pri(pc), -(sp)
	rts

pr_test:move	outhand(pc), d0
	bne.b	prt1
	move.l	pr_buftail(pc), d0
	rts
prt1:	clr	d0
	rts

.export old_prv, old_pri, new_prv, new_pri

        dc.b    'XBRA'
        dc.b    'NLdr'
old_prv:ds.l	1
new_prv:move.l	d1, -(sp)
	bsr.b	pr_test
	bne.b	op1
	move.l	(sp), d1
	move.l	old_prv(pc), (sp)
	rts
op1:	pea	(a0)
	move.b	no_bcon(pc), d0
	bne.b	op8
	move.l	$4ba.w, d0
	add.l	#60*200, d0		; 003: increased from 20 seconds
	move.l	pr_bufsiz(pc), d1
	move.l	kbshift(pc), a0		; 003: test for both Shift keys
op3:	move.b	(a0), d2
	andi.b	#$f, d2
	cmpi.b	#3, d2
	beq.b	opcanc
	cmp.l	pr_count(pc), d1
	bne.b	op2
	tst.b	timeout
	bne.b	op8
	cmp.l	$4ba.w, d0
	bgt.b	op3
	st	timeout
op8:	moveq	#0, d0
	bra.b	op4
op2:	move	sr, -(sp)
	ori	#$700, sr
	move.l	pr_buftail(pc), a0
	move.b	10+4+3(sp), (a0)+
	addq.l	#1, pr_count
        move.l	pr_bufmax(pc), d0
        cmp.l	a0, d0
        bne.b	op9
        move.l	pr_bufstart(pc), a0
op9:	move.l	a0, pr_buftail
	move	(sp)+, sr
	moveq	#-1, d0
op4:	move.l	(sp)+, a0
op5:	move.l	(sp)+, d1
	rts
opcanc:	clr.l	pr_count
	move.l	bufptr(pc), a0
	move.l	pr_buftail(pc), (a0)
	bra.b	op8	
.bss
clockad:   ds.l 1	; 003: into bss
clocksv1:  ds.l 1	;
clocksv2:  ds.l 1	;
frame:	ds.w	1
bcdev:	ds.w	1
timeout:ds.b	1
in_vbl:	ds.b	1
.text

        .EXPORT exit, _exit

        .EXPORT _BasPag
        .EXPORT _app
        .EXPORT errno
        .EXPORT _AtExitVec, _FilSysVec
        .EXPORT _RedirTab
        .EXPORT _StkLim
        .EXPORT _PgmSize



*>>>>>> Import references <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        .IMPORT main
        .IMPORT _FpuInit
        .IMPORT _StkSize
        .IMPORT _FreeAll




*>>>>>> Data structures <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


* Base page structure

        .OFFSET 0

TpaStart:
        .DS.L   1
TpaEnd:
        .DS.L   1
TextSegStart:
        .DS.L   1
TextSegSize:
        .DS.L   1
DataSegStart:
        .DS.L   1
DataSegSize:
        .DS.L   1
BssSegStart:
        .DS.L   1
BssSegSize:
        .DS.L   1
DtaPtr:
        .DS.L   1
PntPrcPtr:
        .DS.L   1
Reserved0:
        .DS.L   1
EnvStrPtr:
        .DS.L   1
Reserved1:
        .DS.B   7
CurDrv:
        .DS.B   1
Reserved2:
        .DS.L   18
CmdLine:
        .DS.B   128
BasePageSize:
        .DS     0



*>>>>>>> Data segment <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        .BSS


* Pointer to base page

_BasPag:
        .DS.L   1


* Applikation flag

_app:
        .DS.W   1


* Stack limit

_StkLim:
        .DS.L   1

* Program size

_PgmSize:
        .DS.L   1

* Redirection address table

_RedirTab:
        .DS.L   6


*>>>>>>> Initialized data segment <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        .DATA


* Global error variable

errno:
        .DC.W   0


* Vector for atexit

_AtExitVec:
        .DC.L   0


* Vector for file system deinitialization

_FilSysVec:
        .DC.L   0


*>>>>>>> Code segment <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        .CODE



******** TcStart ********************************************************

Start:
        jmp     Start0
        dc.w    $4afc           ; Squish cookie.w
        dc.b    'NEO_CLI', 0    ; ex. filename (user definable)
        dc.w    $4510           ; magic
        .export config
config:
version:        dc.w    $202	; no change since 2.2
num_vars:       dc.w    15
bu_size:        dc.w    512
rsbuf:          dc.w    0
batbuf:         dc.w    2000
scrn_siz:       dc.l    10000
alias_size:     dc.w    500
buf:            ds.l    1
bufsiz:         dc.l    1380+13012  ; num_vars*(VAR_LEN+VARNAMLEN+4)+bu_size+
                                    ; rsbuf+batbuf+scrn_size+alias_size
rbufsiz:        ds.l    1

******* Configuration data


* Redirection array pointer

        .DC.L   _RedirTab


* Stack size entry

        .DC.L   _StkSize



******* Tc library copyright note

*       .ALIGN  16

*        .DC.B   '>>> TURBO-C <<<<'
*        .DC.B   '  for ATARI-ST  '
*        .DC.B   '  Version  1.1  '
*        .DC.B   '(C)88/89 Borland'
*        .DC.B   ' International  '

EmpStr:
        .DC.B   $00
        .EVEN



******** Tc startup code

* Setup pointer to base page

Start0:
        LEA.L   Start-256(PC),A3
        MOVE.L  A3,_BasPag


* Setup applikation flag

        TST.L   PntPrcPtr(A3)
        SNE.B   D0
        EXT.W   D0
        MOVE.W  D0,_app

* Compute size of required memory
* := text segment size + data segment size + bss segment size
*  + stack size + base page size
* (base page size includes stack size)

        MOVE.L  TextSegSize(A3),A0
        ADD.L   DataSegSize(A3),A0
        ADD.L   BssSegSize(A3),A0
        lea     BasePageSize(A0), a0

* Setup longword aligned application stack

        MOVE.L  A3,D0
        ADD.L   A0,D0
        AND.B   #$FC,D0
        MOVE.L  D0,A7
        move.l  d0, d1          ; save for "buf" later

* Setup stack limit variable

        SUB.L   #_StkSize-256,D0
        MOVE.L  D0,_StkLim

* scan environment
        SUB.L   #250, D0
        AND.B   #$FC, D0
        MOVE.L  D0, A1
        MOVE.L  A1, A4
        MOVE.L  EnvStrPtr(A3), A2
        MOVE.L  A2, (A1)+
Start1:
        TST.B   (A2)+
        BNE     Start1
        MOVE.L  A2, (A1)+
        TST.B   (A2)+
        BNE     Start1
        CLR.L   -(A1)
        

* Free not required memory

        MOVE.L  A0, _PgmSize
        move.l  bufsiz(pc), rbufsiz
        move.l  d1, buf
        tst     _app
        beq.b   st1
        add.l   bufsiz(pc), d1
        sub.l   TpaEnd(a3), d1
        bcs.b   st0
        sub.l   d1, rbufsiz
st0:    add.l   rbufsiz(pc), a0
        MOVE.L  A0, _PgmSize
        MOVE.L  A0,-(A7)
        MOVE.L  A3,-(A7)
        MOVE.W  #0,-(A7)
        MOVE.W  #74,-(A7)
        TRAP    #1
        LEA.L   12(A7),A7

* scan commandline
st1:    LEA     CmdLine(A3), A0
        MOVE    #1, D3
        MOVE.B  (A0), D1
        EXT.W   D1
        LEA     1(A0, D1.W), A1
        CLR.B   (A1)
        pea     0.w             * argv[argc] = 0
        BRA     Start5

Start2:                         * testing blank (seperator)
        CMPI.B  #' '+1, (A1)
        BPL     Start3
        CLR.B   (A1)
        TST.B   1(A1)
        BEQ     Start3
        PEA     1(A1)
        ADDQ    #1, D3
        BRA     Start5
                                    
Start3:                         * testing quotation marks
        CMPI.B  #'"', (A1)          
        BNE     Start5            
        MOVE.B  #0, (A1)            
                                    
Start4:                         * scan previous            
        SUBQ    #1, A1              
        CMPI.B  #'"', (A1)          
        DBEQ    D1, Start4        
        TST     D1
        BMI     Start6
        PEA     1(A1)
        ADDQ    #1, D3
        CLR.B   (A1)

Start5:
        SUBQ.L  #1, A1
        DBF     D1, Start2
        
Start6:
        TST.B   1(A1)
        BEQ     Start7
        PEA     1(A1)
        ADDQ    #1, D3

Start7:
        PEA     EmpStr
        MOVE.L  A7, A2
        

******* Execute main program *******************************************
*
* Parameter passing:
*   <D0.W> = Command line argument count (argc)
*   <A0.L> = Pointer to command line argument pointer array (argv)
*   <A1.L> = Pointer to tos environment string (env)

        MOVE    D3, D0
        MOVE.L  A2, A0
        MOVE.L  A4, A1
        JSR     main



******** exit ***********************************************************
*
* Terminate program
*
* Entry parameters:
*   <D0.W> = Termination status : Integer
* Return parameters:
*   Never returns

exit:
        MOVE.W  D0,-(A7)

* Execute all registered atexit procedures

        MOVE.L  _AtExitVec,D0
        BEQ     _exit

        MOVE.L  D0,A0
        JSR     (A0)


* Deinitialize file system

_exit:
        MOVE.L  _FilSysVec,D0
        BEQ     Exit1

        MOVE.L  D0,A0
        JSR     (A0)


* Deallocate all heap blocks

Exit1:
        JSR     _FreeAll


* Program termination with return code

        MOVE.W  #76,-(A7)
        TRAP    #1



******* Module end *****************************************************

;       .END

.export vcursor
.import vcur
.import has_blit
.export in_vbl
.import show_curs

vcursor:
        tst.b   show_curs
        beq.b   vc0
        tst.b   in_vbl
        bne.b   vc0
        moveq   #0, d0
        tst.b   has_blit
        beq.b   vc4
        moveq   #7, d3
        movea   #$8a3c, a3
vc3:    btst    d3, (a3)
        nop
        bne.b   vc3
vc4:    bsr     vcur
vc0:    rts
        
        .export la_init
        .export linea0
        .export lineaa
        .export linea9
/*        .export linea5*/
        .export memcpy
/*
        .export strcpy
        .export strcmp
        .export strlen

strcpy: move.l  a0, -(sp)
strc:   move.b  (a1)+, (a0)+
        bne.b   strc
        movea.l (sp)+, a0
        rts
        
strlen: move.l  a0, d1
strll:  tst.b   (a0)+
        bne.b   strll
        move.l  a0, d0
        sub.l   d1, d0
        subq    #1, d0
        rts

strcmp: move.b    (a0)+, d0
        sub.b     (a1), d0
        bne.b     strcmpo
        tst.b     (a1)+
        bne.b     strcmp
strcmpo:ext.w     d0
        ext.l     d0
        rts
*/

memcpy:
        cmpa.l  a0, a1
        blo.b   mc0
        move    a1, d1
        add     a0, d1
        btst    #0, d1
        beq.b   L46c
        subq    #1, d0
        blt.b   L44
L46d:
        move.b  (a1)+, (a0)+
        dbf     d0, L46d
        bra.b   L44
L46c:   
        moveq   #4, d2
        move    a0, d1
        btst    #0, d1
        beq.b   L46a
L46:
        subq    #1, d0
        blt.b   L44
        move.b  (a1)+, (a0)+
L46a:
        cmp     d2, d0
        blt.b   L46
        subq    #4, d0
        move.l  (a1)+, (a0)+
        bra.b   L46a
L44:
        rts

mc0:    adda    d0, a0
        adda    d0, a1
        move    a1, d1
        add     a0, d1
        btst    #0, d1
        beq.b   mc1
        subq    #1, d0
        blt.b   mc2
mc3:
        move.b  -(a1), -(a0)
        dbf     d0, mc3
        bra.b   mc2
mc1:   
        moveq   #4, d2
        move    a0, d1
        btst    #0, d1
        beq.b   mc4
mc5:
        subq    #1, d0
        blt.b   mc2
        move.b  -(a1), -(a0)
mc4:
        cmp     d2, d0
        blt.b   mc5
        subq    #4, d0
        move.l  -(a1), -(a0)
        bra.b   mc4
mc2:
        rts

la_init:        .ds.l   4

regs reg d0/a0-a2
linea0:         movem.l #regs, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                movem.l (sp)+, #regs
                rts
                
linea9:         movem.l #regs, -(sp)
                move.l  la_init+4(pc), a0
                move.l  8(a0), a0
                st      1(a0)
                .dc.w   $a009
                movem.l (sp)+, #regs
                rts
                
lineaa:         movem.l #regs, -(sp)
                .dc.w   $a00a
                movem.l (sp)+, #regs
                rts
/*
linea5:         movem.l #regs, -(sp)
                .dc.w   $a005
                movem.l (sp)+, #regs
                rts
*/
.export t13
.export t13adr
.export t1
.export t1adr
.export nstdh
.import doecho
.import parmptr
.import parms
.import prog_text
.import gtext
.import bcon_nul
.import bconinr
.import hasch
.import parse_lev
.import iredir_hand
.import strlen
.import getstr
.import check_help
.import ctrlc
.import ign_ctrlc
.import col
.import force_cono
.import gt_ign_cr

_longframe      equ     $59e
HNDLS   equ     5

.ifeq DEMO
ssi:    illegal
setstack:
        tas     in_io
        bne.b   ssi
        move.l  d2, -(sp)
        move.l  usp, a1
        move.l  a1, old_usp
        move.l  #stackend, a1
        move.l  a1, usp
        move.l  8(sp), a1
        move.l  a7, old_a7
        movea.l #new_a7, a7
        jsr     (a1)
        movea.l old_a7(pc), a7
        movea.l old_usp(pc), a1
        move.l  a1, usp
        move.l  (sp)+, d2
        clr.b   in_io
        rts

_bconst:lea     hasch, a1
        bsr.b   bcst
        tst.l   d0
        beq.b   bcs1
        moveq   #-1, d0
bcs1:   rts
_bconin:lea     bconinr(pc), a1
bcst:   move.l  a1, -(sp)
        bsr.b   setstack
        addq    #4, sp
;        moveq  #-1, d1
;        cmp.l  d1, d0
;        beq.b  bcctrlc
        rts
;bcctrlc:move.l  #$002e0003, d0
;       rts

/*
bconst: lea     _bconst(pc), a2
        bra.b   bco
bconin: lea     _bconin(pc), a2
bco:    cmpi    #2, (a0)
        bne.b   t133
        tst.b   in_io
        bne.b   t133
        moveq   #0, d0
        bsr     force_coni
        jsr     (a2)
        move.l  d0, -(sp)
        moveq   #1, d0
        bsr     force_coni
        move.l  (sp)+, d0
        rte
*/
        dc.b    'XBRA'
        dc.b    'NCLI'
t13adr: ds.l    1
t13:    bsr     check_help
        move.l  usp, a0
        btst    #5, (sp)
        beq.b   t131
        lea     6(sp), a0
        tst.w   _longframe.w
        beq.b   t131
        addq    #2, a0
t131: ;  move    (a0)+, d0
;       subq    #1, d0
;       beq.b   bconst
;        subq   #1, d0          ; Bconin
;        beq.b  bconin
;        subq    #3, d0          ; Bconout
        cmpi    #3, (a0)+
        beq.b   t132
t133:   movea.l t13adr(pc), a0
        jmp     (a0)
t132:   cmpi.w  #$2, (a0)
        beq.b   t134
        cmpi.w  #$5, (a0)
        bne.b   t133
        tst.b   in_io
        bne.b   t133
        st      ign_ctrlc
        lea     3(a0), a1
        lea     temp(pc), a0
        move.b  (a1), (a0)
        beq.b   t139
        move.l  a0, -(sp)
        moveq   #0, d0
        bsr     force_cono
        move.l  (sp)+, a0
        st      gt_ign_cr
        pea     gtext(pc)
        bsr     setstack
        addq    #4, sp
        moveq   #1, d0
        bsr     force_cono
t138:   moveq   #0, d0
        sf      ign_ctrlc
        rte
t139:   addq    #1, col
        bra.b   t138
t134:   tst.b   in_io
        bne.b   t133
        st      ign_ctrlc
        lea     3(a0), a1
        moveq   #1, d0
        move.l  d0, bcon_nul
        moveq   #0, d0
        bsr     force_cono
        bsr.b   ech
        moveq   #1, d0
        bsr     force_cono
        sf      ign_ctrlc
        rte
ech:    move.l  a1, parmptr+4
        move    #2, parms
        pea     doecho(pc)
        bsr     setstack
        addq    #4, sp
        moveq   #0, d0
        rts

testh:  move    (a0)+, d0
        beq.b   th1
        cmpi    #-1, d0
        beq.b   th1
        move    nstdh(pc), d1
        beq     th
        subq    #1, d1
        lea     hndls(pc), a1
th2:    cmp     (a1)+, d0
        dbeq    d1, th2
        beq.b   th1
th:     moveq   #1, d0
th1:    rts

frreg reg d2/a6
_fread: bsr.b   testh
        bne     txx
fr1:    move.l  (a0)+, d0       ; length
        beq     fw0
        clr.b   in_t1
        movem.l #frreg, -(sp)
        move.l	#$FFFF, d2
        cmp.l	d2, d0
        bls.b	fr2
        move.l	d2, d0
fr2:    move.l  (a0)+, a6
        move.l  a6, a0
        clr.b   (a0)
        pea     getstr(pc)      ; needs d0, a0
        bsr     setstack
        addq    #4, sp
        subq    #1, d0
        moveq   #-1, d1
        bsr.b	tctrlc
        lea     cmdbuf, a1
fr3:    addq    #1, d1
        move.b  (a1)+, (a6)+
        dbeq    d0, fr3
        bsr     zero_cmd
        movem.l (sp)+, #frreg
        move	d1, d0
        ext.l	d0
        bra     fwr

/*        move.l  (a0), a6
fr3:    bsr     crawio_
        tst.l   d0
        beq.b   fr3
        moveq   #-2, d1
        cmp.l   d1, d0
        beq.b   fr0
        bsr.b   test_ctrlc
        cmp.b   #$d, d0
        beq.b   fr0
        move.b  d0, (a6)+
        bsr     cconech
        subq.l  #1, d6
        bne.b   fr3
fr0:    move.l  d5, d0
        sub.l   d6, d0
frx:    movem.l (sp)+, #frreg
        bra     fwr*/
test_ctrlc:
        moveq   #3, d1
        cmp     d1, d0
        beq.b   pterm32
tctrlc: tst.b   ctrlc
        bne.b   pterm32
        rts
pterm32:clr.b   ctrlc
        move.l  #$004CFFE0, -(sp)       ; Pterm(-32)
        trap    #1
t10:    illegal
fread:  bra.b   _fread
fseek:  addq    #4, a0
        bsr     testh
        bne     txx
        moveq   #0, d0
        bra     fwr

t1reg reg a1/d1-d2      ; critical to fdup, fforce, t1
        dc.b    'XBRA'
        dc.b    'NCLI'
t1adr:  ds.l    1
t1:     tas     in_t1
        bne.b   t10
        movem.l #t1reg, -(sp)
        bsr     check_help
        move.l  usp, a0
        btst    #5, 12(sp)
        beq.b   t11
        lea     18(sp), a0
        tst.w   _longframe.w
        beq.b   t11
        addq    #2, a0
t11:    move    (a0)+, d0
        cmpi    #$10, d0
        bls.b   ccon
        subi    #$3e, d0
        beq.b   fclose
        subq    #1, d0
        beq.b   fread
        subq    #1, d0
        beq.b   fwrite
        subq    #2, d0
        beq.b   fseek
        subq    #3, d0
        beq.b   fdup
        subq    #1, d0
        beq.b   fforce
txx:    movea.l t1adr(pc), a0
        movem.l (sp)+, #t1reg
        clr.b   in_t1
        jmp     (a0)
txz:    addq    #4, sp
        bra.b   txx

fdup:   lea     _fdup(pc), a1
        bra.b   fd1

fforce: lea     _fforce(pc), a1
fd1:    move    (a0), d0
        subq    #2, d0
        bcc.b   txx
        move    d0, fmode
        move.l  14(sp), fsave
        move.l  a1, 14(sp)
        bra.b   txx

ccon:   bra     ccon2

fclose: move    nstdh(pc), d0
        beq.b   txx
        subq    #1, d0
        move    (a0), d1
        lea     hndls(pc), a0
t1_30:  cmp     (a0)+, d1
        dbeq    d0, t1_30
        bne.b   txx
        subq    #1, nstdh
t1_31:  move    (a0)+, -2(a0)
        dbra    d0, t1_31
        bra.b   txx

fwrite: move    (a0)+, d0
        cmpi    #1, d0
        beq.b   fw1
        cmpi    #-1, d0
        beq.b   fw1
        move    nstdh(pc), d1
        beq.b   txx
        lea     hndls(pc), a1
fw2:    cmp     (a1)+, d0
        dbeq    d1, fw2
        bne     txx
fw1:    move.l  (a0), bcon_nul  ; length
        beq.b   fw0
        move.l  (a0)+, -(sp)
        clr.b   in_t1
        bsr     cconws          ; uses (a0) for pointer
        move.l  (sp)+, d0
        bra.b   fwr
fw0:    moveq   #0, d0
fwr:    movem.l (sp)+, #t1reg
        clr.b   in_t1
        rte

_fforce:tst.l   d0
        blt.b   fdx
        move.w  d1, -(sp)
        move    nstdh(pc), d1
        beq.b   fadd
        subq    #1, d1
        lea     hndls(pc), a0
ff1:    cmp     (a0)+, d0
        dbeq    d1, ff1
        bne.b   fadd2
        subq    #1, nstdh
ff2:    move    (a0)+, -2(a0)
        dbra    d1, ff2
fdxx:   move.w  (sp)+, d1
fdx:    move.l  fsave(pc), a0
        jmp     (a0)
_fdup:  tst.l   d0
        blt.b   fdx
        move.w  d1, -(sp)
fadd2:  move    nstdh(pc), d1
        cmpi    #HNDLS, d1
        beq.b   fdxx
        add     d1, d1
fadd:   lea     hndls(pc), a0
        move    d0, 0(a0,d1)
        addq    #1, nstdh
        bra.b   fdxx		; 2.3: was fdx

ccon2:  add     d0, d0
        clr.b   in_t1
        move    t1tbl(pc,d0), d0
        jsr     t1tbl(pc,d0)
        movem.l (sp)+, #t1reg
        rte
t1tbl:  dc.w    txz-t1tbl, cconin-t1tbl, cconout-t1tbl, txz-t1tbl
        dc.w    txz-t1tbl, txz-t1tbl, crawio-t1tbl, crawcin-t1tbl
        dc.w    cnecin-t1tbl, cconws-t1tbl, cconrs-t1tbl, _bconst-t1tbl
        dc.w    txz-t1tbl, txz-t1tbl, txz-t1tbl, txz-t1tbl, cconos-t1tbl

cconws: movea.l (a0), a1
        bsr     ech
        bra     test_ctrlc

crawio: cmpi    #$FF, (a0)
        beq.b   crawio_
        lea     1(a0), a1
        st      ign_ctrlc
        bsr.b   outch
        sf      ign_ctrlc
        rts
crawio_:st      ign_ctrlc
        bsr     _bconst
        sf      ign_ctrlc
        bne.b   crawcin
        rts

cconout:lea     1(a0), a1
        bsr.b   outch
        bra     test_ctrlc

cconin: bsr     _bconin
        bsr     test_ctrlc
        tst.l   d0
        bge.b   cconech
cco3:   rts

crawcin:st      ign_ctrlc
        bsr     _bconin
        sf      ign_ctrlc
        rts

outch:  moveq   #1, d0
        move.l  d0, bcon_nul
        bra     ech

cconech:
        move.b  parse_lev, d1
        ext     d1
        add     d1, d1
        lea     iredir_hand, a1
        cmpi    #2, 0(a1,d1)
        bne.b   cco3
        move.l  d0, -(sp)
        lea     3(sp), a1
        bsr.b   outch
        move.l  (sp)+, d0
        bra     test_ctrlc

.import cmdbuf
.import zero_cmd

cconrs: move.l  (a0), a0
        move.l  a0, -(sp)
        clr     d0
        move.b  (a0), d0
        beq.b   ccrs2
        move    d0, -(sp)
        addq    #2, a0
        clr.b   (a0)
        move.l  a0, -(sp)
        pea     getstr(pc)
        bsr     setstack
        addq    #4, sp
        move.l  (sp)+, a0
        move    (sp)+, d0
        subq    #1, d0
        moveq   #-1, d1
        lea     cmdbuf, a1
ccrs1:  addq    #1, d1
        move.b  (a1)+, (a0)+
        dbeq    d0, ccrs1
        move.l  (sp)+, a0
        move.b  d1, 1(a0)
        bsr     zero_cmd
        bra     test_ctrlc
ccrs2:  clr.b   1(a0)
        bra     test_ctrlc

cnecin: bsr     _bconin
        bra     test_ctrlc

cconos: moveq   #-1, d0
        rts

nstdh:  ds.w    1
hndls:  ds.w    HNDLS
fsave:  ds.l    1
fmode:  ds.w    1

.bss
        ds.b    100
stackend:
        ds.l    500
new_a7:
.text
old_usp:ds.l    1
old_a7: ds.l    1
in_t1:  ds.b    1
in_io:  ds.b    1
temp:   dc.b    ' ', 0
.ENDIF
in_vbl: ds.b    1
.align 2

        .export memclr

memclr: tst.l   d0
        beq.b   mr4
        move.l  a0, d2
        btst    #0, d2
        beq.b   mr0
        clr.b   (a0)+
        subq.l  #1, d0
mr0:    moveq.l #4, d1
mr2:    cmp.l   d1, d0
        blo.b   mr1
        clr.l   (a0)+
        subq.l  #4, d0
        bra.b   mr2
mr1:    tst     d0
mr3:    beq.b   mr4
        clr.b   (a0)+
        subq    #1, d0
        bra.b   mr3
mr4:    rts

;.export new_but
.export old_but
;new_but:
;       tst     d0
;       bne.b   nb1
;       move.l  old_but(pc), -(sp)
;nb1:    rts
old_but:ds.l    1

rreg reg d2-d7/a2-a6

.export reset_mv
reset_mv:
        move.l  a0, -(sp)
        bsr.b	to_super
        move    sr, -(sp)
        ori     #$700, sr
        move.l  la_init(pc), a0
        move    -602(a0), d0
        move    -600(a0), d1
        move.l  2(sp), a0
        movem.l #rreg, -(sp)
        jsr     (a0)
        movem.l (sp)+, #rreg
        move    (sp)+, sr
        bsr.b	unsuper
        addq	#4, sp
        rts

unsuper:tst.b	in_io
	bne.b	usx
	move.l	d2, -(sp)
	move	#$20, -(sp)
	trap	#1
	addq	#6, sp
usx:	rts

to_super:
	tst.b	in_io
	bne.b	tsx
	pea	0.w
	move	#$20, -(sp)
	trap	#1
	move.l	d0, d2
	addq	#6, sp
tsx:	rts
	
.export reset_but
reset_but:
        move    d0, -(sp)
        bsr.b	to_super
        move    sr, -(sp)
        ori     #$700, sr
        move    2(sp), d0
        move.l  old_but(pc), a1
        movem.l #rreg, -(sp)
        jsr     (a1)
        movem.l (sp)+, #rreg
        move    (sp)+, sr
        bsr.b	unsuper
        addq    #2, sp
        rts

/******
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
        ****/
.import mouse_vex
.export call2_w_save
call2_w_save:
                moveq   #1, d0
                jsr     mouse_vex
                move.l  a2, call_sav            * save register
                move.l  (a7)+, retsav           * save function return
                movea.l (a7)+, a0               * get pointer to NeoDesk func
                jsr     (a0)                    * and call it
                subq    #4, a7                  * fix-up stack after last movea
                movea.l call_sav(pc), a2        * restore reg
                clr     d0
                jsr     mouse_vex
                movea.l retsav(pc), a0          * and return to caller
                jmp     (a0)

call_sav:       ds.l    1                       * store a2 register here
retsav:         ds.l    1                       * where to return to caller
/*
.export xargadr
.export xargint
        dc.b    'XBRA'
        dc.b    'NCLI'
xargadr:  ds.l    1
xargint:move.l  a0, -(sp)
        move.l  usp, a0
        btst    #5, 6(sp)
        beq.b   x11
        lea     10(sp), a0
        tst.w   _longframe.w
        beq.b   x11
        addq    #2, a0
x11:    cmpi    #$4b, (a0)
        beq.b   x1
        move.l  (sp), a0
        move.l  xargadr(pc), (sp)
        rts
x1:     move.l  8(a0), a0
        move.b  #127, (a0)
        lea     xargadr(pc), a0
        move.l	(sp), d0
        move.l	(a0), (sp)
        move.l  (a0), $84.w
        clr.l   (a0)
        move.l	d0, a0
        rts*/

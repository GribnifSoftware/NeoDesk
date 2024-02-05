/* For each entry (18) (encrypt, restart key for each):
  Size x in words (1) (unsigned)
  Size y in pixels (1) (unsigned)
  X offset of icon within object (1) (unsigned) (ignored by ND 3.00)
  Y offset of icon within object (1) (unsigned) (ignored by ND 3.00)
  Banner x offset within object in pixels (1) (unsigned) (ignored by ND 3.00)
  Banner y offset within object in pixels (1) (unsigned) (ignored by ND 3.00)
  X offset of character in pixels (1) (unsigned)
  Y offset of character in pixels (1) (unsigned)
  Number of planes (1) (unsigned) (ND 3.00 assumes 2 only)
  Type (1)
  Template (12)
  Pointer to start of color table (4)
  Pointer to start of data (4)
...Color table and data for each entry...*/
        .export icheader
        .export icentries
        .export iccreate
        .export icmodify
        .export icauth
        .export iccomment
        .export iccode
        .export extract_icon
        .export icon_list
icheader:
        dc.b    '.NIC'
        dc.w    $0300
        dc.b    67
        dc.b    $4, $0, $0
        dc.w    _Fseek-extract_icon, _Fread-extract_icon,
        dc.b    'NeoDesk 4 Icon File format, Copyright ½1994 Gribnif Software'
icentries:
        ds.w    1
iccreate:
        ds.l    1
icmodify:
        ds.l    1
icauth: ds.b    26
iccomment:
        ds.b    106
iccode: ds.w    1
extract_icon:
        MOVEM.L   D3-D4/A2/A4,-(A7)
        LEA.L     -$0020(A7),A7
        MOVE.W    D0,D4
        MOVE.W    D1,D3
        MOVEA.L   A0,A4
        LEA.L     iconkey(pc),A2
        ext.l	  d3
        MOVE.L    d3, d2
        LSL.L     #4,D3
        sub.l	  d2, d3
        ADD.L     D3,D3
        add.l     #iconlist-header, d3
        CLR.W     D2
        MOVE.W    D4,D1
        MOVE.L    D3,D0
        JSR       _Fseek(PC)
        CMP.L     D0,D3
        BNE       exir2
        LEA.L     (A7),A0
        MOVEQ.L   #$1E,D1
        MOVE.W    D4,D0
        JSR       _Fread(PC)
        MOVEQ.L   #$1E,D1
        CMP.L     D0,D1
        BNE       exir2
        MOVE.B    #$37,(A2)
        MOVEQ.L   #$1E,D0
        LEA.L     (A7),A1
        LEA.L     (A7),A0
        JSR       unencrypt(PC)
        MOVE.B    (A7),D0
        CMP.B     (A4),D0
        BLS       exi2
        MOVEQ.L   #-$03,D0
        BRA       exi99
exi2:   MOVE.B    $0001(A7),D0
        CMP.B     $0001(A4),D0
        BLS       exi3
        MOVEQ.L   #-$04,D0
        BRA       exi99
exi3:   MOVE.B    $0008(A7),D0
        CMP.B     $0008(A4),D0
        BLS       exi4
        MOVEQ.L   #-$05,D0
        BRA       exi99
exi4:   CLR.W     D2
        MOVE.W    D4,D1
        MOVE.L    $0016(A7),D0
        JSR       _Fseek(PC)
        CMP.L     $0016(A7),D0
        BNE       exir2
        MOVEQ.L   #$00,D1
        MOVE.B    $0008(A7),D1
        MOVE.L    D1,-(A7)
        LEA.L     $0016(A4),A0
        MOVE.W    D4,D0
        JSR       _Fread(PC)
        CMP.L     (A7)+,D0
        BNE       exir2
        CLR.W     D3
        MOVE.B    $0008(A7),D3
        CLR.W     D0
        MOVE.B    (A7),D0
        MULU      D0,D3
        CLR.W     D1
        MOVE.B    $0001(A7),D1
        MULU      D1,D3
        ADD.W     D3,D3
        EXT.L     D3
        CLR.W     D2
        MOVE.L    $001A(A7),D0
        MOVE.W    D4,D1
        JSR       _Fseek(PC)
        CMP.L     $001A(A7),D0
        BNE       exir2
        LEA.L     $0016(A4),A0
        MOVE.L    D3,D1
        MOVE.W    D4,D0
        JSR       _Fread(PC)
        CMP.L     D0,D3
        BEQ       exi5
exir2:  MOVEQ.L   #-$02,D0
        BRA       exi99
exi5:   MOVE.B    #$37,(A2)
        CLR.W     D0
        MOVE.B    $0008(A7),D0
        EXT.L     D0
        LEA.L     $0016(A4),A1
        movea.l   a1,A0
        JSR       unencrypt(PC)
        MOVE.B    #$37,(A2)
        MOVE.L    D3,D0
        LEA.L     $001A(A4),A1
        movea.l   a1,A0
        JSR       unencrypt(PC)
        LEA.L     (A7),A0
        MOVE.L    (A0)+,(A4)+
        MOVE.L    (A0)+,(A4)+
        MOVE.L    (A0)+,(A4)+
        MOVE.L    (A0)+,(A4)+
        MOVE.L    (A0)+,(A4)+
        MOVE.w    (A0),(A4)
        CLR.W     D0
exi99:  LEA.L     $0020(A7),A7
        MOVEM.L   (A7)+,D3-D4/A2/A4
        RTS
unencrypt:
        move.l    a2, -(sp)
        lea       iconkey(pc), a2
        BRA       une1
une2:   MOVE.B    (A0)+,D1
        MOVE.B    (a2),D2
        EOR.B     D2,D1
        MOVE.B    D1,(A1)+
        ADDI.B    #$21, (a2)
une1:   SUBQ.L    #1,D0
        BGE       une2
        movea.l   (sp)+, a2
        RTS
_Fseek:
        move.l  a2, -(sp)
        move.l  a0, -(sp)
        move.w  d1, -(sp)
        move.l  d0, -(sp)
        move.w  #$42, -(sp)
        trap    #1
        lea     12(sp), sp
        movea.l (sp)+, a2
        rts
_Fread:
        move.l  a2, -(sp)
        move.l  a0, -(sp)
        move.l  d1, -(sp)
        move.w  d0, -(sp)
        move.w  #$3f, -(sp)
        trap    #1
        lea     12(sp), sp
        movea.l (sp)+, a2
        rts
iconkey:
        ds.b    2
iconlist:

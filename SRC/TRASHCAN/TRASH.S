.export la_init
.export linea0
.export lineaa
.export linea9
.export linea5
.export call_w_save

ra2d2 reg a2/d2

call_w_save:    movem.l #ra2d2, call_sav
                move.l  (a7)+, retsav
                movea.l (a7)+, a0
                jsr     (a0)
                clr.l	-(a7)
                movem.l call_sav(pc), #ra2d2
                move.l  retsav(pc), -(a7)
                rts

call_sav:       ds.l    2
retsav:         ds.l    1


  
la_init:        .ds.l   4

regs reg d0/a0-a2
linea0:         movem.l #regs, -(sp)
                .dc.w   $a000
                movem.l #regs, la_init
                movem.l (sp)+, #regs
                rts
                
linea9:         movem.l #regs, -(sp)
                .dc.w   $a009
                movem.l (sp)+, #regs
                rts
                
lineaa:         movem.l #regs, -(sp)
                .dc.w   $a00a
                movem.l (sp)+, #regs
                rts

linea5:         movem.l #regs, -(sp)
                .dc.w   $a005
                movem.l (sp)+, #regs
                rts
                

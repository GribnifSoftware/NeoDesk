;---------------------------------------------------------------------------
;
; CyReL CaTTamaran GIOS General I/O System Interface
; Copyright (C) 1994 by Cybercube Research Ltd, Toronto, Canada
; All rights reserved
;
; Version: 1.01b
; Date: 10-06-1993  Time: 16:03 first full release version
; Version: 1.02b
; Date: 18-07-1993  Time: 13:33 first updated to reflect binding changes
;
;---------------------------------------------------------------------------
; Constants
;---------------------------------------------------------------------------

_p_cookies      equ         $5a0

_CaTT_inquire   equ         0
_CaTT_s16_mhz   equ         1
_CaTT_s32_mhz   equ         2
_CaTT_s40_mhz   equ         3
_CaTT_s48_mhz   equ         4
_CaTT_s64_mhz   equ         5
_CaTT_smhz_up   equ         6
_CaTT_smhz_dn   equ         7
_CaTT_g_cmode   equ         8
_CaTT_sense_t   equ         9
_CaTT_sfpumhz   equ         10
_CaTT_gfpumhz   equ         11
_CaTT_s56mhz    equ         12

;---------------------------------------------------------------------------
; Export references
;---------------------------------------------------------------------------

                .globl      Init_CaTT_GIOS

                .globl      CaTT_inquire
                .globl      CaTT_s16_mhz
                .globl      CaTT_s32_mhz
                .globl      CaTT_s40_mhz
                .globl      CaTT_s48_mhz
                .globl      CaTT_s64_mhz
                .globl      CaTT_smhz_up
                .globl      CaTT_smhz_dn
                .globl      CaTT_g_cmode
                .globl      CaTT_sense_t
                .globl      CaTT_sfpumhz
                .globl      CaTT_gfpumhz
                .globl      CaTT_s56_mhz
		.globl	    CaTT_GIOS

;---------------------------------------------------------------------------
; Init GIOS Interface
;---------------------------------------------------------------------------

Init_CaTT_GIOS:
                tst.w   GIOS_enabled
                bpl.s   already		; DW: was exit_icg
                
                pea     check_GIOS
                move.w  #$26,-(sp)
                trap    #14
                addq.l  #6,sp
exit_icg:
                rts

check_GIOS:
                move.l  (_p_cookies).w,d0       ;get pointer to cookies
                move.l  d0,a0                   ;no cookie jar installed
                bne.b   find_cookie             ;if pointer is zero
no_cookie:
                moveq   #-1,d0                  ;set return code
                rts                             ;error
next_cookie:
                addq.l  #4,a0                   ;skip cookie entry
find_cookie:
                move.l  (a0)+,d0                ;get cookie label
                beq.b   no_cookie               ;end reached if zero
                cmp.l   #'CaTT',d0              ;check cookie ID
                bne.b   next_cookie             ;not found, then skip
                move.l  (a0),CaTT_GIOS          ;else get cookie contents
                clr.w   GIOS_enabled            ;enable GIOS functions
already:        moveq   #0,d0                   ;set return code
                rts                             ;OK

;---------------------------------------------------------------------------
; Call GIOS functions
;---------------------------------------------------------------------------

/*CaTT_setmode:	move	d0, d1			;MUST be in supervisor mode!
                move.l  #'CaTT',d0              ;prepare registers
                move.l  CaTT_GIOS,a0
                jmp     (a0)                    ;call subroutine and let it rts */

CaTT_inquire:
                moveq   #_CaTT_inquire,d1
                move.l  a0,a1
                move.l	d0,d2
                bra.s   execute_function
CaTT_s16_mhz:
                moveq   #_CaTT_s16_mhz,d1
                bra.s   execute_function
CaTT_s32_mhz:
                moveq   #_CaTT_s32_mhz,d1
                bra.s   execute_function
CaTT_s40_mhz:
                moveq   #_CaTT_s40_mhz,d1
                bra.s   execute_function
CaTT_s48_mhz:
                moveq   #_CaTT_s48_mhz,d1
                bra.s   execute_function
CaTT_s64_mhz:
                moveq   #_CaTT_s64_mhz,d1
                bra.s   execute_function
CaTT_smhz_up:
                moveq   #_CaTT_smhz_up,d1
                bra.s   execute_function
CaTT_smhz_dn:
                moveq   #_CaTT_smhz_dn,d1
                bra.s   execute_function
CaTT_g_cmode:
                moveq   #_CaTT_g_cmode,d1
                bra.s   execute_function
CaTT_sense_t:
                moveq   #_CaTT_sense_t,d1
                bra.s   execute_function
CaTT_sfpumhz:
                move.w  d0,d2
                moveq   #_CaTT_sfpumhz,d1
                bra.s   execute_function
CaTT_gfpumhz:
                move.w  #_CaTT_gfpumhz,d1
execute_function:
                moveq   #-1,d0                  ;load error code
                tst.w   GIOS_enabled            ;skip if not enabled
                bne.s   exec_fn_exit

                movem.l d1-d2/a1,save_temp+4    ;save parameters

                pea     0.w
                move.w  #$20,-(sp)
                trap    #1
                addq.l  #6,sp                   ;switch to supervisor &
                move.l  d0,save_temp            ;save stack
                
                movem.l  save_temp+4,d1-d2/a1   ;restore parameters

                move.l  #'CaTT',d0              ;prepare registers
                move.l  CaTT_GIOS,a0
                jsr     (a0)                    ;call subroutine

                move.l  save_temp,-(sp)         ;switch back to old
                move.l  d0,save_temp            ;save result code
                move.w  #$20,-(sp)              ;super/user mode
                trap    #1
                addq.l  #6,sp

                move.l  save_temp,d0            ;get result code & terminate
exec_fn_exit:
                rts

;---------------------------------------------------------------------------
; Data segment
;---------------------------------------------------------------------------

                .data
GIOS_enabled:
                dc.w    -1
                                
;---------------------------------------------------------------------------
; Variables segment
;---------------------------------------------------------------------------

                .bss
                
save_temp:      ds.l    4

CaTT_GIOS:      ds.l    1

;---------------------------------------------------------------------------

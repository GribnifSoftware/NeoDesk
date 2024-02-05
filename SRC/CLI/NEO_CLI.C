#define TURBO_C

/*
   ZOO.TTP locks up if no params
   memcpy( to most->env trashes text_reader 
   Cconws should return the count
   updt_neo happens twice if src and dest same for copy
   Continue line
   Multiple commands/line
   CASE statement
   Tokenizer
   MENU default
   crashes if reopened from another prog during a loop
*/

#ifdef NEW
  char new_str[] = "\
o Fixed inverse text at the end of line so that the Blitter won't mess up.";
#endif

#define CLI_VER         0x203
#define CLI_CMDS        "NeoDesk CLI v. 2.3 Commands:\n"
#ifndef DEMO
  #define CLI_ALRT      "[3][|  NeoDesk CLI v. 2.3|\275 1996, Gribnif Software][Like, wow!]"
#else DEMO
  #define CLI_ALRT      "[3][|NeoDesk CLI DEMO v. 2.3|\275 1996, Gribnif Software][Like, wow!]"
#endif

#ifndef DEMO
  #define USE_MOST
#else DEMO
  #define CLI_DEMO_INI 0x2319
#endif DEMO

typedef struct
{
  int version;
  int num_vars;
  unsigned int backup_size;
  int rs232_buf;
  unsigned int batch_buf;
  long screen_size;
  unsigned int alias_size;
  char *buf;
  long bufsiz, real_bufsiz;
} Config;
extern Config config;

#include "tos.h"
#include "aes.h"
#include "new_aes.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "..\neocommn.h"
#include "mwclinea.h"
#include "multevnt.h"
#include "xwind.h"

extern linea0( void ), linea9( void ), lineaa( void );
extern int _app;
extern int cdecl call2_w_save( int (*func)(), ... );

#define rindex  strrchr
#define index   strchr

#define WIND_TYPE0  (NAME|CLOSER|FULLER|MOVER|SIZER|HSLIDE|VSLIDE)
#define WIND_TYPE0G (NAME|CLOSER|FULLER|MOVER|SIZER)
#define WIND_TYPE1  (NAME|CLOSER|FULLER|MOVER)

#define MagX_COOKIE 0x4D616758L

#define CMDBUFLEN  256
#define DISKBUFLEN 640          /* must be >= CMDBUFLEN && >= sizeof(env_vars) */
#define MAX_PARM   20           /* max no. of command parameters + 1 for cmd */
#define MAX_IF     20           /* max nesting level of IF statements */
#define MAX_WHILE  5            /* max nesting level of WHILE statements */
#define MAX_UNTIL  5            /*                   of REPEAT/UNTIL */
#define MAX_FOR    5            /*                   of FOR */
#define LONGEST    9            /* length of longest cmd +1 for space in help */
#define LONG_FUNC  8            /* length of longest func +1 for space in help */
#define LONG_OP    5            /* length of longest op +1 for space in help */
#define MAX_LEN_OTH 24          /* length of longest help string (not cmd) +2 */
#define MIN_H      16*3         /* minimum height of window */
#define MENU_LINES 10           /* maximum # of choices in a menu */
#define VARNAMLEN  8
#define CALLS      5            /* max # of nested subroutines */
#define VAR_LEN    80           /* length of user vars */
#define MAX_PREC   6            /* max op precedence */
#define LABEL_LEN  8            /* length of GOTO/CALL labels */
#define ALIAS_NLEN 8            /* length of alias name */
#define REGEXP_LEN 92           /* longest possible regexp */

#define OS_version  (*(int *)((*(long *)0x4F2)+2))
#define OS_conf     (*(int *)((*(long *)0x4F2)+0x1c))
#define OS_kbs      (*(char **)((*(long *)0x4f2)+36))
#define M_POS_HX    (int *)((long)la_init.li_a0-856)

#define GENERAL   -128
#define SYNTAX    -129
#define AE_OK     0L    /* OK, no error */
#define AERROR   (-1L)  /* Basic, fundamental error */
#define AEDRVNR  (-2L)  /* Drive not ready */
#define AEUNCMD  (-3L)  /* Unknown command */
#define AE_CRC   (-4L)  /* CRC error */
#define AEBADRQ  (-5L)  /* Bad request */
#define AE_SEEK  (-6L)  /* Seek error */
#define AEMEDIA  (-7L)  /* Unknown medium */
#define AESECNF  (-8L)  /* Sector not found */
#define AEPAPER  (-9L)  /* No paper */
#define AEWRITF (-10L)  /* Write fault */
#define AEREADF (-11L)  /* Read fault */
#define AEGENRL (-12L)  /* General error */
#define AEWRPRO (-13L)  /* Write protect */
#define AE_CHNG (-14L)  /* Medium change */
#define AEUNDEV (-15L)  /* Unknown device */
#define AEBADSF (-16L)  /* Bad sectors on format */
#define AEOTHER (-17L)  /* Insert other disk */
#define AEINVFN (-32L)  /* Invalid function number               1 */
#define AEFILNF (-33L)  /* File not found                        2 */
#define AEPTHNF (-34L)  /* Path not found                        3 */
#define AENHNDL (-35L)  /* Too many open files (no handles left) 4 */
#define AEACCDN (-36L)  /* Access denied                         5 */
#define AEIHNDL (-37L)  /* Invalid handle                        6 */
#define AENSMEM (-39L)  /* Insufficient memory                   8 */
#define AEIMBA  (-40L)  /* Invalid memory block address          9 */
#define AEDRIVE (-46L)  /* Invalid drive was specified          15 */
#define AEXDEV  (-48L)  /* Cross device rename (not documented)    */
#define AENMFIL (-49L)  /* No more files                        18 */
#define AERANGE (-64L)  /* Range error, context unknown         33 */
#define AEINTRN (-65L)  /* Internal error                       34 */
#define AEPLFMT (-66L)  /* Invalid program load format          35 */
#define AEGSBF  (-67L)  /* Setblock failed: growth restrictions 36 */

typedef struct
{
  char *name;
  void (*func)(void);
  int parms;
  char *help;
} CMD_STRUCT;

#define MORE_FUNC (void (*)(void))1L
#define TIME_FUNC (void (*)(void))2L

typedef struct
{
  char *name;
  char *val;
  int type;
} VAR_STRUCT;

typedef struct
{
  char *name;
  int (*func)(void);
  int parms;
} FUNC_STRUCT;

typedef struct
{
  char *name;
  int prec;
  int (*func)(int i);
} OPS_STRUCT;

EMULTI emulti;
extern void t1(), t13();
extern long t1adr, t13adr;
int dum, col, row, maxcol=34, maxrow=17, w_handle=-1, parms, cmdbufmax,
    while_line[MAX_WHILE], until_line[MAX_UNTIL], call_line[CALLS],
    for_line[MAX_FOR], batch_hand, bparms=0, neo_apid, line_num, this_line,
    saver, savec, redir_hand[3] = {2}, tos_ver, neo_ver,
    cli_ver=CLI_VER, ignore_redraw=0, iredir_hand[3] = {2}, eredir_hand[3] = {2},
    oldrow, old_celht, old_mx, old_my, errnum, lastline, apid,
    func_pnum, mouse_col, mouse_row, mouse_but, oldrssize, scrn_wid,
    wind_type0=WIND_TYPE0, wind_type, more, more_row, debug_hand, vdi_hand=-1,
    sw_bits, sw0, sw_argv, __diskbuf[DISKBUFLEN/2]/* 2.3: moved from char */;

unsigned long bcon_nul;
int (*old_execute)( int s_h, int f_h, int o_h );
#ifndef DEMO
extern int nstdh;
#endif

long for_val[MAX_FOR], while_pos[MAX_WHILE], until_pos[MAX_UNTIL],
     for_pos[MAX_FOR], call_pos[CALLS], logbase, last_time;

#define diskbuf ((char *)__diskbuf)

char cmdbuf[CMDBUFLEN+2],/* +2 so there will be an extra NUL or room for \r\n */
     bcmdbuf[CMDBUFLEN+1],
     srch_path[VAR_LEN+1],
     vprompt[VAR_LEN+1]="\"$cwd \"",
     suff[VAR_LEN+1]="prg,tos,ttp,bat,btp,npg,ntp,app",
     env_vars[62*10+1],
     *line_buf,
     *old_cmds,
     *screen,           /* start of screen buffer */
     *scrn_bot,         /* points to end of last line of visible screen */
     *scrn_ptr,         /* start of first line of visible screen */
     *scrn_top,         /* lowest address in buffer */
     glob[]="*.*", globs[]="\\*.*", 
     dflt_path[120],
     path[120],
     bpath[120],
     escaped[MAX_PARM],
     bad_if[MAX_IF], in_if=-1,
     while_lev=-1, until_lev=-1, for_lev=-1, bad_next=1, call_lev=-1,
     gto[4], *gtos[4] = { &in_if, &while_lev, &until_lev, &for_lev },
     fontno=1, fonttab[3][2]={ 6, 6, 8, 8, 8, 16 }, char_w, char_h,
     reopen, was_closed, neodesk_present,
     multitask,
     debug, in_batch, has_wild,
     updt_neo,
     open_before,
     menu,
     paused, parse_lev,
     eol, gt, inverse, discard, gt_ign_cr,
     menu_labls[MENU_LINES][LABEL_LEN+1],
     err_label[LABEL_LEN+1], quit_label[LABEL_LEN+1],
     menu_keys[MENU_LINES],
     errmsg[VAR_LEN+1],
     input[2][VAR_LEN+1], output[2][VAR_LEN+1], _error[3][VAR_LEN+1],
     item_path[120],
     ctrlc, ign_ctrlc, insert,
     ichar_cr[3],
     prog_text,
     old_pause,
     last_esc,
     has_blit,
     show_curs=1,
     curs_is_on,
     is_top,
     retried,
     in_ctrlc,
     tos_wind=1,
     b_old_show,
     b_old_hide,
     pexec,
     in_vt,
     vt_appl,
     first_open=1,
     in_hist,
     got_env,
     use_argv,
     sf[REGEXP_LEN],
     new_batch,
     time_it,
     wc_expand,
     env_saved,
     has_magx,
     *kbshift = (char *)0xe1b,
     *alias,
     *alias_ptr,
     *func_parm,
     *oldrsbuf,
     slashes[] = "btnraf$\\\"\'",
     slashes2[] = "\b\t\n\r\a\f$\\\"\'",
     sfmt[] = "%s", dfmt[]="%d", lfmt[]="%D",
#ifndef DEMO
     name[] = "  NeoDesk CLI",
#else DEMO
     name[] = "  NeoDesk CLI Demo",
#endif DEMO
     op[] = "open",
     empty[]="", slash[]="\\", cslash[]=":\\",
     period[]=".", pperiod[]="..", sp[]=" ", *cmdptr, *parmptr[MAX_PARM],
     true[]="TRUE", false[]="FALSE", *bparmptr[MAX_PARM], ons[]="ON",
     offs[]="OFF", nf[]="%s%s not found.", wo[]="%s without %s.",
     parstr[]=" parameter", room[]="Not enough room for more %ss.",
     too_lng[]="%s too long.",
     nst[]="Too many nested %s%ss.", cn[]="Could not ", tm[]="Too many",
     looperr[]="Fatal branching error! Terminating batch file!",
     *devs[6] = { "prn:", "aux:", "con:", "midi:", "ikbd:", "null:" },
     date_fmt[]="%02d/%02d/%02d",
     inp_nam[]=".INPUT", out_nam[]=".OUTPUT", err_nam[]=".ERROR",
#ifdef DEMO
     not_in_demo[]="This function cannot be performed in the demo version.",
     demo_msg[]="[1][This demo version of the|NeoDesk CLI can only be run|as a desk accessory from the|demo version of NeoDesk 3][Ok]",
#endif DEMO
#ifdef USE_MOST
     hlp_most[]="This command is here for testing only",
#endif
     hlp_hlp[]="?|HELP [command]\nGet help.",
     hlp_att[]="ATTRIB|CHMOD filespec [w+|-] [h+|-] [s+|-] [a+|-]\n\
Follow each of w, h, s, a, with + (to set) or - (to unset).\n",
     hlp_sho[]="SHOW filespec\nDisplay text files using NeoDesk.",
     hlp_cat[]="CAT|TYPE filespec\nDisplay text files in CLI window.",
     hlp_cop[]="COPY|CP filespec1 [filespec2] [o-] [d-]\nCopy files.\n\
source and dest may contain wildcards.",
     hlp_deb[]="DEBUG ON|OFF [file]\nTurn debugging ON or OFF, optionally\
 setting output to file.",
     hlp_del[]="DELETE|DEL|ERASE|RM filespec [FULL]\nRemove files.\n\
If FULL is specified any folders are removed along with their contents.",
     hlp_dir[]="DIR|LS [[drive:][\\][path\\][filespec]] [w+] [d-] [f-] [v-]\n\
Get directory listing.\nFlag w+ wide; d- no subdirs; f- no files;\
 v- not verbose (just name).",
     hlp_els[]="ELSEIF|IF statement\nExecute commands \
if statement evaluates to TRUE.",
     hlp_fmt[]="FORMAT [v-] [B:] [f+] 1|2 sectors tracks\nFormat a floppy disk.\
 v- turns off status display; f+ does a \"fast\" format; sectors \
is usually 9 or 10, tracks 40-80.",
     hlp_getc[]="GETCH varname1 [varname2]\nRead one character from keyboard \
and place in varname1. If varname2 is present it gets the scancode.",
     hlp_gem[]="GEMMSG msgnum apname [words...]\n\
Send message to apname with appl_write(). Use %address for vars.",
     hlp_get[]="GETSTR varname [default]\nRead a string of characters \
into varname.",
     hlp_mou[]="MOUSE ON|OFF|RESET|SHAPE [ARROW|LINE|BEE|POINT|GRAB|THINCROSS|\
THICKCROSS|OUTLINECROSS]\nTurn mouse pointer on or \
off, reset it to \"on\" once, or change its shape.",
     hlp_mov[]="MOVE|MV filespec1 [filespec2] [o-] [d-]\nMove files and folders \
from one location to another on the same drive.",
     hlp_rfmt[]="REFORMAT [B:] [sides] [sectors] [tracks]\nReinitialize a floppy disk. \
sectors is usually 9 or 10, tracks 40-80. \n\
Setting sides, sectors, or tracks to 0 keeps old value.",
     hlp_ren[]="RENAME|RN filespec1 filespec2\nRename files.",
     hlp_pro[]="PROMPT FULL|BRIEF\nSet prompt mode.",
     hlp_sel[]="SELECT filespec [file [comment]]\n\
Return name of selected item in $ITEM.",
     hlp_rs[]="SETRS speed NONE|XON|RTS|BOTH ODD|EVEN|NONE 1|1.5|2 8|7|6|5\n\
Set RS-232 port speed, flow control, parity, stop bits, and word length.",
     hlp_win[]="WINDOW WIDE|NARROW|x y width height\nSet the \
window type or size and position.",
     hlp_mkd[]="MD|MKDIR path\nMake a directory (folder).",
     hlp_ali[]="ALIAS [name [[k+ shift scancode]|[i+] command...]]\n\
Create alias. Flag k+ sets key to invoke, i+ asks interactively.",
     hlp_whe[]="WHEREIS filespec [n+ times] [d-] [paths...]\nSearch at filespec for files.\n\
Secondary paths are searched with same mask; n+ limits number of matches; d- won't search subdirs; name is in $ITEM.";

Rect window = { 8, 40, 35*8, 18*8 }, /* change maxcol, maxrow if change this */
    wind_out, temp_rect, maximum, rect2, clip;

struct Vnames
{
  char c[VARNAMLEN+2];	/* + 2 because of null and one even-out byte */
} *vnames;

struct Vvals
{
  char c[VAR_LEN+2];	/* + 2 because of null and one even-out byte */
} *vvals;

DTA dma, new_dma;

NEO_ACC *neo_acc=0L, *oneo_acc;
GRAPHICS *graphics;
IOREC *io, *rsio;

void memclr( void *buf, long size );

int f_date(), f_exists(), f_extn(), f_fname(), f_hidden(), f_isfile(),
    f_isfold(), f_path(), f_size(), f_sysdate(), f_systime(), f_time(),
    f_write(), f_name(), f_upcase(), f_hasch(), f_not(), f_first(), f_next(),
    f_drive(), f_alert(), f_iredir(void), f_redir(void), f_eredir(void),
    f_redira(void), f_ioredir(void), f_expand(), f_len(), f_left(), f_right(),
    f_mid(), f_mf(), f_df(), f_ascii(), f_chr(), f_scan(), f_rscan(), f_getscr(),
    f_archive(), f_address();

FUNC_STRUCT funcs[] = {
    "ADDRESS", &f_address, 1,
    "ALERT", &f_alert, 1,
    "ARCHIVE", f_archive, 1,
    "ASCII", f_ascii, 1,
    "CHR", f_chr, 1,
    "DATE", &f_date, 1,
    "DF", &f_df, 1,
    "DRIVE", &f_drive, 1,
    "EXISTS", &f_exists, 1,
    "EXPAND", &f_expand, 1,
    "EXTN", &f_extn, 1,
    "FIRST", &f_first, 1,
    "FNAME", &f_fname, 1,
    "GETSCR", &f_getscr, 2,
    "HASCH", &f_hasch, 0,
    "HIDDEN", &f_hidden, 1,
    "ISFILE", &f_isfile, 1,
    "ISFOLD", &f_isfold, 1,
    "LEFT", &f_left, 2,
    "LEN", &f_len, 1,
    "MF", &f_mf, 0,
    "MID", &f_mid, 3,
    "NAME", &f_name, 1,
    "NEXT", &f_next, 0,
    "NOT", &f_not, 1,
    "PATH", &f_path, 1,
    "RIGHT", &f_right, 2,
    "RSCAN", f_rscan, 2,
    "SCAN", f_scan, 2,
    "SIZE", &f_size, 1,
    "SYSDATE", &f_sysdate, 0,
    "SYSTIME", &f_systime, 0,
    "TIME", &f_time, 1,
    "UPCASE", &f_upcase, 1,
    "WRITE", &f_write, 1,
    NULL };

int o_mult(), o_div(), o_plus(), o_minus(), o_less(), o_le(),
    o_great(), o_ge(), o_eq(), o_ne(), o_and(), o_or(), o_cat();

OPS_STRUCT ops[] = {
    "MULT", 0, &o_mult,
    "DIV", 0, &o_div,
    "+", 1, &o_plus,
    "-", 1, &o_minus,
    "~", 2, &o_cat,
    "<", 3, &o_less,
    "<=", 3, &o_le,
    ">", 3, &o_great,
    ">=", 3, &o_ge,
    "==", 4, &o_eq,
    "!=", 4, &o_ne,
    "&&", 5, &o_and,
    "||", 6, &o_or,
    NULL };

VAR_STRUCT my_vars[] = {
    "0", (char *)&bparmptr[0], 4,
    "1", (char *)&bparmptr[1], 4,
    "2", (char *)&bparmptr[2], 4,
    "3", (char *)&bparmptr[3], 4,
    "4", (char *)&bparmptr[4], 4,
    "5", (char *)&bparmptr[5], 4,
    "6", (char *)&bparmptr[6], 4,
    "7", (char *)&bparmptr[7], 4,
    "8", (char *)&bparmptr[8], 4,
    "9", (char *)&bparmptr[9], 4,
    "#", (char *)&bparms, 1,
    "TOS_VER", (char *)&tos_ver, 11,
    "NEO_VER", (char *)&neo_ver, 8,
    "CLI_VER", (char *)&cli_ver, 7,
    "AT_NEO", &neodesk_present, 5,
    "PROMPT", vprompt, -99,
    "TOS_WIND", &tos_wind, -5,
    &inp_nam[0], (char *)0L, -9,
    &out_nam[0], (char *)0L, -10,
    &err_nam[0], (char *)0L, -13,
    "COL", (char *)&col, -1,
    "ROW", (char *)&row, -1,
    "MAX_COL", (char *)&maxcol, 1,
    "MAX_ROW", (char *)&maxrow, 1,
    "FONT", &fontno, 6,
    "CHAR_W", &char_w, 6,
    "CHAR_H", &char_h, 6,
    "WINDOW", (char *)&window, 3,
    "MAX_WIND", (char *)&maximum, 3,
    "MOUSE_COL", (char *)&mouse_col, 1,
    "MOUSE_ROW", (char *)&mouse_row, 1,
    "MOUSE_BUT", (char *)&mouse_but, 1,
    "DEBUG", &debug, -5,
    "LINE", (char *)&line_num, 1,
    "LASTLINE", (char *)&lastline, 1,
    "ERRNUM", (char *)&errnum, 1,
    "ERRMSG", errmsg, 0,
    "ITEM", item_path, 0,
    "CWD", dflt_path, 0,
    "PATH", srch_path, -99,
    "SUFF", suff, -99,
    "UPDT_NEO", &updt_neo, -5,
    "USE_ARGV", &use_argv, -5,
    "WC_EXPAND", &wc_expand, -5,
    NULL };

void dohlp(), docat(), docd(void), doexit(), dols(), dopwd(), dowarm(), doquitneo(),
     dofont(), doloadinf(), doprint(), dotouch(), dorm(), dormdir(),
     dorn(), docp(), domkdir(), domv(), domode(), doprompt(), docold(),
     dotext(), dopause(void), doecho(void), doif(), doelse(), doelseif(), doendif(),
     dochmod(),
#ifdef USE_MOST
     domost(),
#endif
#ifdef NEW
     donew(),
#endif
     dowind(), dogoto(), domenu(void), doset(), doshift(), dogemmsg(),
     dogetstr(void), dogetch(), dounset(), domouse(), doclose(), dofor(),
     doforend(), dowhile(), dowhilend(), dorepeat(), dountil(), dodebug(),
     doselect(), dokick(), donull(), doshow(), docursor(), doonerr(),
     doonquit(), docall(void), doreturn(), dopop(), dosetrs(), dovt(void),
     dodup(), doalias(), dounalias(), dowhere() /*, doread() */, dosetenv(),
     dounsetenv(), doformat(), doreformat();

CMD_STRUCT cmds[] = {
#ifdef USE_MOST
    "MOST", &domost, 0, &hlp_most[0],
#endif
#ifdef NEW
    "NEW", &donew, 0, "NEW\nFor testing only.",
#endif
    "?", &dohlp, 0x41, &hlp_hlp[0],
    "ALIAS", &doalias, -1, &hlp_ali[0],
    "ATTRIB", &dochmod, -1, &hlp_att[0],
    "CALL", &docall, 1, "CALL label\nCall lines beginning at label as a \
subroutine. Terminate with RETURN.",
    "CAT", &docat, 1, &hlp_cat[0],
    "CD", &docd, 0x41, "CD [[drive:][\\]path]\nChange working directory.",
    "CHMOD", &dochmod, -1, &hlp_att[0],
    "CLOSE", &doclose, 0, "CLOSE\nCloses NeoDesk CLI window.",
    "COLDBOOT", &docold, 0, "COLDBOOT\n\
Restarts the computer as if it had been turned off and back on.",
    "COPY", &docp, 0x44, &hlp_cop[0],
    "CP", &docp, 0x44, &hlp_cop[0],
    "CURSOR", &docursor, 0x43, "CURSOR [OVERWRITE|INSERT] [b+|b-] [rate]\n\
Turn cursor blink on/off, change rate, or set editing mode.",
    "DEBUG", &dodebug, 0x42, &hlp_deb[0],
    "DEL", &dorm, 0x42, &hlp_del[0],
    "DELETE", &dorm, 0x42, &hlp_del[0],
    "DIR", &dols, 0x45, &hlp_dir[0],
    "DUP", &dodup, 0x41, "DUP [lines]\nCopy lines from current input\
 device to current output device.",
    "ECHO", &doecho, -1, "ECHO [nl-] parm1 parm2...\nDisplay parameter(s).\
\"nl-\" supresses newline.",
    "ELSE", &doelse, 0, "ELSE\nPart of IF-ELSEIF-ELSE construct.",
    "ELSEIF", &doelseif, 1, &hlp_els[0],
    "ENDFOR", &doforend, 0, "ENDFOR\nEnd a FOR loop.",
    "ENDIF", &doendif, 0, "ENDIF\nTerminates IF-ELSEIF-ELSE construct.",
    "ENDWHILE", &dowhilend, 0, "ENDWHILE\nEnd a WHILE loop.",
    "ERASE", &dorm, 0x42, &hlp_del[0],
    "EXIT", &doexit, 0x42, "EXIT [errnum [errmsg]]\nTerminate batch file with \
optional error number and error mssage.",
    "FONT", &dofont, 1, "FONT 0|1|2\nChange display font number.",
    "FOR", &dofor, 0x43, "FOR [varname IN value1[..value2]]\nValues can be \
numbers or single characters. Value1 can also be a string.",
    "FORMAT", &doformat, 0x45, &hlp_fmt[0],
    "GEMMSG", &dogemmsg, -1, &hlp_gem[0],
    "GETCH", &dogetch, 0x42, &hlp_getc[0],
    "GETSTR", &dogetstr, 0x42, &hlp_get[0],
    "GOTO", &dogoto, 1, "GOTO label\nJump to line containing label.",
    "HELP", &dohlp, 0x41, &hlp_hlp[0],
    "IF", &doif, 1, &hlp_els[0],
    "KICK", &dokick, 0x41, "KICK [driveletter]\nForce TOS to think the media\
 of a disk drive has changed.",
    "LOADINF", &doloadinf, 1, "LOADINF filespec\nLoad NeoDesk information \
file.",
    "LS", &dols, 0x45, &hlp_dir[0],
    "MD", &domkdir, 1, &hlp_mkd[0],
    "MENU", &domenu, 0, "MENU\nStart or end interactive menu block.",
    "MKDIR", &domkdir, 1, &hlp_mkd[0],
    "MORE", MORE_FUNC, -1, "MORE command...\nExecute command with paging.",
    "MOUSE", &domouse, 0x42, &hlp_mou[0],
    "MOVE", &domv, 0x44, &hlp_mov[0],
    "MV", &domv, 0x44, &hlp_mov[0],
    "NULL", &donull, -1, "NULL parm1 parm2...\nPerform no action. Used for \
discarding function returns.",
    "ONERR", &doonerr, 1, "ONERR label\nJump to label if error occurs.",
    "ONQUIT", &doonquit, 1, "ONQUIT label\nJump to label if Ctrl-C pressed.",
    "PAUSE", &dopause, 0, "PAUSE\nWait for a key to be pressed.",
    "POP", &dopop, 0, "POP\nRemove the last subroutine RETURN address.",
    "PRINT", &doprint, 1, "PRINT filespec\nPrint files to the printer.",
    "PROMPT", &doprompt, 1, &hlp_pro[0],
    "PWD", &dopwd, 0, "PWD\nPrint working directory.",
    "QUITNEO", &doquitneo, 0, "QUITNEO\nQuit NeoDesk.",
    "REFORMAT", &doreformat, 0x44, &hlp_rfmt[0],
/*  "READ", &doread, 2, "READ varname length\nRead bytes from current input \
device.", */
    "RENAME", &dorn, 2, &hlp_ren[0],
    "REPEAT", &dorepeat, 0, "REPEAT\nExecute commands until UNTIL fails.",
    "RETURN", &doreturn, 0, "RETURN\nReturn from subroutine CALL.",
    "RM", &dorm, 0x42, &hlp_del[0],
    "RMDIR", &dormdir, 1, "RMDIR path\nRemove empty folder.",
    "RN", &dorn, 2, &hlp_ren[0],
    "SELECT", &doselect, 0x43, &hlp_sel[0],
    "SET", &doset, -1, "SET [varname [value...]]\nCreate or modify variable with \
new contents.",
    "SETENV", &dosetenv, -1, "SETENV [name [value...]]\nCreate or modify \
environmental variable. Name is case-sensitive.",
    "SETRS", &dosetrs, 5, &hlp_rs[0],
    "SHIFT", &doshift, 0, "SHIFT\nShift $2 into $1, $3 into $2, etc.",
    "SHOW", &doshow, 1, &hlp_sho[0],
    "TEXT", &dotext, 1, "TEXT file\nCreate a text file.",
    "TIME", TIME_FUNC, -1, "TIME command...\nShow elapsed time of command.",
    "TOUCH", &dotouch, 1, "TOUCH filespec\nUpdate timestamp of files.",
    "TYPE", &docat, 1, &hlp_cat[0],
    "UNALIAS", &dounalias, 1, "UNALIAS name|*\nRemove alias. Use * to remove all.",
    "UNSET", &dounset, 1, "UNSET varname|*\nRemove variable. Use * to \
remove all user-defined variables.",
    "UNSETENV", &dounsetenv, 1, "UNSETENV varname|*\nRemove environmental variable.\
 Use * to remove all. Name is case-sensitive.",
    "UNTIL", &dountil, 1, "UNTIL statement\n\
Execute commands after REPEAT until statement is TRUE.",
    "VT", &dovt, 0, "VT\nInvoke VT52 emulator. Undo key quits.",
    "WARMBOOT", &dowarm, 0, "WARMBOOT\n\
Restarts the computer as if the Reset button had been pressed.",
    "WHEREIS", &dowhere, -1, &hlp_whe[0],
    "WHILE", &dowhile, 1, "WHILE statement\n\
Execute commands up to WHILEND while statement is TRUE.",
    "WINDOW", &dowind, 0x44, &hlp_win[0],
    NULL };

char *fptr;
int flen, flen0;
long fpos, floc;
typedef union
{
  unsigned int old_method;
  struct
  {
    unsigned pexec_mode:8;
    unsigned set_me:1;
    unsigned reserved:1;
    unsigned return_status:1;
    unsigned show_status:1;
    unsigned clear_screen:1;
    unsigned npg:1;
    unsigned tos:1;
    unsigned takes_params:1;
  }new_method;
} _PROG_TYPE;

#include "neo_clid.h"

void wmode0(void)
{
  if( vdi_hand>=0 ) (*graphics->wmode0)();
  else WMODE=0;
}
void wmode2(void)
{
  if( vdi_hand>=0 ) (*graphics->wmode2)();
  else WMODE=2;
}
void hide_mouse(void)
{
  if( vdi_hand>=0 ) (*graphics->hide_mouse)();
  else lineaa();
}
void show_mouse(void)
{
  if( vdi_hand>=0 ) (*graphics->show_mouse)();
  else linea9();
}

void force_cono( int flag )
{
  static int old;
  int *i;

  i = &redir_hand[parse_lev];
  if( !flag )
  {
    old = *i;
    *i = 2;
  }
  else *i = old;
}

void cdecl spf( char *buf, char *fmt, ... )     /* call NeoMaster's sprintf() */
{
  call_w_save( (int (*)())neo_acc->mas->dopf, buf, fmt, &... );
}

char *pathend( char *ptr )
{
  char *ret;

  if( (ret = rindex(ptr,(int)'\\')) != NULL ) return(ret+1);
  return(*(ptr+1)==':' ? ptr+2 : ptr);
}

int min( int a, int b)
{
   if( a > b )
      return( b );
   else
      return( a );
}

int max( int a, int b)
{
   if( a < b )
      return( b );
   else
      return( a );
}

int rc_intersect(Rect *r1, Rect *r2)
{
   int xl, yu, xr, yd;                      /* left, upper, right, down */

   xl      = max( r1->x, r2->x );
   yu      = max( r1->y, r2->y );
   xr      = min( r1->x + r1->w, r2->x + r2->w );
   yd      = min( r1->y + r1->h, r2->y + r2->h );

   r2->x = xl;
   r2->y = yu;
   r2->w = xr - xl;
   r2->h = yd - yu;

   return( r2->w > 0 && r2->h > 0 );
}

void ferrs( int num, char *fmt, ... )
{
  char buf[200];

  call_w_save( (int (*)())neo_acc->mas->dopf, buf, fmt, &... );
  errs( num, buf );
}

int retries;
int find_neo( int *buf )
{
  /* throw away if too many tries */
  if( ++retries>10 )
  {
    open_it();	/* just do error alert */
    return 0;
  }
  /* try to find NeoDesk */
  if( (neo_apid = appl_find("NEODESK ")) >= 0 )
  {
    /* put the message back in my queue */
    appl_write( apid, 16, buf );
    buf[0] = NEO_ACC_ASK;         /* 89 */
    buf[1] = apid;
    buf[3] = NEO_ACC_MAGIC;
    buf[4] = apid;
    appl_write( neo_apid, 16, buf );
  }
  else
  {
    open_it();	/* just do error alert */
    return 0;
  }
  return 1;
}

int neo_write( int hand, int size, int *buf )
{
  if( neodesk_present )
  {
    buf[1] = apid;
    appl_write( hand, size, buf );
    return(1);
  }
  else
  {
    errs( GENERAL, "This function is not available because NeoDesk\
 is not the current application." );
    return(0);
  }
}

long bFseek( long pos )
{
  int i;

  retried = 0;
  if( pos < (i=fpos+flen) && pos >= i-flen0 )
  {
    fptr -= (i=fpos-pos);
    flen += i;
    return(fpos=pos);
  }
  else
  {
    flen = 0;
    return( Fseek( fpos=pos, batch_hand, 0 ) );
  }
}

int tos_error( long l )
{
  int ret, v;

  v = mouse_vex(1);
  ret = call2_w_save( (int (*)())neo_acc->TOS_error, l, 0 );
  if(v) mouse_vex(0);
  return ret;
}

int get_line( char *diskbuff )
{
  int strt=1, file, quit=0;

  if( new_batch ) new_batch = retried = flen = fpos = 0L;
  while(!quit)
  {
    if( !flen )
    {
      if( retried ) return(1);
      retried++;
reread:
      flen = Fread( batch_hand, (long)config.batch_buf, line_buf );
      if( flen == AEIHNDL )
        if( (file=Fopen(bpath,0)) >= 0 )
        {
          batch_hand = file;
          if( Fseek( fpos, batch_hand, 0 ) == fpos ) goto reread;
          return(1);
        }
        else flen=file;         /* next if becomes true */
      if( flen < 0 )
      {
        tos_error( flen );
        return(1);
      }
      if( flen )
      {
        retried = 0;
        flen0 = flen;
        fptr = line_buf;
      }
    }
    if( !flen || *fptr=='\r' )
    {
      *diskbuff = '\0';
      strt = 1;
      quit++;
    }
    else if( strt && *fptr!='\n' )
    {
      strt = 0;
      floc = fpos;
    }
    if( !strt ) *diskbuff++ = *fptr;
    if( flen )
    {
      fptr++;
      flen--;
      fpos++;
    }
  }
  return(0);
}

int _abort(void)
{
  char c;

  for(;;)
    if( ctrlc )
    {
      ctrlc=0;
      if( in_ctrlc ) return(0);
      if( quit_label[0] )
      {
        parmptr[1] =quit_label;
        if( call_lev < CALLS )
        {
          line_num--;
          bFseek( floc );
        }
        in_ctrlc++;
        docall();
      }
      else
      {
        c = err_label[0];
        err_label[0] = '\0';
        errs( AEINVFN, "Control-C pressed." );
        if( in_batch ) err_label[0] = c;
        else alias_ptr = 0L;
      }
      return(!in_batch);
    }
    else if( !is_ctrlc() ) return(0);
    else ctrlc++;
}

int check_drv( int drv, int flg )
{
  if( drv>'Z' || drv<'A' || !(Drvmap()&(1<<drv-'A')) )
  {
    if( flg ) ferrs( AEUNDEV, "Unknown disk drive %c:", drv );
    return(0);
  }
  return(1);
}

void force_debug( int flag )
{
  static int hand;
  int *i;

  i = &redir_hand[parse_lev];
  if( !flag )
  {
    hand = *i;
    *i = debug_hand;
  }
  else *i = hand;
}

/*void force_debugi( int flag )
{
  static int hand;
  int *i;

  i = &iredir_hand[parse_lev];
  if( !flag )
  {
    hand = *i;
    *i = 2;
  }
  else *i = hand;
}*/

int matchh( char *str, char *pat );

int first( char *ptr, char *msg )
{
  register DTA *o_dma;
  register int ret=1, i;
  char temp[120], *end;

  has_wild = 0;
  if( strlen(end=pathend(ptr)) >= REGEXP_LEN )
  {
    ferrs( SYNTAX, too_lng, "Regular expression" );
    return(0);
  }
  strcpy( temp, ptr );
  *(end-ptr+temp) = '\0';
  strcpy( sf, end );
  if( (i=index( sf, '[' ) || index( sf, '{' )) != 0 ||
      index( sf, '*' ) || index( sf, '?' ) )
  {
    if( !msg ) has_wild++;
    if( i ) strcat( temp, "*.*" );
    else strcat( temp, end );
  }
  else strcat( temp, end );
  o_dma = Fgetdta();
  Fsetdta( &dma );
  get_path(temp);
  if( check_drv(*path,1) )
  {
    if( (i=Fsfirst( temp, 0x37 )) == AEFILNF ) ret=0;
    else if( i<0 ) ret = -1;
    else while( !strcmp( dma.d_fname, period ) ||
        !strcmp( dma.d_fname, pperiod ) )
      if( Fsnext() )
      {
        ret=0;
        goto bad;
      }
    if( (i=matchh( dma.d_fname, sf )) == 0 ) ret = next();
bad:if( i<0 ) ret = -1;
    else if( !ret )
    {
      if( msg!=(char *)-1L )
      {
        if( msg ) errs( GENERAL, msg );
        else ferrs( AEFILNF, nf, empty, ptr );
        if( debug )
        {
          force_debug(0);
          newline();
          force_debug(1);
        }
      }
    }
    else if( ret>0 && has_wild )
    {
      gtext( dma.d_fname );
      newline();
    }
  }
  else ret=-1;
  Fsetdta( o_dma );
  return(ret);
}

int next()
{
  DTA *o_dma;
  int ret, i;

  o_dma = Fgetdta();
  Fsetdta( &dma );
  while( (ret = !Fsnext()) != 0 )
    if( (i=matchh( dma.d_fname, sf )) > 0 )
    {
      if( has_wild )
      {
        gtext( dma.d_fname );
        newline();
      }
      break;
    }
    else if( i<0 )
    {
      ret = 0;
      break;
    }
  Fsetdta( o_dma );
  return(ret);
}

void get_path( char *ptr )
{
  char *p;
  int i;

  uppercase(ptr);
  p=rindex(ptr,'\\');
/*  if( (p=rindex(ptr,'\\')) != NULL && *(ptr+1)==':' )
  {
    path[0] ='\0';
    if( *(ptr+2) != '\\' )         /* something like "e:foo\foo" ?? */
    {
      path[0] = *ptr;
      strcpy( path+1, cslash );
      ptr += 2;
    }
  }
  else */ if( *(ptr+1)==':' )             /* something like "e:foo" ?? */
  {
    path[0] = *ptr;
    /* strcpy( path+1, cslash ); */
    path[1] = ':';
    ptr+=2;
    if( *ptr!='\\' )
    {
      Dgetpath( path+2, path[0]-'A'+1 );
      strcat( path, slash );
    }
    else path[2]='\0';
  }
  else                                  /* no drive letter, no path */
  {
    path[0] = dflt_path[0];
    path[1] = ':';
    if( *ptr != '\\' )
    {
      Dgetpath( path+2, path[0]-'A'+1 );
      strcat( path, slash );
    }
    else path[2] = '\0';
/*    if( strcmp(ptr,slash) ) strcat( path, slash );  */
    if( !strcmp( ptr, period ) ) *ptr='\0';
  }
  strcat( path, ptr );
  if( (!p || p && *(p+1)) && is_dir(ptr) )
  {
    if( *(path+strlen(path)-1) != '\\' ) strcat( path, slash );
  }
  else if( (ptr=rindex(path,'\\')) != NULL ) *(ptr+1) = '\0';
  p = path;
  i = 0;
  while( (ptr=index(p,'\\')) != NULL )
    if( !strncmp( ++ptr, "..\\", 3 ) )
      if( i ) strcpy( p, ptr+3 );
      else strcpy( ptr, ptr+3 );
    else if( !strncmp( ptr, ".\\", 2 ) )
        strcpy( ptr, ptr+2 );
    else
    {
      p = ptr;
      i++;
    }
}

int is_dir( char *p )
{
  register int ret;
  register DTA *o_dma;
  DTA dma;
  char *ptr;

  if( !*p || rindex( p, '*' ) || rindex( p, '?' ) ) return(0);
  if( !strcmp( ptr=pathend(p), pperiod ) || !strcmp( ptr, period ) )
      return(1);
  o_dma = Fgetdta();
  if( *(p+1) != ':' || check_drv(*p,0) )
  {
    Fsetdta( &dma );
    ret = !Fsfirst( p, S_IJDIR ) && dma.d_attrib==S_IJDIR;
    Fsetdta( o_dma );
  }
  else ret=0;
  return(ret);
}

char *next_str( char *ptr )
{
  return( ptr + strlen(ptr) + 1 );
}

typedef struct
{
  int cliparray[4],             /* x0, x1, y0, y1 of screen */
      pallette[4],              /* color pallette */
      conf_copy,                /* !=0 : confirm file copies */
      conf_del,                 /* !=0 : confirm file deletes */
      xx1,
      tos_pause,                /* !=0 : pause after TOS apps */
      use_master,               /* !=0 : use Master to execute */
      xx2[9],
      sort_type,
      status_on,                /* !=0 : display status boxes */
      ctrlZ,                    /* Control-Z mode for file display is on */
      xx3[32],                  /* length may change */
      xx4[10],
      showicon,                 /* show as icons */
      stlgsml,                  /* !=0 : large text */
      stcolumn,                 /* !=0 : 1+ text columns */
      sizdattim[3],             /* [0]:show size, [1]:show date, [2]:time */
      files_fold;               /* max. number of files per folder */
  unsigned long pic_ptr;        /* points to desktop picture; 0L means none */
  char rsc_name[13],            /* resource file name */
       dflt_path[120],          /* NeoMaster's path */
       neo_path[120],           /* NeoDesk's path */
       batch_name[33],          /* batch file interpreter */
       autoexec[33],            /* program to autoexecute */
       inf_name[13],            /* default INF file first loaded */
       tail[130],               /* program tail */
       env[620],                /* environmental variables */
       xx5[130];
  Rect maximum,                 /* x, y, w, h of screen (like cliparray) */
       max_area;                /* x, y, w, h of desktop itself (w/o menu) */
} MOST205;

char *env_ptr(void)
{
  return( neo_acc->nac_ver==NAC_VER_205 ? ((MOST205 *)neo_acc->mas->most)->env :
      ((MOST *)neo_acc->mas->most)->env );
}

void set_PATH(void)
{
  char *ptr=env_vars;

  while( *ptr )
  {
    if( !strncmp( ptr, "PATH=", 5 ) )
    {
      strcpy( srch_path, ptr+5 );
      return;
    }
    ptr = next_str(ptr);
  }
}
/*
void getPATH( char **ptr )
{
  get_env( ptr, "PATH=" );
}

void get_env( char **ptr, char *var )
{
  register int l;

  l = strlen(var);
  *ptr = neo_acc->nac_ver==NAC_VER_205 ? ((MOST205 *)neo_acc->mas->most)->env :
      ((MOST300 *)neo_acc->mas->most)->env;
  if( l ) while( *(*ptr+1) )
    if( !strncmp( *ptr, var, l ) )
    {
      *ptr += l;
      return;
    }
    else *ptr = next_str(*ptr);
}
*/

void ack()
{
  int buf[8];

  buf[0] = DUM_MSG;
  buf[1] = apid;
  appl_write( neo_apid, 16, buf );
}

void intswap( void *ptr1, void *ptr2, int num )
{
  int i;

  while( num-- )
  {
    i = *(int *)ptr1;
    *((int *)ptr1)++ = *(int *)ptr2;
    *((int *)ptr2)++ = i;
  }
}

char *get_prompt(void)
{
  int i=0;
  char *p, temp[sizeof(cmdbuf)];

  strcpy( temp, cmdbuf );
  strcpy( cmdbuf, vprompt );
  parms=0;
  variables( cmdbuf, &i, &p, 0 );
  if( !i )
  {
    strcpy( diskbuf, p );
    strcpy( cmdbuf, temp );
  }
  else strcpy( diskbuf, "> " );
  return(diskbuf);
}

void prompt()
{
  int i = redir_hand[parse_lev];
  void igtext( char *str );
  void vcurs_on( int force );

  redir_hand[parse_lev] = 2;
  ctrlc=0;
  if( eol && gt || col ) newline();
#ifdef DEMO
  igtext( "DEMO" );
  gtext( " " );
#endif DEMO
  get_prompt();
  echo(diskbuf);
  zero_cmd();
  if( (redir_hand[parse_lev] = i) == 2 ) vcurs_on(1);
  gt=0;
}

void backsp()
{
  if( --col < 0 )
  {
    col = maxcol;
    row--;
  }
}

extern void vcursor();
#define nvbls *(int *)0x454
#define vblptr *(long **)0x456

void vbl( void func() )
{
  static long *addr;
  long stack, *l;
  int i;

  stack = Super(0L);
  if( func )
  {
    for( i=nvbls, l=vblptr; --i>=0; l++ )
      if( !*l )
      {
        *(addr = l) = (long)func;
        break;
      }
  }
  else if( addr )
  {
    *addr = 0L;
    addr = 0L;
  }
  Super((void *)stack);
}

extern char in_vbl;
char old_show, curs_hide;

void vcurs_off(void)
{
  void vcur( int i );

  if( !curs_hide++ )
    if( (old_show = show_curs) != 0 )
    {
      vcur( in_vbl = 1 );
      in_vbl = show_curs = 0;
    }
}

void vcurs_on( int force )
{
  void vcur( int i );

  if( force || !--curs_hide )
  {
    in_vbl = 1;
    if( (show_curs = force ? 1 : old_show) != 0 ) vcur(0);
    in_vbl = 0;
    curs_hide = 0;
  }
  else if( curs_hide<0 ) curs_hide = 0;
}

#define CURSCONF *(char *)(la_init.li_a0-6)
long get_timer(void)
{
  return( *(long *)0x4ba );
}

unsigned char blink_rate, blink_on;
static int lastrow, lastcol;

void vdi_cursor( int force_off )
{
  static long lasttime;
  long curtime;
  char new_state;
  int arr[4];

  if( vdi_hand>=0 && show_curs && is_top )
  {
    if( !force_off )
    {
      if( blink_on ) curtime = Supexec(get_timer);
      if( blink_on && col==lastcol && row==lastrow )
      {
        new_state = curtime - lasttime >= blink_rate*10/3 ?
            !curs_is_on : curs_is_on;
      }
      else new_state = 1;
    }
    else if( (new_state = !force_off) != 0 ) curs_is_on=0;
    if( new_state != curs_is_on )
    {
      (*graphics->hide_mouse)();
      (*graphics->wmode2)();
      arr[2] = (arr[0] = window.x+col*char_w) +
          (insert ? 1 : char_w-1);
      arr[3] = (arr[1] = window.y+row*char_h) + char_h-1;
      (*neo_acc->blank_box)( arr );
      (*graphics->show_mouse)();
      if( blink_on ) lasttime = curtime;
      lastcol = col;
      lastrow = row;
      curs_is_on = new_state;
    }
  }
}

void vcur( int force_off )
{
  static char count;
  static int wtbl[3]={ 16-6, 16-8, 16-8 },
      mtbl[3]= { 63, 255, 255 };
  char blink, new_state;
  int *scr, *scr0, x, mask, mask2, mwid, i, j;

  if( show_curs && is_top )
  {
    if( vdi_hand>=0 )
    {
      vdi_cursor(force_off);
      return;
    }
    blink = CURSCONF&1;
    if( !force_off && col==lastcol && row==lastrow )
        new_state = blink ? (!--count ? !curs_is_on : curs_is_on) : 1;
    else if( (new_state = !force_off) != 0 ) curs_is_on=0;
    if( new_state != curs_is_on )
    {
      hide_mouse();
      x = window.x+col*char_w;
      scr0 = (int *)(logbase+(long)(window.y+row*char_h)*V_X_WR) +
          (x>>4)*VPLANES;
      if( insert )
      {
        mwid = 16-2;
        mask = 3;
      }
      else
      {
        mwid = wtbl[fontno];
        mask = mtbl[fontno];
      }
      x &= 0xF;
      if( x > mwid )
      {
        mask2 = mask<<(16-x+mwid);
        mask >>= x-mwid;
      }
      else
      {
        mask <<= mwid-x;
        mask2 = 0;
      }
      x = VPLANES<<1;
      for( j=-1; ++j<char_h; (char *)scr0+=V_X_WR )
        for( scr=scr0, i=-1; ++i<VPLANES; scr++ )
        {
          *scr ^= mask;
          *((int *)((char *)scr+x)) ^= mask2;
        }
      show_mouse();
      count = V_CUR_CNT;
      lastcol = col;
      lastrow = row;
      curs_is_on = new_state;
    }
  }
}

void ring_bell(void)
{
  Bconout(2,7);
}

void set_lacurs(void)
{
  if( prog_text )
  {
    V_CUR_CX = col;
    V_CUR_CY = row;
  }
}

char *get_scr( int row, int col )
{
  int line, max;

  max = config.screen_size / scrn_wid;
  if( (line = (scrn_ptr-screen) / scrn_wid + row) >= max ) line -= max;
  return( screen+line*scrn_wid+col );
}

int check_io( int func( char *ptr ) )
{
  unsigned int h, t;
  int i;

  if( (h=io->ibufhd) != (t=io->ibuftl) )
  {
    if( (h+=4) >= io->ibufsiz ) h = 0;
    if( t >= io->ibufsiz ) t = 0;
    if( (i = (*func)( (char *)io->ibuf+h )) != 0 )
    {
      io->ibufhd = h;
      return(i);
    }
    if( (i = (*func)( (char *)io->ibuf+t )) != 0 )
    {
      io->ibufhd = io->ibuftl;
      return(i);
    }
  }
  return(0);
}

int is_cqcs( char *ptr )
{
  if( ign_ctrlc ) return(0);
  switch( *(ptr+3) )
  {
    case '\023':
      return(1);
    case '\021':
      return(2);
    case '\03':
      if( !in_ctrlc && !in_vt ) return(3);
    default:
      return(0);
  }
}

int gtext( char *string )
{
  int x, y, max, not_end, px[4], i, l;
  char is_nl, paused=0, last;
  char *ptr, *ptr2;

  last_esc = 0;
  if( ctrlc ) return(0);
  if( redir_hand[parse_lev] != 2 ) return( write_redir( string ) );
  call_w_save( (int (*)())neo_acc->set_clip_rect, &clip, 1 );
  hide_mouse();
  vcurs_off();
  ptr = string-1;
  do
  {
    if( !gt_ign_cr )
    {
      ptr = index( string=ptr+1, '\n' );
      ptr2 = index( string, '\r' );
    }
    else ptr=ptr2=0;
    is_nl = 0;
    if( ptr && (ptr<ptr2 || !ptr2) ) is_nl++;
    else ptr = ptr2;
    if( ptr )
    {
      last = *ptr;
      *ptr = '\0';
    }
    do
    {
      x = col*char_w + window.x;
      y = row*char_h + window.y;
      do
        switch( check_io( is_cqcs ) )
        {
          case 1:
            paused = 1;
            break;
          case 2:
            paused = 0;
            break;
          case 3:
            ctrlc++;
            more=0;
            newline();
cc:         set_lacurs();
            show_mouse();
            vcurs_on(0);
            return(0);
        }
      while( paused );
      not_end = 0;
      if( (l = strlen(string)) != 0 )
      {
        max = maxcol - col + 1;
        if( (not_end = l>max) != 0 )
          if( (l = max) <= 0 ) goto wrp;
        call_w_save( (int (*)())neo_acc->gtext, x, y, string, fontno, 0 );
        if( scrn_ptr )
        {
          memcpy( ptr2=get_scr( row, col ), string, l );
          for( ptr2+=max, i=col+l; --i>=col; )
            if( !inverse ) *(ptr2+(i>>3)) &= ~(1<<(i&7));
            else *(ptr2+(i>>3)) |= 1<<(i&7);
        }
        if( inverse )
        {
          wmode2();
          px[2] = (px[0] = x) + l*char_w - 1;
          px[3] = (px[1] = y) + char_h - 1;
          call_w_save( (int (*)())neo_acc->blank_box, px );
        }
        if( not_end )
        {
wrp:      if( !discard )
          {
            newline();
            if( ctrlc ) goto cc;
          }
          else col = maxcol;
          string += max;
        }
        else col += l;
      }
    }
    while( not_end );
    if(ptr)
    {
      *ptr = last;
      if( is_nl )
      {
        newline();
        if( ctrlc ) goto cc;
      }
      else col=0;
    }
  }
  while(ptr);
  set_lacurs();
  show_mouse();
  vcurs_on(0);
  gt_ign_cr=0;
  return(1);
}

void exit_stat(void)
{
  spf( diskbuf, "\nExit status %d.\n", errnum );
  gtext( diskbuf );
}

void close_redir(void)
{
  int i;

  if( parse_lev )
    if( (i=redir_hand[parse_lev]) > 5 && i != iredir_hand[parse_lev] && i !=
        redir_hand[parse_lev-1] && i != iredir_hand[parse_lev-1] ) Fclose(i);
  redir_hand[parse_lev] = parse_lev ? redir_hand[parse_lev-1] : 2;
}

void close_iredir(void)
{
  int i;

  if( parse_lev )
    if( (i=iredir_hand[parse_lev]) > 5 && i != redir_hand[parse_lev] && i !=
        redir_hand[parse_lev-1] && i != iredir_hand[parse_lev-1] ) Fclose(i);
    else ichar_cr[parse_lev-1] = ichar_cr[parse_lev];
  iredir_hand[parse_lev] = parse_lev ? iredir_hand[parse_lev-1] : 2;
}

void close_eredir(void)
{
  int i;

  if( parse_lev )
    if( (i=eredir_hand[parse_lev]) > 5 && i != redir_hand[parse_lev] && i !=
        redir_hand[parse_lev-1] && i != eredir_hand[parse_lev-1] ) Fclose(i);
  eredir_hand[parse_lev] = parse_lev ? eredir_hand[parse_lev-1] : 2;
}

int write_redir( char *string )
{
  long x;
  int rh, h;

  if( (rh=redir_hand[parse_lev]) > 5 )
  {
    x = bcon_nul ? bcon_nul : strlen(string);
    if( Fwrite( rh, x, string ) != x )
    {
      close_redir();
      errs( GENERAL, "Error writing redirection file. Disk full?" );
      return(0);
    }
    return(1);
  }
  else if( rh<0 )       /* to user var */
  {
    rh = -rh-1;
    h = strlen(vvals[rh].c);
    strncpy( vvals[rh].c+h, string, VAR_LEN-h );
    return(1);
  }
  else
  {
    h = rh==3 || rh==4 ? 7-rh : rh;     /* avoid bug in ROMs */
    if( h != 5 ) while( *string )               /* ignore NULL: */
    {
      while( !Bcostat(h) )
        if( _abort() )
        {
          redir_hand[parse_lev] = rh = 2;
          return(0);
        }
      Bconout( rh, *string++ );
    }
    return(1);
  }
}

void blank_box( int *px )
{
  wmode0();
  hide_mouse();
  call_w_save( (int (*)())neo_acc->blank_box, px );
  show_mouse();
}

void erase_lines( int slin, int elin )
{
  int px[4], i, j;
  char *ptr;

  if( slin <= elin )
  {
    if( scrn_ptr )
      for( i=slin-1; ++i<=elin; )
      {
        for( ptr=get_scr( i, 0 ), j=-1; ++j<=maxcol; )
          *ptr++ = ' ';
        while( j++<scrn_wid ) *ptr++ = '\0';
      }
    call_w_save( (int (*)())neo_acc->set_clip_rect, &clip, 1 );
    px[0] = px[2] = window.x;
    px[1] = (px[3] = window.y) + slin*char_h;
    px[2] += window.w - 1;
    px[3] += elin*char_h + char_h - 1;
    blank_box(px);
  }
}

void blit_lines( int froms, int frome, int tos, int erase )
{
  Rect box1, box2;
  int i;

  if( froms <= frome )
  {
    box1 = box2 = window;
    box1.h = box2.h = (frome-froms+1) * char_h;
    box1.y += char_h*froms;
    box2.y += char_h*tos;
    call_w_save( (int (*)())neo_acc->blit, &box1, &box2, 0, 3, 0L );
    if( erase )
    {
      i = tos<froms ? frome : froms;
      erase_lines( i, i );
    }
  }
}

void erase_ln( int scol, int ecol )
{
  int px[4], i;
  char *ptr, *ptr0;

  if( scrn_ptr )
  {
    ptr0 = (ptr = get_scr( row, 0 )) + maxcol + 1;
    for( ptr+=(i=scol); i<=ecol; i++ )
    {
      *ptr++ = ' ';
      *(ptr0+(i>>3)) &= ~(1<<(i&7));
    }
  }
  px[0] = (px[2]=window.x) + scol*char_w;
  px[1] = px[3] = window.y + row*char_h;
  px[2] += ecol*char_w + char_w - 1;
  px[3] += char_h - 1;
  blank_box(px);
}

void dec_s( char **ptr )
{
  if( (*ptr -= scrn_wid) < screen ) *ptr +=
      config.screen_size/scrn_wid*scrn_wid;
}

int dec_scrn(void)
{
  if( scrn_ptr != scrn_top )
  {
    dec_s( &scrn_ptr );
    dec_s( &scrn_bot );
    return(1);
  }
  return(0);
}

void inc_scrn( char **ptr )
{
  if( (*ptr += scrn_wid) > screen+config.screen_size-scrn_wid )
      *ptr = screen;
}

void inc_scrnbot(void)
{
  if( scrn_bot==scrn_top ) inc_scrn( &scrn_top );
  inc_scrn( &scrn_bot );
}

void newline(void)
{
  void igtext( char *str );
  char c;
  int r;

  if( ctrlc ) return;
  if( (r=redir_hand[parse_lev]) != 2 )
  {
    write_redir( r<0 ? "\n" : "\r\n" );	/* just \n if to variable */
    return;
  }
  vcurs_off();
  call_w_save( (int (*)())neo_acc->set_clip_rect, &clip, 1 );
  col = 0;
  if( row >= maxrow )
  {
    if( scrn_ptr )
    {
      inc_scrn( &scrn_ptr );
      inc_scrnbot();
    }
    blit_lines( 1, maxrow, 0, 1 );
    oldrow--;
  }
  else row++;
  if( more )
    if( --more_row <= 0 )
    {
      igtext( "<More>" );
      c = bconin(2);
      col=0;
      gtext( "      " );
      col=0;
      switch(c)
      {
        case '\03':
        case '\033':
        case 'Q':
        case 'q':
          ctrlc++;
          break;
        case '\r':
          more_row=1;
          break;
        case '\x1a':    /* ^Z */
          more=0;
          break;
        default:
          more_row=maxrow-1;
      }
    }
  vcurs_on(0);
}

void check_rowcol(void)
{
  if( row>maxrow ) row = maxrow;
  else if( row<0 ) row=0;
  if( col>maxcol ) col = maxcol;
  else if( col<0 ) col=0;
}

void clear_cmd(void)
{
  int j;
  void to_start(void);

  gtext( cmdptr );
  cmdptr += strlen(cmdptr);
  j = row;
  to_start();
  erase_ln( col, maxcol );
  if( row != j ) erase_lines( row+1, j );
}

void clear_screen(void)
{
  int i, j;
  char *ptr;

  row=col=0;
  screen = scrn_ptr = scrn_top = line_buf + config.batch_buf;
  scrn_wid = ((maxcol+8)>>3) + maxcol + 1;
  scrn_bot = screen + (maxrow+1)*scrn_wid;
  if( scrn_wid*maxrow > config.screen_size ) scrn_ptr = 0L;
  else for( i=-1, ptr=scrn_ptr; ++i<=maxrow; )
  {
    for( j=-1; ++j<=maxcol; )
      *ptr++ = ' ';
    while( j++<scrn_wid ) *ptr++ = '\0';
  }
}

void draw_blank( Rect *clip )
{
  int px[4];

  wmode0();
  px[2] = (px[0] = clip->x) + clip->w - 1;
  px[3] = (px[1] = clip->y) + clip->h - 1;
  call_w_save( (int (*)())neo_acc->set_clip_rect, clip, 1 );
  blank_box( px );
}

void zero_cmd(void)
{
  *(cmdptr = cmdbuf) = '\0';
}

void resize( Rect *rect, int prmpt )
{
  Rect old;
  char change=0;
  int nc, nr;

  old = wind_out;
  if( rect->w > maximum.w+1 ) rect->w = maximum.w+2;
  if( rect->h > maximum.h ) rect->h = maximum.h;
  if( rect->x < -1 ) rect->x = -1;
  if( rect->y < maximum.y ) rect->y = maximum.y;
  wind_calc( 1, wind_type, rect->x, rect->y, rect->w, rect->h, &temp_rect.x, &temp_rect.y,
      &temp_rect.w, &temp_rect.h );
  if( temp_rect.w < (MAX_LEN_OTH+1)*char_w ) temp_rect.w =
      (MAX_LEN_OTH+1)*char_w;
  if( temp_rect.h < MIN_H ) temp_rect.h = MIN_H;
  temp_rect.w = (nc = (temp_rect.w+1)/char_w)*char_w;
  temp_rect.h = (nr = (temp_rect.h+1)/char_h)*char_h;
  nc--;
  nr--;
  if( nc != maxcol || nr != maxrow )
  {
    change++;
    zero_cmd();
    scrn_wid = ((nc+8)>>3) + nc + 1;
    maxcol = nc;
    maxrow = nr;
    check_rowcol();
    clear_screen();
  }
  wind_calc( 0, wind_type, temp_rect.x, temp_rect.y, temp_rect.w,
      temp_rect.h, &wind_out.x, &wind_out.y, &wind_out.w, &wind_out.h );
  if( wind_out.x+wind_out.w > maximum.w+2 ) wind_out.x = maximum.w-wind_out.w+2;
  if( wind_out.y+wind_out.h > maximum.y+maximum.h+1 ) wind_out.y =
      maximum.y+maximum.h-wind_out.h+1;
  wind_calc( 1, wind_type, wind_out.x, wind_out.y, wind_out.w, wind_out.h,
      &window.x, &window.y, &window.w, &window.h );
  clip = window;
  rc_intersect( &maximum, &clip );
  if( w_handle>0 )
  {
    wind_set( w_handle, WF_CURRXYWH, wind_out.x, wind_out.y, wind_out.w,
        wind_out.h );
    if( change )
    {
      draw_blank( &clip );
      if( prmpt ) prompt();
    }
    if( old.w < wind_out.w || old.h < wind_out.h ) ignore_redraw = 1;
  }
}

void redraw_lines( int first, int last )
{
  int i, j, y, px[4], l;
  char *ptr, *end, e;

  l = maxcol+1;
  for( y=window.y+char_h*first, i=first-1; ++i<=last; y+=char_h )
  {
    ptr = get_scr(i,0);
    e = *(end=ptr+l);
    *end = 0;
    call_w_save( (int (*)())neo_acc->gtext, window.x, y, ptr, fontno, 0 );
    *end = e;
    for( ptr=end, j=maxcol+1; --j>=0; )
      if( *(ptr+(j>>3)) & (1<<(j&7)) )
      {
        wmode2();
        px[2] = (px[0] = window.x + j * char_w) + char_w - 1;
        px[3] = (px[1] = y) + char_h - 1;
        call_w_save( (int (*)())neo_acc->blank_box, px );
      }
  }
}

void redraw(int prmpt)
{
  int i;

  if( w_handle > 0 && neo_acc )                      /* just in case */
  {                          /* take the rectangles out for a walk */
    vcurs_off();
    curs_is_on = 0;
    eol = in_batch;
    if( neo_acc->nac_ver >= NAC_VER_300 ) *neo_acc->gt_scr_ptr = logbase =
        (long)Logbase();
    i = (maxrow+1)*(maxcol+1) - strlen(get_prompt());
    cmdbufmax = i<CMDBUFLEN ? i : CMDBUFLEN;
    wind_update( BEG_UPDATE );
    wind_get( w_handle, WF_FIRSTXYWH, &temp_rect.x, &temp_rect.y,
        &temp_rect.w, &temp_rect.h );
    hide_mouse();
    while( temp_rect.w && temp_rect.h )
    {
      rc_intersect( &maximum, &temp_rect );
      call_w_save( (int (*)())neo_acc->set_clip_rect, &temp_rect, 1 );
      if( scrn_ptr && !first_open ) redraw_lines( 0, maxrow );
      else draw_blank(&temp_rect);
      wind_get( w_handle, WF_NEXTXYWH, &temp_rect.x, &temp_rect.y,
          &temp_rect.w, &temp_rect.h );     /* get next rectangle */
    }
    call_w_save( (int (*)())neo_acc->set_clip_rect, &clip, 1 );       /* clip the linea */
    if( !scrn_ptr && prmpt && top_wind() )
    {
      row=col=0;
      prompt();
    }
    else if( prmpt && first_open ) prompt();
    first_open=0;
    show_mouse();                             /* mouse on */
    wind_update( END_UPDATE );
    if( !scrn_ptr ) zero_cmd();
    vcurs_on(0);
  }
}

#ifndef DEMO
void set_traps( int flag )
{
  char *ptr;

  if( flag )
  {
    if( old_execute )
    {
      oneo_acc->mas->execute = old_execute;
      old_execute = 0L;
    }
    if( use_argv )
    {
      parmptr[1] = "ARGV";
      find_env( &ptr );
      if( ptr ) *ptr = '\0';
    }
  }
  if( !flag || prog_text )
  {
    t13adr = (long)Setexc( 0x2d, flag ? (void (*)())t13adr : t13 );
    t1adr = (long)Setexc( 0x21, flag ? (void (*)())t1adr : t1 );
  }
}
#endif

int is_sep( int ch )
{
  return( ch && strchr( " \t,\r\n", ch )!=0 );
}

void reset_linea(void)
{
  if( env_saved ) memcpy( env_ptr(), diskbuf, sizeof(env_vars) );
  V_CEL_HT = old_celht;
  V_CEL_MX = old_mx;
  V_CEL_MY = old_my;
  vcurs_off();
  old_show = b_old_show;
  curs_hide = b_old_hide;
  neo_acc->mas->most->tos_pause = old_pause;
}

void dec_parselev(void)
{
  close_redir();
  close_iredir();
  close_eredir();
  parse_lev--;
}

void inc_parselev(void)
{
  int i;

  i = parse_lev++;
  if( ((iredir_hand[i+1] = iredir_hand[i]) > 5 || redir_hand[i]<0 ) &&
      i>0 ) strcpy( input[i], input[i-1] );
  if( ((redir_hand[i+1] = redir_hand[i]) > 5 || redir_hand[i]<0) &&
      i>0 ) strcpy( output[i], output[i-1] );
  if( ((eredir_hand[i+1] = eredir_hand[i]) > 5 || eredir_hand[i]<0) &&
      i>0 ) strcpy( _error[i], _error[i-1] );
  ichar_cr[i+1] = ichar_cr[i];
}

int wild_env( char *name, int prg )
{
  char *ptr, *ptr0, temp[sizeof(path)], *p;
  int i, j, k, env;
  void line_long(void);

  get_path(diskbuf);
  strcat( path, name );
  strcpy( temp, path );
  if( (env = prg && use_argv) != 0 )
  {
    for( ptr=env_vars; *ptr; ptr=next_str(ptr) );
    ptr0 = ptr;
    if( (ptr=add_env( ptr, "ARGV= " )) != 0 && (*(ptr-1)='\0')==0 &&
        (ptr=add_env( ptr, temp )) != 0 &&
        (ptr=add_env( ptr, " " )) != 0 ) *(ptr-1) = '\0';
  }
  else ptr = (char *)1L;
  diskbuf[0] = ' ';
  diskbuf[1] = '\0';
  for( i=1-1; ++i<parms && ptr; )
    if( wc_expand && !escaped[i] && (index(parmptr[i],'[') || index(parmptr[i],'{')
        || index(parmptr[i],'*') || index(parmptr[i],'?')) )
    {
      if( !first(parmptr[i],"No match.") )
      {
        ptr = 0;
        break;
      }
      else
        do
        {
          strcat( diskbuf, " " );
          strcat( diskbuf, dma.d_fname );
          if( env && (ptr=add_env( ptr, dma.d_fname )) != 0 &&
              (ptr=add_env( ptr, " " )) != 0 ) *(ptr-1) = '\0';
        }
        while( next() && ptr );
      if( !prg )
        if( (j=strlen(diskbuf+1)-strlen(parmptr[i])) > CMDBUFLEN+cmdbuf-
            parmptr[parms-1]-strlen(parmptr[parms-1])-1 )
      {
        line_long();
        ptr = 0;
        break;
      }
      else
      {
        if( i != parms-1 )
        {
          p = parmptr[i+1];
          memcpy( p+j, p, parmptr[parms-1]+strlen(parmptr[parms-1])+1-p );
          for( k=i+1; k<parms; k++ )
            parmptr[k] += j;
        }
        strcpy( parmptr[i], diskbuf+1 );
        diskbuf[0] = '\0';
      }
    }
    else
    {
      if( prg )
      {
        strcat( diskbuf, " " );
        strcat( diskbuf, parmptr[i] );
      }
      if( env && (ptr=add_env( ptr, parmptr[i] )) != 0 &&
          (ptr=add_env( ptr, " " )) != 0 ) *(ptr-1) = '\0';
    }
  strcpy( path, temp );
  if( !env && strlen(diskbuf) > 127 ) line_long();
  else if( ptr )
  {
    diskbuf[127] = '\0';
    if( env ) *ptr = *(ptr-1) = '\0';
    return(1);
  }
  if( env ) *ptr0 = '\0';
  return(0);
}

int new_execute( int s_h, int f_h, int o_h )
{
  oneo_acc->mas->execute = old_execute;
  oneo_acc->mas->most->tail[0] = '\x7f';
  return( neo_acc->nac_ver==NAC_VER_205 ? call_w_save( old_execute,
      s_h, f_h, o_h ) : (*old_execute)( s_h, f_h, o_h ) );
}

void timer(void)
{
  if( time_it == parse_lev+1 ) last_time = Supexec(get_timer);
}

void elapsed(void)
{
  long l = last_time, s;

  if( time_it == parse_lev+1 )
  {
    timer();
    l = last_time - l;
    s = l/200;
    spf( cmdbuf, "\nElapsed time %ld.%ld seconds.", s, (l-s*200)>>1 );
    gtext(cmdbuf);
    time_it = 0;
  }
}

char *variables( char *ptr, int *done, char **p3, int flag )
{
  char *ptr3, *ptr2, *ptr4;
  int i, j, k, l, in_comment=0;
  char brace, quote, *p, *dollar;
  static char *var_end;
  static char stops[] = "$\"\'%\\{}[]";

  if( !*ptr ) goto out;
  quote=brace=0;
  dollar=0L;
  ptr3 = ptr;
  if( !parms ) var_end = 0L;
  escaped[parms++] = 0;
  do
  {
    l = *ptr;
    if( ptr >= var_end )
    {
      if( quote!='\'' && (is_sep(l) || index(stops,l) || !l) &&
          !in_comment )
        if( dollar )
        {
          if( insert_var( dollar, &ptr, &var_end, 0 ) )
          {
            ++*done;
            goto out;
          }
          else if( ptr!=var_end )
              while( is_sep(*ptr) && ptr<var_end ) ptr++;
          l = *ptr;
          if( flag && !quote && is_sep(l) ) l=0;
          dollar = 0L;
          continue;
        }
        else if( l == '$' )
        {
          dollar = ptr++;
          continue;
        }
      if( l=='/' && *(ptr+1)=='*' && !quote ) /* C-style comment */
      {
        in_comment = ++ptr-ptr3;
      }
      else if( l=='*' && *(ptr+1)=='/' && !quote ) /* C-style comment */
        if( !in_comment )
        {
          ferrs( SYNTAX, wo, "Comment terminator */", "/*" );
          ++*done;
          parms=0;
          goto out;
        }
        else
        {
          strcpy( ptr4=ptr3+in_comment-1, ptr+2 );
          in_comment = 0;
          l = *(ptr = ptr4);
          while( is_sep(*ptr4) ) ptr4++;
          if( ptr==ptr3 )
            if( !*ptr4 )
            {
              parms--;
              ++*done;
              goto out;
            }
            else l = *(ptr = ptr3 = ptr4);
          if( is_sep(l) ) ptr++;
        }
      else if( !in_comment )
      {
        if( l=='\'' || l=='\"' )
          if( !quote )
          {
            quote = l;
            escaped[parms-1] = 1;
            strcpy( ptr, ptr+1 );
          }
          else if( quote == l )
          {
            quote=0;
            strcpy( ptr, ptr+1 );
          }
          else ptr++;
        else if( quote && l=='\\' )
          if( (ptr2 = index( slashes, *(ptr+1) )) != NULL )
          {
            *ptr = *(ptr2-slashes+slashes2);
            strcpy( ptr+1, ptr+2 );
            ptr++;
          }
          else
          {
            ptr2 = ptr+1;
            i=k=0;
            if( *ptr2 == 'x' )
            {
              ptr2++;
              i++;
            }
            if( (j = is_digit( *ptr2, i )) >= 0 )
            {
              *ptr = 0;
              do
              {
                *ptr = (*ptr << (i+3)) + j;
                ptr2++;
                k++;
              }
              while( k<(!i?3:2) && (j = is_digit( *ptr2, i )) >= 0 );
              strcpy( ++ptr, ptr2 );
            }
            else ptr++;
          }
        else if( l=='{' )
        {
          brace++;
          ptr++;
        }
        else if( l=='}' && brace )
        {
          brace--;
          ptr++;
        }
        else if(l) ptr++;
      }
      else if(l) ptr++;
    }
    else if(l) ptr++;
  }
  while( l && (!flag||quote||in_comment||brace||!is_sep(l)) );
  if( l ) ptr--;
  if( in_comment )
  {
    errs( SYNTAX, "Unterminated comment." );
    parms=0;
    ++*done;
  }
  else if( quote )
  {
    errs( SYNTAX, "Unbalanced quotes." );
    parms=0;
    ++*done;
  }
out:
  *p3 = ptr3;
  return(ptr);
}

int search_exe( char *name )
{
  char *pthptr, *ptr2, *ptr3, *ptr4;
  int l, i;

  pthptr = (l = index(name,'\\') || name[0] && name[1]==':')
      == 0 ? srch_path : empty;
  do
  {
    ptr3 = diskbuf;
    if( !l && *pthptr )
    {
      while( *pthptr && !index( ",; \t", *pthptr ) )
      {
        *ptr3 = *pthptr++;
        to_upper( ptr3++ );
      }
      if( *(ptr3-1) != '\\' && ptr3!=diskbuf ) *ptr3++ = '\\';
      strcpy( ptr4=ptr3, name );
    }
    else
    {
      strcpy( ptr3, name );
      ptr4 = pathend(ptr3);
      pthptr = empty;
    }
    if( (ptr2 = rindex(diskbuf,'.'))==0 || ptr2<rindex(diskbuf,'\\') )
        spf( diskbuf, "%s.{%s}", diskbuf, suff );
    if( _abort() ) return 0;
    else if( (i=first(diskbuf,(char *)-1L)) > 0 )
    {
      while( i && dma.d_attrib & S_IJDIR ) i = next();
      if( i )
      {
        strcpy( ptr4, dma.d_fname );
        return 1;
      }
    }
    else if( i<0 ) return 0;
  }
  while( *pthptr++ );
  return 0;
}

void parse()
{
  int cmdnum, buf[8], done, is_batch, notbad;
  register int i, j, k, l;
  char str[2]="\0", is_if, aliased, *p, *sf2, *ptr4, temp[CMDBUFLEN],
       *ptr=cmdbuf, *ptr2, *ptr3;
  void (*command)(void);
  OPS_STRUCT *os;
  _PROG_TYPE pt;
  SHWRCMD shwrcmd;
  void close_wind(void);

  cmdnum=-1;
  str[0]=is_if=done=aliased=0;
  command=0L;
  ptr = cmdbuf;
  mouse_col = mouseposx;
  mouse_row = mouseposy;
  mouse_but = mousebutstate;
  if( (mouse_col = (mouse_col-window.x)/char_w) < 0 ) mouse_col = 0;
  if( mouse_col > maxcol ) mouse_col = maxcol;
  if( (mouse_row = (mouse_row-window.y)/char_h) < 0 ) mouse_row = 0;
  if( mouse_row > maxrow ) mouse_row = maxrow;
  inc_parselev();
  eol = 1;
  gt = 0;
  set_path();
  parms=is_batch=0;
  notbad=1;
  while( !done && parms<=MAX_PARM )
  {
    done = !*ptr;
    if( parms ) *ptr++ = '\0';
    if( parms==1 )
      if( *parmptr[0] == ';' )
      {
        done++;                               /* comment */
        parms=0;
      }
      else if( *(parmptr[0]+strlen(parmptr[0])-1) == ';' ) parms=0;  /* label */
      else if( !menu )
      {
        if( !escaped[0] && !aliased )
        {
          ptr2 = alias;
          while( *(ptr2+2) || *(ptr2+3) )
          {
            if( !strcmp( ptr2+=2, parmptr[0] ) )
            {
              aliased++;
              if( !done ) strcpy( diskbuf, ptr );
              strcpy( ptr=cmdbuf, next_str(ptr2) );
              strcat( ptr, sp );
              if( !done ) strcat( ptr, diskbuf );
              parms=done=0;
              goto next_parm;
            }
            ptr2 = next_str(next_str(ptr2));
          }
        }
        for( i=0; cmds[i].name != NULL && cmdnum<0; i++ )
          if( !strcmp( parmptr[0], cmds[i].name ) ) cmdnum=i;
        if( cmdnum>=0 )
          if( (command = cmds[cmdnum].func) == MORE_FUNC )
          {
            more = 1;
            more_row = maxrow-1;
            command = 0L;
            cmdnum=-1;
            parms--;
            aliased++;
          }
          else if( command == TIME_FUNC )
          {
            if( !time_it )
            {
              time_it = parse_lev+1;
              timer();
            }
            command = 0L;
            cmdnum=-1;
            parms--;
            aliased++;
          }
          else notbad++;
        else if( in_if<0 || !bad_if[in_if] )
        {
          if( (!strcmp(parmptr[0]+1,":") || !strcmp(parmptr[0]+1,cslash)) )
          {
            parmptr[1] = parmptr[0];
            parms++;
            vcurs_off();
            docd();
            vcurs_on( !in_batch );
            notbad=0;
            parms=0;
          }
          else if( search_exe(parmptr[0]) )
          {
            if( (ptr2=rindex(dma.d_fname,'.')) != NULL ) is_batch =
                !strcmp( ptr2, ".BAT" ) || !strcmp( ptr2, ".BTP" );
          }
          else
          {
            parms=0;
            notbad=0;
            ferrs( GENERAL, nf, "Command ", parmptr[0] );
          }
          if( !notbad ) done++;
        }
      }
    if( !done )
    {
      while( is_sep(*ptr) ) ptr++;
      if( *ptr )
      {
        ptr = variables( ptr, &done, &ptr3, 1 );
        if( parms && !done )
        {
          parmptr[parms-1] = ptr3;
          if( parms==1 && !menu )
            while( ptr3<ptr ) to_upper( ptr3++ );
        }
      }
    }
next_parm:
    ;
  }
  if( parms > MAX_PARM )
  {
    ferrs( SYNTAX, "%s%ss.", tm, parstr );
    parms=0;
  }
  is_if = (i = command==&doelseif || command==&doelse) != 0 || command==&doendif;
  if( (in_if<0 || !bad_if[in_if] && !i || bad_if[in_if]>0 && is_if) &&
      (while_lev<0 || while_line[while_lev]>=0) )
  {
    while( (i=parms-2)>=1 && !escaped[i] )
    {
      func_parm = parmptr[i+1];
      ptr2 = parmptr[i];
      if( !strcmp( ptr2, "->" ) ) parms -= f_redir() ? 2 : parms;
      else if( !strcmp( ptr2, "<-" ) ) parms -= f_iredir() ? 2 : parms;
      else if( !strcmp( ptr2, "->>" ) ) parms -= f_redira() ? 2 : parms;
      else if( !strcmp( ptr2, "<->" ) ) parms -= f_ioredir() ? 2 : parms;
      else if( !strcmp( ptr2, "$>" ) ) parms -= varredir(0) ? 2 : parms;
      else if( !strcmp( ptr2, "$<" ) ) parms -= varredir(1) ? 2 : parms;
      else break;
    }
    for( i=parms; --i>=1; )
      if( parmptr[i][0] == '%' && !escaped[i] )
      {
        ptr2 = next_str(parmptr[i]) - 1;
        p = i<(j=parms)-1 ? parmptr[i+1] : ptr2;
        if( insert_var( parmptr[i], &p, &ptr4, i ) ) break;
        escaped[i]++;
        for( k=i+1-1, j-=parms; ++k<parms; )
        {
          parmptr[k] = parmptr[j+k] - (int)ptr4;
          escaped[k] = escaped[j+k];
        }
      }
    for( j=-1; ++j<=MAX_PREC; )
      for( i=1-1; ++i<parms; )
        for( os=ops; os->name && os->prec<=j && i<parms; os++ )
          if( os->prec==j && !escaped[i] && !strcmp( os->name, parmptr[i] ) )
            if( i==1 /* && os->func!=o_redir && os->func!=o_rediri */ )
            {
              ferrs( SYNTAX, "First%s cannot be %s operator.", parstr, os->name );
              parms=0;
              goto bad_op;
            }
            else if( i==parms-1 )
            {
              ferrs( SYNTAX, "No operand to right of %s operator.", os->name );
              parms=0;
              goto bad_op;
            }
            else if( (*os->func)(i) )
            {
              memcpy( temp, ptr2 = next_str(parmptr[i+1])-1, CMDBUFLEN );
              strcpy( parmptr[i-1], path );
              ptr = parmptr[i-1]+strlen(path);
              memcpy( ptr, temp, CMDBUFLEN-(ptr-cmdbuf) );
              parms-=2;
              for( k=i-1; ++k<parms; )
              {
                parmptr[k] = parmptr[k+2] - (ptr2-ptr);
                escaped[k] = escaped[k+2];
              }
              i--;
            }
            else
            {
              parms=0;
              goto bad_op;
            }
bad_op:
    if( debug && !menu && parms )
    {
      force_debug(0);
      if( in_batch )
      {
        spf( temp, "%d: ", line_num );
        gtext( temp );
      }
      for( i=-1; ++i<parms; )
      {
        gtext(parmptr[i]);
        gtext("ù");
      }
      newline();
      force_debug(1);
    }
  }
  if( parms && notbad && (in_if<0 || !bad_if[in_if] || cmdnum>=0 &&
      (is_if||command==&doif)) && (while_lev<0 || while_line[while_lev]>=0 ||
      cmdnum>=0 && (command==dowhile||command==dowhilend)) )
  {
    i = cmds[cmdnum].parms;
    j = i&0x0F;
    if( cmdnum<0 )
    {
      if( menu )
      {
        if( parms==1 )
          if( !strxcmp( parmptr[0], "MENU", 5 ) ) domenu();
          else echo( parmptr[0] );
        else if( parms==2 ) add_menu();
        else errs( SYNTAX, "A MENU block requires either text or keys \
and labels." );
      }
      else if( is_batch )
      {
        strcpy( temp, dma.d_fname );
        if( wild_env( temp, 0 ) )
        {
          timer();
          switch( open_batch(path) )
          {
            case 1:
              ferrs( AEFILNF, nf, "Batch file ", path );
              break;
            case 2:
              in_batch++;
              new_batch++;
              inc_parselev();
              newline();
              line_num = lastline = 0;
              vcurs_off();
          }
        }
      }
      else
      {
#ifndef DEMO
        strcpy( temp, dma.d_fname );
        if( !wild_env( temp, 1 ) ) goto badrun;
        ptr2 = rindex(temp,'.');
        buf[0] = NEO_ACC_EXC;
        buf[3] = NEO_ACC_MAGIC;
        *(char **)(&buf[4]) = path;
        if( neo_acc->nac_ver >= NAC_VER_300 )
        {
          if( ((i=Kbshift(-1))&0xF)==10 ) Kbshift(i&0xF0);
          pt.old_method=0;
          pt.new_method.set_me=1;
          pt.new_method.return_status=1;
          pt.new_method.clear_screen=1;
          if( ptr2 )
            if( !strcmp(ptr2,".NPG") || !strcmp(ptr2,".NTP") )
            {
              if( _app )
              {
                strcpy( cmdbuf, diskbuf );
                spf( diskbuf, " %D %s", neo_acc, cmdbuf );
              }
              pt.new_method.npg=1;
            }
            else if( !strcmp(ptr2,".TOS") || !strcmp(ptr2,".TTP") )
            {
              if( !neo_acc->mas->most->use_master && tos_wind )
              {
                hide_mouse();
                pt.new_method.clear_screen = 0;
                old_pause = neo_acc->mas->most->tos_pause;
                neo_acc->mas->most->tos_pause = 0;
                prog_text++;
                set_traps(0);
                old_celht = V_CEL_HT;
                V_CEL_HT = char_h;
                old_mx = V_CEL_MX;
                V_CEL_MX = maxcol;
                old_my = V_CEL_MY;
                V_CEL_MY = maxrow;
                b_old_show = old_show;
                b_old_hide = curs_hide;
                vcurs_on(1);
              }
              pt.new_method.tos=1;
            }
          buf[2] = pt.old_method;
        }
        else buf[2] = ptr2 && !strcmp(ptr2,".TTP") ? TOS : 0;
        strcpy( cmdbuf, diskbuf );
        mouse_vex(1);
        env_saved = 0;
        if( _app || multitask )
        {
          if( !prog_text && !multitask )
          {
            close_wind();
            wind_set( 0, WF_NEWDESK, neo_acc->mas->blank, 0, 0 );
            if( pt.new_method.set_me ? pt.new_method.tos : (pt.old_method==TOS) )
            {
              hide_mouse();
              call_w_save( (int (*)())neo_acc->bconws, "\033E\033e\033v\033b\043\033c\040\r" );
            }
            else
            {
              spf( neo_acc->mas->blank[1].ob_spec.tedinfo->te_ptext, "%s",
                  pathend(path) );
              call_w_save( (int (*)())neo_acc->bconws, "\033H\033v" );
              objc_draw( neo_acc->mas->blank, 0, 1, neo_acc->mas->blank[0].ob_x,
                  neo_acc->mas->blank[0].ob_y, neo_acc->mas->blank[0].ob_width,
                  neo_acc->mas->blank[0].ob_height );
            }
            appl_exit();
          }
          cmdbuf[0] = use_argv ? '\x7f' : strlen(cmdbuf+1);
          timer();
          if( multitask && sw_bits && !sw0 )
          {
            shwrcmd.name = path;
            shwrcmd.dflt_dir = dflt_path;
            shwrcmd.environ = env_vars;
            i = !shel_write( 1|(1<<10)|(1<<11), 1, 0, (char *)&shwrcmd, cmdbuf );
            if( !prog_text ) was_closed++;
          }
          else if( has_magx )
          {
            i = !shel_write( 1, 1, 100, path, cmdbuf );
            if( !prog_text ) was_closed++;
          }
          else
          {
            i = Pexec( 0, path, cmdbuf, env_vars );
            was_closed++;
          }
          elapsed();
          pexec++;
          path[0] = i>>8;
          path[1] = i;
          reopen++;
          if( !prog_text && !multitask )
          {
            apid = appl_init();
            call_w_save( (int (*)())neo_acc->bconws, "\033f\033b\043\033c\040" );
            wind_set( 0, WF_NEWDESK, neo_acc->mas->blank, 0, 0 );
            neo_acc->mas->blank[1].ob_spec.tedinfo->te_ptext[0] = '\0';
            objc_draw( neo_acc->mas->blank, 0, 1, neo_acc->mas->blank[0].ob_x,
                neo_acc->mas->blank[0].ob_y, neo_acc->mas->blank[0].ob_width,
                neo_acc->mas->blank[0].ob_height );
            reset_mouse();
          }
        }
        else
        {
          *(char **)(&buf[6]) = cmdbuf;
          memcpy( diskbuf, env_ptr(), sizeof(env_vars) );
          memcpy( env_ptr(), env_vars, sizeof(env_vars) );
          env_saved = 1;
          if( use_argv )
          {
            oneo_acc = neo_acc;
            old_execute = neo_acc->mas->execute;
            neo_acc->mas->execute = new_execute;
          }
          if( neo_write( neo_apid, 16, buf ) ) reopen = w_handle > 0;
          else if( prog_text )
          {
            reset_linea();
            set_traps(1);
            prog_text=0;
            show_mouse();
          }
        }
        mouse_vex(0);
badrun: ;
#else DEMO
        errs( GENERAL, "Programs cannot be run in this demo version." );
#endif DEMO
      }
    }
    else if( i<0 || parms==i+1 || (i&0x40) && parms<=j+1 ||
        is_if&&(command==doelseif||parms==1) || command==doif ||
        command==dowhile )
    {
      vcurs_off();
      timer();
      (*command)();
      elapsed();
      vcurs_on( !in_batch );
    }
    else
    {
      str[0] = j + '0';
      ferrs( SYNTAX, "The \"%s\" command takes %s%s%s.", parmptr[0],
          j ? str : "no", parstr, j==1 ? empty : "s" );
    }
  }
/*  if( !in_batch )
  {
    if( in_vt > 1 ) dovt();
    if( in_vt )
    {
      if( in_vt==1 ) in_vt++;
      parse_lev--;
      goto top;
    }
  }*/
  if( !reopen ) elapsed();
  if( in_batch && w_handle>0 && eol && gt ) newline();
  if( !reopen ) zero_cmd();
  if( !prog_text )
  {
    dec_parselev();
    more=0;
  }
  if( !in_batch ) inverse = discard = 0;
}

void line_long(void)
{
  ferrs( SYNTAX, too_lng, "Expanded string" );
}

int insert_var( char *dollar, char **ptr, char **var_end, int func )
{
  register int l;
  register FUNC_STRUCT *fs;
  char *p, temp[CMDBUFLEN], num_parm;

  strcpy( temp, dollar+1 );
  temp[*ptr-dollar-1] = '\0';
  if( !func )
  {
    if( !strcmp( temp, ">" ) || !strcmp( temp, "<" ) ) return(0);
    if( !find_var( temp, &p ) ) p = empty;
  }
  else
  {
    fs = funcs;
    while( fs->name )
    {
      if( !strxcmp(temp,fs->name,strlen(fs->name)+1) ) break;
      fs++;
    }
    if( !fs->name )
    {
      ferrs( SYNTAX, "Unknown function %s.", temp );
      parms=0;
      return(1);
    }
    if( (func_pnum = func+1) + (num_parm = fs->parms) > parms )
    {
      ferrs( SYNTAX, "Too few%ss for function %%%s.", parstr, temp );
      parms=0;
      return(1);
    }
    func_parm = *ptr;
    *ptr = next_str( !num_parm ? dollar : parmptr[func_pnum+num_parm-1] ) - 1;
    if( !(*fs->func)() )
    {
      parms=0;
      return(1);
    }
    p = path;
  }
  if( (!func ? next_str(*ptr)-1 : next_str(parmptr[parms-1])-1)-
      cmdbuf+(l=strlen(p)) > CMDBUFLEN )
  {
    parms=0;
    line_long();
    return(1);
  }
  else
  {
    memcpy( temp, *ptr, CMDBUFLEN );
    strcpy( dollar, p );
    memcpy( *var_end = dollar+l, temp, CMDBUFLEN-(dollar+l-cmdbuf) );
    if( !func ) *ptr = dollar;
    else
    {
      parms -= num_parm;
      *var_end = (char *)(long)(*ptr - *var_end);
    }
    return(0);
  }
}

int is_digit( int ch, int hex )
{
  static char nums[] = "0123456789ABCDEF";
  register char *ptr;

  if( ch >= 'a' && ch <= 'f' ) ch &= 0x5f;
  if( (ptr = index(nums,ch)) == NULL || ptr-nums > (hex?15:7) ) return(-1);
  else return( ptr-nums );
}

void to_upper( char *ptr )
{
  if( *ptr >= 'a' && *ptr <= 'z' ) *ptr &= 0x5f;
}

void set_path()
{
  int i;

  Dsetdrv( i=dflt_path[0]-'A' );
  if( Mediach(i) ) Fsfirst( "\\*.*", 0x37 );
  Dsetpath( dflt_path+2 );
}

void igtext( char *str )
{
  inverse = 1;
  gtext(str);
  inverse = 0;
}

void dohlp()
{
  register CMD_STRUCT *cs = cmds;

  if( parms==1 )
  {
    igtext( CLI_CMDS );
    help_text( 0, LONGEST );
    igtext( "Redirection:" );
    gtext( "\n ->  <-  <->  ->>  >$  <$\n" );
    igtext( "Functions: precede with %\n" );
    help_text( 1, LONG_FUNC );
    igtext( "Operators:\n" );
    help_text( 2, LONG_OP );
    igtext( "Keys:\n" );
    help_text( 3, MAX_LEN_OTH );
    gtext( "\nEnter \"HELP commandname\" or \"? commandname\" for more help with\
 a command.\n" );
  }
  else
  {
    while( cs->name )
      if( !strxcmp( cs->name, parmptr[1], strlen(cs->name)+1 ) ) break;
      else cs++;
    gtext( cs->name ? cs->help : "No help available for this command." );
  }
}

char *htext( int mode, int i )
{
  static char *others[] = { "^C: Abort operation", "^S: Pause listing",
       "^Q: Resume listing", "^W: Close CLI window", "^X: Clear input line",
       "Ins: Toggle insert", "Help: View history", "Esc: Complete command",
       "Shift-: Previous cmd", "!cmd: Search for cmd", "Backsp  Del     ",
       NULL };

  switch(mode)
  {
    case 0:
      return(cmds[i].name);
    case 1:
      return(funcs[i].name);
    case 2:
      return(ops[i].name);
    case 3:
      return(others[i]);
  }
  return(NULL);
}

void to_col( int len, int c )
{
  int k;

  if( redir_hand[parse_lev] != 2 )
    for( k = c-len; --k>=0; )
      gtext(" ");
  else col = c;
}

void help_text( int mode, int lng )
{
  register int w, i, j, c;
  register char *ptr;

  w = (maxcol+1) / lng;
#ifdef USE_MOST
  i = !mode ? 1 : 0;
#else
  i = 0;
#endif
  vcurs_off();
  do
  {
    gtext(" ");
    c = 1;
    for( j=-1; ++j<w && (ptr=htext(mode,i)) != NULL; i++ )
    {
      gtext(ptr);
      to_col( strlen(ptr)+c, (j+1)*lng + 1 );
      c = (j+1)*lng+1;
    }
    if( j || redir_hand[parse_lev]!=2 ) newline();
    else col=0;
  }
  while( ptr != NULL );
  vcurs_on(0);
}

void doshift()
{
  if( ck_batch() && bparms>1 )
  {
    memcpy( &bparmptr[1], &bparmptr[2], (MAX_PARM-1)*sizeof(bparmptr[0]) );
    bparmptr[--bparms] = empty;
  }
}

void doshow()
{
  static int buf[8] = { NEO_ACC_TXT, 0, 0, NEO_ACC_MAGIC };
  int buf2[8];
  char c;

  if( first( parmptr[1], 0L ) > 0 )
  {
    if( neo_acc->nac_ver >= 0x300 )
    {
      c = neo_acc->mas->most->text_reader[0];
      neo_acc->mas->most->text_reader[0] = '\0';
    }
    mouse_vex(1);
    do
    {
      *(char **)(&buf[4]) = dma.d_fname;
      *(char **)(&buf[6]) = path;
      if( neo_write( neo_apid, 16, buf ) ) evnt_mesag( buf2 );
      else break;
    }
    while( !_abort() && next() );
    mouse_vex(0);
    if( neo_acc->nac_ver >= 0x300 ) neo_acc->mas->most->text_reader[0] = c;
    redraw(0);
  }
}

void dopwd(void);

int ck_batch(void)
{
  if( !in_batch )
  {
    errs( GENERAL, "This command can only be run within a batch file." );
    return(0);
  }
  return(1);
}

void docall()
{
  int l;
  long p;
  char temp[LABEL_LEN+2];

  if( ck_batch() )
    if( call_lev < CALLS )
    {
      l = line_num+1;
      p = fpos;
      strncpy( temp, parmptr[1], LABEL_LEN );
      temp[LABEL_LEN] = '\0';
/*      strcat( temp, ";" );*/
      if( find_next( temp, l ) )
      {
        call_line[++call_lev] = l;
        call_pos[call_lev] = p;
        this_line = line_num;
      }
    }
    else ferrs( GENERAL, nst, "CALL", empty );
}

void docat()
{
  int hand, err=0, len;
  char *max;

  if( first( parmptr[1], 0L ) > 0 )
    do
    {
      strcpy( diskbuf, path );
      strcat( diskbuf, dma.d_fname );
      if( (hand = Fopen(diskbuf,0)) > 0 )
      {
        hide_mouse();
        do
          if( (len=Fread( hand, (long)CMDBUFLEN-1, cmdbuf )) > 0 )
          {
            bcon_nul = len;
            echo( cmdbuf );
            err = _abort();
          }
        while( !err && len>0 );
        show_mouse();
        Fclose(hand);
      }
      else
      {
        ferrs( hand, "%sopen %s.", cn, diskbuf );
        err++;
      }
    }
    while( !err && !_abort() && next() );
}

void docd()
{
  int i, j;

  if( parms==1 ) dopwd();
  else
  {
    i = strcmp( parmptr[1]+1, ":" );
    append( parmptr[1] );
    get_path(parmptr[1]);
    if( !check_drv(path[0],1) ) return;
    Dsetdrv( path[0] - 'A' );
    if( i && (j=Dsetpath( path+2 )) < 0 ) ferrs( j,
        "%schange path to %s.", cn, parmptr[1] );
    else
    {
      Dgetpath( dflt_path+2, (dflt_path[0] = path[0]) - 'A' + 1 );
      strcat( dflt_path, slash );
    }
  }
}

void append( char *p )
{
  char *ptr;

  if( (ptr=rindex(p,'\\')) == NULL || *(ptr+1) ) strcat( p, slash );
}

void dochmod()
{
  int i, j, and=-1, or=0, err=0, att;
  char *ptr, let[]="WHS\1\1A";

  if( parms <= 2 ) errs( SYNTAX, hlp_att );
  else if( first(parmptr[1],0L) > 0 )
  {
    for( i=2; i<parms; i++ )
      if( (ptr=rindex(let,*parmptr[i]&0x5f)) != NULL )
      {
        j = (1<<((long)ptr - (long)let));
        ptr = next_str(parmptr[i]) - 2;
        if( *ptr=='+' || *ptr=='-' )
          if( *ptr=='+' && j>1 || *ptr=='-' && j==1 ) or |= j;
          else and ^= j;
      }
      else
      {
        ferrs( SYNTAX, "Unknown flag: %s", parmptr[i] );
        err++;
      }
    if( !err )
    {
      ptr = next_str(path)-1;
      do
        if( !(dma.d_attrib&S_IJDIR) )
        {
          strcpy( ptr, dma.d_fname );
          if( (att=Fattrib(path,0,0)) < 0 ) err++;
          else if( (att=Fattrib(path,1,att&and|or)) < 0 ) err++;
        }
      while( !err && !_abort() && next() );
      if( err ) ferrs( att, "%saccess %s.", cn, path );
    }
  }
}

void doexit(void), dogoto(void);

void doclose()
{
  paused = in_batch;
  doexit();
}

void docold()
{
  call_w_save( (int (*)())neo_acc->coldboot );
}

void docp()
{
  if( parms==1 ) errs( SYNTAX, hlp_cop );
  else copy(0);
}

void docursor()
{
  long l;
  int find_opt( char *str );
  char *ptr;
  void shift_parms( int start, int num );

  if( find_opt("b+") )
  {
    Cursconf( 2, -1 );
    blink_on=1;
  }
  else if( find_opt("b-") )
  {
    Cursconf( 3, -1 );
    blink_on=0;
  }
  if( parms>1 )
  {
    to_upper( ptr=parmptr[1] );
    if( *ptr == 'I' )
    {
      insert=1;
      shift_parms( 1, 1 );
    }
    else if( *ptr=='O' )
    {
      insert=0;
      shift_parms( 1, 1 );
    }
    if( parms>1 )
      if( is_num3( parmptr[1], &l ) )
      {
        if( !vdi_hand ) V_CUR_CNT=(int)l;
        Cursconf( 4, blink_rate=(int)l );
      }
  }
}

int ddelete(char *str)
{
  int err;

  if( (err=Ddelete(str)) < 0 ) ferrs( err, "\n%sdelete %s%s", cn, str,
      err==AEACCDN ? ". Is it empty?" : period );
  return(err);
}

void shift_parms( int start, int num )
{
  for( parms-=num; start<parms; start++ )
  {
    parmptr[start] = parmptr[start+num];
    escaped[start] = escaped[start+num];
  }
}

int find_opt( char *str )
{
  int i;

  for( i=1; i<parms; i++ )
    if( !escaped[i] && !strxcmp( parmptr[i], str, strlen(str)+1 ) )
    {
      shift_parms( i, 1 );
      return(i);
    }
  return(0);
}

void update_neo( char *path )
{
  if( updt_neo )
  {
    wind_update( BEG_UPDATE );
    if( neo_acc->nac_ver >= NAC_VER_300 )
        call2_w_save( (int (*)())neo_acc->update_drive, path );
    else call2_w_save( (int (*)())((long)(neo_acc->trash_files)+0x55A), path );
    wind_update( END_UPDATE );
  }
}

void copy( int mode )
{
  int err=0, st, old_over;
  char *ptr, *ptr2, *ptr3, temp[120], over=1, no_dirs=0;

  if( find_opt( "o-" ) ) over=0;
  if( find_opt( "d-" ) ) no_dirs++;
  if( first(parmptr[1],0L) > 0 )
  {
    if( is_dir(parmptr[1]) )
    {
      *(pathend(path)-1) = '\0';
      *(pathend(path)) = '\0';
    }
    st = *(neo_acc->status_on);
    *(neo_acc->status_on) = 0;
    if( neo_acc->nac_ver >= NAC_VER_300 )
    {
      old_over = neo_acc->mas->most->conf_over;
      neo_acc->mas->most->conf_over = over;
    }
    mouse_vex(1);
    call_w_save( (int (*)())neo_acc->copy_init );
    strcpy( temp, path );
    if( parms==2 ) parmptr[2] = dflt_path;
    do
    {
      wind_update( BEG_UPDATE );
      *(neo_acc->moving) = mode;
      get_path(parmptr[2]);
      ptr3 = next_str(path)-1;
      strcpy( diskbuf, temp );
      strcpy( ptr2 = next_str(diskbuf)-1, dma.d_fname );
      if( dma.d_attrib & S_IJDIR )
      {
        if( !no_dirs )
        {
          strcat( diskbuf, slash );
          strcpy( ptr3, dma.d_fname );
          strcat( path, slash );
          err = call_w_save( (int (*)())neo_acc->copy_files, diskbuf, path, &dum, &dum, 0 );
          if( !err && mode )
            if( (err = call_w_save( (int (*)())neo_acc->copy_a_buffer, 0L, &dum, &dum )) == 0 )
                err = ddelete(diskbuf);
        }
      }
      else
      {
        if( (ptr = rindex( parmptr[2], '\\' )) == NULL ) ptr =
            rindex( parmptr[2], ':' );                  /* like "e:name" */
        if( ptr && !*(ptr+1) || !*parmptr[2] || is_dir(parmptr[2]) )
             ptr = dma.d_fname;       /* like "e:\" or "e:" or "\" or "." */
        else if( !ptr ) ptr = parmptr[2];               /* just a name */
        else ptr++;                                     /* go past drive let */
        rmatch( ptr2, ptr, ptr3 );
        err = call_w_save( (int (*)())neo_acc->copy_a_file, diskbuf, path, 0L, 0, 0,
            &dum, &dum );
      }
      wind_update( END_UPDATE );
    }
    while( !err && !_abort() && next() );
    if( !err ) call_w_save( (int (*)())neo_acc->copy_a_buffer, 0L, &dum, &dum );
    call_w_save( (int (*)())neo_acc->copy_free );
    *(neo_acc->status_on) = st;
    if( neo_acc->nac_ver >= NAC_VER_300 ) neo_acc->mas->most->conf_over = old_over;
    mouse_vex(0);
    update_neo(temp);
#ifdef DEMO
    gtext( "Files cannot be copied or moved in the demo version." );
#endif DEMO
  }
}

void dodebug()
{
  register int on;

  if( (on=!strxcmp(parmptr[1],ons,3)) != 0 || !strxcmp(parmptr[1],offs,4) )
  {
    if( (debug=on) != 0 )
      if( parms==3 )
      {
        func_parm = parmptr[2];
        redirect( path, &debug_hand, &Fcreate, "creat", 0 );
      }
      else debug_hand = 2;
    else if( debug_hand > 5 ) Fclose(debug_hand);
  }
  else errs( SYNTAX, hlp_deb );
}

void dodup()
{
  long lines;
  int eof=0;

  if( parms==2 )
  {
    if( !is_num3( parmptr[1], &lines ) ) return;
  }
  else lines=1;
  parmptr[1] = cmdbuf;
  parms=2;
  while( lines && !_abort() && !eof )
    if( (eof = getstr( empty, CMDBUFLEN )) == 0 || cmdbuf[0] )
    {
      doecho();
      newline();
      lines--;
    }
  eol=0;
}

void bcauxstr( char *ptr )
{
  while( *ptr ) Bconout( 1, *ptr++ );
}

void echo_nl(void)
{
  if( gt )
  {
    if( eol && !prog_text ) newline();
    gt=0;
  }
}

void echo( char *ptr )
{
  parms=2;
  parmptr[1] = ptr;
  doecho();
}

void doecho(void)
{
  register int i, j;
  register char c, oldc, nul;
  register char *ptr, *ptr2, *ptr3;
  static char esc_str[] = "\033\b\t\a\f", temp[2]="x";

  if( !prog_text && find_opt("nl-") ) eol = 0;
  for( i=1-1; ++i<parms; )
  {
    if( redir_hand[parse_lev] != 2 )
    {
      gtext(parmptr[i]);   /* raw if redirected */
      gt=1;
    }
    else
    {
      vcurs_off();
      ptr = parmptr[i];
      nul = bcon_nul>0;
      while( !nul && *ptr || nul )
      {
        switch( last_esc )
        {
          case 4:
            inverse = (*ptr&0xf)!=0;
          case 5:
            ptr++;
            last_esc = 0;
            goto nextch;
          case 3:
            col = *ptr - '\040';
            last_esc = 0;
            check_rowcol();
            ptr++;
            goto nextch;
          case 2:
            row = *ptr - '\040';
            last_esc++;
            check_rowcol();
            ptr++;
            goto nextch;
          case 1:
            ptr3 = ptr+1;
            ptr2 = ptr-1;
            last_esc = 0;
            goto sw;
        }
        if( !nul )
        {
          ptr2 = ptr;
          while( (c=*ptr2) != 0 && !index(esc_str,c) ) ptr2++;
          ptr3 = ptr2 + (c?1:0) + (c=='\033' && *(ptr2+1));
          if( ptr != ptr2 )
          {
            oldc = *ptr2;
            *ptr2 = '\0';
            gt=1;
            gtext( ptr );
            *ptr2 = oldc;
          }
        }
        else
        {
          ptr2 = ptr-1;
          ptr3 = ptr+1;
          if( (c = index(esc_str,*ptr) ? *ptr : 0) == 0 )
          {
            temp[0] = *ptr;
            gt=1;
            gtext(temp);
          }
          else if( c=='\033' )
            if( bcon_nul==1 )
            {
              last_esc=1;
              break;
            }
            else
            {
              ptr2++;
              ptr3++;
              bcon_nul--;
            }
        }
        if( c=='\b' ) goto escD;
        if( c=='\t' )
        {
          j = (col&0xFFF8) + 8 - col;
/*          if( *(ptr2+1) )
          {
            for( ; j>0; j-- )
              if( !discard || col<maxcol ) gtext( " " );
          }
          else */ if( (col += j) > maxcol )
            if( discard ) col=maxcol;
            else col -= maxcol;
        }
        else if( c=='\a' ) ring_bell();
        else if( c=='\f' ) goto escE;
        else if( c )
sw:         switch( *(ptr2+1) )
        {
          case '\0':
            last_esc=1;
            break;
          case 'A':
            if( row ) row--;
            break;
          case 'B':
            if( row < maxrow ) row++;
            break;
          case 'C':
            if( col < maxcol ) col++;
            break;
          case 'D':
escD:       if( col ) col--;
            break;
          case 'E':
escE:       erase_lines( 0, maxrow );
          case 'H':
            row = col = 0;
            break;
          case 'I':       /* up w/scroll */
            if( row ) row--;
            else blit_lines( 0, maxrow-1, 1, 1 );
            break;
          case 'J':       /* erase to eop */
            erase_ln( col, maxcol );
            erase_lines( row+1, maxrow );
            break;
          case 'K':       /* erase to eol */
escK:       erase_ln( col, maxcol );
            break;
          case 'L':       /* ins line */
            blit_lines( row, maxrow, row+1, 1 );
            col = 0;
            break;
          case 'M':       /* del line */
            blit_lines( row+1, maxrow, row, 1 );
            col = 0;
            break;
          case 'Y':
            last_esc = 2;
            break;
          case 'Z':
            if( in_vt && !in_batch ) bcauxstr( "\033/Z" );
            else eol=0;
            break;
          case 'b':
            last_esc = 5; /* ignore foreground color */
            break;
          case 'c':
            last_esc = 4; /* set background color */
            break;
          case 'd':       /* erase to (incl) cursor */
            erase_lines( 0, row );
            erase_ln( 0, col );
            break;
          case 'e':
            old_show = 1;
            break;
          case 'f':
            old_show = 0;
            break;
          case 'j':
            saver = row;
            savec = col;
            break;
          case 'k':
            row = saver;
            col = savec;
            break;
          case 'l':
            col = 0;
            goto escK;
          case 'o':       /* erase beg line to (incl) curs */
            erase_ln( 0, col );
            break;
          case 'p':
            inverse = 1;
            break;
          case 'q':
            inverse = 0;
            break;
          case 'v':
            discard = 0;
            break;
          case 'w':
            discard = 1;
            break;
          case '=':
            if( in_vt && !in_batch ) vt_appl = 1;
            break;
          case '>':
            if( in_vt && !in_batch ) vt_appl = 0;
            break;
          default:
            ptr3--;
        }
        ptr = ptr3;
nextch: if( bcon_nul )
          if( --bcon_nul<=0 ) break;
      }
      vcurs_on(0);
    }
    if( i<parms-1 ) gtext( sp );
  }
  set_lacurs();
  bcon_nul = 0;
}

void doelse()
{
  if( countif() )
    if( !bad_if[in_if] ) bad_if[in_if] = -1;
    else if( bad_if[in_if] > 0 ) bad_if[in_if] = 0;
}

void doelseif()
{
  if( countif() )
    if( !bad_if[in_if] ) bad_if[in_if] = -1;
    else if( bad_if[in_if] > 0 ) iftest();
}

void doendif()
{
  if( countif() ) in_if--;
}

void close_wind(void)
{
  vbl(0L);
  emulti.type &= ~MU_M1;
  is_top = 0;
  wind_close( w_handle );
  wind_delete( w_handle );
  w_handle = -1;
}

void final_exit(void)
{
  mouse_vex(1);
  if( config.rs232_buf )
  {
    rsio->ibuf = oldrsbuf;
    rsio->ibufsiz = oldrssize;
    rsio->ibufhd = rsio->ibuftl = 0;
  }
  exit(0);
}

void doext(void)
{
  long l;

  if( w_handle > 0 )
  {
    if( !paused ) in_if = -1;
    if( !in_batch || !open_before || paused )
    {
      if( in_batch && !paused && scrn_ptr ) prompt();
      wind_update( BEG_UPDATE );
      close_wind();
      if( !paused ) close_batch();
      if( neo_acc )
      {
        call_w_save( (int (*)())neo_acc->close_form, wind_out.x, wind_out.y, wind_out.w+2,
            wind_out.h+2, 1 );
      }
      wind_update( END_UPDATE );
/*    reset_mouse();*/
      if( _app ) final_exit();
    }
    else
    {
      errnum = 0;
      errmsg[0] = '\0';
      if( parms>1 )
      {
        if( !is_num3( parmptr[1], &l ) ) return;
        errnum = l;
      }
      if( parms>2 )
      {
        strncpy( errmsg, parmptr[2], VAR_LEN );
        gtext( parmptr[2] );
      }
      else if( errnum ) exit_stat();
      close_batch();
    }
  }
}

void doexit()
{
  if( !in_batch ) prompt();
  doext();
}

void dofont()
{
  int f;

  if( *(parmptr[1]) < '0' || *(parmptr[1]) > '2' || *(parmptr[1]+1) )
      errs( SYNTAX, "Fonts range from 0 to 2." );
  else if( (f = *(parmptr[1]) - '0') != fontno )
  {
    fontno = f;
    char_w = fonttab[fontno][0];
    char_h = fonttab[fontno][1];
    resize( &wind_out, 0 );
  }
}

void dofor()
{
  long a, b;
  register char first=0, *ptr, is_ch=0, *ptr2;

  if( !ck_batch() ) return;
  if( parms==2 || parms==3 )
  {
    ferrs( SYNTAX, "Bad number of FOR%ss.", parstr );
    return;
  }
  if( parms==4 && strxcmp( parmptr[2], "in", 3 ) )
  {
    errs( SYNTAX, "Incorrect FOR syntax." );
    return;
  }
  if( for_lev < 0 || for_line[for_lev] != line_num )
    if( for_lev < MAX_FOR-1 )
    {
      for_lev++;
      first++;
    }
    else
    {
      ferrs( SYNTAX, nst, "FOR", "loop" );
      return;
    }
  for_line[for_lev] = line_num;
  for_pos[for_lev] = floc;
  if( parms>1 )
  {
    strcpy( diskbuf, parmptr[3] );
    if( (ptr=index(diskbuf,'.')) != NULL && *(ptr+1)=='.' )
    {
      *ptr = '\0';
      if( !is_num2( diskbuf, &a ) || !is_num2( ptr+2, &b ) )
      {
        if( !c_to_l( diskbuf, &a ) || !c_to_l( ptr+2, &b ) ) return;
        is_ch++;
      }
      for_val[for_lev] = first ? a : (for_val[for_lev] + (a<=b ? 1 : -1));
      if( is_ch ) spf( diskbuf, "%c", (char)for_val[for_lev] );
      else spf( diskbuf, lfmt, for_val[for_lev] );
      make_var( parmptr[1], diskbuf );
      if( a<=b && for_val[for_lev]>b || a>b && for_val[for_lev]<b ) for_jump();
    }
    else
    {
      if( first ) for_val[for_lev] = 0;
      if( for_val[for_lev] > strlen(parmptr[3]) ) for_jump();
      else
      {
        ptr = for_val[for_lev] + parmptr[3];
        while( is_sep(*ptr) ) ptr++;
        ptr2 = ptr;
        while( *ptr2 && !is_sep(*ptr2) ) ptr2++;
        if( *ptr2 )
        {
          *ptr2++ = '\0';
          while( is_sep(*ptr2) ) ptr2++;
        }
        if( *ptr )
        {
          make_var( parmptr[1], ptr );
          for_val[for_lev] = (long)ptr2 - (long)parmptr[3];
        }
        else for_jump();
      }
    }
  }
}

#define MAGIC 0x87654321

int format_sec( char *ptr, int drive, int spt, int trak, int s, int twst, char fast )
{
  register int i, j, k=0;
  int skew[10];
  register long ll;
  char err=0;
  static char start[5] = { 1, 9, 7, 5, 3 };
  static int scnum;

  if( (err = _abort()) != 0 ) return(2);
  if( twst )
  {
    if( !trak && !s || scnum > 4 ) scnum = 0;
    j = start[scnum++];
    for( i=0; i<10; i++ )
    {
      skew[i] = j;
      if( ++j > spt ) j = 1;
    }
  }
  do
    if( (ll = Flopfmt(ptr, skew, drive, spt, trak, s, twst?-1:(fast?11:1),
        MAGIC, 0xE5E5 )) != 0 )
      if( ll == AEWRPRO )
      {
        if(col) newline();
        errs( AEWRPRO, "The disk is write-protected." );
        err++;
      }
  while( ll && ++k<2 && !err );
  if( !err && ll ) err = !tos_error( ll );
  return(err);
}

int f_alert1( char *s )
{
  return form_alert( 1, s );
}

int f_dif( int *val, int new )
{
  if( *val<=0 )
  {
    *val = new;
    return 0;
  }
  return *val>new;
}

char *alloc_fmt( int spt )
{
  char *ptr;

  if( (ptr = (char *)Malloc( spt*1024 )) == 0 ) errs( GENERAL,
      "Not enough memory for format." );
  return ptr;
}

void format( char reinit, char quiet, char fast, int drv, int sides, int spt, int tracks )
{
  char *ptr;
  int i, j, twst, err=0, r, c;
  static char boot_inf[24] = { '\xE9', '\0', 'N', 'e', 'o', 'C', 'L', 'I',
         '\0', '\0', '\0', '\0', '\2', '\2', '\1', '\0', '\2', '\160',
         '\0', '\177', '\177', '\371', '\5', '\0' };
  union
  {
    char c[2];
    unsigned int i;
  } u;

    if( !reinit )
    {
      if( (ptr=alloc_fmt(spt)) == 0 ) return;
      twst = tos_ver>=0x0102 && !fast;
      r = row;
      c = col;
      for( j=0; j<tracks && !err; j++ )
        for( i=0; i<sides && !err; i++ )
        {
          if( !quiet )
          {
            row = r;
            col = c;
            spf( diskbuf, "%sting track %d, side %d of disk %c",
                twst?"Twis":"Format", j, i+1, drv+'A' );
            gtext( diskbuf );
          }
          err = format_sec( ptr, drv, spt, j, i, twst, fast );
        }
      if( !quiet ) newline();
    }
    if( !err )
    {
      if( reinit )
      {
        if( tos_error( (long)Floprd( diskbuf, 0L, drv, 1, 0, 0, 1 ) ) )
        {
          u.c[0] = diskbuf[20];
          u.c[1] = diskbuf[19];
          if( diskbuf[26]<=0 || diskbuf[24]<=0 )
            if( tracks<=0 || sides<=0 || spt<=0 )
            {
              errs( GENERAL, "Error in old disk bootsector. Specify all values \
in REFORMAT command." );
              Mfree(ptr);
              return;
            }
            else i = 0;
          else i = u.i / diskbuf[26] / diskbuf[24];
          if( f_dif( &sides, diskbuf[26] ) | f_dif( &spt, diskbuf[24] ) |
              f_dif( &tracks, i ) )
          {
            mouse_vex(1);
            i = f_alert1( "[2][The new disk format is|larger than the old one.|\
The disk may have to be|completely formatted|instead. Continue anyway?]\
[Continue|Cancel]" );
            mouse_vex(0);
            if( i == 2 ) return;
          }
        }
        if( (ptr=alloc_fmt(spt)) == 0 ) return;
      }
      memclr( ptr, spt<<9 );
      if( tos_error( (long)Flopwr( ptr, 0L, drv, 1, 2-sides, sides-1, spt ) ) )
      {
         memcpy( ptr, boot_inf, 24 );
         *(ptr+26) = sides;
         *(ptr+24) = spt;
         u.i = spt*tracks*sides;
         if( spt<=11 )
         {
           /* (total-1-7-2x)/2-2 = 512x*8/12 */
           *(ptr+13) = 2;
           i = *(ptr+22) = (3 * u.i - 36 + 2053) / 2054;
         }
         else
         {
           /* (total-1-7-2x)/2-2 = 512x*8/12 */
           *(ptr+13) = 1;
           i = *(ptr+22) = (3 * u.i - 30 + 1029) / 1030;
	   *(ptr+21) = 0xF0;	/* HD media desc */
         }
         *(ptr+20) = u.c[0];
         *(ptr+19) = u.c[1];
	 *(long *)(ptr+512) = *(long *)(ptr+((i+1L)<<9)) =	/* media desc in FAT */
	    ((long)*(ptr+21)<<24L) | 0xFFFF00L;
         for( i=8; i<11; i++ )
           *(ptr+i) = Random();
         for( i=j=0; i<14; i++ )
           j += *((int *)ptr+i);
         j = 0x1235-j;
         *(ptr+510) = j>>8;
         *(ptr+511) = j;
         err = !tos_error( (long)Flopwr( ptr, 0L, drv, 1, 0, 0, spt ) );
      }
    }
    Mfree(ptr);
}

void doformat()
{
  char quiet, fast;
  int spt, drv, sides, tracks;
  long l;

  quiet = find_opt("v-");
  drv = find_opt("b:")!=0;
  fast = find_opt("f+");
  if( parms<4 )
  {
    errs( SYNTAX, hlp_fmt );
    return;
  }
  if( !is_num3( parmptr[1], &l ) ) return;
  sides = l;
  if( !is_num3( parmptr[2], &l ) ) return;
  spt = l;
  if( !is_num3( parmptr[3], &l ) ) return;
  tracks = l;
  format( 0, quiet, fast, drv, sides, spt, tracks );
}

void doreformat()	/* 2.3 */
{
  int spt=0, drv, sides=0, tracks=0, i, *tbl[3];
  long l;

  tbl[0] = &sides;
  tbl[1] = &spt;
  tbl[2] = &tracks;
  drv = find_opt("b:")!=0;
  for( i=1; i<=3; i++ )
    if( parms>i )
    {
      if( !is_num3( parmptr[i], &l ) ) return;
      *tbl[i-1] = l;
    }
  format( 1, 0, 0, drv, sides, spt, tracks );
}

int c_or_l( char *ptr, long *l )
{
  if( !is_num2( ptr, l ) ) return( c_to_l( ptr, l ) );
  return(1);
}

int c_to_l( char *ptr, long *l )
{
  if( *(ptr+1) )
  {
    errs( SYNTAX, "Range in FOR must be char..char or num..num." );
    return(0);
  }
  *l = (unsigned)*ptr;
  return(1);
}

void for_jump()
{
  for_lev--;
  find_next( "ENDFOR", 0 );
}

int fn_found( char *ptr, int l )
{
  char *ptr2;

  ptr2 = diskbuf;
  while( is_sep( *ptr2 ) ) ptr2++;
  if( !strxcmp( ptr, ptr2, l ) )
  {
    ptr2 += l;
    if( !*ptr2 || is_sep(*ptr2) || *ptr2==';' ) return(1);
    else if( l==LABEL_LEN )
      while( *ptr2 )
        if( *ptr2++ == ';' ) return(1);
  }
  return(0);
}

int find_next( char *ptr, int loop )
{
  register int l;
  register char found=0;

  l = strlen(ptr);
  while( !found && !get_line( diskbuf ) )
  {
    line_num++;
    found = fn_found( ptr, l );
  }
  if( !found )
    if( !loop ) ferrs( SYNTAX, "%sfind closing %s.", cn, ptr );
    else
    {
      line_num=0;
      bFseek( 0L );
      while( !found && line_num!=loop && !get_line( diskbuf ) )
      {
        line_num++;
        found = fn_found( ptr, l );
      }
      if( !found )
      {
        ferrs( SYNTAX, nf, "Subroutine label ", ptr );
        return(0);
      }
    }
  return(1);
}

void doforend()
{
  if( for_lev < 0 ) ferrs( SYNTAX, wo, "ENDFOR", "FOR" );
  else jump_to_line( for_line[for_lev], for_pos[for_lev] );
}

void dogemmsg()
{
  int id;
  char buf[100];
  int msg[8], i, j, k;
  long l;
  
  if( parms<2 ) errs( SYNTAX, hlp_gem );
  else
  {
    spf( buf, "%-8s", parmptr[2] );
    if( (id = appl_find(buf)) < 0 ) ferrs( GENERAL, nf,
        "Application ", parmptr[2] );
    else if( is_num3( parmptr[1], &l ) )
    {
      msg[0] = l;
      msg[2] = 0;
      for( i=j=3; i<parms && j<8; i++ )
        if( !is_num3( parmptr[i], &l ) ) return;
        else
        {
          if( j<7 && (k=l>>16L)!=0 && k!=-1 ) msg[j++] = k;
          msg[j++] = l;
        }
      appl_write( msg[1]=id, 16, msg );
    }
  }
}

void dogetch()
{
  long l;
  char buf[2];

  if( parms==1 ) errs( SYNTAX, hlp_getc );
  else if( (l=bconinr()) == -1L )
  {
    if( !ign_ctrlc ) ctrlc++;
  }
  else if( l==-2L ) end_in();
  else
  {
    buf[0] = l;
    buf[1] = 0;
    make_var( parmptr[1], buf );
    if( parms==3 )
    {
      buf[0] = l>>16;
      make_var( parmptr[2], buf );
    }
  }
}

void force_cur( int flag )
{
  static int oc, oh, oos;

  if( !flag )
  {
    oc = show_curs;
    oh = curs_hide;
    oos = old_show;
    vcurs_on(1);
  }
  else
  {
    if( oh ) vcurs_off();
    show_curs = oc;
    curs_hide = oh;
    old_show = oos;
  }
}

int read_str(int ctrlz)
{
  long l;
  int i;

  gtext(cmdbuf);
  force_cur(0);
  mouse_vex(1);
  for(;;)
  {
    i = read_key( (int)(((l=bconin(2)) >> 8) + (l&0xff)) );
    if( i==1 || ctrlz && i==4 ) break;
  }
  force_cur(1);
  mouse_vex(0);
  return( i==4 );
}


int getstr( char *string, int len )
{
  int c, cnt;
  long l;
  char *ptr;

  if( iredir_hand[parse_lev] != 2 )
  {
    ptr = cmdbuf;
    cnt = 0;
    while( cnt<len && (l=bconinr())>=0 && (c=(char)l)!='\r' && c!='\n' && c )
    {
      *ptr++ = c;
      cnt++;
    }
    if( c == '\r' ) ichar_cr[parse_lev]++;
    if( l==-1L ) ctrlc++;
    *ptr++ = '\0';
    if( l==-2L ) return(1);
    return(0);
  }
  else
  {
    if( len > CMDBUFLEN ) len = CMDBUFLEN;
    strncpy( cmdbuf, string, len );
    *(cmdbuf+len) = '\0';
    cmdptr = next_str(cmdbuf)-1;
    c = cmdbufmax;
    cmdbufmax = len;
    cnt = read_str(0);
    cmdbufmax = c;
    return(cnt);
  }
}

void dogetstr(void)
{
  if( parms==1 ) errs( SYNTAX, hlp_get );
  else
  {
    strcpy( path, parmptr[1] );
    getstr( parms==3 ? parmptr[2] : empty, VAR_LEN );
    if( iredir_hand[parse_lev] == 2 && in_batch &&
        redir_hand[parse_lev] == 2 ) newline();
    make_var( path, cmdbuf );
  }
}

void dogoto()
{
  register int i, j, found=0, l;
  register char *pm;
  int err=0;

  if( ck_batch() )
  {
    for( j=4; --j>=0; )
      gto[j] = *gtos[j];
    *((pm=parmptr[1])+LABEL_LEN) = '\0'; /* no more than 8 chars in label */
    l = strlen(pm);
    j = line_num;
    while( !found && !err && !get_line( diskbuf ) )
    {
      j++;
      found = is_found( pm, l, &err );
    }
    if( !found && !err )
    {
      gto[0] = gto[1] = gto[2] = gto[3] = -1;
      bFseek( 0L );
      i = 0;
      while( i != line_num && !found && !err && !get_line( diskbuf ))
      {
        i++;
        if( !found ) found = is_found( pm, l, &err );
      }
      if( i != line_num && !found && !err )
      {
        gtext( looperr );
        close_batch();
      }
    }
    else i = j;
    if( !err )
      if( !found && i==line_num ) ferrs( SYNTAX, nf, "Label ", parmptr[1] );
      else if( found ) this_line = i;
  }
}

int is_found( char *pm, int l, int *err )
{
  register char *ptr, *ptr2;
  register int i, ll;
  static char *st[] = { "IF", "WHILE", "REPEAT", "FOR" }, *end[] = {
      "ENDIF", "ENDWHILE", "UNTIL", "ENDFOR" }, lst[] = { 2, 5, 6, 3 },
      lend[] = { 5, 8, 5, 6 };

  ptr = diskbuf;
  while( is_sep(*ptr) ) ptr++;
  ptr2 = ptr;
  while( !is_sep(*ptr2) && *ptr2 ) ptr2++;
  ll = ptr2 - ptr;
  for( i=0; i<4; i++ )
    if( lst[i]==ll && !strxcmp( ptr, st[i], lst[i]) ) gto[i]++;
    else if( lend[i]==ll && !strxcmp( ptr, end[i], lend[i]) ) gto[i]--;
  if( *(ptr2-1) == ';' && !strxcmp(ptr,pm,l) )
  {
    for( i=0; i<4; i++ )
      if( gto[i] > *gtos[i] )
      {
        ferrs( SYNTAX, "Cannot branch to within this %s block.", st[i] );
        *err = 1;
        return(0);
      }
    for( i=0; i<4; i++ )
      *gtos[i] = gto[i];
    return(1);
  }
  return(0);
}

int strxcmp( char *a, char *b, int l )
{
  register char c1, c2;

  c1 = *a++;
  c2 = *b++;
  while( l-- && (c1 || c2) )
  {
    if( c1>='a' && c1<='z' ) c1 &= 0x5f;
    if( c2>='a' && c2<='z' ) c2 &= 0x5f;
    if( c1 != c2 ) return(1);
    c1 = *a++;
    c2 = *b++;
  }
  return(0);
}

void doif()
{
  if( ck_batch() )
    if( in_if < MAX_IF-1 )
    {
      if( ++in_if >= 1 && bad_if[in_if-1] ) bad_if[in_if] = -2;
      else if( !iftest() ) in_if--;
    }
    else ferrs( SYNTAX, nst, "IF", "command" );
}

int iftest()
{
  int i;

  if( (i=logical(parmptr[1])) != 0 )
  {
    bad_if[in_if] = i-1;
    return(1);
  }
  return(0);
}

void dokick()
{
  char spec[]="x:\\";
  char drv;

  if( check_drv( drv = parms>1 ? *parmptr[1]&0x5f : dflt_path[0], 1 ) )
  {
    *(neo_acc->mas->bad_media) = (*spec=drv) - 'A';
    Fsfirst( spec, 0x37 );
    update_neo(spec);
  }
}

void errs( int num, char *ptr )
{
  int oo, i, h;
  static char recur=0, gt_recur=0;

  errnum = num;
  strncpy( errmsg, ptr, VAR_LEN );
  ctrlc = 0;
  if( err_label[0] && !recur )
  {
    strcpy( parmptr[1], err_label );
    recur++;
    docall();
    recur=0;
    if( in_batch ) return;
    newline();
  }
  if( debug && num==SYNTAX && !gt_recur )
  {
    force_debug(0);
    gtext( "ð" );
    for( i=-1; ++i<parms; )
    {
      gtext( parmptr[i] );
      gtext( "ø" );
    }
    newline();
    force_debug(1);
  }
  oo = redir_hand[parse_lev];
  redir_hand[parse_lev] = gt_recur++ ? 2 : eredir_hand[parse_lev];
  gtext( ptr );
  if( recur )
  {
    ferrs( SYNTAX, nf, empty, "\nONERR subroutine " );
    close_batch();
  }
  else if( in_batch )
  {
    if( (h = iredir_hand[parse_lev])!=1 && h!=3 ) redir_hand[parse_lev] = 2;
    gtext( "\n\nTerminate batch file? (Y/n)" );
    while( h==1||h==3 ? !hasch() : !Bconstat(2) );
    newline();
    i = (h==1||h==3 ? bconin(2) : Bconin(2))&0x5f;
    redir_hand[parse_lev] = oo;
    if( i != 'N' ) close_batch();
  }
  else redir_hand[parse_lev] = oo;
  gt_recur--;
}

int countif()
{
  if( in_if < 0 )
  {
    ferrs( SYNTAX, wo, parmptr[0], "IF" );
    return(0);
  }
  return(1);
}

void doloadinf()
{
#ifndef DEMO
  static int buf[8] = { NEO_ACC_INF, 0, 0, NEO_ACC_MAGIC };
  int buf2[8];
#endif

  if( first(parmptr[1],0L) > 0 )
  {
#ifndef DEMO
    strcpy( diskbuf, path );
    strcat( diskbuf, dma.d_fname );
    *(char **)(&buf[4]) = diskbuf;
    mouse_vex(1);
    if( neo_write( neo_apid, 16, buf ) )
    {
/*      parms=0;
      doext();*/
      evnt_mesag( buf2 );
/*      open_it();*/
    }
#else DEMO
    errs( GENERAL, not_in_demo );
#endif DEMO
    mouse_vex(0);
  }
}

void dols()
{
  register int hr, mi;
  int mmode, wid;
  char *ptr, size[8], pm, wide=0, no_dirs=0, no_files=0, not_verb=0;

  hide_mouse();
  mmode = !strcmp( parmptr[0], "DIR" );
  if( find_opt( "w+" ) ) wide++;
  if( find_opt( "d-" ) ) no_dirs++;
  if( find_opt( "f-" ) ) no_files++;
  if( find_opt( "v-" ) ) not_verb++;
  if( parms!=1 )
  {
    if( (ptr=rindex(parmptr[1],'\\')) == NULL ) ptr = rindex(parmptr[1],':');
    if( ptr && !*(ptr+1) ) strcat( parmptr[1], glob );
    else if( !strcmp(parmptr[1],pperiod) || !strcmp(parmptr[1],period) )
        strcat( parmptr[1], globs );
  }
  else parmptr[1] = glob;
  if( wide ) wid = not_verb ? 13 : (mmode ? 38 : 48);
  if( mmode && !not_verb )
  {
    get_path( parmptr[1] );
    gtext( "    Directory of " );
    gtext( path );
    newline();
  }
  if( first( parmptr[1], "Empty directory." ) > 0 )
    do
      if( (hr=dma.d_attrib&0x10)!=0 && !no_dirs || !no_files && hr==0 )
      {
        if( !not_verb )
        {
          hr = dma.d_time>>11 & 0x1F;
          mi = dma.d_time>>5 & 0x3F;
          spf( path, date_fmt, dma.d_date>>5 & 0xf, dma.d_date&0x1f,
              ((dma.d_date>>9 & 0x7F) + 80) % 100 );
          if( mmode )
          {
            if( (ptr=rindex(dma.d_fname,'.')) != NULL ) *ptr++ = '\0';
            else ptr = empty;
            if( !(dma.d_attrib&0x10) ) spf( size, "%7D", dma.d_length );
            else strcpy( size, "<DIR>  " );
            pm = hr>11 ? 'p' : 'a';
            if( hr>12 ) hr -= 12;
            if( !hr ) hr = 12;
            spf( diskbuf, " %-8s %-3s %s %s %2d:%02d%c",
              dma.d_fname, ptr, size, path, hr, mi, pm );
          }
          else spf( diskbuf,
              " %cr%c%c%c%c %-13s %02d:%02d:%02d %s %D",
              dma.d_attrib&0x10 ? 'd' : '-', dma.d_attrib&0x1 ? '-' : 'w',
              dma.d_attrib&0x2 ? 'h' : '-', dma.d_attrib&0x4 ? 's' : '-',
              dma.d_attrib&0x20 ? 'a' : '-', dma.d_fname, hr, mi,
              (dma.d_time & 0x1F)<<1, path, dma.d_length );
          ptr = diskbuf;
        }
        else ptr = dma.d_fname;
        if( !gtext(ptr) )
        {
          ctrlc = 0;
          break;
        }
        if( wide ) to_col( strlen(ptr), (col+wid)/wid*wid );
        if( wide && col > maxcol-wid+2 || !wide ) newline();
      }
      while( next() );
  show_mouse();
}

void to_date( unsigned int date )
{
  spf( path, date_fmt, ((date>>9 & 0x7F) + 80)%100, date>>5 & 0xf, date&0x1f );
}

void to_time( unsigned int time )
{
  spf( path, "%02d:%02d:%02d", time>>11 & 0x1f, time>>5 & 0x3f,
      (time&0x1f)<<1 );
}

void domenu()
{
  register int i;
  register char c;
  long l;

  if( ck_batch() )
    if( menu )
    {
      menu--;
      do
      {
        c = (char)(l = bconinr());
        if( l==-2L ) end_in();
        else if( c == '\03' || l==-1L ) errs( AEINVFN, empty );
        if( in_batch )
        {
          for( i=0; i<menu; i++ )
            if( menu_keys[i] == c ) break;
          if( i<menu )
          {
            parmptr[1] = menu_labls[i];
            dogoto();
          }
        }
      }
      while( i>=menu && in_batch );
      menu=0;
    }
    else menu=1;
}

void add_menu()
{
  if( *(parmptr[0]+1) ) errs( SYNTAX,
      "MENU key definitions may only be one character." );
  else if( menu <= MENU_LINES )
  {
    menu_keys[menu-1] = *parmptr[0];
    strncpy( menu_labls[menu-1], parmptr[1], LABEL_LEN );
    menu++;
  }
  else ferrs( GENERAL, "%s MENU key assignments.", tm );
}

void domkdir()
{
  int i;

  append(parmptr[1]);
  get_path( parmptr[1] );
  *(path+strlen(path)-1) = '\0';                /* remove trailing slash */
  if( (i=Dcreate(path)) < 0 ) ferrs( i, "%screate %s.", cn, path );
  else update_neo(path);
}

#ifdef USE_MOST
void domost()
{
  spf( diskbuf, "neo_acc = %D  neo_acc->mas = %D\n", neo_acc, neo_acc->mas );
  gtext( diskbuf );
  gtext( hlp_most );
}
#endif

int list_scan( char *ptr, char *list[], int size )
{
  int i;

  for( i=0; i<size; i++ )
    if( !strxcmp( ptr, list[i], strlen(list[i])+1 ) ) return(i);
  return(-1);
}

void domouse()
{
  int i;
  static char *list1[] = { &ons[0], &offs[0], "RESET", "SHAPE" },
      *list2[] = { "ARROW", "LINE", "BEE", "POINT", "GRAB", "THINCROSS",
      "THICKCROSS", "OUTLINECROSS" };

  if( parms>1 ) switch( list_scan( parmptr[1], list1, sizeof(list1)/4 ) )
  {
    case 0:
      show_mouse();
      break;
    case 1:
      hide_mouse();
      break;
    case 2:
      reset_mouse();
      break;
    case 3:
      if( parms==3 && (i=list_scan( parmptr[2], list2,
          sizeof(list2)/4 )) >= 0 )
      {
        graf_mouse( i, 0L );
        break;
      }
    default:
      errs( SYNTAX, hlp_mou );
  }
  else errs( SYNTAX, hlp_mou );
}

void domv()
{
  if( parms==1 ) errs( SYNTAX, hlp_mov );
  else copy(1);
}

#ifdef NEW
void donew()
{
  gtext( new_str );
}
#endif NEW

void donull()
{
}

void dopause(void)
{
  gtext( "Press any key to continue" );
  bconin(2);
}

void doonerr()
{
  if( ck_batch() ) strncpy( err_label, parmptr[1], LABEL_LEN );
}

void doonquit()
{
  if( ck_batch() ) strncpy( quit_label, parmptr[1], LABEL_LEN );
}

int ck_hlp( char *ptr )
{
  return( *(ptr+1) == '\x62' && (*kbshift&3)==3 );
}

int is_ctrlc_( char *ptr )
{
  if( !in_vt && !ign_ctrlc && *(ptr+3) == '\03' ) return(1);
  else if( ck_hlp(ptr) ) return(2);
  return(0);
}

int is_ctrlc(void)
{
  unsigned int l;
  void view_hist(void);

  switch( check_io( is_ctrlc_ ) )
  {
    case 1:
      return(!in_ctrlc);
    case 2:
      view_hist();
    default:
      return(0);
  }
}

long bconinr( void )
{
  unsigned char ch;
  int h;

  if( !ign_ctrlc && is_ctrlc() ) return(-1L);
  h = iredir_hand[parse_lev];
  if( h < 0 )			/* from var */
  {
    h = -h-1;
    if( !vvals[h].c[0] ) return(-2L);
    ch = vvals[h].c[0];
    strcpy( vvals[h].c, vvals[h].c+1 );
    return((long)ch);
  }
  else if( h < 5 ) return( bconin(h) );
  else if( h==5 ) return(-2L);
  else
  {
getch:
    if( Fread( h, 1L, &ch ) != 1L ) return(-2L);
    else if( ichar_cr[parse_lev] && ch == '\n' )
    {
      ichar_cr[parse_lev] = 0;
      goto getch;
    }
    else return( (long)ch );
  }
}

void end_in(void)
{
  errs( GENERAL, "End of input file." );
}

void check_help(void)
{
  unsigned int l;
  void view_hist(void);

  if( in_hist ) return;
  if( check_io( ck_hlp ) ) view_hist();
}

long bconin( int hand )
{
  while( !Bconstat(hand) );
  check_help();
  return( Bconin(hand) );
}

void dopop()
{
  if( ck_batch() )
    if( call_lev>=0 )
    {
      call_lev--;
      in_ctrlc=0;
    }
    else ferrs( SYNTAX, wo, "POP", "CALL" );
}

void doprint()
{
  mouse_vex(1);
  if( first(parmptr[1],0L) > 0 )
    do
    {
      strcpy( diskbuf, path );
      strcat( diskbuf, dma.d_fname );
      call_w_save( (int (*)())neo_acc->print_file, diskbuf );
    }
    while( !_abort() && next() );
  mouse_vex(0);
}

void doprompt()
{
  register char *ptr;

  to_upper( ptr=parmptr[1] );
  if( *ptr == 'F' ) strcpy( vprompt, "\"$cwd \"" );
  else if( *ptr == 'B' ) strcpy( vprompt, "\"> \"" );
  else errs( SYNTAX, hlp_pro );
}

void dopwd()
{
  gtext(dflt_path);
}

void doquitneo()
{
  static int buf[8] = { NEO_ACC_QUI, 0, 0, NEO_ACC_MAGIC, 0 };

  mouse_vex(1);
  neo_write( neo_apid, 16, buf );
  mouse_vex(0);
}

/*
void doread()
{
  long bytes, c;
  int eof=0;
  char *ptr;

  if( !is_num3( parmptr[2], &bytes ) ) return;
  if( bytes<0 || bytes>VAR_LEN ) errs( SYNTAX, "Bad number of bytes in READ." );
  else
  {
    *(ptr = diskbuf) = '\0';
    while( bytes-- )
      if( (c=bconinr()) == -1L )
      {
        ctrlc++;
        break;
      }
      else if( c==-2L )
      {
        end_in();
        break;
      }
      else *ptr++ = c;
    make_var( parmptr[1], diskbuf );
  }
}*/

void dorepeat()
{
  if( ck_batch() )
    if( until_lev < MAX_UNTIL-1 )
    {
      until_line[++until_lev] = line_num+1;
      until_pos[until_lev] = fpos;
    }
    else ferrs( SYNTAX, nst, "REPEAT/UNTIL", "loop" );
}

void doreturn()
{
  if( ck_batch() )
    if( call_lev>=0 )
    {
      in_ctrlc = 0;
      jump_to_line( call_line[call_lev--], call_pos[call_lev] );
    }
    else ferrs( SYNTAX, wo, "RETURN", "CALL" );
}

void fold_msg(void)
{
  gtext( dma.d_fname );
  gtext( " is a folder.\n" );  /* don't abort batch files on this one */
}

void dorm()
{
  int oldst, full;
  long err=0L;
  char *ptr;

  if( parms==1 ) errs( SYNTAX, hlp_del );
  else if( first(parmptr[1],0L) > 0 )
  {
    full = parms==3 && !strcmp( parmptr[2], "FULL" );
    oldst = *(neo_acc->status_on);
    *(neo_acc->status_on) = 0;
    if( is_dir(parmptr[1]) )
    {
      *rindex(path,'\\') = '\0';
      if( (ptr=rindex(path,'\\')) != NULL ) *(ptr+1) = '\0';
    }
    mouse_vex(1);
    do
    {
      strcpy( diskbuf, path );
      strcat( diskbuf, dma.d_fname );
      if( dma.d_attrib & S_IJDIR )
        if( full ) call_w_save( (int (*)())neo_acc->trash_files, diskbuf, &dum, &dum );
        else fold_msg();
      else if( (err=Fdelete(diskbuf)) < 0 ) ferrs( err, "\n%sdelete %s%s",
          cn, diskbuf, err==AEACCDN ? ". Is it read-only?" : period );
    }
    while( !err && !_abort() && next() );
    *(neo_acc->status_on) = oldst;
    mouse_vex(0);
    update_neo(path);
  }
}

void dormdir()
{
  long err=0L;
  char *ptr;

  if( first(parmptr[1],0L) > 0 )
  {
    if( is_dir(parmptr[1]) ) *(pathend(path)-1) = '\0';
    ptr = pathend(path);
    do
      if( dma.d_attrib & S_IJDIR )
      {
        strcpy( ptr, dma.d_fname );
        strcat( ptr, slash );
        err = ddelete(path);
      }
      else
      {
        gtext( dma.d_fname );
        gtext( " is not a folder.\n" );
      }
    while( !err && !_abort() && next() );
    *ptr = '\0';
    update_neo(path);
  }
}

void dorn()
{
  char *ptr, *ptr2, *ptr3;
  int err=0;

  if( first(parmptr[1],0L) > 0 )
  {
    if( is_dir(parmptr[1]) ) *(pathend(path)-1) = '\0';
    ptr3 = pathend(path);
    do
    {
      ptr = pathend(parmptr[2]);
      strcpy( diskbuf, parmptr[2] );
      ptr2 = ptr-parmptr[2]+diskbuf;
      rmatch( dma.d_fname, ptr, ptr2 );
      strcpy( ptr3, dma.d_fname );
      if( (err=Frename( 0, path, diskbuf )) != 0 )
          ferrs( err, "%srename %s to %s.", cn, path, diskbuf );
    }
    while( !err && !_abort() && next() );
    update_neo(path);
  }
}

void rmatch( char *str, char *pat, char *out )
{
  char *ast, *per1, *per2, *ptr, *max1, *max2;

  max1 = next_str(str)-1;
  max2 = next_str(pat)-1;
  if( (per1 = index( str, '.' )) == NULL ) per1 = max1;
  if( (per2 = index( pat, '.' )) == NULL ) per2 = max2;
  do
  {
    if( str >= per1 ) per1 = max1;
    if( pat >= per2 ) per2 = max2;
    if( (ast = index(pat,'*')) == NULL ) ast = max2;
    while( pat<ast )
    {
      if( *pat == '?' )
      {
        if( (pat<=per2) == (str<=per1) && str!=per1 ) *out++ = *str;
      }
      else *out++ = *pat;
      if( *str && (pat<=per2) == (str<=per1) ) str++;
      pat++;
    }
    if( str<=per1 && per2<=ast ) str += (per1-str)+(pat-per2);
    if( str>max1 ) str=max1;
    if( *ast )
    {
      while( *ast == '*' || *ast=='?' ) ast++;
      ptr = ast<=per2 && str<=per1 ? per1 - (per2-ast) : max1;
      while( str<ptr ) *out++ = *str++;
    }
    pat = ast;
  }
  while( *pat );
  *out++ = '\0';
}

void doselect()
{
  int but;
  char tempp[120], tempf[13]="";

  if( parms<2 ) errs( SYNTAX, hlp_sel );
  else
  {
    strcpy( tempp, parmptr[1] );
    uppercase(tempp);
    if( parms>=3 ) strcpy( tempf, parmptr[2] );
    uppercase(tempf);
    if( parms==4 ) *(parmptr[3]+30) = '\0';
    mouse_vex(1);
    if( (tos_ver>=0x104 && parms==4 ? fsel_exinput( tempp, tempf, &but, parmptr[3] ) :
        fsel_input( tempp, tempf, &but)) && but==1 )
    {
      strcpy( item_path, tempp );
      strcpy( pathend(item_path), tempf );
    }
    else strcpy( item_path, false );
    mouse_vex(0);
    redraw(0);
    ignore_redraw=1;
  }
}

void uppercase( char *ptr )
{
  while( *ptr ) to_upper( ptr++ );
}

void doset()
{
  register int i, j;
  register VAR_STRUCT *vs;
  char *ptr;

  if( parms==1 )
  {
    igtext( "CLI Variables:" );
    vs = my_vars;
    while( vs->name )
    {
      get_mvar_val( vs, diskbuf );
      dispvar( vs->name, diskbuf, vs->type<0 );
      vs++;
    }
    for( i=j=0; i<config.num_vars; i++ )
      if( vnames[i].c[0] )
      {
        if( !j ) igtext( "\nUser-defined Variables:" );
        dispvar( &vnames[i].c[0], &vvals[i].c[0], -1 );
        j++;
      }
  }
  else if( parms==2 )
    if( find_var( parmptr[1], &ptr ) ) gtext(ptr);
    else var_not_found();
  else
  {
    diskbuf[0] = '\0';
    for( i=2; i<parms; i++ )
    {
      strcat( diskbuf, parmptr[i] );
      if( i!=parms-1 ) strcat( diskbuf, sp );
    }
    make_var( parmptr[1], diskbuf );
  }
}

void alias_not_found(void)
{
  ferrs( GENERAL, nf, "Alias ", parmptr[1] );
}

void remove_alias( char *ptr )
{
  char *ptr1 = next_str(next_str(ptr+2));
  memcpy( ptr, ptr1, alias+config.alias_size-ptr1 );
  alias_ptr = 0L;
}

int find_alias( char *name, char **ptr )
{
  *ptr = alias;
  while( *(*ptr+2) || *(*ptr+3) )
  {
    if( !strxcmp( name, *ptr+2, strlen(name)+1 ) ) return(1);
    *ptr = next_str(next_str(*ptr+2));
  }
  return(0);
}

void disp_alias( int all, char **ptr )
{
  char s[2] = " ", *p;
  static char xstr[] = "\\X";

  if( **ptr != -1 || *(*ptr+1) != -1 )
  {
    spf( diskbuf, "\'\\x%02x\' \'\\x%02x\' ", **ptr, *(*ptr+1) );
    gtext( diskbuf );
  }
  else if( all ) to_col(all,ALIAS_NLEN+1+14);
  gtext( "\'" );
  *ptr = next_str(*ptr+2);
  while( **ptr )
  {
    if( (unsigned char)**ptr >= ' ' )
    {
      s[0] = **ptr;
      gtext(s);
    }
    else if( (p=strchr( slashes2, **ptr )) != 0 )
    {
      xstr[1] = slashes[p-slashes2];
      gtext(xstr);
    }
    else
    {
      spf( diskbuf, "\\x%x", **ptr );
      gtext(diskbuf);
    }
    ++*ptr;
  }
  ++*ptr;
  gtext( "\'" );
}

char *add_alias( char *ptr, char *add, int len )
{
  if( ptr-alias+len+4 >= config.alias_size )
  {
    ferrs( GENERAL, room, "aliase" );
    return(0L);
  }
  memcpy( ptr, add, len );
  return(ptr+len);
}

void doalias()
{
  register int i;
  char *ptr, shift, scan, *ptr2;

  if( parms==1 )
    for( ptr=alias; *(ptr+2)||*(ptr+3); )
    {
      gtext( ptr+2 );
      to_col( i=strlen(ptr+2), ALIAS_NLEN+1 );
      disp_alias( ALIAS_NLEN+1, &ptr );
      newline();
    }
  else if( parms==2 )
    if( find_alias( parmptr[1], &ptr ) ) disp_alias( 0, &ptr );
    else alias_not_found();
  else
  {
    if( (i=find_opt("k+")) != 0 )
      if( parms-i < 1 )
      {
        errs( SYNTAX, hlp_ali );
        return;
      }
      else
      {
        shift = *parmptr[i];
        scan = *parmptr[i+1];
        shift_parms( i, 2 );
      }
    else if( find_opt("i+") )
    {
      gtext( "Press the key to assign to this alias" );
      scan = bconin(2)>>16;
      shift = *kbshift&0xf;
    }
    else shift=scan=-1;
    if( parms < 2 )
    {
      errs( SYNTAX, hlp_ali );
      return;
    }
    uppercase(parmptr[1]);
    if( strlen(parmptr[1]) > ALIAS_NLEN ) *(parmptr[1]+ALIAS_NLEN) = '\0';
    if( find_alias( parmptr[1], &ptr ) ) remove_alias(ptr);
    ptr = alias;
    while( *(ptr+2) || *(ptr+3) )
      ptr = next_str(next_str(ptr+2));
    if( (ptr2 = add_alias( ptr, &shift, 1 )) != 0 &&
        (ptr2=add_alias( ptr2, &scan, 1 )) != 0 )
        ptr2 = add_alias( ptr2, parmptr[1], strlen(parmptr[1])+1 );
    for( i=2; i<parms && ptr2; i++ )
      if( (ptr2 = add_alias( ptr2, parmptr[i], strlen(parmptr[i]) )) != 0 )
          ptr2 = add_alias( ptr2, sp, 1 );
    memclr( ptr2 ? ptr2-1 : ptr, 5 );
  }
}

void dounalias()
{
  char *ptr;

  if( !strcmp( parmptr[1], "*" ) )
  {
    memclr( alias, config.alias_size );
    alias_ptr = 0L;
  }
  else if( find_alias( parmptr[1], &ptr ) ) remove_alias(ptr);
  else alias_not_found();
}

char *add_env( char *ptr, char *add )
{
  int len = strlen(add);

  if( ptr-env_vars+len+2 >= sizeof(env_vars) )
  {
    ferrs( GENERAL, room, "environmental variable" );
    return(0L);
  }
  memcpy( ptr, add, len );
  return(ptr+len);
}

int find_env( char **ptr2 )
{
  char *ptr;
  int i;

  for( *ptr2=0L, ptr=env_vars; *ptr; ptr = next_str(ptr) )
    if( !strncmp( ptr, parmptr[1], i=strlen(parmptr[1]) ) && *(ptr+i)=='=' )
    {
      *ptr2 = ptr;
      return(i);
    }
  return(i);
}

void remove_env( char *ptr2 )
{
  char *ptr = ptr2 + strlen(ptr2) + 1;

  memcpy( ptr2, ptr, env_vars+sizeof(env_vars)-ptr );
}

void env_not_found(void)
{
  ferrs( GENERAL, nf, "Environmental variable ", parmptr[1] );
}

void dosetenv()
{
  char *ptr, *ptr2;
  int i;

  if( parms==1 )
    for( ptr=env_vars; *ptr; ptr=next_str(ptr) )
    {
      gtext(ptr);
      newline();
    }
  else
  {
    i = find_env( &ptr2 );
    if( parms==2 )
      if( ptr2 ) gtext(ptr2+i+1);
      else env_not_found();
    else
    {
      if( ptr2 ) remove_env( ptr2 );
      for( ptr=env_vars; *ptr; ptr=next_str(ptr) );
      ptr2 = ptr;
      if( (ptr = add_env( ptr, parmptr[1] )) != 0 ) ptr = add_env( ptr, "=" );
      for( i=2; i<parms && ptr; i++ )
        if( (ptr = add_env( ptr, parmptr[i] )) != 0 ) ptr = add_env( ptr, " " );
      if( !ptr ) ptr = ptr2;
      if( ptr ) *(ptr-1) = *ptr = '\0';
    }
  }
}

void dounsetenv()
{
  char *ptr;

  if( !strcmp( parmptr[1], "*" ) ) memclr( env_vars, sizeof(env_vars) );
  else
  {
    find_env( &ptr );
    if( ptr ) remove_env( ptr );
    else env_not_found();
  }
}

void get_mvar_val( VAR_STRUCT *vs, char *buf )
{
  char *val = vs->val;

  switch( abs(vs->type) )
  {
    case 12:	/* just for prompt */
    case 99:
    case 0:
      strcpy( buf, val );
      break;
    case 1:
      spf( buf, dfmt, *(int *)val );
      break;
    case 2:
      spf( buf, lfmt, *(long *)val );
      break;
    case 3:
      spf( buf, "%d %d %d %d", *(Rect *)val );
      break;
    case 4:
      spf( buf, sfmt, *(char **)val );
      break;
    case 5:
      strcpy( buf, *val ? true : false );
      break;
    case 6:
      spf( buf, dfmt, *val );
      break;
    case 7:
xx:   spf( buf, "%x.%x", *val, *(val+1) );
      break;
    case 8:
x0x:  spf( buf, "%x.%02x", *val, *(val+1) );
      break;
    case 9:
      strcpy( buf, (unsigned)iredir_hand[parse_lev] < 6 ?
          devs[iredir_hand[parse_lev]] : input[parse_lev-1] );
      break;
    case 10:
      strcpy( buf, (unsigned)redir_hand[parse_lev] < 6 ?
          devs[redir_hand[parse_lev]] : output[parse_lev-1] );
      break;
    case 11:   /* just for TOS_VER */
      if( *(int *)val >= 0x160 ) goto x0x;
      goto xx;
    case 13:
      strcpy( buf, (unsigned)eredir_hand[parse_lev] < 6 ?
          devs[eredir_hand[parse_lev]] : _error[parse_lev-1] );
  }
}

void dispvar( char *name, char *val, int flag )
{
  newline();
  gtext( flag <= 0 ? sp : "ø" );
  gtext( name );
  gtext( " =" );
  to_col( 1+strlen(name)+2, 13 );
  gtext( val );
}

int check_lev(void)
{
  if( parse_lev > 1 ) return(1);
  errs( GENERAL, ".INPUT and .OUTPUT cannot be changed at the command prompt." );
  return(0);
}

int new_uvar( char *name, char *val )
{
  int i;

  for( i=0; i<config.num_vars; i++ )
    if( !vnames[i].c[0] ) break;
  if( i<config.num_vars )
  {
    strncpy( &vnames[i].c[0], name, 8 );
    strncpy( &vvals[i].c[0], val, VAR_LEN );
    return(i+1);
  }
  errs( GENERAL, "You already have the maximum number of variables assigned." );
  return(0);
}

void make_var( char *name, char *val )
{
  char *ptr, temp[sizeof(vprompt)];
  int i, j;
  long l;

  uppercase(name);
  if( (i=find_var(name,&ptr)) > 0 ) strncpy( ptr, val, VAR_LEN );
  else if( i<0 ) switch( my_vars[i=-(i+1)].type )
  {
    case -1:
      if( is_num3( val, &l ) ) *(int *)my_vars[i].val = l;
      check_rowcol();
      break;
    case -5:
      if( (j=logical(val)) != 0 ) *my_vars[i].val = 2-j;
      break;
    case -9:
      if( check_lev() )
      {
        iredir_hand[parse_lev--] = 2;   /* ignore present handle */
        close_iredir();
        func_parm = val;
        f_iredir();
        parse_lev++;
      }
      break;
    case -10:
      if( check_lev() )
      {
        redir_hand[parse_lev--] = 2;
        close_redir();
        func_parm = val;
        f_redir();
        parse_lev++;
      }
      break;
    case -13:
      eredir_hand[parse_lev--] = 2;
      close_eredir();
      func_parm = val;
      f_eredir();
      parse_lev++;
      break;
    case -99:
      strncpy( my_vars[i].val, val, VAR_LEN );
      break;
    default:
      bad_var( name );
  }
  else new_uvar( name, val );
}

void bad_var( char *name )
{
  ferrs( GENERAL, "%s is a CLI variable which cannot be changed.", name );
}

void var_not_found()
{
  ferrs( GENERAL, nf, "Variable ", parmptr[1] );
}

int find_var( char *name, char **val )       /* USES path AS A BUFFER!! */
{
  register VAR_STRUCT *vs;
  register int i;

  vs = my_vars;
  i=-1;
  while( vs->name )
    if( !strxcmp( vs->name, name, 8 ) )
    {
      get_mvar_val( vs, path );
      *val = path;
      return(i);
    }
    else
    {
      vs++;
      i--;
    }
  for( i=0; i<config.num_vars; i++ )
    if( !strxcmp( &vnames[i].c[0], name, 8 ) )
    {
      *val = &vvals[i].c[0];
      return(i+1);
    }
  return(0);
}

int scan( char *str, char *cmp[], int num, int len )
{
  int i;

  for( i=0; num--; i++ )
    if( !strxcmp( str, cmp[i], len ) ) return(i);
  return(-1);
}

void dosetrs()
{
  int i, spd, flow, par, stop;
  static char *spds[]={ "19200", "9600", "4800", "3600", "2400",
      "2000", "1800", "1200", "600", "300", "200", "150", "134",
      "110", "75", "30" }, *pars[]={ "NONE", "ODD", "EVEN" },
      *flows[]={ "NONE", "XON", "RTS", "BOTH" },
      *stops[]={ "1.5", "1", "2" }, *bits[]={ "8", "6", "7", "5" };

  if( (i=scan( parmptr[1], spds, 16, 6 )) >= 0 )
  {
    spd = i;
    if( (i=scan( parmptr[2], flows, 4, 1 )) >= 0 )
    {
      flow = i;
      if( (i=scan( parmptr[3], pars, 3, 1 )) >= 0 )
      {
        par = i;
        if( (i=scan( parmptr[4], stops, 3, 4 )) >= 0 ) stop = i+1;
      }
    }
  }
  if( i<0 ) errs( SYNTAX, hlp_rs );
  else if( (i=scan( parmptr[5], bits, 4, 2 )) >= 0 ) Rsconf( spd, flow,
      (1<<7)|(i<<5)|(stop<<3)|(par<<1), -1, -1, -1 );
}

void dotext()
{
  int text_hand, err=0;
  long len;

  if( is_dir(parmptr[1]) ) ferrs( GENERAL, "%s is a folder.", parmptr[1] );
  else
  {
    get_path( parmptr[1] );
    strcat( path, pathend(parmptr[1]) );
    if( (text_hand = Fcreate( path, 0 )) < 0 )
        ferrs( text_hand, "%screate text file %s.", cn, path );
    else
    {
      if( iredir_hand[parse_lev] == 2 ) gtext( "Enter text. Type ^Z to finish.\n" );
      while( !err )
      {
        zero_cmd();
        if( read_str(1) ) err++;
        else strcat( cmdbuf, "\r\n" );
        if( *cmdbuf )
        {
          len = strlen(cmdbuf);
          if( Fwrite( text_hand, len, cmdbuf ) != len )
          {
            errs( GENERAL, "Error writing file. Disk full?" );
            err++;
          }
          newline();
        }
      }
      Fclose(text_hand);
      update_neo(path);
    }
  }
}

void dotouch()
{
  int hand, time[2], err=0;

  if( first(parmptr[1],0L) > 0 )
  {
    do
      if( dma.d_attrib & S_IJDIR ) fold_msg();
      else
      {
        strcpy( diskbuf, path );
        strcat( diskbuf, dma.d_fname );
        if( (hand=Fopen(diskbuf,0)) < 0 )
        {
          ferrs( hand, "%sopen %s.", cn, dma.d_fname );
          err++;
        }
        else
        {
          time[0] = Tgettime();
          time[1] = Tgetdate();
          Fdatime( (DOSTIME *)time, hand, 1 );
          Fclose(hand);
        }
      }
    while( !err && !_abort() && next() );
    update_neo(path);
  }
}

void dounset()
{
  char *ptr;
  int i;

  if( !strcmp(parmptr[1],"*") )
    for( i=0; i<config.num_vars; i++ )
      vnames[i].c[0] = vvals[i].c[0] = '\0';
  else
  {
    if( (i=find_var( parmptr[1], &ptr )) < 0 ) bad_var( parmptr[1] );
    else if( i>0 ) *ptr = vnames[i-1].c[0] = '\0';
    else var_not_found();
  }
}

void dovt(void)
{
  long l;
  int i, cnt, nul, s;
  char *ptr;
  static char eqstr[]="\033? ", eqs[]="wxytuvqrsp", fk[]="\033 ";
  void view_hist();

  parms=2;
  i = iredir_hand[parse_lev];
  if( in_vt )
  {
    vcurs_off();
    newline();
    dopause();
    echo("\f");
  }
  else
  {
    hide_mouse();
    in_vt = 1;
    vt_appl = 0;
  }
  *((parmptr[1] = path)+1) = '\0';
  force_cur(0);
  for(;;)
  {
    cnt = CMDBUFLEN;
    ptr = path;
    nul = 0;
    while( Bconstat(1) && cnt && !nul )
    {
      cnt--;
      if( (*ptr++ = Bconin(1)&0x7F) == 0 ) nul++;
    }
    if( cnt<CMDBUFLEN )
    {
      *ptr++ = '\0';
      doecho();
    }
    if( nul )
    {
      path[0] = '\0';
      bcon_nul = 1L;
      doecho();
    }
    if( i > 5 || Bconstat(i) )
      if( (l=bconinr()) == -1L ) l = 0x7;
      else if( l==-2L || (s=l>>16)==0x61 )
      {
        in_vt = 0;
        force_cur(1);
        show_mouse();
        return;
      }
      else if( s==0x62 ) view_hist();
/*      {
        force_cur(1);
        row = col = inverse = discard = 0;
        erase_ln( 0, maxcol );
        gtext( "Command: " );
        zero_cmd();
        read_str(0);
        cls();
        return;
      }*/
      else if( s>=0x67 && s<=0x70 && vt_appl )
      {
        eqstr[2] = eqs[s-0x67];
        bcauxstr(eqstr);
      }
      else if( s>=0x3b && s<=0x44 && vt_appl )
      {
        fk[1] = s-0x3b+'P';
        bcauxstr(fk);
      }
      else if( s==0x48 ) bcauxstr( "\033A" );
      else if( s==0x50 ) bcauxstr( "\033B" );
      else if( s==0x4D ) bcauxstr( "\033C" );
      else if( s==0x4B ) bcauxstr( "\033D" );
      else Bconout( 1, (char)l );
  }
}

void dountil()
{
  int i;

  if( until_lev < 0 ) ferrs( SYNTAX, wo, "UNTIL", "REPEAT" );
  else if( (i=logical( parmptr[1] )) != 0 )
    if( i==2 ) jump_to_line( until_line[until_lev], until_pos[until_lev] );
    else until_lev--;
}

void dowarm()
{
  call_w_save( (int (*)())neo_acc->warmboot );
}

void where_file( char *str )
{
  strcpy( item_path, path );
  strcpy( pathend(item_path), str );
}

int match( char *str, char *pat )
{
  char s, p, per=0;

  for(;;)
  {
    if( (p = *pat++) == '\0' )
      if( *str ) return(0);
      else return(1);
    s = *str++;
    if( p == '*' )
    {
      if( !s ) return(1);
      str--;
      do
      {
        if( *str == '.' ) per=1;
        if( match( str, pat ) ) return(1);
      }
      while( *str++ );
      if( *pat++ != '.' ) return(0);
      if( *pat == '*' )
        if( !*(pat+1) ) return(1);
        else return(0);
      else if( *pat || per ) return(0);
      return(1);
    }
    else if( p == '?' )
    {
      if( !s ) return(0);
    }
    else if( p != s ) return(0);
  }
}

int matchh( char *str, char *pat )
{
  char buf[40], *ptr, *ptr2, *ptr3;
  static char miss[]="Missing %c in wildcard.";
  int i, j;

  for( ptr=buf, j=0; *pat && j<sizeof(buf)-1; pat++ )
    if( *pat == '[' )
    {
      ptr2 = ++pat;
      while( *pat && *pat != ']' ) pat++;
      if( !*pat )
      {
        ferrs( SYNTAX, miss, ']' );
        return(-1);
      }
      ptr3 = ptr;
      while( (*++ptr = *++pat) != 0 && ++j<sizeof(buf)-1 );
      if( *ptr ) *++ptr = '\0';
      while( *ptr2 != ']' )
      {
        *ptr3 = *ptr2++;
        if( (i=matchh( str, buf )) != 0 ) return(i);
        if( *ptr2 == '-' )
        {
          ptr2++;
          while( ++(*ptr3) <= *ptr2 )
            if( (i=matchh( str, buf )) != 0 ) return(i);
          ptr2++;
        }
      }
      return(0);
    }
    else if( *pat == '{' )
      for(;;)
      {
        ptr3 = ptr;
        while( *++pat && !is_sep(*pat) && *pat != '}' )
          *ptr3++ = *pat;
        if( !*pat )
        {
no_brace: ferrs( SYNTAX, miss, '}' );
          return(-1);
        }
        ptr2 = pat;
        while( *ptr2 && *ptr2 != '}' ) ptr2++;
        if( !*ptr2 ) goto no_brace;
        i = j;
        while( (*ptr3++ = *++ptr2) != 0 && ++i<sizeof(buf)-1 );
        if( *ptr3 ) *++ptr3 = '\0';
        if( (i=matchh( str, buf )) != 0 ) return(i);
        if( *pat == '}' ) return(0);
      }
    else *ptr++ = *pat>='a' && *pat<='z' ? (*pat&0x5f) : *pat;
  *ptr = '\0';
  return( match( str, buf ) );
}

int whereis( char *pat, int dir, int times )
{
  DTA *old, new;
  int ret=0, i;
  char *ptr;

  old = Fgetdta();
  Fsetdta( &new );
  if( (i=Fsfirst( path, dir ? 0x37 : 0x27 )) == 0 )
  {
    do
      if( (ret = !_abort()) == 0 ) times=0;
      else if( new.d_attrib & 0x10 )
      {
        if( new.d_fname[0] != '.' || (new.d_fname[1] != '.' &&
            new.d_fname[1] != '\0') )
        {
          where_file( new.d_fname );
          strcpy( path, item_path );
          strcat( path, "\\*.*" );
          if( (ret = whereis( pat, 1, times )) >= 0 ) times=ret;
        }
      }
      else if( matchh( new.d_fname, pat ) )
      {
        where_file( new.d_fname );
        gtext( item_path );
        newline();
        if( !--times ) ret = 0;
      }
    while( times>0 && ret>=0 && (ret=Fsnext()) == 0 );
    if( ret == AENMFIL )
      if( (ret=times) != 0 )
      {
        *((ptr=pathend(path))-1) = '\0';
        strcpy( pathend(path), ptr );
      }
  }
  else if( i != AEFILNF ) ret=i;
  Fsetdta( old );
  return(ret);
}

void dowhere()
{
  long l;
  int i, dir, p;
  int times, old;

  times = 32767;
  if( (i=find_opt("n+")) != 0 )
    if( is_num3( parmptr[i], &l ) )
    {
      if( l>=0 ) times=l;
      shift_parms( i, 1 );
    }
  dir = !find_opt("d-");
  if( !parms ) errs( GENERAL, hlp_whe );
  else
  {
    old = times;
    for( p=1; times>0 && p<parms; p++ )
    {
      get_path(parmptr[p]);
      strcat( path, "*.*" );
      if( check_drv(*path,1) )
        times = whereis( pathend(parmptr[1]), dir, times );
    }
    if( times==old ) gtext( "No match found." );
    else if( times<0 ) errs( times, "Error in WHEREIS." );
  }
}

void dowhile()
{
  if( ck_batch() )
  {
    if( while_lev < 0 || while_line[while_lev] != line_num )
      if( while_lev < MAX_WHILE-1 ) while_lev++;
      else
      {
        ferrs( SYNTAX, nst, "WHILE", "loop" );
        return;
      }
    if( while_lev >= 1 && while_line[while_lev-1] < 0 ) while_line[while_lev]=-1;
    else if( logical( parmptr[1] )==1 )
    {
      while_line[while_lev] = line_num;
      while_pos[while_lev] = floc;
    }
    else while_line[while_lev] = -1;
  }
}

void dowhilend()
{
  if( while_lev < 0 ) ferrs( SYNTAX, wo, "ENDWHILE", "WHILE" );
  else if( while_line[while_lev] >= 0 ) jump_to_line( while_line[while_lev],
      while_pos[while_lev] );
  else while_lev--;
}

void jump_to_line( int line, long pos )
{
  register int i;

  if( bFseek( pos ) != pos )
  {
    gtext( looperr );
    close_batch();
  }
  else line_num = line-1;
}

void change_wind( int type )
{
  if( wind_type != type )
  {
    close_wind();
    wind_type = type;
    resize( &wind_out, 0 );
    open_it();
  }
}

void dowind()
{
  Rect rec;
  register int i;

  if( parms==1 ) errs( GENERAL, hlp_win );
  else if( (i=*parmptr[1]&0xdf) == 'W' ) change_wind(WIND_TYPE1);
  else if( i == 'N' ) change_wind(wind_type0);
  else if( parms!=5 ) errs( GENERAL, hlp_win );
  else
  {
    rec.x = (i=atoi(parmptr[1])) >= 0 ? i : window.x;
    rec.y = (i=atoi(parmptr[2])) >= 0 ? i : window.y;
    rec.w = (i=atoi(parmptr[3])) >= 0 ? i : window.w;
    rec.h = (i=atoi(parmptr[4])) >= 0 ? i : window.h;
    wind_calc( 0, wind_type, rec.x, rec.y, rec.w, rec.h,
        &rec.x, &rec.y, &rec.w, &rec.h );
    resize( &rec, 0 );
  }
}

f_address()
{
  char *ptr;
  
  if( find_var( func_parm, &ptr ) )
  {
    spf( path, lfmt, (long)ptr );
    return 1;
  }
  else
  {
    var_not_found();
    return 0;
  }
}

f_alert()
{
  if( func_parm[0] != '[' || func_parm[2] != ']' || func_parm[3] != '[' ||
      func_parm[strlen(func_parm)-1] != ']' )
  {
    errs( SYNTAX, "A GEM alert string must be\
 in the format \'[x][line1|line2|line3|line4|line5][Button1|Button2|Button3]\',\
 where \'x\' is from 0 to 3, each optional line is no more than 30\
 characters, and each button no more than 10." );
    return(0);
  }
  mouse_vex(1);
  spf( path, dfmt, f_alert1( func_parm ) );
  mouse_vex(0);
  return(1);
}

f_archive()
{
  return( func_test( S_IJWAC ) );
}

f_ascii()
{
  spf( path, dfmt, *func_parm );
  return(1);
}

f_chr()
{
  long s;

  if( is_num3( func_parm, &s ) )
  {
    path[0] = s;
    path[1] = '\0';
    return(1);
  }
  return(0);
}

f_date()
{
  if( first(func_parm,0L) > 0 ) to_date( dma.d_date );
  else return(0);
  return(1);
}

f_df()
{
  DISKINFO di;

  *func_parm &= 0x5F;
  if( check_drv( *func_parm, 1 ) )
  {
    Dfree( &di, *func_parm - 'A' + 1 );
    spf( path, "%D", di.b_secsiz*di.b_clsiz*di.b_free );
    return(1);
  }
  return(0);
}

f_drive()
{
  get_path(func_parm);
  path[2] = '\0';
  return(1);
}

f_expand()
{
  char *ptr;

  if( !find_var( func_parm, &ptr ) )
  {
    parmptr[1] = func_parm;
    var_not_found();
    return(0);
  }
  strcpy( path, ptr );
  return(1);
}

f_exists()
{
  truth( first(func_parm,(char *)-1L) > 0 );
  return(1);
}

f_extn()
{
  char *ptr;

  strcpy( path, (ptr=rindex(func_parm,'.')) != NULL ? ptr : empty );
  return(1);
}

f_first()
{
  int err;

  dmabuf(0);
  if( (err=Fsfirst( func_parm, 0x37 )) == 0 )
    while( !strcmp( new_dma.d_fname, period ) || !strcmp( new_dma.d_fname,
        pperiod ) && !err ) err = Fsnext();
  if( (bad_next = err) == 0 )
  {
    get_path( func_parm );
    strcpy( item_path, path );
    strcat( item_path, new_dma.d_fname );
  }
  else item_path[0] = '\0';
  dmabuf(1);
  truth( !err );
  return(1);
}

void dmabuf(int flag)
{
  static DTA *dma;

  if( !flag )
  {
    dma = Fgetdta();
    Fsetdta( &new_dma );
  }
  else Fsetdta( dma );
}

f_fname()
{
  char *ptr;

  strcpy( path, pathend(func_parm) );
  return(1);
}

f_getscr()
{
  long a, b;

  if( !scrn_ptr ) truth(0);
  else if( !is_num3( func_parm, &a ) || !is_num3( parmptr[func_pnum+1], &b ) )
      return(0);
  else if( a<0 || b<0 || a>maxrow || b>maxcol ) truth(0);
  else
  {
    path[0] = *get_scr( (int)a, (int)b );
    path[1] = '\0';
  }
  return(1);
}

f_hidden()
{
  return( func_test( S_IJHID ) );
}

f_isfile()
{
  if( first(func_parm,0L) > 0 ) truth( !(dma.d_attrib&(S_IJDIR|S_IJVOL)) );
  else return(0);
  return(1);
}

f_isfold()
{
  return( func_test( S_IJDIR ) );
}

f_left()
{
  long l;

  if( is_num3( parmptr[func_pnum+1], &l ) )
  {
    if( l<0 ) l=0;
    if( l>119 ) l=119;
    strncpy( path, func_parm, l );
    *(path+l) = '\0';
    return(1);
  }
  return(0);
}

f_len()
{
  spf( path, "%d", (int)strlen(func_parm) );
  return(1);
}

f_mf()
{
  spf( path, "%D", Malloc(-1L) );
  return(1);
}

f_mid()
{
  long s, l;
  int i;

  if( is_num3( parmptr[func_pnum+1], &s ) &&
      is_num3( parmptr[func_pnum+2], &l ) )
  {
    if( --s<0 ) s=0;
    if( l<0 ) l=0;
    if( s>(i=strlen(func_parm)) ) s = i;
    if( l>i ) l=i;
    strncpy( path, func_parm+s, l );
    *(path+l) = '\0';
    return(1);
  }
  return(0);
}

int hasch(void)
{
  long pos;
  int hand, ret;
  char c=0;

  hand = iredir_hand[parse_lev];
  if( hand < 0 ) return( vvals[-hand-1].c[0] != 0 );
  else if( hand < 5 ) ret = Bconstat(hand);
  else if( hand == 5 ) return(0);
  else
  {
    pos = Fseek( 0L, hand, 1 );
    if( ichar_cr[parse_lev] )
    {
      Fread( hand, 1L, &c );
      if( c == '\n' ) pos++;
    }
    ret = Fseek( 0L, hand, 2 ) != pos;
    Fseek( pos, hand, 0 );
  }
  return(ret);
}

f_hasch()
{
  truth( hasch() );
  return(1);
}

f_name()
{
  char *ptr, *end;

  end = pathend(func_parm);
  if( (ptr=rindex(func_parm,'.')) != NULL && ptr>=end ) *ptr='\0';
  strcpy( path, end );
  return(1);
}

f_next()
{
  if( bad_next )
  {
    ferrs( SYNTAX, wo, "%NEXT", "%FIRST" );
    return(0);
  }
  dmabuf(0);
  if( (bad_next = Fsnext()) == 0 ) strcpy( rindex(item_path,'\\')+1,
      new_dma.d_fname );
  else item_path[0] = '\0';
  dmabuf(1);
  truth( !bad_next );
  return(1);
}

f_not()
{
  register int i;

  if( (i=logical(func_parm)) != 0 )
  {
    truth(i-1);
    return(1);
  }
  return(0);
}

int logical( char *ptr )        /* TRUE=1, FALSE=2 */
{
  int i;

  if( (i=strcmp(ptr,true)) == 0 || !strcmp(ptr,false) ) return(!i ? 1 : 2);
  errs( SYNTAX, "TRUE or FALSE expected." );
  return(0);
}

f_path()
{
  get_path(func_parm);
  strcpy( path, path+2 );
  return(1);
}

f_right()
{
  long l;
  int pos;

  if( is_num3( parmptr[func_pnum+1], &l ) )
  {
    if( l<0 ) l=0;
    if( (pos=strlen(func_parm)-l) < 0 ) pos=0;
    strcpy( path, func_parm+pos );
    return(1);
  }
  return(0);
}

f_rscan()
{
  char *ptr, *ptr2, *ptr3;

  ptr3 = 0L;
  if( *(ptr2 = func_parm) != 0 )
    while( (ptr=strstr( ptr2, parmptr[func_pnum+1] )) != 0 )
    {
      ptr3 = ptr;
      ptr2 = ptr+1;
    }
  spf( path, dfmt, ptr3 ? (int)(ptr3-func_parm)+1 : 0 );
  return(1);
}

f_scan()
{
  char *ptr3;

  ptr3 = 0L;
  if( *func_parm != 0 ) ptr3 = strstr( func_parm, parmptr[func_pnum+1] );
  spf( path, dfmt, ptr3 ? (int)(ptr3-func_parm)+1 : 0 );
  return(1);
}

int get_dev( char *dev )
{
  return( list_scan( dev, devs, sizeof(devs)/4 ) );
}

int redirect( char *str, int *hand,
    long (*dosf)(const char *p, int m), char *msg, int md )
{
  if( (*hand = get_dev(func_parm)) < 0 )
  {
    get_path( func_parm );
    strcat( path, pathend(func_parm) );
    if( (*hand = (*dosf)(path,md)) < 0 )
    {
      strcpy( str, devs[*hand = 2] );
      ferrs( *hand, "Error %sing redirection file %s.", msg, path );
      return(0);
    }
    strncpy( str, path, VAR_LEN );
  }
  else strcpy( str, devs[*hand] );
  return(1);
}

f_redir()
{
  return( redirect( output[parse_lev-1], &redir_hand[parse_lev], &Fcreate,
      "creat", 0 ) );
}

f_eredir()
{
  return( redirect( _error[parse_lev-1+1], &eredir_hand[parse_lev], &Fcreate,
      "creat", 0 ) );
}

f_iredir()
{
  return( redirect( input[parse_lev-1], &iredir_hand[parse_lev], &Fopen,
      op, 0 ) );
}

f_redira()	/* output redirect with append */
{
  if( !redirect( output[parse_lev-1], &redir_hand[parse_lev], &Fopen,
      op, 0 ) ) return(0);
  Fseek( 0L, redir_hand[parse_lev], 2 );
  return(1);
}

f_ioredir()	/* input and output redirect */
{
  if( !redirect( input[parse_lev-1], &iredir_hand[parse_lev], &Fopen,
      op, 2 ) ) return(0);
  redir_hand[parse_lev] = iredir_hand[parse_lev];
  strcpy( output[parse_lev-1], input[parse_lev-1] );
  return(1);
}

int varredir(int in)
{
  int i;
  char *ptr;

  uppercase(func_parm);
  if( (i=find_var( func_parm, &ptr )) < 0 )     /* CLI var */
  {
    bad_var( func_parm );
    return(0);
  }
  else if( !i )                                 /* not found */
    if(in)
    {
      var_not_found();
      return(0);
    }
    else if( (i=new_uvar(func_parm,""))==0 ) return(0);       /* create new? */
  if(in)
  {
    iredir_hand[parse_lev] = -i;
    input[parse_lev-1][0] = '$';
    strcpy( input[parse_lev-1]+1, func_parm );
  }
  else
  {
    redir_hand[parse_lev] = -i;
    output[parse_lev-1][0] = '$';
    strcpy( output[parse_lev-1]+1, func_parm );
  }
  return(1);
}

f_size()
{
  if( first(func_parm,0L) > 0 ) spf( path, lfmt, dma.d_length );
  else return(0);
  return(1);
}

f_sysdate()
{
  to_date( (int)Tgetdate() );
  return(1);
}

f_systime()
{
  to_time( (int)Tgettime() );
  return(1);
}

f_time()
{
  if( first(func_parm,0L) > 0 ) to_time( dma.d_time );
  else return(0);
  return(1);
}

f_upcase()
{
  register char *ptr;

  uppercase( func_parm );
  strcpy( path, func_parm );
  return(1);
}

f_write()
{
  if( first(func_parm,0L) > 0 ) truth( !(dma.d_attrib&S_IJRON) );
  else return(0);
  return(1);
}

func_test( int bit )
{
  if( first(func_parm,0L) > 0 ) truth( dma.d_attrib&bit );
  else return(0);
  return(1);
}

int o_mult(i)
{
  long a, b;

  if( is_num( i, &a, &b, 1 ) ) spf( path, lfmt, a*b );
  else return(0);
  return(1);
}

int is_num( int i, long *a, long *b, int flg )
{
  int a1, b1;

  a1 = is_num2( parmptr[i-1], a );
  b1 = is_num2( parmptr[i+1], b );
  if( a1 && b1 ) return(1);
  if( flg ) errs( SYNTAX, "Arithmetic operators require numeric operands." );
  return(0);
}

int is_num3( char *ptr, long *num )
{
  if( is_num2(ptr,num) ) return(1);
  errs( SYNTAX, "Number expected." );
  return(0);
}

int is_num2( char *ptr, long *num )
{
  register char *p=ptr;
  extern long atol();

  if( !*p ) return(0);
  while( *p )
  {
    if( (*p<'0' || *p>'9') && (*p!='-' || p>ptr) || *p=='-' && !*(p+1) )
        return(0);
    p++;
  }
  *num = atol(ptr);
  return(1);
}

int o_div(i)
{
  long a, b;

  if( is_num( i, &a, &b, 2 ) )
    if( b ) spf( path, lfmt, a/b );
    else
    {
      errs( GENERAL, "Cannot DIV by zero." );
      return(0);
    }
  else return(0);
  return(1);
}

int o_plus(i)
{
  long a, b;

  if( is_num( i, &a, &b, 2 ) ) spf( path, lfmt, a+b );
  else return(0);
  return(1);
}

int o_minus(i)
{
  long a, b;

  if( is_num( i, &a, &b, 2 ) ) spf( path, lfmt, a-b );
  else return(0);
  return(1);
}


o_cat(i)
{
  strcpy( path, parmptr[i-1] );
  strcat( path, parmptr[i+1] );
  return(1);
}

o_less(i)
{
  long a, b;

  truth( is_num( i, &a, &b, 0 ) ? a<b :
      strcmp( parmptr[i-1], parmptr[i+1] )<0 );
  return(1);
}

void truth(int n)
{
  strcpy( path, n ? true : false );
}

o_le(i)
{
  long a, b;

  truth( is_num( i, &a, &b, 0 ) ? a<=b :
      strcmp( parmptr[i-1], parmptr[i+1] )<=0 );
  return(1);
}

o_great(i)
{
  long a, b;

  truth( is_num( i, &a, &b, 0 ) ? a>b :
      strcmp( parmptr[i-1], parmptr[i+1] )>0 );
  return(1);
}

o_ge(i)
{
  long a, b;

  truth( is_num( i, &a, &b, 0 ) ? a>=b :
      strcmp( parmptr[i-1], parmptr[i+1] )>=0 );
  return(1);
}

o_eq(i)
{
  long a, b;

  truth( is_num( i, &a, &b, 0 ) ? a==b :
      strcmp( parmptr[i-1], parmptr[i+1] )==0 );
  return(1);
}

o_ne(i)
{
  long a, b;

  truth( is_num( i, &a, &b, 0 ) ? a!=b :
      strcmp( parmptr[i-1], parmptr[i+1] )!=0 );
  return(1);
}

o_and(i)
{
  register int a, b;

  if( (a=logical(parmptr[i-1])) != 0 )
    if( (b=logical(parmptr[i+1])) != 0 )
    {
      truth( a==1 && b==1 );
      return(1);
    }
  return(0);
}

o_or(i)
{
  register int a, b;

  if( (a=logical(parmptr[i-1])) != 0 )
    if( (b=logical(parmptr[i+1])) != 0 )
    {
      truth( a==1 || b==1 );
      return(1);
    }
  return(0);
}

void name_it( char *s )
{
  wind_set( w_handle, WF_NAME, s, 0, 0 );
}

void open_it()
{
  if( !neo_acc )
#ifndef DEMO
      f_alert1( "[1][Are you running NeoDesk 2.05|or newer? Is NEO_CLI in the|\
\"Accessories\" list? Is NeoDesk|set not to Unload for Execute?][Um...]" );
#else DEMO
      f_alert1( demo_msg );
#endif DEMO
  else if( w_handle <= 0 )                      /* no window open */
  {
    wind_calc( 0, wind_type, window.x, window.y, window.w, window.h,
        &wind_out.x, &wind_out.y, &wind_out.w, &wind_out.h );
    if( (w_handle=wind_create( wind_type, wind_out.x, wind_out.y,
        wind_out.w, wind_out.h )) < 0 ) f_alert1(
        "[1][You already have the|maximum number of|windows open!][Bleh!]" );
    else
    {
      if( vdi_hand < 0 ) vbl( vcursor );
      emulti.type |= MU_M1|MU_BUTTON;
      name_it(name+2);
      wind_set( w_handle, WF_VSLSIZE, 1000, 0, 0 );
      wind_set( w_handle, WF_HSLSIZE, 1000, 0, 0 );
      wind_open( w_handle, wind_out.x, wind_out.y, wind_out.w, wind_out.h );
      clip = window;
      rc_intersect( &maximum, &clip );
      rect2 = maximum;
      rect2.x--;
      rect2.w += 2;
#ifndef DEMO
      set_traps(1);
#endif
      reopen = prog_text = was_closed = 0;
      is_top = 1;
      vcurs_on(1);
      close_redir();
      close_iredir();
      close_eredir();
    }
  }
}

void top_it()
{
  if( w_handle > 0 )
    if( !top_wind() )
    {
      wind_set( w_handle, WF_TOP, w_handle, 0, 0, 0 );
      redraw(1);
    }
    else f_alert1( CLI_ALRT );
}

int open_batch( char *ptr )
{
  register int i;

  if( in_batch )
  {
    errs( GENERAL, "Cannot run batch files within batch files." );
    return(0);
  }
  if( (batch_hand=Fopen(ptr,0))<0 ) return(1);
  line_num=0;
  memcpy( bcmdbuf, cmdbuf, CMDBUFLEN );
  strcpy( bparmptr[0]=bpath, ptr );
  for( i=1, bparms=parms; i<bparms; i++ )
    bparmptr[i] = parmptr[i] - cmdbuf + bcmdbuf;
  return(2);
}

void batch(void)
{
  int end=0, next_line;

  while( in_batch && w_handle>0 && !reopen &&
      (end=get_line( cmdbuf )) == 0 )
  {
    line_num++;
next:
    _abort();
    if( in_batch )
    {
      next_line = line_num;
      if( this_line ) goto tl;
      if( debug )
      {
        force_debug(0);
        gtext( "¯" );
        gtext( cmdbuf );
        gtext( "®" );
        newline();
        force_debug(1);
      }
      parse();
      lastline = next_line;
      if( this_line )
      {
tl:     line_num = this_line;
        this_line = 0;
        strcpy( cmdbuf, diskbuf );
        goto next;
      }
    }
  }
  if( end || w_handle<=0 && !paused && !pexec )
  {
    elapsed();
    close_batch();
  }
}

void close_batch()
{
  register int i;

  if( in_batch )
  {
    Fclose(batch_hand);
    for( i=MAX_PARM; --i>=0; )
      bparmptr[i] = empty;
    in_batch=bparms=paused=new_batch=0;
    zero_cmd();
    dec_parselev();
  }
  reset_mouse();
  in_if = while_lev = until_lev = for_lev = call_lev = -1;
  menu=eol=this_line=0;
  *quit_label = *err_label = '\0';
}

int top_wind()
{
  int i;

  wind_get( 0, WF_TOP, &i, &dum, &dum, &dum );   /* who's on top? */
  return( i==w_handle );
}

void reset_mouse()
{
  if( neo_acc ) call_w_save( (int (*)())neo_acc->mas->clear_mouse );
  graf_mouse( ARROW, 0L );
}

int get_selx( int pos, int i, int w )
{
  return( (pos-i)/w );
}

char *get_sel( int i, int j, int *selrow, int *selcol )
{
  if( (*selcol = get_selx( i, window.x, char_w )) < 0 ) *selcol=0;
  else if( *selcol > maxcol+1 ) *selcol = maxcol+1;
  if( (*selrow = get_selx( j, window.y, char_h )) < 0 )
      *selrow=*selcol=0;
  else if( *selrow > maxrow )
  {
    *selrow = maxrow;
    *selcol = maxcol+1;
  }
  return( get_scr( *selrow, *selcol ) );
}

void draw_sel( int row1, int col1, int row2, int col2 )
{
  int i, px[4];

  hide_mouse();
  wmode2();
  if( row1 > row2 || row1==row2 && col1 > col2 )
  {
    i = row1;
    row1 = row2;
    row2 = i;
    i = col1;
    col1 = col2;
    col2 = i;
  }
  if( row1 < 0 ) row1 = col1 = 0;
  if( row2 > maxrow )
  {
    row2 = maxrow;
    col2 = maxcol+1;
  }
  col1 *= char_w;
  for( --row1; ++row1<=row2; )
  {
    px[0] = window.x + col1;
    px[2] = window.x + (row1==row2 ? col2 : maxcol+1)*char_w;
    if( px[2]-- != px[0] )
    {
      px[3] = (px[1] = window.y + row1*char_h) + char_h - 1;
      call_w_save( (int (*)())neo_acc->blank_box, px );
    }
    col1 = 0;
  }
  show_mouse();
}

void insert_char( int ch );
void to_right(void);
void to_left(void);

void mouse_curs(void)
{
  int r, c, i, j;

  get_sel( emulti.mouse_x, emulti.mouse_y, &r, &c );
  if( (i = (r-row)*(maxcol+1) + c-col) > (j=strlen(cmdptr)) ) i = j;
  else if( i < (j=cmdbuf-cmdptr) ) i = j;	/* before start of line */
  if(i)
  {
    vcurs_off();
    if( i>0 ) while( --i>=0 )
      to_right();
    else while( ++i<=0 )
      to_left();
    vcurs_on(0);
  }
}

int x_scrp_get( char *out, int delete )
{
  long map;
  int ret;
  char ok=0, *e;
  DTA dta, *old;

  old = Fgetdta();
  Fsetdta(&dta);
  scrp_read(out);
  map = Drvmap();
  if( *out && *(out+1)==':' && (map & (*out&=0x5f)-'A')!=0 )
    if( (e=pathend(out))<=out+3 && !*e )
    {
      ok=1;
      if( e<out+3 ) strcat(out,"\\");
    }
    else
    {
      if( !*e ) *(e-1) = 0;
      if( !Fsfirst(out,FA_SUBDIR) ) ok=1;
      strcat(out,"\\");
    }
  if( !ok )
  {
    out[0] = (map&(1<<2)) ? 'C' : 'A';
    strcpy( out+1, ":\\CLIPBRD" );
    if( Fsfirst(out,0x37) && Dcreate( out ) )
    {
      Fsetdta(old);
      return 0;
    }
    strcat( out, "\\" );
  }
  scrp_write(out);
  if( delete )
  {
    strcat( out, "SCRAP.*" );
    ret = Fsfirst( out, 0x23 );
    while( !ret )
    {
      strcpy( pathend(out), dta.d_fname );
      Fdelete( out );
      ret = Fsnext();
    }
  }
  *pathend(out) = 0;
  Fsetdta(old);
  return 1;
}

int scrp_txt( char *p, int del )
{
  if( x_scrp_get( p, del ) )
  {
    strcat( p, "SCRAP.TXT" );
    return 1;
  }
  return 0;
}

int edit_to_clip(void)
{
  char temp[120];
  int h;

  if( scrp_txt( temp, 1 ) && (h = Fcreate(temp,0)) > 0 ) return h;
  return 0;
}

void edit_from_clip(void)
{
  char temp[120];
  int h;

  if( scrp_txt( temp, 0 ) && (h = Fopen(temp,0)) > 0 )
  {
    for(;;)
    {
      if( Fread( h, 1L, temp ) <= 0 ) break;
      if( temp[0]=='\r' || temp[0]=='\n' ) break;
      if( cmdptr < cmdbuf+cmdbufmax && (!insert ||
          strlen(cmdbuf) < cmdbufmax) ) insert_char(temp[0]);
    }
    Fclose(h);
  }
}

void view_hist(void)
{
  char *optr, *obot, in_sel, *ptr, *sel_start, *sel_end, seldir, scroll=0;
  int i, j, old_hid, old_mouse[37], selrow, selcol, endrow, endcol,
      newrow, newcol, h, k;
  long *iptr, *iptr2;

  in_hist++;
  if( (optr = scrn_ptr) == 0L )
  {
    ring_bell();
    goto out;
  }
  obot = scrn_bot;
  vcurs_off();
  old_hid = mousehidden;
  memcpy( old_mouse, iptr=(long *)M_POS_HX, sizeof(old_mouse) );
  hide_mouse();
  *iptr++ = 0x10001L;
  *iptr++ = 0x10000L;
  *((int *)iptr)++ = 15;
  if( (j = char_h) < 16 ) j += 2;
  iptr2 = iptr + (i=j);
  for( ; ++i<17; )
    *iptr2++ = 0L;
  iptr2 = iptr + j - 1;
  *iptr++ = *iptr2-- = 0xff000000L;
  if( j != 8 ) *iptr++ = *iptr2-- = 0xff007e00L;
  *iptr++ = *iptr2 = 0x7e003c00L;
  while( iptr != iptr2 )
    *iptr++ = 0x3c001800L;
  while( mousehidden > 0 ) show_mouse();
  call_w_save( (int (*)())neo_acc->set_clip_rect, &clip, 1 );
  for(in_sel=0;;)
  {
    i=mouseposx;
    j=mouseposy;
    if( mousebutstate&1 && !prog_text )
      if( !in_sel )
      {
        if( i >= window.x && j >= window.y &&
            i<window.x+window.w && j<window.y+window.h )
        {
          in_sel++;
          sel_start = sel_end = get_sel( i, j, &selrow, &selcol );
          sel_start -= selcol;
          endrow = endcol = seldir = 0;
        }
        j = 0;
      }
      else if( (ptr = get_sel( i, j, &newrow, &newcol )) !=
          sel_end+endcol )
      {
        sel_end = ptr-newcol;
        i = seldir;
        if( newrow==selrow )
          if( newcol==selcol ) seldir=0;
          else if( newcol > selcol ) seldir=1;
          else seldir = -1;
        else if( newrow > selrow ) seldir = 1;
        else seldir = -1;
        if( seldir != i )
        {
          if( i ) draw_sel( selrow, selcol, endrow, endcol );
          if( seldir ) draw_sel( selrow, selcol, newrow, newcol );
        }
        else if( seldir )
            draw_sel( endrow, endcol, newrow, newcol );
        endrow = newrow;
        endcol = newcol;
        if( j < window.y )
        {
          j = 0x48;
          scroll = seldir > 0;
        }
        else if( j >= window.y+window.h )
        {
          j = 0x50;
          scroll = seldir < 0;
        }
        else j=scroll=0;
        i = 1;
      }
      else j=scroll=0;
    else if( in_sel )
    {
      if( seldir ) draw_sel( selrow, selcol, endrow, endcol );
      j = 0x61;
    }
    else if( Bconstat(2) )
    {
      j = (char)(bconin(2)>>16);
      i = (*kbshift&3) ? maxrow+1 : 1;
    }
    else j=scroll=0;
    switch(j)
    {
      case 0x47:
        j = config.screen_size / scrn_wid;
        if( i==1 )
        {
          if( (i = (scrn_ptr-scrn_top) / scrn_wid) < 0 ) i += j;
          goto up;
        }
        else
        {
          if( (i = (scrn_top-scrn_bot) / scrn_wid) < 0 ) i += j;
          goto down;
        }
      case 0x62:        /* Help */
      case 0x61:
      case 0x01:	/* Esc */
        hide_mouse();
        if( scrn_ptr != optr )
        {
          scrn_ptr = optr;
          scrn_bot = obot;
          redraw_lines( 0, maxrow );
        }
        if( in_sel && seldir )
        {
          name_it("C: Copy | V: Paste | P: Print");
          i = (char)(bconin(2)>>16);
          name_it(name+2);
          h = redir_hand[parse_lev];
          switch(i)
          {
            case 0x2E:
              if( (i=edit_to_clip())<=0 ) goto out2;
              redir_hand[parse_lev] = i;
              break;
            case 0x2F:
              break;
            case 0x19:
              redir_hand[parse_lev] = 0;
              break;
            default:
              goto out2;
          }
          if( seldir<0 )
          {
            intswap( &sel_start, &sel_end, 2 );
            intswap( &selcol, &endcol, 1 );
          }
          i = selcol;
          for(;;)
          {
            j = sel_start==sel_end ? endcol-1 : maxcol;
            ptr = sel_start+i;
            if( redir_hand[parse_lev]==2 )
              for( ; i<=j; i++ )
              {
                if( cmdptr < cmdbuf+cmdbufmax && (!insert ||
                    strlen(cmdbuf) < cmdbufmax) ) insert_char(*ptr++);
              }
            else
            {
              for( optr=sel_start+(k=maxcol+1); --k>=i; )
                if( *--optr!=' ' ) break;
              bcon_nul = (k>j ? j : k)-i+1;
              write_redir( ptr );
              bcon_nul = 0L;
              write_redir( "\r\n" );
            }
            if( sel_start==sel_end ) break;
            inc_scrn(&sel_start);
            i = 0;
          }
          if( redir_hand[parse_lev] > 5 ) Fclose(redir_hand[parse_lev]);
          redir_hand[parse_lev] = h;
        }
out2:   vcurs_on(0);
        memcpy( M_POS_HX, old_mouse, sizeof(old_mouse) );
        while( mousehidden < old_hid ) hide_mouse();
        while( mousehidden > old_hid ) show_mouse();
out:    in_hist--;
        return;
      case 0x48:
up:     for( j=0; i--; )
          if( dec_scrn() ) j++;
        if( j )
        {
          hide_mouse();
          if( j!=maxrow+1 ) blit_lines( 0, maxrow-j, j, 0 );
          redraw_lines( 0, j-1 );
          selrow += j;
          endrow += j;
          if( scroll ) draw_sel( selrow, selcol, 0, maxcol+1 );
          show_mouse();
        }
        break;
      case 0x50:
down:   for( j=0; i--; )
          if( scrn_ptr != optr )
          {
            inc_scrn( &scrn_bot );
            inc_scrn( &scrn_ptr );
            j++;
          }
        if( j )
        {
          hide_mouse();
          if( j != maxrow+1 ) blit_lines( j, maxrow, 0, 0 );
          redraw_lines( maxrow-j+1, maxrow );
          selrow -= j;
          endrow -= j;
          if( scroll ) draw_sel( maxrow, 0, selrow, selcol );
          show_mouse();
        }
        break;
    }
  }
}

void new_mov(void)
{
}

#define USER_BUT *(void (**)())((long)la_init.li_a0-58)
#define USER_MOV *(void (**)())((long)la_init.li_a0-50)

int mouse_vex( int flag )
{
  void reset_mv( void mov(void) ), reset_but(int state), new_but(void);
  extern void (*old_but)(void);
  static void (*old_mov)(void);
  static int state;
  static char valid=0;

  if( !flag )
  {
    if( !valid )
    {
      old_but = USER_BUT;
      USER_BUT = new_mov;
      old_mov = USER_MOV;
      USER_MOV = new_mov;
    }
    state = mousebutstate;
    valid = 1;
  }
  else if( valid )
  {
    if( state != mousebutstate ) reset_but(mousebutstate);
    reset_mv(old_mov);
    USER_BUT = old_but;
    USER_MOV = old_mov;
    valid = 0;
  }
  else return 0;
  return 1;
}

void disp_cmd(int space)
{
  int j;

  oldrow = row;
  j = col;
  gtext( cmdptr );
  if( space ) gtext(sp);
  row = oldrow;
  col = j;
}

void to_right(void)
{
  if( *cmdptr )
  {
    cmdptr++;
    if( ++col > maxcol && row < maxrow )
    {
      col = 0;
      row++;
    }
  }
}

void to_left(void)
{
  if( cmdptr > cmdbuf )
  {
    cmdptr--;
    backsp();
  }
}

void to_start(void)
{
  while( cmdptr > cmdbuf )
  {
    backsp();
    cmdptr--;
  }
}

void insert_char( int ch )
{
  static char string[2]=" ";
  char temp[CMDBUFLEN+2];
  int i, j;

  i = *cmdptr;
  if( insert ) strcpy( temp, cmdptr );
  *cmdptr++ = string[0] = ch;
  if( !i ) *cmdptr = '\0';
  gtext(string);
  if( insert )
  {
    strcpy( cmdptr, temp );
    disp_cmd(0);
  }
}

int read_key( int k )
{
  register int i, j, key, ret=0;
  char *ptr, *ptr2;

  key = k & 0xFF;                 /* isolate just the ASCII value */
  vcurs_off();
  if( key=='\b' )
  {
    if( cmdptr > cmdbuf )
    {
      backsp();
      cmdptr--;
      strcpy( cmdptr, cmdptr+1 );
      disp_cmd(1);
    }
  }
  else if( key=='\x16' ) 	/* ^V */
      edit_from_clip();
  else if( key=='\t' )
  {
    j = (long)cmdptr - (long)cmdbuf;
    i = (j&0xFFF8) + 8 - j;
    while( strlen(cmdbuf) < cmdbufmax && i-- ) insert_char( ' ' );
    if( i>=0 ) ring_bell();
  }
  else if( key == '\x7f' )               /* Del */
  {
    if( *cmdptr )
    {
      strcpy( cmdptr, cmdptr+1 );
      disp_cmd(1);
    }
  }
  else if( key == '\r' ) ret = 1;	/* CR */
  else if( key=='\03' )			/* ^C */
  {
    if( prog_text )
    {
      ctrlc++;
      ret = 1;
    }
    else if( cmdptr!=cmdbuf && (k=edit_to_clip()) > 0 )
    {
      Fwrite( k, strlen(cmdbuf), cmdbuf );
      Fclose(k);
    }
  }
  else if( k == 0x6200 )                /* Help */
  {
    if( !prog_text )
    {
      mouse_vex(0);
      view_hist();
      mouse_vex(1);
    }
    else if( (*kbshift&3) == 3 ) view_hist();
  }
  else if( key == '\x18' )	/* ^X */
  {
    if( cmdptr!=cmdbuf && (k=edit_to_clip()) > 0 )
    {
      Fwrite( k, strlen(cmdbuf), cmdbuf );
      Fclose(k);
      clear_cmd();
      *cmdptr = '\0';
    }
  }
  else if( k == 0x4700 )  	/* Clr */
  {
    clear_cmd();
    *cmdptr = '\0';
  }
  else if( k == 0x4d36 )        /* Shift-right */
  {
    while( *cmdptr ) to_right();
  }
  else if( k == 0x4b34 ) to_start();    /* Shift-left */
  else if( k == 0x4838 ) ret = 2;   /* Shift-up */
  else if( k == 0x5032 ) ret = 3;   /* Shift-down */
  else if( key == '\x1a' ) ret = 4;  /* ^Z */
  else if( key && key != '\n' && cmdptr < cmdbuf+cmdbufmax && (!insert ||
      strlen(cmdbuf) < cmdbufmax) ) insert_char(key);
  else if( k == 0x5200 ) insert = !insert;
  else if( k == 0x4800 )                /* Up */
  {
    if( cmdptr > cmdbuf+maxcol )
    {
      cmdptr -= maxcol + 1;
      row--;
    }
  }
  else if( k == 0x5000 )                /* Down */
  {
    if( cmdptr < cmdbuf+strlen(cmdbuf)-maxcol )
    {
      cmdptr += maxcol + 1;
      row++;
    }
  }
  else if( k == 0x4B00 ) to_left();      /* Left */
  else if( k == 0x4D00 ) to_right();    /* Right */
  else ring_bell();
  vcurs_on(0);
  return(ret);
}

void prep_exec(void)
{
  if( paused ) close_batch();
  open_before = w_handle > 0;
  if( w_handle <= 0 ) open_it();
  else if( !top_wind() ) top_it();
  redraw(0);
  ignore_redraw = 1;
}

void get_ver()
{
  if( (tos_ver = OS_version) > 0x100 ) kbshift = OS_kbs;
}

void cond_prompt(void)
{
  if( w_handle > 0 && !reopen && !in_batch ) prompt();
}

int new_path(void)
{
  if( !neodesk_present )
  {
    strcpy( path, dflt_path );
    dflt_path[0] = Dgetdrv()+'A';
    Dgetpath( dflt_path+2, 0 );
    strcat( dflt_path, slash );
    return( strcmp(path,dflt_path) );
  }
  return(0);
}

void cmdcpy( char *to, char *from, int to_in_buf )
{
  char c;

  do
  {
    *to++ = c = *from++;
    if( to_in_buf && to >= &old_cmds[config.backup_size] ) to -= config.backup_size;
    else if( from >= &old_cmds[config.backup_size] ) from -= config.backup_size;
  }
  while(c);
}

void dec_cmd( char **ptr )
{
  if( --*ptr < old_cmds ) *ptr += config.backup_size;
}

void inc_cmd( char **ptr )
{
  if( ++*ptr >= &old_cmds[config.backup_size] ) *ptr -= config.backup_size;
}

char *search_cmd( char *newcmd )
{
  char temp[sizeof(cmdbuf)];

  while( *newcmd )
  {
    cmdcpy( temp, newcmd, 0 );
    while( *newcmd ) inc_cmd( &newcmd );
    inc_cmd( &newcmd );
    if( !strncmp( temp, cmdbuf+1, strlen(cmdbuf+1) ) )
    {
      strcpy( cmdbuf, temp );
      return(newcmd);
    }
  }
  ring_bell();
  return(0);
}

void set_ver(void)
{
  vdi_hand = (neo_ver = neo_acc->nac_ver) >= 0x0303 ?
      (graphics=neo_acc->mas->most->graphics)->handle : -1;
}

int main( int argc, char *argv[] )
{
  int buffer[8], i, j, mum1=0, box_w, box_h;
  char *ptr, *ptr2, *oldcmd, *newcmd, last_dir=0, ok=1, has_gnva;
  long stack;

  emulti.type=MU_MESAG|MU_KEYBD;
  emulti.state = emulti.mask = 1;
  emulti.clicks = 2;
  apid = appl_init();
  multitask = _GemParBlk.global[1]==-1 && _GemParBlk.global[0]>=0x400;
  has_gnva = getcookie( GENEVA_COOKIE, &stack ) == CJar_OK && stack;
  if( getcookie( MagX_COOKIE, &stack ) == CJar_OK )
  {
    has_magx = multitask = 1;
  }
  if( _GemParBlk.global[0]>=0x400 ) shel_write( 9, 1, 0, 0L, 0L );
  if( _GemParBlk.global[0]>=0x340 )
  {
    appl_getinfo( 10, &sw_bits, &sw0, buffer, &sw_argv );
    if( has_gnva ) sw_bits = -1;	/* avoid a bug in 003 */
    else sw_bits &= (1<<10)|(1<<11);	/* only bits I care about */
  }
  if( _app )
#ifndef DEMO
    if( argc<2 || (neo_acc = (NEO_ACC *)atol(argv[1])) == 0 || (long)neo_acc & 1 ||
        neo_acc->nac_ver < NAC_VER_300 )
    {
      f_alert1(
"[1][In order to run as a program,|NEO_CLI needs to be run from|NeoDesk \
3.00 or newer with the|.NPG filename extension][Ok]" );
out:  appl_exit();
      return(-1);
    }
    else
    {
      set_ver();
      open_before = 1;
    }
#else DEMO
  {
    f_alert1( demo_msg );
    appl_exit();
    return(-1);
  }
#endif DEMO
  blink_on = 1;
  blink_rate = Cursconf(5,-1);
  graf_handle( &dum, &dum, &box_w, &box_h );
  if( !_app && (long)Malloc(-1L) < config.real_bufsiz ||
      _app && config.real_bufsiz != config.bufsiz )
  {
    f_alert1( "[1][There is not enough|free memory for \
this|NEO_CLI configuration][Ok]" );
#ifndef DEMO
    if( _app ) goto out;
#endif
    ok=0;
  }
  else if( !_app ) config.buf = Malloc(config.real_bufsiz);
  if(ok)
  {
    if( has_gnva && stack!=0L ) wind_type0 = WIND_TYPE0G;
    wind_type = wind_type0;
    memclr( config.buf, config.real_bufsiz );
    vnames = (struct Vnames *)config.buf;
    vvals = (struct Vvals *)&vnames[config.num_vars].c[0];
    oldcmd = (newcmd = old_cmds = &vvals[config.num_vars].c[0]) +
        config.backup_size;
    rsio = Iorec(0);
    if( config.rs232_buf > rsio->ibufsiz )
    {
      oldrsbuf = (char *)rsio->ibuf;
      rsio->ibuf = oldcmd;
      oldrssize = rsio->ibufsiz;
      rsio->ibufsiz = config.rs232_buf;
      rsio->ibufhd = rsio->ibuftl = 0;
    }
    line_buf = oldcmd+config.rs232_buf;
    clear_screen();
    ptr = alias = screen + config.screen_size;
    dflt_path[0] = Dgetdrv() + 'A';
    dflt_path[1] = ':';
    Dgetpath( dflt_path+2, 0 );
    strcat( dflt_path, slash );
    linea0();
    wind_get( 0, WF_WORKXYWH, &maximum.x, &maximum.y, &maximum.w, &maximum.h );
    *(Rect *)&emulti.m1x = maximum;
    char_w = fonttab[fontno][0];
    char_h = fonttab[fontno][1];
    for( i=MAX_PARM; --i>=0; )
      bparmptr[i] = empty;
    has_blit = Blitmode(-1)&1;
    Supexec((long (*)())&get_ver);
    cmdptr = cmdbuf;
    io = Iorec(1);
  }
  if( !_app && ok || multitask ) menu_register( apid, name );
  if( _app )
  {
    memcpy( env_vars, _BasPag->p_env, sizeof(env_vars)-2 );
    set_PATH();
    open_it();
    new_path();
    reset_mouse();
    if( argc >= 3 )
    {
      redraw(0);
      ignore_redraw++;
      for( i=2; i<argc; i++ )
      {
        strcat( cmdbuf, argv[i] );
        strcat( cmdbuf, sp );
      }
      mouse_vex(0);
      parse();
      cond_prompt();
      mouse_vex(1);
    }
  }

  for(;;)
  {
    multi_evnt( &emulti, buffer );

    if( emulti.event & MU_BUTTON && emulti.times==1 && emulti.mouse_b&1 &&
        !in_batch )
      if( wind_type==WIND_TYPE1 && emulti.mouse_x >= window.x+window.w-box_w &&
          emulti.mouse_y >= window.y+window.h-box_h && mousebutstate&1 )
      {
        if( graf_rubberbox( wind_out.x, wind_out.y, (MAX_LEN_OTH+1)*char_w,
            MIN_H+box_h, &temp_rect.w, &temp_rect.h ) )
        {
          temp_rect.x = wind_out.x;
          temp_rect.y = wind_out.y;
          resize( &temp_rect, 1 );
        }
      }
      else if( emulti.mouse_x >= window.x && emulti.mouse_x < window.x +
          window.w && emulti.mouse_y >= window.y && emulti.mouse_y < window.y +
          window.h && top_wind() )
        if( !(mousebutstate&1) ) mouse_curs();
        else
        {
          mouse_vex(0);
          view_hist();
          mouse_vex(1);
        }
    if( emulti.event & MU_MESAG )                        /* message event */
    {
      switch( buffer[0] )
      {
        case NEO_AC_OPEN:
        case AC_OPEN:
          if( !ok ) break;
          if( w_handle <= 0 )
          {
            i = reopen;
            ignore_redraw = 0;
            if( neo_acc ) open_before=1;
            else
            {
              find_neo(buffer);
              break;
            }
            open_it();
            if( paused && neo_acc )
            {
              redraw(0);
              ignore_redraw = 1;
              paused=0;
/*              if( w_handle > 0 && !reopen ) prompt();*/
            }
            else if( new_path() || i ) cond_prompt();
            break;
          }
        case WM_TOPPED:
          top_it();
          if( reopen ) was_closed++;
          break;
#ifndef DEMO
        case NEO_ACC_INI:
#else DEMO
        case CLI_DEMO_INI:
#endif
          if(ok)
            if( buffer[3]==NEO_ACC_MAGIC )
            {
              neo_acc = *(NEO_ACC **)&buffer[4];
              retries = 0;
              set_ver();
              neo_apid = buffer[6];
              neodesk_present = 1;
              if( prog_text ) was_closed++;
              if( !got_env )
              {
                memcpy( env_vars, env_ptr(), sizeof(env_vars)-2 );
                set_PATH();
                got_env++;
              }
            }
            else /* CH_EXIT for Geneva<=006 */
            {
              errnum = buffer[4];
              was_closed++;
            }
          break;
        case CH_EXIT:
          errnum = buffer[4];
          was_closed++;
          break;
        case NEO_CLI_RUN:
          if( buffer[3]==NEO_ACC_MAGIC )
            if( !neo_acc ) find_neo(buffer);
            else
            {
              ack();
              prep_exec();
              strcpy( cmdbuf, *(char **)(&buffer[4])+1 );
              mouse_vex(0);
              parse();
              cond_prompt();
              mouse_vex(1);
            }
          break;
        case NEO_ACC_PAS:
          if( buffer[3]==NEO_ACC_MAGIC )
            if( !neo_acc ) find_neo(buffer);
            else
            {
              prep_exec();
              if( w_handle > 0 && call_w_save( (int (*)())neo_acc->list_files, &ptr ) )
              {
                ack();
                if( (ptr2=rindex(ptr,':')) == NULL || *(ptr2+1) &&
                    strcmp(ptr2+1,":\\") )
                {
                  strcpy( cmdbuf, ptr );
                  mouse_vex(0);
                  parse();
                  mouse_vex(1);
                }
              }
              else ack();
              cond_prompt();
            }
          break;
        case WM_REDRAW:
          if( !ignore_redraw ) redraw(1);
          else ignore_redraw--;
          if( reopen && multitask )	/* 2.3: added mtask */
            was_closed++;
          break;
        case AC_CLOSE:
/*          if( w_handle > 0 ) reopen++;*/
          if( !prog_text )
          {
            if( w_handle > 0 ) vbl(0L);
            w_handle = -1;
          }
          was_closed++;
          neodesk_present = 0;
          emulti.type &= ~MU_M1;
          break;
        case AP_TERM:
          close_batch();
          parms = 0;
          doext();
          final_exit();
          break;
        case WM_CLOSED:
          parms=0;
          doext();
          break;
        case WM_FULLED:
          intswap( (int *)&rect2, &buffer[4], sizeof(Rect)/sizeof(int) );
        case WM_SIZED:
        case WM_MOVED:        /* move the window somewhere else */
          resize( (Rect *)&buffer[4], 1 );
          break;
#ifndef DEMO
        case NEO_ACC_BAD:
          if( buffer[3] == NEO_ACC_MAGIC )
          {
            neo_acc=0;
            neodesk_present = 0;
            close_batch();
            parms=0;
            doext();
          }
          break;
#endif DEMO
      }
    }
    if( neo_acc && w_handle>0 && !prog_text && !in_batch )
    {
      if( emulti.event & MU_KEYBD && !alias_ptr )
      {
        if( reopen ) was_closed++;
        i = emulti.mouse_k&0xf;
        ptr = alias;
        while( *(ptr+2) || *(ptr+3) )
        {
          ptr2 = ptr;
          ptr = next_str(ptr+2);
          if( *ptr2 == i && *(ptr2+1) == (emulti.key>>8) )
          {
            if( *ptr ) alias_ptr = ptr;
            goto alias_key;
          }
          ptr = next_str(ptr);
        }
      }
      else if( alias_ptr )
      {
alias_key:
        if( (emulti.mouse_k&3) == 3 )
        {
          alias_ptr = 0L;
          goto next_key;
        }
        emulti.key = *alias_ptr++;
        if( !*alias_ptr ) alias_ptr = 0L;
      }
      else goto next_key;
      if( (char)emulti.key=='\x17' )
      {
        parms=0;
        doext();
      }
      else if( (char)emulti.key=='\033' )       /* Esc */
      {
        strcpy( ptr=next_str(cmdptr)-1, "*" );
        for( ptr2=ptr; ptr2!=cmdbuf && !is_sep(*(ptr2-1)); ptr2-- );
        if( search_exe(ptr2) )
        {
          strcpy( ptr2, diskbuf );
          vcurs_off();
          goto disp;
        }
        else
        {
          ring_bell();
          *ptr = '\0';
        }
      }
      else switch( read_key( emulti.key ) )
      {
        case 1:		/* Return */
          if( *cmdbuf=='!' )
          {
            if( last_dir<0 )
            {
              while( *newcmd ) inc_cmd( &newcmd );
              inc_cmd( &newcmd );
            }
            last_dir = 1;
            if( (ptr=search_cmd(newcmd)) != 0 )
            {
              newcmd = ptr;
              goto disp;
            }
            else break;
          }
          else if( *cmdbuf )
          {
            gtext(cmdptr);
            oldcmd -= strlen(cmdbuf);
            dec_cmd( &oldcmd );		/* subtract one more for NUL */
            ptr = oldcmd;
            dec_cmd( &ptr );
            *ptr = '\0';
            dec_cmd( &ptr );
            *ptr = '\0';
            cmdcpy( oldcmd, cmdbuf, 1 );
          }
          newcmd = oldcmd;
          last_dir = 0;
          if( col ) newline();
          mouse_vex(0);
          parse();
          mouse_vex(1);
          ctrlc = 0;
          cond_prompt();
          break;
        case 3:         /* Shift-down */
          ptr = newcmd;
          if( last_dir>0 )
          {
            dec_cmd( &ptr );
            dec_cmd( &ptr );
            while( *ptr )
            {
              ptr2 = ptr;
              dec_cmd( &ptr );
            }
            ptr = ptr2;
          }
          last_dir = -1;
          dec_cmd( &ptr );
          dec_cmd( &ptr );
          vcurs_off();
          if( *ptr )
          {
            while( *ptr )
            {
              ptr2 = ptr;
              dec_cmd( &ptr );
            }
            cmdcpy( cmdbuf, ptr2, 0 );
            dec_cmd( &ptr );
            newcmd = ptr2;
            last_dir = -1;
disp:       clear_cmd();
            *(cmdptr+cmdbufmax) = '\0';
            cmdptr = next_str(cmdbuf)-1;
            gtext( cmdbuf );
          }
          vcurs_on(0);
          break;
        case 2:         /* Shift-up */
          if( last_dir<0 )
          {
            while( *newcmd ) inc_cmd( &newcmd );
            inc_cmd( &newcmd );
          }
          last_dir = 1;
          vcurs_off();
          if( *newcmd )
          {
            cmdcpy( cmdbuf, newcmd, 0 );
            while( *newcmd ) inc_cmd( &newcmd );
            inc_cmd( &newcmd );
            goto disp;
          }
          vcurs_on(0);
          break;
      }
    }
    if( in_batch && !paused && w_handle>0 )
    {
      mouse_vex(0);
      batch();
      cond_prompt();
      mouse_vex(1);
    }
    if( reopen && was_closed && neo_acc && (prog_text||w_handle<=0||multitask) &&
        !paused && (pexec||neodesk_present) )
    {
      if( !multitask ) errnum = neo_acc->nac_ver >= NAC_VER_300 ? ((path[0]<<8)|path[1]) : 0;
      if( !prog_text )
      {
        open_it();
        redraw(0);
        ignore_redraw = 1;
      }
      else
      {
        emulti.type |= MU_M1;
        reset_linea();
        dec_parselev();
        more=0;
        reset_mouse();
#ifndef DEMO
        nstdh=0;
#endif
      }
#ifndef DEMO
      set_traps(1);
#endif
      prog_text = reopen = was_closed = pexec = 0;
      if( time_it == parse_lev+2 )
      {
        time_it--;
        elapsed();
      }
      if( errnum != 0 && !in_batch ) exit_stat();
      eol = 1;
      cond_prompt();
      zero_cmd();
    }
    /* don't insert anything in here */
next_key:
    if( !(++mum1%4) ) is_top = top_wind();
    vdi_cursor(0);
  }
}

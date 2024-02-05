#ifndef _WIN_VAR_
  #define _WIN_VAR_

#include "xwind.h"
#include "tos.h"
#include "aes.h"
#include "vdi.h"
#define Xrect(r) (r).x, (r).y, (r).w, (r).h

#define STKSIZE         40      /* stack for APP struct */
#define MAX_HANDLE      256     /* max no. of windows */
#define INFO_LEN        100     /* length of info bar string */
#define IS_TEAR         -23     /* magic number in info_pos meaning tear-away */
#define SHLINELEN 	120	/* max length of line in GENEVA.CNF */
#define MAX_KEYS	20	/* number of kbd equivs/dialog */

#define DTREECNT	1
#define TREECNT 	WSIZE+1

#define ICON_WID	72
#define ICON_HT		72

#define PATHLEN 120
#define TAILLEN 128

#define SH_RUNHELP	0x7101	/* run Help program */
#define SH_RUNSHELL	0x7102	/* run shell program */
#define SH_RUNSLEEP	0x7103	/* run program asleep */

#ifndef RECTDEF
  typedef struct { int x, y, w, h; } Rect;
  #define RECTDEF
#endif
typedef struct rect_list
{
  struct rect_list *next;
  Rect r;
} Rect_list;

typedef struct windoW
{
  struct windoW *next, *tear_parent;
  int handle, place;
  char unused,		/*****/
       chop_redraw;
  Rect outer, working, full, prev, dirty;
  Rect_list *rects, *rectptr;
  char *top_bar, info_bar[INFO_LEN+1], info_end;
  int apid;
  int hslide,           /* also used for title # of a menu */
      vslide,           /* also for pull # */
      hslidesz, vslidesz,
      hslide2, vslide2,
      hslidesz2, vslidesz2;
  int hsplit, vsplit, hsp_min1, vsp_min1, hsp_min2, vsp_min2;
  int type, xtype, bevent;
  int info_pos;         /* may also have IS_TEAR */
  int min_w, min_h, max_w, max_h;
  OBJECT *tree;
  int treecnt, treeflag;
  OBJECT *menu, *dialog, *tool;
  int menu_tA, menu_tZ;
  int dial_obj, dial_edit, dial_edind,
      dial_swid, dial_sht;
  int colors[2][WGSIZE+1];	/* 003 */
  int cdecl (*objhand)( int hand, int obj );
  Rect iconify;
  int icon_index, old_type, old_xtype;
} Window;

typedef struct
{
        int             ob_next;        /* -> object's next sibling     */
        int             ob_head;        /* -> head of object's children */
        int             ob_tail;        /* -> tail of object's children */
        unsigned char   ob_typex;
        unsigned char   ob_type;        /* object type: BOX, CHAR,...   */
        unsigned int    ob_flags;       /* object flags                 */
        unsigned char   ob_statex;
        unsigned char   ob_state;       /* state: SELECTED, OPEN, ...   */
        OBSPEC          ob_spec;        /* "out": -> anything else      */
        int             ob_x;           /* upper left corner of object  */
        int             ob_y;           /* upper left corner of object  */
        int             ob_width;       /* object width                 */
        int             ob_height;      /* object height                */
} OBJECT2;

typedef struct
{
  int state, magic, effects;
  union
  {
    unsigned long l;
    bfobspec b;
  } color;
  unsigned int atari_col;
  char atari3d, atari_move;
} OBDESC;

typedef struct
{
  MENU menu;
  OBJECT2 *parent;
} INT_MENU;

typedef struct mdesc
{
  struct mdesc *next, *prev;
  void *addr;
  long len;
} MDESC;

typedef struct App
{
  struct App *next, *prev, *child, *parent, *old_update;
  int type, clicks, mask, state, m1flags, m1x, m1y, m1w, m1h,
      m2flags, m2x, m2y, m2w, m2h, low, high;
  int event, mouse_x, mouse_y, mouse_b, mouse_k, key, times;
  int id, parent_id, *buf;
  int ap_type, ap_msgs, mint_id, apread_cnt, apread_id;
  APPFLAGS flags;
  BASPAG *basepage;
  OBJECT *menu, *desk;
  MFORM *mouse, *save_mouse;
  int mouse_on;
  int desk_obj;
  union
  {
    unsigned char c[2];
    unsigned int i;
  } update;
  long stack, mem_used;
  MDESC *mem_first;
  char dflt_acc[11], no_evnt, has_wind, start_end, asleep, waiting,
       path[PATHLEN], tail[TAILLEN], *env, *app_desc;
  struct App *ap_srch;
  int old_handles[7];
  int attaches;
  INT_MENU *menu_att;
  int dummy[STKSIZE/2];         /* should be near the end */
  int stktop;
} _APP;

#define MAX_BUTQ 8
typedef struct
{
  int x, y, b, k;
  unsigned long time;
} BUTQ;

typedef struct Msg_q
{
  struct Msg_q *next;
  int app, len;
  void *buf;
  unsigned long tic;
} MSGQ;

typedef union
{
  long l;
  int i[2];
} PULL;

typedef struct Acc_list
{
  struct Acc_list *next, *next_sort;
  char *name;
  int apid, index;
  char is_acc, is_dflt;
} ACC_LIST;

typedef struct _appflags
{
  struct _appflags *next;
  APPFLAGS *flags;
} _APPFLAGS;

typedef union
{
  char c[2];
  unsigned int i;
} UPDATE;

typedef struct
{
  int num;
  char *str;
} ERRSTRUCT;

/* contained in WINS.S */
extern char in_t1, in_t2;
extern int emult_out[7];
extern BASPAG **bp_addr;
extern void (*old_kbd)(), (*old_200)();
extern void (*old_t13)(), (*old_t1)(), (*old_t2)(), (*old_crit)(), (*old_term)();
extern void my_t1(), my_t13(), my_term();
extern int my_butv(), my_motv(), my_timv(),
    (*old_butv)(), (*old_motv)(), (*old_timv)();
extern void new_exec();
extern long old_exec;
extern BASPAG *db_bp;
extern int *old_stack;
extern void (*old_t13_term)();
extern void term_t13();
extern long set_t2(), get_t2();
extern void my_kbd(), my_200();
extern int my_motv( int x, int y );
extern int (*t2atbl)(), (*t2xtbl)();
void start_magic( BASPAG *bp );
void acc_start(), magic_init(), pdisp_init();

#ifndef PRINT_ALLOCS
#define lalloc(a,b) _lalloc(a,b)
#define lfree(a) _lfree(a)
#endif
#define lfreeall(a) _lfreeall(a)
#define _p_lalloc(a,b,c,d) __p_lalloc(a,b,c,d)
#define _p_lfree(a,b,c) __p_lfree(a,b,c)
#define APP _APP

#ifdef _APPLIC
  #define EXTERN
#else
  #define EXTERN extern
  extern OBJECT rs_object[], ascii_tbl[];
  extern TEDINFO rs_tedinfo[];
  extern unsigned char alt_obj[], alt[], alt_off[];
  extern int ind_move, ind_change, act_move, act_change;	/* char in G */
  extern int ind_col, act_col, bkgrnd_col, add3d_h, add3d_v;
  extern XFONTINFO fontinfo[9];
#endif

#ifdef DEBUG
  extern char fvalid[];
#else
  EXTERN char fvalid[]
  #ifdef _APPLIC
      = "_!@#$%^&()+-=~`;\'\",<>|[]{}";
  #endif
  ;
#endif
EXTERN NSETTINGS *settings;
EXTERN void (*clip_ini)( Rect *r );
EXTERN int (*clip_it)(void);
EXTERN APP *app0, *curapp, *has_menu, *has_desk, *has_mouse,
    *has_update, *menu_owner, *form_app, *txt_app;
EXTERN char *undo_ptr, *unkn_err;
EXTERN OBJECT *guimenu;
EXTERN Window *desktop, *next_menu, *top_wind, *ascii_w;
EXTERN Rect guimin;
EXTERN int next_obj, edit_idx, edit_obj;
EXTERN int fn_dir, fn_last;
EXTERN int dc_pause, dtree_cnt;
EXTERN int place;
EXTERN PULL pull_buf, pull_siz;
#if defined(DEBUG)
extern int vdi_hand, vplanes, v_bpp, cur_rez, new_rez, char_w, char_h;
extern unsigned long farbtbl2[256];
extern int farbtbl[256][32];
#else
EXTERN int vdi_hand, vplanes, v_bpp, cur_rez, new_rez, char_w, char_h;
EXTERN unsigned long farbtbl2[256];
EXTERN int farbtbl[256][32];
#endif
EXTERN int ptsiz, font_id, font_mode, cel_w, menu_h, cel_h, desk_obj, wcolor_mode;
EXTERN OBJECT *dflt_wind, *dflt_desk, *dflt_desk0;
EXTERN int ticcal;
EXTERN int kr, ks, br, mx, my, mb, m_obj;
EXTERN int oarray[14];
EXTERN int lastkey, lastsh;
EXTERN MFDB fdb0, fdb2;
EXTERN char no_top, draw_menu_alts, font_scalable;
EXTERN UPDATE update, cnt_update;
EXTERN int num_keys;
EXTERN char *environ;
EXTERN unsigned long idt_fmt;
EXTERN Rect clip_rect;
EXTERN int mouse_hide;
EXTERN ERRSTRUCT *dflt_errors;
EXTERN void (*drw_image)( int *data, int x, int y, int w, int h, int planes, int mode, int *cols, int x0, int y0 );
EXTERN int dc_rate;
EXTERN char dum_msg;
EXTERN IOREC **io;

#endif

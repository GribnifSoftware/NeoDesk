/* NeoDesk 3.02 by Dan Wilga
   Copyright � 1990, Gribnif Software.
   All Rights Reserved.
*/
/*  #define PRINT_ALLOCS*/

#ifdef DEBUG
  #define OFFSET
#endif
#define USE_NEO_ACC
#ifdef DEMO
  #undef USE_NEO_ACC
#endif
#include "multevnt.h"
#include "vdi.h"
#include "\source\neodesk\dither.h"
#include "xwind.h"

#ifndef Rect
  typedef struct { int x, y, w, h; } Rect;
  #define RECTDEF
#endif
#define Xrect(r) (r).x, (r).y, (r).w, (r).h

#define EXE1_MSGS 37
#define MAS0      15
#ifndef DEMO
  #define EXE_MSGS  182		/* 004: was off by 2 */
  #define IED_MSGS  43
#else DEMO
  #define EXE_MSGS  (182+3)         /* previous + 3 */
  #define IED_MSGS  (43+2)          /* previous + 2 */
#endif

#define WIND_TYPE   (NAME|MOVER|CLOSER|SIZER|UPARROW|DNARROW|VSLIDE|INFO|FULLER)
#define XWIND_TYPE  (X_MENU|X_VSPLIT)

#define ALLOC_MAS	-3
#define ALLOC_CLIP	-2

#define CJar_cookie	0x434A6172L	/* "CJar" */
#define CJar_xbios	0x434A		/* "CJ" */
#define	CJar_OK		0x6172		/* "ar" */
#define CJar( mode, cookie, value )	(int)xbios(CJar_xbios,mode,cookie,value)
#define getcookie(a,b) CJar(0,a,b)
#define OS_version  (*(int *)((*(long *)0x4F2)+2))

#define CLIP_LET    '0'
#define CLIP_HAND   0x7f19
#define CLIP_ERR    -200
#define CLIP_IGN    -201
#define LOCK_ERR    -202
#define S_IJRON         1
#define S_IJHID         2
#define S_IJSYS         4
#define S_IJVOL		8
#define S_IJDIR         16
#define S_IJWAC         32
#define CS_ODD      1
#define CS_FIRST    2
#define CS_LAST     4
#define CS_EMPTY    8
#define CS_CLIPOK   16
typedef struct
{
  char from[120];
  char to[120];
  int time[2];
  long len;
  char att, state, new_dir, noerr, del_dir;
  unsigned char dest_off;
} copy_struct;

#define MSELD   254
#define MSELW   253
#define MCMENU  0
#define MCAF    1
#define MOPDL   2
#define MOPDP   3
#define MOPWF   4
#define MOPRG   5
#define MOPT    6
#define MCOPF   7
#define MDELF   8
#define MRENF   9
#define MPRNF   10
#define MDELALL 11
#define MLIST   12
#define MNEWVOL 13
#define MNEWPTH 14
#define MWTOP   15
#define MWSPLIT 16
#define MWSALL  17
#define MWIOT   18
#define MWMOVE  19
#define MWSIZE  20
#define MWARROW 21
#define MWBACK  22
#define MWCLOSE 23
#define MWFULL  24
#define MOPDF   25
#define MDUPLIC 26
#define MACOPEN 27
#define MWMENU  28
#define MGMENU  29
#define MICONS  30
#define MOPNPI  31
#define MDELFNC 32	/* delete w/o confirm */

#define MACLOAD -1
#define MACINT  0
#define MACSTR  1
#define MACCHR  2
#define MACPTH  3
#define MACWIND 4
#define MACSELD 5
#define MACICON 6
#define MACCPYQ	7
#define chtoi(x)        ((*(x)<<8)|*(unsigned char *)((x)+1))

#define MACRO_SIZ   32766
#define OLD_NOTE_SIZ    1024
#define MAX_DIAL    PICOPTS	/* maximum held in NAC.dialogs-1 for ABOUT */

#define MAX_COLOR 16            /* number of control panel colors */
#define MAX_SAVECOL 32          /* number of screen saver colors */

#define PR_INIT 66
#define PR_LIST 73
#define PR_SCRN 74
#define PR_REQ  75

#define CNTRL_INIT  60
#define CNTRL_REQ   61
#define CNTRL_INIT4 460
#define CNTRL_REQ4  461

#define NEO_ACC_INI 80
#define NEO_ACC_BAD 81
#define NEO_CLI_RUN 82
#define NEO_ACC_EXC 83
#define NEO_ACC_QUI 84
#define NEO_ACC_TXT 85
#define NEO_ACC_INF 86
#define NEO_ACC_PAS 87
#define NEO_AC_OPEN 88
#define NEO_ACC_ASK 89
#define NEO_ACC_INF2 9089
#define NEO_INF2_RES 9090
#define NEO_ACC_COPY 9091
#define NEO_MAC_RESP 9092
#define NEO_MAC_PLAY 9093
#define NEO_MAC_ADD  9094
#define NEO_ACC_DEL  9095
#define CLI_DEMO_INI 0x2319

#define MAX_NEO_ACC 10

#define DUM_MSG 99
/* three unusual values you can get from list_files() */
#define PASSED_PRN       "PRN:"
#define PASSED_TRASH     "TRASH:"
#define PASSED_CLIP      "CLIP:"        /* may have path afterward */

#define DFLT_ERRORS 14

#define MANY_ACTIVE 32767

/* icon types */
#define FLOPPY  0
#define HARDDSK 1
#define RAMDISK 2       /* disks before here */
#define CLIPBRD 3
#define PRINTER 4
#define TRASH   5
#define FOLDER  6
#define PROG    7
#define TEXT    8
#define BATCH   9
#define GROUP	10
#define NPI	11
#define D_PROG  12      /* must be last */
#define NOT_DFLT 99	/* a big number */
/* application types */
#define NOAPP (PROG_TYPE)0
     #define PRG   1
     #define TOS   2
     #define TTP   3
     #define BTP   4

#define MEM_LIMIT	0
#define MEM_ALWAYS	1
#define MEM_NONE	2

typedef struct
{
  int sides, bps, tps, spt, clsiz;
  unsigned long secs;
} Bsec;

typedef struct
{
  union
  {
    struct
    {
      unsigned unused   :2;
      unsigned make_grp :1;	/* search only */
      unsigned templates:1;
      unsigned size     :1;
      unsigned date     :1;
      unsigned time     :1;
      unsigned attrib   :1;
      unsigned allfold  :1;
      unsigned use_long :1;
      unsigned use_temp :6;
    } s;
    unsigned int i;
  } flags;
  char use_size, use_date, use_time;
  int mask;
  unsigned int times[3];
  unsigned int dates[3];
  unsigned long sizes[3];
  char long_tmpl[3][29];
} FILT_TYPE;

typedef union
{
  unsigned int i;
  struct
  {
    unsigned pexec_mode:8;      /* also holds icon type in "file" list */
    unsigned set_me:1;
    unsigned batch:1;           /* reserved in NEO_ACC.H */
    unsigned return_status:1;
    unsigned show_status:1;
    unsigned clear_screen:1;
    unsigned npg:1;
    unsigned tos:1;
    unsigned takes_params:1;
  }p;
} PROG_TYPE;

typedef union
{
  unsigned int i;
  struct
  {
    unsigned docpath:1;
    unsigned reload:2;
    unsigned clock:1;
    unsigned MSTe_16M:1;
    unsigned MSTe_cache:1;
    unsigned TT_cache:1;
    unsigned Blitter:1;
    unsigned reserved:1;	/* must be 0 for set_caches to work */
    unsigned CaTT:3;
    unsigned singletask:1;
    unsigned unused:3;
  } p;
} APP_FLAGS;

typedef struct
{
  long magic;
  int ver;
  char path[120],
       name[21],
       params[5][39],
       use_argv;
  APP_FLAGS af;
  PROG_TYPE pt;
  char env[621], use_parent;
} NPI_TYPE;

typedef struct NpiDesc
{
  struct NpiDesc *next;
  NPI_TYPE npi;
  char path[120];
} NPI_DESC;

typedef union
{
  PROG_TYPE pt;
  unsigned int old_method;
} PRG_TYPE;

typedef union
{
  unsigned char c;
  struct
  {
    unsigned size:1;
    unsigned date:1;
    unsigned time:1;
    unsigned fold:1;
  }u;
} DPRN_TYPE;

typedef union
{
  char c[2];
  int i;
} STATUS_RETURN;

/* master program stuff */
#define MINIT  0
#define MEXEC  1
#define BEXEC  2
#define MMAGIC 0x4321

#define MASTER_ON      1
#define MASTER_HALFON  99
#define MASTER_HALFOFF -1

#ifndef DEMO
  #define NEO_VER '\xA'
#else
  #define NEO_VER '\x7C'	/* version in master */
#endif
#define INF_VER         "4.00"
#define INFCONV_VER	0x400	/* earliest INF_CONV that can be used */
#define INFDAT_VER      '\x3'
#define NAC_VER_205     0x0205
#define NAC_VER_300     0x0300
#define NAC_VER_302     0x0302
#define NAC_VER_303     0x0303
#define NAC_VER_304     0x0304
#define NAC_VER_402     0x0402
#define NIC_VER		0x0300	/* version in NIC file (not copyright) */

#define NAC_VER         NAC_VER_402
#define MACRO_VER       0x0400
#define NOTE_VER        0x0400
#define FILT_VER        0x0400
#define FILT_MAGIC	0x2E4E464CL	/* ".NFL" */
#define NPI_VER         0x0400
#define NPI_MAGIC	0x2E4E5049L	/* ".NPI" */

#define LOAD_COOKIE 0x4e4c6472L          /* "NLdr" */
#define LOADER_VER  0x0406

typedef union
{
  unsigned char c;
  struct
  {
    unsigned unused:3;
    unsigned unkn:1;
    unsigned tos:1;
    unsigned bat:1;
    unsigned parm:1;
    unsigned npg:1;
  } s;
} EXTN_TYPE;

typedef struct
{
  EXTN_TYPE type;
  char extns[5];
} EXTENSION;

typedef struct Grp_item GROUP_ITEM;
#define GROUP_VER	0x47525001	/* GRP1 */
typedef struct Group_icon GROUP_ICON;

typedef struct
{
  ICONBLK *ib;
  CICON *ci;
  CICON *list[3];
} NICONBLK;

#define ICON_WW 2
#define ICON_W  (ICON_WW<<4)
#define ICON_H  32
#define DATASIZ (ICON_H*ICON_WW*2)
#define MASKSIZ DATASIZ
typedef struct
{
  NICONBLK nicb;	/* must be at start for delete_icon & swap_icons to work */
  ICONBLK icb;
  int type;
  char text[13];
} ICONBUF;

typedef union
{
  unsigned char i;
  struct
  {
    unsigned folder:1;
    unsigned file:1;
    unsigned drive:1;	/* 003 */
    signed dflt:5;
  } type;
} ICON_TYPE;

typedef struct
{
  long create, modify;
  char auth[26], comment[3][36];
} NIC_INFO;

typedef struct
{
  char name[13];
  char read_only;
  PROG_TYPE type;
  int state;
  int x, y[2];
  unsigned long date, size;
  NICONBLK *nib;
  char groupname[22];
  GROUP_ITEM *grp_item;
} FSTRUCT;

typedef struct
{
  char path[120];
  PROG_TYPE type;
  NICONBLK *nib;
} PSTRUCT;

typedef struct
{
  PSTRUCT p;
  char is_acc;
} PROG_STRUCT;

typedef struct
{
  int prn_set, ffd;
} PRN_PARAM;

typedef struct
{
  int num;
  char *str;
} ERRSTRUC;

typedef struct
{
  char label[13], c;
  int type, state;
  NICONBLK *nicb;
} ICONSAVE;

struct Grp_item
{
  struct Grp_item *next;
  PSTRUCT p;
  int x, y;
  char name[22];
  char desc[2][21];
/*  struct Group_icon *icon;*/
  NICONBLK *nicon;
  KEYCODE key;	/* 003 */
};

typedef union
{
  unsigned char fix[2];
  unsigned int coord;
} COORD;

typedef union
{
  unsigned int i;
  struct
  {
    unsigned showicon:1;
    unsigned largetext:1;
    unsigned showtype:1;
    unsigned showpath:1;
  } s;
} GH_OPTS;

typedef struct Group_hdr
{
  long ver;
  unsigned int hdr_size, ent_size, entries;
  unsigned long create, modify;
  char name[13],
       desc[2][21],
       sort, snapx, snapy;
  COORD x, y, w, h;
  int split, offset[2];
  GH_OPTS opts;
} GROUP_HDR;

struct Group_icon
{
  int *ib_pmask;
  int *ib_pdata;
  unsigned int ib_char;
  int ib_xchar, ib_ychar;
  int ib_wicon, ib_hicon;
  CICON *cicon;
};

typedef struct
{
  GROUP_HDR hdr;
  char path[120];
  long filelen;
  int chksum;
} GROUP_DESC;

typedef union
{
  struct
  {
    unsigned unused	:4;
    unsigned textcol    :4;
    unsigned textmode   :1;
    unsigned fillpattern:3;
    unsigned interiorcol:4;
  } s;
  unsigned int i;
} WIND_PRF;

typedef struct
{
  int id, size;
  int w, h, scale;
} WIND_FONT;

#define FONT_ICON  -1
#define FONT_SMALL -2
#define FONT_LARGE -3
#define FONT_WIND  -4
#define FONT_NOTE  -5

typedef struct
{
  int *w_num, *w_handle;
  unsigned int *witems;
  void (*select_w)( int icon, int state, int wind, int change );
  void (*de_act)( int except, int wind );
  void (*info)(void);
  int (*match)( char *str, char *pat );
  void (*redraw_arrows)(void);
  void (*get_top_wind)(void);
  FSTRUCT **wfile;
#ifdef TEMP_DEBUG
  void (*reload)(char *name);/* Only if nac_ver >= NAC_VER_400 !! */
#endif
} ICHOOSE;

typedef struct
{
  int ver;
  int (*is_ok)( int index );	/* set by converter */
  int (*inf_load)( int err, int file, int set_me );
  int (*scan_inf_line)( char *buf, int i );
  int (*wrap_load)( int after, int is_ext );
} INF_CONV;

typedef struct Gui GUI;

typedef struct
{
  void (*colbits)( int bit );
  void (*pats)( int *patptr, int patmsk );
  void (*x1y1arr)( int *arr );
  void (*blit)( Rect *box1, Rect *box2, int mode, int tr, long ptr );
  void (*blitit)( Rect *box1, Rect *box2, int tr );
  int (*set_wmode)( int mode );
  int (*wmode0)(void);
  int (*wmode1)(void);
  int (*wmode2)(void);
  void (*draw_bx)( int *box );
  void (*set_lnmask)( int mask );
  int (*get_lnmask)(void);
  void (*desk_color)( int col );
  void (*blit_init)( MFDB *fdb );
  void (*x1y1val)( int x1, int y1, int x2, int y2 );
  void (*set_pattern)( int *box );
  void (*set_intin12)( int i1, int i2 );
  void (*small_char)( int x, int y, int fg, unsigned int c );
  void (*form_copy)( int flag, char *buf, Rect *r );
  void (*init_scrptr)(void);
  void (*gtext)( int x, int y, unsigned char *str, int fnum, int center, int mode, int color );
  void (*hide_mouse)(void);
  void (*show_mouse)(void);
  void (*gr_box)(void);
  void (*gr_hline)(void);
  void (*gr_linebox)( int *arr );
  void (*put_pixel)( int x, int y, int col );
  void (*set_butv)( int flag );
  void (*wait_mbut)(void);
  int (*get_mbut)(void);
  void (*set_clip)( int *arr, int mode );
  void (*graph_exit)(void);
  char *font;
  int vplanes, v_x_max, v_y_max, v_cel_mx, v_cel_my, cel_ht;
  long *screen_ptr;
  int *work_out;
  int handle;
  void (*reset_mouse)( int grf_mouse( int num, MFORM *addr ) );
  int (*reinit)(void);
  int has_clut;
  int total_fonts;
  char has_gdos, speedo, scalable, is_scalable;
  void (*load_fonts)( WIND_FONT *fonts, int size );
  void (*unload_fonts)(void);
  int (*vst_point)( int size, int *out1, int *out2, int *out3, int *out4 );
  int (*vst_font)( int id, int scale );
  void (*vst_color)( int id );
  char (*check_mono)( char scale );
  int (*vqt_extent)( unsigned char *str, int fnum );
  void (*vst_alignment)( int hor, int vert );
  void (*vst_effects)( int eff );
  void (*vsl_type)( int type );
  void (*vsf_style)( int style );
  void (*vsf_interior)( int style );
  int *text_arr;
  void (*text_2_arr)( unsigned char *str, int *num );
  void (*vsl_color)( int color );
  void (*vsl_ends)( int start, int end );
  void (*vsl_width)( int width );
  char have_butv, NVDI3;
  void (*ftext16_mono)( int x, int y, int len );
  void (*vst_charmap)( int mode );
} GRAPHICS;

typedef struct
{
  int mem_mode;
  long limit_to, take_always;
} M_LIMIT;

typedef struct
{
  char name[13], path[120], extn[21];
  PROG_TYPE type;
  APP_FLAGS flags;
} APP;

typedef struct Helpcmd HELPCMD;

typedef struct
{
  int font, point;
  int rows, cols;
  int xoff, yoff;
  char topic[21], match, all;
  char hlp_path[120];
  int wind;	/* >0 if in use */
  void (*fulled)( int *buf );
  void (*moved)( int *buf );
  void (*use_menu)( int num, int title );
  void (*new_topic)( HELPCMD *h );
} HELP_SET;

typedef struct
{
  char fit, mode;
} PICOPT;

typedef struct Avinf
{
  struct Avinf *next;
  char name[10], data[257];
} AV_INF;

typedef struct
{
  int version;
  int struct_len;
  int unused[2];  /*boot_rez, falcon_rez;*/
  union
  {
    struct
    {
      unsigned pulldown          :1;
      unsigned insert_mode       :1;
      unsigned long_titles       :1;
      unsigned alerts_under_mouse:1;	/* not actually used (yet) */
      unsigned no_alt_modal_equiv:1;	/* rel 003 */
      unsigned no_alt_modeless_eq:1;	/* rel 003 */
      unsigned tear_aways_topped :1;
      unsigned unused3           :1;
      unsigned unused4           :1;
      unsigned use_wcolors_cpx   :1;
      unsigned reserved          :6;
    } s;
    unsigned int i;
  } flags;
  int gadget_pause;             /* 50 Hz timer tics */
  KEYCODE menu_start, app_switch, app_sleep, ascii_table, redraw_all,
      wind_keys[13];
  OB_PREFER color_3D, color_root, color_exit, color_other;
  KEYCODE cycle_in_app, iconify, alliconify;		/* rel 003 */
  char graymenu, wcolor_mode;				/* rel 003 */
  int wstates[3][WGSIZE+1];
  int dwcolors[3][2][WGSIZE+1];
} NSETTINGS;
#define NSET_VER NAC_VER

typedef struct NeoAcc
{
  int cliparray[4],  /* can't add to this list w/o messing up env for CLI */
      unused_pallette[16],	/****************/
      conf_copy,
      conf_del,
      conf_over,	/* used by CLI */
      tos_pause,	/* used by CLI */
      use_master,	/* used by CLI */
      sort_type[7],
      status_on,	/* not used; leave here anyway for NAC */
      ctrlZ,
      status_report,
      quit_alert,
      showicon[7], stlgsml[7], stcolumn[7], sizdattim[7][3],
      format_drive,
      tracks, sides, spt, twst;
  unsigned long pic_ptr;
  char unused_rsc_name[13],	/**************/
       dflt_path[120],
       unused_neo_path[105],	/**************/
       num_sep,
       open_fold,
       has_magx,
       real_time,
       is_acc,
       copy_bak, del_bak, dskcpy_bak,
       format_bak,
       rename_dest,
       dial_in_wind,
       filt_copy, filt_del,
       count_copy, count_del,
       batch_name[33],
       autoexec[33],
       saveconf,
       macr_rec,
       macr_play,
       drezname[9],
       diskcopy,
       tail[130],	/* used by CLI */
       env[620],	/* used by CLI */
       text_reader[33],	/* used by CLI */
       template[6][13],
       use_argv,
       env_parent,
       in_copy,
       unused_use_temp[4],  /********/
       unused_extns[10][5], /********/
       rezname[10][9],
       new_inf_name[3][9],
       *inf_name;
  int macstrt, macptr;
  int cdecl (*macro_func)( int type, void *parms );
  Rect maximum, max_area;
  int rezes[10][2],
      move_mode,
      snapx, snapy,
      idt_fmt,
      dial_mode,
      back_speed,
      pic_colormode;
  GEMPARBLK *gemparblk;
  WIND_PRF wind_prf;
  WIND_FONT wind_font[6];
  GUI *gui;
  INF_CONV *inf_conv;
  char snap_resort, snap_over;
  long limit_to, take_always;
  int mem_mode,
      new_mem_mode;
  char *env_ptr,
       *long_tail,
       *free_env;
  int rez_num;
  M_LIMIT mem_limit[11];
  char **msg_ptr;
  Rect wind_pos;
  char unused_old_ttp[2];	/******/
  char note_col, note_opaq;
  struct Wstruct
  {
    int place, split;
    int x, y, w, h;
    union { int i[2]; long l; } f_off;
    char path[120];
  } w[7];
  FSTRUCT *file[7];
  int num_files[7], files_rem[7];
  char desk_pic[120],
       desk_in_wind,
       show_pic,
       wall_pic, unusedchar;	/*****/
  int mac_resp;
  char *old_macro;
  AV_INF *av_status;
  char unused_idat[400-4];	/*********/
  char unused_programs[3091];   /********/
  char ttp_params[5][39];	/* odd-sized array must follow unused_progs */
  PICOPT desk_picopts,
         view_picopts;
  long pallette[16];
  PROG_STRUCT *programs;
  int num_progs, prog_rem;
  OBJECT *desk;
  ICONSAVE *idat;
  int num_icons, num_desk, icons_rem, num_idat, idat_rem;
  APP *apps;
  int num_apps, apps_rem;
  EXTENSION *extension;
  int num_ext, ext_rem;
  int dialogs[60][2];	/* only the first 47 are used */
  HELP_SET help;
  char unused_apps[688];	/*********/
  DPRN_TYPE dir_prn;
  PROG_TYPE exec_type;
  int msort_type, mshowicon, mstlgsml, mstcolumn, msizdattim[3];
  char *notes;
  int notes_len, notes_rem;
  char *macro;
  int macro_len, macro_rem;
  int speed, menu_id;
  int spc[3];
  char unused_dialogs[84-6];	/*****************/
  int rezvdi;
  union
  {
    struct
    {
      unsigned virus_check :1;
      unsigned check_fnames:1;
      unsigned dflt_twst   :2;
      unsigned clip_mode   :2;
      unsigned consumption :1;
      unsigned av_server   :1;
      unsigned use_kobold  :1;
      unsigned prevent_mult:1;		/* 004 */
      unsigned long_numbers:1;		/* 005 */
      unsigned unused      :5;
    } b;
    unsigned int i;
  } other_pref;
  union { int i[2]; long l; } o_f_off[7][7];
  int new_cache, disk_copies;
  char has_multigem, multitask;
  int cntrl_set[16];	/* two spare words at end */
  FILT_TYPE filter[7], search_filt, copydel_filt;
  IOREC *kbio;
  char unused_is_acc[28-4],	/*****************/
       neo_acc[MAX_NEO_ACC][9], cli,
       set_path, rez_let, space1;
  NSETTINGS gui_settings;
  char unused_mac_notes[1023+1024-sizeof(NSETTINGS)],	/*****************/
       old_drv,
       old_path[118], *stat_return;
  ICHOOSE *ichoose;
  long unused_call_neo;		/*****************/
  OBJECT **unused_desk;		/*****************/
  GRAPHICS *graphics;	/* used by CLI */
  /* add new stuff after here so CLI 2.1 won't get confused */
} MOST400;

#define MOST MOST400

typedef struct
{
  char ver, rez, path[120];
  MOST *most;
  ERRSTRUC *dflt_errors;
  int state;
  OBJECT *blank;
  long (*exe_load)( int mode, char *name, void *a0, int d0, char **start, int open );
  int (*redirect)( int *fh, int *sh );
  int (*execute)( int s_h, int f_h, int o_h );	/* NEO_CLI depends on this */
  void cdecl (*clear_mouse)(void);
  int cdecl (*wait_key)(void);
  int cdecl (*dsetpath)( char *path );
  void cdecl (*dopf)( char *a, char *b, ... );
  int cdecl (*get_line)( int file, char *diskbuff, int ignore, int new );
  int cdecl (*read_messages)( char *name, int end_num, char *start_mem, 
                              char *end_mem, char *ptrs[], char **new_msgs );
  long *open_wind;
  int *bad_media;
  /* add new items after here! */
  int cdecl (*set_caches)( int bits );
  OBJECT **about;
  void *(*lalloc)( long size, int id, int alert );
  int (*lfree)( void *xfb );
  int (*lfreeall)( int id );
  void (*memstat)( long *in_use, long *allocated );
  int (*lshrink)( void *block, long newsiz );
  int (*lrealloc)( void **xfb, long size, int alert );
  char *parent_env;
  void (*close_exe)(void);
  int singletask;	/* 006: must keep these two in order */
  long singleregs[11];
} MASTER;

#define NEO_ACC_MAGIC  0x4321

typedef struct
{
  int nac_ver;
  MASTER *mas;
  int cdecl (*abort)();
  void cdecl (*bconws)();
  void cdecl (*blank_box)( int *arr );           /* set WMODE */
  void cdecl (*blit)(Rect *box1,Rect *box2,int mode,int tr,long ptr);
  int cdecl (*check_dir)();
  int cdecl (*check_prn)();
  int cdecl (*close_all)(void);
  void cdecl (*close_form)( Rect r, int f );
  void cdecl (*copy_init)(void);
  void cdecl (*copy_free)(void);
  char **c_buf, **c_curbuf;
  long *c_buflen, *c_bufmax;
  int cdecl (*copy_files)();
  int cdecl (*copy_a_buffer)();
  int cdecl (*copy_a_file)();
  int *moving;
  int *status_on;
  void cdecl (*draw_box)( int *arr );
  void cdecl (*draw_icon)();
  void cdecl (*gtext)( int x, int y, unsigned char *str, int fnum, int center );
  int cdecl (*list_files)( char **ptr );
  void cdecl (*bytecpy)( void *src, void *dest, int size );
  void cdecl (*byteswap)( void *src, void *dest, int size );
  int cdecl (*print_file)();
  void cdecl (*redraw_desk)( int x, int y, int w, int h, int num );
  char *font;                   /* 3.00 or newer */
  int cdecl (*rubber_box)( int x0, int y0, Rect *box, int flag );
  void cdecl (*set_clip)( int *i, int f );
  void cdecl (*set_clip_rect)( Rect *r, int f );
  void cdecl (*update_drive)( char *path );
  int cdecl (*TOS_error)( long num, int errs, ... );
  int cdecl (*trash_files)( char *path, int *file, int *fold );
  void cdecl (*warmboot)();
  void cdecl (*coldboot)();
  int cdecl (*sscnf)( char *buf, char *fmt, ... );
  long *gt_scr_ptr;             /* Only if nac_ver >= NAC_VER_300 !! */
  void cdecl (*save_desktop)(void); /* Only if nac_ver >= NAC_VER_300 !! */
  void cdecl (*reset_desktop)(void);/* Only if nac_ver >= NAC_VER_300 !! */
  long (*setcolor)( int num, long val );/* Only if nac_ver >= NAC_VER_303 !! */
  void (*draw_image)( int *data, int x, int y, int w, int h, int planes, int mode, int *cols, int x0, int y0 );
  int (*test_rez)(void);
  void (*set_temps)( int dplanes );
  void (*xfix_cicon)(unsigned int *col_data, long len, int old_planes, int new_planes, MFDB *s, int devspef);
  long (*drvmap)(void);
} NEO_ACC;

extern int cdecl call_w_save( int (*func)(), ... );

typedef struct
{
  int ver;
  void cdecl (*dopf)( char *a, char *b, ... );
  void cdecl (*bconws)( char *string );
  int *bad_media;
  
  char *top;
  int *outhand;
  char ***bufptr;         /* set the POINTER */
  int *count;		/* for backward compatibility */
  
  int *savcnt;
  char *scrsav, saveon, ampm, clockon, savmins, **kbshift;
  long (*pall)[];           /* use the POINTER */
  char *clock_cnt;
  
  long *open_wind;      /* set the POINTER to a function */
  
  char *t2table;
  MASTER *mas;
  char *do_malloc;
  long *baspag, **pgmsize;
  char *inittab;
  
  void (**saver)( int onoff );  /* 1: on   0: off   other: interrupt */
  void (*shrink)( BASPAG *bp, long stack );

  int (**fselect)( int apid, char *path, char *file, int *button, char *title );
  
  void (**vclock)(void);
  void (*clock_on)( void vclock(void), long *ptr );
  void (*clock_off)(void);
  
  int *mdwa;
  char *ldpath,
      *clock_temp;	/* 0 if clock is temporarily off */
  int (*w_colors)[][2];
  char (*lock_drive)[4];

  long *pr_bufsiz, *pr_count;
  char **pr_bufmax, **pr_bufstart, **pr_buftail, *no_bcon;
  int pr_valid;
  char *num_sep;	/* 003 */
  int *avserver;	/* 003 */
  char *long_numbers, *has_cyrel;	/* 005 */
} LoadCookie;
#define PR_VALID	0x1223

typedef struct BufHdr BUF_HDR;
struct BufHdr
{
  long bufsiz;
  BUF_HDR *nextbuf;
  BUF_HDR *lastbuf;
};

typedef struct
{
  MOST **most;
  NEO_ACC *nac;
  OBJECT **menu, **icons, *(*wmenu)[7];
  ICONBUF **nic_icons;
  NIC_INFO *nic_info;
  int *showicon;
  char *in_showinf, *showinf_ok, *reorder_on;
  int *show_ret;
  int *num_icons, *icons_rem;
  int *AES_handle;
  char **icon_text;
  unsigned char (*floydbytes)[65][8];
  long (*dflt_pall)[16];
  void (*to_filename)( char *src, char *dest );
  void (*from_filename)( char *src, char *dest, int flg );
  void (*reset_all_icons)( int flag );
  void (*get_all_icons)(void);
  int (*first)(int i);
  int (**match)( char *str, char *pat );
  void (*tandd_to_str)( int *time, char *str );
  void (*to_tandd)( unsigned long time, int *bytes );
  int (*set_filename)( int fl, int ind, char *ptr1, char *ptr2 );  /* fix me */
  int (*open_to_path)( char *path );
  OBJECT **deskpat;
  int (*fix_rez)( int *data, int **out, int pl, int dplanes, int (*rgb)[16][3], int devspef );
  int (*copy_cicon)( int pl, CICON *from, CICON *to );
  int (*alloc_im)( int **p, int pl );
  int (*add_desk)(void);
  void (*install_devices)( int iconedit );
  int (*dflt_icon)( char c, int type );
  int (*add_program)( char *name, PROG_TYPE *t );
  void (*free_desk)(void);
  void (*_graf_mouse)( int shape );
  int (*add_thing)( void **start, int *total, int *remain, void *add,
      int addinc, long size, int id );
  void (*unpak_rgb)( unsigned char *c, int *rgb );
  void (*free_pic)( PICTURE **pic, int do_pic );
  int (*fix_icon)( ICONBUF *ib );
  int (*load_img)( int h, PICTURE *pic );
  int (*load_bmp)( int h, int is_ico, PICTURE *pic );
  void (*bmp_data)( char *data, char *comp, int swidth, int height, int planes, int is_ico, int pos, int invert );
  int (*to_stand)( PICTURE *picture );
  void (*set_longedit)( OBJECT *o, int ind, int count );
  void (*do_desk)(void);
  void (*imenu_bar)( OBJECT *tree, int flag );
  void (*draw_icon)( NICONBLK *i_ptr, int x, int y, char *text, unsigned int c, int s, int w );
  int (*prep_save)( char *file );
  void (*obj_true1)( OBJECT *form, int truth, int start );
  void (*obj_true)( OBJECT *form, int truth, int start );
  int (*read_header)( int hand, unsigned int *entries, void *ni, unsigned int *codelen );
  int (*read_nic)( int hand, int some, NIC_INFO *nif, ICONBUF **start, int *num_icons, int *icons_rem );
  int (*read_dflt_nic)( int some );
  ICONBUF *(*add_icon)( ICONBUF **start, int *num_icons, int *icons_rem );
  void (*free_icon)( CICON **ci, CICON *curr, int root );
  void (*free_nib_icon)( NICONBLK *n );
  void (*free_iconbuf)( ICONBUF **i, int start, int *num, int *rem );
  void (*free_nic)( ICONBUF **i, int *num );
  void (*reset_icbs)( ICONBUF *icb, int i, int max );
  void (*update_othwind)( int num, int draw );
  void (*update_wnum)( int wnum, char *path );
  int (*save_img)( char *name, PICTURE *pic );
  void (*set_temps)( int dplanes );
  int (*transform_pic)( PICTURE *pic, int devspef );
  void (*free_desk_pic)(void);
  int (*close_ev_ic)(void);
  void (*fit_pulls)( OBJECT *menu, int pull );
  int (*fit_pic)( PICTURE *p, int inside, int aspect, int w, int h );
  void (*window_box)( int *arr );
} ICNEO;

typedef struct
{
  int (*use_menu)( int wind, int num );
  unsigned char *menu_xrefk, *menu_xrefm;
  int menu_items;
  int (*trans_wmenu)( int wnum, int num, int from_neo );
  void (*edit_desk)(void);
  int (*ed_wind_type)( int num );
  void (*offset_objc)( OBJECT *tree, int obj, int *x, int *y );
  int (*open_iwind)( int wind, int type );
  char *icon_str, *info_fmt, *info_fmt_sel1, *info_fmt_sel;
  int (*get_icon)( int wind, int ind, FSTRUCT *file );
  int (*edit_icon)( FSTRUCT *file, int wnum );
  void (*enable_menu)( int wnum );
  int (*delete_icon)( int wnum, int num );
  int (*close_icons)( int num );
  void (*clear_icons)( int t );
  void (*bad_op)(void);
  int (*copy_icon)( int wnum, int s_num, FSTRUCT *wfile, int witems, NICONBLK *ditem );
  int (*ic_reorder)( FSTRUCT *wfile, int witems, char *reorder_on, int wnum );
  int (*swap_icons)( FSTRUCT *fs1, FSTRUCT *fs2, int wnum, int first, int next );
  int (*nic_showinf)( int wnum, char *file );
  int (*del_conf)(void);
  int (*nt_nic)(int wnum);
  int (*trans_mmenu)( int num, int from_neo );
  OBJECT *(*icon_menu)( int wnum );
  int (*add_nic_patt)( int *data, int col );
  NICONBLK *icon_nib;
  BITBLK *pattern;
} ICIC;

long ic_main( ICNEO *ic_neo, int close );

/* returned by ed_wind_type() */
#define EDW_GROUP	-2
#define EDW_DISK        -1
#define EDW_ICONS       0
#define EDW_FILE        1

/* icon attributes */
#define ICON_PATT       0
#define ICON_FILE       1
#define ICON_FOLD       2
#define ICON_BOTH       3
#define ICON_DRIVE      4

#define ICON_LET        '1'

typedef struct
{
  int wnum;
  union
  {
    FSTRUCT *fs;
    int desk_item;
  } u;
} SEL_ICON;

typedef struct
{
  SEL_ICON *icons;
  int nicons, cicon;
} SELICON_DESC;

typedef struct
{
  unsigned wmenu	  :1;
  unsigned dmenu	  :1;
  unsigned group	  :1;
  unsigned locked	  :1;
  unsigned close_on_update:1;
  unsigned modal	  :1;
  unsigned copyable	  :1;
  unsigned force_modeless :1;
  unsigned multiple	  :1;
  unsigned ignore_events  :1;
  unsigned double_click   :1;
  unsigned no_blit        :1;
  unsigned no_center      :1;
  unsigned unused	  :1; /**/
  unsigned parent_closed  :1;
  unsigned opened	  :1;
} FORMFLAGS;

#define NO_POS	-999	/* -999 and lower: no dialog position in z->dialogs */

typedef struct Form_Type
{
  unsigned char menu_num;		/* menu string index */
  int treenum;				/* rsc file index */
  FORMFLAGS flags;
  long memory;				/* amount of memory */
  int (*init)( OBJECT *o, struct Form *f );             /* function to initialize it */
  int (*touch)( OBJECT *o, int num, struct Form *f );   /* called when TOUCHEXIT is clicked */
  int (*exit)( OBJECT *o, int num, struct Form *f );    /* called when EXIT is clicked */
  int (*update)( OBJECT *o, struct Form *f );           /* called during timer event */
  OBJECT *tree;                         /* dial's object tree */
} FORM_TYPE;

/* describe a modeless dialog */
typedef struct Form
{  /* first part of structure must match FORM_TYPE! */
  unsigned char menu_num;		/* menu string index */
  int treenum;				/* rsc file index */
  FORMFLAGS flags;
  long memory;				/* amount of memory */
  int (*init)( OBJECT *o, struct Form *f );             /* function to initialize it */
  int (*touch)( OBJECT *o, int num, struct Form *f );   /* called when TOUCHEXIT is clicked */
  int (*exit)( OBJECT *o, int num, struct Form *f );    /* called when EXIT is clicked */
  int (*update)( OBJECT *o, struct Form *f );           /* called during timer event */
  OBJECT *tree;                         /* dial's object tree */
  int handle;                           /* handle of window containing dial */
  void *mem_ptr;			/* pointer to memory */
  char win_path[50];			/* path of calling window */
  char *old_title;			/* old title */
  int wind;				/* number of associated window */
  void *copy;				/* points to memory of dialog copy */
  SELICON_DESC icons;  			/* list of selected icons */
  int apid;				/* apid of owner */
  FORM_TYPE *ft;
} FORM;

typedef struct
{
  int (*gui_init)( GEMPARBLK *blk );
  void (*gui_exit)( int hand, int apexit );
  void (*multi_evnt)( EMULTI *a, int *msgbuf );
  int (*evnt_timer)( int ev_tlocount, int ev_thicount );
  int (*rsrc_rcfix)( RSHDR *rsc );
  int (*objc_draw)( OBJECT *ob_drtree, int ob_drstart, int ob_drdepth, int ob_drxclip,
      int ob_dryclip, int ob_drwclip, int ob_drhclip );
  int (*objc_change)( OBJECT *ob_ctree, int ob_cobject, int ob_cresvd, int ob_cxclip,
      int ob_cyclip, int ob_cwclip, int ob_chclip, int ob_cnewstate, int ob_credraw );
  int (*objc_offset)( OBJECT *ob_oftree, int ob_ofobject, int *ob_ofxoff,
      int *ob_ofyoff );
  int (*form_dial)( int fo_diflag, int fo_dilittlx, int fo_dilittly, int fo_dilittlw,
      int fo_dilittlh, int fo_dibigx, int fo_dibigy, int fo_dibigw, int fo_dibigh );
  int (*form_center)( OBJECT *fo_ctree, int *fo_cx, int *fo_cy, int *fo_cw,
      int *fo_ch );
  int (*form_do)( OBJECT *fo_dotree, int fo_dostartob );
  int (*form_button)( OBJECT *fo_btree, int fo_bobject, int fo_bclicks,
      int *fo_bnxtobj );
  int (*x_form_center)( OBJECT *tree, int *cx, int *cy, int *cw, int *ch );
  int (*x_form_error)( char *fmt, int num );
  int (*x_wdial_draw)( int handle, int start, int depth );
  int (*x_wdial_change)( int handle, int object, int newstate );
  int (*menu_popup)( MENU *me_menu, int me_xpos, int me_ypos, MENU *me_mdata );
  int (*menu_text)( OBJECT *me_ttree, int me_titem, const char *me_ttext );
  int (*menu_icheck)( OBJECT *me_ctree, int me_citem, int me_ccheck );
  int (*menu_ienable)( OBJECT *me_etree, int me_eitem, int me_eenable );
  int (*menu_tnormal)( OBJECT *me_ntree, int me_ntitle, int me_nnormal );
  int (*graf_slidebox)( OBJECT *gr_slptree, int gr_slparent, int gr_slobject,
      int gr_slvh );
  int (*x_graf_rubberbox)( GRECT *area, GRECT *outer, int minwidth,
      int minheight, int maxwidth, int maxheight, int snap, int lag );
  int (*x_graf_blit)( GRECT *r1, GRECT *r2 );
  int (*x_wind_calc)( int type, int kind, int xkind, int inx, int iny,
      int inw, int inh, int *outx, int *outy, int *outw, int *outh );
  int (*x_wind_create)( int kind, int xkind, int wx, int wy, int ww,
      int wh );
  int (*wind_open)( int wi_ohandle, int wi_owx, int wi_owy, int wi_oww,
      int wi_owh );
  int (*wind_close)( int wi_clhandle );
  int (*wind_delete)( int wi_dhandle );
  int (*wind_get)( int wi_ghandle, int wi_gfield, ... );
  int (*wind_set)( int wi_shandle, int wi_sfield, ... );
  int (*wind_update)( int wi_ubegend );
  void (*wind_new)(void);
  int (*x_wind_tree)( int mode, WIND_TREE *wt );
  void (*set_dclick)( int rate );
  void (*gad_redraw)( int w_handle, Rect *r, int obj );
  int (*menu_bar)( OBJECT *tree, int show );
  void (*arrow_dial)( int *msg );
  int (*form_keybd)( OBJECT *tree, int edit_obj, int obj, int kr, int *out_obj, int *okr );
  int (*objc_find)( OBJECT *tree, int this, int depth, int mx, int my );
  int (*x_settings)( int getset, int length, NSETTINGS *user );
  char *dum_msg;
  int (*x_scrp_get)( char *out, int delete );
} EXEGUI;

struct Gui
{
  int (*start_form)( int apid, long type, FORM_TYPE *ft );
  void (*use_form)( int handle, int obj );
  void (*form_draw)( FORM *f, int num, int edit );
  void (*close_all_fwind)( int apid, int ac_close );
  void (*prev_blit)( FORM *f, int unblit );
  int (*Nrsc_load)( char *name, DTA *dta, RSHDR **rsc );
  int (*Nrsc_rcfix)( RSHDR *rsc );
  int (*Nrsc_gaddr)( int re_gtype, int re_gindex, void *gaddr, RSHDR *rsc );
  void (*Nrsc_free)( RSHDR *rsc );
  EXEGUI xtern;
};

enum
{
  ICON_FORM,
  FILT_FORM,
  WSEA_FORM,
  DSEA_FORM,
  GROU_FORM,
  ABOU_FORM,
  GITM_FORM,
  DSNA_FORM,
  WSNA_FORM,
  SAVE_FORM,
  TTP_FORM,
  FOLD_FORM,
  IDRV_FORM,
  IFIL_FORM,
  IFOL_FORM,
  IGRP_FORM,
  MGRP_FORM,
  DAPP_FORM,
  WAPP_FORM,
  DNPI_FORM,
  WNPI_FORM,
  FMT_FORM,
  OPST_FORM,
  ENV_FORM,
  NENV_FORM,
  ENVE_FORM,
  RENF_FORM,
  PDIR_FORM,
  CPAL_FORM,
  FCON_FORM,
  FILE_FORM,
  FOP_FILT,
  REZ_FORM,
  OCON_FORM,
  MACR_FORM,
  NEWN_FORM,
  QFYI_FORM,
/*  HFYI_FORM,*/
  MORE_SUB,
  FILE_SUB,
  ACC_SUB,
  INF_SUB,
  EXT_SUB,
  PTH_SUB,
  MEM_SUB,
  DIAL_SUB,
  DESK_SUB,
  WIND_SUB,
  NOTE_SUB,
  PIC_FORM,
  LAST_FORM
};

typedef struct
{
  char type[8],
       *fmt;
  void **list;
  int msg;
} INF_OFF;

typedef struct
{
  char type[8],
       *fmt;
  void **start;
  int (*write)( int num, void **point, void **out );
  int (*read)( char *buf, void **point, char *fmt );
  int msg;
} FUNC_INF_OFF;

typedef struct
{
  int filt_sub, filt_hand, filt_wnum, filt_idt;
  FILT_TYPE *cur_filt, *filter;
} FILT_DESC;

typedef struct
{
  char copy, sides, back, cside, dside, sdrv, ddrv, twst, err, paused,
      src_ok, rwf;
  int form_num, scnum, spt, bps;
  int tracks, ctrack, dtrack;
  int slice, altslice, altcnt, blksiz, times, times0;
  char *ptr, *ptr0, *fmt;
  int (*func)( FORM *frm, OBJECT *o, int draw );
  long memory;
  void *mem_ptr;
  char first;
  int spc[3], dens;
} DISKOP;

typedef struct
{
  int count;
  SEL_ICON *icon;
  char update;
} RENAME;

typedef struct
{
  char *parent, *env;
  char temp_parent;
} ENV_TYPE;

typedef struct
{
  int tree_lev, folders, files, f_hidden, tree_stat;
  long bytes_total, bytes_hidden, tree_fsize, tree_date;
  char tree_path[120], tree_fname[13], tree_valid, tree_att;
  DTA tree[9], tree_curr;
  FILT_TYPE *filt;
  char *filt_templ;
} TREE;

enum
{
  INF_AUTOEXC,
  INF_QUEUE,
  INF_CONTROL,
  INF_PALETTE,
  INF_SNAP,
  INF_PRNTDIR,
  INF_COPYMOV,
  INF_MISCPRF,
  INF_FORMAT,
  INF_BATCH,
  INF_TXTREAD,
  INF_WINDOW1,
  INF_WINDOW2,
  INF_WINDOW3,
  INF_WINDOW4,
  INF_WINDOW5,
  INF_WINDOW6,
  INF_WINDOW7,
  INF_FILTER1,
  INF_FILTER2,
  INF_FILTER3,
  INF_FILTER4,
  INF_FILTER5,
  INF_FILTER6,
  INF_FILTER7,
  INF_CPDLFLT,
  INF_SRCHFLT,
  INF_SRCHTMP,
  INF_TTP1,
  INF_TTP2,
  INF_TTP3,
  INF_TTP4,
  INF_TTP5,
  INF_ENVOPTS,
  INF_DIALPRF,
  INF_DESKPRF,
  INF_WINDPRF,
  INF_GUI,
  INF_DESKICN=100,
  INF_APPLIC,
  INF_EXTENSN,
  INF_ENVIRON,
  INF_DIALOG,
  INF_AV		/* 003 */
};

#ifdef PRINT_ALLOCS
  void prnall( char *str, ... );
  void *_p_lalloc( long size, int id, char *file, int line );
  #define lalloc(a,b)  _p_lalloc(a,b,__FILE__,__LINE__)
  void *_p_lalloc2( long size, int id, char *file, int line );
  #define lalloc2(a,b) _p_lalloc2(a,b,__FILE__,__LINE__)
  int _p_lrealloc( void **xfb, long size, char *file, int line );
  #define lrealloc(a,b) _p_lrealloc(a,b,__FILE__,__LINE__)
  int _p_lfree( void *xfb, char *file, int line );
  #define lfree(a) _p_lfree(a,__FILE__,__LINE__)
  int _p_lshrink( void *block, long newsiz, char *file, int line );
  #define lshrink(a,b) _p_lshrink(a,b,__FILE__,__LINE__)
  void _p_cmfree( char **ptr, char *file, int line );
  #define cmfree(a) _p_cmfree(a,__FILE__,__LINE__)
#endif

struct Helpcmd
{
  NEO_ACC *nac;
  int modal;
  char *topic, *file, has_Geneva;
  int caps, aes_ver, AES_handle;
  void (*signal_exit)(void);
};

enum
{
  mABOUT, mOPEN, mSHOWINF, mQUICKINF, mSEARCHDR, mDELITEM, mFORMAT, mPRINTDSK,
  mFORMFEED, mQUIT, mMPOPMEM, mMPOPFILE, mMPOPDESK, mMPOPWIND, mMPOPDIAL, mMPOPPATH,
  mMPOPEXT, mMPOPINF, mMPOPACC, mEDITENV, mMPOPMISC, mMPOPRELD, mINSTICON,
  mINSTAPP, mPROGINFO, mBEMACRO, mEXCHMAC, mSAVEDESK, mLOADINF, mEDITICON,
  mSNAPGRID, mCHANGREZ,
  mWIMOPEN, mWIMSHOW, mWIMQUICK, mWIMSRCH, mWIMDEL, mCREATE, mCLOSEFLD, mCLOSEWIN,
  mSELALL, mWIMGRP, mSHOWICON, mSHOWTEXT, mSTLGSML, mSTCOLUMN, mSTSIZE, mSTDATE,
  mSTTIME, mSORTFILT, mPRINTDIR, mUPDATE, mFUPDATE, mSORTNAME, mSORTDATE, mSORTSIZE,
  mSORTTYPE, mSORTNONE, mREORDER, mWIMAPP, mWIMNPI, mWIMLOAD,
  mGWIMOPEN, mGWIMSHOW, mGWIMCLOS, mGWIMSEL, mGWIMDEL, mGWIMSAVE, mGWIMICON, mGWIMTEXT,
  mGWIMLGSM, mGWIMNAME, mGWIMTYPE, mGWIMSTYP, mGWIMSPTH, mGWIMUPDT, mGWIMAPP,
  mGWIMSNAP, mGWIMCHNG, mGWIMLOAD
};

typedef struct
{
  int (*start_dial)( int num );
  int AES_handle;  /* set by Neo */
  NEO_ACC *nac;
  int first_form;
  OBJECT *fills, *colors;
  void (*signal_xset)(void);
  int (*popup_fill)( OBJECT *o, int parent, int val );
  int (*popup_col)( OBJECT *o, int parent, int val );
  void (*get_wcolors)(void);
  void (*new_colors)(void);
  int modal_ret;
} SET_STRUCT;

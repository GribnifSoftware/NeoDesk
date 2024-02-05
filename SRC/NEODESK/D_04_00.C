/* NeoDesk 3.03 by Dan Wilga
   Copyright 1992, Gribnif Software.
   All Rights Reserved.
*/

#include "neodesk.h"   /* moving folder same device tries to delete if Use Ex*/
#include "new_aes.h"
#include "xwind.h"
#include "string.h"
#include "ctype.h"
#include "tos.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "lerrno.h"
#include "ierrno.h"
#include "vdi.h"
#include "neocommn.h"
#include "neod2_id.h"
#include "iconedit.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */
#include "xacc.h"
extern GUI *gui;
/* pnmatch for folders */
/*?copy same file twice from clip, 2nd copy has bad time/date */

#define _VDO_COOKIE 0x5F56444FL
#define _FDC_COOKIE 0x5F464443L
#define SBL_COOKIE  0x4F53424CL
#define CYREL_COOKIE  0x434D3136L

#define MENU_ITEMS  (39-1)		 /* one less than the real number */
#define NUM_DIMS    30
#define MAGIC	    0x87654321L
#define conterm     (*((char *) 0x484))
#define OS_memvalid (*((long *) 0x420))
#define OS_memval2  (*((long *) 0x43A))
#define OS_resvalid (*((long *) 0x426))
#define OS_restart  (*((void (*)(void)) *(long *) (*(long *)0x4f2+4) ))
#define _bootdev    (*(int *)0x446)
#define _p_cookies  (*(long **)0x5a0)
#define RING_BELL   Crawio(7)
#define TRANSTXT    73
#define TRANSCOL    74
#define TRANSMEM1   79
#define TRANSMEM2   119
#define TRANSMEM3   120
#define TRANSROM    81
#define TRANFILE    75
#define TRANFOP     76
#define TRANSTAT    77
#define TRANSTA2    78
#define TRANSALL    80
#define TRANSMAC    98
#define TRANSTXT2   113
#define TRANSCOL2   114
#define TRANSMAC2   115
#define W_ADD  (WGUP2-WGUP)

MOST *z;
MASTER *mas;
RSHDR *rsc;

int AES_handle,
    diskbuff[512],
    rects,
    w_handle,
    c_handle,
    q_handle,
    w_num, snum,
    w_open,
    valid_path,
    moving,
    pxarray[10],
    dboxs, drag_snum,
    bar_w, bar_h, char_w, char_h,
    min_slid,
    stat_h,
/*%    colors, */
    rows[7][2], i_per_row[7], in_wind[7][2], max_itm[7][2], extra[7][2],
    w_dims[7][NUM_DIMS],
    oslid_h[2], oslid_y[2],
    d_active = -1, w_active = -1, num_w_active = -1,
    oth_w_active = -1, oth_hand_w = -1,
    use_8x16,
    user_icons,
/*%    sort_type=0, stcolumn=1, stlgsml=1, sizdattim[3]={1,1,1},
    showicon=1,	  003 */
    scnum=0,
    list_point, list_ignore, list_max, list_w,
    dial_wid,
    dir_items,
    mlistcnt, mlistptr,
    cli_handle,
    dtop_handle,
    real_top,
    new_winobj,
    aes_vdi_hand,
    vdi_hand,
    in_drag=-1,
    drag_icons,
    group_rows[7],
    clsizb[7],
    in_tab = -1,
    new_tab = -1,
    vplanes, xmax, ymax, colvals, v_bpp=-1, colmax, colmax1, vwrap,
    nameconf_ret,
    reorder_num,
    icon_voff,
    form_handle,
    Geneva_ver,
    menu_id,
    alt_acc[MAX_NEO_ACC] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    xasp, yasp,
    drag_x, drag_y,
    key_minus,
    key_equals,
    key_B,
    key_slash,
    dum;
unsigned int items[7], witems, dir_sec;

#define diskbuff ((char *)diskbuff)

struct Max_icon
{
  int text_w, data_w, h;
} max_icon;

long c_buflen, c_bufmax, clip_seek=-1L;
unsigned long total[7], consume[7];
#ifdef DEBUG
extern void (*old_crit)(), new_crit();
#endif
extern void cdecl byteswap( void *src, void *dest, int num );
long dflt_pall[16] = { 0xFFFFFF, 0x0, 0xFF0000, 0x5E9E, 0xFF, 0xFFFF,
    0xFFFF00, 0xFF00FF, 0xCCCCCC, 0x858585, 0xA70000, 0xA700, 0xA7, 0xA7A7,
    0xA7A700, 0xA700A7 };

unsigned char
     *mac,
     fdc_level,
     dest_off;
char boot_inf[24] = { '\xE9', '\0', 'N', 'e', 'o', 'D', 's', 'k',
	 '\0', '\0', '\0', '\0', '\2', '\2', '\1', '\0', '\2', '\160',
	 '\0', '\177', '\177', '\371', '\5', '\0' },
     tmpf[156], /* MUST be >127 for tail in rez=0, >=144 for dtext_fmt, divis by 4 */
     filename[125],
     volname[21],
     w_info[7][71], w_info_off[7], w_info_max[7],
     w_volname[7][21],
     over_112[7],
     know_icons[7],
     TOS_abort,
     ver_gt_10,
     ver_gt_12,
     ver_ge_20,
     aes_ge_14,
     aes_ge_20,
     aes_ge_40,
     use_wcolors,
     has_Geneva,
     has_sblast,
     has_magx,
     upsel[2], dwnsel[2],
     upself[2], dwnself[2],
     searchname[13],
     exe_boot[7],
     con_set,
     is_clip,
     clip_drive,
     dir_att,
     reorder_on,
     reordering,
     iconedit,
     m_on,
     list_current,
     TT_mono,
     TT_vid,
     falc_vid,
     just_open,
     update_clip,
     first_no_close,
     re_info, re_name,
     exec_master,
     has_clut,
     tab_str[21],
     dtop_wind,
     is_bee,
     in_list,
     jog_background,
     is_ac_close,
     loading_inf,
     no_b4,
     *reord_buf,
     *form_buf,
     *c_buf, *c_curbuf, *last_buf,
     *ocur, *obuf,
     *icon_buf,
     *exec_env,
     *nameconf_old, *nameconf_new,
     *external_gui,
     nil[] = "",
     slash[] = "\\",
     ifmt[] = "%d", nfmt[]="%n",
     lfmt[] = "%D", Nfmt[]="%N",
     glob[] = "*.*",
     globs[] = "\\*.*",
     colon_slash[] = ":\\",
     space[] = " ",
     colon[] = ":",
     crlf[] = "\r\n",
     neodesk_dat[] = "x:\\NEODESK.DAT",
     neoq[] = "NEOQUEUE",
     neocntrl[] = "NEOCNTRL",
     fvalid[]="._!@#$%^&()+-=~`;'\",<>|[]{}",
     ext[3][5] = { ".INF", ".MAC", ".NOT" };

extern char *msg_ptr[], *ttp_ptr, *ttp_path;
char *new_msgs, *sec_buf, *fat, *dirname;
int dirdrv, using_qdir=0, mch;
int copyqlen, copyqrem;
unsigned char *copy_q, copy_ok;
BPB *bp;
unsigned int ents_sec, fat_sec;
typedef struct
{
  unsigned char name[8], ext[3], att, unused[10];
  unsigned int time, date, cluster;
  unsigned long len;
} Dirent;
Dirent *de;

Rect prev, *list, ww[7][2], acc_wind, b4group[7], dtop_prev;
extern Rect form_rect[3];
extern int flev;
OBJECT *form, *deskpat, *menu, *lowmenu, *popups, *icons, *images, *winmenu,
    *grpmenu, *wmenu[7], *wtree[7];
FSTRUCT *wfile;
PRN_PARAM prn_param;
ICIC *icic;
GRAPHICS *graphics;
LoadCookie *lc;
GROUP_ITEM *group_start[7];
GROUP_DESC *group_desc[7];
NPI_DESC *npi_start[8], *last_npi;
PICTURE *desk_pic;
TREE maintree;
NIC_INFO nic_info;
int num_icons, icons_rem;
ICONBUF *nic_icons;
int ignore_events=-1;

TEDINFO vol_ted =  { 0L, "____________________", "XXXXXXXXXXXXXXXXXXXX",
                     IBM, 0, TE_CNTR , 0x11F0, 0, 1, 21, 21 };
OBJECT vol_tree[] = {{ -1, 1, 1,  G_BOX, SELECTABLE|DEFAULT|EXIT, 0, 0L },
                     { 0, -1, -1, G_FBOXTEXT, EDITABLE, 0, (long)&vol_ted } };

SELICON_DESC showinf_icon;
char showinf_path[120], in_showinf, showinf_ok, showinf_update, showinf_all,
    *show_path;
int show_ret, show_parm1, *show_date, show_wind;
long show_size;
char show_name[35];
GROUP_HDR *show_grp;
int (*show_func)( GROUP_HDR *gh );
extern FORM_TYPE formst[];

struct
{
  int num;
  ERRSTRUC *errstruc;
} fold_err;

typedef struct { int i0, i1, i2, i3, i4, i5, i6, i7; } DBX;
DBX *dbx;

void cdecl bytecpy( void *dest, void *src, int num )
{
  memcpy( dest, src, num );
}
void wtree_font( int num )
{
  OBJECT *o = wtree[num];
  TEDINFO *t1, *t2, *t3;
  WIND_FONT *wf;
  int f;
  
  if( Geneva_ver > 0x102 )
  {
    wf = &z->wind_font[3];
    t1 = o[WGINFO].ob_spec.tedinfo;
    t2 = o[WGMOVE].ob_spec.tedinfo;
    t3 = o[new_winobj+4].ob_spec.tedinfo;
    if( wf->h > bar_h-2 ) f = IBM;
    else f = wf->scale&1 ? GDOS_PROP : GDOS_BITM;
    vol_ted.te_font = t1->te_font = t2->te_font = t3->te_font = f;
    vol_ted.te_junk1 = t1->te_junk1 = t2->te_junk1 = t3->te_junk1 = wf->id;
    vol_ted.te_junk2 = t1->te_junk2 = t2->te_junk2 = t3->te_junk2 = wf->size;
  }
}
void load_fonts(void)
{
  int i;
  OBJECT *o;
  char mouse=0;

  if( !is_bee )
  {
    bee();
    mouse++;
  }
  (*graphics->load_fonts)( z->wind_font, 6 );
  /* icon+notes fonts always proportional if not system */
  if( z->wind_font[0].id != 1 ) z->wind_font[0].scale &= ~2;
  if( z->wind_font[4].id != 1 ) z->wind_font[4].scale &= ~2;
  get_max_icon(-1);
  for( i=z->num_icons, o=z->desk+1; --i>=0; o++ )
  {
    o->ob_width = max_icon.text_w;
    o->ob_height = max_icon.h;
  }
  for( i=0; i<7; i++ )
    if( z->w[i].place>0 ) wtree_font(i);
  if( mouse ) arrow();
}
void colbits(int i)
{
  (*graphics->colbits)(i?-1:0);
}
void pats( int *patptr, int patmsk )
{
  (*graphics->pats)( patptr, patmsk );
}
void x1y1arr( int *arr )
{
  (*graphics->x1y1arr)( arr );
}
void cdecl blit( Rect *box1, Rect *box2, int mode, int tr, long ptr )
{
  (*graphics->blit)( box1, box2, mode, tr, ptr );
}
void blitit( Rect *box1, Rect *box2, int tr )
{
  (*graphics->blitit)( box1, box2, tr );
}
int set_wmode( int mode )
{
  return( (*graphics->set_wmode)(mode) );
}
int wmode0(void)
{
  return( (*graphics->wmode0)() );
}
int wmode1(void)
{
  return( (*graphics->wmode1)() );
}
int wmode2(void)
{
  return( (*graphics->wmode2)() );
}
void draw_bx( int *box )
{
  (*graphics->draw_bx)(box);
}
void set_lnmask( int mask )
{
  (*graphics->set_lnmask)(mask);
}
void desk_color( int col )
{
  (*graphics->desk_color)(col);
}
void blit_init( MFDB *fdb )
{
  (*graphics->blit_init)(fdb);
}
void x1y1val( int x1, int y1, int x2, int y2 )
{
  (*graphics->x1y1val)(x1,y1,x2,y2);
}
void set_pattern( int *box )
{
  (*graphics->set_pattern)(box);
}
void set_intin12( int i1, int i2 )
{
  (*graphics->set_intin12)(i1,i2);
}
void form_copy( int flag )
{
  (*graphics->form_copy)(flag,form_buf,&form_rect[flev]);
}
void init_scrptr(void)
{
  (*graphics->init_scrptr)();
}
void cdecl gtext( int x, int y, unsigned char *str, int fnum, int center )
{
  (*graphics->gtext)( x, y, str, fnum, center, 0, 1 );
}
void gtext2( int x, int y, unsigned char *str, int fnum, int center, int mode, int color )
{
  (*graphics->gtext)( x, y, str, fnum, center, mode, color );
}
void hide_mouse(void)
{
  (*graphics->hide_mouse)();
}
void show_mouse(void)
{
  (*graphics->show_mouse)();
}
void gr_box(void)
{
  (*graphics->gr_box)();
}
void wait_mbut(void)
{
  (*graphics->wait_mbut)();
}
void cdecl set_clip( int *arr, int mode )
{
  (*graphics->set_clip)(arr,mode);
}
/**********************************************************************/
int Getshift(void)
{
  return(Kbshift(-1)&0xf);
}
/********************************************************************/
#ifdef DEMO
void demo_version(void)
{
  f_alert1( msg_ptr[EXE_MSGS-1] );
}
void cli_demo_init(void)
{
  extern NEO_ACC nac;
  int hand, cmsg[8];

  if( (hand = appl_pfind("CLI_DEMO")) >= 0 )
  {
    cmsg[0] = CLI_DEMO_INI;
    cmsg[2] = 0;
    cmsg[3] = NEO_ACC_MAGIC;
    *(long *)&cmsg[4] = (long)&nac;
    cmsg[6] = cmsg[1] = AES_handle;
    appl_pwrite( hand, 16, cmsg );
  }
}
#endif DEMO
/**********************************************************************/
int check_iconedit(void)
{
  if( iconedit )
  {
    f_alert1( msg_ptr[111] );
    return(0);
  }
  return(1);
}
/*****************************************************************/
int trans_gmenu( int wind, int num, int from_drv )
{	/* num<0 for group menu item */
  static unsigned char drv[] = { WIMOPEN, WIMSHOW, CLOSEWIN,
      SELALL, WIMDEL, SHOWICON, SHOWTEXT, STLGSML, SORTNAME, SORTTYPE,
      UPDATE, WIMAPP, WIMLOAD, 0 };
  static unsigned char grp[] = { GWIMOPEN, GWIMSHOW, GWIMCLOS,
      GWIMSEL, GWIMDEL, GWIMICON, GWIMTEXT, GWIMLGSM, GWIMNAME, GWIMTYPE,
      GWIMUPDT, GWIMAPP, GWIMLOAD, 0 };
  unsigned char *p;
  int i;

  if( wind>=0 && ed_wind_type(wind)==EDW_GROUP )
  {
    if( from_drv )
    {
      if( num<0 ) return -num;
      if( (p=strchr(drv,num)) != 0 ) return grp[p-drv];
    }
    else if( (p=strchr(grp,num)) != 0 ) return drv[p-grp];
    return 0;
  }
  else if( iconedit )
    if( wind<0 ) return (*icic->trans_mmenu)( num, from_drv );
    else return (*icic->trans_wmenu)( wind, num, from_drv );
  return num<0 ? 0 : num;
}
/*****************************************************************/
void menu_enable( int wind, int item, int flag )
{
  int *i;
  OBJECT *mnu;

  mnu = wind<0 ? menu : wmenu[wind];
  if( (item = trans_gmenu( wind, item, 1 )) == 0 ) return;
  i = (int *)&mnu[item].ob_state;
  if( *i != (flag ? *i&~DISABLED : *i|DISABLED) ) menu_ienable( mnu, item, flag );
}
/*****************************************************************/
void menu_check( int wind, int item, int flag )
{
  int *i;
  OBJECT *mnu;

  mnu = wind<0 ? menu : wmenu[wind];
  if( (item = trans_gmenu( wind, item, 1 )) == 0 ) return;
  i = (int *)&mnu[item].ob_state;
  if( *i != (!flag ? *i&~CHECKED : *i|CHECKED) ) menu_icheck( mnu, item, flag );
}
/********************************************************************/
int ed_wind_type( int ind )
{
  int i;

  if( ind<0 ) return 0;
  if( group_desc[ind] ) return EDW_GROUP;
  if( !iconedit ) return EDW_DISK;
  return (*icic->ed_wind_type)(ind);
}
/********************************************************************/
int uppath( char *path )
{
  int i;

  i = pathend(path) - 2;		   /* last slash in path - 1 */
  for( ; i>=0; i-- )
    if( path[i] == '\\' ) return i;	   /* find next-to-last slash */
  return 0;
}
void backup( int flag )
	  /* move the current window's path up by one directory level */
{
  register int j;

  if( w_num >= 0 )
  {
    j = uppath( z->w[w_num].path );
    if( !j || ed_wind_type(w_num) != EDW_DISK )
    {
      if( flag )
      {
	add_macro( MACWIND, MWCLOSE );
	close_wind();
      }
    }
    else new_path( j+1, flag );
  }
}
/********************************************************************/
int fix_bootsec( Bsec *boot )
{
  if( boot->secs && boot->bps )
  {
    if( !boot->spt ) boot->spt = 16;
    if( !boot->sides ) boot->sides = 1;
    if( !boot->tps ) boot->tps = boot->secs>>4;
    return(1);
  }
  return(0);
}
int is_MetaDOS( int drv )
{
  long buf[4] = { 0, 0, 0, 0 };
  extern void cdecl CDROM( int op, ... );
  #define META_INIT 0x30

  CDROM( META_INIT, buf );
  if( buf[0]&(1L<<drv) ) return(1);
  return(0);
}
int bootsec( int drv, Bsec *boot )
{
  int errnum;
  union
  {
    unsigned char c[2];
    unsigned int i;
  } u;

  memclr( boot, sizeof(Bsec) );
  if( is_MetaDOS(drv-'A') ) return(0);
  if( (errnum = readboot(drv)) == 0 )
  {
    u.c[0] = diskbuff[25];
    u.c[1] = diskbuff[24];
    boot->spt = u.i;
    u.c[0] = diskbuff[20];
    u.c[1] = diskbuff[19];
    boot->secs = u.i;
    u.c[0] = diskbuff[12];
    u.c[1] = diskbuff[11];
    boot->bps = u.i;
    u.c[0] = diskbuff[27];
    u.c[1] = diskbuff[26];
    boot->sides = u.i;
    if( boot->sides && boot->spt ) boot->tps = boot->secs/boot->sides/boot->spt;
    boot->clsiz = boot->bps*diskbuff[13];
  }
  else if( errnum==CLIP_ERR ) errnum = 0;
  return( errnum==-1L ? 1 : errnum );
}
int readboot( int drv )
{
  char drive;
  int err;
  void end_qdir(void);
  int getbpb( char *path, int mult );

  if( drv == CLIP_LET ) return(CLIP_ERR);
  if( is_locked(drv) ) return LOCK_ERR;
  drv = (drive=drv)-'A';
  if( (err=getbpb( &drive, -1 )) == 0 )
  {
    err = Rwabs( 2, sec_buf, 1, 0, drv );
    memcpy( diskbuff, sec_buf, 512 );
    using_qdir++;
    end_qdir();
    return(err);
  }
  if( err < 0 ) return(-1);
  return(AENSMEM);
/*  return( Floprd(diskbuff, nil, drv, 1, 0, 0, 1) ); */
}
/**********************************************************************/
int consump( char drive, char *s, long l, int wnum )
{
  int *clsiz;
  
  if( l && z->other_pref.b.consumption && drive!=CLIP_LET )
  {
    if( wnum<0 )
    {
      if( getbpb( &drive, -1 ) ) goto bad;
      using_qdir++;
      end_qdir();
      clsiz = &bp->clsizb;
    }
    else clsiz = &clsizb[wnum];
    spf( s, msg_ptr[45], l, (l+*clsiz-1) / *clsiz * *clsiz,
        l!=1 ? msg_ptr[23] : nil );
    return 1;
  }
bad:
  spf( s, msg_ptr[44], l, l!=1 ? msg_ptr[23] : nil );
  return 0;
}
/**********************************************************************/
int check_batch( int ignore )	    /* is there a batch file interpreter? 1=yes */
{
  cli_handle = -1;
  if( z->cli >= 0 )
    if( (cli_handle = appl_pfind(z->neo_acc[z->cli])) >= 0 ) return(1);
  if( z->batch_name[0] && !cFsfirst( z->batch_name, 0x37 ) ) return(1);
  if( !ignore ) f_alert1( msg_ptr[4] );
  return(0);
}
/********************************************************************/
int check_copy( char *src, char *dest )
{
  char temp[200];
  register int i;

  if( *src==*dest && slashes(src)<slashes(dest) && !*((i=pathend(src))+src) )
    if( !strncmp( src, dest, i ) )
    {
      spf( temp, msg_ptr[5], moving ? msg_ptr[6] : msg_ptr[7] );
      f_alert1( temp );
      arrow();
      return(0);
    }
  return(1);
}
/********************************************************************/
int check_split( int foo )
{
  register int k, m, bh2;
  register struct Wstruct *ws;

  ws = &z->w[w_num];
  if( ws->split != foo )
  {
    if( !ws->split ) upsel[0] = dwnsel[0] = -1;
    ws->split = foo;
    return(1);
  }
  return(0);
}
/********************************************************************/
int wget_top(void)
{
  int i;

  wind_get( 0, WF_TOP, &i, &dum, &dum, &dum );
  return( real_top = i );
}

void clean_up(void)
{
  int i;

  if( !z->multitask && !z->is_acc/*002*/ )
    if( aes_ge_14 )
    {
      wind_new();		  /* clears wind_update(), so restore balance */
/*%      wind_update( BEG_UPDATE );*/
      /* hack to avoid a bug in AES in TOS 2.0x/3.0x */
      if( aes_ge_20 && !aes_ge_40 && (i=x_wind_create( 0, 0, z->cliparray[2], 30, 1, 1 )) >= 0 )
      {
	wind_open( i, z->cliparray[2], 30, 1, 1 );
	wind_close(i);
	wind_delete(i);
      }
    }
    else
      do
	if( (i=wget_top())>0 )
	{
	  wind_close(i);
	  wind_delete(i);
	}
      while( i>0 );
}
/********************************************************************/
int close_ev_ic(void)
{
  if( !close_all() ) return 0;
  close_all_fwind( AES_handle, 0 );
  return 1;
}
void close_deskwind(void)
{
  if( !is_ac_close && dtop_handle>0 )
  {
    wind_close( dtop_handle );
    wind_delete( dtop_handle );
  }
  dtop_handle = 0;
}
int close_every(void)
{
  close_deskwind();
  if( !close_ev_ic() ) return 0;
  clean_up();
  return 1;
}
/********************************************************************/
int find_place( int func( int a, int b ) );

int wxref[7] = { -1, -1, -1, -1, -1, -1, -1 };

int *wxref_ind( int num )
{
  int i;
  static int dum;

  for( i=0; i<7; i++ )
    if( wxref[i]==num ) return &wxref[i];
  dum = -1;
  return &dum;
}

int wind_xref( int num )
{
  int i;

  for( i=0; i<7; i++ )
    if( wxref[i]==num ) return i;
  return -1;
}

void free_files( int num )
{
  cmfree((char **)&z->file[num]);
  z->num_files[num] = z->files_rem[num] = 0;
}

void free_npis( int num )
{
  NPI_DESC *n, *n2;
  
  for( n2=npi_start[num+1]; (n=n2)!=0; )
  {
    n2 = n->next;
    lfree(n);
  }
  npi_start[num+1] = 0L;
}

void free_wind( int num, int some_forms )
{
  int i;
  GROUP_ITEM *gi, *gi2;
/*%  Rect *r, *r2;*/

  close_wforms( num, some_forms );
  cmfree((char **)&wmenu[num]);
  cmfree((char **)&wtree[num]);
  free_files(num);
  if( group_desc[num] )
  {
    strcpy( z->w[num].path, group_desc[num]->path );
/*%    for( i=num, r=(Rect *)&z->w[num].x; ++i<7; )	/* 003 */  rmv 004 
      if( z->w[i].place<=0 )
      {
        r2 = (Rect *)&z->w[i].x;
        *r = *r2;
        r = r2;
      }
    *r = b4group[--b4ptr]; */
    *(Rect *)&z->w[num].x = b4group[num];
    for( gi2=group_start[num]; (gi=gi2)!=0; )
    {
      gi2 = gi->next;
      lfree(gi);
    }
    group_start[num] = 0L;
    cmfree( (char **)&group_desc[num] );
  }
  free_npis(num);
  lfreeall(num);
  if( iconedit && (i=ed_wind_type(num)) >= EDW_FILE ) (*icic->clear_icons)(i);
}
int close_pl( int wh, int num )
{
  int oh, on, ret;

  if( !is_ac_close )
  {
    oh = w_handle;
    on = w_num;
    w_handle = wh;
    w_num = num;
    ret = m_savegrp(1)<0;
    w_handle = oh;
    w_num = on;
    if( ret ) return 1;
    wind_close( wh );
    wind_delete( wh );
  }
  if( wh==oth_hand_w ) oth_w_active = oth_hand_w = -1;
  free_wind( num, 0 );
  z->w[num].place = -z->w[num].place;	/* 003: so grp wind pos doesn't interfere */
  *wxref_ind(wh) = -1;
  if( iconedit && (ret=ed_wind_type(num)) > 0 ) (*icic->clear_icons)(ret);
  return(0);
}
void all_inactive(void)
{
  register int i;
  static char inactive[] = { OPEN, SHOWINF, INSTAPP, LOADINF, SEARCHDR };
/*CREATE, CLOSEFLD, CLOSEWIN, UPDATE, FUPDATE, SORTFILT, SEARCHPT, SELALL, PRINTDIR, REORDER  */

  for( i=0; i<sizeof(inactive); i++ )
    menu_enable( -1, inactive[i], 0 );
}
void no_wactive(void)
{
  w_active = num_w_active = in_tab = -1;
}
int cdecl close_all(void)	/* close all windows opened by this program */
{
/*  int i;*/
  
  if( find_place( close_pl ) ) return 0;
  w_open = 0;
  no_wactive();
  w_num = w_handle = -1;
  all_inactive();
/*  for( i=0; i<7; i++ )
    if( z->w[i].place>0 ) z->w[i].place = -z->w[i].place;   003: in close_pl */
  is_ac_close = 0;
  return 1;
}
/********************************************************************/
void cdecl close_form( Rect cbox, int flag )
{
/*%  register int oldn, oldh, i, pl, pl2;

  if( !flag ) form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect( cbox ) );
  oldn = w_num;
  oldh = w_handle;
  pl = z->w[w_num].place;
  for( w_num=0; w_num<7; w_num++ )
    if( (w_handle=wxref[w_num]) >= 0 )
    {
      set_wfile();
      i = ignore[w_num];
      rects=0;
      if( (pl2=z->w[w_num].place) > 0 )
	if( flag>=0 || pl2 < pl )
	{
	  ignore[w_num] = 0;
	  if( flag > 0 && pl2==w_open ) redraw_mover( pl2 );
	  redraw_wind( cbox, 1 );
	}
	else if( pl2 != pl ) select( cbox, w_handle );
      ignore[w_num] = rects || flag>0 ? 2 : i;
    }
  w_handle = oldh;
  w_num = oldn;
  set_wfile(); */

  register int oldn, oldh, i, pl, pl2;
  int buf[8] = { WM_REDRAW, 0, 0 };

  if( !flag ) form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect( cbox ) );
  else
  {
    oldn = w_num;
    oldh = w_handle;
    pl = z->w[w_num].place;
    for( w_num=0; w_num<7; w_num++ )
      if( (w_handle=wxref[w_num]) >= 0 )
      {
	set_wfile();
	rects=0;
	if( (pl2=z->w[w_num].place) > 0 )
	  if( flag>=0 || pl2 < pl )
	  {
	    buf[3] = w_handle;
	    *(Rect *)&buf[4] = cbox;
	    appl_write( buf[1]=AES_handle, 16, buf );
	  }
      }
    w_handle = oldh;
    w_num = oldn;
    set_wfile();
  }
}
/********************************************************************/
void close_wind(void)				   /* close a window */
{
  register int i, pl;
  Rect r;

  if( w_num >= 0 && m_savegrp(1)>=0 )
  {
    if( iconedit && !(*icic->close_icons)( w_num ) ) return;
    free_wind( w_num, 1 );
    if( w_handle==oth_hand_w ) oth_w_active = oth_hand_w = -1;
    wind_close( w_handle );
    wind_delete( w_handle );
    pl = z->w[w_num].place;
    z->w[w_num].place = z->w[w_num].f_off.l = 0;
    r.x = z->w[w_num].x;
    r.y = z->w[w_num].y;
    r.w = z->w[w_num].w + 2;
    r.h = z->w[w_num].h + 2;
    *wxref_ind(w_handle) = -1;
    for( i=0; i<7; i++ )
      if( z->w[i].place && z->w[i].place > pl ) z->w[i].place--;
    if( num_w_active == w_num ) no_wactive();
    w_open--;
    close_form( r, 1 );
    if( !w_open ) all_inactive();
    get_top_wind();
  }
}
/******************************************************************/
void pad_it( char *ptr )
{
  int j;

  j = strlen(ptr);
  while( j<11 ) ptr[j++] = ' ';
  ptr[11] = '\0';
}
int set_filename( FORM **src, int modal, int fl, int ind, char *ptr1, char *ptr2 )
{	/* 0: no error	 1: skip   -1: abort   -2: bad name */
	/* ptr1: old path   ptr2: new path\name (in/out) */
  int i, flags;
  char temp[20], name[20];
  FSTRUCT *fs;
  extern FORM *forms;

  if( src ) form = (*src)->tree;
  if( fl>=0 )
  {
    strcpy( name, (fs=&wfile[fl])->name );
    strcpy( ptr1, z->w[w_num].path );
  }
  else strcpy( name, spathend(ptr1) );
  if( iconedit && ed_wind_type(w_num)!=EDW_DISK )
  {
    from_filename( form[ind].ob_spec.tedinfo->te_ptext, temp, 0 );
    strcpy( ptr1=fs->nib->ib->ib_ptext, form[ind].ob_spec.tedinfo->te_ptext );
    pad_it( ptr1 );
    strcpy( fs->name, temp );
    return(0);
  }
  if( ind >= 0 )
  {
    if( !test_filename( form[ind].ob_spec.tedinfo->te_ptext, temp, 1 ) )
	return(-2);
  }
  else strcpy( temp, spathend(ptr2) );
  *spathend(ptr1) = '\0';
  strcpy( ptr2, ptr1 );
  strcat( nameconf_old=ptr1, name );
  strcat( nameconf_new=ptr2, temp );
/***  if( src ) src->update = u_nameconfl; ***/
  modal = 1;	/* for now always modal */
  formst[FCON_FORM].flags.modal = modal;
  if( src ) i = *src-forms;
  start_form( FCON_FORM );
  /* caution: *src might have moved now! */
  if( src ) *src = forms+i;
  return nameconf_ret;
}
int _set_filename( int fl, int ind, char *ptr1, char *ptr2 )
{ /*% remove me eventually */
  return set_filename( 0L, 1, fl, ind, ptr1, ptr2 );
}
/********************************************************************/
void adj_all_gitem( int wind, int dir, int off )
{
  GROUP_ITEM *gi;
  DBX *d;
  int i;

  for( gi=group_start[wind]; gi; gi=gi->next )
    *(&gi->x + dir) -= off;
  for( d=dbx, i=0; i<dboxs; i++, d++ )
  {
    *(&d->i0 + dir) -= off;
    *(&d->i2 + dir) -= off;
    *(&d->i4 + dir) -= off;
    *(&d->i6 + dir) -= off;
  }
}
int add_group( int wind, int snum, char *path, char *name, PROG_TYPE *p, NICONBLK *ib, int x, int y )
{
  GROUP_ITEM *gi, **gi2;

  if( (gi = (GROUP_ITEM *)lalloc(sizeof(GROUP_ITEM),wind)) != 0 )
  {
    memclr( gi, sizeof(GROUP_ITEM) );
    /* 003: used to add at start, now at tail so order is preserved for GRun */
    for( gi2=&group_start[wind]; *gi2; gi2=&(*gi2)->next );
    *gi2 = gi;
    if( snum>=0 )
    {
      /* account for diffs in icon widths */
      x += (max_icon.text_w - 20*6) >> 1;
      if( (gi->x = x - ww[wind][snum].x) < 0 )
      {
        adj_all_gitem( wind, 0, gi->x );
        gi->x = 0;
      }
      if( (gi->y = y - ww[wind][snum].y + (z->w[wind].f_off.i[snum]<<4)) < 0 )
      {
        adj_all_gitem( wind, 1, gi->y );
        gi->y = 0;
      }
    }
    else
    {
      gi->x = x;
      gi->y = y;
    }
/* 003    group_start[wind] = gi; */
    strcpy( gi->p.path, path );
    gi->p.type = *p;
    gi->p.nib = ib;
    strcpy( gi->name, name );
    /*gi->desc[0][0] = gi->desc[1][0] = 0;  handled by memclr */
    group_desc[wind]->hdr.entries++;
    return 1;
  }
  return 0;
}
/********************************************************************/
int ask_move(void)
{
  if( z->move_mode == PREFMASK-PREFMCPY )
      if( (moving = form_alert( moving+1, msg_ptr[3] ) - 1) == 2 ) return(0);
  return(1);
}
/***************************************************************/
void from_w_2group( int wind )
{
  int k, i;
  DBX *ptr;
  char temp[120], src_grp, dst_grp, update=0;
  FSTRUCT *fs;
  GROUP_ITEM *gi;

  src_grp = ed_wind_type(w_num)==EDW_GROUP;
  dst_grp = ed_wind_type(wind)==EDW_GROUP;
  if( !src_grp || wind==w_num || ask_move() ) for( fs=wfile,
      ptr=dbx, k=0; k<witems; k++, fs++ )
    if( fs->state )
      if( wind != w_num )
      {
	get_full_name( temp, k, w_num );
	if( temp[0]==CLIP_LET )
	{
	  TOS_error( CLIP_ERR, 0 );
	  break;
	}
	if( !add_group( wind, drag_snum, temp, src_grp&&dst_grp ||
	    (i=fs->type.p.pexec_mode)==GROUP || i==NPI ? fs->groupname :
	    spathend(temp), &fs->type, fs->nib, ptr->i0, ptr->i1 ) ) break;
	if( src_grp && moving ) trash_init( temp );
	ptr++;
	update = 1;
      }
      else if( z->mshowicon /* 003 */ )	/* only change pos if icons */
      {
	gi = fs->grp_item;
	if( (gi->x = ptr->i0 - ww[wind][drag_snum].x) < 0 ) gi->x = 0;
	if( (gi->y = ptr->i1 - ww[wind][drag_snum].y +
	    (z->w[wind].f_off.i[drag_snum]<<4)) < 0 ) gi->y = 0;
	ptr++;
	update = 1;
      }
  drag_free();
  if( update )
  {
    if( src_grp && moving ) first(0);
    update_othwind( wind, 0 );
  }
}
/********************************************************************/
int dcreate( char *path )
{
  long l;

  if( (l=cDcreate(path))==AEACCDN && pathend(path)==3 )
  {
    f_alert1( msg_ptr[14] );
    return(0);
  }
  return(TOS_error(l,0));
}
/********************************************************************/
void de_act( int except, int wind )
{
  de_act_d( wind==-1 ? except : -1 );
  de_act_w( except, wind );
}
/******************************************************************/
void de_act_d( int except )
{
  register int i;

  if( Getshift()&1 ) return;
  if( d_active >= 0 )
  {
    list_current = 2;
    for( i=1; i<=z->num_icons; i++ )
      if( i != except )
	select_d( i, 0 );
    list_current = 0;
  }
  if( except < 0 ) d_active = -1;
}
/********************************************************************/
void de_act_other( int flag )
{
  int oldn, oldh;

  if( num_w_active >= 0 )
  {
    oldn = w_num;
    oldh = w_handle;
    w_num = num_w_active;
    set_wfile();
    find_handle();
    set_window();
    de_act_w( -1, -1 );
    if( w_num != oldn || flag ) info();
    w_handle = oldh;
    w_num = oldn;
    set_wfile();
    set_window();
  }
}
/*****************************************************************/
void de_act_w( int except, int wind )
{
  register int j, oh, on;

  if( Getshift()&1 ) return;
  oh = w_handle;
  on = w_num;
  if( w_active >= 0 )
  {
    w_num = num_w_active;
    set_wfile();
    find_handle();
    set_window();
    list_current = z->w[w_num].split ? 0 : 2;
    for( j=0; j<witems; j++ )
      if( num_w_active != wind || j != except ) select_w( j, 0, w_handle, 1 );
    list_current = 0;
    redraw_arrows();
  }
  if( oth_w_active >= 0 )
  {
    w_num = wind_xref(w_handle=oth_hand_w);
    set_wfile();
    set_window();
    select_w( oth_w_active, 0, oth_hand_w, 0 );
    oth_w_active = oth_hand_w = -1;
  }
  w_num = on;
  w_handle = oh;
  set_wfile();
  set_window();
}
/********************************************************************/
int drag_set( int a, int b, int c, int d, int id )
{
  DBX i;
  static int dbx_rem;

  i.i2 = i.i4 = (i.i0 = i.i6 = d) + b;
  i.i5 = i.i7 = (i.i1 = i.i3 = c) + a;
  return add_thing( (void **)&dbx, &dboxs, &dbx_rem, &i, 5, sizeof(DBX), id );
}

void drag_free(void)
{
  cmfree( (char **)&dbx );
}

void drag_gad( int ind, int state )
{
  ind = new_winobj-ind-1;
  if( ind >= new_winobj+1 && ind<=new_winobj+3 &&
      state!=(wtree[w_num][ind].ob_state&SELECTED) )
  {
    obj_true1( wtree[w_num], state, ind );
    _redraw_obj( z->maximum, ind );
  }
}

int is_icgroup( int w )
{
  return ed_wind_type(w) == EDW_GROUP && z->showicon[w];
}

int get_hilo( int w, int snum, int *hi )
{
  int lo;

  if( is_icgroup(w) )
  {
    lo = 0;
    *hi = items[w];
  }
  else if( (*hi = (lo=z->w[w].f_off.i[snum]) + in_wind[w][snum] +
      extra[w][snum]) > items[w] ) *hi = items[w];
  return lo;
}

int drag_box( int *flag, int *ind, int oldx, int oldy )
	     /* return: 0=error, 1=not on obj, 2=on obj, 3=on parent */
	     /* flag:	num of window (in and out)     */
	     /* ind:  object selected (in and out)   */
/* Caller must call free_drag()!! */
{
  int i, j, l, x, y, b, k=0, state=0, dx, dy, high,
      max[4] = { 0, 0, 0, 0 }, oldin, oldfl, oldw, oldk, oldst;
  FSTRUCT *fs;

  dbx = 0L;
  get_max_icon(*flag);
  if( *flag>=0 )
  {
    i = get_hilo( *flag, snum, &high );
    for( fs=&z->file[*flag][i]; i<high; i++, fs++ )
      if( fs->state )
	if( !drag_set( max_icon.h, mas->rez || max_icon.text_w<=112 ?
	    max_icon.text_w : 112, fs->y[snum], fs->x, *flag ) ) break;
  }
  else
    for( i=1; i<=z->num_icons; i++ )
      if( z->idat[i-1].state&1 )
	if( !drag_set( max_icon.h, max_icon.text_w, z->desk[i].ob_y+z->desk[0].ob_y,
	    z->desk[i].ob_x+z->desk[0].ob_x, -1 ) ) break;
  if( !dboxs ) return 0;
  for( i=1; i<dboxs; i++ )     /** can be left as 1 **/
  {
    if( dbx[i].i0 < dbx[max[0]].i0 ) max[0] = i;
    if( dbx[i].i1 < dbx[max[1]].i1 ) max[1] = i;
    if( dbx[i].i2 > dbx[max[2]].i2 ) max[2] = i;
    if( dbx[i].i5 > dbx[max[3]].i5 ) max[3] = i;
  }
  moving = 0;
  wind_lock(1);
  set_clp_rect( &z->maximum, 1 );
  graf_mkstate( &x, &y, &b, &oldk );
  if( reorder_on ) _graf_mouse( POINT_HAND );
  check_moving(oldk);
  oldin = *ind;
  oldfl = *flag;
  oldst = 0;
  oldw = w_handle;
  hide_mouse();
  while( b & 1 )
  {
    if( state )
    {
      hide_mouse();
      for( i=0; i<dboxs; i++ )
	draw_box( (int *)&dbx[i] );
    }
    dx = x - oldx;
    dy = y - oldy;
    if( dbx[max[0]].i0 + dx < z->cliparray[0] ) dx = z->cliparray[0]
	- dbx[max[0]].i0;
    if( dbx[max[1]].i1 + dy < z->cliparray[1] ) dy = z->cliparray[1]
	- dbx[max[1]].i1;
    if( dbx[max[2]].i4 + dx > z->cliparray[2] ) dx = z->cliparray[2]
	- dbx[max[2]].i4;
    if( dbx[max[3]].i5 + dy > z->cliparray[3] ) dy = z->cliparray[3]
	- dbx[max[3]].i5;
    for( i=0; i<dboxs; i++ )
    {
      dbx[i].i0 = dbx[i].i6 += dx;
      dbx[i].i1 = dbx[i].i3 += dy;
      dbx[i].i2 = dbx[i].i4 += dx;
      dbx[i].i5 = dbx[i].i7 += dy;
    }
    state = 1;
    if( (*flag=wind_xref(j = wind_find(x,y))) < 0 )
    {
      if( j!=dtop_handle ) state = -1;
      else if( (*ind = find_d( x, y )) >= 0 )
	if( z->idat[*ind].state&1 ) state = -1;
	else if( (i=z->idat[*ind].type) < D_PROG || z->programs[i-D_PROG].is_acc ||
	    (l=z->programs[i-D_PROG].p.type.p.pexec_mode) == FOLDER ||
	    l == PROG || l == BATCH || l == NPI || iconedit )
	{
	  select_d( *ind+1, SELECTED );
          set_clp_rect( &z->maximum, 1 );	/* 002: added */
	  state++;
	}
    }
    else
    {
      drag_snum = y >= ww[*flag][0].y && y < ww[*flag][0].y+ww[*flag][0].h ? 0 : 1;
      if( (*ind=find_w( x, y, j )) >= 0 )
      {
	fs = &z->file[*flag][*ind];
	if( fs->state ) state = -1;
	else if( ed_wind_type(*flag) > EDW_DISK ||
	    (l=fs->type.p.pexec_mode) == FOLDER || l == PROG || l == NPI ||
	    l == BATCH || reorder_on )
	{
	  select_w( *ind, SELECTED, j, 0 );
	  state++;
	}
      }
      else if( *flag==in_drag && (i=objc_find( wtree[*flag], 0, 8, x, y ))>=new_winobj+1 &&
	  i<=new_winobj+3 && drag_icons&(1<<i-new_winobj-1) )
      {
	state = 3;
	*ind = new_winobj-i-1;
      }
    }
    if( in_drag>=0 )
    {
      if( *ind != oldin ) drag_gad( oldin, 0 );
      drag_gad( *ind, state==3 );
      set_clp_rect( &z->maximum, 1 );
    }
    if( oldst>0 )
    {
      if( oldin == *ind && j == oldw /* 003 oldfl == *flag*/ ) state = oldst;
      if( (oldin != *ind || j != oldw /* 003 oldfl != *flag*/) && oldin >= 0 )
        if( oldfl < 0 )
        {
          select_d( oldin+1, 0 );
          set_clp_rect( &z->maximum, 1 );	/* 002: added */
        }
        else select_w( oldin, 0, oldw, 0 );
    }
    for( i=0; i<dboxs; i++ )
      draw_box( (int *)&dbx[i] );
    show_mouse();
    oldx = x;
    oldy = y;
    oldk = k;
    oldin = *ind;
    oldfl = *flag;
    oldst = state;
    oldw = j;
    do
    {
      graf_mkstate( &x, &y, &b, &k );
      if( k != oldk ) check_moving(k);
    }
    while( b&1 && x==oldx && y==oldy );
  }
  if( state )
  {
    hide_mouse();
    for( i=0; i<dboxs; i++ )
      draw_box( (int *)&dbx[i] );
  }
  show_mouse();
  /*if( state<2 || reorder_on ) 003 */ arrow();
  if( dtop_wind && state>0/* 003: was != 0 */ && *flag<0 )
    for( i=0; i<dboxs; i++ )
    {
      if( dbx[i].i0 < (j=z->desk[0].ob_x) ) dbx[i].i0 = j;
      if( dbx[i].i1 < (j=z->desk[0].ob_y) ) dbx[i].i1 = j;
      if( dbx[i].i2 > (j=z->desk[0].ob_x+z->desk[0].ob_width) ) dbx[i].i0 = j-72;
      if( dbx[i].i5 > (j=z->desk[0].ob_y+z->desk[0].ob_height) ) dbx[i].i1 = j-40;
    }
  wind_lock(0);
  drag_x = x;	/* 003 */
  drag_y = y;
  return( state );
}
int wdrag_box( int *flag, int *ind, int oldx, int oldy )
{
  int ret, t;

  in_drag = *flag;
  drag_icons = 4|2;
  t = ed_wind_type(*flag);
  if( t>=EDW_ICONS ) drag_icons = 4;
  else if( t!=EDW_GROUP && slashes(z->w[*flag].path)>1 ) drag_icons = 4|2|1;
  redraw_obj( z->maximum, new_winobj );
  if( (ret = drag_box( flag, ind, oldx, oldy )) != 3 )
  {
    in_drag = -1;
    obj_true1( wtree[w_num], 0, new_winobj+1 );
    redraw_obj( z->maximum, new_winobj );
  }
  return ret;
}
/****************/
void check_moving( int key )
{
  char old;
  int mode;

  old = moving;
  moving = (key&0x0E)==0x0E;
  if( /*(mode = z->move_mode) != PREFMASK-PREFMCPY && 003*/ !reorder_on )
  {
    if( (mode = z->move_mode) != PREFMASK-PREFMCPY )	/* 003: moved here */
      if( moving ) moving = !mode;
      else moving = mode;
    if( moving != old ) _graf_mouse( moving ? FLAT_HAND : ARROW );
  }
}
/********************************************************************/
int icon_width( int len )
{
  return (len *= z->wind_font[0].w) < ICON_W ? ICON_W : len;
}
int cdecl draw_d_icon( PARMBLK *pb )
{
  register NICONBLK *i_ptr;
  register int i, j;

  i = z->idat[j = pb->pb_obj-1].type;
  if( i<0 ) return 0;
  else if( i < D_PROG ) i_ptr = z->idat[j].nicb;
  else if( (i-=D_PROG) >= z->num_progs ) return 0;
  else i_ptr = z->programs[i].p.nib;
  set_clp_rect( (Rect *)&pb->pb_xc, 1 );
  draw_icon( i_ptr, pb->pb_x, pb->pb_y, z->idat[j].label, z->idat[j].c,
      z->idat[j].state, icon_width(12) );
  set_clp_rect( &z->maximum, 1 );
  return(0);
}
/********************************************************************/
void cdecl blank_box( int *arr )
{
  x1y1arr( arr );
  pats( 0L, 1 );
  colbits(0);
  gr_box();
}
void window_box( int *arr )
{
  WIND_PRF w;
  
  w = z->wind_prf;
  x1y1arr( arr );
  pats( 0L, 1 );
  desk_color( w.s.interiorcol );
  (*graphics->vsf_interior)( w.s.fillpattern ? 2 : 0 );
  (*graphics->vsf_style)( w.s.fillpattern < 7 ? w.s.fillpattern : 8 );
  gr_box();
  (*graphics->vsf_interior)( 4 );
}
void sel_draw( Rect *rect, int hand, void func( Rect *r ) )
{
  int i;

  wind_update( BEG_UPDATE );
  select( *rect, hand );
  for( i=0; i<rects; i++ )
    (*func)( &list[i] );
  wind_update( END_UPDATE );
}
void drwmen( Rect *r )
{
  Rect r2;
  int arr[4];

  set_clp_rect( r, 1 );
  arr[2] = (arr[0] = r->x) + r->w - 1;
  arr[3] = (arr[1] = r->y) + r->h - 1;
  wmode0();
  hide_mouse();
  blank_box(arr);
  objc_draw( menu, 2, 1, Xrect(*r) );
  r2.x = r->x;
  r2.y = menu[0].ob_y+bar_h;
  r2.w = r->w;
  r2.h = 1;
  if( rc_intersect( r, &r2 ) )
  {
    x1y1val( r2.x, r2.y, r2.x+r2.w-1, 0 );
    set_lnmask( -1 );
    colbits(1);
    wmode0();
    (*graphics->gr_hline)();
  }
  show_mouse();
}
void draw_menu(void)
{
  Rect r;

  r.x = z->maximum.x;
  r.y = menu[0].ob_y;
  r.w = z->maximum.w;
  r.h = bar_h+1;
  sel_draw( &r, dtop_handle, drwmen );
}
void clear_menu(void)
{
  pxarray[0] = pxarray[1] = 0;
  pxarray[2] = z->cliparray[2];
  pxarray[3] = z->maximum.y - 2;
  set_clip( pxarray, 1 );
  wmode0();
  blank_box( pxarray );
}
void cdecl draw_box( int *box )
{
  wmode2();
  set_lnmask( 0x3333 );
  draw_bx( box );
}
/********************************************************************/
int cdecl draw_desk( PARMBLK *pb )
{
  int box[16], i, j;
  BITBLK *b;
  MFDB fdb0;
  unsigned char *ptr;
  Rect r;

  set_clp_rect( (Rect *)&pb->pb_xc, 1 );
  if( z->pic_ptr )
  {
    blit_init( &fdb0 );
    fdb0.fd_addr = (void *)z->pic_ptr;
    fdb0.fd_h = pb->pb_h;
    fdb0.fd_wdwidth = (fdb0.fd_w = z->desk[0].ob_width+1) / 16;
    fdb0.fd_nplanes = vplanes;
    fdb0.fd_r1 = fdb0.fd_r2 = fdb0.fd_r3 = 0;
    r.x = pb->pb_xc - z->desk[0].ob_x;
    r.y = pb->pb_yc - z->desk[0].ob_y + z->maximum.y;
    *(long *)&r.w = *(long *)&(pb->pb_wc);
    blitit( &r, (Rect *)&(pb->pb_xc), 3 );
  }
  else
  {
    if( !iconedit || !pb->pb_obj || (b = icic->pattern)==0L )
        b = deskpat->ob_spec.bitblk;
    pats( b->bi_pdata, 15 );
    desk_color( b->bi_color&0xf );
    x1y1val( pb->pb_x, pb->pb_y, pb->pb_x+pb->pb_w-1, pb->pb_y+pb->pb_h-1 );
    wmode0();
    gr_box();
    if( b->bi_color&0xf0 )
    {
      desk_color( b->bi_color>>4 );
      for( i=0; i<16; i++ )
	box[i] = ~b->bi_pdata[i];
      set_pattern( box );
      wmode1();
      gr_box();
    }
  }
  if( pb->pb_tree[pb->pb_obj].ob_flags & (1<<10) )
  {
    (*icic->offset_objc)( pb->pb_tree, pb->pb_obj, &box[0], &box[1] );
    box[2] = box[4] = (box[6] = box[0]) + pb->pb_tree[pb->pb_obj].ob_width-1;
    box[5] = box[7] = (box[3] = box[1]) + pb->pb_tree[pb->pb_obj].ob_height-1;
    wmode0();
    set_lnmask(-1);
    (*graphics->vsl_color)(BLACK); 	/* 003: was colbits(1); */
    draw_bx( box );
  }
  ptr = z->notes;
  i = z->notes_len;
  while( i>0 )
  {
    gtext2( *ptr*z->wind_font[4].w + z->desk[0].ob_x, *(ptr+1)*z->wind_font[4].h +
        z->desk[0].ob_y, ptr+2, FONT_NOTE, 0, 1-z->note_opaq, z->note_col );
    ptr += (j = 3 + strlen(ptr+2));
    i -= j;
  }
  return(0);
}
/********************************************************************/
char *file_text( FSTRUCT *f, int *is_grp )
{
  int i;
  
  if( (i=f->type.p.pexec_mode)==GROUP || i==NPI )
  {
    if( is_grp ) *is_grp = 1;
    return f->groupname;
  }
  if( is_grp ) *is_grp = 0;
  return f->name;
}
void draw_w_icon( NICONBLK *i_ptr, int num, int i, int bak )
{
  FSTRUCT *f;
  char *ptr;
  int w, arr[4];

  f = &z->file[num][i];
  if( ed_wind_type(num) == EDW_GROUP )
  {
    ptr = f->grp_item->name;
    w = icon_width(20);
  }
  else
  {
    ptr = file_text(f,0L);
    w = icon_width(12);
  }
  arr[0] = f->x;
  arr[1] = f->y[snum];
  if( bak && i_ptr->ci )
  {
    arr[2] = arr[0] + w;
    arr[3] = arr[1] + ICON_H;
    wmode0();
    window_box(arr);
  }
  draw_icon( i_ptr, arr[0], arr[1], ptr, 0, f->state|(f->read_only&2), w );
}
/********************************************************************/
void draw_image( int *data, int x, int y, int w, int h, int planes, int mode, int *cols, int x0, int y0 )
{
  int px[8];
  MFDB fdb2;
  static MFDB fdb0;

  fdb2.fd_addr = data;
  fdb2.fd_h = h;
  fdb2.fd_wdwidth = (fdb2.fd_w = w<<3) >> 4;
  fdb2.fd_nplanes = planes==1 ? 1 : vplanes;
  fdb2.fd_r1 = fdb2.fd_r2 = fdb2.fd_r3 = fdb2.fd_stand = 0;
  px[2] = (px[0] = x0) + fdb2.fd_w - 1;
  px[3] = (px[1] = y0) + h - 1;
  px[6] = (px[4] = x) + fdb2.fd_w - 1;
  px[7] = (px[5] = y) + h - 1;
  if( planes==1 ) vrt_cpyfm( vdi_hand, mode, px, &fdb2, &fdb0, cols );
  else vro_cpyfm( vdi_hand, mode, px, &fdb2, &fdb0 );
}
void draw_cicon( NICONBLK *nic, int ob_x, int ob_y, int state )
{
	int 	m_mode, i_mode, mskcol, icncol, planes;
	int     *mask, *data, *dark = 0L;
	int     mindex[2], iindex[2], buf;
	char    invert = 0, selected;
	Rect r2;
	ICONBLK *iconblk = nic->ib;
	CICON *cicn = nic->ci;

	selected = state & SELECTED;

        r2.x = ob_x;  /*+iconblk->ib_xicon+1;*/
        r2.y = ob_y;  /*+iconblk->ib_yicon+1; */
        r2.w = (iconblk->ib_wicon+7)>>3;
        r2.h = iconblk->ib_hicon;

        m_mode  = MD_TRANS;

        if( !cicn )
        {
          if( selected ) invert = 1;
          mask = iconblk->ib_pmask;
          data = iconblk->ib_pdata;
          planes = 1;
 	  i_mode = MD_TRANS;
        }
	else if (selected) /* it was an objc_change */
	{
		planes = cicn->num_planes;
		if ( cicn->sel_data )
		{
			mask = cicn->sel_mask;
			data = cicn->sel_data;
			if (planes > 1)
			{	if (planes > 8)	/* TrueColor, bzw RGB-orientierte Grafikkarte? */
					i_mode = S_AND_D;
				else
					i_mode = S_OR_D;
			}
			else
				i_mode = MD_TRANS;
		}
		else
		{
			mask = cicn->col_mask;
			data = cicn->col_data;

			if (planes > 1)
			{	if (planes > 8)
					i_mode = S_AND_D;
				else
					i_mode = S_OR_D;
				dark = cicn->sel_mask;
			}
			else
				invert = 1;
		}
	}
	else
	{
		planes = cicn->num_planes;
		mask = cicn->col_mask;
		data = cicn->col_data;

		if (planes > 1)
		{	if (planes > 8)
				i_mode = S_AND_D;
			else
				i_mode = S_OR_D;
		}
		else
			i_mode = MD_TRANS;
	}

	mindex[0] = !cicn || ((iconblk->ib_char & 0x0f00) != 0x0100) ?
	    (iconblk->ib_char & 0x0f00) >> 8 : WHITE;
	mindex[1] = WHITE;

	icncol = iindex[0] = (int)(((unsigned int)iconblk->ib_char & 0xf000U) >> 12U);
	iindex[1] = WHITE;

	mskcol = (iconblk->ib_char & 0x0f00) >> 8;

	if (invert)
	{
		buf       = iindex[0];
		iindex[0] = mindex[0];
		mindex[0] = buf;
		i_mode    = MD_TRANS;
	}
	if (selected)
	{
		buf    = icncol;
		icncol = mskcol;
		mskcol = buf;
	}

	draw_image (mask, Xrect(r2), 1, m_mode, mindex, 0, 0);
	draw_image (data, Xrect(r2), planes, i_mode, iindex, 0, 0);

	if (dark)
	{
		mindex [0] = BLACK;
		mindex [1] = WHITE;
		draw_image (dark, Xrect(r2), 1, MD_TRANS, mindex, 0, 0);
	}
}

void draw_icon( NICONBLK *i_ptr, int x, int y, char *text, unsigned int c,
		      int s, int w )
{
  int arr[4], old_mode, bx;
  static long patptr[] = { 0x5555AAAAL, 0x5555AAAAL, 0x5555AAAAL, 0x5555AAAAL,
			   0x5555AAAAL, 0x5555AAAAL, 0x5555AAAAL, 0x5555AAAAL };
  if( s==-1 ) s=1;
  bx = x + ((w-32)>>1) + 1;
  draw_cicon( i_ptr, bx, y, s );
  arr[2] = (arr[0] = x) + w + 1;
  arr[3] = (arr[1] = y + ICON_H) + z->wind_font[0].h+1;
  old_mode = wmode0();
  if( s&2 )
  {
    x1y1arr(arr);
    pats( (int *)&patptr, 1 );
    colbits(1);
    gr_box();
  }
  else blank_box( arr );
  gtext( arr[0]+(w>>1)+1, arr[1]+1, text, FONT_ICON, 1 );
  if(c)
  {
    wmode1();
    (*graphics->small_char)( bx + i_ptr->ib->ib_xchar, y + i_ptr->ib->ib_ychar, (s&1)?0:15, c );	/* 004: &1 */
  }
  if( s&1 )
  {
    wmode2();
    blank_box( arr );
  }
  set_wmode( old_mode );
}
/********************************************************************/
void cdecl ndraw_icon( NICONBLK *i_ptr, int x, int y, char *text, unsigned int c, int s )
{
  draw_icon( i_ptr, x, y, text, c, s, icon_width(12) );
}
/********************************************************************/
long get_cons( long l )
{
  if( clsizb[w_num]>1 && z->other_pref.b.consumption )
      return (l+clsizb[w_num]-1) / clsizb[w_num] * clsizb[w_num];
  return -1L;
}
void dtext_fmt( char *s, FSTRUCT *f, int wnum, int ssize, int sdate, int stime, int isfold )
{ /* s needs to be at least 12 + 20 + 120 + 4 big! */
  char tmp[40], c;
  int p, time[8], grp;
  long l;

  if( ed_wind_type(wnum)==EDW_GROUP )
  {
    if( group_desc[wnum]->hdr.opts.s.showtype ) spf( tmp, "%-12s ",
	(p=f->type.p.pexec_mode)<D_PROG ? icons[p+1].ob_spec.iconblk->ib_ptext :
	msg_ptr[138] );
    else tmp[0] = 0;
    spf( s, "%s%-20s", tmp, f->grp_item->name );
    if( group_desc[wnum]->hdr.opts.s.showpath ) spfcat( s, " %-120s",
	f->grp_item->p.path );
  }
  else
  {
    strcpy( tmp, file_text(f,&grp) );
    if( !grp )
    {
      c = tmp[p = find_extn(tmp)];
      tmp[p] = '\0';
      spf( s, "%c%-8s%c%-3s ", isfold, tmp, c ? '.' : ' ', c ? tmp+p+1 : nil );
    }
    else spf( s, " %-12s ", tmp );
    if( ssize )
    {
      spfcat( s, z->other_pref.b.long_numbers ? "%8D " : "%5N ", f->size );	/* 005: added %N's, changed format */
      if( (l=get_cons(f->size)) >= 0L )
      {
        spfcat( s, z->other_pref.b.long_numbers ? "%8D" : "%5N", l );
        *strrchr( s, ' ' ) = '\xf7';
        strcat( s, " " );
      }
    }
    to_tandd( f->date, time );
    if( sdate ) cat_date( s, time[4], time[3], time[5], 1 );
    if( stime ) spfcat( s, "%02d:%02d ", time[2], time[1] );
  }
}
/********************************************************************/
void draw_text( int i, int num, int back )
{
  int arr[4], old_mode;
  char s[156];
  FSTRUCT *f;
  WIND_PRF p;

  f = &z->file[num][i];
  dtext_fmt( s, f, num, z->sizdattim[num][0], z->sizdattim[num][1],
      z->sizdattim[num][2], f->type.p.pexec_mode==FOLDER ? '\7' : ' ' );
  arr[2] = (arr[0] = f->x) + max_icon.text_w - 1;
  arr[3] = (arr[1] = f->y[snum]) + max_icon.h - 1;
  p = z->wind_prf;
  if( back )
  {
    wmode0();
    window_box( arr );
  }
  gtext2( arr[0], arr[1], s, FONT_SMALL-z->stlgsml[num], 0,
      p.s.textmode?MD_REPLACE-1:MD_TRANS-1, p.s.textcol );
  if( f->state )
  {
    wmode2();
    blank_box( arr );
  }
}
/********************************************************************/
long dsetpath( char *path )
{
  char temp[120];
  long ret;
  DTA dma, *old;

  old = Fgetdta();
  Fsetdta(&dma);
  strcpy( temp, path );
  strcpy( temp+3, glob );
  if( (ret=cFsfirst( temp, 0x37 )) == 0 || ret==AEFILNF )
      ret = cDsetpath( path+2 );
  Fsetdta(old);
  return(ret);
}
/********************************************************************/
void blit_menu( int flag )
{
  static char have_it;
  Rect r;
  
  r.x = r.y = 0;
  r.w = z->maximum.w;
  r.h = bar_h;
  if( !flag )
  {
    if( x_graf_blit( (GRECT *)&r, 0L ) ) have_it = 1;
  }
  else if( have_it )
  {
    x_graf_blit( 0L, (GRECT *)&r );
    have_it = 0;
  }
}
char protect_keys( OBJECT *tree, int flag )
{
  /* protect the keyboard combos from being wiped out */
  if( flag && tree==menu && lowmenu )
  {
    tree[0].ob_state &= ~X_MAGIC;
    return 1;
  }
  return 0;
}
void _menu_bar( OBJECT *tree, int flag, int blit )
{
  char keys=0;
  
  if( !dtop_wind )
  {
    if( !z->is_acc )		/* 002 */
    {
      keys = protect_keys( tree, flag );
      menu_bar( tree, flag );
      if( keys ) tree[0].ob_state |= X_MAGIC;
    }
  }
  else if( blit ) blit_menu( flag );
  if( flag )
  {
    m_on = -1;
    macro_ind();
  }
}
void ic_menu_bar( OBJECT *tree, int flag )
{
  if( dtop_handle>0 ) wind_set( dtop_handle, X_WF_MENU, flag ? tree : 0L );
  else _menu_bar( tree, flag, 0 );
}
/********************************************************************/
long (*kbvec)(void);
long new_kbd(void)
{
  return 0L;
}
#define bconvec *(long (**)(void))(0x51e+8)
#pragma warn -rvl
long kb_off(void)
{
  kbvec = bconvec;
  bconvec = new_kbd;
}
long kb_on(void)
{
  bconvec = kbvec;
}
#pragma warn +rvl
int add_note( int len, char *str )
{
  if( !len ) len = strlen(str+2) + 3;
  if( !add_string( (void **)&z->notes, &z->notes_len, &z->notes_rem, str, 20, len, ALLOC_MAS ) )
  {
    f_alert1( msg_ptr[107] );
    return 0;
  }
  return 1;
}
void free_notes(void)
{
  cmfree( &z->notes );
  z->notes_len = 0;
}
int note_extent( char *ptr, int len )
{
  int ret;
  char c;
  
  if( len>=0 )
  {
    c = *(ptr+len);
    *(ptr+len) = 0;
  }
  ret = (*graphics->vqt_extent)(ptr,FONT_NOTE);
  if( len>=0 ) *(ptr+len) = c;
  return ret;
}
void edit_note( int x, int y )
{
  int x1, y1, off, arr[4], sync, llen, olen, i, w, h;
  unsigned char *ptr, *mptr=NULL, c, c1, quit=0, curs, redraw;
  long l;

  hide_mouse();
  menu_msg( msg_ptr[121] );
  gtext( 0, 0, "", FONT_NOTE, 0 );
  w = z->wind_font[4].w;
  h = z->wind_font[4].h;
  x1 = x-z->desk[0].ob_x;
  y1 = (y-z->desk[0].ob_y)/h;
  ptr = (unsigned char *)z->notes;
  off = z->notes_len;
  memclr( diskbuff, sizeof(diskbuff) );
  while( off>0 )
  {
    i = *ptr*w;
    if( *(ptr+1)==y1 && x1>=i && x1<i+note_extent(ptr+2,-1) )
    {
      mptr=ptr+2;
      break;
    }
    else
    {
      ptr += (llen = 3+strlen(ptr+2));
      off -= llen;
    }
  }
  if( !mptr )
    {
      x1 /= w;
      diskbuff[0] = x1;
      diskbuff[1] = y1;
      off = 0;
      *(mptr = diskbuff+2) = 0;
    }
  else
  {
    diskbuff[0] = *ptr;
    diskbuff[1] = *(ptr+1);
    i = strlen(mptr) + 3;
    memcpy( diskbuff+2, mptr, i-2 );
    memcpy( ptr, ptr+i, z->notes+z->notes_len-(ptr+i) );
    z->notes_len -= i;
    z->notes_rem += i;
    mptr = diskbuff+2;
    for( off=0, i=(unsigned char)diskbuff[0]*w-2; *(mptr+off) && i<x1; off++ )
      i += note_extent(mptr+off,1);
    if( off ) off--;
    x1 = (unsigned char)diskbuff[0];
  }
  x1 *= w;
  x1 += z->desk[0].ob_x;
  arr[3] = (arr[1] = y1*h+z->desk[0].ob_y) + h-1;
  redraw = 1;
  olen = strlen(mptr);
  while( !quit )
  {
    arr[2] = (arr[0] = x1+note_extent(mptr,off)) + 1;
    llen = strlen(mptr);
    if( redraw )
    {
      if( *mptr )
        if( !add_note( 0, diskbuff ) ) break;
      Supexec( kb_off );
      flush();
      redraw_desk( x1, arr[1], olen*w, h, 0 );
      Supexec( kb_on );
    }
    olen = llen;
    set_clp_rect( (Rect *)&z->desk[0].ob_x, 1 );
    wmode2();
    blank_box(arr);
    curs = 1;
    while( !Bconstat(2) )
    {
      sync = 0;
      while( !Bconstat(2) && ++sync<13 ) Vsync();
      blank_box(arr);
      curs ^= 1;
    }
    if( curs ) blank_box(arr);
    redraw = 1;
    switch( (char)((l=Bconin(2)|((long)Getshift()<<24))>>16) )
    {
      case 1:					/* esc */
	off = 0;
	mptr[0] = 0;
	break;
      case 0x4B:				/* left */
	if( l&0x3000000L ) off = 0;
	else if( off ) off--;
	redraw=0;
	break;
      case 0x4D:				/* right */
	if( l&0x3000000L ) off = llen;
	else if( off<llen ) off++;
	redraw=0;
	break;
      case 0x0E:				/* backsp */
	if( off )
	{
	  strcpy( mptr+off-1, mptr+off );
	  off--;
	}
        else redraw = 0;
	break;
      case 0x53:				/* delete */
	if( off<llen )
            strcpy( mptr+off, mptr+off+1 );
        else redraw = 0;
	break;
      case 0x1C:
      case 0x72:
	quit++;
	break;
      default:
        i = x1+note_extent(mptr,-1)+w-1;
	if( (char)l && i-z->desk[0].ob_x < z->desk[0].ob_width &&
	    i < z->maximum.w )
	{
	  memcpy( mptr+off+1, mptr+off, llen-off+1 );
	  mptr[off++] = (unsigned char)l;
	  olen++;
        }
        else redraw = 0;
    }
    if( redraw && llen )
    {
      i = llen + 3;
      z->notes_len -= i;
      z->notes_rem += i;
    }
  }
  if( *mptr ) add_note( 0, diskbuff );
  flush();
  menu_msg(0L);
  show_mouse();
}
/********************************************************************/
int fdelete( char *ptr, int flg, int num )
{
  char temp[120];

  if( flg )
    if( !TOS_error( (int)(num=cFattrib(ptr,0,0)), 0 ) ) return(0);
  if( num&1 )
  {
    spf( temp, msg_ptr[35], spathend(ptr) );
    num = f_alert1( temp );
    bee();
    switch(num)
    {
      case 1:
	if( !TOS_error( cFattrib(ptr,1,num&-2), 0 ) ) return(0);
	break;
      case 2:
	return(1);
      case 3:
	return(0);
    }
  }
  return( TOS_error( cFdelete(ptr), 0 ) );  /* also for case 1 */
}
/********************************************************************/
int find_d( int x0, int y0 )
{
  register int i, x, y, tw, ih, half;
  ICONSAVE *id;

  if( !reorder_on )
  {
    x0 -= z->desk[0].ob_x;
    y0 -= z->desk[0].ob_y;
    get_max_icon(-1);
    tw = max_icon.text_w + 2;
    ih = max_icon.h;
    half = ((max_icon.text_w - ICON_W) >> 1) + 1;
    for( id=z->idat+(i=z->num_icons)-1; --i>=0; id-- )
      if( id->type >= 0 )
      {
        x = x0 - z->desk[i+1].ob_x;
        y = y0 - z->desk[i+1].ob_y;
	if( (x >= 0 && x <= tw && y >= ICON_H && y <= ih) ||
	    (x >= half && x <= half + ICON_W && y>=0 && y <= ICON_H) )
  	    return i;
      }
  }
  return(-1);
}
/********************************************************************/
int find_extn( char *ptr )
{
  register int i;

  i = pathend(ptr);
  while( *(ptr+i) != '.' && *(ptr+i) ) i++;
  return(i);
}
/********************************************************************/
void find_handle(void)
{
  register int i;

/*  for( i=1; i<8; i++ )
    if( wxref[i].internal == w_num ) w_handle = wxref[i].aes_hand;*/
  w_handle = wxref[w_num];
}
/********************************************************************/
int find_w( int x0, int y0, int wind )
{
  register int j, x, y, num, w;
  int max, sn;
  FSTRUCT *fs;
  Rect r;

  w = (max_icon.text_w-ICON_W)>>1;
  if( (num=wind_xref(wind)) >= 0 )
  {
    get_max_icon(num);
    for( sn=!z->w[num].split; sn<(z->w[num].split>=0?2:1); sn++ )
      if( y0 >= ww[num][sn].y && y0 < ww[num][sn].y + ww[num][sn].h )
    {
      j = get_hilo( num, sn, &max );
      fs = &z->file[num][j];
      for( ; j<max; j++, fs++ )
      {
	x = x0 - fs->x;
	y = y0 - fs->y[sn];
	if( z->showicon[num] )
	{
	  if( (x < 0 || x > max_icon.text_w || y < ICON_H || y > max_icon.h) &&
	      (x < w || x > w+ICON_W || y < 0 || y > ICON_H) ) continue;
	}
	else if( x < 0 || x > max_icon.text_w || y<0 || y > max_icon.h ) continue;
	r.x = fs->x;
	r.y = fs->y[sn];
	r.w = max_icon.text_w;
	r.h = max_icon.h;
        wind_update( BEG_UPDATE );
	select( r, wind );
        wind_update( END_UPDATE );
	if( rects ) return(j);
      }
    }
  }
  return(-1);
}
/******************************************************************/
PROG_TYPE iprog_type( int wnum, char *str )  /* must get full path */
{
  PROG_TYPE type; /* can't return extended method because bits reused by
		     install app */

  type = prog_type( wnum, str );
  if( type.i == 0 ) type.p.pexec_mode = TEXT;
  else if( last_npi ) type.p.pexec_mode = NPI;
  else if( type.p.batch ) type.p.pexec_mode = BATCH;
  else type.p.pexec_mode = PROG;
  return(type);
}
/***********************************************************************/
int read_fat( unsigned int *cluster )
{
  register unsigned int ncl;
  int f, ret;
  union
  {
    unsigned char c[2];
    unsigned int i;
  } u;

  u.i = 0;
  if( (f = *cluster/ents_sec + bp->fatrec) != fat_sec )
    if( (ret=Rwabs( 2, fat, 2, f, dirdrv )) < 0 ) return(ret);
    else fat_sec = f;
  if( !(bp->bflags&1) )
  {
    f = (*cluster + (*cluster>>1)) % bp->recsiz;
    u.c[0] = fat[f+1];
    u.c[1] = fat[f];
    ncl = u.i;
    if( *cluster&1 ) ncl >>= 4;
    if( (ncl &= 0xFFF) >= 0xFF8 ) ncl = -1;
  }
  else
  {
    f = (*cluster % ents_sec) << 1;
    u.c[0] = fat[f+1];
    u.c[1] = fat[f];
    if( (ncl = u.i) >= 0xfff8 ) ncl = -1;
  }
  *cluster = ncl;
  return(0);
}

int dir_find( int flag )
{
  static unsigned int cl, max_cl;
  static char found, last=0, *ptr2, buf[12];
  static unsigned int cluster;
  int ret;

  if( using_qdir==1 )
  {
    buf[11] = '\0';
    ents_sec = ((bp->recsiz+1)<<3) / (bp->bflags&1?16:12);
    dir_sec = bp->fatrec + bp->fsiz;
    last = cluster = 0;
    max_cl = bp->rdlen;
    fat_sec = 0;
  }
  for(;;)
  {
    found=0;
    if( !last )
    {
      to_dirname( dirname, buf );
      if( (ptr2 = strchr(dirname,'\\')) != 0 ) dirname = ptr2+1;
      else last++;
      cl=0;
    }
    else if( using_qdir>1 ) goto incr;
    using_qdir = 2;
    while( !found )
    {
      for( ; cl<max_cl && !found; cl++ )
      {
	if( (ret=Rwabs( 2, sec_buf, 1, dir_sec, dirdrv )) < 0 ) return(ret);
	de = (Dirent *)sec_buf;
	while( (char *)de-sec_buf < bp->recsiz && !found )
	{
	  if( de->name[0] )
	  {
	    if( (int)(de->name[0]) == 0xE5 )
	    {
	      if( last && flag ) return(0);	/* looking for empties? */
	    }
	    else
	      if( last ) return(0);
	      else if( !strncmp( buf, de->name, 11 ) ) found++;
	  }
	  else return(last?AENMFIL:AEPTHNF);
incr:	  if( !found ) de++;
	}
	if( !found ) dir_sec++;
      }
      if( cluster )
      {
	if( read_fat( &cluster ) < 0 ) return(-1);
	if( cluster==0xffff ) goto nfnd;	    /* end of file */
	dir_sec = (cluster-2) * bp->clsiz + bp->datrec;
	cl=0;
      }
      else if( !found ) return(last?AENMFIL:AEPTHNF);
    }
nfnd:
    if( found )
      if( de->att & S_IJDIR )
      {
	dir_sec = ((cluster=canw(de->cluster))-2) * bp->clsiz + bp->datrec;
	max_cl = bp->clsiz;
	cl = 0;
      }
      else return(AEACCDN);
    else return(last?AENMFIL:AEPTHNF);
  }
}

void end_qdir(void)
{
  if( using_qdir )
  {
    if( mch==2 ) *(mas->bad_media) = dirdrv;
    if( sec_buf != diskbuff ) lfree(sec_buf);
    using_qdir = 0;
  }
}
int cfFsnext(void)
{
  int ret;
  DTA *dta;

  if( using_qdir )
  {
again:
    if( (ret=dir_find(reordering)) != 0 ) end_qdir();
    else if( dir_att & de->att || !de->att ||
	reordering && (int)de->name[0]=='\xE5' )
    {
      if( (reordering || reord_buf) && de->name[0]=='.' ) goto again;
      if( !reordering )
      {
	dta = Fgetdta();
	dta->d_attrib = de->att;
	dta->d_time = canw(de->time);
	dta->d_date = canw(de->date);
	dta->d_length = ((unsigned long)canw((int)de->len)<<16) |
            (unsigned)canw((int)(de->len>>16));
	from_filename( de->name, dta->d_fname, 1 );
      }
    }
    else goto again;
    return(ret);
  }
  return(cFsnext());
}
int getbpb( char *path, int mult )
{
  if( is_locked(*path) )
  {
    TOS_error( LOCK_ERR, 0 );
    return -1;
  }
  mch = Mediach( dirdrv=(*path-'A') );
  bp = Getbpb(dirdrv);
  if( !bp )
  {
    if( mch==2 ) *(mas->bad_media) = dirdrv;
    return -1;
  }
  if( mult<0 )
  {
    if( bp->recsiz <= sizeof(diskbuff) )
    {
      sec_buf = diskbuff;
      return 0;
    }
    mult = 0;
  }
  if( (sec_buf = lalloc((long)(bp->recsiz<<mult),-1)) != 0 ) return 0;
  bp = 0L;
  return 1;
}
int cfFsfirst( char *path, int att )
{
  int ret;

  if( (reorder_on || reord_buf) && *path!=CLIP_LET )
  {
    if( (ret=getbpb( path, 2 )) == 0 )
    {
      using_qdir = 1;
      dirname = path+3;
      fat = sec_buf + (bp->recsiz<<1);
      dir_att = att;
      return( (ret=cfFsnext()) == AENMFIL ? AEFILNF : ret );
    }
    if( ret<0 ) return(-1);
    if( reorder_on || reord_buf ) return(AENSMEM);
  }
  return( cFsfirst(path,att) );
}
/***********************************************************************/
int (*pnmatch)( char *a, char *b );
void set_pnmatch(void)
{
  pnmatch = !z->other_pref.b.check_fnames ? match2 : match;
}
int lmatch( char *str, char *pat, int len )
{
  char s, p, per=0, not;

  for(;;)
  {
    if( !len-- ) return 1;
    if( (p = *pat++) == '\0' )
      if( *str ) return 0;
      else return 1;
    s = *str++;
    if( p == '*' )
    {
      if( !s ) return 1;
      str--;
      do
      {
	if( *str == '.' ) per=1;
	if( lmatch( str, pat, len ) ) return 1;
      }
      while( *str++ && --len );
      if( *pat++ != '.' ) return 0;
      if( *pat == '*' )
	if( !*(pat+1) ) return 1;
	else return 0;
      else if( *pat || per ) return 0;
      return 1;
    }
    else if( p == '?' )
    {
      if( !s ) return 0;
    }
    else if( p == '!' )
    {
      not = *pat++;
      if( lmatch( --str, pat, not ) ) return 0;
      if( !*(pat += not) ) return 1;
      str += not;
    }
    else if( p!=s ) return 0;
  }
}
int lpnmatch( char *str, char *pat )
{
  char buf[40], *ptr, *ptr2, *ptr3, not=0;
  int i, j;

  for( ptr=buf, j=0; *pat && j<sizeof(buf)-1; pat++ )
    if( *pat == '[' )
    {
      ptr2 = ++pat;
      while( *pat && *pat != ']' ) pat++;
/*	if( !*pat )
      {
	ferrs( SYNTAX, miss, ']' );
	return(-1);
      } */
      if( not )
      {
	*ptr++ = 1;
	j++;
      }
      ptr3 = ptr;	/* char to replace */
      while( (*++ptr = *++pat) != 0 && ++j<sizeof(buf)-1 );	/* copy rest */
      if( *ptr ) *++ptr = '\0'; /* end if too long */
/*	if( not && !*(ptr3+1) ) *(ptr3-1) = 2;	/* inc len if null after char */*/
      while( *ptr2 != ']' )
      {
	*ptr3 = *ptr2++;
	if( lpnmatch( str, buf ) ) { if( !not ) return 1; }
	else if( not ) return 0;
	if( *ptr2 == '-' )
	{
	  ptr2++;
	  while( ++(*ptr3) <= *ptr2 )
	    if( lpnmatch( str, buf ) ) { if( !not ) return 1; }
	    else if( not ) return 0;
	  ptr2++;
	}
      }
      return(not);
    }
    else if( *pat == '{' )
    {
      if( not )
      {
	*ptr++ = 1;	/* temporary length */
	j++;
      }
      for(;;)
      {
	ptr3 = ptr;
	i = j;
	while( *++pat && *pat!=',' && *pat != '}' && ++i<sizeof(buf)-1 )
	  *ptr3++ = *pat;
/*	  if( !*pat )
	{
no_brace: ferrs( SYNTAX, miss, '}' );
	  return(-1);
	} */
	if( not ) *(ptr-1) = i-j;	/* set length of not */
	ptr2 = pat;			/* skip to end of regexp */
	while( *ptr2 && *ptr2 != '}' ) ptr2++;
/*	  if( !*ptr2 ) goto no_brace; */
	while( (*ptr3++ = *++ptr2) != 0 && ++i<sizeof(buf)-1 ); /* copy rest */
	if( *ptr3 ) *++ptr3 = '\0';	/* string too large: end it */
/*	  if( not && !*(ptr + *(ptr-1)) ) *(ptr-1) += 1; /* ! for null at end */ */
	if( lpnmatch( str, buf ) ) { if( !not ) return 1; }
	else if( not ) return 0;
	if( *pat == '}' || !*pat ) return(not);
      }
    }
    else
    {
      if( not )
      {
	*ptr++ = 1;
	j++;
	not=0;
	if( *pat=='!' ) continue;
      }
      else if( *pat=='!' ) not=1;	/* copy ! in next line */
      *ptr++ = *pat>='a' && *pat<='z' ? (*pat&0x5f) : *pat;
      if( not && *(pat+1)<13 )	/* already know length */
      {
	*ptr++ = *++pat;
	j++;
	not=0;
      }
      j++;
    }
  *ptr = '\0';
  return( lmatch( str, buf, -1 ) );
}
/***********************************************************************/
void get_filt_templ( FILT_TYPE *f, char *temp )
{
  int i, count;

  if( f->flags.s.templates )
    if( f->flags.s.use_long )
      for( i=0, temp[0]=0; i<3; i++ )
	strcat( temp, f->long_tmpl[i] );
    else
    {
      strcpy( temp, "{" );
      for( i=count=0; i<6; i++ )
	if( f->flags.s.use_temp&(1<<i) )
	{
	  if( count++ ) strcat( temp, "," );
	  strcat( temp, z->template[i] );
	}
      if( !count ) temp[0] = 0;
      else if( count==1 ) strcpy( temp, temp+1 );
      else strcat( temp, "}" );
    }
  else temp[0] = 0;
}
int cmp_dt( unsigned int num, char use, unsigned int *arr )
{
  switch( use )
  {
    case 0:
      if( num <= arr[0] ) return 0;
      break;
    case 1:
      if( num >= arr[1] ) return 0;
      break;
    case 2:
      if( num != arr[2] ) return 0;
      break;
  }
  return 1;
}
int filter_it( FILT_TYPE *f, DTA *dta, char *templ )
{
  unsigned long l;

  if( dta->d_fname[0] == '.' && (!dta->d_fname[1] ||
      dta->d_fname[1] == '.') ) return 0;
  if( reorder_on ) return 1;
  if( f->flags.s.allfold && dta->d_attrib&S_IJDIR ) return 1;
  if( dta->d_attrib&(S_IJHID|S_IJSYS) ) return 0;
  if( f->flags.s.attrib && (dta->d_attrib&f->mask)!=f->mask ) return 0;
  if( templ[0] && !lpnmatch( dta->d_fname, templ ) ) return 0;
  if( f->flags.s.size )
  {
    l = dta->d_length;
    switch( f->use_size )
    {
      case 0:
	if( l <= f->sizes[0] ) return 0;
	break;
      case 1:
	if( l >= f->sizes[1] ) return 0;
	break;
      case 2:
	if( l != f->sizes[2] ) return 0;
	break;
    }
  }
  if( f->flags.s.date && !cmp_dt( dta->d_date, f->use_date, f->dates ) ) return 0;
  if( f->flags.s.time && !cmp_dt( dta->d_time, f->use_time, f->times ) ) return 0;
  return 1;
}
GROUP_HDR gh;
int has_GRP( char *temp )
{
  char *ptr;

  if( (ptr = strchr( spathend(temp), '.' )) == 0 ||
      strcmp( ptr, ".GRP" ) ) return 0;
  return 1;
}
int is_group( FSTRUCT *fs, char *temp, int close )
{
  int i, ret=0;

  if( fs ) fs->groupname[0] = 0;
  if( !has_GRP(temp) ) return 0;
  if( (i=cFopen(temp,2)) > 0 )
  {
    if( cFread( i, sizeof(gh), &gh ) == sizeof(gh) &&
	(gh.ver==GROUP_VER || gh.ver==GROUP_VER-1) )
    {
      if( fs ) strcpy( fs->groupname, gh.name );
      if( !close ) return i;
      ret = 1;
    }
    else if( !fs )
    {
      cFclose(i);
      spf( tmpf, msg_ptr[56], ".GRP" );
      f_alert1(tmpf);
      return -1;
    }
    cFclose(i);
  }
  else if( !fs )
  {
    TOS_error( i, 0 );
    ret = -1;
  }
  return ret;
}
char filt_templ[sizeof(z->filter[0].long_tmpl)];
int add_file( int num )
{
  int i;
  
  if( !add_thing( (void **)&z->file[num], &z->num_files[num],
      &z->files_rem[num], 0L, 25, sizeof(FSTRUCT), num ) ) return -1;
  return z->num_files[num]-1;
}
int first0( char *path, int err )
{
  int i, j, filenum=0, is_fold;
  FSTRUCT *fs;
  char *end, *ptr, temp[120];
  PROG_TYPE type;

  Fsetdta( &maintree.tree[maintree.tree_lev = 0] );
  total[w_num] = consume[w_num] = 0L;
  end = spathend(path);
  strcpy( end, glob );
  get_filt_templ( &z->filter[w_num], filt_templ );
  over_112[w_num] = 0;
  clsizb[w_num] = 1;
  j = 0;
  strcpy( temp, path );
  if( !err )	/* 003: used to do cfFsfirst & test here */
  {
    err = cfFsfirst( path, reorder_on ? 0x3F : 0x31 );
    if( (err==IEFILNF || err==IEPTHNF || TOS_error((long)err,0)) &&
        z->other_pref.b.virus_check && path[0]!=CLIP_LET && !reorder_on /*&&
	!is_MetaDOS(path[0]-'A')*/ )
    {
      if( (i = readboot( path[0] )) == 0 )  /* != AENSMEM */
/*        if( TOS_error( (long)i, 0 ) )*/
          for( i=0; i<256; i++ )
            j += *((int *)diskbuff+i);
      if( bp ) clsizb[w_num] = bp->clsizb;
    }
    else if( !err/*003*/ && path[0]!=CLIP_LET && (using_qdir || !getbpb(path,-1)) )
    {
      if( !using_qdir )
      {
        using_qdir++;
        end_qdir();
      }
      if( bp ) clsizb[w_num] = bp->clsizb;
    }
    if( !err ) do
      if( filter_it( &z->filter[w_num], &maintree.tree[0], filt_templ ) )
      {
	if( (filenum = add_file(w_num)) < 0 ) over_112[w_num] = 1;
	else
	{
	  fs = &z->file[w_num][filenum];  /* can change during add */
	  strcpy( fs->name, maintree.tree[0].d_fname );
	  is_fold = maintree.tree[0].d_attrib & S_IJDIR;
	  fs->groupname[0]=0;
          strcpy( temp+(end-path), fs->name );
	  if( !is_fold && strstr( fs->name, ".GRP" ) ) is_group( fs, temp, 1 );
	  if( is_fold ) type.p.pexec_mode = FOLDER;
	  else if( fs->groupname[0] ) type.p.pexec_mode = GROUP;
	  else
	  {
	    type = iprog_type( w_num, temp );
	    if( type.p.pexec_mode == NPI )
  	        strcpy( fs->groupname, last_npi->npi.name );
  	  }
	  if( z->showicon[w_num] ) get_icon_match( temp, fs->name, &fs->nib,
	      type.p.pexec_mode );
	  fs->type = type;
	  fs->state = 0;
	  fs->date = ((unsigned long)maintree.tree[0].d_date<<16) | maintree.tree[0].d_time;
	  fs->read_only = maintree.tree[0].d_attrib;
	  total[w_num] += fs->size = maintree.tree[0].d_length;
	  consume[w_num] += get_cons(fs->size);
	}
	if( reord_buf ) memcpy( reord_buf + (long)filenum*sizeof(Dirent),
	    de, sizeof(Dirent) );
      }
    while( !over_112[w_num] && !cfFsnext() );
  }
  exe_boot[w_num] = j==0x1234;
  set_wfile();	/* z->file may have changed, so reset wfile */
  know_icons[w_num] = z->showicon[w_num];
  dir_items = witems = items[w_num] = z->num_files[w_num];
  valid_path = 0;
  *end = '\0';
  if( !err || err == AEFILNF || err == AEPTHNF )
  {
    cDsetdrv(path[0]-'A');
    if( dsetpath( path ) != AEPTHNF )
    {
      valid_path++;
      if( !reorder_on ) sort();
    }
  }
  else valid_path++;
  if( w_num==num_w_active ) no_wactive();
  if( w_handle==oth_hand_w ) oth_w_active = -1;
  if( using_qdir ) end_qdir();
  return(err);
}
/******************************************************************/
void set_boot(void)
{
  hide_if( wtree[w_num], new_winobj+5, exe_boot[w_num] );
}
int first(int err)
{
  register int i, j, n;
  char *end, *ptr, *ptr2;
  FSTRUCT *fs, f;
  GROUP_ITEM *g;

  ptr = z->w[w_num].path;
  end = spathend(ptr);
  if( !first_no_close ) close_wforms( w_num, 1 );
  free_files(w_num);
  free_npis(w_num);
  clsizb[w_num] = 1;
  switch( ed_wind_type(w_num) )
  {
    case EDW_GROUP:
      if( group_desc[w_num]->hdr.name[0] )
         strcpy( end, group_desc[w_num]->hdr.name );  /* not search results */
      over_112[w_num] = 0;
      for( i=0, j=0, g=group_start[w_num];
	  i < group_desc[w_num]->hdr.entries; i++, g=g->next )
	if( (n = add_file(w_num)) < 0 )
	{
	  over_112[w_num] = 1;
	  break;
	}
	else
	{
	  fs = &z->file[w_num][n];
	  strcpy( fs->name, g->name );
	  strcpy( fs->groupname, g->name );
	  fs->name[sizeof(g->name)] = 0;
	  fs->read_only = 0;
	  fs->type = g->p.type;
	  fs->state = 0;
	  if( fs->type.p.pexec_mode == NPI )
	  {
	    ptr = g->p.path;
	    ptr2 = spathend(ptr);
            prog_type( w_num, ptr );	/* 002: just read NPI now, match does the rest */
	  }
	  else ptr2 = spathend(g->p.path);  /* 002: was fs->name */
	  get_icon_match( ptr, ptr2, &g->nicon, fs->type.p.pexec_mode );
	  fs->nib = g->nicon;
	  fs->grp_item = g;
	  if( g->y > j ) j = g->y;
	}
      set_wfile();	/* z->file may have changed, so reset wfile */
      group_rows[w_num] = (j+max_icon.h+15)>>4;
      witems = items[w_num] = i;
      if( !z->showicon[w_num] ) sort();
      goto new;
    default:		/* file of icons */
      i = 0;
      while( (*icic->get_icon)( w_num, i, &f ) )
        if( (j = add_file(w_num)) < 0 ) over_112[w_num] = 1;
	else
	{
	  f.grp_item = 0L;
	  memcpy( &z->file[w_num][j], &f, sizeof(FSTRUCT) );  /* can change during add */
	  i++;
	}
      items[w_num] = i;
      set_wfile();	/* z->file may have changed, so reset wfile */
new:  exe_boot[w_num] = 0;
      know_icons[w_num] = 1;
      if( w_num==num_w_active ) no_wactive();
      valid_path++;
      break;
    case EDW_DISK:
      bee();
      err = first0( ptr, err );
      if( !err && reorder_on ) strcpy( end, msg_ptr[105] );
      else if( !filt_templ[0] ) strcpy( end, glob );
      else if( strlen(filt_templ)+(end-ptr)<120 ) strcpy( end, filt_templ );
      else *end = 0;
  }
  set_boot();
  if( wind_xref(w_handle) >= 0 )
  {
    rdrw_all();
    redraw_arrows();
  }
  else
  {
    set_window();
    s_reset_icons();
  }
  arrow();
  return(err);
}
/********************************************************************/
int format_sec( char *ptr, int drive, int spt, int trak, int s, int twst )
{		/* initialize scnum before calling!! */
#ifndef DEMO
  register int i, j, k=0;
  int skew[20];
  register long ll;
  char err=0;
  static char start[5] = { 3, 5, 7, 9, 1 };
  ERRSTRUC errstruc[1];

  errstruc[0].num = IEBADSF;
  errstruc[0].str = msg_ptr[17];
  if( (err = _abort()) != 0 ) return(2);
  if( twst==1 )
  {
    if( /*   !trak && !s || */	 scnum > 4 ) scnum = 0;
    j = start[scnum++];
    for( i=0; i<20; i++ )
    {
      skew[i] = j;
      if( ++j > spt ) j = 1;
    }
  }
  do
  {
    ll = Flopfmt(ptr, skew, drive-'A', spt, trak, s,
	twst==1?-1:(twst==2?11:1), MAGIC, 0xE5E5 );
    if( !ll && *(int *)ptr ) ll = AEBADSF;
    if( ll == AEWRPRO ) err = f_writepro();
  }
  while( ll && ++k<2 && !err );
  if( ll != AEWRPRO && !err ) err = !TOS_error( ll, 1, errstruc );
  return(err);
#else DEMO
  demo_version();
  return(1);
#endif DEMO
}
/********************************************************************/
void get_d_icon( int ind )
{
  register int i, t;

  if( ind < 0 )
  {
    for( i=0; i<z->num_progs; i++ )
      get_desktop_match(i);
    for( i=0; i<z->num_icons; i++ )
      if( (t=z->idat[i].type) <= RAMDISK )
          get_icon_match( &z->idat[i].c, 0L, &z->idat[i].nicb, t );
      else if( t<D_PROG ) z->idat[i].nicb = &nic_icons[t].nicb;
  }
  else get_desktop_match( ind );
}
/*********/
void get_desktop_match( int ind )
{
  register char *ptr, *ptr2;
  char temp[9];
  PROG_TYPE *typ;
  int i;

  if( iconedit )
  {
    if( !ind ) z->programs[0].p.nib = icic->icon_nib;
  }
  else if( (typ=&z->programs[ind].p.type)->p.pexec_mode != 0 )
  {
    ptr = z->programs[ind].p.path;
    ptr2 = spathend(ptr);
    if( typ->p.pexec_mode != FOLDER && typ->p.pexec_mode != GROUP )
	*typ = iprog_type( -1, ptr );
    get_icon_match( ptr, ptr2, &z->programs[ind].p.nib, typ->p.pexec_mode );
    if( !strcmp( ptr2+find_extn(ptr2), ".ACC" ) )
      for( z->programs[ind].is_acc=i=0; i<MAX_NEO_ACC; i++ )
      {
	strcpy( temp, z->neo_acc[i] );
	ptr = strchr(temp,' ');
	if( ptr != temp )
	{
	  if( ptr ) *ptr='\0';
	  if( !strncmp( ptr2, temp, strlen(temp) ) )
	      z->programs[ind].is_acc = i+1;
	}
      }
  }
}
/********************************************************************/
NPI_DESC *find_npi( char *path )
{
  int i;
  NPI_DESC *n;
  
  for( i=0; i<8; i++ )
    for( n=npi_start[i]; n; n=n->next )
      if( !strcmp( n->path, path ) ) return n;
  return 0L;
}
int npi_type( char *path, char *name )
{
  char temp[120];
  NPI_DESC *n;
  
  if( name )
  {
    strcpy( temp, path );
    iso(temp);
    strcat( path=temp, name );
  }
  if( (n=find_npi(path)) == 0 ) return TEXT;
  if( n->npi.pt.i == 0 ) return TEXT;
  if( n->npi.pt.p.batch ) return BATCH;
  return PROG;
}
void get_icon_match( char *path, char *str, NICONBLK **nib, int type )
{
  register int k, att;
  ICONBUF *ib;
  NICONBLK *icb;
  FSTRUCT f;
  char temp[13], *is;

  if( type > RAMDISK )		/* 003 */
  {
    if( type == NPI ) type = npi_type(path,str);
    att = type==FOLDER ? ICON_FOLD : ICON_FILE;
    if( !strchr(str,'.') )
    {
      strcpy( temp, str );
      strcat( str=temp, "." );
    }
    set_pnmatch();
  }
  for( k=0, is=icon_buf, ib=nic_icons+D_PROG-1; k<user_icons; k++, ib++, is+=13 )
    if( type<=RAMDISK ? (ib->icb.ib_char&ICON_DRIVE && *path==*is) :
        ((ib->icb.ib_char^3)&att && (*pnmatch)( str, is )) )
    {
      *nib = &ib->nicb;
      return;
    }
  *nib = &nic_icons[type].nicb;
}
/********************************************************************/
void get_icn_matches( int num )
{
  register int i, j;
  FSTRUCT *fs;
  char *temp;

  bee();
  temp = z->w[num].path;
  for( i=0, fs=z->file[num]; i<items[num]; i++, fs++ )
    get_icon_match( temp, fs->name, &fs->nib, fs->type.p.pexec_mode );
  know_icons[num] = 1;
  arrow();
}
void get_all_icons(void)	/* only used by icon editor */
{
  int oh;

  get_icon_names();	/* 003: reversed with below, since d_icon depends on icon_buf now */
  get_d_icon(-1);
  oh = w_num;
  for( w_num=0; w_num<7; w_num++ )
    if( (w_handle = wxref[w_num]) >= 0 && ed_wind_type(w_num) == EDW_DISK )
    {
      set_wfile();
      set_window();
      get_icn_matches(w_num);
      rdrw_all();
    }
  w_handle = wxref[w_num=oh];
  set_wfile();
}
/*******************************************************************/
int get_drive( int ind )
{
  register char c;

  if( z->idat[ind-1].type == CLIPBRD ) return(CLIP_LET);
  else if( (c=z->idat[ind-1].c) >= 'A' && c <= 'Z' )
    if( drvmap() & (1L << c-'A') ) return( c );
    else f_alert1( msg_ptr[19] );
  else f_alert1( msg_ptr[20] );
  return(0);
}
/********************************************************************/
void get_full_name( char *buf, int num, int wind )
{
  if( ed_wind_type(wind)==EDW_GROUP ) strcpy( buf,
      z->file[wind][num].grp_item->p.path );
  else
  {
    strcpy( buf, z->w[wind].path );
    iso(buf);
    strcat( buf, z->file[wind][num].name );
  }
}
/********************************************************************/
void wind_enable( int index, int on )
{
  int i;

  for( i=0; i<7; i++ )
    if( z->w[i].place>0 ) menu_enable( i, index, on );
}
/********************************************************************/
void get_top_wind(void)
{
  register int i, j;
  int open, show, app=0, npi=0, load=0, searchdr=reorder_on^1, etype;
  register char c, *ptr;
  FSTRUCT *fs;

  w_num = -1;
  if( (w_handle=wget_top()) > 0 )
    if( (w_num = wind_xref(w_handle)) < 0 )
    {
      w_handle = -1;
      for( i=0; i<7; i++ )
	if( (j=wxref[i]) >= 0 && z->w[i].place == w_open )
	{
	  w_handle = j;
	  w_num = i;
	  break;
	}
    }
  if( w_num>=0 ) set_wfile();
  etype = ed_wind_type(w_num);
  if( (open = (d_active>=0 && d_active<MANY_ACTIVE && (i=z->idat[d_active].type) !=
      TRASH)) != 0 )
    if( i >= D_PROG )
    {
      ptr = z->programs[i-=D_PROG].p.path;
      i = z->programs[i].p.type.p.pexec_mode;
      if( *ptr != CLIP_LET && (i == PROG || i == BATCH || i == NPI) ) app++;
      ptr = find_extn(ptr) + ptr;
      if( !strcmp( ptr, ".NPI" ) ) npi++;
      for( i=0; i<3; i++ )
	if( !strcmp( ptr, ext[i] ) ) load++;
    }
  if( d_active==MANY_ACTIVE )
  {
    for( i=0; i<z->num_icons; i++ )
      if( (j=z->idat[i].type) >= 0 )
	if( z->idat[i].state&1 && j>CLIPBRD && j!=FOLDER )
	{
	  searchdr=0;
	  break;
	}
  }
  else searchdr = d_active>=0 && d_active<MANY_ACTIVE &&
      (z->idat[d_active].type <= CLIPBRD || z->idat[d_active].type == FOLDER);
  if( d_active<0 && w_active<0 )
  {
    npi = 2;
    if( !app ) for( i=0; i<z->num_apps; i++ )
      if( z->apps[i].type.i ) app += 2;
  }
  else if( !reorder_on ) open++;
  if( w_active>=0 && w_active<MANY_ACTIVE && !reorder_on )   /* only one window icon selected */
  {
    i = (fs=&z->file[num_w_active][w_active])->type.p.pexec_mode;
    if( z->w[num_w_active].path[0] != CLIP_LET )
      if( i==PROG || i==BATCH || i==NPI ) app++;
    ptr = find_extn(ptr = fs->name) + ptr;
    if( !strcmp( ptr, ".NPI" ) || i==NPI/*004*/ ) npi++;
    for( i=0; i<3; i++ )
      if( !strcmp( ptr, ext[i] ) ) load++;
  }
  menu_enable( -1, OPEN, open );
  wind_enable( WIMOPEN, open );
  menu_enable( -1, INSTAPP, app );
  menu_enable( -1, PROGINFO, npi );
  menu_enable( -1, SHOWINF, show = (d_active>=0 || w_num>=0 && etype!=EDW_ICONS) && !in_showinf &&
      (w_active<0 || ed_wind_type(num_w_active)<=EDW_DISK) && !reorder_on/*004*/ );	/* 002 */
  menu_enable( -1, QUICKINF, show );
  wind_enable( WIMSHOW, show );		/* 003 */
  wind_enable( WIMQUICK, show );	/* 003 */
  menu_enable( -1, DELITEM, open );
  menu_enable( -1, LOADINF, load );
  wind_enable( WIMLOAD, load );
  menu_enable( -1, SEARCHDR, i = searchdr || w_num>=0 && !reorder_on && etype==EDW_DISK/*002*/ );
  wind_enable( WIMSRCH, i );
  wind_enable( WIMAPP, app );
  wind_enable( WIMNPI, npi );
  wind_enable( -GWIMCHNG, 0 );
  wind_enable( WIMDEL, open );
  menu_enable( num_w_active, -GWIMCHNG, 1 );
  for( i=7; --i>=0; )
    if( z->w[i].place>0 ) menu_enable( i, CLOSEFLD, pathend(z->w[i].path) > 3 && !reorder_on );
  menu_enable( -1, EXCHMAC, !z->macr_rec && z->macro && (*z->macro || *(z->macro+1)) );
  if( etype==EDW_GROUP ) menu_enable( w_num, -GWIMSNAP, z->showicon[w_num] /* 003 */ );
  if( iconedit ) (*icic->enable_menu)(w_num);
  text_menu_check();
  get_widths();
}
/********************************************************************/
void get_volname( char *path, int flag )
{
  register int i, j;
  int onum, ohand, which[7], one=0;
  char temp[3], *p;
  ICONSAVE *is;

  maintree.tree_stat = 0L;
  if( w_num>=0/*002*/ && (j=ed_wind_type(w_num)) != EDW_DISK )
  {
    strcpy( w_volname[w_num], j==EDW_ICONS ? icic->icon_str :
         (j==EDW_GROUP ? group_desc[w_num]->hdr.desc[0] : nil) );
    if( flag ) volname_redraw();
    return;
  }
  for( i=0; i<7; i++ )
    one += which[i] = wxref[i]>=0 && *path==z->w[i].path[0] &&
	ed_wind_type(i)==EDW_DISK;
  if( one || !flag )
  {
    bee();
    temp[0] = *path;
    temp[1] = ':';
    temp[2] = '\0';
    treeini0(temp,&maintree);
    if( maintree.tree_stat==LOCK_ERR ) TOS_error( LOCK_ERR, 0 );
    if( flag )
    {
      onum = w_num;
      ohand = w_handle;
      for( i=7; --i>=0; )
	if( which[i] )
	{
	  strcpy( w_volname[w_num=i], volname );
	  w_handle = wxref[i];
	  volname_redraw();
	}
      w_handle = ohand;
      w_num = onum;
      arrow();
    }
    else if( w_num >= 0 ) strcpy( w_volname[w_num], volname );
    for( i=0, is=z->idat; i<z->num_icons; i++, is++ )	/* 004 */
      if( is->type>=0 && is->type<=RAMDISK && is->c == temp[0] && is->state&4 )
      {
        if( *(p = volname) == 0 ) p = icons[is->type+1].ob_spec.iconblk->ib_ptext;
        if( strncmp( p, is->label, 12 ) )
        {
          strncpy( is->label, p, 12 );
          is->label[12] = 0;
          rmv_icon_redraw(i);
        }
      }
  }
}
void icon_volnames(void)	/* 004 */
{
  int onum, i;
  ICONSAVE *is;
  char c;

  onum = w_num;
  w_num = -1;
  for( i=z->num_icons, is=z->idat; --i>=0; is++ )
    if( (c=is->c)>='A' && c<='Z' && (drvmap()&(1L<<(long)(c-'A'))) != 0 &&	/* 005: long shift */
        is->type>=0 && is->type<=RAMDISK && is->state&4 ) get_volname( &is->c, 0 );
  arrow();
  w_num = onum;
}
/***********************************************************************/
void info(void)
{
  if( w_num >= 0 )
  {
    info_text();
    info_redraw();
  }
}
/********************************************************************/
void txt_cons( long cons, char *str2 )
{
  if( clsizb[w_num]>1 && z->other_pref.b.consumption ) spf( str2, " (%N)", cons );
  else str2[0] = 0;
}
void info_text(void)
{
  register int i, itms=0;
  char ed_icons, ed_group;
  int time[6];
  register unsigned long size = 0L, cons = 0L;
  char str[25], temp[150], str2[30], srch[25];
  FSTRUCT *fs, *fs2;

  if( w_num >= 0 )
  {
    ed_icons = (i=ed_wind_type(w_num)) > EDW_DISK;
    ed_group = i == EDW_GROUP;
    for( fs2=wfile, i=0; i<witems; i++, fs2++ )
      if( fs2->state )
      {
	size += (fs = fs2)->size;
        cons += get_cons(fs->size);
	itms++;
      }
    if( !itms )
      if( over_112[w_num] ) strcpy( temp, msg_ptr[21] );
      else
      {
        txt_cons( consume[w_num], str2 );
        spf( temp, ed_group ? msg_ptr[135] : (ed_icons ? icic->info_fmt : msg_ptr[22]),
 	    witems, witems!=1 ? msg_ptr[23] : nil, total[w_num], str2, total[w_num]!=1L ?
	    msg_ptr[23] : nil );
      }
    else if( itms == 1 )
    {
      if( in_tab>=0 && tab_str[0] )
      {
        strcpy( srch, tab_str );
        strcat( srch, " | " );
      }
      else srch[0] = 0;
      if( ed_group ) spf( temp, " %s%s %s", srch, (i=fs->type.p.pexec_mode)<D_PROG ?
	  icons[i+1].ob_spec.iconblk->ib_ptext : msg_ptr[138], fs->grp_item->p.path );
      else if( ed_icons ) strcpy( temp, icic->info_fmt_sel1 );
      else
      {
	to_tandd( fs->date, time );
	tandd_to_str( time, str );
	if( fs->type.p.pexec_mode == FOLDER ) spf( temp,
	    msg_ptr[24], srch, str );
	else
	{
	  consump( z->w[w_num].path[0], str2, size, w_num );
	  spf( temp, msg_ptr[25], srch, str2, str );
	  if( !(fs->read_only & S_IJRON) ) strcat( temp, "/W" );
	  if( fs->read_only & S_IJWAC ) strcat( temp, "/A" );
	}
      }
    }
    else
    {
      txt_cons( cons, str2 );
      spf( temp, ed_group ? msg_ptr[137] :
 	  (ed_icons ? icic->info_fmt_sel : msg_ptr[26]),
	  itms, size, str2, size!=1L ? msg_ptr[23] : nil );
    }
    if( (w_info_max[w_num] = strlen(temp) - (w_dims[w_num][1]>>3) + 1)
	< 0 ) w_info_max[w_num] = 0;
    if( w_info_off[w_num] > w_info_max[w_num] ) w_info_off[w_num] =
	w_info_max[w_num];
    re_info = 0;
    if( strcmp( temp, w_info[w_num] ) )
    {
      strcpy( w_info[w_num], temp );
      re_info = 1;
    }
  }
}
/********************************************************************/
void info_redraw(void)
{
  if( re_info ) wind_set( w_handle, WF_INFO, w_info[w_num] );
  re_info = 0;
}
/********************************************************************/
int intersect( char *path1, char *path2 )
			   /* returns true if path2 is a subdir of path1 */
{
  int l;
  char temp[120], temp2[120];

  if( !*path1 && *path2 && strcmp(path2,msg_ptr[105]+1) ) return(0);
  strcpy( temp, path1 );
  strcpy( temp2, path2 );
  iso(temp2);
  iso(temp);
  if( (l=strlen(temp)) <= strlen(temp2) )
  {
    temp2[l] = '\0';
    return( !strcmp( temp, temp2 ) );
  }
  return(0);
}
/********************************************************************/
int desk_path( char *p, int i )
{
  strcpy( p, z->programs[i=z->idat[i].type-D_PROG].p.path );
  if( z->programs[i].p.type.p.pexec_mode == FOLDER )
  {
    strcat( p, slash );
    return 1;
  }
  return 0;
}
int cdecl list_files( char **fname )
{
  static char drive[]="x:\\";
  register int i, j;
  FSTRUCT *fs;

  if( w_active>=0 )
    while( list_point<list_max )
    {
      fs = &z->file[list_w][i=list_point++];
      if( fs->state )
      {
	get_full_name( *fname=filename, i, list_w );
	if( filename[0] == CLIP_LET )
	{
	  strcpy( *fname=tmpf, PASSED_CLIP );
	  strcat( tmpf, filename+2 );
	}
	if( fs->type.p.pexec_mode == FOLDER ) strcat( *fname, slash );
	goto ok;
      }
    }
  else if( d_active>=0 )
    while( list_point<z->num_icons )
    {
      i = list_point++;
      if( z->idat[i].state&1 && i != list_ignore )
      {
	switch( z->idat[i].type )
	{
	  case FLOPPY:
	  case HARDDSK:
	  case RAMDISK:
	    if( (*drive = get_drive(i+1)) != 0 ) *fname=drive;
	    else goto done;
	    break;
	  case TRASH:
	    *fname = PASSED_TRASH;
	    break;
	  case PRINTER:
	    *fname = PASSED_PRN;
	    break;
	  case CLIPBRD:
	    *fname = PASSED_CLIP;
	    break;
	  default:
	    desk_path( *fname=filename, i );
	    break;
	}
	goto ok;
      }
    }
  else if( z->macr_play )
    if( list_max )
    {
      *fname = z->macro+z->macptr;
      next_macs();
      list_max--;
      return(1);
    }

done:

  *fname = NULL;
  return(0);

ok:

  if( add_macro( MACSTR, *fname ) )
  {
    mlistcnt++;
    *(z->macro+mlistptr) = mlistcnt>>8;
    *(z->macro+mlistptr+1) = mlistcnt;
  }
  return(1);
}
/********************************************************************/
void list_setup( int num, int hand )
{
  int cmsg[8], new;
  char *ptr;

  arrow();
  if( z->macr_play )
  {
    ptr = z->macro+z->macptr;
    next_macs();
  }
  else if( num<0 )
  {
    ptr = neoq;
    num = -num;
  }
  else if( hand<0 )	/* 003 */
      ptr = z->neo_acc[z->programs[z->idat[num].type-D_PROG].is_acc-1];
  else ptr=0L;		/* 003 */
  new = 0;
  if( hand<0/*003*/ && (hand = appl_pfind(ptr)) < 0 )
  {
    in_list = 1;
    open_d_icon(num);
    in_list = 0;
    new++;
  }
  if( hand>=0/*003*/ || (hand = appl_pfind(ptr)) >= 0 )  /* 3.04: was > 0 */
  {
    if( new ) acc_init1(hand);
    if( ptr/*003*/ && add_macro( MACCHR, MLIST ) && add_macro( MACSTR, ptr ) )
    {
      mlistptr = z->macptr;
      mlistcnt = 0;
      add_macro( MACINT, 0 );
    }
    if( w_active >= 0 )
    {
      list_max = witems;
      list_w = w_num;
      list_point = 0;
    }
    else if( d_active >= 0 )
    {
      list_ignore = num;
      list_point=0;
    }
    else if( z->macr_play ) list_max = chtoii();
    cmsg[0] = NEO_ACC_PAS;
    cmsg[1] = AES_handle;
    cmsg[2] = 0;
    cmsg[3] = NEO_ACC_MAGIC;
    appl_pwrite( hand, 16, cmsg );
    get_ack( cmsg, hand );
  }
  else f_alert1( msg_ptr[71] );
}
int pcol( int c )
{
  static char pall[16] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };
  int pl;
  
  if( (pl=vplanes)>4 )
    if( c==255 ) return 1;
    else if( c==15 ) return pl==8 ? 255 : 16;
  return c<16 ? pall[c] : (pl==8 ? c : c+1);
}
void pak_rgb( int *rgb, unsigned char *c )
{
  *c = (*rgb * 255L + 499) / 1000;
}
void unpak_rgb( unsigned char *c, int *rgb )
{
  *rgb = (*c * 1000L + 127) / 255;
}
long setcolor( int num, long val )
{
  int rgb[4], old[4], i;
  long old_l;
  static char vq_mode=1;
  
  for(;;)
  {
    if( vq_color( aes_vdi_hand, num, vq_mode, old ) < 0 ) return -1L;
    if( vq_mode && (old[0]>1000 || old[1]>1000 || old[2]>1000) ) vq_mode=0;
    else break;
  }
  old_l = 0L;
  for( i=1; i<4; i++ )
  {
    pak_rgb( old+(i-1), (unsigned char *)&old_l+i );
    unpak_rgb( (unsigned char *)&val+i, rgb+(i-1) );
  }
  if( val>=0 ) vs_color( aes_vdi_hand, num, rgb );
  return old_l;
}
/********************************************************************/
void set_newdesk( OBJECT *o, int wind )
{
  static OBJECT nodesk[] = { -1, -1, -1, G_IBOX, 0, 0, 0L, 0, 0, 1, 1 },
      *old;

  if( !dtop_wind && !z->is_acc/*002*/ )
  {
    if( has_magx && !o && old ) wind_set( 0, WF_NEWDESK, nodesk, 0, 0 );	/* 003 */
    wind_set( 0, WF_NEWDESK, old=o, 0, 0 );
  }
  else if( wind && dtop_handle>0 ) wind_set( dtop_handle, X_WF_DIALOG, o );
}
int objc_add( OBJECT *tree, int parent, int child )	/* 004: for INF_LOAD */
{
  int i;
  OBJECT *p = &tree[parent];

  i = p->ob_tail;
  p->ob_tail = child;
  if( p->ob_head < 0 ) p->ob_head = child;
  if( i >= 0 ) tree[i].ob_next = child;
  tree[child].ob_next = parent;
  return(1);
}
void set_desk_ub( OBJECT *ob, int num )
{
  static USERBLK ub = { draw_desk, 0L }, ubi = { draw_d_icon, 0L };

  ob->ob_spec.userblk = !num ? &ub : &ubi;
}
int add_desk(void)
{
  int i, foo;
  OBJECT *ob, *d;
  static OBJECT o = { -1, -1, -1, G_USERDEF, TOUCHEXIT, 0, 0L, 0, 0, 0, 0 };

  d = z->desk;
  for( i=1, foo=-1; i<=z->num_icons; i++ )
    if( d[i].ob_flags&HIDETREE )
    {
      foo = i;
      break;
    }
  if( foo<0 )
    if( add_thing( (void **)&z->desk, &z->num_desk, &z->icons_rem, &o, 5,
	sizeof(o), ALLOC_MAS ) )
    {
      if( (foo = z->num_desk-1) != 0 )
      {
	objc_add( z->desk, 0, foo );
	if( z->num_idat<=foo && !add_thing( (void **)&z->idat, &z->num_idat,
	    &z->idat_rem, 0L, 5, sizeof(ICONSAVE), ALLOC_MAS ) )
	{
	  z->desk[foo].ob_flags |= HIDETREE;
	  return -1;
	}
      }
      z->num_icons = foo;
    }
    else return -1;
  ob = &z->desk[foo];
  if( !foo )	/* first object */
      *(Rect *)&ob->ob_x = z->maximum;
  else
  {
    z->idat[foo-1].type = -1;
    z->idat[foo-1].state = 0;
    get_max_icon(-1);
    ob->ob_width = max_icon.text_w;
    ob->ob_height = max_icon.h;
    ob->ob_flags &= ~HIDETREE;
  }
  set_desk_ub( ob, foo );
  if( z->desk != d && !loading_inf/*004*/ ) set_newdesk( z->desk, 1 );
  return foo;
}
void free_desk(void)
{
  cmfree( (char **)&z->desk );
  cmfree( (char **)&z->idat );
  z->num_icons = z->num_idat = 0;
}
int lrsrc_gaddr( int type, int index, void *addr )
{
  return (*gui->Nrsc_gaddr)( type, index, addr, rsc );
}
void center_dials( int all )
{
  int i;
  OBJECT *form;
  
  for( i=all ? FILTER : MAX_DIAL+1; i<=MAX_DIAL; i++ )
  {
    lrsrc_gaddr( 0, i, &form );
    form_center( form, &dum, &dum, &dum, &dum );
    if( all ) *(long *)&z->dialogs[i-FILTER][0] = *(long *)&form[0].ob_x;
  }
  if( all )
  {
    form_center( *mas->about, &dum, &dum, &dum, &dum );
    *(long *)&z->dialogs[MAX_DIAL-FILTER+1][0] = *(long *)&((*mas->about)[0].ob_x);
  }
}

OBJECT *fixlow;
int fixdial( OBJECT *tree, int tmp )
{
  *(long *)&tree[tmp].ob_x = *(long *)&fixlow[tmp].ob_x;
  *(long *)&tree[tmp].ob_width = *(long *)&fixlow[tmp].ob_width;
  return 1;
}

void rsrc_fixdial( int dial, int low )
{
  OBJECT *hi;
  
  lrsrc_gaddr( 0, dial, &hi );
  lrsrc_gaddr( 0, low, &fixlow );
  map_tree( hi, 0, -1, fixdial );
}

int copykeys( OBJECT *tree, int tmp )
{
  tree[tmp].ob_flags = lowmenu[tmp].ob_flags;
  tree[tmp].ob_state = lowmenu[tmp].ob_state;
  return 1;
}

void fit_pulls( OBJECT *menu, int pull )
{
  int i, j, x, y;
  
  for( i=menu[pull].ob_next; i>pull; i=menu[i].ob_next )
  {
    objc_offset( menu, i, &x, &y );
    if( (j = x + menu[i].ob_width - z->maximum.w + 1) > 0 )
        menu[i].ob_x = (menu[i].ob_x-j) & ~7;
  }
}

void load_rsc( char *name, int center )
{
  register int i, j;
  register char *ptr;
  int err, x, y;
  long scr, max, l;

  Fsetdta( &maintree.tree[0] );
  switch( Nrsc_load( name, &maintree.tree[0], &rsc ) )
  {
    case 0:
      spf( filename, msg_ptr[27], filename );
      f_alert1( filename );
      stop_it();
      return;
    case -1:
      form_error(8);
      stop_it();
      return;
  }
  rsrc_rcfix( rsc );
  if( graphics->cel_ht < 16 )
  {
    rsrc_fixdial( GRID, GRIDLOW );
    rsrc_fixdial( INSTALL, INSTALOW );
  }
  if( center )
  {
    free_desk();
    if( add_desk()<0 )
    {
      form_error(8);
      stop_it();
      return;
    }
  }
  center_dials( center );
  lrsrc_gaddr( 0, DESKPAT, &deskpat );
  lrsrc_gaddr( 0, ICONS, &icons );
  lrsrc_gaddr( 0, MMENU, &menu );
  if( !has_Geneva )
  {
    /* make sure pulldowns in main menu will fit into AES blit buffer */
    wind_get( 0, WF_SCREEN, &dum, &dum, &scr, (int *)&scr + 1 );
    for( max=0L, i=menu[PULL1].ob_next; i>PULL1; i=menu[i].ob_next )
    {
      l = ((menu[i].ob_width+15)>>4<<1) * (long)menu[i].ob_height * vplanes;
      if( l > max ) max = l;
    }
    if( max > scr )	/* too big */
    {
      wind_update( BEG_UPDATE );
      menu_bar( menu, 1 );	/* set the kbd equivs; won't really set bar if !_app */
      menu_bar( menu, 0 );
      if( !z->is_acc/*002*/ ) clear_menu();
      wind_update( END_UPDATE );
      lowmenu = menu;
      lrsrc_gaddr( 0, MMENULOW, &menu );
      map_tree( menu, 0, -1, copykeys );
    }
    fit_pulls( menu, PULL1 );
  }
  lrsrc_gaddr( 0, POPUPS, &popups );
  lrsrc_gaddr( 0, IMAGES, &images );
  if( graphics->cel_ht<16 ) images += WCHECK;	/* go to low rez images */
  lrsrc_gaddr( 0, WINMENU, &winmenu );
  lrsrc_gaddr( 0, GWINMENU, &grpmenu );
  aes_vdi_hand = graf_handle( &dum, &dum, &bar_w, &bar_h );
  char_w = 8;
  char_h = use_8x16 ? 16 : 8;
  if( (min_slid=bar_h*3) < 44 ) min_slid = 44;
  read_dflt_nic(1);
  if( !falc_vid )
  {
    i = Getrez();
    if( !TT_vid && i==2 || TT_vid && i==6 ) menu_enable( -1, CHANGREZ, 0 );
  }
  get_d_icon(-1);
  upsel[0] = upsel[1] = upself[0] = upsel[1] = -1;
  dwnsel[0] = dwnsel[1] = dwnself[0] = dwnsel[1] = -1;
}
/********************************************************************/
int cond_freecopy( int stat )
{
  return !z->in_copy || stat<0 || f_alert1( msg_ptr[175] ) == 1;
}
int free_memory( int stat, int pic )
{
  if( !last_buf || stat<0 || f_alert1( msg_ptr[97] ) == 1 )
  {
    free_npis(-1);
    if( !z->is_acc && pic ) free_desk_pic();
    cmfree(&reord_buf);
    reorder_on = 0;
    free_clip();
    return(1);
  }
  return(0);
}
/********************************************************************/
#ifdef DEBUG
void setup_debug(void)
{
  /* so that debugging will work properly */
  if( !z->use_master ) z->use_master = MASTER_HALFON;
  save_desktop();
}
char *crit_str[] = {
  "The disk in drive %c: is physically write-protected. Retry|Cancel",
  "Drive %c: is not responding. Please check the disk drive\
 or insert a disk. Retry|Cancel",
  "Data on the disk in drive %c: may be damaged. Retry|Cancel",
  "This application cannot read data on the disk in drive %c:.\
 Retry|Cancel",
  "Your output device is not receiving data. Retry|Cancel",
  "Please insert disk %c: into drive A:" },
  ce_xref[] = { 4, 1, 1, 2, 1, 1, 2, 2, 4, 2, 2, 2, 0, 3, 4, 2, 5 },
  ce_bad[] = "Bad function #";     

void cconws( char *ptr )
{
  while( *ptr ) Bconout( 2, *ptr++ );
}
int crit_error( int err, int drive )
{
  int num = -1-err;
  char temp[120];
  
  cconws( "\033H" );
  spf( temp, num<0 || num>=sizeof(ce_xref) ? ce_bad :
      crit_str[ce_xref[num]], drive+'A' );
  cconws( temp );
  cconws( "\033K" );
  num = (char)Bconin(2)=='2' ? 2 : 1;
  cconws( "\033H\033K" );
  return num;
}
#endif

void free_copyq(void)
{
  if( z->in_copy )
  {
    cmfree( (char **)&copy_q );
    copyqlen = 0;
    z->in_copy = 0;
  }
}
void m_quit(int stat)
{	/* stat: exit code or -1 if AC_CLOSEing DA */
  int cmsg[8], acc;
  void cdecl warmboot(void);
  extern void multi_acc(void);

  if( stat != MMAGIC && stat!=-1 && !z->is_acc && z->quit_alert )
    if( f_alert1( msg_ptr[65] ) == 2 ) return;
  if( free_memory( stat, 0 ) )
  {
    acc = z->is_acc;
    is_ac_close = stat==-1;
    if( acc && !close_all() || !acc && (!cond_freecopy(stat) || !close_every()) ) return;
    free_desk_pic();
/*%    wind_update( END_UPDATE );*/
    *(mas->open_wind) = 0L;
    in_showinf = showinf_ok = 0;
    if( z->in_copy ) copy_free();
    free_copyq();
    if( acc )
    {
      close_deskwind();
      return;
    }
    if( stat==-1 )
    {
      dtop_handle = 0;
      return;
    }
#ifdef USE_NEO_ACC
    write_acc_bad();
#endif
    free_nic( &nic_icons, &num_icons );
    cmfree(&icon_buf);
    cmfree((char **)&z->pic_ptr);
/*%    rsrc_free();*/
    cmfree(&new_msgs);		/* intentionally don't free desk */
    lfreeall(-1);
    set_newdesk( 0L, 0 );
    _menu_bar( menu, 0, 0 );  /* 002: run PRG w/ unload */
    Supexec( (long (*)())reset_conterm );
#ifdef DEBUG
    if( old_crit ) Setexc( 0x404/4, old_crit );
    setup_debug();
#endif
#ifdef DEMO
    *(lc->saver) = 0L;
#endif DEMO
    _XAccAvExit();
    (*graphics->unload_fonts)();
    (*gui->xtern.gui_exit)( AES_handle, 1 );
    if( external_gui ) Mfree(external_gui);
#if defined(DEMO) && !defined(DEBUG)
    if( stat != MMAGIC )
    {
      f_alert1( msg_ptr[EXE_MSGS-2] );
      warmboot();
    }
#endif DEMO
    exit(stat);
  }
}
/********************************************************************/
#ifdef USE_NEO_ACC
void w_all_acc( int *cmsg, int id )
{
  cmsg[1] = AES_handle;
  appl_pwrite( id, 16, cmsg );
}
void write0_all_acc( int *cmsg, char *name )
{
  register int h, *i;

  if( (h=appl_pfind(name)) >= 0 )
  {
    w_all_acc( cmsg, h );
    if( cmsg[0]==NEO_ACC_BAD )		/* 003: added if */
      for( h=MAX_NEO_ACC, i=alt_acc; --h>=0; i++ )
        if( *i==h ) *i = -1;		/* don't send twice */
  }
}
void write_all_acc( int *cmsg )
{
  register int i;

  cmsg[2] = 0;
  cmsg[3] = NEO_ACC_MAGIC;
  for( i=MAX_NEO_ACC; --i>=0; )
    if( z->neo_acc[i][0] ) write0_all_acc( cmsg, z->neo_acc[i] );
  for( i=MAX_NEO_ACC; --i>=0; )
    if( alt_acc[i]>=0 ) w_all_acc( cmsg, alt_acc[i] );
  write0_all_acc( cmsg, neoq );
}
void write_acc_bad(void)
{
  int cmsg[8];
  
  cmsg[0] = NEO_ACC_BAD;
  write_all_acc(cmsg);
}
void neo_acc_init(void)
{
  int cmsg[8];
  extern NEO_ACC nac;

  cmsg[0] = NEO_ACC_INI;
  *(long *)&cmsg[4] = (long)&nac;
  cmsg[6] = AES_handle;
  write_all_acc(cmsg);
}
#endif
/********************************************************************/
void m_open(void)
{
  SEL_ICON *s;
  SELICON_DESC i;
  
  i.icons = 0L;
  while( (s=get_msel_icon( &i, 1, 0 )) != 0 )
    if( s->wnum<0 ) open_d_icon( s->u.desk_item-1 );
    else
    {
      w_num = s->wnum;
      set_wfile();
      find_handle();
      set_window();
      if( open_w_icon( s->u.fs - z->file[w_num] ) ) break;
    }
  cmfree( (char **)&i.icons );
}
/********************************************************************/
int get_showret( int i )
{
  if( i<0 )
  {
    showinf_ok = 0;
    return 1;
  }
  jog_background = 1;	/* modal: go to next */
  if( !i ) return 0;
  showinf_ok = 1;
  return show_ret;
}
void set_wnum( int wnum )
{
  w_handle = (w_num = show_wind = wnum) >= 0 ? wxref[wnum] : -1;
}
int group_info( int wnum, char *path, GROUP_HDR *gh, long len, int func( GROUP_HDR *gh ) )
{
  char *ptr, *end;
  int buf[8];

  strcpy( showinf_path, show_path=path );
  showinf_path[find_extn(showinf_path)] = 0;
  show_grp = gh;
  show_size = len;
  show_func = func;
  set_wnum( wnum );
  return get_showret( start_form( len==-1L ? MGRP_FORM : IGRP_FORM ) );
}
int grpfunc1( GROUP_HDR *gh )
{
  int hand;
  
  if( (hand=is_group( 0L, show_path, 0 )) > 0 )
  {
    cFseek( 0L, hand, 0 );
    TOS_error( cFwrite( hand, sizeof(GROUP_HDR), gh ), 0 );
    cFclose(hand);
    return 2;
  }
  return 0;
}
int fileinf( int wnum, char *path, int type, int *date, long size, int oldrw )
/* return:  0: error  1: ok  2: update */
{
  int ret, i, hand;
  long l;
  
  show_path = path;
  show_date = date;
  show_size = size;
  show_parm1 = oldrw;
  set_wnum( wnum );
  to_filename( spathend(path), show_name );
  if( type != FOLDER && iconedit && (ret=(*icic->nic_showinf)( -1, path ))>=-1 )
      return get_showret(ret);
  else
  {
    switch(type)
    {
      case FOLDER:
        ret = start_form( IFOL_FORM );
        break;
      case GROUP:
        ret = 0;
        if( (hand=is_group( 0L, path, 0 )) > 0 )
        {
	  i = TOS_error( l=cFseek( 0L, hand, 2 ), 0 );
	  cFclose(hand);
	  if(i) ret = group_info( wnum, path, &gh, l, grpfunc1 );
	  return i;
        }
        break;
      case PROG:
      case TEXT:
      case BATCH:
	ret = start_form( IFIL_FORM );
	break;
      case NPI:
        ret = start_form( wnum>=0 ? WNPI_FORM : DNPI_FORM );
        break;
      default:
        return 1;
    }
    return get_showret(ret);
  }
}
int drv_inf( int wnum, int num )
{
  show_parm1 = num;
  set_wnum( wnum );
  return get_showret( start_form( IDRV_FORM ) );
}
void showinf_next(void)
{
  static SEL_ICON *s;
  int ok, date[6], i, redo;

  do
  {
    if( !showinf_ok ) return;
    if( showinf_ok>0 )
    {
      if( (long)showinf_icon.icons == -1L )
      {
        showinf_ok = 0;
        free_show();
        if( showinf_update ) update_drive( showinf_path, 0 );
        return;
      }
      if( (s=get_msel_icon( &showinf_icon, 1, 1 )) == 0L )
      {
bad:
        free_show();
        if( showinf_update ) update_drive( showinf_path, 0 );
        return;
      }
      if( s->wnum>=0 )
      {
        to_tandd( s->u.fs->date, date );
        get_full_name( filename, s->u.fs-z->file[s->wnum], s->wnum );
        ok = fileinf( s->wnum, filename, s->u.fs->type.p.pexec_mode,
	    ed_wind_type(s->wnum)==EDW_GROUP ? 0L : date, s->u.fs->size,
            s->u.fs->read_only );
      }
      else switch( z->idat[s->u.desk_item-1].type )
      {
        case FLOPPY:
        case HARDDSK:
        case CLIPBRD:
        case RAMDISK:
          ok = 0;
          if( (i = get_drive( s->u.desk_item )) != 0 ) ok = drv_inf( -1, i );
          break;
        default:
          ok = 1;
          if( z->idat[s->u.desk_item-1].type >= D_PROG )
          {
            i = z->idat[s->u.desk_item-1].type-D_PROG;
	    ok = fileinf( -1, z->programs[i].p.path,
	        z->programs[i].p.type.p.pexec_mode, 0L, 0L, 0 );
          }
      }
    }
    else
    {
      showinf_ok = 1;
      ok = show_ret;
      redo = 1;
    }
    if( !ok ) goto bad;
    if( ok==2 )
      if( s && s->wnum>=0 )
      {
        strcpy( showinf_path, z->w[s->wnum].path );
        showinf_ok = showinf_update = 1;
      }
      else update_drive( tmpf, 0 );
  }
  while( redo );
}
int grpfunc2( GROUP_HDR *gh )
{
  if( showinf_path[1] == ':' )
  {
    first_no_close = 1;
    update_drive( group_desc[show_wind]->path, 0 );
    first_no_close = 0;
    memcpy( &group_desc[show_wind]->hdr, gh, sizeof(GROUP_HDR) );
    re_name = 1;
    set_window();
    return 1;
  }
  return 0;
}
void m_showinf( int flag, int all )
{
  showinf_all = all;
  showinf_ok = in_showinf = 1;
  mac_play_icons();
  if( flag )
  {
    de_act(-1,-1);
    drv_inf( -1, flag );
  }
  else if( d_active >= 0 || all && w_active >= 0 )
  {
    if( d_active<0 )
    {
      w_num = num_w_active;
      set_wfile();
    }
    else w_num = -1;
    showinf_path[0] = 0;
    showinf_update = 0;
    showinf_next();
  }
  else
  {
    (long)showinf_icon.icons = -1L;	/* signals just one time through */
    if( ed_wind_type(w_num) == EDW_DISK )
      if( !all || slashes(z->w[w_num].path)==1 ) drv_inf( w_num, z->w[w_num].path[0] );
      else
      {
        strcpy( filename, z->w[w_num].path );
        *(spathend(filename)-1)=0;
        if( fileinf( w_num, filename, FOLDER, 0L, 0L, 0 )==2 ) update_drive( filename, 0 );
      }
    else if( ed_wind_type(w_num) == EDW_GROUP )
    {
      bytecpy( &gh, &group_desc[show_wind=w_num]->hdr, sizeof(GROUP_HDR) );
      group_info( w_num, group_desc[w_num]->path, &gh, group_desc[w_num]->filelen,
          grpfunc2 );
    }
    else if( iconedit ) (*icic->nic_showinf)( w_num, 0L );
  }
}
/********************************************************************/
void m_search(void)
{
  start_form( d_active >= 0 ? DSEA_FORM : WSEA_FORM );
}
/********************************************************************/
int add_copy_str( char *msg, int size )
{
  if( size<0 ) size = strlen(msg)+1;
  if( !copy_q )
  { /* add a blank entry first */
    if( !add_string( (void **)&copy_q, &copyqlen, &copyqrem, "\0", 20, 2, -1 ) ) return 0;
    copy_ok = 1;
  }
  if( add_string( (void **)&copy_q, &copyqlen, &copyqrem, msg, 20, size, -1 ) )
  {
    z->in_copy = 1;
    return 1;
  }
  return 0;
}
char no_fyi;
int add_copy( char *msg, int size )
{
  char was_in;
  
  msg[0] = (size-2)>>8;
  msg[1] = size-2;
  was_in = z->in_copy;
  if( add_copy_str( msg, size ) )
  {
    if( was_in && !no_fyi ) start_form( QFYI_FORM );
    return 1;
  }
  return 0;
}
int caf_path( char *path, int num )
{
  int j;

  j = z->idat[num-1].type;
  if( j >= D_PROG )
    if( iconedit ) return(0);
    else desk_path( path, num-1 );	/* 004: num-1 was j */
  else if( j <= CLIPBRD )
    if( (path[0] = get_drive(num)) == 0 ) return(0);
    else strcpy( path+1, colon_slash );
  else return(0);
  return(1);
}
void copy_all_files( int num, int mac, int dst_fold )
{
  char msg[5];
  SEL_ICON *s;
  SELICON_DESC i;
  int k;
  
  msg[2] = MCAF;
  if( !dst_fold && (msg[4] = get_drive(num)) == 0 ) return;
  if( mac )
  {
    msg[3] = mac;
    add_copy( msg, sizeof(msg) );
  }
  else
  {
    i.icons = 0L;
    while( (s=get_msel_icon( &i, 0, 0 )) != 0 )
      if( s->wnum<0 && s->u.desk_item!=num )
        if( z->idat[s->u.desk_item-1].type <= CLIPBRD && z->idat[num-1].type <= CLIPBRD )
        {
          if( (msg[3] = get_drive(s->u.desk_item)) == 0 ) return;
          if( !add_copy( msg, sizeof(msg) ) ) return;
        }
        else if( caf_path( filename, s->u.desk_item ) &&
            caf_path( tmpf, num ) ) cpy_from_d( filename, tmpf, 0 );
        else
        {
	  f_alert1( msg_ptr[10] );
	  break;
        }
    cmfree( (char **)&i.icons );
    if( z->in_copy ) jog_background = 1;
  }
}
void copy_next(void)
{
  int size;
  
  if( !copy_ok ) return;
  if( copyqlen>0 )
  { /* Delete the previous entry. If first time, it's a blank */
    size = (*copy_q<<8) + *(copy_q+1) + 2;
    copyqrem += size;
    if( (copyqlen-=size) > 0 )
    {
      memcpy( copy_q, copy_q+size, copyqlen );
      switch( *(copy_q+2) )
      {
        case MCOPF:
          is_clip = *(copy_q+4) == CLIP_LET;
        case MDELF:
        case MDELFNC:
          add_macro( MACCPYQ );
          copy_init();
          start_form(FILE_FORM);
          break;
        case MCAF:
          add_macro( MACCPYQ );
          copy_init();
          start_form(CPAL_FORM);
          break;
      }
      if( copy_ok ) copy_free();
    }
  }
  if( copyqlen<=0 && copy_ok ) free_copyq();
}
/********************************************************************/
void m_reorder(void)
{
  int i, j, noerr, ret=0, chk1, chk2;
  Dirent *de2;
  char *ptr, *ptr2;
  static unsigned char wentries[] = { WIMOPEN, CLOSEFLD, CLOSEWIN, CREATE,
      WIMSRCH, WIMDEL, SORTNAME, SORTDATE, SORTSIZE, SORTTYPE, SORTNONE, SORTFILT,
      WIMLOAD, WIMGRP, PRINTDIR }, entries[] = {
      OPEN, FORMAT, SEARCHDR, DELITEM, SAVEDESK, ABOUT+2, ABOUT+3,
      ABOUT+4, ABOUT+5, ABOUT+6, ABOUT+7, EDITICON, INSTICON };

  re_name = 1;
  if( iconedit )
  {
    /* i=witems necessary for buggy compiler */
    switch( (*icic->ic_reorder)( wfile, i=witems, &reorder_on, w_num ) )
    {
      case 1:
	ptr = z->w[w_num].path;
        reorder_num = w_num;	/* used in new_top() */
	if( reorder_on ) strcat( ptr, msg_ptr[105]+(*ptr?0:1) );
	else
	{
	  if( *(ptr+1) == ':' )
	    if( (ptr2=strchr(ptr,' ')) != 0 ) ptr = ptr2;
	  *ptr = '\0';
	}
	redraw_mover(w_open);
	break;
      case -1:
	update_othwind( w_num, 0 );
    }
  }
  else if( (reorder_on ^= 1) != 0 )
  {
    reorder_num = w_num;	/* used in new_top() */
    if( first(0) ) goto f_err;
    if( (reord_buf = lalloc((long)dir_items*sizeof(Dirent),-1)) == 0L )
    {
      f_alert1( msg_ptr[29] );
      goto f_err;
    }
    i = wxref[w_num];
    wxref[w_num] = -1;
    set_window();
    j = first(0);
    wxref[w_num] = i;
    if(j)
    {
      cmfree(&reord_buf);
f_err:first(reorder_on = 0);
      return;
    }
    de_act_d(-1);
  }
  menu_check( w_num, REORDER, reorder_on );
  for( i=0; i<sizeof(entries); i++ )
  {
    menu_enable( -1, entries[i], !reorder_on );
    if( reorder_on ) close_form_reord( 1, entries[i] );
  }
  for( i=0; i<sizeof(wentries); i++ )
  {
    menu_enable( w_num, wentries[i], !reorder_on );
    if( reorder_on ) close_form_reord( 0, wentries[i] );
  }
  if( !reorder_on && !iconedit )
  {
    if( f_alert1( msg_ptr[103] ) == 1 )
#ifndef DEMO
      if( Mediach(z->w[w_num].path[0]-'A') ) f_alert1( msg_ptr[104] );
      else
      {
	bee();
	if( (i=cfFsfirst( z->w[w_num].path, 0x3F ))!=AEFILNF )
	{
	  noerr = TOS_error( (long)i, 0 );
	  for( de2=(Dirent *)reord_buf, i=chk1=chk2=0; noerr && i<dir_items;
	      i++, de2++ )
	  {
	    for( j=0; j<sizeof(Dirent); j++ )
	    {
	      chk1 += j * *((char *)de2+j);
	      chk2 += j * *((char *)de+j);
	    }
	    ret = cfFsnext();
	    if( ret != AENMFIL ) noerr = TOS_error( (long)ret, 0 );
	  }
	  end_qdir();
	  if( !noerr || i!=dir_items || chk1!=chk2 ) f_alert1( msg_ptr[104] );
	  else
	  {
	    reordering++;
	    noerr = TOS_error( cfFsfirst( z->w[w_num].path, 0x3F ), 0 );
	    for( ret=0, de2=(Dirent *)reord_buf, i=0; noerr && !ret; i++ )
	    {
	      if( i<dir_items ) memcpy( de, de2++, sizeof(Dirent) );
	      else memclr( de, sizeof(Dirent) );
	      if( (char *)de-sec_buf == bp->recsiz - sizeof(Dirent) ) noerr =
		  TOS_error( Rwabs( 1, sec_buf, 1, dir_sec, dirdrv ), 0 );
	      if( noerr )
	      {
		ret = cfFsnext();
		if( ret != AENMFIL ) noerr = TOS_error( (long)ret, 0 );
	      }
	    }
	    if( noerr && (char *)de-sec_buf < bp->recsiz )
		TOS_error( Rwabs( 1, sec_buf, 1, dir_sec, dirdrv ), 0 );
	    reordering=0;
	  }
	}
      }
#else DEMO
      demo_version();
#endif DEMO
    end_qdir();
    cmfree(&reord_buf);
    force(dirdrv+'A');
    first(0);
    re_name = 1;
    set_window();
  }
}
/********************************************************************/
void d_update( char c )
{
  static char temp[]="x:\\";

  temp[0] = c;
  update_drive( temp, 0 );
}
void m_update( int force )
{
  char c, temp[120];
  int i;

  if( w_num>=0 )
    if( ed_wind_type(w_num)==EDW_GROUP )
    {
      strcpy( temp, group_desc[i=w_num]->path );
      close_wind();
      if( z->w[i].place<=0 ) open_to_path(temp);
    }
    else
    {
      if( (c=z->w[w_num].path[0]) != CLIP_LET && force )
	  *(mas->bad_media) = c-'A';
      d_update(c);
    }
}
/********************************************************************/
void rebuild_desk(void)
{
  int i;
  OBJECT *o;
  
  for( i=0, o=z->desk; i<z->num_desk; o++, i++ )
    set_desk_ub( o, i );
}
void do_desk(void)
{
  redraw_desk( Xrect(*(Rect *)&z->desk[0].ob_x), 0 );
}
/********************************************************************/
void read_q_set(void)
{
  unsigned long cmsg[4];

  if( check_q(0) )
  {
/*%    wind_update( END_UPDATE );*/
    cmsg[0] = ((long)PR_REQ << 16) | AES_handle;
    cmsg[1] = (long) &prn_param;
    appl_pwrite( q_handle, 16, cmsg );
    get_ack( (int *)cmsg, q_handle );
/*%    wind_update( BEG_UPDATE );*/
  }
}
int prep_save( char *file )
{
  char temp[10];

  temp[0] = file[0];
  temp[1] = ':';
  strcpy( temp+2, globs );
  cFsfirst( temp, 0x37 );
  if( !cFsfirst( file, 0x36 ) ) cFdelete( file );
  return cFcreate( file, 0 );
}
int _chknum;
void _chksm( char *place, int *chk, int size )
{
  while( size-- ) *chk += *place++ * (size+_chknum);
  _chknum++;
}
void _chksmstr( char *place, int *chk )
{
  _chksm( place, chk, strlen(place)+1 );
}
int group_chksum(void)
{
  int sum=0;
  GROUP_HDR *gh = &group_desc[w_num]->hdr;
  GROUP_ITEM *gi;

  _chknum = 0;
  _chksm( (char *)gh, &sum, 4+2+2+2 );
  _chksm( gh->name, &sum, 12 );
  _chksmstr( gh->desc[0], &sum );
  _chksmstr( gh->desc[1], &sum );
  for( gi=group_start[w_num]; gi; gi=gi->next )
  {
    _chksm( (char *)&gi->p, &sum, sizeof(PSTRUCT)+2+2 );
    _chksmstr( gi->name, &sum );
    _chksmstr( gi->desc[0], &sum );
    _chksmstr( gi->desc[1], &sum );
    _chksm( (char *)&gi->key, &sum, 2 );	/* 003 */
  }
  return sum;
}
int grpfunc3( GROUP_HDR *gh )
{
  char *ptr;
  
  memcpy( show_grp, gh, sizeof(GROUP_HDR) );
  strcpy( ptr = z->w[w_num].path, group_desc[w_num]->path );
  ptr = spathend(ptr);
  if( group_desc[w_num]->hdr.name[0] )
     strcpy( ptr, group_desc[w_num]->hdr.name );  /* not search results */
  re_name = 1;
  set_window();
  return 1;
}
int m_savegrp( int alert )	/* 0: no alert, always save  1: alert if changed */
{
  char temp[120];
  int ret=0, w, h, chk;
  GROUP_DESC *gd;

  if( ed_wind_type(w_num) != EDW_GROUP ) return 0;
  gd = group_desc[w_num];
  ret = 1;
  chk = group_chksum();
  if( !alert || chk != gd->chksum )
  {
    spf( temp, msg_ptr[139], gd->hdr.name[0] ? gd->hdr.name : msg_ptr[33] );
    if( alert ) switch( f_alert1(temp) )
    {
      case 1:	/* Save */
	break;
      case 2:	/* Abandon */
	return 0;
      case 3:	/* Cancel */
	return -1;
    }
    if( !gd->hdr.name[0] )
    {
      if( !fselect( 0L, "*.GRP", 0L, msg_ptr[91] ) ) return -1;
      strcpy( gd->path, filename );
      if( group_info( w_num, gd->path, &gd->hdr, -1L, grpfunc3 ) == 0 ) return -1;
    }
    strcpy( temp, gd->path );
    strcpy( strrchr( temp, '.' ), ".$G$" );
    gd->hdr.split = z->w[w_num].split;
    *(long *)&gd->hdr.offset[0] = z->w[w_num].f_off.l;
    group_unit( w_num, &w, &h );
    fix_coord( 0, &gd->hdr.x, ww[w_num][0].x, char_w );
    fix_coord( 0, &gd->hdr.y, ww[w_num][0].y, char_h );
    fix_coord( 0, &gd->hdr.w, ww[w_num][0].w, w );
    fix_coord( 0, &gd->hdr.h, ww[w_num][1].y+ww[w_num][1].h-
        ww[w_num][0].y, h );
    gd->hdr.opts.s.showicon = z->showicon[w_num];	/* 004: use w_num, not z->mXXX */
    gd->hdr.opts.s.largetext = z->stlgsml[w_num];
    gd->hdr.sort = z->sort_type[w_num];	/* 003 */
    if( save_group( temp, gd, w_num ) )
    {
      cFdelete( gd->path );
      cFrename( 0, temp, gd->path );
      gd->chksum = group_chksum();
      if( z->file[w_num] ) update_drive( gd->path, 0 );
    }
  }
  return ret;
}
int prn_dev;
int fselect( char *path, char *templ, char *ltempl, char *title )
{
  int b, r;
  char fname[13], *ptr;

  if( !path || !*path )
  {
    if( !path ) path=filename;
    strcpy( path, z->dflt_path );
    fname[0] = 0;
    ptr = spathend(path);
  }
  else strcpy( fname, ptr=spathend(path) );
  if( has_Geneva )
  {
    strcpy( ptr, ltempl ? ltempl : templ );
    r = x_fsel_input( path, 120, fname, 1, &b, title );
  }
  else
  {
    strcpy( ptr, templ );
    if( aes_ge_14 ) r = fsel_exinput( path, fname, &b, title );
    else r = fsel_input( path, fname, &b );
  }
  ptr = spathend(path);
  if( r && b && fname[0] )
  {
    if( templ[b=find_extn(templ)] && strcmp(templ+b,".*") )
        strcpy( fname+find_extn(fname), templ+b );
    strcpy( ptr, fname );
    return 1;
  }
  *ptr = 0;
  return 0;
}
void prn_str(char *s)
{
  int l, i;

  if( prn_dev > 0 )
  {
    l = strlen(s);
    if( !TOS_error( i=cFwrite( prn_dev, l, s ), 0 ) ) prn_dev=-1;
    else if( i!=l )
    {
      cFclose(prn_dev);
      prn_dev=-2;
    }
  }
  else if( !prn_dev )
      while( *s ) Bconout( 0, *s++ );
}
int print_tree( int sp )
{
  int i, j;
  char *ptr;
  FSTRUCT *fs;

  for( fs=wfile, i=0; i<witems && valid_path && !_abort() && prn_dev>=0; i++, fs++ )
  {
    for( j=0; j<sp; j++ )
      prn_str( " " );
    maintree.files++;
    maintree.bytes_total += fs->size;
    dtext_fmt( tmpf, fs, w_num, z->dir_prn.u.size, z->dir_prn.u.date, z->dir_prn.u.time,
	fs->type.p.pexec_mode==FOLDER ? '>' : ' ' );
    prn_str( tmpf );
    prn_str( crlf );
    if( fs->type.p.pexec_mode==FOLDER && z->dir_prn.u.fold )
    {
      strcpy( (ptr=spathend(filename)), fs->name );
      strcat( filename, slash );
      free_files(w_num);
      free_npis(w_num);
      first0( filename, 0 );
      sp += 2;
      if( valid_path )
	if( !print_tree(sp) ) valid_path=0;
      sp -= 2;
      *ptr = '\0';
      free_files(w_num);
      free_npis(w_num);
      first0( filename, 0 );
    }
  }
  return( i>=witems );
}
/********************************************************************/
void m_printdsk(void)
{
  unsigned int qmsg[8];

  if( Setprt( -1 ) & 0x01 )
  {
    f_alert1( msg_ptr[43] );
    return;
  }
  if( check_q(0) )
  {
    qmsg[0] = PR_SCRN;
    qmsg[1] = AES_handle;
    appl_pwrite( q_handle, 16, qmsg );
  }
  else if( !check_prn() ) Scrdmp();
}
/********************************************************************/
void m_formfeed(void)
{
  if( !check_prn() ) Bconout( 0, '\f' );
}
/********************************************************************/
void menu_clip( Rect *box )
{
  wind_get( dtop_handle, WF_WORKXYWH, &box->x, &box->y, &box->w, &box->h );
  box->y += bar_h;
  box->h -= bar_h;
  acc_wind = *box;
}
/******************************************************************/
void fix_foff( int num )
{
  int *i;
  
  i = &z->w[w_num].f_off.i[num];
  *i = *i / i_per_row[w_num] * i_per_row[w_num];
}
void arrow_tab(void)		/* 003: moved from arrowed() */
{
  if( new_tab>=0 )
  {
    select_w( new_tab, 1, w_handle, 1 );
    info();
    new_tab = -1;
  }
}
void cond_arrow( int num )	/* 003: only scroll if not visible */
{
  int n = num-z->w[w_num].f_off.i[snum];

  if( n < 0 || n >= in_wind[w_num][snum] ) arrowed( num, 0, 1 );
  else arrow_tab();
}
void new_path( int off, int upd )
{
  int tab;
  
  z->w[w_num].path[off] = '\0';
  z->w[w_num].f_off.l = z->o_f_off[w_num][slashes(z->w[w_num].path)-1].l;
  if( (tab = in_tab) >= 0 ) new_tab = z->w[w_num].f_off.i[1];
  fix_foff(0);
  fix_foff(1);
  if( add_macro( MACWIND, MNEWPTH ) ) add_macro( MACCHR, off );
  re_name = 1;
  first(0);				/* read new directory */
  if( !valid_path && upd ) update_drive( z->w[w_num].path, 0 );
  set_window();
  if( valid_path && tab>=0 && new_tab<witems )
  {
    in_tab = tab;
    tab_str[0] = 0;
    cond_arrow( z->w[w_num].f_off.i[1] );
  }
  else new_tab = -1;
}
/******************************************************************/
void new_sort( int type )
{
  if( w_num >= 0 )
  {
    if( z->sort_type[w_num] != type )
    {
      z->sort_type[w_num] = type;
      if( !is_icgroup(w_num) )
      {
	de_act( -1, -1 );
	bee();
	if( type == SORTNONE-SORTNAME ) first(0);
	else
	{
	  sort();
	  rdrw_all();
	}
	arrow();
      }
    }
  }
  else z->msort_type = type;  /* 003 */
  text_menu_check();
}
/*********************************************************************/
void new_volname( int wnum, char *ptr, int ch )
{
  int hand;
  long l;

  if( add_macro( MACCHR, MNEWVOL ) && add_macro( MACSTR, ptr ) ) add_macro(
      MACCHR, ch );
  if( wnum>=0 && ed_wind_type(wnum) == EDW_GROUP ) strcpy( group_desc[wnum]->hdr.desc[0], ptr );
  else
  {
    bee();
    *neodesk_dat = ch;
    if( *ptr )
    {
      if( (hand = cFopen( neodesk_dat, 1 )) == IEFILNF ) hand =
  	  cFcreate(neodesk_dat, 0);
      if( TOS_error( (long) hand, 0 ) )
      {
        TOS_error( l=cFwrite( hand, 21L, ptr ), 0 );
        cFclose( hand );
        cFattrib( neodesk_dat, 1, 2 );
        if( l != 21L && l>=0L ) f_alert1( msg_ptr[15] );
      }
    }
    else TOS_error( cFdelete( neodesk_dat ), 0 );
    filename[0] = ch;
  }
  if( wnum>=0 ) get_volname( filename, 1 );
  else arrow();
}
/*********************************************************************/
void open_all( int clear, int nob4 )
{
  register int i, j, next=-1, k;

  w_open = 0;
  no_b4 = nob4;
  for( i=j=1; i<8; i++ )
    for( w_num=0; w_num<7; w_num++ )
      if( z->w[w_num].place == -i )
	if(!j) z->w[w_num].place = 0;
	else if( (w_handle = x_wind_create( WIND_TYPE, XWIND_TYPE, Xrect(z->max_area) )) < 0 )
	{
/*	    error( 2 );*/
	  j--;
	  z->w[w_num].place = 0;
	}
	else
	{
	  if( clear )
	    for( k=0; k<7; k++ )
	      z->o_f_off[w_num][k].l = 0L;
	  set_wfile();
	  if( open_window() )
	  {
	    next = w_handle;
	    just_open++;
	  }
	  else
	  {
	    wind_delete(w_handle);
	    z->w[w_num].place = 0;
	  }
	}
  no_b4 = 0;
  w_handle = next;
  w_num = next>0 ? wind_xref(next) : -1;
  set_wfile();
}
/********************************************************************/
void open_prn(void)
{
  int cmsg[8];

  if( check_q(1) )
  {
    cmsg[0] = PR_LIST;
    cmsg[1] = AES_handle;
    appl_pwrite( q_handle, 16, cmsg );
  }
}
void neo_ac_open_id( int h )
{
  int cmsg[8];

  cmsg[0] = NEO_AC_OPEN;
  cmsg[1] = AES_handle;
  cmsg[2] = cmsg[3] = 0;
  appl_pwrite( h, 16, cmsg );
}
int neo_ac_open( char *s )
{
  int h;

  if( (h=appl_pfind(s)) >= 0 )
  {
    if( add_macro( MACCHR, MACOPEN ) ) add_macro( MACSTR, s );
    neo_ac_open_id(h);
    return 1;
  }
  return 0;
}
void set_half( char *old, char new )
{
  if( *old != new ) *old = new ? MASTER_HALFON : MASTER_HALFOFF;
}
void unset_half( char *c )
{
  if( *c==MASTER_HALFON ) *c = 0;
  else if( *c==MASTER_HALFOFF ) *c = 1;
}
void npi_argv( int off )
{
  if( !off )
  {
    set_half( &z->use_argv, last_npi->npi.use_argv );
    set_half( &z->env_parent, last_npi->npi.use_parent );
    exec_env = last_npi->npi.use_parent ? mas->parent_env : last_npi->npi.env;
    if( (exec_master = last_npi->npi.af.p.reload) != 0 &&
        !z->use_master ) exec_master = MASTER_HALFON;
  }
  else
  {
    unset_half( &z->use_argv );
    unset_half( &z->env_parent );
    if( z->use_argv==MASTER_HALFON ) z->use_argv = 0;
    if( z->env_parent==MASTER_HALFON ) z->env_parent = 0;
    exec_master = -1;
  }
}
void open_d_icon( int ind )
{
  register int j, l, h, ign, da;
  PROG_TYPE pt;
  char *path, (*npi_parm)[5][39]=0L;
  unsigned int cache;

  if( check_reorder() ) switch( z->idat[ind].type )
  {
    case CLIPBRD:
      if( add_macro( MACCHR, MOPDL ) ) add_macro( MACCHR, CLIP_LET );
      goto cont;
    case FLOPPY:
    case HARDDSK:
    case RAMDISK:
      if( add_macro( MACCHR, MOPDL ) ) add_macro( MACCHR, z->idat[ind].c );
cont: de_act_other(1);
      open_drive( ind+1 );
      break;
    case TRASH:
      f_alert1( msg_ptr[47] );
      break;
    case PRINTER:
      add_macro( MACCHR, MOPDP );
      open_prn();
      break;
    default:
      if( z->idat[ind].type < D_PROG+z->num_progs )
	if( iconedit ) open_to_path(nil);
	else
	{
	  j = z->idat[ind].type - D_PROG;
	  l = pathend( path=z->programs[j].p.path );
	  pt = z->programs[j].p.type;
	  pt.p.pexec_mode = 0;
	  h = z->programs[j].p.type.p.pexec_mode;
	  if( Getshift()==8 && h != FOLDER ) open_text( path+l, path, 1 );
	  else if( (da=neo_da(path)) >= 0 ) neo_ac_open_id(da);		/* 003 */
	  else if( !z->programs[j].is_acc || !neo_ac_open(z->neo_acc[z->programs[j].is_acc-1]) )
	  {
	    if( h==NPI )
	    {
	      pt = iprog_type( -1, path );
	      if( !last_npi ) break;
              if( add_macro( MACCHR, MOPNPI ) ) add_macro( MACSTR, path );
	      h = npi_type( path, 0L );
	      pt.p.pexec_mode = 0;
	      cache = last_npi->npi.af.i;
	      npi_argv(0);
	      npi_parm = &last_npi->npi.params;
	      l = pathend( path = last_npi->npi.path );
	    }
	    else cache = -1;
	    ign = w_active>=0 ? -1 : ind;
	    switch(h)
	  {
	    case GROUP:
	      if( add_macro( MACCHR, MOPDF ) ) add_macro( MACSTR, path );
	      open_to_path( path );
	      break;
	    case PROG:
/*	      if( !av_open(path,ign) )	004
	      { */
	        strcpy( filename, path );
	        filename[l] = '\0';
	        z->new_cache = cache;
	        open_program( ign, path+l, filename, pt, nil, npi_parm, 1, 1 );
	        z->new_cache = -1;
	        exec_env = 0L;
/*	      }*/
	      break;
	    case BATCH:
	      if( check_batch(0) )
	      {
		tmpf[0] = ' ';
		strcpy( tmpf+1, path );
  	        z->new_cache = cache;
		open_program( ind, NULL, NULL, pt, tmpf, npi_parm, 1, 1 );
	        z->new_cache = -1;
	        exec_env = 0L;
	      }
	      break;
	    case FOLDER:
	      strcpy( filename, path );
	      strcat( filename, slash );
	      if( add_macro( MACCHR, MOPDF ) ) add_macro( MACSTR, filename );
	      open_to_path(filename);
	      break;
	    default:
	      /*if( !av_open(path,ign) ) 004 */
	      open_text( path+l, path, 1 );
	      break;
	  }
	  npi_argv(1);
	  }
	}
  }
  if( !in_list ) de_act( -1, -1 );
}
/********************************************************************/
int open_w_icon( int ind )	/* returns 1 if folder */
{
  PROG_TYPE pt;
  int t;
  FSTRUCT *fs = &wfile[ind];

  if( check_reorder() )
    if( (t=ed_wind_type(w_num)) > EDW_DISK )
    {
      if( (*icic->edit_icon)( fs, w_num ) && t==EDW_ICONS ) get_all_icons();
    }
    else
    {
      pt = fs->type;
      get_full_name( filename, ind, w_num );
      if( (t=_open_w_icon( ind, filename, w_num, t, &pt, fs->type.p.pexec_mode, Getshift() /*, d_active>=0 ? -1 : ind*/ )) >= 0 ) return t;
    }
  select_w( ind, 0, w_handle, 1 );
  return 0;
}

int _open_w_icon( int ind, char *filename, int w_num, int wtype, PROG_TYPE *pt, int type, int i )
{
  unsigned int cache;
  char (*npi_parm)[5][39] = 0L;

      pt->p.pexec_mode = 0;
      if( type==NPI )
      {
        *pt = iprog_type( w_num, filename );
        if( !last_npi ) return 0;
        if( add_macro( MACCHR, MOPNPI ) ) add_macro( MACSTR, filename );
        type = npi_type( filename, 0L );
        pt->p.pexec_mode = 0;
	cache = last_npi->npi.af.i;
	npi_argv(0);
        npi_parm = &last_npi->npi.params;
        strcpy( filename, last_npi->npi.path );
      }
      else cache = -1;
      strcpy( tmpf, spathend(filename) );
      switch( type )
      {
	case FOLDER:
	  npi_argv(1);
	  if( wtype!=EDW_GROUP )
	  {
	    if( add_macro( MACCHR, MOPWF ) )
	      if( add_macro( MACPTH ) ) add_macro( MACSTR, tmpf );
	    open_folder(ind);
	    return 1;		       /* don't deselect */
	  }
	  strcat( filename, slash );
	case GROUP:
	  npi_argv(1);
	  if( add_macro( MACCHR, MOPDF ) ) add_macro( MACSTR, filename );
	  open_to_path( filename );
	  de_act_other(1);
	  return 0;
	case PROG:
	  if( i!=8 )
	  {
	    isolate();		     /* chop off name from get_full */
	    z->new_cache = cache;
	    open_program( w_num == num_w_active ? ind : -1,
	       tmpf, filename, *pt, nil, npi_parm, 1, 1 );
	    z->new_cache = -1;
	    exec_env = 0L;
	    npi_argv(1);
	    if( !z->multitask ) return 0;     /* don't deselect */
	    break;
	  }
	case BATCH:
	  if( i!=8 )
	  {
	    if( check_batch(0) )
	    {
	      tmpf[0] = ' ';
	      strcpy( tmpf+1, filename );
	      z->new_cache = cache;
	      open_program( ind, NULL, NULL, *pt, tmpf, npi_parm, 1, 1 );
	      z->new_cache = -1;
	      exec_env = 0L;
	      npi_argv(1);
	      if( !z->multitask ) return 0;     /* don't deselect */
	      break;
	    }
	    npi_argv(1);
	    de_act_other(1);
	    break;
	  }
	default:
	  npi_argv(1);
	  if( iconedit )
	  {
	    i = w_handle;
	    open_to_path(filename);
	    w_num = wind_xref(w_handle = i);
	    set_wfile();
	  }
	  else open_text( tmpf, filename, 1 );
	  break;
      }
  return -1;
}
/********************************************************************/
void open_drive( int ind )
{
  static char pth[]="x:\\";

  if( (*pth = get_drive( ind )) != 0 ) open_to_path(pth);
  de_act( -1, -1 );
}
/********************************************************************/
int open_to_path( char *path )
{
  int j, oldh;
  register struct Wstruct *ws;

  oldh = w_handle;
  if( !strcmp( path, msg_ptr[33] ) || has_GRP(path) )	/* 004: put grp's at end */
  {
    for( j=7; --j>=0; )
      if( !z->w[j].place ) break;
    if( j<0 ) j=7;
  }
  else
  {
    j=0;
    while( z->w[j].place && j++<7 );
  }
  if( j>=7 || (w_handle = x_wind_create( WIND_TYPE, XWIND_TYPE, Xrect(z->max_area) )) < 0 )
  {
    f_alert1( msg_ptr[48] );
    w_num = wind_xref(w_handle = oldh);
    return(0);
  }
  else
  {
    (ws = &z->w[w_num=j])->f_off.l = 0L;
    set_wfile();
    strcpy( ws->path, path );
/*    ws->split = 0; 003 */
    z->sort_type[w_num] = z->msort_type;  /* 003 */
    z->showicon[w_num] = z->mshowicon;  /* 003 */
    z->stlgsml[w_num] = z->mstlgsml;  /* 003 */
    z->stcolumn[w_num] = z->mstcolumn;  /* 003 */
    memcpy( z->sizdattim[w_num], z->msizdattim, 3*sizeof(int) );  /* 003 */
    for( j=0; j<7; j++ )
      z->o_f_off[w_num][j].l = 0L;
    if( (j=open_window()) > 0 ) return(1);
    if( !j ) wind_delete( w_handle );
    w_num = wind_xref(w_handle = oldh);
    return(0);
  }
}
/********************************************************************/
void open_folder( int num )
{
  get_full_name( filename, num, w_num );
  if( !too_many_dirs( filename ) )
  {
    z->o_f_off[w_num][slashes(z->w[w_num].path)-1].l = in_tab>=0 &&
        in_tab==w_num ? w_active : z->w[w_num].f_off.l;
    strcat( filename, slash );
    strcpy( z->w[w_num].path, filename );
    z->w[w_num].f_off.l = 0L;
    re_name = 1;
    first(0);
    set_window();
    info();
  }
  if( num_w_active == w_num ) no_wactive();
}
/********************************************************************/
void init_appl( int use_gui )
{
  AES_handle = form_handle = use_gui ? (*gui->xtern.gui_init)( &_GemParBlk ) : appl_init();
}
char *tail, tail_err;
int tail_len, tail_rem;
/* z->tail:      static 130-char buffer
   z->long_tail: 
   tail:         pointer to a local (possibly long) tail or diskbuff */
void free_exec(void)
{
  cmfree( &z->free_env );
  if( z->long_tail != z->tail )
  {
    if( z->long_tail != diskbuff ) cmfree( &z->long_tail );
/*    tail = 0L; */
  }
  else if( tail != z->tail ) cmfree( &tail );
  tail = 0L;	/* 003: moved here */
}
char npi_pstr[5*38+2];
void get_npi_parms( int icons, char *tail, char (*npi_parm)[5][39] )
{
  int i;
  
  strcpy( npi_pstr, tail );
  if( !npi_parm )
    if( icons || tail[0] ) strcat( npi_pstr, " $$" );
    else
    {
      strcpy( npi_pstr, space );
      for( i=0; i<5; i++ )
        strcat( npi_pstr, z->ttp_params[i] );
    }
  else
  {
    strcat( npi_pstr, space );	/* put space after NAC or before 1st parm */
    for( i=0; i<5; i++ )
      strncat( npi_pstr, (*npi_parm)[i], sizeof(npi_pstr)-1 );
  }
}
void add_ttp_str( char *s, int len )
{
  if( !tail_err ) tail_err =
      !add_string( (void **)&tail, &tail_len, &tail_rem, s, 50, len, ALLOC_MAS );
}
void add_ttp_param( SELICON_DESC *icon, int num, char *prog_path, int group, char docpath )
{
  SEL_ICON *s;
  char *path, *item;
  int end;
  
  if( num>=icon->nicons ) return;
  s = icon->icons + num;
  if( s->wnum>=0 )
    if( !group )
    {
      path = z->w[s->wnum].path;
      item = s->u.fs->name;
    }
    else item = pathend(path=s->u.fs->grp_item->p.path)+path;
  else item = pathend(path=z->programs[z->idat[s->u.desk_item-1].type-D_PROG].p.path)+path;
  end = pathend(path);
  if( !docpath && (pathend(prog_path)!=end || strncmp( prog_path, path, end )) )
      add_ttp_str( path, end );
  add_ttp_str( item, strlen(item) );
}
int create_tail( int ignore, char do_ttp, char (*npi_parm)[5][39], char *path, char *dflt_tail, char docpath )
{
  SELICON_DESC icon_desc;
  SEL_ICON *s;
  int l, i, ret=-1;
  char *ptr, temp[130];

  free_exec();		/* 003: in case av_open left it around */
  icon_desc.icons = 0L;
  l = w_active>=0 && ed_wind_type(num_w_active)==EDW_GROUP;
  s = get_msel_icon( &icon_desc, 0, 0 );
  if(s)  /* remove dest icon and non-progs from list */
    for( i=0; i<icon_desc.nicons; s++ )
      if( s->wnum>=0 && s->u.fs-z->file[s->wnum]==ignore ||
          s->wnum<0 && (s->u.desk_item-1==ignore || z->idat[s->u.desk_item-1].type<D_PROG) )
          memcpy( s, s+1, (--icon_desc.nicons-i)*sizeof(SEL_ICON) );
      else i++;		/* 003: was skipping next (bad) icon */
  tail_err = 0;		/* 003 */
  if( icon_desc.nicons || do_ttp || npi_parm )
  {
    if( dflt_tail ) get_npi_parms( icon_desc.nicons, dflt_tail, npi_parm );
    else strcpy( npi_pstr, " $$" );
    strcpy( temp, path );
    iso(temp);
    for( ptr=npi_pstr; *ptr; )
      if( *ptr=='$' )
        if( *++ptr>='1' && *ptr<='9' )
        {
          i = atoi(ptr)-1;
          while( *++ptr>='0' && *ptr<='9' );
          add_ttp_param( &icon_desc, i, temp, l, docpath );
        }
        else if( *ptr=='$' )
        {
          for( i=0; ; i++ )
          {
            add_ttp_param( &icon_desc, i, temp, l, docpath );
            if( i>=icon_desc.nicons-1 ) break;
            add_ttp_str( " ", 1 );
          }
          ptr++;
        }
        else
        {
          add_ttp_str( "$", 1 );
          add_ttp_str( ptr++, 1 );
        }
      else add_ttp_str( ptr++, 1 );
    if( !tail_err )			/* 003: don't scan if error */
    {
      add_ttp_str( nil, 1 );
      ptr = tail+strlen(tail);
      while( --ptr>=tail && *ptr==' ' );
      *(ptr+1) = 0;			/* remove trailing spaces */
      ret = 1;
    }
  }
  cmfree( (char **)&icon_desc.icons );	/* 003: always free */
  return tail_err ? 0 : ret;
}
char *add_env( char *add )
{
  char *e, *ptr;
  static int env_rem, env_len;
  int ret;

  if( z->env_ptr==exec_env )
  {
    for( e=z->env_ptr; *e || *(e+1); )
      while( *++e );
    if( (ptr = lalloc( env_len=e-exec_env+1, ALLOC_MAS )) == 0 ) return 0L;
    memcpy( z->env_ptr=ptr, exec_env, env_len );
    if( e==exec_env ) env_len = 0;	/* only var */
    env_rem = 0;
  }
  ret = add_string( (void **)&z->env_ptr, &env_len, &env_rem, add, 50, strlen(add), ALLOC_MAS );
  z->free_env = z->env_ptr;
  return !ret ? 0L : z->env_ptr+env_len;
}
int group_run( char scan, char shift )
{
  GROUP_ITEM **gs, *gi;
  int i, w, h;
  unsigned int j;
  
  for( i=0, gs=group_start; i<7; i++ )
    if( (gi=*gs++) != 0 )
      for( j=0; gi; j++ )
        if( gi->key.scan==scan && gi->key.shift==shift )
        {
          w = w_num;
          h = w_handle;
          w_handle = wxref[w_num=i];
          set_wfile();
          open_w_icon(j);
          w_num = w;
          w_handle = h;
          set_wfile();
          return 1;
        }
        else gi=gi->next;
  return 0;
}
void deact_info(void)
{
  de_act(-1,-1);
  info();
}
void open_program( int num, char *name, char *path, PROG_TYPE type, char *parm,
		   char (*npi_parm)[5][39], int reopen, int set_path )
{
  register int i, k, l;
  int f_handle=-1, s_hand, old_hand, status=0, cmsg[8], j, use_master;
  static int errs=1;
  char temp[130], temp4[13], err=0, *ptr, add_items=1, docpath=0, do_ttp, do_av=0;
  ERRSTRUC errstruc[1];
  extern NEO_ACC nac;
  PROG_TYPE pt;
  FSTRUCT *fs;

  if( !check_reorder() ) return;
#ifdef DEBUG
  if( z->use_master == MASTER_HALFON ) z->use_master = 0;
#endif
  use_master = exec_master>=0 ? exec_master : z->use_master;
  if( set_path<0 )
  {
    set_path = 0;
    docpath++;
  }
  else if( !set_path ) add_items = 0;
  errstruc[0].num = IEPTHNF;
  errstruc[0].str = msg_ptr[49];
  if( !type.p.return_status ) z->stat_return = NULL;
  if( path==NULL )			/* batch file */
    if( cli_handle < 0 )
    {
      strcpy( filename, z->batch_name );
      strcpy( name=temp4, filename+(i=pathend(filename)) );
      k = type.p.takes_params;
      type = prog_type( -1, z->batch_name );
      if(k) type.p.takes_params = 1;
      filename[i] = '\0';
      path = filename;
    }
    else path=name=nil;
  if( path != nil )
    if( !z->multitask && z->is_acc )
    {
      f_alert1( msg_ptr[150] );
      goto out;
    }
  j = Getshift();
  /*	prevent TTP box from NEO_ACC_EXC (set_path=0) */
  do_ttp = (set_path||docpath) && (type.p.takes_params && !z->macr_play) != (j==10);
  if( (set_path||docpath) && path != nil )
    for( i=0; i<z->num_apps; i++ )
    {
      pt = z->apps[i].type;
      if( pt.i && !strcmp( path, z->apps[i].path ) &&
	  !strcmp( name, z->apps[i].name ) )
      {
	type = pt;
	switch( z->apps[i].flags.p.reload )
	{
	  case 1:
	    use_master = 0;
	    break;
	  case 2:
	    if( !use_master && !type.p.npg ) use_master =
		z->use_master = MASTER_HALFON;
	}
	break;
      }
    }
  z->exec_type = type;
  free_exec();		/* 003 */
  if( type.p.npg )
  {
    spf( z->tail, " %D", &nac );
    strcat( z->tail, parm );
    use_master = 0;
  }
  else strcpy( z->tail, parm );
/*  tail = 0L;  003 */
  if( (set_path||docpath) && path != nil )	/* 004 */
    if( av_open(name,0) ) do_av = 1;
  if( add_items )
  {
    if( w_active >= 0 && z->w[num_w_active].path[0] == CLIP_LET )
    {
      TOS_error( CLIP_ERR, 0 );
      goto out;
    }
    if( (i=create_tail( num, do_ttp, npi_parm, do_av/*004*/ ? "x" : path,
        z->tail, docpath )) == 0 ) goto out;
    else if( i>0 )
    {
      strncpy( z->tail, tail, 125 );
      z->tail[125] = 0;
    }
  }
  if( path!=nil && set_path ) cDsetdrv( *path - 'A' );
  else if( !set_path )
  {
    z->old_drv = Dgetdrv();  /* no clipboard equiv. for either of these */
    Dgetpath( z->old_path, 0 );
  }
  z->env_ptr = exec_env = exec_env ? exec_env : (z->env_parent>0 ? mas->parent_env : z->env);
  if( path==nil || !set_path || TOS_error( dsetpath( path ), errs, errstruc ) )
  {
    if( do_ttp )
    {
      ttp_ptr = tail ? tail+1 : 0L;
      ttp_path = *path && set_path ? path : z->dflt_path;
      if( start_form(TTP_FORM) == TOSOK )
      {
	strcpy( diskbuff, space );
	for( j=0; j<5; j++ )
	  strcat( diskbuff, z->ttp_params[j] );
	if( !tail || strcmp( tail+1, diskbuff+1 ) ) tail=diskbuff;
	z->long_tail = tail;
	strncpy( z->tail, tail, 125 );
	z->tail[125] = 0;
	if( !use_master /* && type.p.tos */ )
	    err = (*mas->redirect)( &f_handle, &s_hand );
      }
      else err++;
      if( err ) goto out;
    }
    if( !tail ) tail = z->tail;
    z->long_tail = tail;
    if( !z->tail[0] ) z->tail[1] = 0;
    if( z->use_argv>0 && (!z->multitask || !aes_ge_40) )
    {
      z->tail[0] = '\x7f';
      if( (ptr=add_env( "ARGV= " )) != 0 && (i=ptr-z->env_ptr-2)!=0 &&
          add_env( path ) != 0 && add_env( name ) != 0 && 
          add_env( " " ) != 0 && add_env( tail+1 ) != 0 &&
          (ptr=add_env( "  " )) != 0 )
      {
        *(ptr-1) = *(ptr-2) = 0;
        ptr = z->env_ptr+i;
        while( *ptr )
          if( *ptr==' ' )
          {
            *ptr++ = 0;
            while( *ptr==' ' ) ptr++;
          }
          else ptr++;
      }
      else z->env_ptr = exec_env;
    }
    else z->tail[0] = strlen( z->tail+1 );
    if( *path == CLIP_LET )
    {
      TOS_error( CLIP_ERR, 0 );
      goto out;
    }
    ptr = z->tail;
    if( type.p.npg ) ptr = strchr(ptr,' ');
    if( !npi_parm && add_macro( MACCHR, MOPRG ) && add_macro( MACINT, type.i ) &&
	add_macro( MACSTR, path ) && add_macro( MACSTR, name ) &&
	add_macro( MACSTR, z->long_tail ) ) add_macro( MACCHR, path==nil );
    if( path==nil )
    {
      cmsg[0] = *parm ? NEO_CLI_RUN : NEO_AC_OPEN;
      cmsg[1] = AES_handle;
      cmsg[2] = 0;
      cmsg[3] = NEO_ACC_MAGIC;
      *(char **)(&cmsg[4]) = z->tail;
/*%      wind_update( END_UPDATE ); */
      appl_pwrite( cli_handle, 16, cmsg );
      if( *parm ) get_ack( (int *)cmsg, cli_handle );
/*%      wind_update( BEG_UPDATE );*/
out:  de_act(-1,-1);
out2: free_exec();
      return;
    }
    if( do_av && av_open(name,1) ) goto out;	/* 004 */
    strcpy( mas->path, path );
    strcat( mas->path, name );
    i = cFsfirst(mas->path,0x27);
    if( i==AEPTHNF || i==AEFILNF )	/* 004 */
    {
      missing_file( mas->path );
      goto out2;
    }
    if( !TOS_error( i, 0 ) ) goto out2;	/* 004: was return */
    if( z->multitask ) type.p.clear_screen=0;
    if( type.p.clear_screen )
    {
      if( !free_memory( 0, 0 ) || !cond_freecopy(0) || !close_every() ) goto out;
      _menu_bar( menu, 0, 0 );
      /* 003: close_every was here */
      set_clip( z->cliparray, 0 );
      set_newdesk( mas->blank, 0 );
      if( !type.p.tos )
      {
	spf( (mas->blank[1].ob_spec.tedinfo)->te_ptext, msg_ptr[50], name );
	if( !use_master )
	{
	  bee();
	  objc_draw( mas->blank, 0, 1, mas->blank[0].ob_x,
	      mas->blank[0].ob_y, mas->blank[0].ob_width,
	      mas->blank[0].ob_height );
	}
      }
      bconws( "\033H\033v" );
    }
    if( !use_master )
    {
#ifdef DEBUG
      setup_debug();
#endif
      if( type.p.tos && type.p.clear_screen )
      {
	hide_mouse();
	bconws( "\033E\033e\033v\033b\057\033c\040\r" );/* BEFORE redirection */
      }
/*%      wind_update( END_UPDATE );*/
      if( !z->multitask ) (*gui->xtern.gui_exit)( AES_handle, 1 );	 /* can't have AES left open for redirection */
      if( f_handle>0 )
      {
	old_hand = Fdup( s_hand );
	if( (i = Fforce( s_hand, f_handle )) < 0 )
	{
	  if( !z->multitask ) init_appl(1);
	  err = !TOS_error( (long) i, errs, errstruc );
	  cFclose( f_handle );
	  cFclose( old_hand );
	  f_handle = -1;
	}
      }
      if( !err )
      {
	*(mas->open_wind) = 0L;
	/* so that debugging will work properly */
	Supexec( (long (*)())reset_conterm );
	status = (*mas->execute)( s_hand, f_handle, old_hand );
	Supexec( (long (*)())set_conterm );
	z->kbio = Iorec(1);
	if( !aes_ge_40 ) *(mas->open_wind) = (long)&top_bar;
	if( !z->multitask ) init_appl(1);	    /* only if appl_exit() was done */
	init_scrptr();
      }
#ifdef DEBUG
      use_master = z->use_master = 0;
      if( !err && type.p.clear_screen ) reset_desktop();
#endif
      if( status && type.p.show_status || type.p.tos && z->tos_pause && !z->multitask )
      {
	bconws( msg_ptr[51] );
	(*mas->wait_key)();
      }
/*%      wind_update( BEG_UPDATE );*/
      init_screen();
      if( type.p.clear_screen )
      {
	if( type.p.tos ) show_mouse();
	clean_up();
	if( dtop_wind ) init_desktop( 0, 1 );
	_menu_bar( menu, 1, 0 );
	set_newdesk( z->desk, 0 );
	if( reopen )
	{
	  do_desk();
	  open_all( 1, 0 );
	}
      }
#ifdef USE_NEO_ACC
      if( !z->multitask ) neo_acc_init();
#endif
    }
    else
    {
      z->set_path = set_path;
      /* copy env if it's part of an NPI, since it will get unloaded */
      if( exec_env != z->env && exec_env != mas->parent_env && z->env_ptr == exec_env )
        if( (ptr = lalloc( sizeof(last_npi->npi.env), ALLOC_MAS )) != 0 )
            memcpy( z->env_ptr=z->free_env=ptr, exec_env, sizeof(last_npi->npi.env) );
      save_desktop();
      m_quit( MMAGIC );
    }
  }
  if( type.p.clear_screen ) de_act_d(-1);
  else if( z->multitask && !in_list )
  {
    de_act_other(1);
    deact_info();
  }
  free_exec();
}
/************/
void cdecl save_desktop(void)
{
/*%  register int i;	003
  OBJECT *obj;

  z->msort_type = sort_type;
  z->mshowicon = showicon;
  z->mstlgsml = stlgsml;
  z->mstcolumn = stcolumn;
  memcpy( z->msizdattim, sizdattim, 3*sizeof(int) );  */
}
/********************************************************************/
void menu_msg( char *msg )
{
  if( !dtop_wind )
    if( msg )
    {
      clear_menu();
      gtext( z->maximum.w>>1, 1, msg, 1+use_8x16, 1 );
    }
    else _menu_bar( menu, 1, 1 );
  else if( dtop_handle>0 ) wind_set( dtop_handle, WF_NAME, msg?msg:" NeoDesk " );
}
void open_text( char *name, char *path, int apps )
{
  int i, j, k, f_handle, pause=0, wrap=0, control, max_col,
      max_row, pages /* , temp_hand */;
  register int l, c, cr, line;
  register char *ptr;
  register long count, *page, *max_page, pos, *ptr0, *ll;
  PROG_TYPE pt, pt2;
  Rect box;

  if( !z->macr_play )
  {
    strcpy( filename, path );
    isolate();
    strcat( filename, name );
  }
  if( Getshift()!=8 && apps )
  {
    if( *(name+(k = find_extn(name))) )
    {
      k++;
      set_pnmatch();
      for( i=0, j=-1; i<z->num_apps; i++ )
	if( z->apps[i].type.i )
	{
	  spf( diskbuff, "{%s}", z->apps[i].extn );	/* 003: new extn */
	  if( lpnmatch( name+k, diskbuff ) )
	  {
	    j = i;
	    break;
	  }
	}
      if(j>=0)
      {
	pt = z->apps[j].type;
	pt.p.pexec_mode = 0;
	if( *path == CLIP_LET ) TOS_error( CLIP_ERR, 0 );
	else if( !z->apps[j].type.p.batch )
	{
	  i = 1;
	  if( z->apps[j].flags.p.docpath )
	  {
	    cDsetdrv( *path - 'A' );
	    strcpy( diskbuff, path );
	    iso(diskbuff);
	    if( !TOS_error( dsetpath( diskbuff ), 0L ) ) return;
	    i = -1;
	  }
          strcpy( diskbuff, ptr=z->apps[j].path );
          strcat( diskbuff, z->apps[j].name );
	  pt2 = iprog_type( -1, diskbuff );
	  if( pt2.p.pexec_mode==NPI )
	  {
	    if( !last_npi ) return;
	    strcpy( diskbuff, last_npi->npi.path );
	    diskbuff[j = pathend(diskbuff)] = 0;
	    pt2.p.pexec_mode = 0;
 	    z->new_cache = last_npi->npi.af.i;
 	    exec_env = last_npi->npi.env;
 	    npi_argv(0);
 	    open_program( -1, last_npi->npi.path+j, diskbuff, pt2, nil,
 	        &last_npi->npi.params, 1, i );
 	    npi_argv(1);
 	    exec_env = 0L;
	    z->new_cache = -1;
	  }
	  else open_program( -1, z->apps[j].name, ptr, pt, nil, 0L, 1, i );
	}
	else if( check_batch(0) )
	{
	  tmpf[0] = ' ';
	  strcpy( tmpf+1, z->apps[j].path );
	  strcat( tmpf, z->apps[j].name );
	  open_program( -1, NULL, NULL, pt, tmpf, 0L, 1, 1 );
	}
	de_act_other(1);
	return;
      }
    }
  }
  if( z->text_reader[0] )
  {
/*    if( av_open(z->text_reader,-1) ) return;   004 */
    ptr = spathend(z->text_reader);
    strcpy( filename, z->text_reader );
    isolate();
    open_program( -1, ptr, filename, prog_type( -1, z->text_reader ),
        nil, 0L, 1, 1 );
    return;
  }
  f_handle = cFopen( filename, 0 );
  if( f_handle==AEPTHNF || f_handle==AEFILNF )	/* 004 */
  {
    missing_file( filename );
    de_act( -1, -1 );
    return;
  }
  if( !TOS_error( (long)f_handle, 0 ) )
  {
    de_act( -1, -1 );
    return;
  }
  if( add_macro( MACCHR, MOPT ) ) add_macro( MACSTR, filename );
  wind_lock(1);
  _menu_bar( menu, 0, 1 );
  hide_mouse();
  max_col = graphics->v_cel_mx;
/*  if( (temp_hand = wind_create( 0, box.x=z->maximum.x-1, box.y=z->maximum.y-1,
      box.w=z->maximum.w+2, box.h=z->maximum.h+2 )) > 0 )
  {
    wind_update( END_UPDATE );
    wind_open( temp_hand, Xrect(box) );
  }*/
  do graf_mkstate( &dum, &dum, &k, &dum );
  while( k&1 );
  (*graphics->set_butv)(1);
  *(lc->clock_temp) = 0;	/* turn corner clock off */
  flush();
  if( view_pic( f_handle, filename ) == AEPLFMT )
  {
    bconws( "\033E\033v\033b\057\033c\040\r" );
    max_row = graphics->v_cel_my - 2;
    if( (long)(ptr0 = (long *)tmpf) & 1 ) ptr0 = (long *)((long)ptr0+1);
    pages = (tmpf+sizeof(tmpf)-(char *)ptr0)>>2;
    page = max_page = ptr0;
    pos = 0L;
    l = c = cr = line = 0;
    while( l>=0 && (count=cFread( f_handle, 1024L, diskbuff )) > 0L )
    {
      ptr = diskbuff;
      while( count-- && l>=0 )
      {
	if( (*graphics->get_mbut)()==3 ) goto quit;
	while( pause || Bconstat(2) )
	{
	  while( !Bconstat(2) );
	  control = Getshift() & 4;
	  switch( (char)(Bconin(2)>>16) )
	  {
	    case 0x2C:				  /* Z key */
	      if( control )			    /* with Control */
	      {
		z->ctrlZ ^= 1;
		pause = 0;
		l = max_row;
	      }
	      break;
	    case 0x2E:				  /* C key */
	      if( !control ) break;		    /* with Control */
	      goto quit;
	    case 0x10:				  /* Q key */
	      if( control ) break;		    /* without Control */
	    case 0x61:				  /* Undo key */
	    case 0x01:				  /* Esc key */
quit:	      l = -99;
	      pause = 0;
	      break;
	    default:
	      if( z->ctrlZ ) pause ^= 1;
	  }
	}
	if( !c && !line )
	{
	  if( ++page >= ptr0+pages ) page = ptr0;
	  if( page == max_page )
	    if( ++max_page >= ptr0+pages ) max_page = ptr0;
	  *page = pos;
	}
	switch( *ptr )			  /* process control characters */
	{
	  case '\t':			  /* horizontal tab */
	    c = (c&0xFFF8) + 7; 	  /* advance to next tab stop */
	    break;
	  case '\n':			  /* newline */
	    if( cr ) break;		  /* without a <cr> previously */
	  case '\v':			  /* vertical tab */
	  case '\f':			  /* form feed */
	    l++;			  /* next line without changing column */
	    line++;
	    break;
	  case '\a':			  /* bell */
	    c--;			  /* back-up one column (gets advanced */
	    break;			  /*	 later on) */
	  case '\b':			  /* backspace */
	    if( c>1 ) c -= 2;		  /* column gets advanced later on, so */
	    else c=-1;			  /*	 back-up one-too-many */
	}
	if( *ptr == '\r' || cr )
	{
	  if( count>0L )
	  {
	    if( !cr && *(ptr+1)=='\n' || cr && *ptr=='\n' )
	    {
	      ptr += 2 - cr;
	      pos += 2 - cr;
	      if( !cr ) count--;
	      if( !wrap )
	      {
		bconws( crlf );
		l++;
		line++;
	      }
	    }
	    else
	    {
	      if( cr ) Crawio( '\r' );
	      Crawio( *ptr++ );
	    }
	    cr = 0;
	  }
	  else cr++;
	  c=wrap=0;
	}
	else if( *ptr && (unsigned)*ptr != 0xFF )
	{
	  wrap = cr = 0;
	  Crawio( *ptr++ );
	  pos++;
	  if( ++c>max_col )
	  {
	    c=0;
	    l++;
	    line++;
	    wrap = 1;
	  }
	}
	else
	{
	  ptr++;
	  pos++;
	}
	if( line>max_row && !c ) line=0;
	if( l>max_row && !z->ctrlZ )
	{
	  bconws( msg_ptr[52] );
	while( l>max_row && !z->ctrlZ )
	{
	  i = (*mas->wait_key)();
	  control = Getshift() == 4;
	  switch( i )
	  {
	    case 0x2C:				  /* Z key */
	      if( control ) z->ctrlZ = 1;	       /* with Control */
	      break;
	    case 0x48:				  /* up arrow */
uparr:	      if( (ll = page-1)<ptr0 ) ll = ptr0+pages-1;
	      if( ll != max_page )
	      {
		pos = *(page=ll);
newpage:	cFseek( pos, f_handle, 0 );
		count=l=c=cr=wrap=0;
		line = -(max_row+1);
		bconws( "\033E" );
	      }
	      break;
	    case 0x47:				  /* Clr/Home */
	      page = max_page;
	      pos = 0L;
	      goto newpage;
	    case 0x50:				  /* down arrow */
	    case 0x39:				  /* space bar */
	      l=line=0;
	      break;
	    case 0x32:				  /* M key */
	      if( !control ) break;		    /* with Control */
	    case 0x1C:				  /* Return key */
	    case 0x72:
	      l = max_row;
	      break;
	    case 0x2E:				  /* C key */
	      if( !control ) break;		    /* with Control */
	      control = 0;
	    case 0x10:				  /* Q key */
	      if( control ) break;		    /* without Control */
	    case 0x61:				  /* Undo key */
	    case 0x01:				  /* Esc key */
	      l = -1;
	      break;
	  }
	  if( z->ctrlZ ) l=0;
	}
	bconws( "\033o\b\b\b\b\b\b\r" );
	}
      }
    }
    if( l>=0 )
    {
      bconws( msg_ptr[53] );
      wait_mbut();
      if( (*mas->wait_key)() == 0x48 && page-1 != max_page )
      {
	z->ctrlZ = 0;
	goto uparr;
      }
    }
  }
  wait_mbut();
  (*graphics->set_butv)(0);
  bconws( "\033E" );
/*  if( temp_hand > 0 )
  {
    wind_close( temp_hand );
    wind_delete( temp_hand );
    wind_update( BEG_UPDATE );
  }
  else */ form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect(z->maximum) );
  flush();
  cFclose( f_handle );
  _menu_bar( menu, 1, 1 );
  *(lc->clock_temp) = 1;	/* turn corner clock back on */
  show_mouse();
  wind_lock(0);
  unset_w();
  info_text();
  de_act_d( -1 );
}
/********************************************************************/
void new_top( int hand )
{
  int i, j;

  i = wind_xref(hand);
  if( i<0 || i==reorder_num || check_reorder() )
  {
    if( i>=0 && hand!=real_top/*003*/ )
    {
      w_num = i;
      w_handle = hand;
      if( in_tab>=0 ) de_act_other(1);
      add_macro( MACWIND, MWTOP );
      prev = z->maximum;
      for( j=0; j<7; j++ )
	if( z->w[j].place > z->w[w_num].place ) z->w[j].place--;
      z->w[w_num].place = w_open;
    }
    wind_set( hand, WF_TOP, 0, 0, 0, 0 );
  }
}
/********************************************************************/
void set_icontxt(void)
{
  if( z->showicon[w_num] ) (wtree[w_num])[new_winobj+2].ob_flags |= (1<<12);
  else (wtree[w_num])[new_winobj+2].ob_flags &= ~(1<<12);
}
int cdecl set_usrvol( PARMBLK *pb );
int cdecl set_usrimg( PARMBLK *pb );
int cdecl set_exeboot( PARMBLK *pb );
USERBLK usrvol = { set_usrvol }, usrimg = { set_usrimg }, usrarr = { set_usrimg };
void offset_objc( OBJECT *tree, int obj, int *x, int *y )
{
  register int parent=1, lastobj;

  *x = *y = 0;
  do
  {
    if( parent )
    {
      parent=0;
      *x += tree[obj].ob_x;
      *y += tree[obj].ob_y;
    }
    if( tree[obj = tree[lastobj=obj].ob_next].ob_tail == lastobj ) parent++;
  }
  while( obj >= 0 && lastobj );
}
void vol_size( OBJECT *root, OBJECT *me )
{
  *(long *)&me[4].ob_width = (*(long *)&me->ob_width =
      *(long *)&root[me->ob_next].ob_width) -
      ((long)(3*me[1].ob_width-3)<<16L);	/* 003: was 3*(w-1) */
  root[WGHBIGSL2].ob_flags |= HIDETREE;		/* 003 */
}
int cdecl set_usrvol( PARMBLK *pb )
{
  OBJECT *me;

  vol_size( pb->pb_tree, me = pb->pb_tree + pb->pb_obj );
  usrimg.ub_parm = (pb->pb_tree[WGCLOSE].ob_spec.index&0xFFFFL) | (1L<<16);
  if( vplanes==1 ) usrimg.ub_parm &= 0xFFFFFFF0L;
  me[4].ob_spec.tedinfo->te_color = pb->pb_tree[WGINFO].ob_spec.tedinfo->te_color;
  me[5].ob_x = me[4].ob_width - 7;
  return pb->pb_currstate;
}
void inc_pts( int *p, int i )
{
  p[4] += i;
  p[5] += i;
  p[6] += i;
  p[7] += i;
}
int cdecl set_usrimg( PARMBLK *pb )
{
  Rect box1, box2;
  OBJECT *img, *ob;
  int pts[8], blitcols[2]={1,0}, obj=pb->pb_obj;
  static MFDB fdb = { 0L, 16, 32, 1, 1, 1 }, fdb2 = { 0L };

  ob = &pb->pb_tree[obj];
  if( obj==WGUP || obj==WGUP2 ) img = &images[WMOREUP];
  else if( obj==WGDOWN || obj==WGDOWN2 ) img = &images[WMOREDWN];
  else if( in_drag<0 && !(ob->ob_flags&SELECTABLE) ) return pb->pb_currstate;
  else switch( obj-new_winobj )
  {
    case 1:
      if( in_drag>=0 )
	if( !(drag_icons&1) ) return pb->pb_currstate;
	else img = &images[WPARENT];
      else img = &images[WDUPLIMG];
      break;
    case 2:
      if( in_drag>=0 )
	if( !(drag_icons&2) ) return pb->pb_currstate;
	else img = &images[WCHECK];
      else img = &images[ob->ob_flags&(1<<12) ? WT : WI];
      break;
    case 3:
      if( in_drag>=0 )
/*        if( !(drag_icons&4) ) return pb->pb_currstate;  always have at least trash
	else */ img = &images[WTRASH];
      else img = &images[WSELAL];
      break;
    default:
      return pb->pb_currstate;
  }
  *(long *)&box2.w = *(long *)&box1.w = *(long *)&img->ob_width;
  fdb.fd_addr = img->ob_spec.bitblk->bi_pdata;
  offset_objc( pb->pb_tree, obj, &box2.x, &box2.y );
  if( ob->ob_width<15 ) box2.w = ob->ob_width;	/* 004 */
  else box2.x += (ob->ob_width-15)>>1;
  if( ob->ob_height<img->ob_spec.bitblk->bi_hl ) box2.w = ob->ob_height;	/* 004 */
  else box2.y += (ob->ob_height-img->ob_spec.bitblk->bi_hl)>>1;
  if( (ob->ob_state&(SELECTED|X_MAGIC|X_DRAW3D))==(SELECTED|X_MAGIC|X_DRAW3D) )  /* 003: check DRAW3D */
  {
    box2.x++;
    box2.y++;
  }
  pts[2] = (pts[0] = 0) + box1.w - 1;
  pts[3] = (pts[1] = 0) + box1.h - 1;
  pts[6] = (pts[4] = box2.x) + box2.w - 1;
  pts[7] = (pts[5] = box2.y) + box2.h - 1;
  if( (blitcols[0] = (pb->pb_parm>>8)&0xf) == (pb->pb_parm&0xf) )
      blitcols[0] = blitcols[0] ? 0 : 1;	/* get text color */
  set_clp_rect( (Rect *)&pb->pb_xc, 1 );
  if( (ob->ob_state&(X_SHADOWTEXT|X_MAGIC))==(X_SHADOWTEXT|X_MAGIC) &&
      pb->pb_parm&0xf )	/* 004 */
  {
    inc_pts( pts, 1 );
    vrt_cpyfm( vdi_hand, 2, pts, &fdb, &fdb2, blitcols );
    inc_pts( pts, -1 );
    blitcols[0] = blitcols[0] ? 0 : 1;
  }
  vrt_cpyfm( vdi_hand, 2, pts, &fdb, &fdb2, blitcols );
  return pb->pb_currstate;
}
#define Hz_200  *(long *)0x4ba
int ob_clicks;
long click_time;
long _get_timer(void)
{
  return Hz_200;
}
long get_timer(void)
{
  return Supexec(_get_timer);
}
int cdecl objhand( int hand, int obj )
{
  int num;
  static int ob_wnum, ob_onum;

  num=wind_xref(hand);
  if( obj==WGMOVE )
  {
    if( !ob_clicks || num!=ob_wnum || obj!=ob_onum || Hz_200-click_time > 39 )
    {
      click_time = Hz_200;
      ob_wnum = num;
      ob_onum = obj;
      ob_clicks = 1;
    }
    else
    {
      ob_clicks++;
      return 0;
    }
    return 1;
  }
  if( obj>=new_winobj+1 && obj<=new_winobj+4 || obj==WGVSMLSL ||
      obj==WGVSMLSL2 ) return 0;
  return 1;
}
void group_seek( int hand, long read, long actual )
{
  if( read != actual ) cFseek( actual-read, hand, 1 );
}
int open_group( char *ptr )
{
  int i, hand, minw, minh, ret=1, is_search, es;
  GROUP_DESC *gd;
  GROUP_ITEM *gi;
  Rect *r;
  unsigned int j;
  long l;

  is_search = !strcmp( ptr, msg_ptr[33] );
  if( !is_search ) for( i=0; i<7; i++ )
    if( group_desc[i] && !strcmp( group_desc[i]->path, ptr ) )
    {
      new_top( wxref[i] );
      return -1;
    }
  if( is_search || (hand=is_group( 0L, ptr, 0 )) > 0 )
  {
    if( (gd = group_desc[w_num] = (GROUP_DESC *)lalloc(sizeof(GROUP_DESC),w_num)) != 0 )
    {
      strcpy( gd->path, ptr );
      gd->chksum = 0;
      if( is_search )
      {
        memclr( &gh, sizeof(gh) );
        new_group( &gh, w_num );
        gd->filelen = 0;
      }
      memcpy( &gd->hdr, &gh, sizeof(GROUP_HDR) );
      if( !is_search )
      {
        group_seek( hand, sizeof(GROUP_HDR), gd->hdr.hdr_size );
        if( (es = gd->hdr.ent_size) > sizeof(GROUP_ITEM)-4 )
            es = sizeof(GROUP_ITEM)-4;		/* 003 */
        for( j=0; j<gd->hdr.entries; j++ )
  	  if( (gi = (GROUP_ITEM *)lalloc( sizeof(GROUP_ITEM), w_num )) == 0 ) break;
  	  else
  	  {
  	    memclr( gi, sizeof(GROUP_ITEM) );
	    if( TOS_error( l=cFread( hand, es, (char *)gi+4 ), 0 ) && l==es )
	    {
	      group_seek( hand, es, gd->hdr.ent_size );
	      gi->next = group_start[w_num];
	      group_start[w_num] = gi;
	    }
	    else break;
	  }
        gd->hdr.entries = j;
        gd->filelen = cFseek( 0L, hand, 1 );
      }
      gd->chksum = group_chksum();
      group_unit( w_num, &minw, &minh );
      if( !no_b4 )		/* 003 */
          b4group[w_num] = *(Rect *)&z->w[w_num].x;	/* 004 */
/*%        for( i=7, r=0L; --i>=w_num; )
          if( z->w[i].place<=0 )
          {
            r2 = (Rect *)&z->w[i].x;
            if(r) *r = *r2;
            else b4group[b4ptr++] = *r2;
            r = r2;
          } */
      r = (Rect *)&z->w[w_num].x;
      x_wind_calc( WC_BORDER, WIND_TYPE, XWIND_TYPE,
	  fix_coord( 1, &gh.x, 0, char_w ),
	  fix_coord( 1, &gh.y, 0, char_h ),
	  fix_coord( 1, &gh.w, 0, minw ),
	  fix_coord( 1, &gh.h, 0, minh ),
	  &r->x, &r->y, &r->w, &r->h );
      constrain(r);	/* 003: function now */
      z->w[w_num].split = gh.split;
      z->w[w_num].f_off.l = *(long *)&gh.offset;
      z->showicon[w_num] = gh.opts.s.showicon;
      z->stlgsml[w_num] = gh.opts.s.largetext;
      z->sort_type[w_num] = gh.sort;
    }
    else ret = 0;
    if( !is_search ) cFclose(hand);
  }
  else if( hand<0 ) return 0;
  return ret;
}
int open_window(void)
{
  char *ptr, is_group;
  unsigned int j;
  int i, minw, minh;
  long l;
  WIND_TREE wt;
  static OBJECT ob = { -1, -1, -1, (X_USRDEFPOST<<8)|G_IBOX, 0, X_MAGIC },
      exec = { -1, -1, -1, G_STRING, HIDETREE, X_MAGIC|X_SMALLTEXT, (long)"E", 0, 2, 6, 6 };
  static TEDINFO ted ={ "\0", "\0", "\0", IBM, 0, TE_CNTR , 0x11A1, 0, 1, 1, 1 };
  OBJECT *last, *m;

  ptr = z->w[w_num].path;
  if( iconedit && !(*icic->open_iwind)( w_num, !*ptr ? EDW_ICONS :
      (*spathend(ptr)!=0 ? EDW_FILE : EDW_DISK) ) ) return(0);
  bee();
  if( (i=open_group(ptr)) <= 0 )
  {
    arrow();
    return i;
  }
  is_group = ed_wind_type(w_num) == EDW_GROUP;
  if( iconedit ) m = (*icic->icon_menu)( w_num );
  else m = is_group ? grpmenu : winmenu;
  for( i=0; m[i].ob_tail > i; i = m[i].ob_tail );
  if( (wmenu[w_num] = (OBJECT *)lalloc( l=(i+1)*(long)sizeof(OBJECT), w_num ))
      == 0L ) return 0;
  memcpy( wmenu[w_num], m, l );
  wt.handle = w_handle;
  if( !x_wind_tree( 0, &wt ) || (wt.tree = wtree[w_num] =
      (OBJECT *)lalloc((wt.count+6)*sizeof(OBJECT)+3*sizeof(TEDINFO),w_num)) == 0L )
  {
    cmfree((char **)&wmenu[w_num]);
    return 0;
  }
  x_wind_tree( 1, &wt );
  usrarr.ub_parm = wt.tree[WGUP].ob_spec.index;
  dum = W_UPARROW;
  wind_get( w_handle, WF_COLOR, &dum, (int *)&usrarr.ub_parm+1, &dum );
  ob.ob_spec.userblk = &usrvol;
  usrvol.ub_parm = 0L;
  memcpy( last=wt.tree+(new_winobj=wt.count), &ob, sizeof(OBJECT) );
  memcpy( last+1, wt.tree+1, sizeof(OBJECT) );
  if( last[1].ob_width < 17 ) last[1].ob_width = 17;	/* 002 */
  last[1].ob_y = 0;
  last[1].ob_type = (X_USRDEFPOST<<8)|G_BOX;
  usrimg.ub_parm = last[1].ob_spec.index;
  last[1].ob_spec.userblk = &usrimg;
  memcpy( last+2, last+1, sizeof(OBJECT) );
  memcpy( last+3, last+1, sizeof(OBJECT) );
  memcpy( last+4, &vol_tree[1], sizeof(OBJECT) );
  memcpy( last+5, &exec, sizeof(OBJECT) );
  if( is_group ) last[1].ob_flags &= ~SELECTABLE;
  i = last[1].ob_width-1;
  last[4].ob_type = G_BOXTEXT;
  last[4].ob_state = X_MAGIC;
  last[4].ob_x = (last[3].ob_x = (last[2].ob_x = i) + i) + i;
  last[4].ob_y = 0;
  ted.te_just = TE_CNTR;
  memcpy( wt.tree[WGMOVE].ob_spec.tedinfo=(TEDINFO *)&last[6],
      &ted, sizeof(ted) );
  ted.te_just = TE_LEFT;
  memcpy( wt.tree[WGINFO].ob_spec.tedinfo=(TEDINFO *)&last[6]+1,
      &ted, sizeof(ted) );
  memcpy( (void *)(last[4].ob_spec.tedinfo=(TEDINFO *)&last[6]+2),
      &vol_ted, sizeof(ted) );
  last[4].ob_spec.tedinfo->te_ptext = w_volname[w_num];
  objc_add( wt.tree, WGHBIGSL, wt.count );
  for( i=0, j=wt.count++; i<4; i++ )
    objc_add( wt.tree, j, wt.count++ );
  objc_add( wt.tree, new_winobj+4, wt.count++ );
  wtree_font(w_num);
  x_wind_tree( 2, &wt );
  w_info_off[w_num] = 0;
  get_volname( ptr, 0 );
  if( maintree.tree_stat == AEPTHNF ) maintree.tree_stat = -1;
  else if( maintree.tree_stat == AEFILNF ) maintree.tree_stat = 0;
  re_info = re_name = 1;
  w_info[w_num][0] = 0;
  first( maintree.tree_stat );
  info_text();
  w_open++;
  wxref[w_num] = w_handle;
  z->w[w_num].place = w_open;
  text_menu_check();	/* 003: so initial strings are correct */
  wind_set( w_handle, X_WF_MENU, wmenu[w_num] );
  wind_set( w_handle, X_WF_VSPLIT, z->w[w_num].split );
  wind_set( w_handle, X_WF_SPLMIN, min_slid, min_slid, -1, -1 );
  x_wind_calc( WC_BORDER, WIND_TYPE, XWIND_TYPE, 0, 0, 6*12, 32+6+2,
      &dum, &dum, &minw, &minh );
  if( minw < (i=bar_w*4+2) ) minw = i;
  if( minh < (i=bar_h*7+4) ) minh = i;
  wind_set( w_handle, X_WF_MINMAX, minw, minh, -1, -1 );
  wind_set( w_handle, X_WF_OBJHAND, objhand );
  set_icontxt();
  vol_size( wt.tree, last );
  wind_open( w_handle, Xrect(z->w[w_num]) );
  recalc_wind();
  s_reset_icons();
  set_window();
  redraw_slider( 0, 0 );
  redraw_slider( 1, 0 );
  wind_get( w_handle, X_WF_VSPLIT, &z->w[w_num].split, &dum, &dum );
  prev = z->maximum;
  if( !valid_path ) update_drive( ptr, 0 );
  menu_enable( w_num, REORDER, !z->is_acc && *ptr!=CLIP_LET );
  return(1);
}
/********************************************************************/
int pathend( char *ptr )
{
  register char *ch;

  if( (ch=strrchr(ptr,'\\')) == NULL ) return(0);
  return( ch - ptr + 1 );
}
char *spathend( char *path )
{
  return path+pathend(path);
}
/********************************************************************/
void prnt_files(int ind)
{
  register int k, err=0, m;
  FSTRUCT *fs;

  if( ind<MANY_ACTIVE )
  {
    for( k=0; k<z->num_icons && !err; k++ )
      if( z->idat[k].state&1 && k != ind )
	if( (m=z->idat[k].type-D_PROG) >= 0 && z->programs[m].p.type.p.pexec_mode
	    != FOLDER ) err = print_file( z->programs[m].p.path );
  }
  else
    for( fs=wfile, k=0; k<witems && !err; k++, fs++ )
      if( fs->state && fs->type.p.pexec_mode != FOLDER )
      {
	get_full_name( filename, k, w_num );
	err = print_file( filename );
      }
}
/**********/
int cdecl print_file( char *name )
{
  register int handle, len, err;
  register char *ptr;

  bee();
  if( add_macro( MACCHR, MPRNF ) ) add_macro( MACSTR, name );
  if( (err = !TOS_error( (long)(handle=cFopen(name,0)), 0 )) == 0 )
  {
    if( (err = check_prn()) == 0 )
      while( (err = !TOS_error( (long)(len=cFread(handle,1024L,diskbuff)),
	  0 )) == 0 && len )
      {
	ptr = diskbuff;
	while( len-- ) Bconout( 0, *ptr++ );
      }
    cFclose(handle);
    read_q_set();
    if( !err && prn_param.ffd ) m_formfeed();
  }
  arrow();
  return(err);
}
/********************************************************************/
void update_npi( NPI_TYPE *npi, char *path )
{
  int i, j;
  NPI_DESC *n;
  
  for( i=0; i<8; i++ )
  {
    for( j=0, n=npi_start[i]; n; n=n->next )
      if( !strcmp( n->path, path ) )
      {
        memcpy( &n->npi, npi, sizeof(NPI_TYPE) );
        j = 1;
      }
    if(j)
      if(i) update_othwind( i-1, 0 );
      else get_d_icon(-1);
  }
}
PROG_TYPE prog_type( int wnum, char *path )
{
  int i;
  char *ptr;
  PROG_TYPE pt;
  EXTENSION *e;
  NPI_DESC *n;

  pt.i = 0;
  ptr = path+find_extn(path);
  last_npi = 0L;
  if( !strcmp( ptr, ".NPI" ) )
    if( (n=find_npi(path)) != 0 ) return (last_npi=n)->npi.pt;
    else if( (n=lalloc(sizeof(NPI_DESC),wnum)) != 0 )
      if( read_npi( &n->npi, path ) )
      {
        strcpy( n->path, path );
        n->next = npi_start[wnum+1];
        npi_start[wnum+1] = n;
        return (last_npi=n)->npi.pt;
      }
      else lfree(n);
  for( e=z->extension, i=0; ++i<=z->num_ext; e++ )
    if( e->type.c && !strcmp( ptr, e->extns ) )
    {
      pt.p.batch = e->type.s.bat;
      pt.p.npg = e->type.s.npg;
      pt.p.tos = e->type.s.tos;
      pt.p.takes_params = e->type.s.parm;
      pt.p.set_me = pt.p.show_status = pt.p.clear_screen = 1;
      return pt;
    }
  return pt;
}
/********************************************************************/
void recalc_wind(void)
{
  int x, y, w, h;

  wind_get( w_handle, WF_WORKXYWH, &x, &y, &w, &h );
  wind_get( w_handle, X_WF_VSPLIT, &z->w[w_num].split, &ww[w_num][0].h, &ww[w_num][1].h );
  ww[w_num][0].x = ww[w_num][1].x = x;
  ww[w_num][0].w = ww[w_num][1].w = w;
  ww[w_num][1].y = (ww[w_num][0].y = y) + h - ww[w_num][1].h;
/*  int w, h;
  register int *wd, sp, wh0, wh1, o, bh2;
  register Rect *r;

  bh2 = bar_h<<1;
  r = ww[w_num];
/*  wind_calc( 1, 0xFFF, Xrect(z->w[w_num]), &r[1].x, &r[1].y, &w, &h );*/
  r[1].x = z->w[w_num].x + 1;
  r[1].y = z->w[w_num].y + bh2 + 1;
  w = z->w[w_num].w - 2 - bar_w;
  h = z->w[w_num].h - bh2 - bar_h - 2;
  wd = w_dims[w_num];
  if( (sp=z->w[w_num].split) != 0 )
  {
    wh0 = r[0].h = sp - bh2 - 1;
    *(long *)&r[0].x = *(long *)&r[1].x;
  }
  r[1].y += (o = (sp ? sp-bh2 : 0) + windo[WSPLIT].ob_height) - 1;
  wh1 = r[1].h = h - o + 1;
  *(wd+29) = (*wd = *(wd+4) = *(wd+5) = *(wd+7) = *(wd+2) = *(wd+15) =
      *(wd+18) = *(wd+21) = *(wd+22) =
      (r[0].w = r[1].w = w) + 1) - bar_w;
  *(wd+14) = (*(wd+11) = (*(wd+1) = (*(wd+17) = w - bar_w) + 2) - bar_w) -
      bar_w;
  *(wd+3) = wh0 - bh2 + 2;
  *(wd+20) = wh1 - bh2 + 2;
  *(wd+6) = wh0 + bar_h + 1;
  *(wd+23) = (*(wd+8) = *(wd+10) = *(wd+9) = *(wd+16) =
      h + bh2 + 1) - bar_h;
  *(wd+19) = (*(wd+28) = (*(wd+26) = sp ? sp : bh2) +
      windo[WSPLIT].ob_height - 1) + bar_h;
  *(wd+27) = z->w[w_num].w; */
}
/*******************************************************************/
void redraw( Rect rbox, int pos, int high )
{
  register int j, i;
  FSTRUCT *fs;
  char icg;

  if( (pxarray[0] = rbox.x) < 0 ) pxarray[0] = 0;
  pxarray[1] = rbox.y;
  if( (pxarray[2] = rbox.x + rbox.w - 1) >= (j = z->maximum.x + z->maximum.w) )
      pxarray[2] = j - 1;
  if( (pxarray[3] = rbox.y + rbox.h - 1) >= (j = z->maximum.y + z->maximum.h) )
      pxarray[3] = j - 1;
  wmode0();
  if( (icg=is_icgroup(w_num)) != 0 )
  {
    pos = 0;
    high = witems;
  }
  else
  {
    i = z->w[w_num].f_off.i[snum];
    if( pos >= 0 )
    {
      pos = pos * (j=i_per_row[w_num]) + i;
      high = high*j + pos;
    }
    if( high>(j = i+in_wind[w_num][snum]+extra[w_num][snum]) || pos<0 )
    {
      high=j;
      if( pos < 0 ) pos=i;
    }
    if( high > witems ) high = witems;
  }
  hide_mouse();
  set_clip( pxarray, 1 );
  window_box( pxarray );
  get_max_icon(w_num);
  for( fs=&wfile[pos]; pos<high; pos++, fs++ )
    if( z->showicon[w_num] )
      if( icg )
      {
        if( fs->y[snum]+max_icon.h >= ww[w_num][snum].y && fs->y[snum] <=
            ww[w_num][snum].y+ww[w_num][snum].h )
            draw_w_icon( fs->nib, w_num, pos, 0 );
      }
      else draw_w_icon( fs->nib, w_num, pos, 0 );
    else draw_text( pos, w_num, 0 );
  show_mouse();
  set_clip( z->cliparray, 1 );
}
/*********************************************************************/
void rdrw_all(void)
{
  register int os;

  wind_update( BEG_UPDATE );
  s_reset_icons();
  redraw_slider( 0, 0 );
  redraw_slider( 1, 0 );
  os = snum;
  for( snum=!z->w[w_num].split; snum<(z->w[w_num].split>=0?2:1); snum++ )
    rdrw_al0();
  snum=os;
  info();
  wind_update( END_UPDATE );
}
/********************************************************************/
void rdrwal0( Rect *r )
{
  redraw( *r, -1, 0 );
}
void rdrw_al0(void)
{
  sel_draw( &ww[w_num][snum], w_handle, rdrwal0 );
}
/********************************************************************/
void redraw_arrows(void)
{
  int j;

  if( w_handle > 0 )
  {
    set_arrows();
    for( j=!z->w[w_num].split; j<(z->w[w_num].split>=0?2:1); j++ )
    {
      if( upself[j] )
      {
	upself[j]=0;
	_redraw_obj( z->maximum, WGUP + (WGUP2-WGUP)*j );
      }
      if( dwnself[j] )
      {
	dwnself[j]=0;
	_redraw_obj( z->maximum, WGDOWN + (WGUP2-WGUP)*j );
      }
    }
  }
}
/********************************************************************/
#pragma warn -par
void cdecl redraw_desk( int x, int y, int w, int h, int num )
{
  register int i;
  Rect box;

  wind_update( BEG_UPDATE );
  if( list_current != 1 )
  {
    if( list_current==2 )
    {
      box = *(Rect *)&z->desk[0].ob_x;
      list_current = 1;
    }
    else box = *(Rect *)&x;	/* valid because of cdecl */
    select( box, dtop_handle );
  }
  for( i=0; i<rects; i++ )
    if( list_current==1 )
    {
      box = *(Rect *)&x;
      if( rc_intersect( &list[i], &box ) )
          objc_draw( z->desk, num, 1, Xrect(box) );
    }
    else objc_draw( z->desk, num, 1, Xrect(list[i]) );
  wind_update( END_UPDATE );
}
#pragma warn +par
/*******************************************************************/
void redraw_mover( int num )
{
  int oh, on, oi;

  if( num > 0 )
  {
    oh = w_handle;
    on = w_num;
    for( w_num=0; w_num<7; w_num++ )
      if( z->w[w_num].place == num )
      {
	w_handle = wxref[w_num];
	wind_set( w_handle, WF_NAME, z->w[w_num].path );
	break;
      }
    w_handle = oh;
    w_num = on;
  }
}
/*******************************************************************/
void _redraw_obj( Rect rect, int num )
{
  (*gui->xtern.gad_redraw)( w_handle, &rect, num );
}
void redraw_obj( Rect rect, int num )
{
  set_window();
  _redraw_obj( rect, num );
}
/********************************************************************/
void redraw_slider( int slid, int part )
{
/*  register int obj, i, *wd, j;
  Rect r;

  obj = WBIGSL+W_ADD*slid;
  i = slid ? 24 : 12;
  wd = w_dims[w_num];
  if( !part || oslid_h[slid] != wd[i+1] ) redraw_obj( z->maximum, obj );
  else
  {
    j = wd[i] - oslid_y[slid];
    r.x = z->w[w_num].x+windo[obj].ob_x;
    r.y = z->w[w_num].y + windo[obj].ob_y + (j > 0 ? oslid_y[slid] : wd[i]);
    r.w = windo[obj].ob_width;
    r.h = wd[i+1] + abs(j) + 1;
    redraw_obj( r, obj );
  } */

  int i, it;
  long l;

  i = slid ? 24 : 12;
  if( !part || oslid_h[slid] != w_dims[w_num][i+1] )
      wind_set( w_handle, slid ? X_WF_VSLSIZE2 : WF_VSLSIZE,
      w_dims[w_num][i+1] );
  it = is_icgroup(w_num) ? group_rows[w_num] : items[w_num];
  if( (l = (it + i_per_row[w_num] - 1)/i_per_row[w_num] - rows[w_num][slid]) != 0 )
      l = z->w[w_num].f_off.i[slid] / i_per_row[w_num] * 1000L / l;
  wind_set( w_handle, slid ? X_WF_VSLIDE2 : WF_VSLIDE, (int)l );
}
/********************************************************************/
void redraw_wind( Rect rect, int flag )
{
  register int i, j, os;
  Rect r;

  wind_update( BEG_UPDATE );
  set_window();
  select( rect, w_handle );
  os=snum;
  for( i=0; i<rects; i++ )
    if( flag ) for( snum=!z->w[w_num].split; snum<(z->w[w_num].split>=0?2:1); snum++ )
    {
      r = list[i];
      if( rc_intersect( &ww[w_num][snum], &r ) ) redraw( r, -1, 0 );
    }
  snum=os;
  wind_update( END_UPDATE );
}
/********************************************************************/
void reformat_msg(void)
{
  f_alert1( msg_ptr[54] );
}
/********************************************************************/
void use_gmenu( int wind, int item )
{
  if( (item = trans_gmenu( wind, item, 1 )) != 0 )
      use_menu( wind>=0 ? w_handle : -1, item );
}
/********************************************************************/
int add_program( char *name, PROG_TYPE *t )
{
  int i, foo;
  PROG_STRUCT p;

  strcpy( p.p.path, name );
  p.p.type = *t;
  p.is_acc = 0;
  for( i=0, foo=-1; i<z->num_progs; i++ )
    if( !z->programs[i].p.type.i )
    {
      foo = i;
      break;
    }
  if( foo<0 )
    if( add_thing( (void **)&z->programs, &z->num_progs, &z->prog_rem, &p, 5,
	sizeof(p), ALLOC_MAS ) ) return z->num_progs-1;
    else return -1;
  memcpy( &z->programs[foo], &p, sizeof(p) );
  return foo;
}
int add_app(void)
{
  int i, foo;

  for( i=0, foo=-1; i<z->num_apps; i++ )
    if( !z->apps[i].type.i )
    {
      foo = i;
      break;
    }
  if( foo<0 )
    if( add_thing( (void **)&z->apps, &z->num_apps, &z->apps_rem, 0L, 5,
	sizeof(APP), ALLOC_MAS ) ) foo = z->num_apps-1;
    else return -1;
  memclr( &z->apps[foo], sizeof(APP) );
  return foo;
}
int add_extn(void)
{
  int i, foo;

  for( i=0, foo=-1; i<z->num_ext; i++ )
    if( !z->extension[i].type.c )
    {
      foo = i;
      break;
    }
  if( foo<0 )
    if( add_thing( (void **)&z->extension, &z->num_ext, &z->ext_rem, 0L, 5,
	sizeof(EXTENSION), ALLOC_MAS ) ) foo = z->num_ext-1;
    else return -1;
  memclr( &z->extension[foo], sizeof(EXTENSION) );
  return foo;
}
/********************************************************************/
void reload_inf( int macr )	 /* should not use tmpf */
{
#ifndef DEMO
  if( !macr )
    if( d_active >= 0 ) strcpy( filename,
	z->programs[z->idat[d_active].type-D_PROG].p.path );
    else get_full_name( filename, w_active, num_w_active);
  really_reload_inf( filename, -1, 0 );
#else DEMO
  demo_version();
#endif DEMO
}
/********************************************************************/
extern INF_CONV inf_conv;
void neg_places(void)
{
  int i, *p;
  
  for( i=0; i<7; i++ )
    if( *(p=&z->w[i].place) > 0 ) *p = -*p;
    else *p = 0;
}
int wrap_load( int after, int is_ext )
{
#ifndef DEMO
  int winds, i;
  long l;

  if( !after )
  {
    if( is_ext ) de_act( -1, -1 );
    i = free_memory( 0, 0 ) | cond_freecopy(0);
    if( !i ) return 1;
    if( !inf_conv.is_ok || (*inf_conv.is_ok)( INF_WINDOW1 ) )
      if( !close_all() ) return 1;
    close_all_fwind( AES_handle, 0 );
    if( !inf_conv.is_ok || (*inf_conv.is_ok)( INF_DESKPRF ) )
        cmfree( (char **)&z->pic_ptr );
    if( is_ext )
    {
      init_inf_offs();
      reset_inf( 0, 1 );
    }
  }
  else
  {
    if( is_ext ) finish_load(1);
    if( (winds = !inf_conv.is_ok || (*inf_conv.is_ok)( INF_WINDOW1 )) != 0 )
        neg_places();
    load_fonts();
    if( !inf_conv.is_ok || (*inf_conv.is_ok)( INF_DESKPRF ) )
        load_desk_pic();
    init_desktop( 1, 1 );
    if( inf_conv.is_ok ) set_newdesk( z->desk, 1 );
    do_desk();
    icon_volnames();		/* 004 */
    if( winds ) open_all( 1, 1 );
    loading_inf = 0;		/* 003 */
  }
  return 0;
#else DEMO
  return 1;
#endif DEMO
}
void really_reload_inf( char *name, int num, int is_ext )
{
#ifndef DEMO
  int i;
  int (*old_ok)( int index );

  if( !is_ext )
  {
    old_ok = inf_conv.is_ok;
    inf_conv.is_ok = 0L;
  }
  if( num<0 ) for( i=0; i<3; i++ )
    if( !strcmp( name+find_extn(name), ext[i] ) ) num=i;
  if( num>=0 )
  {
    if( !num )
      if( wrap_load( 0, 0 ) ) return;
    reload( name );
    if( !num ) wrap_load( 1, 0 );
    else
    {
      if( num!=1 ) do_desk();
      deact_info();
    }
  }
  if( !is_ext ) inf_conv.is_ok = old_ok;
#endif DEMO
}
/********************************************************************/
void remove_icon( int num )
{
  register int i;

  if( z->idat[num].type>=D_PROG )
  {
    z->programs[i=z->idat[num].type-D_PROG].p.type.i = 0;
    z->programs[i].is_acc = 0;
  }
  z->idat[num].type = -1;
  z->desk[num+1].ob_flags |= HIDETREE;
}
void rmv_icon_redraw( int num )
{
  num++;
  get_max_icon(-1);
  redraw_desk( z->desk[num].ob_x + z->desk[0].ob_x, z->desk[num].ob_y + z->desk[0].ob_y,
      max_icon.text_w, max_icon.h, 0 );
}
/********************************************************************/
void reset_all_icons( int flag )  /* only used by icon editor */
{
  char *ptr;

  free_memory( 0, 1 );
/*%  cmfree(&icon_buf); */
/*%  cmfree((char **)&z->pic_ptr);*/
/*%  rsrc_free();*/
  ptr = z->dflt_path;
  Dsetdrv( *ptr - 'A' );
  dsetpath( ptr );
  iconedit = 0;
/*%  wind_update( END_UPDATE );*/
/*%  load_rsc( "NEODESK.RSC", 0 );*/
/*%  wind_update( BEG_UPDATE );*/
  if( !flag )		/* definitely quitting */
  {
    read_dflt_nic(1);	/* already reread in ic_main if !quitting */
    load_desk_pic();
    _menu_bar( menu, 1, 0 );
  }
  reset_desktop();
  set_newdesk( z->desk, 1 );
  iconedit = 1;
}
/********************************************************************/
void cdecl reset_desktop(void)
{
  register int i;
  OBJECT *obj;

  for( i=z->num_icons; --i>=0; )
    z->idat[i].state &= ~4;
  get_d_icon(-1);
/*%  sort_type = z->msort_type;		003
  showicon = z->mshowicon;
  stlgsml = z->mstlgsml;
  stcolumn = z->mstcolumn;
  memcpy( sizdattim, z->msizdattim, 3*sizeof(int) ); */
  no_wactive();
  d_active = w_handle = w_num = -1;
#ifdef DEBUG
  set_newdesk( z->desk, 1 );
#endif
}
/********************************************************************/
void get_voff(void)
{
  icon_voff = max_icon.h + 1;
  if( z->wind_prf.s.fillpattern >= 1 && z->wind_prf.s.fillpattern <= 6 &&
      icon_voff&1 ) icon_voff--;
}
void reset_icons(void)
{
  register int i, x, y, x0, c;
  int flag=0, x_inc, high;
  register Rect *r;
  FSTRUCT *fs;

  r = ww[w_num];
  get_widths();
  get_max_icon(w_num);
  get_voff();
  x = x0 = r[snum].x + (z->showicon[w_num] ? 2 : 7);
  y = r[snum].y + 2;
  x_inc = slider();
  if( is_icgroup(w_num) )
  {
    for( fs=wfile, i=0; i<witems; i++, fs++ )
    {
      fs->x = fs->grp_item->x + x;
      fs->y[snum] = fs->grp_item->y + y - (z->w[w_num].f_off.i[snum]<<4);
    }
    return;
  }
  if( (high = (i=z->w[w_num].f_off.i[snum]) + in_wind[w_num][snum] +
      extra[w_num][snum]) > witems ) high = witems;
  c = i_per_row[w_num];
  for( fs = &wfile[i]; ++i<=high; fs++ )
  {
    fs->x = x;
    fs->y[snum] = flag ? 30000 : y;
    x += x_inc;
    if( --c <= 0 )
    {
      x = x0;
      y += icon_voff;
      if( y > r[snum].y + r[snum].h ) flag++;
      c = i_per_row[w_num];
    }
  }
}
/********************************************************************/
void s_reset_icons(void)
{
  register int os=snum;

  for( snum=!z->w[w_num].split; snum<(z->w[w_num].split>=0?2:1); snum++ )
    reset_icons();
  snum=os;
}
/********************************************************************/
int cdecl rubber_box( int x0, int y0, Rect *box, int flag )
{
  int x, y, b, k;
  register int state=0;

  flag++;
  if( x0>=2 ) x0 -= 2;
  if( y0>=2 ) y0 -= 2;
  pxarray[0] = pxarray[6] = x0 > 0 ? x0 : 0;
  pxarray[1] = pxarray[3] = y0;
  wind_lock(1);
  graf_mkstate( &x, &y, &b, &k );
  set_clip( z->cliparray, 1 );
  while( b & 1 )
  {
    if( state )
    {
      hide_mouse();
      draw_box( pxarray );
    }
    pxarray[2] = pxarray[4] = x;
    pxarray[5] = pxarray[7] = y;
    if( !state ) hide_mouse();
    draw_box( pxarray );
    show_mouse();
    do
      graf_mkstate( &x, &y, &b, &k );
    while( b&1 && x==pxarray[2] && y==pxarray[5] );
    state = 1;
  }
  if( state )
  {
    hide_mouse();
    draw_box( pxarray );
    show_mouse();
    if( pxarray[2] < x0 )
    {
      box->x = pxarray[2];
      box->w = x0 - pxarray[2];
    }
    else
    {
      box->x = x0;
      box->w = pxarray[2] - x0;
    }
    if( pxarray[5] < y0 )
    {
      box->y = pxarray[5];
      box->h = y0 - pxarray[5];
    }
    else
    {
      box->y = y0;
      box->h = pxarray[5] - y0;
    }
  }
  wind_lock(0);
  return( state );
}

void do_box( int flag )
{
  register int i;
  int arr[8];

  draw_box( pxarray );
  if( flag )
  {
    memcpy( arr, pxarray, 16 );
    arr[1] = arr[3] += bar_h << 1;
    arr[2] = arr[4] -= bar_w;
    arr[5] = arr[7] -= bar_h;
    (*graphics->gr_linebox)(arr);
  }
}
/********************************************************************/
void select( Rect box, int w_hand )
{
  Rect temp;
  static int rrem;

  rects=0;
  rc_intersect( &z->maximum, &box );
  wind_get( w_hand, WF_FIRSTXYWH, &temp.x, &temp.y, &temp.w, &temp.h );
  cmfree( (char **)&list );
  rects = 0;
  while( temp.w || temp.h )
  {
    if( rc_intersect( &box, &temp ) )
      if( !add_thing( (void **)&list, &rects, &rrem, &temp, 5, sizeof(Rect), -1 ) )
          break;
    wind_get( w_hand, WF_NEXTXYWH, &temp.x, &temp.y, &temp.w, &temp.h );
  }
}
/*********************************************************************/
void select_all(void)
{
  register int i;
  FSTRUCT *fs;

  if( w_num != num_w_active ) de_act_other(0);
  de_act_d(-1);
  for( i=0, fs=wfile; i<witems; i++, fs++ )
    if( !fs->state ) break;
  if( i != witems )
  {
    list_current = z->w[w_num].split ? 0 : 2;
    for( i=0; i<witems; i++ )
      select_w( i, SELECTED, w_handle, 1 );
    list_current = 0;
  }
  else de_act_w(-1,-1);
  redraw_arrows();
  info();
}
/*********************************************************************/
void select_d( int icon, int state )
{
  register int i, *s;

  if( (*(s=&z->idat[icon-1].state)&1) != state )
  {
    *s = (*s&~1)|state;		/* 004: volname attribute */
    get_max_icon(-1);
    redraw_desk( z->desk[icon].ob_x + z->desk[0].ob_x, z->desk[icon].ob_y +
        z->desk[0].ob_y, max_icon.text_w, max_icon.h, 0 );
    if( d_active==MANY_ACTIVE && !state )
    {
      d_active = -1;
      for( i=0; i<z->num_icons && d_active<MANY_ACTIVE; i++ )
	if( z->idat[i].state&1 )
	  if( d_active<0 ) d_active = i;
	  else d_active = MANY_ACTIVE;
    }
    else if( d_active>=0 && state ) d_active = MANY_ACTIVE;
    else if( d_active>=0 && !state ) d_active = -1;
    else d_active = icon-1;
  }
}
/*********************************************************************/
void select_w( int icon, int state, int wind, int change )
{
  Rect box;
  register int i, num, os;
  register Rect *r;
  register struct Wstruct *ws;
  char is_group;
  FSTRUCT *fs;

  r = ww[num = wind_xref(wind)];
  if( (fs=&z->file[num][icon])->state != state )
  {
    wind_update( BEG_UPDATE );
    ws = &z->w[num];
    os = snum;
    fs->state = !change && state ? -1 : state;
    get_max_icon(num);
    is_group = is_icgroup(num);
    for( snum = !ws->split; snum<(ws->split>=0?2:1); snum++ )
      if( is_group || icon >= ws->f_off.i[snum] && icon < ws->f_off.i[snum] +
	  in_wind[num][snum] + extra[num][snum] )
      {
	if( list_current != 1 )
	{
	  if( list_current==2 )
	  {
	    box = r[snum];
	    list_current = 1;
	  }
	  else
	  {
	    box.x = fs->x;
	    box.y = fs->y[snum];
	    box.w = max_icon.text_w;
	    box.h = max_icon.h;
	    if( !rc_intersect( &r[snum], &box ) ) continue;
	  }
	  select( box, wind );
	}
	hide_mouse();
	for( i=0; i<rects; i++ )
	{
	  set_clp_rect( &list[i], 1 );
	  if( z->showicon[num] ) draw_w_icon( fs->nib, num, icon, 1 );
	  else draw_text( icon, num, !state && !z->wind_prf.s.textmode );
	}
	show_mouse();
      }
    if( change )
    {
      if( w_active==MANY_ACTIVE && !state )
      {
	w_active = -1;
	for( i=0, fs=z->file[num]; i<items[num] && w_active<MANY_ACTIVE; i++, fs++ )
	  if( fs->state )
	    if( w_active<0 ) w_active = i;
	    else w_active = MANY_ACTIVE;
      }
      else if( w_active>=0 && state )
      {
        w_active = MANY_ACTIVE;
        in_tab = -1;
      }
      else if( w_active>=0 && !state ) w_active = in_tab = -1;
      else if( state ) w_active = icon;
      num_w_active = w_active>=0 ? num : -1;
    }
    else if( state )
    {
      oth_w_active = icon;
      oth_hand_w = wind;
    }
    set_clip( z->cliparray, 1 );
    snum=os;
    wind_update( END_UPDATE );
  }
}
/*********************************************************************/
void sel_many_d( Rect *box )
{
  Rect icon, text;
  register int i, x;

  list_current = 2;
  for( i=1; i<=z->num_icons; i++ )
    if( z->idat[i-1].type >= 0 )
    {
      x = ((text.w = icon_width(12)+2)-34)>>1;
      text.h = z->wind_font[0].h+2;
      icon.w = ICON_W;
      icon.h = ICON_H;
      icon.x = (text.x = z->desk[i].ob_x + z->desk[0].ob_x) + x;
      text.y = (icon.y = z->desk[0].ob_y + z->desk[i].ob_y) + ICON_H;
      if( rc_intersect( box, &icon ) || rc_intersect( box, &text ) )
	  select_d( i, SELECTED );
    }
  list_current = 0;
}
/*********************************************************************/
void select_many_w( Rect *box )
{
  Rect icon, text;
  register int i, high, half;
  FSTRUCT *fs;

  if( is_icgroup(w_num) )
  {
    i = 0;
    high = witems;
  }
  else if( (high = (i=z->w[w_num].f_off.i[snum]) + in_wind[w_num][snum] +
      extra[w_num][snum]) > witems ) high = witems;
  get_max_icon(w_num);
  list_current = z->w[w_num].split ? 0 : 2;
  half = (max_icon.text_w - ICON_W)>>1;
  for( fs=&wfile[i]; i<high; i++, fs++ )
  {
    text.x = fs->x;
    text.y = fs->y[snum];
    text.w = max_icon.text_w;
    text.h = max_icon.h;
    icon.w = ICON_W;
    icon.h = ICON_H;
    if( z->showicon[w_num] )
    {
      icon.x = text.x+half;
      icon.y = text.y;
      if( rc_intersect( box, &icon ) )
      {
        select_w( i, SELECTED, w_handle, 1 );
        continue;
      }
      text.y += ICON_H;
      text.h -= ICON_H;
    }
    if( rc_intersect( box, &text ) ) select_w( i, SELECTED, w_handle, 1 );
  }
  list_current = 0;
  if( z->w[w_num].split ) redraw_arrows();
}
/********************************************************************/
void set_ar( int num, int truth, char ch )
{
  OBJECT *ob;

  ob = &wtree[w_num][num];
  if( truth )
  {
    ob->ob_type = (X_USRDEFPOST<<8)|G_BOX;
    ob->ob_spec.userblk = &usrarr;
  }
  else
  {
    ob->ob_type = G_BOXCHAR;
    ob->ob_spec.index = ((long)ch<<24L) | (usrarr.ub_parm&0xFFFFFFL);
  }
}
void set_arrows(void)
{
  register int i, j, k, snum, tmp;
  register struct Wstruct *ws;
  FSTRUCT *fs;
  char group;

  if( (group = is_icgroup(w_num)) != 0 ) get_max_icon(w_num);
  for( snum=!(ws=&z->w[w_num])->split; snum<(ws->split>=0?2:1); snum++ )
  {
    j=k=0;
    if( w_num == num_w_active )
      for( fs=wfile, i=0; i<witems; i++, fs++ )
	if( fs->state )
	  if( group )
	  {
	    if( (tmp=fs->y[snum]-ws->f_off.i[snum])+max_icon.h <
		ww[w_num][snum].y ) j=1;
	    else if( tmp >= ww[w_num][snum].y+ww[w_num][snum].h ) k=1;
	  }
	  else if( i<ws->f_off.i[snum] ) j=1;
	  else if( i>=ws->f_off.i[snum]+in_wind[w_num][snum]+
	      extra[w_num][snum] ) k=1;
    if( upsel[snum]!=j )
    {
      set_ar( WGUP+snum*(WGUP2-WGUP), upsel[snum]=j, '\01' );
      upself[snum]++;
    }
    if( dwnsel[snum]!=k )
    {
      set_ar( WGDOWN+snum*(WGUP2-WGUP), dwnsel[snum]=k, '\02' );
      dwnself[snum]++;
    }
  }
}
/*********************************************************************/
void set_attrib(int flag)	/* 003: flag==3: don't draw contents */
{
  if( w_num>=0 && (flag && !z->showicon[w_num] || flag>1) )
  {
    if( flag<=2 )
    {
      unset_w();
      rdrw_all();
    }
    set_icontxt();
    redraw_obj( *((Rect *)&z->w[w_num].x), new_winobj+2 );
  }
  text_menu_check();
}
/********************************************************************/
void cdecl set_clp_rect( Rect *clip, int mode )
{
  int arr[4];

  arr[2] = (arr[0] = clip->x) + clip->w - 1;
  arr[3] = (arr[1] = clip->y) + clip->h - 1;
  set_clip( arr, mode );
}
/********************************************************************/
void set_wfile(void)
{
  if( w_num>=0 )
  {
    wfile = z->file[w_num];
    witems = items[w_num];
  }
}
/********************************************************************/
void set_window(void)
{
  register int i, j, top;
  register struct Wstruct *ws;

  if( w_num >= 0 )
  {
    get_widths();
    ws = &z->w[w_num];
    set_boot();
    if( re_info ) wind_set( w_handle, WF_INFO, w_info[w_num] );
    if( re_name )
    {
      wind_set( w_handle, WF_NAME,
	ws->path + (ws->path[0] == CLIP_LET ? 2 : 0) );
      form_path( w_num );
    }
    re_info = re_name = 0;
    set_icontxt();
    set_arrows();
  }
}
/********************************************************************/
int slider(void)
{
  register long a, b;
  register int x_inc, i, snum, *wd;	   /* snum is local */
  register struct Wstruct *ws;
  register Rect *r;
  char is_group;

  ws = &z->w[w_num];
  r = ww[w_num];
  wd = w_dims[w_num];
  get_max_icon(w_num);
  get_voff();
  x_inc = max_icon.text_w + ((i=z->showicon[w_num]) != 0 ? 3 : 8);
  is_group = is_icgroup(w_num);
  i_per_row[w_num] = is_group ? 1 : (z->stcolumn[w_num] || i ?
      (r[1].w-(i?max_icon.text_w>>1:max_icon.data_w)) / x_inc + 1 : 1);
  if( i_per_row[w_num] <= 0 ) i_per_row[w_num] = 1;
  for( snum=!ws->split; snum<(ws->split>=0?2:1); snum++ )
  {
    extra[w_num][snum] = is_group ? 1 : (r[snum].h - (rows[w_num][snum] =
	r[snum].h / (i=icon_voff)) * i > 2 ? i_per_row[w_num] : 0);
    if( is_group ) rows[w_num][snum] = r[snum].h >> 4;
    in_wind[w_num][snum] = rows[w_num][snum] * i_per_row[w_num];
    if( (max_itm[w_num][snum] = ((is_group ? group_rows[w_num] : witems)+i_per_row[w_num]-1) /
	i_per_row[w_num] * i_per_row[w_num] - in_wind[w_num][snum]) < 0 )
	max_itm[w_num][snum] = 0;
    if( ws->f_off.i[snum] > max_itm[w_num][snum] ) ws->f_off.i[snum] =
	max_itm[w_num][snum];
    ws->f_off.i[snum] = ws->f_off.i[snum] / i_per_row[w_num] *
	i_per_row[w_num];
    a = 0;
    b = wd[snum?20:3];
    if( max_itm[w_num][snum] > 0 )
    {
/*	if( (b = b * in_wind[w_num][snum] /
	 (max_itm[w_num][snum]+in_wind[w_num][snum])) < bar_h ) b=bar_h; */
      b = 1000L * in_wind[w_num][snum] /
	 (max_itm[w_num][snum]+in_wind[w_num][snum]);
      a = (wd[snum?20:3]-b) * ws->f_off.i[snum] /
	  max_itm[w_num][snum];
    }
    else b = 1000L;
    i = snum ? 24 : 12;
    oslid_h[snum] = wd[i+1];
    oslid_y[snum] = wd[i];
    wd[i] = a;
    wd[i+1] = b;
  }
  return( x_inc );
}
/*********************************************************************/
int cmp_name(FSTRUCT *a,FSTRUCT *b)
{
  return( strcmp( a->name, b->name ) );
}
int cmp_date(FSTRUCT *a,FSTRUCT *b)
{
  long l = b->date - a->date;

  if( l>0L ) return 1;
  if( l<0L ) return -1;
  return cmp_name(a,b);
}
int cmp_size(FSTRUCT *a,FSTRUCT *b)
{
  long l = b->size - a->size;

  if( l>0L ) return 1;
  if( l<0L ) return -1;
  return cmp_name(a,b);
}
int cmp_type(FSTRUCT *a,FSTRUCT *b)
{
  int i;

  if( a->x<0 || b->x<0 || (i=strcmp(a->name+a->x,b->name+b->x)) == 0 )
      return( cmp_name( a, b ) );
  return i;
}
int cmp_gtype(FSTRUCT *a,FSTRUCT *b)
{
  int i;

  i = b->type.p.pexec_mode - a->type.p.pexec_mode;
  if( i>0 ) return 1;
  if( i<0 ) return -1;
  return cmp_name(a,b);
}

void sort(void)
{
  register int i, t;
  int off=0, *typ;
  static int (*scmp[])(FSTRUCT *a,FSTRUCT *b) = { cmp_name, cmp_date, cmp_size, cmp_type };
  FSTRUCT *fs;
  char has_group=0;

  typ = &z->sort_type[w_num];
  /* catchall for bad INF files or for new versions */
  if( *typ < 0 || *typ > SORTNONE-SORTNAME ) *typ = 0;
  if( *typ != SORTNONE-SORTNAME )
  {
    for( fs=wfile, i=0; i<witems; i++, fs++ )
    {
      if( *typ == SORTTYPE-SORTNAME ) fs->x = find_extn( fs->name );
      if( (t=fs->type.p.pexec_mode) == FOLDER )
      {
	if( i != off ) byteswap( wfile[off].name, fs->name,
	    sizeof(FSTRUCT) );
	off++;
      }        /* if group, sort using group name, not filename */
      else if( t==GROUP || t==NPI )
      {
	byteswap( fs->name, fs->groupname, 13 );
	fs->x = -1;	/* no extension */
	has_group = 1;
      }
    }
    if( ed_wind_type(w_num)==EDW_GROUP )
	qsort( wfile, witems, sizeof(wfile[0]), *typ==0 ? cmp_name : cmp_gtype );
    else
    {
      qsort( wfile, off, sizeof(wfile[0]), scmp[*typ] );
      qsort( &wfile[off], witems-off, sizeof(wfile[0]), scmp[*typ] );
    }
    if( has_group )
      for( fs=wfile, i=0; i<witems; i++, fs++ )   /* put group names back */
	if( (t=fs->type.p.pexec_mode) == GROUP || t == NPI )
	    byteswap( fs->name, fs->groupname, 13 );
  }
}
/********************************************************************/
void stop_it(void)
{
  if(z)
  {
    if( z->multitask ) z->is_acc = 0;
    if( !z->is_acc ) (*gui->xtern.gui_exit)( AES_handle, 1 );
    *(mas->open_wind) = 0L;
  }
  exit(0);
}
/********************************************************************/
void tandd_to_str( int *time, char *str )
{
  spf( str, "%02d:%02d:%02d%s ", time[2], time[1], time[0]<<1, msg_ptr[82] );
  cat_date( str, time[4], time[3], time[5], 0 );
}
/********************************************************************/
void chk_if( char *s, int i )
{
  *(s+2) = i ? '\10' : ' ';
}
void text_menu_check(void)
{
  register int i;
  int typ;

  if( !iconedit ) translate( lowmenu ? TRANSMAC2 : TRANSMAC,
       (long)z->macr_rec, 0L, 0, 0 );
  if( !iconedit && w_num>=0 )
  {
    txt_menu_copy();
    translate( TRANSTXT, (long)z->stlgsml[w_num]/* 003 */, 0L, 0, 0 );
    translate( TRANSCOL, (long)z->stcolumn[w_num]/* 003 */, 0L, 0, 0 );
    menu_check( w_num, SHOWICON, z->showicon[w_num]/* 003 */ );
    menu_check( w_num, SHOWICON+1, !z->showicon[w_num]/* 003 */ );
    typ = ed_wind_type(w_num);
    if( typ==EDW_GROUP )
    {
      i = z->sort_type[w_num];  /* 003 */
      chk_if( wmenu[w_num][GWIMNAME].ob_spec.free_string, !i );
      chk_if( wmenu[w_num][GWIMTYPE].ob_spec.free_string, i );
      chk_if( wmenu[w_num][GWIMSTYP].ob_spec.free_string,
	  group_desc[w_num]->hdr.opts.s.showtype );
      chk_if( wmenu[w_num][GWIMSPTH].ob_spec.free_string,
	  group_desc[w_num]->hdr.opts.s.showpath );
    }
    else for( i=0; i<=4; i++ )
      menu_check( w_num, i+SORTNAME, i==z->sort_type[w_num] );  /* 003 */
    if( typ==EDW_DISK ) for( i=0; i<3; i++ )
      chk_if( wmenu[w_num][STSIZE+i].ob_spec.free_string, z->sizdattim[w_num][i] );
  }
}
void txt_menu_copy(void)
{
  if( w_num >= 0 && ed_wind_type(w_num)!=EDW_GROUP/*003*/ )
  {
    z->msort_type = z->sort_type[w_num];
    z->mshowicon = z->showicon[w_num];
    z->mstlgsml = z->stlgsml[w_num];
    z->mstcolumn = z->stcolumn[w_num];
    memcpy( z->msizdattim, z->sizdattim[w_num], 3*sizeof(int) );
  }
}
/*********************************************************************/
void to_filename( char *src, char *dest )
{
  register int j=0;

  do
  {
    if( *src != '.' || j>7 )
    {
      if( *src == '.' ) src++;
      *(dest+j) = *(src++);
    }
    else *(dest+j) = ' ';
    j++;
  }
  while( *(src-1) );
}
/*********************************************************************/
void to_tandd( unsigned long time, int *bytes )
{
  *bytes = time & 0x1F; 		     /* seconds */
  *(bytes+1) = time>>5 & 0x3F;		     /* minutes */
  *(bytes+2) = time>>11 & 0x1F; 	     /* hours */
  *(bytes+3) = time>>16 & 0x1F; 	     /* days */
  *(bytes+4) = time>>21 & 0x0F; 	     /* months */
  *(bytes+5) = (time>>25 & 0x7F) + 80;	     /* years */
}
/*********************************************************************/
int too_many_dirs( char *path )
{
  register int i;

  for( i=0; *path; path++ )
    if( *path == '\\' ) i++;
  if( i>8 )
  {
    f_alert1( msg_ptr[60] );
    arrow();
  }
  return( i>8 );
}
/********************************************************************/
void send_dum_msg(void)
{
  int buf[8];

  buf[0] = DUM_MSG;
  buf[2] = 0;
  appl_write( buf[1]=AES_handle, 16, buf );
}
void top_bar(void)  /* jog _dispatch into realizing something happened */
{
  if( !*gui->xtern.dum_msg )	/* 003: avoid too many appl_writes */
  {
    ++*gui->xtern.dum_msg;
    send_dum_msg();
  }
}
/********************************************************************/
void menu_txt( OBJECT *o, int i, char *ptrs1, char *ptrs2 )	/* 003: only update if changed */
{
  char tmp[80], *s;
  
  s = o[i].ob_spec.free_string;
  if( aes_ge_40 ) strcpy( tmp, s );
  spf( s, ptrs1, ptrs2 );
  if( aes_ge_40 && strcmp(s,tmp) ) menu_text( o, i, s );
}
void translate( int ind, long parm1, long parm2, int parm3, int parm4 )
{
  register int i;
  char *ptrs[7];
  register char *fmtptr;
  char temp[80];

  strcpy( ptrs[0] = fmtptr = temp, msg_ptr[ind] );
  for( i=1; *fmtptr; fmtptr++ )
    if( *fmtptr=='|' )
    {
      ptrs[i++] = fmtptr+1;
      *fmtptr = '\0';
    }
  switch( ind )
  {
    case TRANSTXT:
      i = STLGSML;
      goto dospf;
    case TRANSCOL:
      i = STCOLUMN;
dospf:if( (i=trans_gmenu( w_num, i, 1 )) == 0 ) break;
      menu_txt( wmenu[w_num], i, ptrs[2], ptrs[parm1] );
      break;
    case TRANSMAC:
    case TRANSMAC2:
      i = BEMACRO;
      menu_txt( menu, i, ptrs[2], ptrs[parm1] );
      break;
    case TRANFILE:
      spf( form[FCONSEG].ob_spec.free_string, ptrs[3], ptrs[parm1] );
      break;
    case TRANFOP:
      spf( (char *)parm2, ptrs[2], ptrs[parm1] );
      break;
    case TRANSTAT:
      spf( form[DOPTYPE].ob_spec.free_string, ptrs[4], ptrs[parm1],
          (long)(unsigned int)parm2, (long)(unsigned int)parm3, parm4 );
      break;
    case TRANSALL:
      spf( form[FOSTAT].ob_spec.tedinfo->te_ptext, ptrs[6], ptrs[parm1] );
      break;
  }
}
/*********************************************************************/
int cdecl TOS_error( long num, int errs, ... )
{
  register int i;
  register char *fmt1 = msg_ptr[61], *fmt2 = msg_ptr[62];
  char temp[120] = "";
  ERRSTRUC *errstruc;
  va_list argpoint;

  if( TOS_abort ) if( _abort() ) return(0);
  if( num < 0L && num != CLIP_IGN )
  {
    if( errs )
    {
      va_start( argpoint, errs );
      errstruc = (ERRSTRUC *)(va_arg( argpoint, ERRSTRUC * ));
      for( i=0; i<errs; i++ )
	if( num == (long)errstruc[i].num ) spf( temp, fmt1, errstruc[i].str,
	    num );
    }
    if( num==CLIP_ERR ) f_alert1( msg_ptr[96] );
    else if( num==LOCK_ERR ) f_alert1( msg_ptr[146] );
    else
    {
      if( !temp[0] )
      {
	if( num >= -12L ) num = IERROR;
	for( i=0; i<DFLT_ERRORS; i++ )
	  if( num == (long)mas->dflt_errors[i].num )
	      spf( temp, fmt1, mas->dflt_errors[i].str, num );
	if( !temp[0] ) spf( temp, fmt2, num );
      }
      f_alert1( temp );
    }
    return(0);
  }
  return( num!=CLIP_IGN );
}
/**********************************************************************/
int trash_all( char *path )
{
  int i, noerr, ents_sec, f, n;
  unsigned int cl, ncl, j;
  union
  {
    unsigned char c[2];
    unsigned int i;
  } u;

  spf( tmpf, msg_ptr[70], *path );
  if( add_macro( MACCHR, MDELALL ) ) add_macro( MACCHR, *path );
  if( *path==CLIP_LET )
  {
    if( last_buf && f_alert1( msg_ptr[97] ) == 1 ) free_clip();
  }
  else if( form_alert( 2, tmpf ) == 1 )
  {
    strcpy( path+1, colon_slash );
    bee();
    if( !getbpb( path, 1 ) )
    {
      ents_sec = ((bp->recsiz+1)<<3) / (bp->bflags&1?16:12);
      n = !(bp->bflags&1) + 1;
      u.i = 0;
      for( noerr=i=1, cl=0; i<=bp->fsiz<<1 && noerr; i++ )
	if( (noerr = TOS_error( Rwabs( 0, sec_buf, n, i, dirdrv ), 0 )) != 0 )
	{
	  for( j=0; j<ents_sec; j++, cl++ )
	    if( !(bp->bflags&1) )
	    {
	      f = (cl + (cl>>1)) % bp->recsiz;
	      u.c[0] = sec_buf[f+1];
	      u.c[1] = sec_buf[f];
	      ncl = u.i;
	      if( cl&1 ) ncl >>= 4;
	      else ncl &= 0xFFF;
	      if( ncl<0xff0 || ncl>0xff7 )
		if( cl&1 )
		{
		  sec_buf[f] &= 0xF;
		  sec_buf[f+1] = 0;
		}
		else
		{
		  sec_buf[f] = 0;
		  sec_buf[f+1] &= 0xF0;
		}
	    }
	    else
	    {
	      f = (cl % ents_sec) << 1;
	      u.c[0] = sec_buf[f+1];
	      u.c[1] = sec_buf[f];
	      if( u.i<0xfff0 || u.i>0xfff7 ) sec_buf[f] = sec_buf[f+1] = 0;
	    }
	  noerr = TOS_error( Rwabs( 1, sec_buf, n, i, dirdrv ), 0 );
	}
      memclr( sec_buf, bp->recsiz );
      for( i=0; i<bp->rdlen && noerr; i++ )
	noerr = TOS_error( Rwabs( 1, sec_buf, 1, i+bp->fatrec+bp->fsiz,
	    dirdrv ), 0 );
      lfree(sec_buf);
      force(path[0]);
    }
    arrow();
  }
  else return(0);
  return(1);
}
/*********************************************************************/
TREE *last_tree;
void treeini0( char *path, TREE *tree )
{
  register int hand;
  char temp[]="x:\\*.*";

  last_tree = tree;
  tree->tree_lev = 0;
  Fsetdta( tree->tree );
  strcpy( tree->tree_path, path );
  strcat( tree->tree_path, *(path+strlen(path)-1)=='\\' ? glob : globs );
  volname[0] = '\0';
  if( (neodesk_dat[0] = *path) == CLIP_LET ) strcpy( volname,
      msg_ptr[95] );		/* must always Fsfirst */
  if( (tree->tree_stat = cFsfirst( tree->tree_path, 0x37 ))==0 && *path!=CLIP_LET )
    if( (hand=cFopen(neodesk_dat, 0)) > 0 )
    {
      cFread( hand, 21L, volname );
      cFclose( hand );
    }
    else if( hand != AEFILNF && hand != AEACCDN/*003:GhostLink*/ && !tree->tree_stat )
        tree->tree_stat = hand;
  /* avoid bug in TOS 1.0/2; no disk in drive */
  else if( !ver_gt_12 && tree->tree_stat==AEFILNF ) tree->tree_stat=AEPTHNF;

  if( !volname[0] && (!tree->tree_stat || tree->tree_stat==AEFILNF) )
  {
    temp[0] = *path;
    if( !cFsfirst( temp, 8 ) ) strcpy( volname, tree->tree[0].d_fname );
    tree->tree_stat = cFsfirst( tree->tree_path, 0x37 );
  }
}
/*********************************************************************/
int tree_init( char *path, TREE *tree )
{
  if( !tree ) tree = &maintree;
  tree->folders = tree->files = tree->f_hidden = 0;
  tree->bytes_total = tree->bytes_hidden = 0L;
  treeini0( path, tree );
  return( tree->tree_stat==AEFILNF ? 1 : TOS_error( (long)tree->tree_stat, 0 ) );
}
/*********************************************************************/
int tree_next( TREE *tree )  /* return: -2 = moved down to next level */
{			     /* 	-1 = found "." or ".."	      */
			     /* 	-3 = moved up to previous     */
			     /* 	 1 = all done, 0 = file       */

  register int i, j, ret=0, new_stat;
  register char *ptr;

  if( !tree ) tree = &maintree;
  if( last_tree != tree )
  {
    last_tree = tree;
    Fsetdta( &tree->tree[tree->tree_lev] );
  }
  strcpy( tree->tree_fname, ptr=tree->tree[tree->tree_lev].d_fname );
  memcpy( &tree->tree_curr, &tree->tree[tree->tree_lev], sizeof(DTA) );
  tree->tree_fsize = tree->tree[tree->tree_lev].d_length;
  tree->tree_date = *(long *)&tree->tree[tree->tree_lev].d_time;
  tree->tree_att = tree->tree[tree->tree_lev].d_attrib;
  if( tree->tree_stat<0 )
    if( !tree->tree_lev ) return(1);
    else
    {
      if( (ptr = strrchr(tree->tree_path,'\\')) != 0 )
      {
	*ptr = '\0';
	iso(tree->tree_path);
      }
      Fsetdta( &tree->tree[--(tree->tree_lev)] );
      ret = -3;
    }
  else if( tree->filt &&
      !filter_it( tree->filt, &tree->tree[tree->tree_lev], tree->filt_templ ) )
      ret = -1;
  else if( tree->tree_att & S_IJDIR )
  {
    if( *ptr!='.' || *(ptr+1)!='\0' && *(ptr+1)!='.' )
      if( tree->tree_lev>=8 ) f_alert1( msg_ptr[60] );
      else
      {
	iso(tree->tree_path);
	strcat( tree->tree_path, ptr );
	strcat( tree->tree_path, globs );
	Fsetdta( &tree->tree[++(tree->tree_lev)] );
	tree->folders++;
	tree->tree_stat = cFsfirst( tree->tree_path, 0x37 );
	return( -2 );
      }
    ret = -1;	/* found . or .. or went down too many folders */
  }
  else
  {
    tree->files++;
    tree->bytes_total += tree->tree[tree->tree_lev].d_length;
    if( tree->tree_att & (S_IJHID|S_IJSYS) )
    {
      tree->f_hidden++;
      tree->bytes_hidden += tree->tree[tree->tree_lev].d_length;
    }
  }
  if( (new_stat = cFsnext()) != AENMFIL )
    if( !TOS_error( (long)new_stat, 0 ) ) return(1);
  tree->tree_stat = new_stat;
  return(ret);
}
/********************************************************************/
void unset_w(void)
{
  register int i, j;
  FSTRUCT *fs;

  if( num_w_active >= 0 && num_w_active==w_num )
  {
    for( i=0, fs=z->file[num_w_active]; i<items[num_w_active]; i++, fs++ )
      fs->state = 0;
    no_wactive();
    set_window();
    redraw_arrows();
  }
}
/*********************************************************************/
void bad_wind( int num )
{
  int n;
  
  over_112[num] = items[num] = total[num] = exe_boot[num] = 0;
  strcpy( z->w[num].path+3, glob );
  redraw_mover( z->w[num].place );
  free_files(num);
  free_npis(num);
  n = w_num;
  w_handle = wxref[w_num=num];
  rdrw_all();
  w_handle = wxref[w_num=n];
}
void update_wnum( int wnum, char *path )
{	/* only used by icon editor */
  int w = w_num;
  
  w_num = wnum;
  update_drive( path, 0 );
  w_num = w;
}
void update_drive( char *path, int keep )
{
  int oldh, oldn, type, oldh2, flag=1;
  register int i, j;
  char temp[120], err;
  static char done[7];

  if( *path /* 002: && w_num >= 0 */)
  {
    if( iconedit && *path==ICON_LET ) *path='\0';
    if( !keep ) memclr( done, sizeof(done) );
    remove_clip();
    strcpy( temp, path );
    if( *path )
    {
      get_volname( path, 1 );
      if( (err = maintree.tree_stat<0 && maintree.tree_stat != AEFILNF) != 0 ) temp[3] = '\0';
    }
    else err = maintree.tree_stat = 0;
    oldh = w_handle;
    oldn = w_num;
    type = ed_wind_type(oldn);
    while( flag ) for( flag=0, w_num=0; w_num<7; w_num++ )
      if( (w_handle = wxref[w_num]) >= 0 )
      {
	if( !done[w_num] && ((!iconedit || type<EDW_FILE) &&
	    intersect( temp, z->w[w_num].path ) || iconedit && type>=EDW_FILE &&
	    !strcmp( temp, z->w[w_num].path )) )
	{
	  set_wfile();
	  re_name = 1;
	  set_window();
	  if( w_num == num_w_active ) no_wactive();
	  if( !err )
	  {
	    first(0);
	    while( !valid_path )
	    {
	      backup(0);
	      flag = 1;
	      temp[3] = '\0';
	    }
	    done[w_num] = 1;
/*%	    for( j=0; j<7; j++ )
	      if( (i=wxref[j]) >= 0 )
		if( !done[j] && !strcmp( z->w[j].path, z->w[w_num].path ) ) /*% &&
		    z->w[j].fold_flag == z->w[w_num].fold_flag ) fix for Filters */
		{
		  while( z->num_files[j] < z->num_files[w_num] &&
		      (err = add_file(j) < 0) == 0 );
		  if( err )
		  {
		    bad_wind(j);
		    over_112[j] = 1;
		    continue;
		  }
		  memcpy( z->file[j], wfile, (long)witems*sizeof(FSTRUCT) );
		  items[j] = witems;
		  total[j] = total[w_num];
		  exe_boot[j] = exe_boot[w_num];
		  over_112[j] = over_112[w_num];
		  if( !z->showicon[w_num] && z->showicon[j] )
		      get_icn_matches(j);
		  oldh2 = w_handle;
		  w_handle = i;
		  if( (w_num = j) == num_w_active ) no_wactive();
		  set_wfile();
		  rdrw_all();
		  w_num = wind_xref(w_handle = oldh2);
		  done[j] = 1;
		} */
	  }
	  else
	  {
	    witems = 0;
	    bad_wind(w_num);
	  }
	}
      }
    w_handle = oldh;
    w_num = oldn;
    set_wfile();
  }
}
/********************************************************************/
void update_othwind( int num, int draw )
{
  int h;

  h = w_handle;
  if( num<0 || ed_wind_type(num) == EDW_ICONS )
  {
    get_all_icons();
    filename[0] = ICON_LET;
    filename[1] = '\0';
    first_no_close = 1;
    update_drive( filename, 0 );
    first_no_close = 0;
    if( draw ) do_desk();
  }
  else
  {
    w_num = num;
    set_wfile();
    find_handle();
    re_name = 1;
    first_no_close = draw;
    first(0);
    first_no_close = 0;
    if( (w_num = wind_xref(w_handle=h)) >= 0 )
    {
      set_wfile();
      set_window();
    }
  }
}
/********************************************************************/
void volname_redraw(void)
{
  redraw_obj( z->maximum, new_winobj+4 );
}
/********************************************************************/
void cdecl warmboot(void)
{
  Super((void *)0L);
  OS_restart();
}
/********************************************************************/
void cdecl coldboot(void)
{
  Super( (void *)0L );
  OS_memvalid = 	   /* Setting the flags to zero fools */
  OS_memval2  = 	   /* the OS into thinking the RAM is */
  OS_resvalid = 0L;	   /* bad. If a reset is performed it */
  OS_restart(); 	   /* will do a coldstart. */
}
/********************************************************************/
void set_conterm(void)
{
  if( !has_Geneva )
    if( conterm & (1<<3) ) con_set=1;
    else conterm |= (1<<3);
}
void reset_conterm(void)
{
  if( !has_Geneva )
    if( !con_set ) conterm &= ~(1<<3);
}
/********************************************************************/
void cdecl oupdate_drive( char *path )
{
  wind_update(BEG_UPDATE);	/* 004 */
  update_drive( path, 0 );
  wind_update(END_UPDATE);	/* 004 */
}
/********************************************************************/
void arrowed( int foo, char shift, int draw )
{
  int i, j, k;
  Rect box, box2, *r;
  register struct Wstruct *ws;

  wind_update( BEG_UPDATE );
  foo = foo / i_per_row[w_num] * i_per_row[w_num];
  if( foo > max_itm[w_num][snum] ) foo = max_itm[w_num][snum];
  if( foo < 0 ) foo = 0;
  ws = &z->w[w_num];
  if( (k = foo - ws->f_off.i[snum]) != 0 )
  {
    if( add_macro( MACWIND, MWARROW ) && add_macro( MACINT, foo ) &&
        add_macro( MACCHR, shift ) ) add_macro( MACCHR, snum );
    i = abs(k) / i_per_row[w_num];
    get_max_icon(w_num);
    get_voff();
    r = &ww[w_num][snum];
    box.x = box2.x = r->x;
    box.w = box2.w = r->w;
    if( is_icgroup(w_num) )
    {
      j = i<<4;
      box2.y = (box.y = r->y) + j;
      box.h = box2.h = r->h - j;
    }
    else
    {
      j = i * icon_voff;
      box2.y = (box.y = r->y + 2) + j;
      box.h = box2.h = r->h - j - 2;
    }
    if( box2.y+box2.h <= z->cliparray[3] && w_handle==real_top )
    {
      if( !shift && num_w_active == w_num )
      {
	if( box.h <= 0 ) unset_w();
	else de_act_w(-1,-1);
	info();
      }
      ws->f_off.i[snum] = foo;
      reset_icons();
      if( draw ) redraw_slider( snum, 1 );
      if( shift ) redraw_arrows();
      set_clip( z->cliparray, 1 );
      if( box.h <= 0 ) redraw( *r, 0, rows[w_num][snum]+1 );
      else if( k > 0 )
      {
	blit( &box2, &box, 0, 3, 0L );
	box.y += box.h;
	box.h = j;
	redraw( box, rows[w_num][snum]-i, i+1 );
      }
      else
      {
	blit( &box, &box2, 0, 3, 0L );
	box.h = j;
	redraw( box, 0, i );
      }
    }
    else
    {
      if( num_w_active == w_num )
	if( !shift )
	{
	  unset_w();
	  if( new_tab<0 ) info();
	}
	else redraw_arrows();
      ws->f_off.i[snum] = foo;
      reset_icons();
      if( draw ) redraw_slider( snum, 1 );
      rdrw_al0();
    }
  }
  arrow_tab();
  wind_update( END_UPDATE );
}
/********************************************************************/
int do_slid( int s, int mode )
{
  return graf_slidebox( wtree[w_num], s?WGVBIGSL2:WGVBIGSL,
      s?WGVSMLSL2:WGVSMLSL, mode );
}
void drag_slid( int s, char shift )
{
  int foo, i, o;
  WIND_TREE wt;

  snum = s;
  set_wfile();
  set_window();
  wind_update( BEG_UPDATE );
  if( w_handle != real_top || !z->real_time || graf_slidebox( 0L,
      (max_itm[w_num][s]+in_wind[w_num][s])/i_per_row[w_num],
      rows[w_num][s], 0x101 ) < 0 )
      arrowed( ((long)max_itm[w_num][s] * do_slid( s, 1 ) + 999)
      / 1000, shift, 1 );	/* 003: % in arrowed() */
  else
  {
    o = s?WGVSMLSL2:WGVSMLSL;
    wt.handle = w_handle;
    x_wind_tree( X_WT_GETCNT, &wt );
    wt.flag &= ~X_WTFL_RESIZE;
    wt.tree = wtree[w_num];
    x_wind_tree( X_WT_SET, &wt );
    objc_change( wtree[w_num], o, 0, Xrect(z->maximum),
       (i=wtree[w_num][o].ob_state)|SELECTED, 1 );
    foo = do_slid( s, 0x201 );
    while( foo >= 0 )
    {
      arrowed( foo*i_per_row[w_num], shift, 0 );
      foo = do_slid( s, 0x301 );
    }
    objc_change( wtree[w_num], o, 0, Xrect(z->maximum), i, 1 );
    wt.flag |= X_WTFL_RESIZE;
    x_wind_tree( X_WT_SET, &wt );
    redraw_slider( snum, 1 );
  }
  wind_update( END_UPDATE );
}
/********************************************************************/
#pragma warn -par
int cdecl trash_files( char *path, int *file, int *fold )
{	/* 003: fixed significantly */
  char msg[123], *s, *d;
  int r;
  
  msg[2] = MDELFNC;
  strcpy( msg+3, path );
  s = path+(r=pathend(path));
  d = msg+3 + r;
  *d++ = 0;
  strcpy( d, s );
  strcat( d, slash );
  no_fyi = 1;
  r = add_copy( msg, 3+strlen(msg+3)+1+strlen(d)+1 );
  no_fyi = 0;
  if( z->in_copy )
  {
    jog_background = 1;
    send_dum_msg();
  }
  return !r;
}
#pragma warn +par
/********************************************************************/
void trash_init( char *path )
{
  char msg[123];
  unsigned char *ptr;
  SEL_ICON *s;
  SELICON_DESC i;
  int k, first=1, err=0, update=-1, qpos;
  GROUP_ITEM *gi, *gip;
  FSTRUCT *fs;
  
  path[0] = 0;
  msg[2] = MDELF;
  i.icons = 0L;
  if( w_active>=0 && ed_wind_type(num_w_active)>=EDW_ICONS )
  {
    if( !z->conf_del || (*icic->del_conf)() )
      for( fs=&z->file[num_w_active][(k=items[num_w_active])-1]; --k>=0 &&
          !err; fs-- )
	if( fs->state ) err = !(*icic->delete_icon)( update=num_w_active, k );
  }
  else while( !err && (s=get_msel_icon( &i, 0, 0 )) != 0 )
  {
    if( s->wnum>=0 )
    {
      switch( ed_wind_type(s->wnum) )
      {
        case EDW_DISK:
          if( first )
          {
            strcpy( msg+3, z->w[s->wnum].path );
            iso(msg+3);
            if( !add_copy( msg, k=3+strlen(msg+3)+1 ) ) return;
            qpos = copyqlen - k;
          }
          strcpy( msg, s->u.fs->name );
          if( s->u.fs->type.p.pexec_mode==FOLDER ) strcat( msg, "\\" );
          if( !add_copy_str( msg, k=strlen(msg)+1 ) ) return;
          ptr = copy_q+qpos;
          k += (*ptr<<8) + *(ptr+1);
          *ptr++ = k>>8;
          *ptr = k;
          if( z->in_copy ) jog_background = 1;
          break;
        case EDW_GROUP:
	  for( gi=group_start[s->wnum], gip=0L; gi && gi!=s->u.fs->grp_item;
	      gip=gi, gi=gi->next );
	  if( gi )
	  {
	    if( !gip ) group_start[s->wnum] = gi->next;
	    else gip->next = gi->next;
	    lfree(gi);
	    group_desc[s->wnum]->hdr.entries--;
 	    update = s->wnum;
          }
	  break;
      }
    }
    else
    {
      remove_icon( s->u.desk_item-1 );
      rmv_icon_redraw( s->u.desk_item-1 );
    }
    first = 0;
  }
  if( update>=0 ) update_othwind( update, 0 );
  cmfree( (char **)&i.icons );
}
/********************************************************************/
void tab_new( int off, int dir )
{  /* dir:  0=x, 1=y, -1=both */
  FSTRUCT *f;
  int i, x, y, x0, y0, cx, cy;
  static char nest;
  
  if( is_icgroup(w_num) )
  {
    f = wfile;
    i = w_active>=0 ? w_active : 0;
    x0 = f[i].x;
    y0 = f[i].y[1];
    if( off>=-1 && off<=1 ) x = y = off*32000;
    else
    {
      x = x0;
      y = y0;
    }
    for( i=0; i<witems; i++, f++ )
      if( i!=w_active )
      {
        cx = f->x;
        cy = f->y[1];
        if( !dir )
        {
          if( cy>y0-20 && cy<y0+20 )
            if( off>1 )			/* end of x line */
            {
              if( cx>x ) goto ok;
            }
            else if( off==1 )		/* next x */
            {
              if( cx>x0 && cx<x ) goto ok;
            }
            else if( off<-1 )		/* start of x line */
            {
              if( cx<x ) goto ok;
            }
            else if( cx<x0 && cx>x ) goto ok;	/* prev x */
        }
        else if( dir>0 )
        {
          if( cx>x0-60 && cx<x0+60 )
            if( off>1 )
            {
              if( cy>y ) goto ok;
            }
            else if( off==1 )
            {
              if( cy>y0 && cy<y ) goto ok;
            }
            else if( off<-1 )
            {
              if( cy<y ) goto ok;
            }
            else if( cy<y0 && cy>y ) goto ok;
        }
        else if( off>0 )  		/* go to end */
        {
          if( cx>x || cy>y ) goto ok;
        }
        else if( cx<x || cy<y )		/* go to start */
        {
ok:       x = cx;
          y = cy;
          new_tab = i;
        }
      }
    if( new_tab<0 )
    {
      if( nest ) return;
      nest++;
      if( !dir )
        if( off==1 || off==-1 )
        {
          tab_new( off>0 ? -30000 : 30000, 0 );
          if( new_tab>=0 ) w_active = new_tab;
          nest++;
          tab_new( off, 1 );
          w_active = -1;
        }
      nest = 0;
    }
  }
  else new_tab = w_active + off;
  if( new_tab < 0 ) new_tab = 0;
  else if( new_tab >= z->num_files[w_num] ) new_tab = z->num_files[w_num] - 1;
  if( w_active>=0 && nest<=1 ) select_w( w_active, 0, w_handle, 1 );
  in_tab = w_num;
  tab_str[0] = 0;
}
int tab_grppos(void)
{
  int i, foo;
  
  snum = 1;
  foo = z->w[w_num].f_off.i[1];
  get_max_icon(w_num);
  if( (i=ww[w_num][1].y-wfile[new_tab].y[1]) > 0 ) foo -= (i+15)>>4;
  else if( (i=wfile[new_tab].y[1]+max_icon.h-1-
      ww[w_num][1].y-ww[w_num][1].h) > 0 ) foo += (i+15)>>4;
  return foo;
}
void tab_init( int w, int n )
{
  new_tab = n;
  if( is_icgroup(w) )
  {
    if( !n/*004*/ )
    {
      tab_new( -30000, -1 );
      n = tab_grppos();
    }
    cond_arrow(n);
    in_tab = w_num;
  }
  else
  {
    cond_arrow( n );		/* 003: position to selected icon */
/*%    select_w( n, 1, w_handle, 1 );
    info();
    new_tab = -1; */
    in_tab = w_num;
  }
}
void enter_tab(void)
{
  int pl, w, i;
  
  /* disable in icon editor for now (until scan_tab can be made to work) */
  if( !w_open || iconedit ) return;
  if( in_tab>=0 )	/* 004: exit upon Tab second time */
  {
    deact_info();
    return;
  }
  set_wfile();
  pl = z->w[w_num].place;
  if( in_tab>=0 || !witems )
    if( (pl=1)==w_open ) return;
  if( w_active>=0 && w_active<MANY_ACTIVE &&
      z->w[w=num_w_active].place==w_open )		/* 003: position to selected icon */
  {
    w_handle = wxref[w_num=w];
    set_wfile();
    tab_str[0] = 0;
    tab_init( w, w_active );
    return;
  }
  de_act( -1, -1 );
  tab_str[0] = 0;
  for( w=0; w<7; w++ )
    if( z->w[w].place==pl )
    {
      i = wxref[w];
      if( pl != w_open ) new_top(i);
      w_handle = i;
      w_num = w;
      set_wfile();
      if( !witems )
      {
        in_tab = new_tab = -1;
        return;
      }
      tab_init( w, is_icgroup(w) ? 0 : z->w[w].f_off.i[1] );	/* 004: conditional */
      return;
    }
}
int has_match(void)
{
  FSTRUCT *f;
  int i, l;
  
  l = strlen(tab_str);
  for( f=wfile, i=-1; ++i<witems; f++ )
    if( !strncmpi( file_text(f,0L), tab_str, l ) ) return i;
  return -1;
}
void start_rename(void)
{
  add_macro( MACCHR, MRENF );
  start_form( RENF_FORM );
}

void scan_tab( int ch, int sc, int sh )
{
  char *ptr;
  int len, i, j, old, old_tab;

  if( sc==0x1c || sc==0x72 )
  {
    old = slashes(ptr=z->w[w_num].path);
    old_tab = w_active;
    i = w_handle;
    if( sh==4 )
      if( ed_wind_type(w_num) == EDW_GROUP ) use_menu( w_handle, GWIMCHNG );	/* 003: was use_gmenu */
      else start_rename();
    else use_gmenu( w_num, /* 003: was w_handle, used GWIMOPEN for GRP */
        ed_wind_type(w_num)>=EDW_ICONS ? IFILOPEN :
        (iconedit ? IWIMOPEN : WIMOPEN) );
    if( in_tab<0 && wget_top()==w_handle )
      if( w_handle!=i || slashes(ptr)!=old ) enter_tab();
      else
      {
        select_w( old_tab, 1, w_handle, 1 );
        in_tab = w_num;
        info();
      }
  }
  else if( sc==0x61 && !sh )	/* 003 */
  {
    tab_str[0] = 0;
    info();
  }
  else if( (ch = (unsigned char)ch) != 0 )
  {
    len = strlen(tab_str);
    if( ch >= 'a' && ch <= 'z' ) ch -= 'a'-'A';
    if( ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch=='.' ||
        strchr(fvalid,ch) )
    {
      if( len == sizeof(tab_str)-1 ) return;
      tab_str[len++] = ch;
      tab_str[len]=0;
      set_wfile();
      if( (new_tab=has_match()) >= 0 )
      {
        snum=1;
        set_window();
        select_w( w_active, 0, w_handle, 1 );
        in_tab = w_num;
        cond_arrow( new_tab );	/* 003: now does % in arrowed() */
      }
      else tab_str[len-1]=0;
    }
    else if( ch=='\b' && len )
    {
      tab_str[len-1] = 0;
      info();
    }
  }
}
/********************************************************************/
void new_colors(void)
{
  int i, j, *s;
  OBJECT **o, *o2;

  for( i=0, o=wtree; i<7; i++ )
    if( (o2=*o++) != 0 )
    {
      s = z->gui_settings.wstates[z->gui_settings.wcolor_mode];
      for( j=WGSIZE+1; --j>=0; o2++ )
        o2->ob_state = *s++;
    }
}
/********************************************************************/
int events_ignored(void)
{
  if( ignore_events>=0 )
  {
    RING_BELL;
    new_top( ignore_events );
    return 1;
  }
  return 0;
}
#define W_TYPE2 (FULLER|CLOSER|NAME|MOVER|SIZER|UPARROW|DNARROW|VSLIDE|LFARROW|RTARROW|HSLIDE)
void fix_dtop(void)
{
  if( z->wind_pos.w > z->maximum.w ) z->wind_pos.w = z->maximum.w;
  if( z->wind_pos.x > z->maximum.w-bar_w ) z->wind_pos.x = z->maximum.w-bar_w;
  if( z->wind_pos.h > z->maximum.h ) z->wind_pos.h = z->maximum.h;
  if( z->wind_pos.y > z->maximum.y+z->maximum.h-1 ) z->wind_pos.y =
      z->maximum.y+z->maximum.h-1;
}
void init_desktop( int old, int setup )
{
  int new, i, x, y, dum, redo;
  static int old_h, old_t;
  char keys;
  
  new = z->is_acc ? 1 : z->desk_in_wind;
  if( setup ) rebuild_desk();
  if( old && new==dtop_wind )
  {
    if( new )
    {
      fix_dtop();
      if( dtop_handle>0 ) wind_set( dtop_handle, WF_CURRXYWH, Xrect(z->wind_pos) );
    }
    return;
  }
  old = dtop_wind;
  if( (dtop_wind=new) != 0 )
  {
    if( setup<0 && z->is_acc/*003*/ ) return;	/* 002: don't open DA window at start */
    fix_dtop();
    if( (dtop_handle=x_wind_create( W_TYPE2, X_MENU, Xrect(z->wind_pos) )) <= 0 )
    {
      f_alert1( msg_ptr[48] );
      dtop_wind = 0;
      dtop_handle = 0;
    }
  }
  if( dtop_wind )
  {
    if( !old_t )	/* might be resetting after run prg */
    {
      old_h = menu[PULL1].ob_height;
      old_t = menu[PULL1].ob_tail;
      menu[PULL1].ob_height = menu[ABOUT].ob_height;
      menu[PULL1].ob_tail = ABOUT;
      menu[ABOUT].ob_next = PULL1;
      redo = 0;
    }
    else redo = 1;
    dtop_wind = 0;	/* turn off temporarily */
    _menu_bar( menu, 0, 0 );
    set_newdesk( 0L, 0 );
    if( !z->multitask && !z->is_acc/*002*/ )
    {
      clear_menu();
      form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect(z->maximum) );
    }
    dtop_wind = 1;
    do_desk();
    keys = protect_keys( menu, 1 );
    wind_set( dtop_handle, X_WF_MENU, menu );
    if( keys ) menu[0].ob_state |= X_MAGIC;
    if( !redo )
    {
      wind_get( dtop_handle, WF_WORKXYWH, &x, &y, &dum, &dum );
      z->desk[0].ob_x = x;
      z->desk[0].ob_y = y;
    }
    wind_set( dtop_handle, X_WF_DIALOG, z->desk );
    wind_set( dtop_handle, X_WF_DIALWID, 16 );
    wind_set( dtop_handle, X_WF_DIALHT, 16 );
    wind_set( dtop_handle, WF_BEVENT, 1 );
    menu_msg(0L);
    wind_open( dtop_handle, Xrect(z->wind_pos) );
    dtop_prev = z->maximum;
  }
  else if( old )
  {
    if( dtop_handle>0 ) close_deskwind();
    menu[PULL1].ob_height = old_h;
    menu[PULL1].ob_tail = old_t;
    menu[ABOUT].ob_next = ABOUT+1;
    _menu_bar( menu, 1, 0 );
    *(long *)&z->desk[0].ob_x = *(long *)&z->maximum.x;
    set_newdesk( z->desk, 0 );
    old_t = 0;
  }
  else if( setup ) set_newdesk( z->desk, 0 );
}
long get_OS(void)
{
  return OS_version;
}

void run_acc_prog( int type, char *path, char *tail, int *stat, int set_path )
{
  PROG_TYPE pt, pt2;
  int j;

  if( !path ) return;
  pt.i = 0;
  pt.p.set_me = 1;
  pt.p.clear_screen = 1;
  pt.p.show_status = 1;
  switch( type )
  {
    case 0:
      if( *path )
      {
	pt2 = prog_type( -1, spathend(path) );
	if( pt2.i ) pt = pt2;
	break;
      }
    case 1:
      break;
    case 3:
      pt.p.takes_params = 1;
    case 2:
      pt.p.tos = 1;
      break;
    case 4:
      pt.p.batch = 1;
      pt.p.takes_params = 1;
      break;
    default:
      pt2.i = type;
      if( pt2.p.set_me ) pt.i = type;
  }
  strcpy( filename, path );
  filename[j = pathend(filename)] = '\0';
  if( pt.p.return_status ) z->stat_return = path;
  else if( stat )
  {
    *stat = 0;
    z->stat_return = (char *)stat;
  }
  if( set_path ) de_act(-1,-1);		/* 004 */
  open_program( -1, path+j, *filename ? filename : NULL, pt,
      tail ? tail : "\0", 0L, 1, set_path/*004*/ );
}

#ifdef DEMO
long savecount=200*10*60L;

void my_saver( int onoff )
{
  if( onoff>1 ) savecount++;
}
#endif DEMO

int _fornkey( char *k, char c )		/* 004 */
{
  char *s;

  if( (s = memchr( k, c, 128 )) != 0 ) return s-k;
  return -1;
}

char fornkey( KEYTAB *k, char c )	/* 004 */
{
  int i;

  if( (i = _fornkey( k->unshift, c )) > 0 ) return i;
  return _fornkey( k->shift, c );
}

void foreign_keys(void)		/* 004 */
{
  KEYTAB *k = Keytbl( (void *)-1L, (void *)-1L, (void *)-1L );

  key_minus = fornkey( k, '-' );
  key_equals = fornkey( k, '=' );
  key_B = fornkey( k, 'B' );
  key_slash = fornkey( k, '\\' );
}

ICHOOSE ichoose;

#ifndef DEMO
INF_CONV inf_conv = { INFCONV_VER, 0L, inf_load, scan_inf_line, wrap_load };
#endif
NEO_ACC nac = { NAC_VER, 0L, &_abort, &bconws, &blank_box, &blit, &check_dir,
		&check_prn, &close_all, &close_form, &copy_init, &copy_free,
		&c_buf, &c_curbuf, &c_buflen, &c_bufmax, &copy_files,
		&copy_a_buffer, &copy_a_file, &moving, 0L, &draw_box,
		&ndraw_icon, &gtext, &list_files, &bytecpy, &byteswap,
		&print_file, &redraw_desk, 0L, &rubber_box,
		&set_clip, &set_clp_rect, &oupdate_drive, &TOS_error,
		&trash_files, &warmboot, &coldboot, &sscnf, 0L,
		&save_desktop, &reset_desktop, &setcolor, &draw_image,
                &test_rez, &set_temps, &xfix_cicon, &drvmap };
extern unsigned char floydbytes[65][8];
EMULTI emulti = { 0, 2, 1, 1,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 100, 0 };
void multi_fake( int apid, int w_handle );

#if defined DEBUG
int main( int argc )
#else
int main(void)
#endif
{
  int i, j, k, l, m, foo,
      buffer[8], cmsg[8],
      flag_redraw, key;
  long stack;
  unsigned char *mac;
  char temp[120], tmpf[120], wait, shift, control, alt, err, *ptr,
      nomacerr;
  ERRSTRUC errstruc = { IEACCDN };
  Rect box;
  register struct Wstruct *ws;
  register Rect *r;
  FSTRUCT *fs;
  PROG_TYPE pt;

  init_appl(0);

  if( getcookie( LOAD_COOKIE, (long *)&lc ) != CJar_OK || (mas = lc->mas) == 0 ||
#ifndef DEBUG
      mas->ver != NEO_VER ||
#endif
      lc->ver != LOADER_VER ||
      (*mas->read_messages)( "NEODESK2.MSG", EXE_MSGS, msg_ptr[0],
      msg_ptr[EXE_MSGS], &msg_ptr[0], &new_msgs ) ) stop_it();
  errstruc.str = msg_ptr[63];
  if( getcookie( _FDC_COOKIE, &stack ) != CJar_OK ) fdc_level=0;
  else fdc_level = stack>>24L;
  z = mas->most;
#ifndef DEMO
  z->inf_conv = &inf_conv;
#endif
  z->new_cache = -1;
  z->msg_ptr = msg_ptr;
  if( mas->state==MINIT )
    for( i=0; i<6; i++ )
    {
      z->wind_font[i].id = 1;
      z->wind_font[i].size = 9;	/* defaults */
    }
  npi_argv(1);
  free_exec();
#ifdef DEBUG
  stack = (long)Setexc( 0x404/4, (void (*)())-1L );
  if( *(long *)stack == 0x70ff4e75L )
  {
    (long)old_crit = stack;
    Setexc( 0x404/4, new_crit );
  }
  for( i=0; i<7; i++ )
    if( z->w[i].place>0 ) z->w[i].place = -z->w[i].place;
  if( /*from_debugger=*/ argc != 0 && !z->use_master )
  {
    z->use_master = MASTER_HALFON;
    z->wind_pos = z->maximum;
  }
  else
#endif
  z->ichoose = &ichoose;
  z->kbio = Iorec(1);
  ver_gt_10 = (i=Supexec(get_OS)) > 0x0100;
  ver_gt_12 = i > 0x0102;
  ver_ge_20 = i >= 0x0200;
  if( (has_Geneva = getcookie( GENEVA_COOKIE, &stack ) == CJar_OK && stack!=0L) != 0 )
      Geneva_ver = ((G_COOKIE *)stack)->ver;	/* needed in init_gui */
/*%  if( _GemParBlk.global[1]!=-1 ) z->multitask=0;   prevents MagX from working */
  if( has_Geneva && Geneva_ver == 0x102 )
  {
    /* hack to prevent Geneva 003 from messing up USERDEFPRE's in windows */
    ptr = (char *)((long)((G_COOKIE *)stack)->aes_funcs[43-10] - 0x1e);
    if( *(int *)ptr==0x6616 )
    {
      *ptr = 0x60;
#ifdef DEBUG
      Cconws( "Fixed Geneva 003 objc_find. " );
#endif
    }
    ptr = (char *)((long)((G_COOKIE *)stack)->aes_funcs[50-10] - 0xb6);
    if( *(int *)ptr==0x673A )
    {
      *ptr = 0x6F;
#ifdef DEBUG
      Cconws( "Fixed Geneva 003 form_do. " );
#endif
    }
    ptr = (char *)(((G_COOKIE *)stack)->aes_funcs)+(0x600ae-0x5c8ae);
    if( *ptr == 0x54 )
    {
      *ptr = 0x5c;
#ifdef DEBUG
      Cconws( "Fixed Geneva 003 recalc" );
#endif
    }
    else if( *ptr != 0x5c )
    {
      ptr = (char *)(((G_COOKIE *)stack)->aes_funcs)+(0x113a3b8-0x11366fc);
      if( *ptr == 0x54 )
      {
        *ptr = 0x5c;
#ifdef DEBUG
        Cconws( "Fixed Geneva DB 003 recalc" );
#endif
      }
    }
    ptr = (char *)(((G_COOKIE *)stack)->aes_funcs)-22;
    if( *(int *)ptr == 0x2008 )
    {
      *(ptr+1) = 9;
#ifdef DEBUG
      Cconws( "Fixed Geneva 003 x_graf_rubberbox" );
#endif
    }
  }
  z->gemparblk = &_GemParBlk;
  nac.mas = mas;
  graphics = z->graphics;
  nac.gt_scr_ptr = graphics->screen_ptr;
  nac.font = graphics->font;
  vplanes = graphics->vplanes;		/***** 005: moved a bunch here */
  xmax = graphics->v_x_max;
  ymax = graphics->v_y_max;
  colvals = graphics->work_out[39];
  xasp = graphics->work_out[3];
  yasp = graphics->work_out[4];
  has_clut = graphics->has_clut && vplanes<=8;
  vwrap = ((long)xmax * vplanes) >> 3;
  if( colvals && colvals<=512 )
  {
    colmax = 0x777;
    colmax1 = 7;
  }
  else
  {
    colmax = 0xFFF;
    colmax1 = 0xF;
  }    /****/
/*%  colors = vplanes >= 4 ? 16 : (1<<vplanes);*/	/* 005: reworked, but removed because not used */
  init_screen();
  use_8x16 = graphics->cel_ht==16;
  has_sblast = CJar( 0, SBL_COOKIE, 0L ) == CJar_OK;
  *lc->has_cyrel = CJar( 0, CYREL_COOKIE, 0L ) == CJar_OK;	/* 005 */
  if( getcookie( _VDO_COOKIE, &stack )==CJar_OK )
  {
    stack >>= 16;
    TT_mono = (TT_vid = (int)stack==2)!=0 && Getrez()==2;
    falc_vid = (int)stack==3;
  }
  if( (vdi_hand = graphics->handle)<=0 ) vdi_hand = aes_vdi_hand;
  if( !init_gui( &nac, i=_GemParBlk.global[0] ) ) stop_it();
  aes_ge_14 = i >= 0x0140;
  aes_ge_20 = i >= 0x0200;
  aes_ge_40 = i >= 0x0400;
  Supexec( (long (*)())set_conterm );  /* relies on has_Geneva */
  fold_err.num = 1;
  fold_err.errstruc = &errstruc;
  foreign_keys();	/* 004 */

#if !defined(DEMO) || defined(DEBUG)
  load_rsc( "NEODESK.RSC", mas->state==MINIT );
#else DEMO
  load_rsc( "NEODEMO.RSC", mas->state==MINIT );
#endif DEMO

  if( !aes_ge_40 ) *(mas->open_wind) = (long)&top_bar;
  else shel_write( 9, 1, 0, 0L, 0L );
  nac.status_on = &z->status_on;
  clean_up();
  if( mas->state == MINIT )
  {
    reload( NULL );
    neg_places();
    get_d_icon(-1);
  }
  else if( z->use_master ) reset_desktop();
  load_fonts();
  load_desk_pic();
  all_inactive();
  new_inf_name();
  init_desktop( 0, mas->state==MINIT/*002*/ ? -1 : 1 );
  if( z->use_master == MASTER_HALFON ) z->use_master = 0;
  if( mas->state == MINIT )
    if( z->autoexec[0] )
    {
      if( z->autoexec[1] != ':' ) strcpy( temp, z->dflt_path );
      else temp[0] = '\0';
      strcat( temp, z->autoexec );
      iso(temp);
      i = pathend(z->autoexec);
      mas->state = BEXEC;
      pt = prog_type( -1, z->autoexec );
      if( pt.p.takes_params ) show_mouse();
      open_program( -1, z->autoexec+i, temp, pt, nil, 0L, 0, 1 );
    }
  w_num = -1;

  bconws( "\033H\033v\033b\057\033c\040\033q\r" );
  if( !dtop_wind )
  {
    clear_menu();
    _menu_bar( menu, 1, 0 );
    do_desk();
  }
  arrow();

  open_all( 0, 1 );
  get_top_wind();
  if( mas->state == MINIT ) icon_volnames();		/* 004 */
#ifdef USE_NEO_ACC
  neo_acc_init();
#else
  cli_demo_init();
#endif
  menu_id = z->menu_id;
  has_magx = z->has_magx;
#ifdef DEBUG
  z->other_pref.b.av_server = 1;
  set_avserv();
#endif
  _XAccSendStartup( "NeoDesk 4\0XDSC\0\61desktop\0\62DT\0",
       "NEODESK ", VA_SETSTATUS|VA_START,
       MSG_ASKFILEFONT|MSG_ASKOBJECT|MSG_OPENWIND|MSG_STARTPROG|MSG_STATUS|\
       MSG_COPY_DRAGGED|MSG_PATH_UPDATE, 0 /* could be X_MSG_TEXT, etc. */ );
  if( mas->state == MINIT || mas->state == BEXEC )
  {
    tmpf[0] = ' ';
    strcpy( tmpf+1, z->dflt_path );
    strcat( tmpf, "NEOAUTO.BAT" );
    if( !cFsfirst( tmpf+1, 0x37 ) )
      if( check_batch(1) )
      {
	mas->state = MEXEC;
	pt.i = 0;
/*%	wind_update( BEG_UPDATE );*/
	open_program( -1, NULL, NULL, pt, tmpf, 0L, 1, 1 );
/*%	wind_update( END_UPDATE );*/
      }
  }
  mas->state = MEXEC;
#ifdef DEMO
  *(lc->saver) = my_saver;  
#endif DEMO

  for(;;)
  {
#ifdef DEMO
    if( emulti.event!=MU_TIMER && savecount >= 10*60*200L )
    {
      f_alert1( msg_ptr[EXE_MSGS-3] );
      savecount = 0L;
    }
#endif DEMO
multi:
    macro_ind();
    j = z->macr_rec;
    z->macr_rec = 0;		/* don't record functions called from DA */
    emulti.type = z->macr_play ? (MU_MESAG|MU_TIMER) : (!just_open ?
	(MU_MESAG|MU_BUTTON|MU_KEYBD|X_MU_DIALOG) : MU_MESAG);
    if( update_forms(0) )
    {
      emulti.type |= MU_TIMER;
      if( z->macr_play ) emulti.low = 100;
      else emulti.low = (5-z->back_speed)*z->speed;
    }
    multi_evnt( &emulti, buffer );
    z->macr_rec = j;
    emulti.mouse_k &= 0xf;
    if( z->macr_play && (Getshift()&3)==3 ) z->macr_play = 0;
    macro_ind();
    if( z->macr_play && !(emulti.event&MU_MESAG) && ignore_events<0 )
    {
      key = 0;
      i = main_macro( buffer );
      ws = &z->w[w_num];
      switch(i)
      {
	case MDUPLIC:
	  goto duplic;
	case MWTOP:
	  goto w_topped;
	case MWMOVE:
	  goto w_moved;
	case MWSIZE:
	  goto w_sized;
	case MWFULL:
	  goto w_fulled;
	case MWSPLIT:
	  goto split;
	default:
          goto bottom;
      }
    }
/*%    if( !z->macr_play ) wind_update( BEG_UPDATE );*/
    get_top_wind();
    ws = &z->w[w_num];
    flag_redraw = 0;
    if( emulti.event & MU_MESAG && !_XAccComm( buffer ) )		/* 003 */
          switch( buffer[0] )
    {
      case AV_SENDKEY:
	emulti.event &= ~MU_MESAG;
	if (emulti.type & MU_KEYBD)
	{
	  emulti.event |= MU_KEYBD;
	  emulti.mouse_k = buffer[4];
	  emulti.key = buffer[3];
	}
	break;
      case ACC_KEY:
	emulti.event &= ~MU_MESAG;
	if (emulti.type & MU_KEYBD)
	{
	  emulti.event |= MU_KEYBD;
	  emulti.mouse_k = buffer[4];
	  emulti.key = buffer[3];
	  XAccSendAck(buffer[1],1);
	}
	else XAccSendAck(buffer[1],0);
	break;
      case ACC_META:
/*	if (_xacc_msgs & X_MSG_META) break;  I ignore all Group 1's, so just ack */
      case ACC_IMG:
/*	if (_xacc_msgs & X_MSG_IMG) break; */
      case ACC_TEXT:
/*	if (_xacc_msgs & X_MSG_TEXT) break; */
	XAccSendAck(buffer[1],0);
	emulti.event &= ~MU_MESAG;
	break;
    }
    emulti.mouse_k &= 0x0F;
    shift = emulti.mouse_k & 3;
    control = emulti.mouse_k & 4;
    alt = emulti.mouse_k & 8;
    if( emulti.event & MU_KEYBD && !events_ignored() )
    {
      k = emulti.key>>8;
      if( !(iconedit | z->macr_rec) && z->macro_len != 0 )
      {
	mac = z->macro;
	while( *mac || *(mac+1) )
	  if( *(mac+2) == emulti.mouse_k && *(mac+3) == k )
	  {
	    z->macptr = (z->macstrt = mac-z->macro) + sizeof(int) + sizeof(int) + 21;
	    z->macr_play = 1;
	    goto mumesag;
	  }
	  else mac += chtoi(mac);
      }
      if( !reorder_on && group_run( k, emulti.mouse_k ) ) goto mumesag;
      if( shift )
	if( k>=2 && k<=10 ) emulti.key = k+'0'-1;
	else if( k==0xB ) emulti.key = '0';
	else if( emulti.key>='a' && emulti.key<='z' ) emulti.key &= 0x5F;
      emulti.key = (char)emulti.key;
      if( k == key_minus && emulti.mouse_k == 4 && !reorder_on )/* ^- */
	  neo_ac_open( neocntrl );
      else if( k == key_equals && emulti.mouse_k == 4 && !reorder_on ) /* ^= */
	  neo_ac_open( neoq );
      else if( k == 0x71 && emulti.mouse_k == 0x0E ) /* keypad . key */
	  (*mas->clear_mouse)();
      else if( k == 0x72 && emulti.mouse_k == 0x0E ) /* keypad Enter key */
      {
	wind_update( END_UPDATE );
	RING_BELL;
      }
      else if( k == 0x66 && emulti.mouse_k == 0x0E ) /* keypad * key */
      {
        for( i=0; i<16; i++ )
          setcolor( i, dflt_pall[i] );
      }
      else if( !(reorder_on | iconedit) && k == key_B && emulti.mouse_k == 4 )
	   /* ^B */
      {
	pt.i = 0;
	if( check_batch(0) ) open_program( -1, NULL, NULL, pt, nil, 0L, 1, 1 );
      }
      else if( k == 0x77 && w_num >= 0 )  /* ^Clr/Home */
      {
        if( emulti.mouse_k == 4 )	/* w/ Control: go to root */
  	  if( pathend(ws->path) > 3 ) new_path( 3, 1 );
      }
      else if( k == key_slash && emulti.mouse_k==8 && w_num >= 0 )  /* 003: Alt \ */
      {
  	if( pathend(ws->path) > 3 ) new_path( 3, 1 );
      }
      else if( k == 0x47 && w_num >= 0 )  /* Clr/Home */
      {
        i = shift ? 20000 : -20000;
        j = -1;
        goto page;
      }
      else if( k == 0x48 && emulti.mouse_k==shift && w_num >= 0 )
      { 					     /* up arrow key */
        i = shift ? -in_wind[w_num][1] : -i_per_row[w_num];  /* line/page up */
        if( shift && rows[w_num][1] > 1 ) i += i_per_row[w_num];
        j = 1;
	goto page;
      }
      else if( k == 0x50 && emulti.mouse_k==shift && w_num >= 0 )
      { 					     /* down_arrow key */
        i = shift ? in_wind[w_num][1] : i_per_row[w_num];
        if( shift && rows[w_num][1] > 1 ) i -= i_per_row[w_num];
        j = 1;
page:   foo = ws->f_off.i[1];
	if( in_tab==w_num )
        {
	  tab_new( i, j );
	  if( is_icgroup(w_num) )
	  {
	    if( new_tab>=0 ) foo = tab_grppos();
	  }
	  else
	  {
            while( (k=new_tab-foo) >= in_wind[w_num][1] )
              foo += i_per_row[w_num];
            if( k < 0 ) foo = new_tab;	/* 003: now does % in arrowed() */
          }
        }
	else foo += i;
        snum=1;
	set_window();
	set_wfile();		/* 003: used to goto arrowed instead */
	arrowed( foo, shift, 1 );
      }
      else if( k == 0x4B && emulti.mouse_k==shift && in_tab==w_num && w_num>=0 )
      {        	/* left arrow */
        i = shift ? (w_active/i_per_row[w_num]*i_per_row[w_num])-w_active : -1;
        j = 0;
        goto page;
      }
      else if( k == 0x4d && emulti.mouse_k==shift && in_tab==w_num && w_num>=0 )
      { 	/* right arrow */
        i = shift ? (w_active/i_per_row[w_num]*i_per_row[w_num])+
            i_per_row[w_num]-1-w_active : 1;
        j = 0;
        goto page;
      }
      else if( !(iconedit | reorder_on) && k>=2 && k<=0xb && emulti.mouse_k==4 )
      {
	k -= 2;
	if( z->neo_acc[k][0] ) neo_ac_open(z->neo_acc[k]);
      }
      else if( dtop_wind && main_equivs( &emulti.event,
          emulti.mouse_k, k, buffer ) ) goto mumesag;
      else if( emulti.mouse_k == 4 )
  	  scan_tab( emulti.key, k, emulti.mouse_k );
      else if( k==0xf ) enter_tab();
      else if( !emulti.mouse_k && k==0x62 ) _x_help( 0, 0L, 0 );	/* Help key */
      else if( w_num==in_tab && in_tab>=0 ) scan_tab( emulti.key, k, emulti.mouse_k );
      else if( !reorder_on && !control && !alt && (emulti.key>='A' && emulti.key<='Z' ||
	  emulti.key>='a' && emulti.key<='z' || emulti.key>='0' && emulti.key<='9') )
      {
        if( dtop_wind && dtop_handle<=0 ) init_desktop( 0, 0 );
	for( j=0, i=0; i<z->num_icons; i++ )
	  if( z->idat[i].type >= 0 && z->idat[i].c == emulti.key )
	    if( z->idat[i].state & SELECTED && d_active >=0 &&
		d_active < MANY_ACTIVE ) open_d_icon(i);
	    else
	    {
	      de_act_other(1);
	      de_act_d( i+1 );
	      select_d( i+1, SELECTED );
	      break;
	    }
      }
    }
mumesag:
    snum = 0;
    if( emulti.event & MU_MESAG )
    {
      ws = &z->w[w_num = wind_xref(w_handle = buffer[3])];
      set_wfile();
      if( (unsigned int)buffer[0]==X_MN_SELECTED && dtop_handle>0 &&
          buffer[7]==dtop_handle ) buffer[0]=MN_SELECTED;
      switch(buffer[0])
      {
      case AP_TERM:
ap_term:z->quit_alert = 0;
	m_quit(0);
        stop_it();	/* just in case it's a DA */
	break;
      case SH_WDRAW:
	if( has_Geneva && buffer[4]==AES_handle ) break;
	if( buffer[3]>=0 ) d_update( buffer[3]+'A' );
	else
	  for( i=0, stack=drvmap(); i<26; i++ )
	    if( stack&(1L<<i) ) d_update( i+'A' );	/* 005: 1L */
	break;
      case 80:			/* for Geneva <= 006, old CH_EXIT */
        if( !has_Geneva || Geneva_ver >= 0x107 ) break;
      case CH_EXIT:
	if( buffer[4] )
	{
	  if( z->status_report==2 || z->status_report==1 && buffer[4]>0 ) break;
	  if( has_Geneva ) appl_search( -1, tmpf, &l, &m );
	  else m = -1;
	  spf( filename, msg_ptr[126],
	      m==buffer[3] ? tmpf : msg_ptr[127], buffer[4] );
	  if( buffer[4]<0 ) x_form_error( filename, buffer[4] );
	  else
	  {
	    spf( tmpf, filename, msg_ptr[128] );
	    for( i=0; i<DFLT_ERRORS; i++ )
	      if( buffer[4] == mas->dflt_errors[i].num )
	      {
		spf( tmpf, filename, mas->dflt_errors[i].str );
		break;
	      }
	    f_alert1( tmpf );
	  }
	}
	break;
      case RESCH_COMPLETED:
        /* a rez change I started is complete */
        if( buffer[3]==0 ) f_alert1( msg_ptr[99] );
        else
        {
          z->quit_alert = 0;
          m_quit(0);
          stop_it();	/* just in case it's a DA */
        }
        break;
      case AC_CLOSE:
	close_all_fwind( AES_handle, 1 );
	m_quit(-1);
	break;
      case AC_OPEN:
	if( dtop_handle<=0 ) init_desktop( 0, 0 );
	if( !w_open ) open_all( 0, 0 );
	break;
      case MN_SELECTED:
	get_top_wind();
        if( !events_ignored() )
  	  if( buffer[4] == DELITEM && !iconedit )
	    if( w_active>=0 )
	    {
              trash_init(temp);
	      de_act( -1, -1 );
	    }
	    else
  	    {
	      menu_tnormal( menu, buffer[3], 1 );
  	      i = -1;
	      goto desk_trash;
	    }
	  else use_menu( -1, buffer[4] );
	menu_tnormal( menu, buffer[3], 1 );
	break;
      case X_MN_SELECTED:
        if( buffer[7]==z->help.wind )
        {
          (*z->help.use_menu)( buffer[4], buffer[3] );
          break;
        }
	w_num = wind_xref(w_handle = buffer[7]);
        if( !events_ignored() )
	  if( w_num>=0 )
	  {
  	    set_wfile();
	    i = (j=ed_wind_type(w_num))==EDW_GROUP;
	    if( iconedit && (j==EDW_DISK && buffer[4]==IDELITM ||
	        j>=EDW_ICONS && buffer[4]==IFILDEL) ||
	        !i && buffer[4]==WIMDEL || i && buffer[4]==GWIMDEL )
	    {
	      menu_tnormal( wmenu[w_num], buffer[3], 1 );
	      if( w_active>=0 )
	      {
                trash_init(temp);
	        de_act( -1, -1 );
	      }
	      else
	      {
  	        i = -1;
	        goto desk_trash;
	      }
	    }
	    else use_menu( buffer[7], buffer[4] );
	  }
	  else use_menu( buffer[7], buffer[4] );
	i = wind_xref(buffer[7]);
	if( iconedit || i>=0 && z->w[i].place>0 )	/* otherwise undraws when ^W w/o G */
	    menu_tnormal( *(OBJECT **)&buffer[5], buffer[3], 1 );
	break;
      case X_WM_OBJECT:
        if( events_ignored() ) break;
	if( buffer[4]==WGVSMLSL ) drag_slid( 0, shift );
	else if( buffer[4]==WGVSMLSL2 ) drag_slid( 1, shift );
	else if( buffer[4]==WGMOVE )
	{
	  i = emulti.mouse_x;
	  j = emulti.mouse_y;
	  emulti.times = ob_clicks;
	  if( emulti.times>=2 )
	  {
	    key=0;
	    if( (k=wtree[w_num][WGMOVE].ob_width-(strlen(ptr=ws->path)<<3))
		< 0 ) k = 0;
	    if( (j=(emulti.mouse_x-wtree[w_num][0].ob_x-wtree[w_num][WGMOVE].ob_x-(k>>1))>>3) <
		pathend(ptr)-1 )
	    {
	      ptr+=j;
	      while( *ptr++ != '\\' );
	      if( pathend(ptr) && ed_wind_type(w_num) == EDW_DISK &&
		  check_reorder() ) new_path( ptr-ws->path, 1 );
	    }
	    break;
	  }
	  *(long *)&buffer[6] = *(long *)&ws->w;
#ifdef OFF_LEFT
	  if( !graf_dragbox( buffer[6], buffer[7], ws->x,
	      ws->y, z->maximum.x-buffer[6], z->maximum.y,
	      z->maximum.w+(buffer[6]<<1), z->maximum.h+buffer[7],
	      &buffer[4], &buffer[5] ) ) break;
#else
	  if( !graf_dragbox( buffer[6], buffer[7], ws->x+(emulti.mouse_x-i),
	      ws->y+(emulti.mouse_y-j), z->maximum.x, z->maximum.y, z->maximum.w<<1,
	      z->maximum.h<<1, &buffer[4], &buffer[5] ) ) break;
#endif
	  goto moved;
	}
	else switch( buffer[4]-new_winobj )
	{
	  case 1:
duplic:     if( ed_wind_type(w_num)==EDW_DISK && check_reorder() )
	    {
	      add_macro( MACCHR, MDUPLIC );
	      strcpy( filename, z->w[w_num].path );
	      isolate();
	      open_to_path(filename);
	    }
	    break;
	  case 2:
	    if( !iconedit )
	    {
	      add_macro( MACWIND, MWIOT );
	      use_gmenu( w_num, z->showicon[w_num] ? SHOWTEXT : SHOWICON );
	    }
	    break;
	  case 3:
	    add_macro( MACWIND, MWSALL );
	    select_all();
	    break;
	  case 4:
	    if( emulti.mouse_k==4 && ws->place==w_open &&
		ed_wind_type(w_num)<=EDW_DISK && z->w[w_num].path[0]!=CLIP_LET &&
	        check_reorder() )
	    {
	      offset_objc( wtree[w_num], new_winobj+4, &vol_tree[0].ob_x,
		  &vol_tree[0].ob_y );
	      *(long *)&vol_tree[0].ob_width =
		  *(long *)&vol_tree[1].ob_width =
		  *(long *)&wtree[w_num][new_winobj+4].ob_width;
	      vol_ted.te_ptext = w_volname[w_num];
	      strcpy( filename, w_volname[w_num] );
              wind_lock(1);
	      objc_draw( vol_tree, 1, 0,  0, 0, 0, 0 );
	      obj_selec( vol_tree, 0, 1, form_do( vol_tree, 1 ) );
              wind_lock(0);
	      if( strcmp( filename, w_volname[w_num] ) )
		  new_volname( w_num, w_volname[w_num], ws->path[0] );
	      else volname_redraw();
	      key=0;
	    }
	    break;
	}
	break;
      case X_WM_VSPLIT:
split:  if( events_ignored() ) break;
	if( add_macro( MACWIND, MWSPLIT ) ) add_macro( MACINT, buffer[4] );
	wind_set( buffer[3], X_WF_VSPLIT, z->w[w_num].split = buffer[4] );
	recalc_wind();
	s_reset_icons();
	redraw_slider( 0, 0 );
	redraw_slider( 1, 0 );
	redraw_arrows();
	break;
      case WM_MOVED:
moved:  if( buffer[3]>0 && buffer[3]==z->help.wind ) (*z->help.moved)(buffer);
	else if( w_num < 0 )
	{
d_moved:  wind_set( buffer[3], WF_CURRXYWH, buffer[4], buffer[5], buffer[6], buffer[7] );
	  if( buffer[3]==dtop_handle ) z->wind_pos = *(Rect *)&buffer[4];
	}
	else
	{
          if( events_ignored() ) break;
#ifdef OFF_LEFT
	  if( (buffer[4] < 0 || ws->x < 0) && ws->x + ws->w <
	      z->maximum.w && ws->y + ws->h < z->maximum.y+z->maximum.h )
	      flag_redraw = 1;
#endif
	  if( add_macro( MACWIND, MWMOVE ) && add_macro( MACINT,
	      buffer[4] ) ) add_macro( MACINT, buffer[5] );
w_moved:  box = *(Rect *)&ws->x;
	  *(Rect *)&ws->x = *(Rect *)&buffer[4];
	  wind_set( w_handle, WF_CURRXYWH, ws->x, ws->y, ws->w, ws->h );
	  box.w += 2;
	  box.h += 2;
	  close_form( box, -1 );
	  check_split( ws->split );
	  recalc_wind();
	  set_window();
	  info_text();
	  s_reset_icons();
	  redraw_slider( 0, 0 );
	  redraw_slider( 1, 0 );
	  if( flag_redraw ) redraw_wind( z->maximum, 1 );
	}
	break;
      case WM_SIZED:
	if( buffer[3]==dtop_handle ) goto d_moved;
        else if( buffer[3]>0 && buffer[3]==z->help.wind ) (*z->help.moved)(buffer);
	else
	{
          if( events_ignored() ) break;
	  if( add_macro( MACWIND, MWSIZE ) && add_macro( MACINT, buffer[4] )
	      && add_macro( MACINT, buffer[5] ) && add_macro( MACINT,
	      buffer[6] ) ) add_macro( MACINT, buffer[7] );
w_sized:  if( buffer[4] == ws->x && buffer[5] == ws->y && buffer[6] == ws->w
	      && buffer[7] == ws->h ) break;
	  if( buffer[7] <= ws->h )
	  {
	    if( buffer[6] <= ws->w )
	    {
	      if( buffer[4] == ws->x && buffer[5] == ws->y )
		  flag_redraw = 1;
	      if( !shift ) unset_w();
	    }
	    else if( buffer[7] != ws->h && !shift ) unset_w();
	  }
	  else if( buffer[6] < ws->w && !shift ) unset_w();
	  goto w_moved;
	}
	break;
      case X_WM_ARROWED2:
	snum = 1;
      case WM_ARROWED:
        if( events_ignored() ) break;
	set_wfile();
	set_window();
	wait = 1;
        do
        {
	  ws = &z->w[w_num];
	  foo = ws->f_off.i[snum];
	  switch( buffer[4] )
	  {
	    case WA_UPLINE:
	      foo -= i_per_row[w_num];
	      break;
	    case WA_DNLINE:
	      foo += i_per_row[w_num];
	      break;
	    case WA_UPPAGE:
	      foo -= in_wind[w_num][snum];
	      break;
	    case WA_DNPAGE:
	      foo += in_wind[w_num][snum];
	      break;
	  }
	  arrowed( foo, shift, 1 );
	  if( wait )
	  {
	    evnt_timer( 120, 0 );
	    wait=0;
	  }
	  graf_mkstate( &emulti.mouse_x, &emulti.mouse_y, &emulti.mouse_b, &emulti.mouse_k );
	}
	while( emulti.mouse_b&1 );
	break;
      case WM_REDRAW:
	if( just_open ) just_open--;
	if( w_num >= 0 ) redraw_wind( *(Rect *)&buffer[4], 1 );
	break;
      case WM_TOPPED:
w_topped:
	if( buffer[3]==dtop_handle ) wind_set( buffer[3], WF_TOP, 0, 0, 0, 0 );
	else if( !events_ignored() ) new_top( buffer[3] );
	break;
      case WM_CLOSED:
	if( buffer[3]==dtop_handle )
	  if( control || !z->is_acc ) m_quit(0);
	  else close_deskwind();
	else if( w_num<0 ) use_form( buffer[3], -1 );
	else if( !events_ignored() && check_reorder() )
	  if( control )
 	  {
   	    add_macro( MACWIND, MWCLOSE );
	    close_wind();
	  }
	  else backup(1);
	break;
      case WM_FULLED:
        if( buffer[3]>0 && buffer[3]==z->help.wind )
        {
          (*z->help.fulled)(buffer);
          break;
        }
	else if( buffer[3]!=dtop_handle )
	{
          if( events_ignored() ) break;
	  add_macro( MACWIND, MWFULL );
w_fulled: memcpy( &buffer[4], &prev.x, sizeof(prev) );
	  memcpy( &prev.x, &ws->x, sizeof(prev) );
	  goto w_sized;
	}
        memcpy( &buffer[4], &dtop_prev.x, sizeof(dtop_prev) );
	memcpy( &dtop_prev.x, &z->wind_pos, sizeof(dtop_prev) );
	goto d_moved;
      case NEO_ACC_ASK:
	if( buffer[3]==NEO_ACC_MAGIC ) acc_init1( buffer[4] );
	break;
#ifndef DEMO
      case NEO_ACC_EXC:
	if( buffer[3]==NEO_ACC_MAGIC )
	    run_acc_prog( buffer[2], *(char **)&buffer[4],
	    *(char **)&buffer[6], 0L, 0 );
	break;
#endif DEMO
      case NEO_ACC_QUI:
	if( buffer[3]==NEO_ACC_MAGIC ) m_quit( buffer[4] );
	break;
      case NEO_ACC_INF:
	if( buffer[3]==NEO_ACC_MAGIC && check_reorder() )
	{
	  really_reload_inf( *(char **)&buffer[4], -1, 1 );
#ifdef USE_NEO_ACC
	  neo_acc_init();
#else
	  cli_demo_init();
#endif
	}
	break;
      case NEO_ACC_INF2:
	if( buffer[3]==NEO_ACC_MAGIC )
	  do
	  {
	    buffer[4] = wrap_load( i=buffer[4], 1 );
	    buffer[1] = AES_handle;
	    buffer[0] = NEO_INF2_RES;
	    appl_pwrite( buffer[5], 16, buffer );
            if( !i && !buffer[4] ) loading_inf = 1;	/* 003 */
            if( loading_inf )				/* 003 */
              for(;;)
              {
                evnt_mesag(buffer);
                if( buffer[0]==NEO_ACC_INF2 && buffer[3]==NEO_ACC_MAGIC )
                  if( !buffer[4] ) continue;		/* 004: INF_LOAD sent msg again */
                  else break;
                if( buffer[0]==AP_TERM ) goto ap_term;
                appl_write( AES_handle, 16, buffer );
              }
	  }
	  while( loading_inf );
	break;
      case NEO_ACC_TXT:
	if( buffer[3]==NEO_ACC_MAGIC )
	{
	  open_text( *(char **)&buffer[4], *(char **)&buffer[6], 0 );
#ifdef USE_NEO_ACC
	  neo_acc_init();
#else
	  cli_demo_init();
#endif
	}
	break;
      case NEO_ACC_COPY:
	if( buffer[3]==NEO_ACC_MAGIC && buffer[5]>0 ) msg_copy( buffer );
	break;
      case NEO_ACC_DEL:
	if( buffer[3]==NEO_ACC_MAGIC && buffer[5]>0 ) msg_del( buffer );
	break;
      case NEO_MAC_PLAY:
      case NEO_MAC_ADD:
        rcv_mac_msg( buffer );
        break;
    }
    }
    if( emulti.event & MU_BUTTON && !events_ignored() )
    {
      if( (i = wind_find(emulti.mouse_x,emulti.mouse_y))
	   != dtop_handle ) /* Button pressed in window */
      {
	ws = &z->w[w_num = wind_xref(w_handle = i)];
	set_wfile();
	r = &ww[w_num][snum = emulti.mouse_y >= ww[w_num][1].y];
	set_window();
/*%	wait = 0;	003
	key = 0;
	do {	*/
	      k = find_w( emulti.mouse_x, emulti.mouse_y, i );
	      if( emulti.times==1 && emulti.mouse_b&1 )
		   /* pressed once, left still pressed */
	      {
		evnt_timer( 10, 0 );
		graf_mkstate( &dum, &dum, &emulti.mouse_b, &emulti.mouse_k );
	      }
	      if( !(emulti.mouse_b&1) || emulti.times==2 )
		  /* left button up or pressed twice*/
	      {
		if( !shift || num_w_active>=0 && w_num!=num_w_active &&
		    emulti.times!=2 )
		{
		  if( w_num != num_w_active ) de_act_other(0);
		  else de_act_w( k, w_num );
		  de_act_d(-1);
		}
		else if( emulti.times != 2 ) de_act_d( -1 );
		if( k>=0 )			  /* on an item in the window */
		{
		  foo = ed_wind_type(w_num);
		  if( shift && (!control||alt) && emulti.times<2 ) select_w( k,
		      wfile[k].state^SELECTED, i, 1 );
		  else select_w( k, SELECTED, i, !shift||control );
		  if( ws->split>0 ) redraw_arrows();
		  if( emulti.times==1 && control && !alt )
		    if( foo == EDW_GROUP ) use_menu( w_handle, GWIMCHNG );
		    else start_rename();
		  else if( emulti.times == 2 )
		    if( foo == EDW_GROUP && control && !alt )
		    {
		      strcpy( filename, ptr=wfile[k].grp_item->p.path );
		      isolate();
		      strcat( filename, glob );
		      search_open( filename, spathend(ptr) );
		    }
		    else if( control && wfile[k].type.p.pexec_mode==FOLDER )
		    {
		      get_full_name( filename, k, w_num );
		      strcat( filename, slash );
		      de_act( -1, -1 );
		      open_to_path(filename);
		    }
		    else open_w_icon( k );
		}
	      }
	      else if( emulti.times==1 )
	      {
		de_act_d( -1 );
		if( k>=0 )	/* click once on item in window */
		{
		  if( !shift && !wfile[k].state ||
		      num_w_active >= 0 && w_num != num_w_active )
		  {
		    de_act_other(0);
		    de_act_w( k, w_num );
		  }
		  select_w( k, SELECTED, i, 1 );
		  if( ws->split>0 ) redraw_arrows();
		  info();
		  j = w_num;
		  i = k;
		  switch( wdrag_box( &j, &i, emulti.mouse_x, emulti.mouse_y ) )
		  {
		    case -1:		/* 003 */
		      drag_free();
		      av_dragto();
		      break;
		    default:
		      drag_free();
		      break;
		    case 3:
		      drag_free();
		      switch(i)
		      {
			case -2:		/* parent */
			  strcpy( temp, z->w[w_num].path );
			  temp[uppath(temp)+1] = 0;
			  copy_from_w( -1, w_num, temp );
			  break;
			case -3:		/* check (showinf) */
			  m_showinf( 0, 1 );
			  break;
			case -4:		/* trash */
			  trash_init( temp );
		      }
		      de_act( -1, -1 );
		      in_drag = -1;
		      obj_true1( wtree[w_num], 0, new_winobj-i-1 );
		      redraw_obj( z->maximum, new_winobj );
		      break;
		    case 1:			     /* not on an object */
		      get_max_icon(-1);
		      if( j<0 && (!dtop_wind || dtop_handle>0 ) &&
		          check_iconedit() && check_reorder() )
		      { 			     /* on the desktop */
		        l = get_hilo( w_num, snum, &emulti.times );
			for( fs=&wfile[l], m=0; l<emulti.times; l++, fs++ )
			  if( fs->state )
			  {
			    /* make this a desk icon */
			    if( (k=add_desk()) >= 0 )
			    {
			      get_full_name( filename, l, w_num );
			      if( filename[0] == CLIP_LET )
			      {
				TOS_error( CLIP_ERR, 0 );
				break;
			      }
			      if( (foo=add_program( filename, &wfile[l].type )) >= 0 )
			      {
				z->idat[k-1].c = 0;
				strncpy( z->idat[k-1].label, file_text(&wfile[l],0L), 12 );
				z->idat[k-1].label[12] = 0;
				z->idat[k-1].type = foo + D_PROG;
				get_d_icon(foo);
				emulti.mouse_x = dbx[m].i0 - z->desk[0].ob_x;
				emulti.mouse_y = dbx[m].i1 - z->desk[0].ob_y;
				redraw_desk( (z->desk[k].ob_x = emulti.mouse_x) +
				    z->desk[0].ob_x, (z->desk[k].ob_y = emulti.mouse_y)
				    + z->desk[0].ob_y, max_icon.text_w, max_icon.h, k );
			      }
			      else break;
			    }
			    else break;
			    m++;
			  }
			drag_free();
		      }
		      else		/* not on an object */
		      {
			l = ed_wind_type(j);
			if( l!=EDW_GROUP ) drag_free();
			if( j>=0 && !reorder_on )   /* in a window */
			  if( (m=ed_wind_type(w_num))>EDW_DISK && /* source type */
			      l<=EDW_DISK /* dest type */ ) (*icic->bad_op)();
			  else if( m>=EDW_ICONS || l>=EDW_ICONS )
			  {	  /* m=j is needed for buggy compiler */
			    if( (*icic->copy_icon)( m=j, w_num, wfile, witems, 0L ) )
				update_othwind( j, 1 );
			  }
			  else if( l==EDW_GROUP ) from_w_2group(j);
			  else
			  {			   /* copy files to current dir */
			    strcpy( temp, z->w[j].path );
			    iso(temp);
			    copy_from_w( -1, j, temp );
			  }
			else break;
		      }
		      de_act( -1, -1 );
		      break;
		    case 2:			      /* on an object */
		      drag_free();
		      temp[0] = '\0';
		      if( j<0 ) switch( z->idat[i].type )    /* on the desktop */
		      {
			case TRASH:		    /* delete files in window */
			  trash_init(temp);
			  break;
			case FLOPPY:			  /* to a drive */
			case HARDDSK:
			case CLIPBRD:
			case RAMDISK:
			  if( ed_wind_type(w_num) > EDW_DISK )
			      (*icic->bad_op)();
			  else if( (temp[0] = get_drive(i+1)) != 0 )
			  {
			    strcpy( temp+1, colon_slash );
			    copy_from_w( -1, -1, temp );
			  }
			  break;
			case PRINTER:
			  if( check_q(0) ) list_setup( -i, -1 );
			  else prnt_files(MANY_ACTIVE);
			  break;
			default:	/* to a file on desktop */
			  ptr = z->programs[l=z->idat[i].type-D_PROG].p.path;
	                  if( z->programs[l].is_acc ) list_setup( i, -1 );
	                  else if( (m=neo_da(ptr)) >= 0 ) list_setup( i, m ); /*003*/
			  else
			    if( iconedit )
			    {		/* j<0 */
			      if( (*icic->copy_icon)( m=j, w_num, wfile, witems, 0L ) )
				  update_othwind( j, 1 );
			    }
			    else if( z->programs[l].p.type.p.pexec_mode == FOLDER )
			    {
			      strcpy( temp, ptr );
			      strcat( temp, slash );
			      copy_from_w( -1, -1, temp );
			    }
			    else open_d_icon(i);
			  break;
		      }
		      else if( !reorder_on )	      /* in a window */
			if( ((m=ed_wind_type(w_num))<=EDW_DISK) !=
			    (ed_wind_type(j)<=EDW_DISK) ) (*icic->bad_op)();
			else if( m>=EDW_ICONS )
			{
			  if( w_active==MANY_ACTIVE ) (*icic->bad_op)();
			  /* m=j for buggy compiler */
			  else if( (*icic->copy_icon)( m=j, w_num, wfile, witems,
			      z->file[j][i].nib ) ) update_othwind( j, i<D_PROG );
			}
			else if( (l=z->file[j][i].type.p.pexec_mode)
			    == PROG || l == BATCH || l == NPI )
			{
			  m = w_num;
			  w_num = j;
			  set_wfile();
			  find_handle();
			  open_w_icon(i);
			  w_num = m;
			  set_wfile();
			  find_handle();
			}
			else
			{				/* copy to a folder */
			  get_full_name( temp, i, j );
			  strcat( temp, slash );
			  copy_from_w( i, j, temp );
			}
		      else	/* reorder...j==w_num */
		      {
			for( fs=wfile, j=foo=0; j<witems && !foo; j++, fs++ )
			  if( fs->state )
			  {
			    fs->state = 0;
			    if( j<i )
			    {
			      for( k=j; k<i && !foo; k++ )
				if( iconedit ) foo = (*icic->swap_icons)
				    (&wfile[k], &wfile[k+1], w_num, k, k+1 );
				else
				{
				  byteswap( reord_buf+k*sizeof(Dirent),
				      reord_buf+(k+1)*sizeof(Dirent),
				      sizeof(Dirent) );
				  byteswap( &wfile[k], &wfile[k+1], sizeof(FSTRUCT) );
				}
			      j--;
			      fs--;
			      i--;
			    }
			    else if( j>i )
			    {
			      for( k=j; k>i && !foo; k-- )
				if( iconedit ) foo = (*icic->swap_icons)
				    ( &wfile[k], &wfile[k-1], w_num, k, k-1 );
				else
				{
				  byteswap( reord_buf+k*sizeof(Dirent),
				      reord_buf+(k-1)*sizeof(Dirent),
				      sizeof(Dirent) );
				  byteswap( &wfile[k], &wfile[k-1], sizeof(FSTRUCT) );
				}
			      i++;
			    }
			  }
			w_active = num_w_active = oth_w_active = in_tab = -1;
			if( iconedit ) update_othwind( w_num, 0 );
			else rdrw_all();
			break;
		      }
		      de_act( -1, -1 );
		      break;
		  }
		}
		else if( (l=emulti.mouse_x - r->x) >= 0 &&
		    (m=emulti.mouse_y - r->y) >= 0 && l<=r->w && m<=r->h )
		{	/* click once in window not on item */
		  if( !shift || num_w_active >= 0 && w_num != num_w_active )
		      de_act_other(1);
		  if( rubber_box( emulti.mouse_x, emulti.mouse_y, &box, 0 ) )
		  {
		    w_num = wind_xref(w_handle = i);
		    set_wfile();
		    select_many_w( &box );
		  }
		}
/*%		else key++;	/* force a pause; key=0 already */ */
	      }
/*%	  if( !wait && key )		003: removed
	  {
	    evnt_timer( 120, 0 );
	    wait++;
	  }
	  graf_mkstate( &dum, &dum, &emulti.mouse_b, &dum );
	}
	while( key && emulti.mouse_b&1 ); */
      }
      else				 /* button pressed on desktop */
      {
desk_click:
	k = find_d( emulti.mouse_x, emulti.mouse_y );
	if( emulti.times == 1 && emulti.mouse_b&1 )
	{
 	  evnt_timer( 10, 0 );
	  graf_mkstate( &dum, &dum, &emulti.mouse_b, &emulti.mouse_k );
	}
	if( !(emulti.mouse_b & 1) || emulti.times==2 )
	{
	  if( !shift )
	  {
	    de_act_other(1);
	    de_act_d( k+1 );
	  }
	  else if( emulti.times<2 ) de_act_other(1);
	  if( k>=0 )
	  {
	    if( shift && emulti.times != 2 )
		select_d( k+1, z->idat[k].state ^ 1 );
	    else select_d( k+1, 1 );
	    if( emulti.times==2 )
	      if( control && d_active<MANY_ACTIVE && (i=z->idat[k].type)>=D_PROG &&
		  !iconedit )
	      {
		strcpy( filename, ptr=z->programs[i-D_PROG].p.path );
		isolate();
		strcat( filename, glob );
		search_open( filename, spathend(ptr) );
	      }
	      else open_d_icon( k );
	  }
	  else if( emulti.times==2 )
	    if( iconedit )
	    {
	      if( check_reorder() ) (*icic->edit_desk)();
	    }
	    else
	    {
	      m = wget_top();
	      if( m==dtop_handle || wind_xref(m)>=0 || wf_owner(m)==AES_handle )  /* 003: avoid MTOS bug   005: call func*/
	          /*%  aes_ge_40 &&
	          wind_get( m, WF_OWNER, &foo, &m, &m, &m ) &&	
	          foo==AES_handle )*/  edit_note( emulti.mouse_x, emulti.mouse_y );
	      else RING_BELL;
	    }
	}
	else if( emulti.times==1 )
	{
	  de_act_other(1);
	  if( k>=0 )
	  {
	    if( !shift && !(z->idat[k].state&1) ) de_act_d( k+1 );
	    select_d( k+1, 1 );
	    if( control && d_active<MANY_ACTIVE && z->idat[k].type>=D_PROG && !iconedit )
	    {
	      *(filename+(z->maximum.w>>3)) = '\0';
	      wind_lock(1);
	      menu_msg( z->programs[z->idat[k].type-D_PROG].p.path );
	      do graf_mkstate( &dum, &dum, &emulti.mouse_b, &dum );
	      while( emulti.mouse_b&1 );
	      menu_msg(0L);
	      wind_lock(0);
	      de_act_d(-1);
	      goto bottom;
	    }
	    j = -1;
	    i = k;
	    switch( drag_box( &j, &i, emulti.mouse_x, emulti.mouse_y ) )
	    {
	      case -1:		/* 003 */
	        drag_free();
	        av_dragto();
	        break;
	      default:
		drag_free();
		break;
	      case 1:			     /* not on an object */
	        get_max_icon(-1);
		if( j<0 )		       /* move a desktop icon */
		{
		  for( i=0, k=1; k<=z->num_icons; k++ )
		    if( z->idat[k-1].state&1 )
		    {
		      hide_if( z->desk, k, 0 );
		      redraw_desk( z->desk[k].ob_x + z->desk[0].ob_x,
			   z->desk[k].ob_y + z->desk[0].ob_y, max_icon.text_w,
			   max_icon.h, 0 );
		      z->desk[k].ob_x = (l=dbx[i].i0) - z->desk[0].ob_x;
		      z->desk[k].ob_y = (m=dbx[i].i1) - z->desk[0].ob_y;
		      hide_if( z->desk, k, 1 );
		      z->idat[k-1].state &= ~1;
		      redraw_desk( l, m, max_icon.text_w, max_icon.h, k );
		      i++;
		    }
		  drag_free();
		}
		else
		{
		  if( (m=ed_wind_type(j)) > EDW_DISK )
		  {
		    drag_free();
		    (*icic->bad_op)();
		  }
		  else					  /* to a window */
		  {
		    if( m!=EDW_GROUP ) drag_free();
		    else foo=0;
		    for( i=k=0; k<z->num_icons; k++ )
		      if( z->idat[k].state&1 )
		      {
			l = z->idat[k].type;
			if( m==EDW_GROUP )
			{
/**			     if( l < CLIPBRD )
			  {
			    filename[0] = get_drive(k+1);
			    strcpy( filename+1, colon_slash );
			    pt.p.pexec_mode = FOLDER;
			    if( !add_group( j, drag_snum, filename,
				z->idat[k].label, &pt, icons[k+1].ob_spec.iconblk,
				dbx[i].i0, dbx[i].i1 ) ) break;
			  }
			  else**/
			  if( l >= FOLDER )
			    if( !add_group( j, drag_snum, z->programs[l-D_PROG].p.path,
				z->idat[k].label, &z->programs[l-D_PROG].p.type,
				&nic_icons[z->programs[l-D_PROG].p.type.p.pexec_mode].nicb,
				dbx[i].i0, dbx[i].i1 ) ) break;
			    else foo=1;
			  i++;
			}
			else if( l <= CLIPBRD )  /* disk icon? */
			{			    /* copy drive to window path*/
			  if( (filename[0] = get_drive(k+1)) != 0 )
			  {
			    key = 1;
			    strcpy( filename+1, colon_slash );
cpd:			    strcpy( tmpf, z->w[j].path );
			    iso(tmpf);
			    cpy_from_d( filename, tmpf, key );
			  }
			}
			else if( l >= D_PROG /* &&
			    z->programs[l-D_PROG].p.type.p.pexec_mode == FOLDER */ )
			{
cpdprog:		  strcpy( filename, z->programs[l-D_PROG].p.path );
			  key = 0;
			  if( z->programs[l-D_PROG].p.type.p.pexec_mode == FOLDER )
			  {
			    strcat( filename, slash );
			    key = 1;	/* 003 */
			  }
			  goto cpd;
			}
			else f_alert1( msg_ptr[68] );
		      }
		    if( m==EDW_GROUP )
		    {
		      drag_free();
		      if( foo ) update_othwind( j, 0 );
		    }
		  }
		}
		de_act( -1, -1 );
		break;
	      case 2:			  /* on an object */
		drag_free();
		if( j<0 )		    /* on desktop */
		{
		  temp[0] = '\0';
		  switch( z->idat[i].type )
		  {
		    case TRASH:      /* icon dragged to trash */
desk_trash:	      for( k=0; k<z->num_icons; k++ )
			if( z->idat[k].state&1 && k != i )
			  if( z->idat[k].type >= D_PROG )/* remove file on dtop */
			    if( iconedit ) (*icic->use_menu)( -1, IDELALL );  /* Neo Icons to trash */
			    else
			    {
			      remove_icon(k);
			      rmv_icon_redraw(k);
			      arrow();
			    }
			  else if( z->idat[k].type > CLIPBRD ) f_alert1(
			       msg_ptr[69] );
			  else if( (temp[0] = get_drive(k+1)) != 0 )
			      if( !trash_all(temp) ) temp[0] = '\0';
			      else strcpy( temp+1, colon_slash );
			      /* disk to trash */
		      break;
		    case FLOPPY:
		    case HARDDSK:
		    case CLIPBRD:
		    case RAMDISK:
		      copy_all_files( i+1, 0, 0 );
		      break;
		    case PRINTER:
		      if( check_q(0) ) list_setup( -i, -1 );
		      else prnt_files(i);
		      break;
		    default:
		      if( iconedit ) (*icic->bad_op)();
		      else if( z->programs[l=z->idat[i].type-D_PROG].is_acc )
			  list_setup( i, -1 );
		      else if( (m=neo_da(z->programs[l].p.path)) >= 0 ) list_setup( i, m );  /* 003 */
		      else if( z->programs[l].p.type.p.pexec_mode != FOLDER )
			  open_d_icon(i);
		      else copy_all_files( i+1, 0, 1 );
		      break;
		  }
		  update_drive( temp, 0 );
		  de_act( -1, -1 );
		}
		else	/* from desktop to icon in window */
		{
		  if( ed_wind_type(j) > EDW_DISK ) (*icic->bad_op)();
		  else if( z->file[j][i].type.p.pexec_mode != FOLDER )
		  {
		    w_num = j;
		    set_wfile();
		    find_handle();
		    open_w_icon(i);
		  }
		  else for( k=0; k<z->num_icons; k++ )
		    if( z->idat[k].state&1 )
		      if( (l=z->idat[k].type) <= CLIPBRD || l >= D_PROG )
		      { 		/* copy drive/dtop file to folder */
			if( l >= D_PROG )
			{
			  strcpy( filename, z->programs[l-=D_PROG].p.path );
			  if( z->programs[l].p.type.p.pexec_mode == FOLDER )
			      strcat( filename, slash );
			  m = 0;
			}
			else if( (filename[0] = get_drive(k+1)) == 0 ) break;
			else
			{
			  strcpy( filename+1, colon_slash );
			  m = 1;
			}
			get_full_name( temp, i, j );
			strcat( temp, slash );
			cpy_from_d( filename, temp, m );
		      }
		      else f_alert1( msg_ptr[72] );
		  select_w( i, 0, wxref[j], 1 );
		  de_act( -1, -1 );
		}
	    }
	  }
	  else if( !reorder_on )
	  {
	    if( !shift ) de_act_d( -1 );
	    if( rubber_box( emulti.mouse_x, emulti.mouse_y, &box, 0 ) )
		sel_many_d( &box );
	  }
	}
      }
      set_window();
      info();
    }
    if( emulti.event&X_MU_DIALOG )
      if( buffer[3] == dtop_handle )
      {
        emulti.times = buffer[2]<0 ? 2 : 1;
        emulti.event = 0;	/* to prevent an endless loop */
        goto desk_click;
      }
      else use_form( buffer[3], buffer[2] );
bottom:
    get_top_wind();
    if( emulti.event&(X_MU_DIALOG|MU_TIMER) || jog_background )
      for(;;)
      {
        jog_background = 0;
        update_forms(1);
        if( in_showinf ) showinf_next();
        if( z->in_copy ) copy_next();
        if( !jog_background ) break;	/* 003 */
#ifndef DEBUG
        if( !has_Geneva )			/* 003 */
#endif
        {
          emulti.type = MU_TIMER;
          emulti.low = 20;
          multi_evnt( &emulti, buffer );
        }
      }
/*%    if( !z->macr_play ) wind_update( END_UPDATE ); */
  }
}
/******************************************************************/
ICNEO icneo = {
  &z,
  &nac,
  &menu,
  &icons,
  &wmenu,
  &nic_icons,
  &nic_info,
  0L,		/* 003: was showicon */
  &in_showinf,
  &showinf_ok,
  &reorder_on,
  &show_ret,
  &num_icons, &icons_rem,
  &AES_handle,
  &icon_buf,
  &floydbytes,
  &dflt_pall,
  to_filename,
  from_filename,
  reset_all_icons,
  get_all_icons,
  first, 
  &pnmatch, 
  tandd_to_str, 
  to_tandd,
  _set_filename, 
  open_to_path, 
  &deskpat, 
  fix_rez, 
  copy_cicon, 
  alloc_im, 
  add_desk, 
  install_devices,
  dflt_icon, 
  add_program, 
  free_desk, 
  _graf_mouse, 
  add_thing, 
  unpak_rgb, 
  free_pic, 
  fix_icon,
  load_img, 
  load_bmp,
  bmp_data,
  to_stand, 
  set_longedit, 
  do_desk, 
  ic_menu_bar,
  draw_icon, 
  prep_save, 
  obj_true1, 
  obj_true, 
  read_nic_header,
  read_nic, 
  read_dflt_nic,
  add_icon,
  free_icon, 
  free_nib_icon, 
  free_iconbuf, 
  free_nic, 
  reset_icbs,
  update_othwind,
  update_wnum,
  save_img,
  set_temps,
  transform_pic,
  free_desk_pic,
  close_ev_ic,
  fit_pulls,
  fit_pic,	/* 003 */
  window_box	/* 003 */
};

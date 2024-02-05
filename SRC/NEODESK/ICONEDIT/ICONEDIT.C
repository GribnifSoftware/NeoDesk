/* NeoDesk 3.02 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/

#include "neodesk.h"
#include "aes.h"
#include "string.h"
#include "tos.h"
#include "stdlib.h"
#include "stdio.h"
#include "vdi.h"
#include "dither.h"
#include "neocommn.h"
#include "iconedit.h"
#include "xwind.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */

#define PIXELS  (ICON_W*ICON_H)
#define ALLOC_ICON	-1000

#if defined(DEBUG)
  #define map_tree   _lmap_tree
  #define offset_objc  _loffset_objc
#endif

extern long icheader;
extern unsigned int icentries;
extern long iccreate, icmodify;
extern char icauth[], iccomment[];
extern unsigned int iccode;
extern long icon_list;

NIC_INFO nicinfo[7];

int *extract_icon();
void _memclr( void *ptr, unsigned long size );

void restore_pd(void);
int rsrc_ld( char *name );
int use_imenu( int wnum, int num );
void rsrc_adr( int obj, OBJECT **tree );
int trans_wmenu( int wind, int num, int from_neo );
int trans_mmenu( int num, int from_neo );
void edit_desk(void);
int _ed_wind_type( int num );
void map_tree( OBJECT *tree, void func( OBJECT *tree, int tmp ) );
void fix_string( OBJECT *obj, int ind );
void offset_objc( OBJECT *tree, int obj, int *x, int *y );
int open_iwind( int wind, int type );
int get_icon( int wind, int ind, FSTRUCT *file );
int edit_icon( FSTRUCT *file, int wnum );
void enable_menu( int wnum );
void no_memory(void);
void __bytecpy( void *src, void *dest, int len );
int delete_icon( int wnum, int ind );
int checksum( int wind, int keep );
int read_icons( int num, char *path );
int close_icons( int num );
void clear_icons( int t );
void bad_op(void);
int copy_icon( int wnum, int s_num, FSTRUCT *wfile, int witems, NICONBLK *ditem );
int create_nic( int new, int wnum );
int ic_reorder( FSTRUCT *wfile, int witems, char *reorder_on, int wnum );
int swap_icons( FSTRUCT *fs1, FSTRUCT *ib2, int wnum, int first, int next );
int nic_showinf( int wnum, char *file );
int del_conf(void);
int nt_nic(int wnum);
int _f_alert1( char *str );
void free_msg(void);
void get_pixes( int pat );
void disp_all( int corn );
void ed_color( unsigned int *old, unsigned int new, int draw );
int cur_rez( int do_d, int do_s, int get, int planes );
static int plane_size( int pl );
#ifdef PRINT_ALLOCS
static void *_p_lalloc( long size, int id, char *file, int line );
static void _p_cmfree( char **ptr, char *file, int line );
#else
static void *lalloc( long size, int id );
static void cmfree( char **ptr );
#endif
void neo_icons_nib(void);
OBJECT *icon_menu( int wnum );
int add_patt( int wnum, int *data, int col );
int add_nic_patt( int *data, int col );
void free_patt( int w );


OBJECT *editor, *win_menu, *fil_menu;
ICNEO *_icneo;
NEO_ACC *_nac;
MOST *_z;
GRAPHICS *graph;
static GUI *gui;
ICONSAVE *old_idat;
int old_showicon[7], old_idat_rem, old_num_idat, old_num_icons,
    old_icons_rem, old_num_progs, old_prog_rem, old_notes_len,
    old_num_desk;
FILT_TYPE old_filt[7];
char old_use[7], old_mshowicon, no_xlate;
struct Wstruct old_ws[7];
PROG_STRUCT *old_programs;
OBJECT *old_menu, *imenu, *old_desk, *ipopups, *ed_menu;
RSHDR *rshdr;
char not_nic[7], *_new_msgs, shrink/*004*/;
ICONBUF *icnbuf[7], **iconbuf[8];
static int ewind_type[7], num_new0[8], cksum[8],
   *num_icons[8], nmic[7], *icons_rem[8], icrem[7], ed_handle;
extern char *_msg_ptr[];
char istyle, imode, ipat, imirr, _use_8x16, idcml, iset, icoldm,
    old_note, icolors, iselected, fileatt, cutmode;
int last, move, pix_w, pix_h, h_off, v_off, x_max, y_max,
    ed_col, ed_wnum;
char **msg0=&_msg_ptr[0], **msgx=&_msg_ptr[IED_MSGS];
Rect mbox;
BITBLK new_pattern;
long disp_size;
static unsigned int new_pat[16];
static int fill_dat[16][16], *mask_dat, corner_dat=0x8000;
static int om, ed_obj;
static MFDB disp, corner = { &corner_dat, 1, 1, 16, 0, 1 },
     fills = { 0L, 16, 16, 1, 0, 1 }, screen;
static unsigned int color=0xFFFF;
static ICONBLK i_samp;
static NICONBLK nib_samp,
     *patt_nib[8],	/* desktop pattern icon */
     icon_nib;	/* NeoDesk icons icon */
static CICON i_cic[3], ic_samp;
static long pallette[16];
char prev_ok, bounce;
static char n2col[] = { 2, 4, 16 }, n2pl[] = { 1, 2, 4 }, max_col[] = { 1, 3, 15 };
ICIC _icic = { use_imenu, 0L, 0L, 0, trans_wmenu,
    edit_desk, _ed_wind_type, offset_objc, open_iwind, 0L, 0L, 
    0L, 0L, get_icon, edit_icon,
    enable_menu, delete_icon, close_icons, clear_icons, bad_op, copy_icon, ic_reorder,
    swap_icons, nic_showinf, del_conf, nt_nic, trans_mmenu, icon_menu, add_nic_patt, &icon_nib
    };
static TEDINFO *not_drv, *drive;

typedef struct
{
  int chk[3][4];
  long off[3][4];
} SAVECHK;
SAVECHK *chk_start;

long ic_main( ICNEO *_ic_neo, int close )
{
  int i, dum, new, cel;
  char buf[120];
  OBJECT *obj;
  static char inactive[] = { IOPEN, ISHOW };
  ICONBUF *icb;
  PROG_TYPE pt;
  
  istyle=IPOINT;
  imode=IPDATA+2;
  ipat=IPSOLID;
  imirr=INONE;
  idcml=IDRAW;
  iset=IPSET;
  icoldm=0;
  icolors=0;
  iselected=0;
  prev_ok = 0;
  ed_col = IEDCOL0+1;
  i_cic[0].num_planes = 1;
  i_cic[1].num_planes = 4;
  i_cic[2].num_planes = 16;
  _icneo = _ic_neo;
  _z = *(_icneo->most);
  _nac = _icneo->nac;
  graph = _z->graphics;
  gui = _z->gui;
  strcpy( buf, _z->dflt_path );
  graf_handle( &dum, &cel, &dum, &dum );
  _use_8x16 = cel==16;
  strcpy( strrchr(buf,'\\')+1, "ICONEDIT.MSG" );
  if( (*_nac->mas->read_messages)( buf, IED_MSGS, *msg0, *msgx,
      &_msg_ptr[0], &_new_msgs ) ) return(0);
  _icic.info_fmt = _msg_ptr[36];
  _icic.info_fmt_sel1 = _msg_ptr[37];
  _icic.info_fmt_sel = _msg_ptr[38];
  strcpy( strrchr(buf,'\\')+1, "ICONEDIT.RSC" );
  if( (i=rsrc_ld(buf)) == 1 )
  {
    get_pixes(0);
    disp.fd_wdwidth = ((disp.fd_w = h_off*ICON_W + 1) + 15)>>4;
    disp.fd_h = v_off*ICON_H + 1;
    disp.fd_nplanes = graph->vplanes;
    if( (disp.fd_addr = lalloc(
        disp_size=(long)(disp.fd_wdwidth<<1)*disp.fd_nplanes*disp.fd_h, ALLOC_ICON )) == 0 )
    {
      no_memory();
      (*gui->Nrsc_free)(rshdr);
      free_msg();
      return(0);
    }
    iconbuf[0] = _icneo->nic_icons;
    num_icons[0] = _icneo->num_icons;
    icons_rem[0] = _icneo->icons_rem;
    for( i=1; i<8; i++ )
    {
      iconbuf[i] = &icnbuf[i-1];
      num_icons[i] = &nmic[i-1];
      icons_rem[i] = &icrem[i-1];
    }
    if( close )
    {
      old_idat = _z->idat;
      old_idat_rem = _z->idat_rem;
      old_num_idat = _z->num_idat;
      _z->idat = 0L;
      _z->num_idat = 0;
      old_num_icons = _z->num_icons;
      old_icons_rem = _z->icons_rem;
      old_desk = _z->desk;
      old_num_desk = _z->num_desk;
      _z->num_icons = 0;
      _z->desk = 0L;
      (*_icneo->add_desk)();      	/* desktop itself */    /*% move into ICNEO */
      (*_icneo->install_devices)(1);
      old_programs = _z->programs;
      old_num_progs = _z->num_progs;
      old_prog_rem = _z->prog_rem;
      _z->programs = 0L;
      _z->num_progs = 0;
      if( (*_icneo->dflt_icon)( 0, D_PROG ) || (*_icneo->add_program)( "", &pt ) )
      {
        restore_pd();
        (*gui->Nrsc_free)(rshdr);
        free_msg();
        return(0);
      }
      new = _z->num_icons-1;
      _z->desk[new+1].ob_flags |= HIDETREE;
      (*_icneo->close_ev_ic)();
      (*_nac->save_desktop)();
      rsrc_adr( IEDPOPS, &ipopups );
      drive = ipopups[IEDPDRIV].ob_spec.tedinfo;
      rsrc_adr( ICMENU, &ed_menu );
      rsrc_adr( IICON, &obj );
      strcpy( _z->idat[new].label, obj[1].ob_spec.iconblk->ib_ptext );
      _icic.icon_str = _msg_ptr[3];
      _z->desk[new+1].ob_flags &= ~HIDETREE;
      old_notes_len = _z->notes_len;
      _z->notes_len = 0;
      __bytecpy( old_showicon, _z->showicon, 7*sizeof(int) );
      old_mshowicon = _z->mshowicon;
      /*% *(_icneo->showicon) = 1   003 */
      _z->mshowicon = 1;
      __bytecpy( old_ws, _z->w, 7*sizeof(struct Wstruct) );
      __bytecpy( old_filt, _z->filter, 7*sizeof(FILT_TYPE) );
      for( i=0; i<7; i++ )
      {
        _z->filter[i].flags.i = 0x10C0;	/* templates, allfold, use_long */
        strcpy( _z->filter[i].long_tmpl[0], "*.{NIC,RSC,ICN,ICE,IB[I3]," );
        strcpy( _z->filter[i].long_tmpl[1], "ICO,ICA,IMG,BMP}" );
        _z->w[i].place = 0;
      }
    }
    rsrc_adr( IEDIT, &editor );
    form_center( editor, &dum, &dum, &dum, &dum );
    rsrc_adr( IICON, &obj );
    icon_nib.ib = obj[1].ob_spec.iconblk;
    neo_icons_nib();
    (*_icneo->free_desk_pic)();
    if( close ) old_menu = *(_icneo->menu);
    rsrc_adr( IMENU, _icneo->menu );
    (*_icneo->fit_pulls)( imenu = *_icneo->menu, IABOUT-1 );
    for( i=0; i<sizeof(inactive); i++ )
      imenu[inactive[i]].ob_state |= DISABLED;
    (*_icneo->imenu_bar)( imenu, 1 );
    (*_icneo->read_dflt_nic)(0);
    (*_icneo->get_all_icons)();	/* 003: reset matched drive icons */
    neo_icons_nib();	/* 003 */
    add_patt( -1, (*_icneo->deskpat)->ob_spec.bitblk->bi_pdata, (*_icneo->deskpat)->ob_spec.bitblk->bi_color );
    cksum[0] = checksum( -1, 0 );
    rsrc_adr( IABOUTTX, &obj );
    form_center( obj, &dum, &dum, &dum, &dum );
    rsrc_adr( NICINFO, &obj );
    form_center( obj, &dum, &dum, &dum, &dum );
    rsrc_adr( IWINMENU, &win_menu );
    rsrc_adr( IFILMENU, &fil_menu );
    if( close ) form_dial( FMD_FINISH, 0, 0, 0, 0, _z->maximum.x, 
        _z->maximum.y, _z->maximum.w, _z->maximum.h );
    shrink = imenu[ISHRINK].ob_state&CHECKED;	/* 004 */
    return( (long)&_icic );
  }
  else if( !i ) _f_alert1( _msg_ptr[4] );
  free_msg();
  return(0);
}

void restore_pd(void)
{
  (*_icneo->free_desk)();
  _z->idat = old_idat;
  _z->idat_rem = old_idat_rem;
  _z->num_idat = old_num_idat;
  _z->icons_rem = old_icons_rem;
  _z->num_icons = old_num_icons;
  _z->desk = old_desk;
  _z->num_desk = old_num_desk;
  cmfree((char **)&_z->programs);
  _z->prog_rem = old_prog_rem;
  _z->programs = old_programs;
  _z->num_progs = old_num_progs;
  cmfree((char **)&disp.fd_addr);	/* 003 */
  free_patt(0);				/* 003 */
}

static char *spathend( char *ptr )
{
  register char *ch;

  if( (ch=strrchr(ptr,'\\')) == NULL ) return ptr;
  return ch+1;
}

#ifdef PRINT_ALLOCS
static void *_p_lalloc( long size, int id, char *file, int line )
{
  extern int AES_handle;
  void *out;
  
  if( id==-1 ) id = AES_handle-500;
  out = (void *)((*_nac->mas->lalloc)( size, id, 1 ));
  prnall( "%s %d: alloc(%D,%d) \tret=$%X", file, line, size, id, out );
  return out;
}
#else
static void *lalloc( long size, int id )
{
#ifdef DEBUG
  extern int AES_handle;
  
  if( id==-1 ) id = AES_handle-500;
#endif
  return (void *)((*_nac->mas->lalloc)( size, id, 1 ));
}
#endif

#ifdef PRINT_ALLOCS
static int _p_lfree( void *xfb, char *file, int line )
{
  int out = (*_nac->mas->lfree)( xfb );

  prnall( "%s %d: free($%X) \tret=%d", file, line, xfb, out );
  return out;
}
#else
static int lfree( void *xfb )
{
  return (*_nac->mas->lfree)( xfb );
}
#endif

#ifdef PRINT_ALLOCS
static void _p_cmfree( char **ptr, char *file, int line )
{
  if( *ptr )
  {
    _p_lfree( *ptr, file, line );
    *ptr = NULL;
  }
}
#else
static void cmfree( char **ptr )
{
  if( *ptr )
  {
    lfree(*ptr);
    *ptr = NULL;
  }
}
#endif
static int lfreeall( int id )
{
#ifdef DEBUG
  extern int AES_handle;
  
  if( id==-1 ) id = (*_icneo->AES_handle)-500;
#endif
  return (*_nac->mas->lfreeall)( id );
}

void _linea9(void)
{
  (*graph->show_mouse)();
}
void _lineaa(void)
{
  (*graph->hide_mouse)();
}

void _arrow(void)
{
  (*_icneo->_graf_mouse)( ARROW );
}

void _bee(void)
{
  (*_icneo->_graf_mouse)( HOURGLASS );
}

void grf_mouse( int mode )
{
  (*_icneo->_graf_mouse)( mode );
}

int _f_alert1( char *str )
{
  return( form_alert( 1, str ) );
}

void kick( char *path )
{
  char temp[]="x:\\*.*";

  temp[0] = *path;
  Fsfirst( temp, 0x37 );
}

#define KEY_START 0x37
static char key;
static void encrypt( void *from, void *to, long size )
{
  while( --size >= 0 )
  {
    *((char *)to)++ = *((char *)from)++ ^ key;
    key += 0x21;
  }
}

typedef struct
{
  unsigned char size_x, size_y;
  unsigned char xchar, ychar;
  unsigned char colors;
  unsigned char type;
  char string[12];
  long data[3][4];	/* col_data, col_mask, sel_data, sel_mask */
} ICONFILE;

void bad_op(void)
{
  _f_alert1( _msg_ptr[5] );
}

void fixx( int *i )
{
  *i = ((*i&0xFF)<<3) + (*i>>8);
}

void fixy( int *i )
{
  *i = ((*i&0xFF)<<(_use_8x16?4:3)) + (*i>>8);
}

void obfix( OBJECT *tree, int ind )
{
  fixx( &tree[ind].ob_x );
  fixx( &tree[ind].ob_width );
  fixy( &tree[ind].ob_y );
  fixy( &tree[ind].ob_height );
}

int chksm( char *place, unsigned int chk, int size )	/* 005 */
{
/*
 * updcrc macro derived from article Copyright (C) 1986 Stephen Satchell. 
 *  NOTE: First argument must be in range 0 to 255.
 *        Second argument is referenced twice.
 * 
 * Programmers may incorporate any or all code into their programs, 
 * giving proper credit within the source. Publication of the 
 * source routines is permitted so long as proper credit is given 
 * to Stephen Satchell, Satchell Evaluations and Chuck Forsberg, 
 * Omen Technology.
 */

#define updcrcmacro(cp, crc) (crctab[crc >> 8] ^ (crc << 8) ^ cp)

	/* crctab calculated by Mark G. Mendel, Network Systems Corporation */
	static unsigned short crctab[256] = {
	    0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
	    0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
	    0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
	    0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
	    0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
	    0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
	    0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
	    0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
	    0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
	    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
	    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
	    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
	    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
	    0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
	    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
	    0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
	    0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
	    0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
	    0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
	    0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
	    0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
	    0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
	    0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
	    0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
	    0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
	    0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
	    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
	    0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
	    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
	    0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
	    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
	    0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
	};

	while( --size>=0 )
		chk = updcrcmacro( (*(unsigned char *)place++), chk );
	return chk;
}

/*****		005
int chknum;

unsigned int rol( int n ) 0xE358;

int chksm( char *place, int chk, int size )
{
/*  while( size-- ) *chk += *place++ * (size+chknum); */
  chk += chknum++;
  for(;;)
  {
    chk = rol(chk);
    if( !size-- ) break;
    chk ^= *place++;	/* 004: changed from + */
  }
  return chk;
}

void chksm1( int *place, int *chk, int size )
{
  int c;

  if( place )
  {	/* 004: made faster for even-aligned bytes */
    if( !((size|(int)place)&1) )
    {
      c = *chk;   /* 005: don't use chknum     + chknum++; */
      for(;;)
      {
        c = rol(c);
        if( (size-=2)<0 ) break;
        c ^= *place++ - 1;	/* 004: changed from +  005: added -1 */
      }
      *chk = c;
    }
    else *chk = chksm( (char *)place, *chk, size );
    if( !*chk ) *chk = 1;
/*    chknum = 1;  005: don't use chknum */
  }
}
******/

void chksm1( int *place, int *chk, int size )	/* 005: modified muchly */
{
  if( place )
      *chk = chksm( (char *)place, *chk, size );
}

void chksmstr( char *place, int *chk )
{
  *chk = chksm( place, *chk, strlen(place)+1 );
}

int num_chk, chk_rem;

SAVECHK *add_chk(void)
{
  if( (*_icneo->add_thing)( (void **)&chk_start, &num_chk, &chk_rem,
      0L, 10, sizeof(SAVECHK), ALLOC_ICON ) ) return &chk_start[num_chk-1];
  return 0L;
}

int checksum( int wind, int keep )
{
  int i, chk=0, chk2, j, k, **p, *q;
  FSTRUCT f;
  ICONBLK *icb;
  BITBLK *bb;
  SAVECHK *sc, temp;
  
  i = 0;
/*  chknum = 1;  005 */
  while( get_icon( wind, i, &f ) )
  {
    if( !keep ) sc = &temp;
    else if( (sc=add_chk()) == 0 )
    {
      cmfree( (char **)&chk_start );
      keep = 0;
      sc = &temp;
    }
    _memclr( sc, sizeof(SAVECHK) );
    for( j=3; --j>=0; )
      if( f.nib->list[j] == 0 )
      {
        if( !j )
        {
          chksm1( f.nib->ib->ib_pdata, &sc->chk[0][0], DATASIZ );
          chksm1( f.nib->ib->ib_pmask, &sc->chk[0][1], DATASIZ );
        }
      }
      else
        for( p=&f.nib->list[j]->col_data, k=0; k<4; k++, p++ )
          if( *p )
            chksm1( *p, &sc->chk[j][k], !(k&1) ? plane_size(n2pl[j]) : DATASIZ );
/**    for( chk2=0, j=4*3, q=&sc->chk[0][0]; --j>=0; )
      chk2 += *q++ * j;
    chk += chk2*(i+1);  005 replace with below **/
    chk = chksm( (char *)&sc->chk[0][0], chk, 4*3*sizeof(int) ) | i;
    chk = chksm( (char *)(icb = f.nib->ib), chk, sizeof(ICONBLK) );
    chk = chksm( icb->ib_ptext, chk, 12 );
    if( keep && f.type.p.pexec_mode==NPI )  /* this is a pattern, so keep it from being considered for a duplicate image */
        _memclr( sc, sizeof(SAVECHK) );
    i++;
  }
  if( wind>=0 )
  {
    chksmstr( nicinfo[wind].auth, &chk );
    chksmstr( nicinfo[wind].comment[0], &chk );
    chksmstr( nicinfo[wind].comment[1], &chk );
    chksmstr( nicinfo[wind].comment[2], &chk );
  }
  return(chk);
}

void __bytecpy( void *dest, void *src, int len )
{
  (*_nac->bytecpy)( dest, src, len );
}

void no_memory(void)
{
  _f_alert1( _msg_ptr[6] );
}

void _spf(char *buf, char *fmt, ...) {
  (*_nac->mas->dopf)(buf, fmt, (unsigned int *)&...);
}

void map_tree( OBJECT *tree, void func( OBJECT *tree, int tmp ) )
{
  int tmp1, this;
  
  tmp1 = this = 0;
  while (this != -1)
    if (tree[this].ob_tail != tmp1)
    {
      tmp1 = this;
      (*func)( tree, tmp1 );
      this = tree[tmp1].ob_head;
      if (this == -1) this = tree[tmp1].ob_next;
    }
    else
    {
      tmp1 = this;
      this = tree[tmp1].ob_next;
    }
}

void shorten_data( int h, int *ptr, int wb )
{
  /* clear-out every second line in the image data */
  for( h>>=1; --h>=0; ptr+=wb )
    _memclr( ptr, wb );
}

void shorten_bb( BITBLK *bb )
{
  shorten_data( bb->bi_hl, bb->bi_pdata, bb->bi_wb );
  bb->bi_x += bb->bi_wb<<3;
  bb->bi_hl >>= 1;
  bb->bi_wb <<= 1;
}

void shorten_ib( ICONBLK *ib )
{
  int wb;

  shorten_data( ib->ib_hicon, ib->ib_pdata+2, 4 );
  shorten_data( ib->ib_hicon, ib->ib_pmask+2, 4 );
  *((long *)ib->ib_pdata+26) = *((long *)ib->ib_pmask+26) = *(long *)ib->ib_pdata;
  ib->ib_hicon >>= 1;
  ib->ib_wicon <<= 1;
}

int rsrc_ld( char *name )
{
  OBJECT *o;
  int i;
  char **ptr;
  
  switch( (*gui->Nrsc_load)( name, Fgetdta(), &rshdr ) )
  {
    case 0:
      return 0;
    case -1:
    case -2:
      no_memory();
  }
  (*gui->Nrsc_rcfix)( rshdr );
  rsrc_adr( IEDIT, &o );
  if( graph->cel_ht < 16 )
  {
    o[IEDDESK].ob_height = o[IEDWIND].ob_height = 46;
    o[IEDPIMG].ob_spec.bitblk->bi_wb =
        o[IEDPIMG].ob_spec.bitblk->bi_hl = 8;
    o[IEDDESK].ob_y = o[IEDWIND].ob_y = o[IEDTEMPL].ob_y;
    o[IEDTEMPL].ob_x = 0;
    o[IEDTEMPL].ob_y -= 3;
    o[IEDTEMPL].ob_width = o[IEDINVIS].ob_width;
    *((char *)&o[IEDTEMPL].ob_type+1) = G_FTEXT; 	/* no frame */
    objc_delete( o, IEDTEMPL );
    objc_add( o, IEDINVIS, IEDTEMPL );
    ptr = &o[IEDTEMPL].ob_spec.tedinfo->te_ptmplt;
    *ptr = strchr(*ptr,'_');
    o[0].ob_height += 4;
    for( i=IPOINT; i<=IFILBOX; i++ )
      shorten_ib( o[i].ob_spec.iconblk );
    rsrc_adr( IEDPOPS, &o );
    for( i=IEDDPAT0; i<IEDDPAT0+16; i++ )
      o[i].ob_spec.bitblk->bi_hl >>= 1;
    for( i=IPSOLID; i<IPSOLID+5; i++ )
      shorten_bb( o[i].ob_spec.bitblk );
  }
  else o[IEDPIMG].ob_spec.bitblk->bi_wb = 4;
  return 1;
}

void rsrc_adr( int obj, OBJECT **tree )
{
  (*gui->Nrsc_gaddr)( 0, obj, tree, rshdr );
}

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

char no_color, ed_pat;
unsigned char pixel[ICON_H][ICON_W], tmppix[ICON_H][ICON_W], 
    previous[ICON_H][ICON_W];
int x0, y0;

int xpall(int i)
{
  static char pall[16] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };
  
  if( ed_pat ) return i;
  if( i<n2col[icolors]-1 ) return pall[i];
  return 1;
}

void get_rgb( int (*rgb)[16][3] )
{
  int i, j;
  
  for( j=16; --j>=0; )
    for( i=1; i<4; i++ )
      (*_icneo->unpak_rgb)( (unsigned char *)&pallette[xpall(j)]+i, &(*rgb)[j][i-1] );
}

static int fix_rez( int *data, int **out, int pl, int dplanes, int devspef )
{
  int rgb[16][3];
  
  get_rgb( &rgb );
  return (*_icneo->fix_rez)( data, out, pl, dplanes, &rgb, devspef );
}

void draw( int x, int y, unsigned char color, int both )
{
  char nc;
  int cols[2], xarray[8];
  
  nc = !ed_pat && no_color && color!=0x80;
  xarray[0] = nc ? x&7 : 0;
  xarray[1] = nc ? y&7 : 0;
  xarray[6] = (xarray[4]=x)+(xarray[2]=pix_w-1);
  xarray[7] = (xarray[5]=y)+(xarray[3]=pix_h-1);
  xarray[2] += xarray[0];
  xarray[3] += xarray[1];
  fills.fd_addr = color==0x80 ? mask_dat : fill_dat[color&0x7f];
  fills.fd_h = pix_h;
  cols[0] = no_color || color==0x80 ? 1 : xpall(color&0x7f);
  cols[1] = 0;
  (*_nac->set_clip_rect)( (Rect *)&_z->maximum, 1 );	/* 003 */
  if( both ) vrt_cpyfm( graph->handle, MD_REPLACE, xarray, &fills, &screen, cols );
  xarray[4] -= x0 - 1;
  xarray[5] -= y0 - 1;
  xarray[6] -= x0 - 1;
  xarray[7] -= y0 - 1;
  vrt_cpyfm( graph->handle, MD_REPLACE, xarray, &fills, &disp, cols );
}

void ed_draw( int num )
{
  x_wdial_draw( ed_handle, num, 8 );
}

void display(void)
{
  ed_draw( IEDDISP );
}

void sample(void)
{
  ed_draw( IEDDESK );
  if( !ed_pat ) ed_draw( IEDWIND );
}

int plane_size( int pl )
{
  return pl*DATASIZ;
}

/* call before changing icolors to new value! */
void free_rez( int s )
{
  if( !s || s<0 )
    if( i_cic[icolors].col_data == ic_samp.col_data ) ic_samp.col_data = 0L;
    else cmfree( (char **)&ic_samp.col_data );
  if(s)
    if( i_cic[icolors].sel_data == ic_samp.sel_data ) ic_samp.sel_data = 0L;
    else cmfree( (char **)&ic_samp.sel_data );
}

void redo_sample(void)
{
  unsigned int data, mask, *ptr;
  int *d;
  int i, j, k, l, pl, s, do_d;
  unsigned char *p;

  if( !ed_pat )
  {
    do_d = 0;
    if( imode!=IPDATA+1 )
    {
      d = iselected ? i_cic[icolors].sel_data : i_cic[icolors].col_data;
      _memclr( d, plane_size(pl = n2pl[icolors]) );
      p = pixel[0];
      for( i=ICON_H<<1; --i>=0; d++ )
        for( k=16; --k>=0; )
        {
          data = *p++;
          for( j=pl, l=0; --j>=0; l+=ICON_WW*ICON_H )
          {
            *(d+l) |= (data&1)<<k;
            data >>= 1;
          }
        }
      do_d = 1;
    }
    if( imode>IPDATA )
    {
      d = iselected ? i_cic[icolors].sel_mask : i_cic[icolors].col_mask;
      _memclr( d, MASKSIZ );
      p = pixel[0];
      for( i=ICON_H<<1; --i>=0; )
      {
        for( k=16, data=0; --k>=0; )
          data = (data+data) | (*p++>>7);
        *d++ = data;
      }
    }
    if( do_d ) free_rez( iselected );
    cur_rez( do_d, iselected, 0, -1 );
  }
  else
  {
    ptr = new_pat;
    for( i=0; i<16; i++ )
    {
      for( data=k=0; k<16; k++ )
        data = (data<<1) | pixel[i][k];
      *ptr++ = data;
    }
  }
  disp_all(0);
  sample();
}

void get_x0y0(void)
{
  x0 = editor[IEDDISP].ob_x + editor[0].ob_x + 2;
  y0 = editor[IEDDISP].ob_y + editor[0].ob_y + 2;
}

void _draw_xor( int *arr )
{
  (*graph->wmode2)();
  (*_nac->blank_box)( arr );
}

void set_xarray( int x1, int y1, int w, int h, int *xarray )
{
  xarray[2] = (xarray[0] = x0 + h_off*x1 - 1) + w*h_off;
  xarray[3] = (xarray[1] = y0 + v_off*y1 - 1) + h*v_off;
}

void xor_box( int flag, int x1, int y1, int w, int h )
{
  int i, xarray[4];
  static int oflag=-1;
  
  _lineaa();
  grf_mouse( flag ? POINT_HAND : ARROW );
  get_x0y0();
  set_xarray( x1, y1, w, h, xarray );
  _draw_xor( xarray );
  _linea9();
  i_samp.ib_char &= 0xFF00;
  if( !ed_pat )
  {
    if( flag==1 ) i_samp.ib_char |= 'X';
    if( flag==1 || oflag==1 ) sample();
    oflag = flag;
  }
}

int get_mouse( int *x, int *y, int *b, int *mx, int *my )
{
  int mk;
  
  graf_mkstate( mx, my, b, &mk );
  get_x0y0();
  *x = (*mx - x0) / h_off;
  *y = (*my - y0) / v_off;
  if( mk & 3 ) *b |= 2;
  return( *mx >= x0 && *my >= y0 && *x < x_max && *y < y_max );
}

int new_rbox( int x, int y, Rect *rbox, int xor )
{
  GRECT bound;
  
  grf_mouse( THIN_CROSS );
  get_x0y0();
  rbox->x = x0 + x*h_off - 1;
  rbox->y = y0 + y*v_off - 1;
  bound.g_x = x0-1;
  bound.g_y = y0-1;
  bound.g_w = x_max*h_off+1;
  bound.g_h = y_max*v_off+1;
  if( x_graf_rubberbox( (GRECT *)rbox, &bound, h_off, v_off, bound.g_w,
      bound.g_h, 1, 0 ) )
  {
    if( (rbox->w = (rbox->w+(h_off>>1))/h_off) != 0 && 
        (rbox->h = (rbox->h+(v_off>>1))/v_off) != 0 )
    {
      rbox->x = (rbox->x-x0+(h_off>>1))/h_off;
      rbox->y = (rbox->y-y0+(v_off>>1))/v_off;
      if( rbox->x < 0 )
      {
        rbox->w += rbox->x;
        rbox->x = 0;
      }
      if( rbox->y < 0 )
      {
        rbox->h += rbox->y;
        rbox->y = 0;
      }
      if( rbox->x + rbox->w > x_max ) rbox->w = x_max - rbox->x;
      if( rbox->y + rbox->h > y_max ) rbox->h = y_max - rbox->y;
      if( xor ) xor_box( 2, rbox->x, rbox->y, rbox->w, rbox->h );
      else _arrow();
      return(2);
    }
  }
  _arrow();
  return(0);
}

int grab( int *x, int *y, int w, int h, int flag )
{
  int bx, by, nx, ny, xarray[4];
  Rect bounds;

  set_xarray( mbox.x, mbox.y, w, h, xarray );
  if( *x >= xarray[0] && *x <= xarray[2] && *y >= xarray[1] &&
      *y <= xarray[3] )
  {
    get_x0y0();
    bounds.x = x0;
    bounds.y = y0;
    bounds.w = x_max * h_off;
    bounds.h = y_max * v_off;
    if( flag==2 )
    {
      bounds.x -= w=(w-1) * h_off;
      bounds.y -= h=(h-1) * v_off;
      bounds.w += w<<1;
      bounds.h += h<<1;
    }
    grf_mouse( FLAT_HAND );
    if( graf_dragbox( xarray[2]-xarray[0], xarray[3]-xarray[1],
        xarray[0], xarray[1], bounds.x, bounds.y, bounds.w, bounds.h,
        &bx, &by ) )
    {
      nx = bx+(h_off>>1)-x0;
      ny = by+(v_off>>1)-y0;
      if( nx<0 ) nx -= h_off - 1;
      if( ny<0 ) ny -= v_off - 1;
      *x = nx/h_off;
      *y = ny/v_off;
      return(1);
    }
    grf_mouse( POINT_HAND );
  }
  return(0);
}

void drwpix( int newx, int newy, char num, char col )
{
  pixel[newy][newx] = tmppix[newy][newx] = num;
  get_x0y0();
  draw( x0+newx*h_off, y0+newy*v_off, col, 1 );
}

unsigned char pix_mask(void)
{
  static unsigned char masks[3] = { 0x7f, 0x80, 0xff };

  return masks[imode-IPDATA];
}

void mirror( int x, int y )
{
  unsigned char num, col;
  int x1, y1;
  
  x1 = x_max-x-1;
  y1 = y_max-y-1;
  col = (num = pixel[y][x])&pix_mask();
  _lineaa();
  get_x0y0();
  draw( x0+x*h_off, y0+y*v_off, col, 1 );
  if( imirr == ILR || imirr == I4WAY ) drwpix( x1, y, num, col );
  if( imirr == ITB || imirr == I4WAY ) drwpix( x, y1, num, col );
  if( imirr == I4WAY ) drwpix( x1, y1, num, col );
  _linea9();
}

void set_pixel( int x, int y, int b )
{
  unsigned char mask, or, set, i, p;
  
  set = iset==IPSET;
  switch( ipat )
  {
    case IPSOLID+1:
      if( (x&1) != (y&1) ) set ^= 1;
      break;
    case IPSOLID+2:
      if( (x&1) == (y&1) ) set ^= 1;
      break;
    case IPSOLID+3:
      if( x&1 ) set ^= 1;
      break;
    case IPSOLID+4:
      if( !(x&1) ) set ^= 1;
  }
  mask = pix_mask();
  or = set ? b&mask : 0;
  p = pixel[y][x];
  if( iset==IPTOGGLE )
  {
    i = p&mask;
    if( mask==0xff )
      if( i&0x7f ) or = i&0x80 ? 0 : 0x80;
      else or = b|0x80;
    else if( mask==0x7f ) or = i==b ? 0 : b;
    else or = i^mask;
  }
  else if( istyle==IPOINT )
    if( imode == IPBOTH )
      if( ipat==IPSOLID )
      {
/*        if( (b != 0) == set ) last = 0;*/
        if( last >= 0 ) or = last;
        else if( (i=p&0x7f)==0 || i==(b&0x7f) )  /* new color: leave or alone */
          if(i) or = 0x80;
          else if( /*p&0x80*/ !(b&0x7f) ) or = 0;
/*          else or = b;*/
        last = or;
      }
      else
        if( !set ) or = 0;
        else if( last >= 0 ) or = last;
        else
        {
          if( (i=p&0x7f)==0 || i==(b&0x7f) )  /* new color: leave or alone */
            if(i) or = 0x80;
            else if( /*p&0x80*/ !(b&0x7f) ) or = 0;
/*            else or = b&0x7f;*/
          last = or;
        }
    else if( !set ) or = 0;
    else if( last>=0 ) or = last;
    else
    {
      if( (p&mask) == or ) or = 0;
      last = or;
    }
/*    else
      if( ((or&0x80) != 0) == set ) or = 0;
      else or = b|0x80; */
/*******  else if( istyle==IPOINT )
/*%    if( last >= 0 ) or = last;*/
    else last = or = (p&mask) == (b&mask) ? 0 : or;
/*  else if( b&0x80 ) or ^= b&mask;*/ *******/
  pixel[y][x] = tmppix[y][x] = (p&~mask)|or;
  mirror( x, y );
}

void do_disp( int move, Rect mbox )
{
  if( move ) xor_box( 0, mbox.x, mbox.y, mbox.w, mbox.h );
  display();
  if( move ) xor_box( move, mbox.x, mbox.y, mbox.w, mbox.h );
}

void redo_prev(void)
{
  __bytecpy( previous, pixel, PIXELS );
  prev_ok = 1;
}

void shift( int xoff, int yoff )
{
  int i, j, k, l, mask;
  Rect box, box2;
  
  redo_prev();
  __bytecpy( tmppix, pixel, PIXELS );
  mask = pix_mask();
  for( j=0; j<y_max; j++ )
    for( i=0; i<x_max; i++ )
    {
      if( (k = i+xoff) < 0 ) k = x_max-1;
      else if( k >= x_max ) k = 0;
      if( (l = j+yoff) < 0 ) l = y_max-1;
      else if( l >= y_max ) l = 0;
      pixel[l][k] = (pixel[l][k]&~mask) | (tmppix[j][i]&mask);
    }
  redo_sample();
  display();
}

int set_obflags( OBJECT *o, int ind, int true, int bit )
{
  o += ind;
  if( true ) o->ob_flags |= bit;
  else o->ob_flags &= ~bit;
  return true;
}

int ital_if( OBJECT *o, int ind, int true )
{
  return set_obflags( o, ind, true, X_ITALICS );
}

int bold_if( OBJECT *o, int ind, int true )
{
  return set_obflags( o, ind, true, X_BOLD );
}

void pop_set( int main, int pop, int ind, int draw )
{
  int i, ed, dum;
  TEDINFO *t;
  
  if( !ed_pat && main==IEDDSEL )
  {
    if( ind==1 && color&0xf0 ) ed_color( &color, 0, draw );
    else if( ind==0 && !(color&0xf0) ) ed_color( &color, max_col[icolors], draw );
  }
  else if( main==IEDCSEL )
  {
    ital_if( editor, IEDCSEL, (i=ipopups[pop+ind].ob_flags)&X_ITALICS );
    bold_if( editor, IEDCSEL, i&X_BOLD );
  }
  else if( pop==IPFILE )	/* 003 */
  {
    t = fileatt==4 ? drive : not_drv;
    if( editor[IEDTEMPL].ob_spec.tedinfo != t )
    {
      if( draw && ed_obj )
      {
        wind_get( ed_handle, X_WF_DIALEDIT, &ed, &dum );
        wind_set( ed_handle, X_WF_DIALEDIT, 0, 0 );
      }
      editor[IEDTEMPL].ob_spec.tedinfo = t;
      t->te_ptext[0] = 0;
      if( draw )
      {
        ed_draw(IEDTEMPL);
        if( ed && ed_obj ) wind_set( ed_handle, X_WF_DIALEDIT, ed, -1 );
      }
    }
  }
  if( main==IEDPSEL ) editor[IEDPIMG].ob_spec.bitblk->bi_pdata =
      ipopups[IPSOLID+ind].ob_spec.bitblk->bi_pdata+3;
  else editor[main].ob_spec.free_string = ipopups[pop+ind].ob_spec.free_string+2;
  if( draw )
  {
    ed_draw( main );
    if( main==IEDCSEL || main==IEDVSEL || main==IEDDSEL ) ed_draw( main+1 );
  }
}

int pat_pls(void)
{
  if( ed_pat )
    if( graph->vplanes<2 ) return 0;
    else if( graph->vplanes<4 ) return 1;
    else return 2;
  else return icolors;
}

void ed_color( unsigned int *old, unsigned int new, int draw )
{
  unsigned int j;
  
  if( ed_pat )
  {
    if( new > (j=n2col[pat_pls()]-1) ) new = j;
    j = new_pattern.bi_color;
    j = new_pattern.bi_color = !icoldm ? (j&0xf0)|new : (j&0xf)|(new<<4);
  }
  else
  {
    j = (unsigned)i_samp.ib_char>>8;
    j = (j&0xf)|(new<<4);
  }
  *old = j;
  if( new+IEDCOL0 != ed_col )
  {
    j = ed_col;
    ed_col = new+IEDCOL0;
    if( draw )
    {
      ed_draw(j);
      ed_draw(ed_col);
      if( ed_pat ) redo_sample();
    }
  }
  if( !ed_pat )
  {
    j = iset;
    if( !new ) j = IPSET+1;
    else if( iset==IPSET+1 ) j = IPSET;
    if( j != iset ) pop_set( IEDDSEL, IPSET, (iset=j)-IPSET, draw );
  }
}

#define IMPOSSIBLE 100

extern long StkLim;

void seedfill( int xx, int yy, int col, int old )
{
  int last, last1, last_1, last_y;
  int y, x, xinc, yinc, i, j, mask;
  unsigned char *p;
  
  mask = pix_mask();
  if( xx<0 || xx>=x_max || yy<0 || yy>=y_max || (tmppix[yy][xx]&mask) != old ||
      (tmppix[yy][xx]&mask)==col ) return;
  if( (long)&mask < StkLim )
  {
    Crawio(7);
    return;
  }
  xinc = 1;
  yinc = 1;
  last_1 = xx;
  last1 = last_y = IMPOSSIBLE;
  for(;;)
  {
    y = yinc>0 || xinc<0 ? yy : yy-1;
    last = xinc>0 ? last1 : last_1;
    x = xinc>0 ? xx : xx-1;
      while( y>=0 && y<y_max )
      {
        i = x;
        while( i>=0 && i<x_max && (*(p=&tmppix[y][i])&mask) == old )
        {
          *p = *p&~mask | col;
          i += xinc;
        }
        if( last != IMPOSSIBLE && i!=last )
          if( xinc>0 )
            if( i<last )
              for( j=i+1; j<last; j++ )
                seedfill( j, y, col, old );
            else
              for( j=last+1; j<i; j++ )
                seedfill( j, y-yinc, col, old );
          else 
            if( i>last )
              for( j=i-1; j>last; j-- )
                seedfill( j, y, col, old );
            else
              for( j=last-1; j>i; j-- )
                seedfill( j, y-yinc, col, old );
        if( y==yy )
          if( xinc > 0 ) last1 = i;
          else last_1 = i;
        last = i;
        if( i!=x && last_y!=IMPOSSIBLE &&
            (yinc>0 && y>last_y || yinc<0 && y<last_y) ) 
            seedfill( x-xinc, y, col, old );
        if( i==x && (last_y==IMPOSSIBLE || 
            yinc>0 && y>=last_y || yinc<0 && y<=last_y) ) break;
        y += yinc;
      }
    if( xinc>0 )
    {
      xinc = -1;
      last_y = y-yinc;
    }
    else if( yinc>0 )
    {
      yinc = -1;
      xinc = 1;
      last_y = IMPOSSIBLE;
    }
    else return;
  }
}

void waitbut(void)
{
  int dum, b;
  
  do
    graf_mkstate( &dum, &dum, &b, &dum );
  while( b&1 );
}

void _draw_box( int *box )
{
  int ln;
  
  ln = (*graph->get_lnmask)();
  (*graph->draw_bx)(box);
  (*graph->set_lnmask)(ln);
}

void draw_line( int xs, int ys, int xf, int yf, int flag )
{
  int dx, dy, x, y, yinc, inc=0, i, dif, xarray[10];
  
  get_x0y0();
  dx = xf-xs;
  dy = yf-ys;
  y = !dy ? 0 : (dy>0 ? 1 : -1);
  x = !dx ? 0 : (dx>0 ? 1 : -1);
  if( (yinc = abs(dx) < abs(dy)) != 0 )
  {
    i = y;
    dif = x*(dy-i)/2;
  }
  else
  {
    i = x;
    dif = y*(dx-i)/2;
  }
  x = xs;
  y = ys;
  (*graph->wmode2)();
  for(;;)
  {
    xarray[4]=xarray[2] = (xarray[6]=xarray[0] = x0 + h_off*x - 1)
        + h_off;
    xarray[7]=xarray[5] = (xarray[3]=xarray[1] = y0 + v_off*y - 1)
        + v_off;
    _draw_box( xarray );
    if( flag )
    {
      set_pixel( x, y, flag-100 );
      (*graph->wmode2)();
    }
    if( x==xf && y==yf ) return;
    inc += i;
    if( yinc )
    {
      y += i;
      x = (inc*dx+dif)/dy + xs;
    }
    else
    {
      x += i;
      y = (inc*dy+dif)/dx + ys;
    }
  }
}

void fix_mouse( int *x, int *y )
{
  if( *x<0 ) *x=0;
  if( *y<0 ) *y=0;
  if( *x>=x_max ) *x = x_max-1;
  if( *y>=y_max ) *y = y_max-1;
}

void set_line( int x0, int y0, int color )
{
  int xinc, yinc, x, y, ox, oy, b, mx, my, state=0, i, mask=0;
  static int masks[] = { 0xCCCC, 0x6666, 0x3333, 0x9999 };
  
  ox = x0;
  oy = y0;
  grf_mouse( OUTLN_CROSS );
  if( !get_mouse( &x, &y, &b, &mx, &my ) ) fix_mouse( &x, &y );
  (*graph->set_lnmask)(masks[0]);
  while( b & 1 )
  {
    _lineaa();
    if( state ) draw_line( x0, y0, ox, oy, 0 );
    else state++;
    draw_line( x0, y0, x, y, 0 );
    _linea9();
    ox = x;
    oy = y;
    for(;;)
    {
      if( !get_mouse( &x, &y, &b, &mx, &my ) ) fix_mouse( &x, &y );
      if( b&1 && x==ox && y==oy )
      {
        for( i=0; i<4; i++ )
          Vsync();
        _lineaa();
        draw_line( x0, y0, ox, oy, 0 );
        if( ++mask == 4 ) mask = 0;
        (*graph->set_lnmask)(masks[mask]);
        draw_line( x0, y0, ox, oy, 0 );
        _linea9();
      }
      else break;
    }
  }
  if( state )
  {
    _lineaa();
    draw_line( x0, y0, ox, oy, 100+color );
    redo_sample();
    _linea9();
  }
  _arrow();
}

void color_it( int i, unsigned int *color )
{
  if( i != icoldm )
  {
    icoldm = i;
    if( /*%!ed_pat && !i || ed_pat &&*/ i )
    {
      i = *color>>4;
      *color = ((*color&0xf)<<4)|(*color&0xf);
    }
    else
    {
      i = *color&0xf;
      *color = (*color&0xf0)|(*color>>4);
    }
    ed_color( color, i, 1 );
  }
}

void set_toggle(void)
{
  int i;
  
  i = ipopups[IPTOGGLE].ob_state&DISABLED;
  if( ipat!=IPSOLID && !i )
  {
    if( iset==IPTOGGLE ) pop_set( IEDDSEL, iset=IPSET, 0, 1 );
    ipopups[IPTOGGLE].ob_state |= DISABLED;
  }
  else if( ipat==IPSOLID && i ) ipopups[IPTOGGLE].ob_state &= ~DISABLED;
}

void _from_filename( char *src, char *dest, int flg )
{
  (*_icneo->from_filename)( src, dest, flg );
}

void _pad_it( char *ptr )
{
  int j;
  
  j = strlen(ptr);
  while( j<11 ) ptr[j++] = ' ';
  ptr[11] = '\0';
}

int do_ipopup( int parent, int obj, int val )
{
  MENU m, out;
  int x, y, i;

  m.mn_tree = ipopups;
  m.mn_menu = parent;
  m.mn_item = parent+val+1;
  m.mn_scroll = 0;
  if( parent != IEDDPAT0-1 )
    for( i=parent+1; i<=ipopups[parent].ob_tail; i++ )
      if( i==m.mn_item ) ipopups[i].ob_state |= CHECKED;
      else ipopups[i].ob_state &= ~CHECKED;
  objc_offset( editor, obj, &x, &y );
  if( menu_popup( &m, x-2, y-2, &out ) )
  {
    i = out.mn_item-parent-1;
    return i==val ? -1 : i;
  }
  return -1;
}

void imenu_set( int start, int len, int ind )
{
  for( len+=start; --len>=start; )
    if( len==ind ) ed_menu[len].ob_state |= CHECKED;
    else ed_menu[len].ob_state &= ~CHECKED;
}

void to_letter(void)
{
  if( move ) xor_box( 0, mbox.x, mbox.y, mbox.w, mbox.h );
  idcml = IEDLET;
  xor_box( move=1, mbox.x=i_samp.ib_xchar, mbox.y=i_samp.ib_ychar,
      mbox.w=5, mbox.h=5 );
}

void black_box( int *box, int w, int h )
{
  box[2] = box[4] = (box[6] = box[0]) + w-1;
  box[5] = box[7] = (box[3] = box[1]) + h-1;
  (*graph->wmode0)();
  (*graph->set_lnmask)(-1);
  vsl_color( graph->handle, 1 );
  (*graph->draw_bx)( box );
}

int cdecl draw_display( PARMBLK *pb )
{
  int xarray[8];

  xarray[0] = xarray[1] = 0;
  xarray[6] = (xarray[4] = pb->pb_x+1) +
      (xarray[2] = pb->pb_w + 1);
  xarray[7] = (xarray[5] = pb->pb_y+1) +
      (xarray[3] = pb->pb_h + 1);
  (*_nac->set_clip_rect)( (Rect *)&pb->pb_xc, 1 );
  vro_cpyfm( graph->handle, S_ONLY, xarray, &disp, &screen );
  xarray[0] = xarray[4] - 1;
  xarray[1] = xarray[5] - 1;
  black_box( xarray, xarray[2]+3, xarray[3]+3 );
  if( move )
  {
    set_xarray( Xrect(mbox), xarray );
    _draw_xor( xarray );
  }
  return(0);
}

void disp_all( int corn )
{
  int y, i, x, j, cols[2], xarray[8];
  char md;
  unsigned char *p;
  
  md = pix_mask();
  (*graph->wmode0)();
  xarray[0] = xarray[1] = 0;
  xarray[2] = xarray[3] = 0;
  (*_nac->set_clip_rect)( (Rect *)xarray, 0 );
  cols[0] = 1;
  cols[1] = 0;
  get_x0y0();
  for( y=y0, i=y_max+1; --i>=0; y+=v_off )
  {
    if( corn )
    {
      xarray[7] = xarray[5] = y-y0;
      for( j=x_max+1, x=0; --j>=0; x+=h_off )
      {
        xarray[6] = xarray[4] = x;
        vrt_cpyfm( graph->handle, MD_REPLACE, xarray, &corner, &disp, cols );
      }
    }
    if( i>0 )
    {
      p = &pixel[y_max-i][0];
      for( x=x0, j=x_max; --j>=0; x+=h_off )
        draw( x, y, *p++ & md, 0 );
    }
  }
}

void pline_in( int *oarray )
{
  oarray[6] = ++oarray[0];
  oarray[4] = --oarray[2];
  oarray[3] = ++oarray[1];
  oarray[7] = --oarray[5];
}

void in_frame( int *box, int col )
{
  pline_in(box);
  vsl_color( graph->handle, col );
  (*graph->draw_bx)( box );
}

int cdecl draw_color( PARMBLK *pb )
{
  int box[8], i;
  
  if( imode != IPDATA+1 )
  {  
    (*_nac->set_clip_rect)( (Rect *)&pb->pb_xc, 1 );
    (*graph->wmode0)();
    (*graph->pats)( (int *)pb->pb_parm, 15 );
    (*graph->desk_color)( no_color ? 1 : xpall(pb->pb_obj-IEDCOL0) );
    (*graph->x1y1val)( pb->pb_x, pb->pb_y, pb->pb_x+pb->pb_w-1, pb->pb_y+pb->pb_h-1 );
    (*graph->gr_box)();
    offset_objc( pb->pb_tree, pb->pb_obj, &box[0], &box[1] );
    black_box( box, pb->pb_tree[pb->pb_obj].ob_width, pb->pb_tree[pb->pb_obj].ob_height );
    if( pb->pb_obj==ed_col )
    {
      in_frame( box, 0 );
      in_frame( box, 1 );
    }
  }
  return 0;
}

int cdecl draw_sicon( PARMBLK *pb )
{
  int s = pb->pb_currstate&SELECTED;

  (*_icneo->draw_icon)( &nib_samp, pb->pb_x, pb->pb_y, pb->pb_obj==IEDDIC1 ||
      !s ? _msg_ptr[15] : _msg_ptr[16], idcml==IEDLET ? (char)i_samp.ib_char : 0,
      s, pb->pb_w );
  return 0;
}

int i_editor( OBJECT *o, FORM *f );
int x_editor( OBJECT *o, int num, FORM *f );
int t_editor( OBJECT *o, int num, FORM *f );
int i_iabout( OBJECT *o, FORM *f );
int tx_iabout( OBJECT *o, int num, FORM *f );
int i_nic_info( OBJECT *o, FORM *f );
int x_nic_info( OBJECT *o, int num, FORM *f );

static FSTRUCT *edit_file;
static ICONBLK *i_ptr0;
static BITBLK *pptr;
static char pixes[3][2][2] = { 3, 3, 6, 5, 7, 3, 10, 5, 7, 7, 12, 12 };
static Rect old;

static FORM_TYPE forms[] = {
    { -1, NO_POS,   { 0, 0, 0, 0, 1, 0, 0, 1, 0 }, 0L, i_editor, t_editor,
      x_editor, 0L, 0L },
    { -1, NO_POS-1, { 0, 1, 0, 0, 0, 0, 0, 0, 0 }, 0L, i_iabout, tx_iabout,
      tx_iabout, 0L, 0L },
    { -1, NO_POS-2, { 0, 0, 0, 0, 1, 0, 0, 1, 0 }, 9L, i_nic_info, 0L,
      x_nic_info, 0L, 0L }     /* always non-modal so that update will work right */
    };

int edit_icon( FSTRUCT *file, int wnum )
{
  FSTRUCT *old;
  
  old = edit_file;
  edit_file = file;
  ed_wnum = wnum;
  if( !(*gui->start_form)( *_icneo->AES_handle, edit_file==0L || graph->v_y_max > 200 ?
      ((long)X_MENU<<16)|NAME|MOVER|CLOSER : ((long)X_MENU<<16),
      &forms[0] ) ) edit_file = old;	/* if it fails, restore old value */
  return 0;
}

static int crosses[3][7] = {
    { 0xA000, 0x4000, 0xA000 },
    { 0xC600, 0x3800, 0xC600 },
    { 0x0000, 0x4400, 0x2800, 0x1000, 0x2800, 0x4400, 0x0000 } };  /* 0x8200 */

void get_pixes( int pat )
{
  int i=0;

  if( graph->v_x_max >= 640 )
  {	/* reordered for 002 */
    i=1;
    if( _use_8x16 ) i=2;
  }
  h_off = (pix_w = pixes[i][pat][0]) + 1;
  v_off = (pix_h = pixes[i][pat][1]) + 1;
  x_max = ed_pat ? 16 : ICON_W;
  y_max = ed_pat ? 16 : ICON_H;
  mask_dat = crosses[i];
}

static int hide_if( OBJECT *form, int num, int truth )
{
  if( truth ) form[num].ob_flags &= ~HIDETREE;
  else form[num].ob_flags |= HIDETREE;
  return truth;
}

void redo_fills( int draw, int old_cols )
{
  int i, j, cols, x, y;
  long l;
  static divs[3][2] = { 2, 1, 2, 2, 4, 4 };
  unsigned char *fl, *o;

  /* go to black if prev was black or no longer a valid color */  
  if( !ed_pat && (ed_col-IEDCOL0 > (i=max_col[icolors]) ||
      ed_col-IEDCOL0 == max_col[old_cols]) ) ed_col = IEDCOL0+i;
  if( (no_color = graph->work_out[13] < n2col[icolors]) == 0 )
      memset( fill_dat, -1, 16*2*16 );
  else
  {
    o = (unsigned char *)fill_dat[0];
    for( i=0; i<16; i++, o+=16 )
    {
      l = (*_icneo->dflt_pall)[xpall(i)];
      fl = &(*_icneo->floydbytes)[( ((l>>16L)&0xFF)*30 + ((l>>8L)&0xFF)*59 +
          (l&0xFF)*11 ) * 64 / 25500][0];
      for( j=8; --j>=0; o+=2 )
        *o = *(o+1) = *(o+16) = *(o+17) = *fl++;
    }
  }
  cols = pat_pls();
  x = editor[IEDCOL0-1].ob_width / (j=divs[cols][0]);
  y = editor[IEDCOL0-1].ob_height / divs[cols][1];
  for( i=0; i<16; i++ )
    if( hide_if( editor, i+IEDCOL0, i<n2col[cols] ) )
    {
      editor[i+IEDCOL0].ob_x = (i%j) * x;
      editor[i+IEDCOL0].ob_y = (i/j) * y;
      editor[i+IEDCOL0].ob_width = x;
      editor[i+IEDCOL0].ob_height = y;
      editor[i+IEDCOL0].ob_spec.userblk->ub_parm = (long)fill_dat[i];
      editor[i+IEDCOL0].ob_spec.userblk->ub_code = draw_color;
    }
  if( draw ) ed_draw( IEDCOL0-1 );
}

int alloc_rez( int pln, CICON *c )
{
  CICON *ci;
  
  if( !(*_icneo->copy_cicon)( n2pl[pln], c, ci=&i_cic[pln] ) ) return 0;
  nib_samp.list[pln] = ci;
  return 1;
}

void set_collist( int draw )
{
  int i;
  
  for( i=0; i<3; i++ )
    bold_if( ipopups, IP2COL+i, !ital_if( ipopups, IP2COL+i,
        nib_samp.list[i]==0 ) && nib_samp.list[i]->sel_data!=0 );
  pop_set( IEDCSEL, IP2COL, icolors, draw );
}

void get_pixels(void)
{
  int i, j, k, pl, s, lastpl, data;
  long *d, *d2, *m, mask;
  unsigned char *p;
  
  d = iselected ? (long *)i_cic[icolors].sel_data : (long *)i_cic[icolors].col_data;
  m = iselected ? (long *)i_cic[icolors].sel_mask : (long *)i_cic[icolors].col_mask;
  p = pixel[0];
  pl = n2pl[icolors];
  lastpl = ICON_H*(pl-1);
  for( i=y_max; --i>=0; d++ )
  {
    mask = *m++;
    for( k=32; --k>=0; )
    {
      for( d2=d+lastpl, data=0, j=pl; --j>=0; d2-=ICON_H )
        data = (data+data) | ((int)(*d2>>k)&1);
      *p++ = data | ((int)(mask>>k)<<7);  /* don't need &1 */
    }
  }
}

/* must call free_rez first! */
int cur_rez( int do_d, int do_s, int get, int planes )
{
  int pl, devspef;
  CICON *ci = &i_cic[icolors];

  if( (pl=ci->num_planes) != 1 || planes>0 )	  /* skip if source is mono and not copying */
  {
    if( planes<0 )
    {
      planes = graph->vplanes;
      devspef = -1;
    }
    else devspef = -2;
    ic_samp.num_planes = pl > planes ? 1 : planes;
    if( do_d )
      if( (!do_s || do_s<0 ) && !fix_rez( ci->col_data, &ic_samp.col_data, pl, planes, devspef ) ||
          do_s && !fix_rez( ci->sel_data, &ic_samp.sel_data, pl, planes, devspef ) ) return 0;
  }
  else
  {
    ic_samp.num_planes = 1;
    ic_samp.col_data = ci->col_data;
    ic_samp.sel_data = ci->sel_data;
  }
  ic_samp.col_mask = ci->col_mask;
  ic_samp.sel_mask = ci->sel_mask;
  nib_samp.ci = &ic_samp;
  if(get) get_pixels();
  return 1;
}

int start_icon(void)
{
  int i, j;
  CICON temp;
  NICONBLK *nib;
  ICONBLK *ib;
  
  ib=(nib=edit_file->nib)->ib;
  __bytecpy( &i_samp, i_ptr0, sizeof(ICONBLK) );
  nib_samp.ib = &i_samp;
  if( !ib->ib_pdata )
  {
    if( !(*_icneo->alloc_im)( &ib->ib_pdata, 1 ) ) return 0;
    _memclr( ib->ib_pdata, DATASIZ );
  }
  if( !ib->ib_pmask )
  {
    if( !(*_icneo->alloc_im)( &ib->ib_pmask, 1 ) ) return 0;
    _memclr( ib->ib_pmask, DATASIZ );
  }
  for( j=-1, i=0; i<3; i++ )
    if( nib->list[i] )
    {
      if( !alloc_rez( i, nib->list[i] ) ) return 0;
      if( j<0 ) j = i;
    }
    else if( !i )
    {
      temp.col_data = ib->ib_pdata;
      temp.col_mask = ib->ib_pmask;
      temp.sel_data = temp.sel_mask = 0L;
      if( !alloc_rez( i, &temp ) ) return 0;
      if( j<0 ) j = i;
    }
    else nib_samp.list[i] = 0L;
  if( j<0 ) return 0;
  if( !nib_samp.list[icolors] ) icolors = j;
  set_collist(0);
  cur_rez( 1, -1, 1, -1 );
  return 1;
}

static void free_icon( CICON **ci, CICON *curr, int root )
{
  (*_icneo->free_icon)( ci, curr, root );
}

static void free_nib_icon( NICONBLK *n )
{
  (*_icneo->free_nib_icon)(n);
}

void free_cic(void)
{
  int i;
  CICON *ci;
  
  free_rez(-1);
  ci = i_cic;
  for( i=3; --i>=0; ci++ )
    free_icon( &ci, 0L, 0 );
}

void icon2patt( ICONBLK *ib, BITBLK *b )
{
  int *m, i, *o;
  
  for( m=ib->ib_pdata, o=b->bi_pdata, i=16; --i>=0; m+=2 )
    *o++ = *m;
  b->bi_color = ((ib->ib_char>>12)&0xf) | ((ib->ib_char>>4)&0xf0);
}

int edbb_data[16];
BITBLK ed_bb = { edbb_data, 2, 16, 0, 0, 0x10 };

char *dflt_ed_name(void)
{
  char *str;
  
  wind_set( ed_handle, WF_NAME, str = ed_wnum>0 ? spathend(_z->w[ed_wnum].path) : "" );
  return str;
}

void get_pat_pix( int *iptr )
{
  int i, j, k;
  
  for( i=0; i<y_max; i++ )
  {
    k = *iptr++;
    for( j=16; --j>=0; )
    {
      pixel[i][j] = k&1;
      k >>= 1;
    }
  }
}

static int fix_att( int in, int from_ib )	/* 003 */
{
  if( from_ib )
    if( (in&7)==ICON_DRIVE ) return 4;
    else return ((in&3)^3)-1;
  else if( in==4 ) return ICON_DRIVE;
  else return (in+1)^3;
}

/*% static int cdecl w_sample( PARMBLK *pb )
{
  int arr[4];

  (*_nac->set_clip_rect)( (Rect *)&pb->pb_xc, 1 );
  arr[2] = (arr[0]=pb->pb_x) + pb->pb_w-1;
  arr[3] = (arr[1]=pb->pb_y) + pb->pb_h-1;
  (*graph->wmode0)();
  (*_icneo->window_box)(arr);
  return 0;
}

USERBLK wind_samp = { w_sample };  **/

#pragma warn -par

int i_editor( OBJECT *o, FORM *f )
{
  int i, j, k, *iptr;
  
  if( (ed_pat = edit_file==0L || edit_file->type.p.pexec_mode==NPI) == 0 ) i_ptr0 = edit_file->nib->ib;
  move=0;
  get_pixes( ed_pat );
  /* o may not be set yet, so change editor directly */
  i = j = editor[IEDDISP].ob_x;
  i += (editor[IEDDISP].ob_width = x_max * h_off - 1);
  editor[0].ob_width = i + editor[IEDINVIS].ob_width;
  editor[IEDDISP].ob_height = y_max * v_off - 1;
  editor[IEDINVIS].ob_x = i;
  if( !o )
  {
    f->tree = editor;
    return 1;
  }
  if( !not_drv ) not_drv = o[IEDTEMPL].ob_spec.tedinfo;
  _bee();
  o[IEDDIC1].ob_height = o[IEDDIC1+1].ob_height =
      o[IEDWIC1].ob_height = o[IEDWIC1+1].ob_height = 40;
  hide_if( o, IEDWIND, !ed_pat && i>=o[IEDWIND].ob_x+o[IEDWIND].ob_width-4 );
  for( i=0; i<16; i++ )
    if( i<graph->work_out[13] ) pallette[i] = (*_nac->setcolor)( i, -1L );
    else pallette[i] = (*_icneo->dflt_pall)[i];
  o[IEDDESK].ob_type = G_USERDEF;
  o[IEDDESK].ob_spec = _z->desk[0].ob_spec;
  *((char *)&o[IEDWIND].ob_spec.obspec+3) = (char)_z->wind_prf.i;
/*%  if( !wind_samp.ub_parm ) wind_samp.ub_parm = (long)o[IEDWIND].ob_spec.userblk;
  o[IEDWIND].ob_spec.userblk = &wind_samp;
  o[IEDWIND].ob_type = G_IBOX | (X_USRDEFPRE<<8);
  o[IEDWIND].ob_state |= X_MAGIC; */
  hide_if( o, IEDPLUS, hide_if( o, IEDPIMG, hide_if( o, IEDSEL, hide_if( o, IEDCBOX,
      hide_if( o, IEDVBOX, hide_if( o, IEDTBOX, !ed_pat ) ) ) ) ) );
  hide_if( o, IEDDBINV, ed_pat );
  menu_ienable( ed_menu, IEDLET, !ed_pat );
  menu_ienable( ed_menu, IEDMASK, !ed_pat );
  ed_obj = 0;
  color = -1;
  if( ed_pat )
  {
    old = *(Rect *)&o[IEDDESK].ob_x;
    o[IEDDESK].ob_x = j;
    o[IEDDESK].ob_y = (j=o[IEDDISP].ob_y)+o[IEDDISP].ob_height+j;
    o[IEDDESK].ob_width = o[IEDDISP].ob_width + 4;
    o[IEDDESK].ob_height = o[IEDINVIS].ob_y + o[IEDINVIS].ob_height - o[IEDDESK].ob_y;
    o[IEDDIC1].ob_flags = o[IEDDIC1+1].ob_flags |= HIDETREE;
    if( idcml == IEDLET ) idcml = IDRAW;
    ipat = IPSOLID;
    om = imode;
    imode = IPDATA;
    icolors = 0;
    (*_icneo->obj_true)( o, !icoldm, IEDDATA );
    if( edit_file != 0 )	/* desktop pattern */
        icon2patt( edit_file->nib->ib, pptr=&ed_bb );
    else pptr=(*_icneo->deskpat)->ob_spec.bitblk;
    iptr = pptr->bi_pdata;
    ed_color( &color, (unsigned)pptr->bi_color>>(!icoldm?0:4)&0xf, 0 );
    color = pptr->bi_color;
  }
  else
  {
    (*_icneo->obj_true1)( o, iselected=0, IEDSEL );
    if( !start_icon() )
    {
      _arrow();
      return 0;
    }
    o[IEDDIC1].ob_spec.userblk->ub_code = 
        o[IEDDIC1+1].ob_spec.userblk->ub_code = 
        o[IEDWIC1].ob_spec.userblk->ub_code = 
        o[IEDWIC1+1].ob_spec.userblk->ub_code = draw_sicon;
    if( ed_col-IEDCOL0 > (i=max_col[icolors]) ) ed_col = i+IEDCOL0;
    ed_color( &color, ed_col-IEDCOL0, 0 );
    pop_set( IEDTSEL, IPFILE, fileatt = fix_att( i_ptr0->ib_char, 1 ), 0 );	/* 003: moved these three up here */
    pop_set( IEDVSEL, IPDATA, imode-IPDATA, 0 );
    pop_set( IEDCSEL, IP2COL, icolors, 0 );
    if( edit_file->type.p.pexec_mode >= D_PROG )
    {
      if( fileatt==4 ) o[IEDTEMPL].ob_spec.tedinfo->te_ptext[0] = edit_file->nib->ib->ib_ptext[0]; /* 003 */
      else (*_icneo->to_filename)( edit_file->name, o[IEDTEMPL].ob_spec.tedinfo->te_ptext );
      ed_obj = IEDTEMPL;
    }
    else hide_if( o, IEDTBOX, 0 );
  }
  if( !ed_obj )
  {
    o[IEDTEMPL].ob_flags |= HIDETREE;
    o[IEDTEMPL].ob_flags &= ~EDITABLE;
  }
  redo_fills( 0, icolors );
  o[IEDDISP].ob_spec.userblk->ub_code = draw_display;
  pop_set( IEDDSEL, IPSET, iset-IPSET, 0 );
  pop_set( IEDPSEL, IPSOLID, ipat-IPSOLID, 0 );
  imenu_set( IDRAW, 4, idcml );
  imenu_set( INONE, 4, imirr );
  wind_set( ed_handle=f->handle, X_WF_MENU, ed_menu );
  f->old_title = dflt_ed_name();
  if( ed_pat )
  {
    __bytecpy( &new_pattern, pptr, sizeof(BITBLK) );
    __bytecpy( new_pat, iptr, 32 );
    new_pattern.bi_pdata = (int *)new_pat;
    _icic.pattern = &new_pattern;
    get_pat_pix( iptr );
  }
  if( graph->vplanes >= 15 ) memset( disp.fd_addr, -1, disp_size );
  else _memclr( disp.fd_addr, disp_size );
  disp_all(1);
  redo_prev();
  if( idcml == IEDLET ) to_letter();
  _arrow();
  return 1;
}

void samp_state( int first, int second )
{
  editor[first].ob_state ^= SELECTED;
  editor[second].ob_state ^= SELECTED;
  ed_draw(first-1);
  waitbut();
}

int new_rez( CICON *ci, int sel )
{
  int **d;
  
  d = sel ? &ci->sel_data :  &ci->col_data;
  if( (*_icneo->alloc_im)( d, ci->num_planes ) )
    if( (*_icneo->alloc_im)( sel ? &ci->sel_mask :  &ci->col_mask, 1 ) ) return 1;
    else cmfree( (char **)d );
  return 0;
}

void no_select(void)
{
  iselected = 0;
  x_wdial_change( ed_handle, IEDSEL, editor[IEDSEL].ob_state&~SELECTED );
}

int iget_top(void)
{
  int top, dum;
  
  wind_get( 0, WF_TOP, &top, &dum, &dum, &dum );
  return top;
}

void change_sel(void)
{
  prev_ok = 0;
  get_pixels();
  disp_all(0);
  display();
}

void goto_colors( int i )
{
  int j;
  
  prev_ok = 0;
  if( !i_cic[i].sel_data && iselected ) no_select();
  free_rez(-1);	/* free before changing icolors! */
  j = icolors;
  pop_set( IEDCSEL, IP2COL, icolors=i, 1 );
  redo_fills( 1, j );
  cur_rez( 1, -1, 1, -1 );
  disp_all(0);
  display();
  sample();
}

int rez_copy( int old )
{
  int i, list[2], num;
  char temp[200];
  
  for( i=num=0; i<3; i++ )
    if( i!=old && nib_samp.list[i] ) list[num++] = i;
  if( !num ) return -1;	/* nowhere to copy from */
  if( num==2 )
  {
    _spf( temp, _msg_ptr[7], n2col[list[0]], n2col[list[1]] );
    switch( _f_alert1( temp ) )
    {
      case 1:
        return list[0];
      case 2:
        return list[1];
      case 3:
        return -1;
    }
  }
  return list[0];
}

int get_clip( char *out, int del )
{
  if( x_scrp_get( out, del ) )
  {
    strcat( out, "SCRAP.IMG" );
    return 1;
  }
  return 0;
}

enum
{
  NOCUT,
  CUTB,
  COPYB,
  PASTEB
};

void cut_mode( int num )
{
  static char checks[] = { IEDCUTB, IEDCOPYB, IEDPASTB };
  
  if( cutmode == num ) return;
  if( num==NOCUT )
  {
    dflt_ed_name();
    if( cutmode<PASTEB ) menu_icheck( ed_menu, checks[cutmode-CUTB], 0 );
  }
  else
  {
    wind_set( ed_handle, WF_NAME, _msg_ptr[40+num-CUTB] );
    if( num<PASTEB ) menu_icheck( ed_menu, checks[num-CUTB], 1 );
  }
  cutmode = num;
}

void to_mask( char *p )
{
  strcpy( strrchr(p,'.'), ".MSK" );
}

int write_img( char *name, int x, int y, int w, int h, int *data, int pl )
{
#ifndef DEMO
  PICTURE pic;
  int l, rgb[16][3], py, *o;
  long d, *d2, mask;
  
  _memclr( &pic, sizeof(PICTURE) );
  pic.mfdb.fd_w = w;
  pic.mfdb.fd_h = h;
  pic.mfdb.fd_wdwidth = (w+15)>>4;
  pic.mfdb.fd_stand = 1;
  pic.mfdb.fd_nplanes = pl;
  if( (pic.mfdb.fd_addr = lalloc( l=pl*pic.mfdb.fd_wdwidth*pic.mfdb.fd_h<<1,
      ALLOC_ICON )) == 0 ) return 0;
  get_rgb( &rgb );
  pic.intens = rgb;
  _memclr( o=pic.mfdb.fd_addr, l );
  data += y<<1;
  mask = w==32 ? -1L : ((1L<<w)-1)<<(32-w);
  for( ; --pl>=0; data+=DATASIZ/2 )
    for( d2=(long *)data, py=h; --py>=0; )
    {
      d = (*d2++ << x) & mask;
      if( w<=16 ) *o++ = d>>16;
      else *((long *)o)++ = d;
    }
  (*_icneo->save_img)( name, &pic );
  lfree( pic.mfdb.fd_addr );
  return 1;
#else DEMO
  return 0;
#endif DEMO
}

long *img_data( int mask, long *patbuf )
{
  int i, *l, *d;
  
  if( !ed_pat )
    if( mask )
      if( !iselected ) return (long *)i_cic[icolors].col_mask;
      else return (long *)i_cic[icolors].sel_mask;
    else if( !iselected ) return (long *)i_cic[icolors].col_data;
    else return (long *)i_cic[icolors].sel_data;
  for( i=16, l=(int *)patbuf, d=(int *)new_pat; --i>=0; )
  {	/* move the pattern into an icon-sized block */
    *l++ = *d++;
    *l++ = 0;
  }
  return patbuf;
}
      
void save_clip( int x, int y, int w, int h, int cut )
{
  char temp[120], m;
  int i;
  long patbuf[16];

  if( !w || !h ) return;  
  if( !get_clip( temp, 1 ) )
  {
    _arrow();
    return;
  }
  if( imode!=IPDATA+1 )
    if( !write_img( temp, x, y, w, h, (int *)img_data( 0, patbuf ), n2pl[icolors] ) )
    {
      _arrow();
      return;
    }
  if( imode!=IPDATA )
  {
    to_mask(temp);
    write_img( temp, x, y, w, h, (int *)img_data( 1, patbuf ), 1 );
  }
  _arrow();
  if( cut )
  {
    redo_prev();
    h += y;
    m = ~pix_mask();
    while( y++<h )
      for( i=x; i<x+w; i++ )
        pixel[y-1][i] &= m;
    redo_sample();
    do_disp( move, mbox );
  }
}

int paste_img( int x, int y, PICTURE *pic, long *d, int pl )
{
#ifndef DEMO
  int w, h, py;
  long *s, *s2, *d2, mask, pls;
  
  if( pic->mfdb.fd_nplanes != pl )
    if( (*_icneo->transform_pic)( pic, 0 ) )
    {
      (*_icneo->free_pic)( &pic, 0 );
      return 0;
    }
  if( (w=pic->mfdb.fd_w) + x > 32 ) w = 32-x;
  h = pic->mfdb.fd_h;
  if( !ed_pat )
  {
    if( h + y > 32 ) h = 32-y;
  }
  else if( h + y > 16 ) h = 16-y;
  pls = (long)pic->mfdb.fd_wdwidth * pic->mfdb.fd_h;
  s = (long *)pic->mfdb.fd_addr;
  d += y;
  mask = w==32 ? -1L : ((1L<<w)-1)<<(32-x-w);
  for( ; --pl>=0; d+=32 )
  {
    for( d2=d, s2=s, py=h; --py>=0; )
    {
      *d2++ = (*d2 & ~mask) | ((*s2>>x) & mask);
      s2 = (long *)((int *)s2 + pic->mfdb.fd_wdwidth);
    }
    if( pic->mfdb.fd_nplanes != 1 ) s = (long *)((int *)s + pls);	/* if dithered, reuse same plane */
  }
  (*_icneo->free_pic)( &pic, 0 );
  return 1;
#else
  return 0;
#endif
}

int get_pastepos( int *x, int *y )
{
  int i, b, mx, my;
  
  cut_mode( PASTEB );
  wind_update(BEG_UPDATE);
  do
    i = get_mouse( x, y, &b, &mx, &my );
  while( !(b&1) );
  wind_update(END_UPDATE);
  waitbut();
  bounce = 1;
  cut_mode(0);
  return i;
}

void paste_clip( int x, int y )
{
  char temp[120], got_mask=0, do_d;
  PICTURE pic;
  int hand, i, *l, *d;
  long patbuf[16];

  if( !get_clip( temp, 0 ) )
  {
    _arrow();
    return;
  }
  if( imode==IPDATA+1 )
  {
    got_mask++;
    to_mask(temp);
  }
  if( (hand = Fopen(temp,0))<0 )
    if( imode!=IPDATA+2 )
    {
      _arrow();
      _f_alert1( _msg_ptr[20] );
      return;
    }
    else
    {
      got_mask++;
      to_mask(temp);
      if( (hand = Fopen(temp,0))<0 )
      {
        _arrow();
        _f_alert1( _msg_ptr[20] );
        return;
      }
    }
  _memclr( &pic, sizeof(PICTURE) );
  i = (*_icneo->load_img)( hand, &pic );
  Fclose(hand);
  _arrow();
  if( i ) return;
  if( x<0 )    	/* don't know location yet */
    if( !get_pastepos( &x, &y ) ) return;
  redo_prev();
  (*_icneo->set_temps)( got_mask ? 1 : n2pl[icolors] );
  do_d = !got_mask;
  if( paste_img( x, y, &pic, img_data( got_mask, patbuf ), got_mask ?
      1 : n2pl[icolors] ) && imode==IPDATA+2 && !got_mask )
  {
    to_mask(temp);
    _bee();
    if( (hand = Fopen(temp,0))<0 )
    {
      _arrow();
      if( imode==IPDATA+1 ) _f_alert1( _msg_ptr[20] );
      return;
    }
    _memclr( &pic, sizeof(PICTURE) );
    i = (*_icneo->load_img)( hand, &pic );
    Fclose(hand);
    _arrow();
    (*_icneo->set_temps)(1);
    paste_img( x, y, &pic, img_data( 1, patbuf ), 1 );
  }
  for( i=16, l=(int *)patbuf, d=(int *)new_pat; --i>=0; )
  {
    *d++ = *l;
    l+=2;
  }
  if( !ed_pat )
  {
    if( do_d ) free_rez( iselected );
    cur_rez( do_d, iselected, 1, -1 );
  }
  else get_pat_pix( pptr->bi_pdata );
  disp_all(0);
  display();
  sample();
}

int cc_obj( OBJECT *o, int mx, int my )
{
  int new;

  if( (new = objc_find( o, 0, 8, mx, my )) < IEDCOL0 ||
      new >= IEDCOL0+n2col[icolors] ) return 0;
  return new;
}

void copy_color( OBJECT *o, int num )
{
  int mx, my, mw, mh, i, j, new, bx, by;
  
  if( ed_pat || imode==IPDATA+1 ) return;
  graf_mkstate( &mx, &my, &mw, &mh );
  if( !(mw&1) || cc_obj( o, mx, my ) == 0 ) return;
  objc_offset( o, num, &mx, &my );
  mw = o[num].ob_width;
  mh = o[num].ob_height;
  objc_offset( o, IEDCOL0-1, &bx, &by );
  wind_update( BEG_UPDATE );
  grf_mouse( POINT_HAND );
  wind_set( ed_handle, WF_NAME, _msg_ptr[0] );
  if( graf_dragbox( mw, mh, mx, my, bx, by, o[IEDCOL0-1].ob_width,
      o[IEDCOL0-1].ob_height, &mx, &my ) )
  {
    graf_mkstate( &mx, &my, &mw, &mh );
    if( (new = cc_obj( o, mx, my )) > 0 && new != num )
    {
      redo_prev();
      for( i=0; i<x_max; i++ )
        for( j=0; j<x_max; j++ )
          if( (pixel[i][j]&0x7f) == new-IEDCOL0 )
              pixel[i][j] = (pixel[i][j] & 0x80) | (num-IEDCOL0);
      redo_sample();
      do_disp( move, mbox );
    }
  }
  dflt_ed_name();
  _arrow();
  wind_update( END_UPDATE );
}

int t_editor( OBJECT *o, int num, FORM *f )
{
  int i, j, k, l, *iptr, x, y, b, lb, mx, my, data, mask, nx, ny;
  Rect cutbox;

  if( num != IEDDISP ) cut_mode(0);  
  if( num != -1 )
sw: if( !(o[num].ob_state&DISABLED) ) switch(num)
    {
      case IEDDIC1:
      case IEDDIC1+1:
        samp_state( IEDDIC1, IEDDIC1+1 );
        break;
      case IEDWIC1:
      case IEDWIC1+1:
        samp_state( IEDWIC1, IEDWIC1+1 );
        break;
      case IEDUP:
        shift( 0, -1 );
        break;
      case IEDLEFT:
        shift( -1, 0 );
        break;
      case IEDRT:
        shift( 1, 0 );
        break;
      case IEDDOWN:
        shift( 0, 1 );
        break;
      case IEDVPLUS:
        if( (i=imode-IPDATA+1) > 2 ) i = 0;
        goto vplus;
      case IEDVSEL:
        if( (i=do_ipopup( IPDATA-1, IEDVSEL, imode-IPDATA )) >= 0 )
        {
vplus:    pop_set( IEDVSEL, IPDATA, i, 1 );
          j = imode==IPDATA+1;
          imode = i+IPDATA;
          if( j != (i==1) ) ed_draw( IEDCOL0-1 );
          set_toggle();
          disp_all(0);
          do_disp( move, mbox );
        }
        break;
      case IPOINT:
      case ILINE:
      case IFILL:
      case IBOX:
      case IFILBOX:
        istyle = num;
        break;
      case IEDDATA:
      case IEDDATA+1:
        color_it( num-IEDDATA, &color );
        break;
      case IEDDSEL:
        if( (i=do_ipopup( IPSET-1, IEDDSEL, iset-IPSET )) >= 0 )
        {
          pop_set( IEDDSEL, IPSET, i, 1 );
          iset = i+IPSET;
        }
        break;
      case IEDDPLUS:
        if( (i=iset-IPSET+1) > 2 || i==2 && ipat!=IPSOLID ) i = 0;
        pop_set( IEDDSEL, IPSET, i, 1 );
        iset = i+IPSET;
        break;
      case IEDCPLUS:
        for( i=icolors+1; i!=icolors; i++ )
        {
          if( i>2 ) i = 0;
          if( nib_samp.list[i] ) break;
        }
        if( i==icolors ) break;
        goto newcol;
      case IEDCSEL:
        if( (i=do_ipopup( IP2COL-1, IEDCSEL, icolors )) >= 0 )
        {
newcol:   if( !nib_samp.list[i] )
            if( _f_alert1( _msg_ptr[2] ) == 1 )
            { /* i is new rez, j is source rez */
              j = rez_copy(i);
              i_cic[i].num_planes = n2pl[i];
              if( new_rez( &i_cic[i], 0 ) )
              {
                if( j>=0 )
                {
                  free_rez(-1);	/* free before changing icolors! */
                  k = icolors;
                  icolors = j;
                  /* source has more colors: force to dithered mono */
                  cur_rez( 1, 0, 0, /*j>i ? 1 :*/ n2pl[i] );
                  if( ic_samp.num_planes!=n2pl[i] )	/* got dithered */
                    for( l=n2pl[i], iptr=i_cic[i].col_data; --l>=0; iptr+=DATASIZ/2 )
                      __bytecpy( iptr, ic_samp.col_data, DATASIZ );
                  else __bytecpy( i_cic[i].col_data, ic_samp.col_data, plane_size(n2pl[i]) );
                  __bytecpy( i_cic[i].col_mask, i_cic[j].col_mask, MASKSIZ );
                  if( j != icolors ) free_rez(-1);	/* free this sample. new one gets generated in goto_colors */
                  icolors = k;
                }
                else
                {
                  _memclr( i_cic[i].col_data, plane_size(n2pl[i]) );
                  _memclr( i_cic[i].col_mask, MASKSIZ );
                }
                nib_samp.list[i] = &i_cic[i];
                set_collist(0);
              }
              else break;
            }
            else break;
          goto_colors(i);
        }
        break;
      case IEDTSEL:
        if( (i=do_ipopup( IPFILE-1, IEDTSEL, fileatt )) >= 0 )
            pop_set( IEDTSEL, IPFILE, fileatt=i, 1 );
        break;
      case IEDPSEL:
      case IEDPIMG:
        if( ed_pat )
        {
          if( (i=do_ipopup( IEDDPAT0-1, IEDPSEL, -1 )) >= 0 )
          {
            iptr = ipopups[i+IEDDPAT0].ob_spec.bitblk->bi_pdata;
            for( i=0; i<16; i++ )
            {
              k = *iptr++;
              for( j=15; j>=0; j-- )
              {
                pixel[i][j] = k&1;
                k >>= 1;
              }
            }
            redo_sample();
            display();
          }
        }
        else if( (i=do_ipopup( IPSOLID-1, IEDPSEL, ipat-IPSOLID )) >= 0 )
        {
          pop_set( IEDPSEL, IPSOLID, i, 1 );
          ipat = i+IPSOLID;
          set_toggle();
        }
        break;
      case IEDDISP:
        if( iget_top() != f->handle ) break;
        last = -1;
        if( (i = get_mouse( &x, &y, &b, &mx, &my )) != 0 && (b&1 || !bounce) ) redo_prev();
        else break;
        bounce = 0;
        while( i )
        {
          lb = b;
          if( !(b&2) )
          {
            b = imode!=IPDATA ? 0x80 : 0;  /* just mask */
            if( imode!=IPDATA+1 ) b |= ed_pat ? 1 : ed_col-IEDCOL0;
          }
          else b = 0;
          if( cutmode )
          {
            if( new_rbox( x, y, &cutbox, 0 ) ) save_clip( cutbox.x, cutbox.y, cutbox.w,
                cutbox.h, cutmode==CUTB );
            cut_mode(0);
            i = 0;
          }
          else if( !move && idcml >= ICOPY && idcml != IEDLET )
          {
            move = new_rbox( x, y, &mbox, 1 );
            i = 0;
          }
          else if( move )
          {
            if( grab( &mx, &my, mbox.w, mbox.h, move ) )
            { 
              switch( move )
              {
                case 1:
                  if( i_samp.ib_xchar != mx || i_samp.ib_ychar != my )
                  {
                    xor_box( 0, mbox.x, mbox.y, 5, 5 );
                    i_samp.ib_xchar = mbox.x = mx;
                    i_samp.ib_ychar = mbox.y = my;
                    sample();
                    xor_box( 1, mx, my, 5, 5 );
                  }
                  else grf_mouse( POINT_HAND );
                  break;
                case 2:
                  if( mbox.x != mx || mbox.y != my )
                  {
                    xor_box( 0, mbox.x, mbox.y, mbox.w, mbox.h );
                    __bytecpy( tmppix, pixel, PIXELS );
                    data = ~pix_mask();
                    mask = data ? ~data : 0xff;
                    for( i=mbox.x, k=mx; i<mbox.x+mbox.w; i++, k++ )
                      for( j=mbox.y, l=my; j<mbox.y+mbox.h; j++, l++ )
                        if( k>=0 && k<x_max && l>=0 && l<y_max )
                            tmppix[l][k] = pixel[j][i]&mask | tmppix[l][k]&data;
                    if( idcml == IMOVE )
                      for( i=mbox.x; i<mbox.x+mbox.w; i++ )
                        for( j=mbox.y; j<mbox.y+mbox.h; j++ )
                          if( i<mx || i>=mx+mbox.w || j<my || j>=my+mbox.h )
                              tmppix[j][i] &= data;
                    __bytecpy( pixel, tmppix, PIXELS );
                    move = 0;
                    redo_sample();
                    display();
                  }
                  else grf_mouse( POINT_HAND );
              }
            }
            else if( idcml >= ICOPY && idcml != IEDLET )
            {
              xor_box( 0, mbox.x, mbox.y, mbox.w, mbox.h );
              move = new_rbox( x, y, &mbox, 1 );
            }
            i = 0;
          }
          else if( istyle==IFILL )
          {
            _lineaa();
            __bytecpy( tmppix, pixel, PIXELS );
            mask = pix_mask();
            i = (pixel[y][x]&0x7f ? 0 : b) | (pixel[y][x]&0x80 ? 0 : 0x80);
            seedfill( x, y, i&mask, pixel[y][x]&mask );
            for( i=0; i<x_max; i++ )
              for( j=0; j<y_max; j++ )
                if( tmppix[j][i] != pixel[j][i] ) set_pixel( i, j, b );
            redo_sample();
            waitbut();
            _linea9();
            i = 0;
          }
          else if( istyle==ILINE )
          {
            set_line( x, y, b );
            i = 0;
          }
          else if( istyle==IBOX || istyle==IFILBOX )
          {
            if( new_rbox( x, y, &mbox, 0 ) )
            {
              _lineaa();
              for( i=mbox.x; i<mbox.x+mbox.w; i++ )
                for( j=mbox.y; j<mbox.y+mbox.h; j++ )
                  if( istyle==IFILBOX || i==mbox.x || i==mbox.x+mbox.w-1 ||
                      j==mbox.y || j==mbox.y+mbox.h-1 ) set_pixel( i, j, b );
              redo_sample();
              _linea9();
            }
            i = 0;
          }
          else
          {
            set_pixel( x, y, b );
            while( (i = get_mouse( &nx, &ny, &b, &mx, &my )) != 0
                && b&1 && nx == x && ny == y || !i && b&1 );
            if( !(b&1) ) i = 0;
            if( !i || ed_pat ) redo_sample();
            x = nx;
            y = ny;
            if( (b&2) != (lb&2) ) last = -1;
          }
        }
        break;
      case IEDSEL:
        iselected = editor[IEDSEL].ob_state&SELECTED;
        if( iselected && !ic_samp.sel_data )
          if( _f_alert1( _msg_ptr[21] ) == 1 )
          {
            if( new_rez( &i_cic[icolors], 1 ) )
            {
              __bytecpy( i_cic[icolors].sel_data, i_cic[icolors].col_data,
                  plane_size(n2pl[icolors]) );
              __bytecpy( i_cic[icolors].sel_mask, i_cic[icolors].col_mask,
                  MASKSIZ );
              prev_ok = 0;
              set_collist(1);
              free_rez(1);	/* only free selected (should be none) */
              cur_rez( 1, 1, 1, -1 );	/* just redo selected */
              sample();
            }
          }
          else no_select();
        else change_sel();
        break;
      case IEDCOL0:
      default:
        if( imode != IPDATA+1 )
        {
          ed_color( &color, num-IEDCOL0, 1 );
          copy_color( o, num );
        }
        break;
    }
  return 0;
}

void neo_icons_nib(void)
{
  _z->programs[0].p.nib = &icon_nib;
}

static void update_wind( int wnum )
{
  (*_icneo->update_othwind)( wnum, 1 );
  neo_icons_nib();
}

int x_editor( OBJECT *o, int num, FORM *f )
{
  char temp[200];
  int k=0, i, j, l, s, **p, **q;
  CICON *ci;
  NICONBLK *ni;
  
    switch(num)
    {
      case IEDOK:
      case IEDPLUS:
        if( ed_pat )
        {
          __bytecpy( pptr->bi_pdata, new_pat, 32 );
          pptr->bi_color = new_pattern.bi_color;
        }
        k = 1;
      case -1:
      case IEDCANC:
        if( !ed_pat && k )
        {
          for( j=-1, i=0; i<3; i++ )
            if( nib_samp.list[i] && n2pl[i] <= graph->vplanes ) j = i;
          if( j!=icolors )
          {
            free_rez(-1);
            i = icolors;
            icolors = j;
            cur_rez( 1, -1, 0, -1 );	/* regenerate fixed icon at new rez */
            icolors = i;
          }
          if( (ci = (ni=edit_file->nib)->ci) == 0L )
            if( (ci = lalloc( sizeof(CICON), -1 )) != 0 )
                ni->ci = ci;
            else return 0;
          else	/* free transformed icon being used, except for matches with list[] */
            for( i=3; --i>=0; )
              if( ni->list[i] && ni->list[i]->num_planes==ci->num_planes )
              {
                free_icon( &ci, ni->list[i], 0 );
                break;
              }
          ci->num_planes = n2pl[j];
          ci->col_mask = ic_samp.col_mask;
          ci->sel_mask = ic_samp.sel_mask;
          ic_samp.col_mask = ic_samp.sel_mask = 0L;  /* to prevent lfree */
          ci->col_data = ic_samp.col_data;
          ci->sel_data = ic_samp.sel_data;
          ic_samp.col_data = ic_samp.sel_data = 0L;  /* to prevent lfree */
          for( i=0; i<3; i++ )
            if( nib_samp.list[i] )
            {
              if( !ni->list[i] )
              {
                if( (ni->list[i]=lalloc( sizeof(CICON), -1 )) == 0 ) return 0;
                _memclr( ni->list[i], sizeof(CICON) );
                ni->list[i]->num_planes = n2pl[i];
              }
              for( p=&ni->list[i]->col_data, q=&i_cic[i].col_data, l=0; l<4; l++, p++, q++ )
              {
                s = plane_size(l&1 ? 1 : n2pl[i]);
                if( *p && *q ) __bytecpy( *p, *q, s );
                else if( *q )
                {
                  *p = *q;
                  *q = 0L;
                }
                else cmfree( (char **)p );
              }
            }
            else free_icon( &ni->list[i], ci, 1 );
          for( p=&ci->col_data, q=&i_cic[j].col_data, i=4; --i>=0; p++, q++ )
            if( *p==*q ) *q = 0L;	/* to prevent cmfree */
          i_ptr0->ib_xchar = i_samp.ib_xchar;
          i_ptr0->ib_ychar = i_samp.ib_ychar;
          i_ptr0->ib_char = (i_samp.ib_char&0xFF00)|fix_att(fileatt,0);
          if( ed_obj )
          {
            _from_filename( editor[IEDTEMPL].ob_spec.tedinfo->te_ptext, temp, 1 );
            if( strcmp( temp, edit_file->name ) )
            {
              strcpy( edit_file->nib->ib->ib_ptext, 
                  editor[IEDTEMPL].ob_spec.tedinfo->te_ptext );
              _pad_it( edit_file->nib->ib->ib_ptext );
              strcpy( edit_file->name, temp );
            }
          }
        }
        if( ed_pat )
        {
          if(k)
          {
            add_patt( ed_wnum, pptr->bi_pdata, pptr->bi_color );
            if( edit_file && edit_file->nib==patt_nib[0] ) memcpy(
                (*_icneo->deskpat)->ob_spec.bitblk->bi_pdata,
                pptr->bi_pdata, 32 );
            update_wind( ed_wnum );
          }
          _icic.pattern = 0L;
          *(Rect *)&editor[IEDDESK].ob_x = old;
          editor[IEDDIC1].ob_flags = editor[IEDDIC1+1].ob_flags &= ~HIDETREE;
          imode = om;
        }
        else
        {
          free_cic();
          /* get now before update because it might change */
          i = edit_file-_z->file[ed_wnum];
          if(k) update_wind( ed_wnum );
        }
        editor[IEDTEMPL].ob_flags &= ~HIDETREE;
        editor[IEDTEMPL].ob_flags |= EDITABLE;
        if( num==IEDPLUS && ++i < _z->num_files[ed_wnum] )
        {
          cut_mode(0);
          /* _z->file might have already changed */
          edit_file = _z->file[ed_wnum] + i;
          if( ed_obj ) wind_set( f->handle, X_WF_DIALEDIT, 0, 0 );
          if( i_editor( o, f ) )
          {
            ed_draw(0);
            if( ed_obj ) wind_set( f->handle, X_WF_DIALEDIT, IEDTEMPL, -1 );
            return 0;
          }
        }
    }
  cut_mode(0);
  forms[0].tree = 0L;	/* so it will get re-initialized next time (root size may change) */
  _arrow();
  return 1;
}

int i_iabout( OBJECT *o, FORM *f )
{
  if( !o )
  {
    rsrc_adr( IABOUTTX, &o );
    f->tree = o;
    /* 003: used to set longedit for this tree by mistake! */
  }
  return 1;
}

int tx_iabout( OBJECT *o, int num, FORM *f )
{
  return 1;
}

static char nic_old, *nic_ptr, nic_name[120], nic_buf[7], *nic_end;
static int nic_wnum, nic_hand;
static int (*nicfunc)( int ret, int wnum );

#define NIC_TEMP	((char *)f->mem_ptr)

int i_nic_info( OBJECT *obj, FORM *f )
{
  char *ptr, *ptr2, *ptr3;
  int time[6];

  if( !obj )
  {  
    rsrc_adr( NICINFO, &obj );
    (*_icneo->set_longedit)( obj, NICCOMM1, 3 );	/* 003: moved here */
    f->tree = obj;
    return 1;
  }
  if( *_icneo->in_showinf ) *_icneo->show_ret = 0;
  if( !nic_old )
  {
    obj[NICCREAT].ob_spec.free_string[0] = obj[NICMOD].ob_spec.free_string[0] =
        obj[NICAUTH].ob_spec.tedinfo->te_ptext[0] = 
        obj[NICCOMM1].ob_spec.tedinfo->te_ptext[0] = 
        obj[NICCOMM1+1].ob_spec.tedinfo->te_ptext[0] = 
        obj[NICCOMM1+2].ob_spec.tedinfo->te_ptext[0] = 
        obj[NICNAME].ob_spec.tedinfo->te_ptext[0] = '\0';
    obj[NICAUTH].ob_flags |= EDITABLE;
    icentries = D_PROG;
    ptr = "";
  }
  else
  {
    (*_icneo->to_tandd)( iccreate, time );
    (*_icneo->tandd_to_str)( time, obj[NICCREAT].ob_spec.free_string );
    (*_icneo->to_tandd)( icmodify, time );
    (*_icneo->tandd_to_str)( time, obj[NICMOD].ob_spec.free_string );
    strcpy( obj[NICAUTH].ob_spec.tedinfo->te_ptext, icauth );
    strcpy( obj[NICCOMM1].ob_spec.tedinfo->te_ptext, iccomment );
    strcpy( obj[NICCOMM1+1].ob_spec.tedinfo->te_ptext, iccomment+36 );
    strcpy( obj[NICCOMM1+2].ob_spec.tedinfo->te_ptext, iccomment+72 );
    obj[NICAUTH].ob_flags &= ~EDITABLE;
    ptr = nic_end = spathend( nic_ptr ? nic_ptr : _z->w[nic_wnum].path );
    ptr2 = obj[NICNAME].ob_spec.tedinfo->te_ptext;
    for( ptr3=NIC_TEMP; *ptr != '.' && *ptr; )
      *ptr2++ = *ptr3++ = *ptr++;
    *ptr2 = *ptr3 = '\0';
  }
  _spf( obj[NICICONS].ob_spec.free_string, "%D", (unsigned long)icentries );
  return 1;
}

int x_nic_info( OBJECT *obj, int num, FORM *f )
{
  char *ptr;
  
  if( *_icneo->in_showinf ) *_icneo->show_ret = 1;
  if( num == NICOK )
  {
    ptr = obj[NICNAME].ob_spec.tedinfo->te_ptext;
    if( !*ptr ) _f_alert1( _msg_ptr[31] );
    else if( !nic_old )
    {
      f->flags.close_on_update = 0;
      create_nic( 1, nic_wnum );
/*%      (*_icneo->update_wnum)( nic_wnum, _z->w[nic_wnum].path );  already in create */
      f->flags.close_on_update = 1;
    }
    else
    {
      obj[NICAUTH].ob_flags |= EDITABLE;
      strcpy( iccomment, obj[NICCOMM1].ob_spec.tedinfo->te_ptext );
      strcpy( iccomment+36, obj[NICCOMM1+1].ob_spec.tedinfo->te_ptext );
      strcpy( iccomment+72, obj[NICCOMM1+2].ob_spec.tedinfo->te_ptext );
      if( strcmp( NIC_TEMP, ptr ) )
      {
        strcpy( nic_end, ptr );
        strcat( nic_end, ".NIC" );
        if( *_icneo->in_showinf )
        {
          *_icneo->show_ret = nicfunc ? (*nicfunc)( 2, nic_wnum ) : 2;
          *_icneo->showinf_ok = -1;
        }
        return 1;
      }
      if( *_icneo->in_showinf )
      {
        if( nicfunc ) *_icneo->show_ret = (*nicfunc)( 1, nic_wnum );
        *_icneo->showinf_ok = -1;
      }
      return 1;
    }
  }
  if( nicfunc ) (*nicfunc)( 0, nic_wnum );
  /* caution: f might have moved now! */
  if( *_icneo->in_showinf ) *_icneo->showinf_ok = -1;
  return 1;
}

static int nic_info( int old, int wnum, char *str, int func( int ret, int wnum ) )
{
#ifndef DEMO
  int ret;
  
  nic_old = old;
  nic_ptr = str;
  nic_wnum = wnum;
  nicfunc = func;
  if( (ret=(*gui->start_form)( *_icneo->AES_handle, NAME|MOVER|CLOSER, &forms[2] )) < 0 )
      return -1;
  else if( ret ) return 1;
#else DEMO
  _f_alert1( _msg_ptr[IED_MSGS-1] );
#endif
  return(0);
}

int nicfunc1( int ret, int wnum )
{
  int w = ewind_type[wnum];

  if( ret==2 )
  {
    cksum[w]++;
/*%      (*_icneo->redraw_obj)( _z->maximum, WMOVE ); */
    return 2;
  }
  __bytecpy( &nicinfo[wnum], &iccreate, sizeof(NIC_INFO) );
  return 1;
}

int nicfunc2( int ret, int wnum )
{
  if( ret )
  {
    key = KEY_START;
    encrypt( &iccreate, &iccreate, sizeof(nicinfo[0]) );
    Fseek( (unsigned)nic_buf[6]+7+2, nic_hand, 0 );
    Fwrite( nic_hand, sizeof(nicinfo[0]), &iccreate );
    Fclose(nic_hand);
    (*_icneo->set_filename)( -1, -1, nic_name, nic_ptr );
    return 2;
  }
  Fclose(nic_hand);
  return 1;
}

int total_icons( int w )
{
  return *num_icons[w] + (patt_nib[w]!=0);
}

int nic_showinf( int wnum, char *file )
/* return:  -2: not NIC  -1: opened window  0: error  1: ok  2: update */
{
#ifndef DEMO
  int i;
  unsigned int codelen;
  char *p;
  
  if( wnum>=0 )
  {
    if( nt_nic(wnum) ) return -2;
    __bytecpy( &iccreate, &nicinfo[wnum], sizeof(nicinfo[0]) );
    icentries = total_icons(ewind_type[wnum]);
    if( !nic_info( 1, wnum, 0L, nicfunc1 ) ) return 0;
  }
  else if( file/*003*/ && (p=strrchr(file,'.'))!=0/*003*/ &&
      !strcmp( p, ".NIC" ) && (*_nac->TOS_error)( nic_hand=Fopen(file,2), 0 ) )
  {
    if( Fread(nic_hand,7L,nic_buf)==7L && *(long *)nic_buf==0x2E4E4943 && 
        nic_buf[2] <= *(((int *)&icheader)+2) && Fseek( (unsigned)nic_buf[6],
        nic_hand, 1 ) == (unsigned)nic_buf[6]+7 &&
        (*_icneo->read_header)( nic_hand, &icentries, &iccreate, &codelen ) )
    {
      strcpy( nic_name, file );
      if( (i=nic_info( 1, wnum, file, nicfunc2 )) == 0 )
      {
        Fclose(nic_hand);
        return 0;
      }
      if( i<0 ) return -1;
      return 1;
    }
    Fclose(nic_hand);
    return 0;
  }
#endif DEMO
  return -2;
}

#pragma warn +par

void edit_desk(void)
{
  edit_icon( 0L, -1 );
}

int _ed_wind_type( int num )
{
  return( num>=0 ? ewind_type[num] : 0 );
}

OBJECT *icon_menu( int wnum )
{
  if( ewind_type[wnum]>=EDW_ICONS ) return fil_menu;
  return win_menu;
}

int open_iwind( int wind, int type )
{
  int i;
  ICONBUF *icb;
  char *ptr;
  
  not_nic[wind] = (ptr=strrchr(_z->w[wind].path,'.'))==NULL || 
      strcmp(ptr,".NIC");
/*%  _z->use_temp[wind] = (1<<NUM_TEMPL)-1; */
  if( (ewind_type[wind] = type==EDW_FILE ? wind+EDW_FILE : type) >= EDW_FILE )
  {
    ptr = _z->w[wind].path;
    for( i=0; i<7; i++ )
      if( i != wind && _z->w[i].place && ewind_type[i]>=EDW_FILE &&
          !strcmp(ptr,_z->w[i].path) )
      {
        _f_alert1( _msg_ptr[8] );
        return(0);
      }
/*%    for( i=0, icb=(ICONBUF *)((long)memory[0]+65536L-neo_rsc->rsh_rssize);
        icb<(ICONBUF *)((long)memory[0]+mem_all) && !i; )
      if( !*(ICONBUF **)icb ) i++;
      else icb++;
    if( !i )
    {
      no_memory();
      return(0);
    }
    *(ICONBUF **)(memory[i=ewind_type[wind]] = icb) = (ICONBUF *)-1L; */
    if( !read_icons( wind, 0L ) )
    {
/*%      *(ICONBUF **)memory[i] = 0L;
      memory[i] = 0L; */
      return(0);
    }
    cksum[ewind_type[wind]] = checksum( wind, 0 );
  }
  return(1);
}

void del_last_ic( int wnum )
{
  delete_icon( wnum, (*num_icons[_ed_wind_type(wnum)])-1 );
}

static ICONBUF *add_icon( int wnum )
{
  ICONBUF *icb;
  ICONBLK *icon;
  int w;
  OBJECT *obj;

  w = _ed_wind_type(wnum);
  if( (icb=(*_icneo->add_icon)( iconbuf[w], num_icons[w], icons_rem[w] )) == 0 )
  {
    _f_alert1( _msg_ptr[10] );
    return 0L;
  }
  rsrc_adr( IICON, &obj );
  __bytecpy( &icb->icb, obj[2].ob_spec.iconblk, sizeof(ICONBLK) );
  if( !(*_icneo->alloc_im)( &icb->icb.ib_pdata, 1 ) ||
      !(*_icneo->alloc_im)( &icb->icb.ib_pmask, 1 ) )
  {
    del_last_ic( wnum );
    return 0;
  }
  __bytecpy( icb->icb.ib_pdata, obj[2].ob_spec.iconblk->ib_pdata, DATASIZ );
  __bytecpy( icb->icb.ib_pmask, obj[2].ob_spec.iconblk->ib_pmask, MASKSIZ );
  strcpy( icb->icb.ib_ptext=icb->text, "           " );
  icb->icb.ib_wtext = 13*6;
  return icb;
}

void free_patt( int w )
{
  NICONBLK *n;
  
  if( (n=patt_nib[w])!=0 )
  {
    if( n->ib )
    {
      cmfree( (char **)&n->ib->ib_pdata );
      cmfree( (char **)&n->ib->ib_pmask );
      cmfree( (char **)&n->ib );
    }
    cmfree( (char **)&patt_nib[w] );
  }
}

int add_patt( int wnum, int *data, int col )
{
  int w, i, *m, *d;
  OBJECT *obj;
  NICONBLK *n;
  ICONBLK *ib;
  
  w = _ed_wind_type(wnum);
  if( (n=patt_nib[w])==0 )
  {
    if( (n = lalloc( sizeof(NICONBLK), -1 )) == 0 ) return 0;
    _memclr( n, sizeof(NICONBLK) );
    if( (ib = lalloc( sizeof(ICONBLK), -1 )) == 0 )
    {
      free_patt( w );
      return 0;
    }
    _memclr( n->ib=ib, sizeof(ICONBLK) );
    if( !(*_icneo->alloc_im)( &ib->ib_pdata, 1 ) ||
        !(*_icneo->alloc_im)( &ib->ib_pmask, 1 ) )
    {
      free_patt( w );
      return 0;
    }
    patt_nib[w] = n;
    rsrc_adr( IICON, &obj );
    __bytecpy( &n->ib->ib_char, &obj[2].ob_spec.iconblk->ib_char, sizeof(ICONBLK)-12 );
    n->ib->ib_ptext = _msg_ptr[39];
  }
  for( i=16, d=n->ib->ib_pdata, m=n->ib->ib_pmask; --i>=0; d+=2, m+=2 )
  {
    *d = *(d+1) = *(d+32) = *(d+33) = *data;
    *m = *(m+1) = *(m+32) = *(m+33) = ~*data++;
  }
  n->ib->ib_char = (col<<12) | ((col&0xf0)<<4);
  return 1;
}

int del_conf(void)
{
  return( _f_alert1( _msg_ptr[11] ) == 1 );
}

int delete_icon( int wnum, int ind )    /* multiples must be done in reverse */
{
  register int i, w, j;
  FSTRUCT file1;
  NICONBLK *n;
  
  w = ewind_type[wnum];
  get_icon( wnum, ind, &file1 );
  if( file1.type.p.pexec_mode < D_PROG )
  {
    _f_alert1( _msg_ptr[12] );
    return(0);
  }
  n = file1.nib;
  if( w>0  )
  {
    if( n->list[0] && n->list[0]->col_data != n->ib->ib_pdata )
        cmfree( (char **)&n->ib->ib_pdata );
    if( n->list[0] && n->list[0]->col_mask != n->ib->ib_pmask )
        cmfree( (char **)&n->ib->ib_pmask );
  }
  free_nib_icon(n);
  if( patt_nib[w] ) ind--;
  __bytecpy( n, ((ICONBUF *)n+1), (--(*num_icons[w])-ind)*sizeof(ICONBUF) );
  (*_icneo->reset_icbs)( *iconbuf[w], ind, *num_icons[w] );
  ++(*icons_rem[w]);
  return(1);
}

char *dflt_icname( int ind, int t )
{
  if( patt_nib[t] )
    if( !ind ) return _msg_ptr[39];
    else ind--;
  return (*(_icneo->icons))[ind+1/* skip root */].ob_spec.iconblk->ib_ptext;
}

int get_icon( int wnum, int ind, FSTRUCT *file )
{
  int i, t;
  ICONBUF *icb;
  char *p;
  
  if( wnum<0 || (t = ewind_type[wnum])==EDW_ICONS )
    if( ind < total_icons(0) )
    {
      if( ind )
      {
        file->nib = &(*_icneo->nic_icons)[ind-1].nicb;
        file->type.p.pexec_mode = ind>=D_PROG ? NOT_DFLT : ind-1;
      }
      else
      {
        file->nib = patt_nib[0];
        file->type.p.pexec_mode = NPI;
      }
      if( ind>=D_PROG )		/* 003 */
      {
        p = (*_icneo->icon_text) + (ind-D_PROG)*13;
        if( file->nib->ib->ib_char&ICON_DRIVE )
        {
          strcpy( file->name, drive->te_ptmplt );
          *strchr( file->name, '_' ) = *p;
        }
        else strcpy( file->name, p );
      }
      else strcpy( file->name, dflt_icname( ind, 0 ) );
      strcpy( file->groupname, file->name );
/*%    if( ind==NPI ) ind++;*/
      file->read_only = ind<D_PROG ? 2 : 0;
      file->state = 0;
      return(1);
    }
    else return(0);
  else if( ind<total_icons(t) )
    {
      if( patt_nib[t] && !ind )
      {
        file->type.p.pexec_mode = NPI;
        file->nib = patt_nib[t];
        strcpy( file->name, _msg_ptr[39] );
        strcpy( file->groupname, file->name );
        file->read_only = 2;
      }
      else
      {
        if( patt_nib[t] ) ind--;
        icb = &(*iconbuf[t])[ind];
        file->nib=&icb->nicb;
        if( (i=file->type.p.pexec_mode=icb->type)>=D_PROG )
        {
          if( !not_nic[wnum] ) _from_filename( icb->text, file->name, 1 );
          else strcpy( file->name, icb->text );
          file->read_only = 0;
        }
        else	/* default icon in NIC window */
        {
          if( patt_nib[t] ) i++;
          strcpy( file->name, dflt_icname( i, t ) );
          strcpy( file->groupname, file->name );
          file->read_only = 2;
        }
      }
      file->state = 0;
      return(1);
    }
  return(0);
}

/*%void update_icons(void)
{
  char s[2];
  
  s[0] = ICON_LET;
  s[1] = '\0';
  (*_nac->update_drive)(s);
}*/

int fhand, tmp_hand;
void _fwrite( int *hand, long l, void *buf )
{
  long w;
  
  if( *hand>0 && l>0 )
    if( (w=Fwrite( *hand, l, buf )) != l )
    {
      Fclose(*hand);
      *hand = -1;
      if( w >= 0 ) _f_alert1( _msg_ptr[13] );
      else (*_nac->TOS_error)( w, 0 );
    }
}

/********************
long str_pos, obj_pos, icb_pos, icon_pos;

void dec_rsc( long *l )
{
  long ll;
  
  if( *l >= (long)neo_rsc ) *l -= (long)neo_rsc;
  ll = *l;
  if( ll >= str_pos ) *l += num_new[0]*14;	/* INTRFACE */
  if( ll >= obj_pos ) *l += num_new[0]*sizeof(OBJECT);
  if( ll >= icb_pos ) *l += num_new[0]*sizeof(ICONBLK);
  if( ll >= icon_pos ) *l += num_new[0]*(MASKSIZ+DATASIZ);
}

int seek_read( int hand, long size, long pos )
{
  return( Fseek( pos, hand, 0 ) != pos || Fread( hand, size, 
      (char *)neo_rsc+pos ) != size );
}

int save_rsc(void)
{
#ifndef DEMO
  int i;
  unsigned int j, k, *iptr;
  char path1[120], path2[120], *ptr;
  OBJECT *obj;
  ICONBLK *icb;
  long pos, old_size, l, off, off2;

  old_size = neo_rsc->rsh_rssize;
  strcpy( path2, _z->dflt_path );
  strcat( path2, "NEODESK.RSC" );
  if( path2[0] <= 'B' )
  {
    _spf( path1, _msg_ptr[14], path2[0] );
    if( _f_alert1( path1 ) == 2 ) return(0);
  }
  ptr = (*_icneo->icons)[D_PROG].ob_spec.iconblk->ib_ptext; 
  j = (long)ptr + 13 /**strlen(ptr) + 1**/ - (long)neo_rsc;   /* up to D_PROG string */ /* INTRFACE */
  k = j+(*num_icons[0]-D_PROG)*14; /* INTRFACE *//* last icon string */
retry:
  kick(path2);
  if( (i=Fopen( path2, 2 )) < 0 || Fread( i, sizeof(RSHDR), neo_rsc ) != sizeof(RSHDR) ||
      neo_rsc->rsh_rssize != old_size ||
      seek_read( i, j-neo_rsc->rsh_string, neo_rsc->rsh_string ) ||   /* strings up to D_PROG */
      seek_read( i, neo_rsc->rsh_imdata-k, k ) ||  /* and after last icon */
      seek_read( i, neo_rsc->rsh_nted*sizeof(TEDINFO), neo_rsc->rsh_tedinfo ) ||
      seek_read( i, neo_rsc->rsh_nobs*sizeof(OBJECT), neo_rsc->rsh_object ) )
  {
    if( i>0 ) Fclose(i);
    switch( _f_alert1( _msg_ptr[15] ) )
    {
      case 1:
        goto retry;
      case 2:
        return(1);
      case 3:
        return(0);
    }
  }
  if( i>0 ) Fclose(i);
  for( i=1; i<=D_PROG; i++ )
    ((ICONBLK *)((long)(*_icneo->icons)[i].ob_spec.iconblk+(long)neo_rsc))->
        ib_char &= 0xFF00;
  for( ; i<=*num_icons[0]; i++ )
    _pad_it( ((ICONBLK *)((long)
        (*_icneo->icons)[i].ob_spec.iconblk+(long)neo_rsc))->ib_ptext );
  obj = &(*_icneo->icons)[*num_icons[0]];
  icb = (ICONBLK *)((long)obj->ob_spec.iconblk+(long)neo_rsc);
  str_pos = (long)icb->ib_ptext+14 /*strlen(icb->ib_ptext)+1*/-(long)neo_rsc; /* INTRFACE */
  obj_pos = (long)obj+sizeof(OBJECT)-(long)neo_rsc;
  icb_pos = (long)icb+sizeof(ICONBLK)-(long)neo_rsc;
  icon_pos = (long)icb->ib_pmask+DATASIZ+MASKSIZ-(long)neo_rsc;
  fix_rsc( neo_rsc, (char *)neo_rsc+sizeof(RSHDR), dec_rsc );
  neo_rsc->rsh_nobs += num_new[0];
  neo_rsc->rsh_nib += num_new[0];
  for( iptr=(unsigned int *)&neo_rsc->rsh_object; 
      iptr<=(unsigned int *)&neo_rsc->rsh_trindex; )
  {
    l = *iptr;
    dec_rsc(&l);
    *iptr++ = l;
  }
  neo_rsc->rsh_rssize += num_new[0]*(sizeof(ICONBUF)+2); /* INTRFACE */
  if( num_new[0] < 0 )
  {
    str_pos += 14*num_new[0];	/* INTRFACE */
    obj_pos += sizeof(OBJECT)*num_new[0];
    icb_pos += sizeof(ICONBLK)*num_new[0];
    icon_pos += (DATASIZ+MASKSIZ)*num_new[0];
  }
  strcpy( path1, _z->dflt_path );
  strcat( path1, "NEODESK$.RSC" );
  fhand = Fcreate( path1, 0 );
  /* write header and unaffected part of file in next _fwrite() */
  pos=0L;

  /* fixup OBJECTs */
  obj = (OBJECT *)(obj_pos - sizeof(OBJECT) + (long)neo_rsc);
  if( num_new[0] > 0 )
  {
    obj->ob_next = *num_icons[0]+1;
    obj->ob_flags &= ~LASTOB;
  }
  else
  {
    obj->ob_next = 0;
    obj->ob_flags |= LASTOB;
  }
  (*_icneo->icons)[0].ob_tail += num_new[0];
  /* write up to my first OBJECT */
  _fwrite( obj_pos-pos, (char *)neo_rsc+pos );
  /* write new OBJECTs */
  pos = obj_pos;
  off = icb_pos+num_new[0]*sizeof(OBJECT);
  if( num_new[0] > 0 )
    for( i=0; i<num_new[0]; i++ )
    {
      __bytecpy( &memory[0][i].obj, obj, sizeof(OBJECT) );
      memory[0][i].obj.ob_next = i==num_new[0]-1 ? 0 : i+*num_icons[0]+2;
      (long)memory[0][i].obj.ob_spec.iconblk = off+i*sizeof(ICONBLK);
      if( i==num_new[0]-1 ) memory[0][i].obj.ob_flags |= LASTOB;
      else memory[0][i].obj.ob_flags &= ~LASTOB;
      memory[0][i].obj.ob_head = memory[0][i].obj.ob_tail = -1;
      _fwrite( sizeof(OBJECT), &memory[0][i].obj );
    }
  else pos -= num_new[0]*sizeof(OBJECT);

  /* write up to ICONBLKs */
  _fwrite( icb_pos-pos, (char *)neo_rsc+pos );
  /* write new ICONBLKs */
  pos = icb_pos;
  obj = (OBJECT *)(obj_pos - sizeof(OBJECT) + (long)neo_rsc);
  off = str_pos + num_new[0]*(sizeof(ICONBLK)+sizeof(OBJECT));
  off2 = icon_pos + num_new[0]*(sizeof(ICONBLK)+sizeof(OBJECT)+14); /* INTRFACE */
  if( num_new[0] > 0 )
    for( i=0; i<num_new[0]; i++ )
    {
/*      __bytecpy( &memory[0][i].obj, obj, sizeof(OBJECT) );
      (long)memory[0][i].obj.ob_spec.iconblk = pos+i*sizeof(ICONBLK)+off;*/
      (long)memory[0][i].nicb.ib->ib_ptext = 14*i+off;  /* INTRFACE */
      (long)memory[0][i].nicb.ib->ib_pmask = (DATASIZ+MASKSIZ)*i+off2;
      (long)memory[0][i].nicb.ib->ib_pdata = (DATASIZ+MASKSIZ)*i+off2+MASKSIZ;
      _fwrite( sizeof(ICONBLK), memory[0][i].nicb.ib );
    }
  else pos -= num_new[0]*sizeof(ICONBLK);

  /* write up to icon strings */
  _fwrite( str_pos-pos, (char *)neo_rsc+pos );
  /* write icon strings */
  pos = str_pos;
  if( num_new[0] > 0 )
    for( i=0; i<num_new[0]; i++ )
    {
      _pad_it(memory[0][i].text);
      _fwrite( 12, memory[0][i].text );
      _fwrite( 2, "\0" );	/* INTRFACE */
/*      (long)memory[0][i].icb.ib_ptext = pos+12*i+off;*/
    }
  else pos -= num_new[0]*14;	/* INTRFACE */

  /* write up to icon data */
  _fwrite( icon_pos-pos, (char *)neo_rsc+pos );
  /* write new icon data */
  pos = icon_pos;
  if( num_new[0] > 0 )
    for( i=0; i<num_new[0]; i++ )
    {
      _fwrite( MASKSIZ, memory[0][i].mask );
      _fwrite( DATASIZ, memory[0][i].data );
/*      (long)memory[0][i].icb.ib_pmask = pos+(DATASIZ+MASKSIZ)*i+off;
        (long)memory[0][i].icb.ib_pdata = pos+(DATASIZ+MASKSIZ)*i+off+MASKSIZ; */
    }
  else pos -= num_new[0]*(DATASIZ+MASKSIZ);

  /* write to end of file */
  _fwrite( old_size-pos, (char *)neo_rsc+pos );
  if( fhand<0 ) return(0);
  Fclose(fhand);
  if( Fdelete(path2) || Frename( 0, path1, path2 ) )
  {
    Fdelete(path1);
    _f_alert1( _msg_ptr[16] );
    return(0);
  }
#else DEMO
  _f_alert1( _msg_ptr[IED_MSGS-2] );
#endif DEMO
  return(1);
}
***************************/

int nread( int f_hand, void *pos, int count )
{
  return( Fread( f_hand, (long)count, pos ) != (long)count );
}

int iread( int f_hand, int i, int old_fmt, int wnum )
{
  register int err, j, *iptr1, *iptr2;
  char buf[12], *ptr;
  ICONBUF *ib;
  
  if( i==CLIPBRD || i==GROUP-1/*004*/ )
  {
    if( (ib = add_icon(wnum)) == 0 ) return(1);
    ib->type = i==CLIPBRD ? CLIPBRD : GROUP;
  }
  if( (ib = add_icon(wnum)) == 0 ) return(1);
  Fseek( (old_fmt ? 232 : 244)*(long)i, f_hand, 0 );
  /* this centers 28*32 within 32*32 */
  if( (err = nread( f_hand, (iptr1=ib->icb.ib_pdata)+4, 112 )) == 0 )
      err = nread( f_hand, (iptr2=ib->icb.ib_pmask)+4, 112 );
  *(long *)iptr1 = *((long *)iptr1+1) = *(long *)iptr2 = *((long *)iptr2+1) =
      *((long *)iptr1+30) = *((long *)iptr2+30) = *((long *)iptr1+31) =
      *((long *)iptr2+31) = 0;
  ptr=ib->icb.ib_ptext;
  if( !err && !old_fmt ) err = nread( f_hand, i<9 ? buf : ptr, 12 );
  if( !err )
  {
    ib->type = (i+=(i>=CLIPBRD)+(i>=GROUP/*004*/)) >= D_PROG-2 ? NOT_DFLT : i;
    if( i>8 )
    {
      j = strlen(ptr);
      ptr += j;
      for( ; j<11; j++ )
        *ptr++ = ' ';
      *ptr = '\0';
    }
    err = nread( f_hand, &(ib->icb.ib_xchar), 8 );
  }
  return(err);
}

#define IMG_HAND -99
char *img_ptr, *img_start;
long img_count;

void img_open( char *buf, long count )
{
  img_ptr = img_start = buf;
  img_count = count;
}

long iFseek( long offset, int handle, int seekmode )
{
  if( handle==IMG_HAND )
  {
    switch( seekmode )
    {
      case 1:
        offset += img_ptr-img_start;
        break;
      case 2:
        offset = img_count;
    }
    if( offset > img_count ) offset = img_count;
    img_ptr = img_start+offset;
    return offset;
  }
  return Fseek( offset, handle, seekmode );
}

long iFread( int handle, long count, void *buf )
{
  if( handle==IMG_HAND )
  {
    if( img_ptr+count > img_start+img_count ) count = img_start+img_count-img_ptr;
    __bytecpy( buf, img_ptr, count );
    img_ptr += count;
    return count;
  }
  return Fread( handle, count, buf );
}

void read_dm( int hand, int h, int skip_f, int skip_m, int *ptr )
{
  int j;
  
  if( h>ICON_H )
  {
    j = h-ICON_H;
    h = ICON_H;
  }
  else j = 0;
  /* skip_f and skip_m are number of words to skip */
  if( !skip_f && !skip_m ) iFread( hand, h<<2, ptr );
  else
    while(h--)
      if( skip_f )
      {
        iFread( hand, 4L, ptr );
        iFseek( skip_f<<1, hand, 1 );
        ptr += 2;
      }
      else
      {
/*      iFread( hand, 4L-(skip_m<<1), ptr );
        ptr += 2-skip_m;
        while( skip_m-- ) *ptr++ = 0; */
        iFread( hand, 4L-2, ptr++ );
        *ptr++ = 0;
      }
  if(j) iFseek( j*(skip_m ? 2 : (skip_f<<1)+4), hand, 1 );
}

void read_impl( int h, int w, long data, long mask, int *dptr, int *mptr, int f_hand )
{
  int j, skip_f, skip_m;

  j = h >= ICON_H ? ICON_H : h;
  skip_f = w > ICON_W ? (w-ICON_W+15)>>4 : 0;
  skip_m = w < (ICON_W-15) ? 1 : 0;
  if( data!=-1L ) iFseek( data, f_hand, 0 );
  read_dm( f_hand, h, skip_f, skip_m, dptr );
  if( mask )
  {
    if( mask!=-1L ) iFseek( mask, f_hand, 0 );
    read_dm( f_hand, h, skip_f, skip_m, mptr );
  }
  else if( mptr ) _memclr( mptr, MASKSIZ );
  j <<= 1;
  while( j<(ICON_H<<1) )
  {
    dptr[j] = 0;
    if( mptr ) mptr[j] = 0;
    j++;
  }
}

void read_img( int h, int w, long data, long mask, ICONBUF *ib, int f_hand )
{
  read_impl( h, w, data, mask, ib->icb.ib_pdata, ib->icb.ib_pmask, f_hand );
  ib->icb.ib_char &= 0xFF00;
  ib->type = NOT_DFLT;
}

int xlong_rsc( RSHDR2 *rsc )
{
  return rsc->rsh_extvrsn == X_LONGRSC;
}

void fix_iconstr( char *str, ICONBUF *ib )
{
  char *s;

  str = spathend(str);
  s = ib->icb.ib_ptext;
  if( strchr(str,'.') )
  {
    str[12] = '\0';
    (*_icneo->to_filename)( str, s );
  }
  else
  {
    str[11] = '\0';
    strcpy( s, str );
  }
}

void read_cimg( int h, int w, int pl, int hand, int *iptr )
{
  long next = DATASIZ;

  while( --pl >= 0 )
  {
    read_impl( h, w, -1L, 0L, iptr, 0L, hand );
    (char *)iptr += next;
  }
}

#define RSC_ELEM(e) (is_new ? rsh.##e : ((RSHDR *)&rsh)->##e)

int read_rsc_i( char *name, int f_hand, int wnum )
{
  int f_hand2, err;
  unsigned int i;
  long l, ncic, temp, ii;
  RSHDR2 rsh;
  ICONBLK icb;
  BITBLK bb;
  char str[12], is_new;
  ICONBUF *ib;
  CICON cic, *new;
  
  if( (*_nac->TOS_error)(f_hand2=Fopen(name,0),0) && Fread( f_hand, 
      sizeof(RSHDR2), &rsh ) == sizeof(RSHDR2) )
  {
    is_new = xlong_rsc(&rsh);
    l = RSC_ELEM(rsh_iconblk);
    if( Fseek( l, f_hand, 0 ) == l )
  {
    for( i=err=0; i<RSC_ELEM(rsh_nib) && !err; i++ )
      if( Fread( f_hand, sizeof(ICONBLK), &icb ) == sizeof(ICONBLK) &&
          (ib=add_icon(wnum)) != 0 )
      {
        read_img( icb.ib_hicon, icb.ib_wicon, (long)icb.ib_pdata,
            (long)icb.ib_pmask, ib, f_hand2 );
        ib->icb.ib_xchar = icb.ib_xchar;
        ib->icb.ib_ychar = icb.ib_ychar;
        Fseek( (long)icb.ib_ptext, f_hand2, 0 );
        Fread( f_hand2, 11, str );
        fix_iconstr( str, ib );
      }
      else err++;
    l = RSC_ELEM(rsh_bitblk);
    if( !err && Fseek( l, f_hand, 0 ) == l )
      for( i=0; i<RSC_ELEM(rsh_nbb) && !err; i++ )
        if( Fread( f_hand, sizeof(BITBLK), &bb ) == sizeof(BITBLK) &&
            (ib=add_icon(wnum)) != 0 )
      {
        read_img( bb.bi_hl, bb.bi_wb<<3, (long)bb.bi_pdata, 0L, ib, f_hand2 );
        ib->icb.ib_ptext[0] = '\0';
        ib->icb.ib_xchar = ib->icb.ib_ychar = 0;
      }
    if( !err && ((RSC_ELEM(rsh_vrsn) & (1<<2)) || is_new) )
    {
      l = (RSC_ELEM(rsh_rssize) + sizeof(long) + 1L)&-2L;
      if( Fseek( l, f_hand, 0 ) == l && Fread( f_hand, 4L, &temp ) == 4L &&
          Fseek( temp, f_hand, 0 ) == temp )
      {
        for( ii=0L; ; ii++ )	/* find end of list & count */
          if( Fread( f_hand, 4L, &temp ) != 4L || temp ) break;
        if( temp==-1L )
          while( !err && --ii>=0 && (ib=add_icon(wnum)) != 0 )
          {
            if( Fread( f_hand, sizeof(icb), &icb ) != sizeof(icb) ||
                Fread( f_hand, 4L, &temp ) != 4L ) break;
            read_img( icb.ib_hicon, icb.ib_wicon, -1L, -1L, ib, f_hand );
            Fread( f_hand, 12, str );
            fix_iconstr( str, ib );
            ib->icb.ib_char = icb.ib_char & 0xFF00;
            *(long *)&ib->icb.ib_xchar = *(long *)&icb.ib_xchar;
            while( --temp>=0 && !err )
            {
              if( Fread( f_hand, sizeof(CICON), &cic ) != sizeof(CICON) )
              {
                err++;
                break;
              }
              l = (long)((icb.ib_wicon+15)>>4) * icb.ib_hicon;
              if( cic.num_planes == 1 ) i = 0;
              else if( cic.num_planes == 2 ) i = 1;
              else if( cic.num_planes == 4 ) i = 2;
              else
              {
                l *= (cic.num_planes+1)<<1/* word */;
                if( cic.sel_data ) l <<= 1L;
                Fseek( l, f_hand, 1 );	/* skip data */
                continue;
              }
              if( (new = lalloc( sizeof(CICON), -1 )) == 0 ||
                  !(*_icneo->alloc_im)( &new->col_data, cic.num_planes ) ||
                  !(*_icneo->alloc_im)( &new->col_mask, 1 ) )
              {
                err++;
                break;
              }
              if( !cic.sel_data ) new->sel_data = new->sel_mask = 0L;
              else if( !(*_icneo->alloc_im)( &new->sel_data, cic.num_planes ) ||
                  !(*_icneo->alloc_im)( &new->sel_mask, 1 ) )
              {
                cmfree( (char **)&new->col_data );
                cmfree( (char **)&new->col_mask );
                err++;
                break;
              }
              new->num_planes = cic.num_planes;
              read_cimg( icb.ib_hicon, icb.ib_wicon, new->num_planes, f_hand, new->col_data );
              read_cimg( icb.ib_hicon, icb.ib_wicon, 1, f_hand, new->col_mask );
              if( cic.sel_data )
              {
                read_cimg( icb.ib_hicon, icb.ib_wicon, new->num_planes, f_hand, new->sel_data );
                read_cimg( icb.ib_hicon, icb.ib_wicon, 1, f_hand, new->sel_mask );
              }
              ib->nicb.list[i] = new;
            }
            if( !(*_icneo->fix_icon)(ib) ) break;
          }
      }
    }
    Fclose(f_hand2);
    if( !*num_icons[ewind_type[wnum]] )
    {
      _f_alert1( _msg_ptr[17] );
      return(0);
    }
    return(1);
  }
  }
  if( f_hand2 >= 0 )
  {
    Fclose(f_hand2);
    _f_alert1( _msg_ptr[18] );
  }
  return(0);
}

int not_eof( char *ptr )
{
  return( *ptr != -1 );
}

int icn_int( char *ptr, int *out, char **optr )
{
  static char ign[]="\r\n {},";
  
  while( strchr(ign,*ptr) ) ptr++;
  if( not_eof(ptr) )
  {
    if( !strncmp(ptr,"0x",2) && (*_nac->sscnf)( ptr+=2, "%x", out ) || 
        (*_nac->sscnf)( ptr, "%d", out ) )
      if( (*optr=strchr(ptr,',')) != 0 || (*optr=strchr(ptr,'}')) != 0 )
          return( not_eof(ptr) );
  }
  _f_alert1( _msg_ptr[19] );
  return(0);
}

char *_strstr( char *s1, char *s2 )
{
  char *ptr;
  
  if( (ptr = strstr( s1, s2 )) == 0 ) _f_alert1( _msg_ptr[19] );
  return(ptr);
}

int read_icn( char *name, int hand, int wnum )
{
  char *ptr, *ptr0, noerr;
  long len;
  int w, h, i, j, skip_m, dum, *dptr, *mptr;
  ICONBUF *ib;
  
  if( (ptr0=lalloc(len = Fseek( 0L, hand, 2 )+1,ALLOC_ICON)) != NULL )
  {
    noerr = 1;
    Fseek( 0L, hand, 0 );
    len = Fread( hand, len, ptr=ptr0 );
    *(ptr0+len) = -1;
    if( (ptr=_strstr(ptr0,"_W")) != 0 && icn_int( ptr+3, &w, &ptr ) &&
        (ptr=_strstr(ptr0,"_H")) != 0 && icn_int( ptr+3, &h, &ptr ) &&
        (ptr=strchr(ptr0,'{')) != 0 )
    {
      j = h = h >= ICON_H ? ICON_H : h;
/*      skip_m = w < (ICON_W-15) ? (ICON_W-w)>>4 : 0; */
      skip_m = w < (ICON_W-15) ? 1 : 0;
      if( (ib = add_icon(wnum)) != 0 )
      {
        fix_iconstr( name, ib );
        ptr++;
        dptr = ib->icb.ib_pdata;
        mptr = ib->icb.ib_pmask;
        while( j-- && noerr )
        {
          for( i=0; i<(w+15>>4) && noerr; i++ )
          {
            noerr = icn_int( ptr, i<=1 ? dptr : &dum, &ptr );
            if( i<=1 )
            {
              dptr++;
              *mptr++ = 0;
            }
          }
          if( skip_m ) *dptr++ = *mptr++ = 0;
        }
        _memclr( dptr, h=(ICON_H-h)<<2 );
        _memclr( mptr, h );
/**     while( h++<ICON_H ) *((long *)dptr)++ = *((long *)mptr)++ = 0; **/
      }
      else noerr=0;
    }
    else noerr=0;
    lfree(ptr0);
    return(noerr);
  }
  else
  {
/*%    _f_alert1( _msg_ptr[20] );*/
    return(0);
  }
}

void fix_ibx( int *data, int pl )
{
  PICTURE pic = { { 0L, 32, 32, 2, 1, 0, 0, 0, 0 }, 16 };
  
  pic.mfdb.fd_addr = data;
  pic.mfdb.fd_nplanes = pl;
  (*_icneo->to_stand)( &pic );
}

int read_ibx( char *name, int hand, int wnum )
{
  int err=1;
  ICONBUF *ib;
  ICONBLK *icb;
  CICON *new, *new1;
  struct
  {
    long sig;
    long type;
    int width, height, planes;
    char reserved[2];
    int chars[11];
    char author[18], resv2[8];
  } header;
  
  if( Fread( hand, sizeof(header), &header ) == sizeof(header) &&
      (header.sig==0x49434249L && header.type==0L ||
       header.sig==0x49434233L && header.type==0x3000000L) )
  {
    err = 0;
    if( (ib = add_icon(wnum)) != 0 )
    {
      icb = &ib->icb;
      icb->ib_char = header.chars[0] ? header.chars[0] & 0xFF00 : 0x1000;
      icb->ib_xchar = header.chars[1];
      icb->ib_ychar = header.chars[2];
      fix_iconstr( name, ib );
      if( (new = lalloc( sizeof(CICON), -1 )) == 0 ||
          !(*_icneo->alloc_im)( &new->col_data, header.planes ) ||
          !(*_icneo->alloc_im)( &new->col_mask, 1 ) ) err++;
      else
      {
        read_cimg( header.height, header.width, new->num_planes=header.planes,
            hand, new->col_data );
        fix_ibx( new->col_data, header.planes );
        read_cimg( header.height, header.width, 1, hand, new->col_mask );
        ib->nicb.list[2] = new;
        if( !*(char *)&header.type ) new->sel_data = new->sel_mask = 0L;
        else if( !(*_icneo->alloc_im)( &new->sel_data, header.planes ) ||
            !(*_icneo->alloc_im)( &new->sel_mask, 1 ) ||
            (new1 = lalloc( sizeof(CICON), -1 )) == 0 ||
            !(*_icneo->alloc_im)( &new1->col_data, 1 ) ||
            !(*_icneo->alloc_im)( &new1->col_mask, 1 ) ) err++;
        else
        {
          ib->nicb.list[0] = new1;
          new1->num_planes = 1;
          new1->sel_data = new1->sel_mask = 0L;
          read_cimg( header.height, header.width, header.planes, hand,
              new->sel_data );
          fix_ibx( new->sel_data, header.planes );
          read_cimg( header.height, header.width, 1, hand, new->sel_mask );
          read_cimg( header.height, header.width, 1, hand, new1->col_data );
          read_cimg( header.height, header.width, 1, hand, new1->col_mask );
        }
      }
      if( !(*_icneo->fix_icon)(ib) ) err++;
    }
  }
  return 1;
}

long _long86( void *p )
{
  return *(long *)p = *(unsigned char *)p |
      ((unsigned int)*((unsigned char *)p+1)<<8) | 
      ((unsigned long)*((unsigned char *)p+2)<<16) | 
      ((unsigned long)*((unsigned char *)p+3)<<24);
}

int _int86( void *p )
{
  return *(int *)p = *(unsigned char *)p |
      ((unsigned int)*((unsigned char *)p+1)<<8);
}

void fix_icoica( CICON *new )
{
  long *i, *o;
  int rez, l;

  for( l=MASKSIZ/4, i=(long *)new->col_mask; --l>=0; )
    *i++ ^= -1L;		/* invert the mask */
  for( i=(long *)new->col_data, rez=new->num_planes; --rez>=0; )
    for( o=(long *)new->col_mask, l=MASKSIZ/4; --l>=0; )
      *i++ &= *o++;	/* AND the data with it */
  new->sel_data = new->sel_mask = 0L;
}

int read_ico( char *name, int hand, int wnum )
{
  int err=1, hand2, rez;
  long l, *i;
  ICONBUF *ib;
  CICON *new;
  PICTURE pic, *p=&pic;
  struct
  {
    long sig;	/* 0x100L (0w | 1w) */
    unsigned int count;
  } header1;
  struct
  {
    unsigned char width, height, colors, unused;
    int reserved[2];
    long dib_size, dib_offset;
  } header2;
  
  if( (*_nac->TOS_error)(hand2=Fopen(name,0),0) &&
      Fread( hand, sizeof(header1), &header1 ) == sizeof(header1) &&
      header1.sig==0x100L )
  {
    err = 0;
    ib = 0L;
    _int86(&header1.count);
    _memclr( &pic, sizeof(PICTURE) );
    while( !err && header1.count-- &&
        Fread( hand, sizeof(header2), &header2 )==sizeof(header2) &&
        Fseek( _long86(&header2.dib_offset), hand2, 0 ) == header2.dib_offset &&
        !(*_icneo->load_bmp)( hand2, 1, &pic ) )
    {
      if( pic.mfdb.fd_nplanes==1 ) rez = 0;
      else if( pic.mfdb.fd_nplanes==2 ) rez = 1;
      else if( pic.mfdb.fd_nplanes==4 ) rez = 2;
      else
      {
        (*_icneo->free_pic)( &p, 0 );
        continue;
      }
      if( !ib )
        if( (ib = add_icon(wnum)) == 0L )
        {
          (*_icneo->free_pic)( &p, 0 );
          continue;
        }
        else fix_iconstr( name, ib );
      if( (new = lalloc( sizeof(CICON), -1 )) == 0 ||
          !(*_icneo->alloc_im)( &new->col_data, pic.mfdb.fd_nplanes ) ||
          !(*_icneo->alloc_im)( &new->col_mask, 1 ) ) err++;
      else
      {
        ib->nicb.list[rez] = new;
        l = (long)pic.mfdb.fd_wdwidth * pic.mfdb.fd_h;
        img_open( pic.mfdb.fd_addr, l*((pic.mfdb.fd_nplanes+1)<<1) );
        read_cimg( pic.mfdb.fd_h, pic.mfdb.fd_w, new->num_planes=pic.mfdb.fd_nplanes,
            IMG_HAND, new->col_data );
        read_cimg( pic.mfdb.fd_h, pic.mfdb.fd_w, 1, IMG_HAND, new->col_mask );
        fix_icoica( new );
      }
      (*_icneo->free_pic)( &p, 0 );
    }
    if( ib )
      if( !(*_icneo->fix_icon)(ib) ) err++;
  }
  if( hand2>0 ) Fclose(hand2);
  return 1;
}

int read_ica( int hand, int wnum )
{
  int err=1, rez, hsiz;
  long l, *i;
  ICONBUF *ib;
  CICON *new;
  char data[DATASIZ*4];
  struct
  {
    long sig;	/* 0x49433200L or 0x49434D00L */
    unsigned int count;
    char unknown[12];
  } header1;
  struct
  {
    unsigned char planes, xpos[2], ypos[2];
    char label[25];	/* actually 26, but sizeof() is wrong otherwise */
  } header2;
  
  if( Fread( hand, sizeof(header1), &header1 ) == sizeof(header1) &&
      (header1.sig==0x49433200L || header1.sig==0x49434D00L) )
  {
    err = 0;
    ib = 0L;
    _int86(&header1.count);
    if( header1.sig==0x49433200L ) hsiz = sizeof(header2)+1;
    else
    {
      *header2.label = 0;
      hsiz = 5;
    }
    while( !err && header1.count-- && Fread( hand, hsiz, &header2 )==hsiz )
    {
      if( header2.planes==1 ) rez = 0;
      else if( header2.planes==2 ) rez = 1;
      else if( header2.planes==4 ) rez = 2;
      else
      {
        Fseek( (header2.planes+1)*DATASIZ, hand, 1 );
        continue;
      }
      if( (ib = add_icon(wnum)) == 0L ) break;
      fix_iconstr( header2.label, ib );
      if( (new = lalloc( sizeof(CICON), -1 )) == 0 ||
          !(*_icneo->alloc_im)( &new->col_data, header2.planes ) ||
          !(*_icneo->alloc_im)( &new->col_mask, 1 ) ) err++;
      else
      {
        Fread( hand, plane_size(new->num_planes=header2.planes), data );
        (*_icneo->bmp_data)( (char *)new->col_data, data, 32, 32, header2.planes, 0, 0, 0 );
        Fread( hand, MASKSIZ, data );
        (*_icneo->bmp_data)( (char *)new->col_mask, data, 32, 32, 1, 0, 0, 0 );
        fix_icoica( ib->nicb.list[rez] = new );
      }
      if( ib )
        if( !(*_icneo->fix_icon)(ib) ) err++;
    }
  }
  return 1;
}

int read_img_bmp( char *name, int hand, int wnum, int bmp )
{
  int rez;
  long l;
  ICONBUF *ib;
  CICON *new;
  PICTURE pic, *p=&pic;

  _memclr( &pic, sizeof(PICTURE) );
  if( !(bmp ? (*_icneo->load_bmp)( hand, 0, &pic ) :
      (*_icneo->load_img)( hand, &pic )) )
  {
    if( pic.mfdb.fd_nplanes==1 ) rez = 0;
    else if( pic.mfdb.fd_nplanes==2 ) rez = 1;
    else if( pic.mfdb.fd_nplanes==4 ) rez = 2;
    else
    {
      (*_icneo->free_pic)( &p, 0 );
      return 1;
    }
    if( (ib = add_icon(wnum)) != 0 )  
    {
      if( shrink ) (*_icneo->fit_pic)( p, 1, -1, ICON_W, ICON_H ); /* 004 */
      fix_iconstr( name, ib );
      if( (new = lalloc( sizeof(CICON), -1 )) != 0 &&
          (*_icneo->alloc_im)( &new->col_data, pic.mfdb.fd_nplanes ) &&
          (*_icneo->alloc_im)( &new->col_mask, 1 ) )
      {
        _memclr( new->col_mask, MASKSIZ );
        ib->nicb.list[rez] = new;
        l = (long)pic.mfdb.fd_wdwidth * pic.mfdb.fd_h;
        img_open( pic.mfdb.fd_addr, l*(pic.mfdb.fd_nplanes<<1) );
        read_cimg( pic.mfdb.fd_h, pic.mfdb.fd_w, new->num_planes=pic.mfdb.fd_nplanes,
            IMG_HAND, new->col_data );
        new->sel_data = new->sel_mask = 0L;
      }
      (*_icneo->fix_icon)(ib);
      (*_icneo->free_pic)( &p, 0 );
    }
  }
  return 1;
}

int read_ice( int hand, int wnum )
{
  struct
  {
    long i_cookie, size;
    int hdrsize;
    unsigned int entries, feoff, fnsize, nicons, ieoff, msksize, datsize;
  } hdr;
  struct
  {
    long index;
    char iconc, iconx, icony;
  } fname;
  char err, name[13];
  unsigned int i;
  long l;
  ICONBLK *icb;
  ICONBUF *ib;
  
  if( Fread( hand, sizeof(hdr), &hdr ) == sizeof(hdr) )
    for( i=err=0; i<hdr.entries && !err; i++ )
    {
      Fseek( l=hdr.hdrsize+i*hdr.feoff, hand, 0 );
      if( Fread( hand, sizeof(name), name ) != sizeof(name) ) err++;
      else
      {
        name[12] = '\0';
        Fseek( l+hdr.fnsize, hand, 0 );
        if( Fread( hand, sizeof(fname), &fname ) != sizeof(fname) ) err++;
        else if( (ib=add_icon(wnum)) != 0 )
        {
          Fseek( hdr.hdrsize+hdr.entries*hdr.feoff+fname.index*
              hdr.ieoff, hand, 0 );
          Fread( hand, 128L, (icb=&ib->icb)->ib_pmask );
          Fread( hand, 128L, icb->ib_pdata );
          (*_icneo->to_filename)( name, icb->ib_ptext );
          icb->ib_char = 0|0|(0<<8)|(1<<12);
          icb->ib_xchar = fname.iconx;
          icb->ib_ychar = fname.icony;
          ib->type = NOT_DFLT;	/* +i */
        }
        else err++;
      }
    }
  return(1);
}

static int n_wnum;
int add_nic_patt( int *data, int col )
{
  return add_patt( n_wnum, data, col );
}

int read_icons( int wnum, char *path )
{
  int f_hand, err, i, w, num, old_fmt;
  long len;
  char *ptr, was_open;

  if( wnum<0 || (w=ewind_type[wnum]) >= EDW_ICONS )
  {
    _bee();
    if( !path )
    {
      path=_z->w[wnum].path;
      was_open = 1;
    }
    else was_open = 0;
    if( (f_hand = Fopen( path, 0 )) >= 0 )
    {
      ptr = strrchr( path, '.' );
      if( !was_open && strcmp( ptr, ".NIC" ) || was_open && not_nic[wnum] )
      {
        if( !strcmp( ptr, ".RSC" ) && !read_rsc_i( path, f_hand, wnum ) || 
            !strcmp( ptr, ".ICN" ) && !read_icn( path, f_hand, wnum ) ||
            !strcmp( ptr, ".ICE" ) && !read_ice( f_hand, wnum ) ||
            !strcmp( ptr, ".ICO" ) && !read_ico( path, f_hand, wnum ) ||
            !strcmp( ptr, ".ICA" ) && !read_ica( f_hand, wnum ) ||
            !strcmp( ptr, ".BMP" ) && !read_img_bmp( path, f_hand, wnum, 1 ) ||
            !strcmp( ptr, ".IMG" ) && !read_img_bmp( path, f_hand, wnum, 0 ) ||
            !strncmp( ptr, ".IB", 3 ) && !read_ibx( path, f_hand, wnum ) )
        {
badfile:  Fclose(f_hand);
          _arrow();
          return(0);
        }
      }
      else
      {
        n_wnum = wnum;	/* used in add_nic_patt */
        if( (i=(*_icneo->read_nic)( f_hand, 0, &nicinfo[wnum], iconbuf[w],
            num_icons[w], icons_rem[w] )) == 0 ) goto badfile;
        else if( i<0 )
        {
          old_fmt = 0;
          if( (len = Fseek( 0L, f_hand, 2 )) == 2088L )
          {
            old_fmt++;
            num = 9;
          }
          else if( (num = len / 244) <= 0 || num*244L != len )
          {
            Fclose(f_hand);
            _arrow();
            _f_alert1( _msg_ptr[22] );
            return(0);
          }
          for( i=err=0; i<num && !err; i++ )
            err = iread( f_hand, i, old_fmt, wnum );
          nicinfo[wnum].auth[0] = nicinfo[wnum].comment[0][0] =
              nicinfo[wnum].comment[1][0] = nicinfo[wnum].comment[2][0]
              = '\0';
          nicinfo[wnum].create = nicinfo[wnum].modify = 0L;
        }
/*%        else num_new[w] = *num_icons[w];*/
      }
      Fclose( f_hand );
    }
    _arrow();
    if( was_open ) num_new0[i] = *num_icons[i=ewind_type[wnum]];
    return( (*_nac->TOS_error)( (long)f_hand, 0 ) );
  }
  return(1);
}

void clear_icons( int t )
{
  _bee();
  (*_icneo->free_nic)( iconbuf[t], num_icons[t] );
  free_patt(t);
  _arrow();
}

int save_nic( int num )
{
  int t;
  
  if( create_nic( 0, num ) )
  {
    cksum[t=_ed_wind_type(num)] = checksum( num, 0 );
    num_new0[t] = *num_icons[t];
    return(1);
  }
  return(0);
}

int check_save( int num )
{
  int t, chk;
  char temp[100];
  
  if( (t=_ed_wind_type(num)) < EDW_FILE ) return(-1);
  if( not_nic[num] ) return(1);
  _bee();
  chk = checksum( num, 0 );
  _arrow();
  if( chk != cksum[t] || num_new0[t] != *num_icons[t] )
  {
    _spf( temp, _msg_ptr[23], spathend(_z->w[num].path) );
    switch( _f_alert1( temp ) )
    {
      case 1:
        return( save_nic(num) );
      case 2:
        return(1);
      case 3:
        return(0);
    }
  }
  return(1);
}

int close_icons( int num )
{
  switch( check_save( num ) )
  {
    case 1:
      clear_icons( ewind_type[num] );
    case -1:
      return(1);
    default:
      return(0);
  }
}

NICONBLK *find_dflt( int wnum, int num )
{
  FSTRUCT file;
  int i;
  
  if( wnum<0 || ewind_type[wnum]==EDW_ICONS )
  {
    if( num==NPI ) return patt_nib[0];
    return( &(*_icneo->nic_icons)[num].nicb );
  }
  for( i=0; i<D_PROG; i++ )
  {
    get_icon( wnum, i, &file );
    if( file.type.p.pexec_mode==num ) return(file.nib);
  }
  _f_alert1( _msg_ptr[24] );
  return(0);
}

int i_copy( NICONBLK *to, NICONBLK *from )
{
  ICONBLK *ti, *fi;
  int rez;
  CICON **ci, *c;

  if( from==to ) return 1;
  ti = to->ib;
  fi = from->ib;
  __bytecpy( ti->ib_pmask, fi->ib_pmask, MASKSIZ );
  __bytecpy( ti->ib_pdata, fi->ib_pdata, DATASIZ );
  ti->ib_char  = fi->ib_char;
  ti->ib_xchar = fi->ib_xchar;
  ti->ib_ychar = fi->ib_ychar;
  if( to==patt_nib[0] )
  {
    icon2patt( ti, (*_icneo->deskpat)->ob_spec.bitblk );
    /* redo it in case source is an icon not a pattern */
    add_patt( -1, (*_icneo->deskpat)->ob_spec.bitblk->bi_pdata, (*_icneo->deskpat)->ob_spec.bitblk->bi_color );
    (*_icneo->do_desk)();
    return 1;
  }
  if( !from->list[0] )
  {
    if( (c = from->list[0] = lalloc( sizeof(CICON), -1 )) == 0 ) return 0;
    c->col_data = from->ib->ib_pdata;
    c->col_mask = from->ib->ib_pmask;
    c->sel_data = c->sel_mask = 0L;
  }
  for( rez=3; --rez>=0; )
  {
    if( *(ci=&to->list[rez]) != 0 )
      if( !from->list[rez] && rez )
      {
        free_icon( ci, to->ci, 1 );
        continue;	/* do nothing with this rez */
      }
      else
      {
        free_icon( ci, to->ci, 0 );
        if( *ci==to->ci ) to->ci = 0L;
      }
    else if( !from->list[rez] ) continue;
    else if( (*ci = lalloc( sizeof(CICON), -1 )) == 0 ) return 0;
    (*_icneo->copy_cicon)( n2pl[rez], from->list[rez], *ci );
  }
  if( (c = to->ci) != 0 ) free_icon( &c, 0L, 0 );
  cmfree( (char **)&to->ci );
  if( from->ci )
  {
    if( (c = to->ci = lalloc( sizeof(CICON), -1 )) == 0 ) return 0;
    (*_icneo->copy_cicon)( from->ci->num_planes, from->ci, c );
  }
  return 1;
}

int copy_icon( int wnum, int s_num, FSTRUCT *wfile, int witems, NICONBLK *ditem )
{       /* wnum is num of dest window or -1 if icons; s_num is index of src wind */
  ICONBUF *ib;
  NICONBLK *nib;
  char load, temp[120];
  int i, w;
  
  _arrow();
  if( wnum>=0 && not_nic[wnum] && ewind_type[wnum]!=EDW_ICONS )
  {
    _f_alert1( _msg_ptr[25] );
    return(0);
  }
  load = ewind_type[s_num] <= EDW_DISK;
  if( /*!load &&*/ _z->conf_copy )
    if( !ditem )
    {
      if( _f_alert1( _msg_ptr[26] ) == 2 ) return(0);
    }
    else if( _f_alert1( _msg_ptr[27] ) == 2 ) return(0);
  w = _ed_wind_type(wnum);	/* 003 */
  for( i=0; i<witems; i++, wfile++ )
    if( wfile->state )
    {
      if( wnum==s_num )	/* 003: add_icon can mess up wfile->nib */
      {
        (*_icneo->reset_icbs)( *iconbuf[w], i, i );
        get_icon( wnum, i, wfile );
      }
      if( !ditem )
        if( load )
        {
          strcpy( temp, _z->w[s_num].path );
          strcpy( spathend( temp ), wfile->name );
          if( !read_icons( wnum, temp ) ) return 1;
        }
        else if( wfile->type.p.pexec_mode < D_PROG )
        {
          if( (nib = find_dflt( wnum, wfile->type.p.pexec_mode )) != 0 )
            if( !i_copy( nib, wfile->nib ) ) return 1;
        }
        else
        {
          if( (ib=add_icon(wnum)) == 0 ) return(1);
          if( wnum==s_num )	/* 003: add_icon can mess up wfile->nib */
          {
            (*_icneo->reset_icbs)( *iconbuf[w], i, i );
            get_icon( wnum, i, wfile );
          }
          strcpy( ib->icb.ib_ptext, wfile->nib->ib->ib_ptext );
          ib->type = wfile->type.p.pexec_mode;
          if( !i_copy( &ib->nicb, wfile->nib ) ) return 1;
        }
      else if( !i_copy( ditem, wfile->nib ) ) return 1;
    }
  return 1;
}

void free_msg(void)
{
  if( _new_msgs ) lfree(_new_msgs);
}

void quitit( int no_quit )
{
  int i;
  
  _bee();
  if( !no_quit )
  {
    old_menu[OPTTITL].ob_state &= ~SELECTED;
    *(_icneo->menu) = old_menu;
  }
  no_xlate++;
  (*_icneo->close_ev_ic)();
  for( i=0; i<7; i++ )
    if( ewind_type[i]==EDW_DISK ) *spathend(_z->w[i].path) = 0;
  lfreeall(ALLOC_ICON);
  (*gui->Nrsc_free)(rshdr);
  free_msg();
  no_xlate=0;
  (*_icneo->reset_all_icons)(no_quit);
  if( !no_quit )
  {
    restore_pd();
    _z->notes_len = old_notes_len;
    __bytecpy( _z->showicon, old_showicon, 7*sizeof(int) );
    _z->mshowicon = old_mshowicon;
    __bytecpy( _z->w, old_ws, 7*sizeof(struct Wstruct) );
    __bytecpy( _z->filter, old_filt, 7*sizeof(FILT_TYPE) );
  }
  _arrow();
}

int quit_iedit( int no_quit )
{
  int i;
  
  for( i=0; i<7; i++ )
    if( _z->w[i].place ) 
      if( !check_save(i) ) return(0);
  if( cksum[0] != checksum( -1, 0 ) )
    if( no_quit )
    {
      if( _f_alert1( _msg_ptr[28] ) == 2 )
          return(0);
    }
    else switch( _f_alert1( _msg_ptr[29] ) )
    {
      case 1:
        _bee();
        if( !save_nic(-1) )
        {
          _arrow();
          return(0);
        }
        break;
      case 2:
        break;
      case 3:
        return(0);
    }
  quitit(no_quit);
  return(1);
}

unsigned char *bit_ptr;
int bit_count;

void outbits( unsigned char c, int bits )
{
  int i, j;
  
  if( bit_count >= bits )
  {
    if( (i=bit_count&7) == 0 ) i = 8;
    if( (j = i-bits) >= 0 )
    {
      *bit_ptr |= c<<j;
      if( !j ) bit_ptr++;
      bit_count -= bits;
    }
    else
    {
      outbits( c>>i, i );	/* send upper part */
      bits -= i;
      outbits( c&~(-bits), bits );	/* send lower bits */
    }
  }
  else bit_count = 0;
}

void out8bits( unsigned char c )
{
  if( bit_count )
  {
    *bit_ptr++ = c;
    bit_count--;
  }
}

int out_many8( unsigned char *data, int count )
{
  if( bit_count < count ) return 1;
  __bytecpy( bit_ptr, data, count );
  bit_count -= count;
  bit_ptr += count;
  return 0;
}

int out_unenc( unsigned char *u, unsigned char *data )
{
  int i, j;
  
  if( u )
    for( i=data-u; i>0; )
    {
      j = i>=(1<<6) ? (1<<6) : i;
      out8bits( j-1 );
      if( out_many8( u, j ) ) return 1;
      u += j;
      i -= j;
    }
  return 0;
}

int compr1( unsigned char *data, int s, unsigned char *buf )
{
  int i, t, t2, s0=s;
  unsigned char *u;
  
  bit_count = s;
  _memclr( bit_ptr = buf, s );
  u = data;
  while(s)
  {
    for( t=1; data[t]==data[0] && ++t<1<<6 && t<s; );
    if( t>2 )
    {
      if( out_unenc( u, data ) ) return 32767;
      u = 0L;
      out8bits( (1<<6)|(t-1) );
      out8bits( *data );
      data += t;
      s -= t;
      continue;
    }
    for( t=2; t<=5; t++ )
      if( t+t>s ) t=10;
      else if( !memcmp( data, data+t, t ) )
      {
        for( i=1, t2=t; t2+t<=s && !memcmp( data, data+t2, t ); t2+=t )
          if( ++i > 1<<4 )
          {
            t2 += t;
            break;
          }
        if( out_unenc( u, data ) ) return 32767;
        u = 0L;
        out8bits( (2<<6)|((t-2)<<4)|(i-2) );
        if( out_many8( data, t ) ) return 32767;
        data += t2;
        s -= t2;
        goto next;
      }
    if( !u ) u = data;
    data++;
    s--;
next:  ;
  }
  if( out_unenc( u, data ) ) return 32767;
  out8bits( (3<<6) );	/* end token */
  return s0-bit_count;
}

int comprx( unsigned char *data, int s, unsigned char *buf )
{
  *buf = *data+s;
  return 32767;
/*  return s-((bit_count+7)>>3) */
}

void write_enc( int planes, unsigned char type, int l, int *data, unsigned char *buf )
{
  char tok;
  
  tok = (planes<<2) | type;
  _fwrite( &tmp_hand, 2L, &l );
  _fwrite( &tmp_hand, 1L, &tok );
  key = KEY_START;
  encrypt( data, buf, l );
  _fwrite( &tmp_hand, l, buf );
}

int compress( int *data, int planes )
{
  unsigned char buf[DATASIZ*4], tok, type=0;
  int s, l, m;

  s = plane_size(planes);
  if( planes==1 )
  {
    m = comprx( (unsigned char *)data, s, buf );
    type = 2;
    if( (l = compr1( (unsigned char *)data, s, buf )) > m && m<s )
    {
      l = comprx( (unsigned char *)data, s, buf );
      type = 1;
    }
  }
  else
  {
    m = compr1( (unsigned char *)data, s, buf );
    type = 1;
    if( (l = comprx( (unsigned char *)data, s, buf )) > m && m<s )
    {
      l = compr1( (unsigned char *)data, s, buf );
      type = 2;
    }
  }
  if( l>=s )
  {
    type = 0;
    l = s;
  }
  else data = (int *)buf;
  write_enc( planes, type, l, data, buf );
/*%  switch(type)
  {
    case 0:
      Crawio('u');
      break;
    case 1:
      Crawio('x');
      break;
    case 2:
      Crawio('1');
  } */
  return l+3;
}

long find_chk( unsigned int num, int *chk, long *val, int data, long len, int pl, long *off, int *writ )
{
  SAVECHK *sc;
  int i, c;
  long *found, l;

  if( (c=*chk)==0 ) return len;
  found = 0L;
  sc = chk_start;
  while( (long)sc < (long)off && !found )	/* until (and including) dest chk */
  {
    for( i=0; i<3; i++ )
      if( data )
      {
        if( sc->chk[i][0]==c )
        {
          found = &sc->off[i][0];
          break;
        }
        if( sc->chk[i][2]==c )
        {
          found = &sc->off[i][2];
          break;
        }
      }
      else
      {
        if( sc->chk[i][1]==c )
        {
          found = &sc->off[i][1];
          break;
        }
        if( sc->chk[i][3]==c )
        {
          found = &sc->off[i][3];
          break;
        }
      }
    sc++;
  }
  /* make sure it didn't find itself */
  if( found && found!=off && *found )
  {
    *val = *found;
    *chk = 0;  /* invalidate, so that data won't be written */
    return len; /* leave *off as 0L */
  }
  l = compress( writ, data ? n2pl[pl] : 1 );
  *val = *off = len + sizeof(ICONFILE)*num;
  return len + l;
}

int write_patt( ICONBLK *ib )
{
  int bb_data[16];
  BITBLK bb;
  unsigned char buf[32];
  
  bb.bi_pdata = bb_data;
  icon2patt( ib, &bb );
  write_enc( 1, 0, 32, bb_data, buf );
  return bb.bi_color;
}

int create_nic( int new, int wnum )
{
#ifndef DEMO
  long l, len, *start;
  unsigned int i, num;
  unsigned char c;
  int j, k, **p;
  OBJECT *obj;
  char *ptr, *buf, temp[120];
  static char tmpname[]="x:ICONEDIT.$!$";
  ICONFILE icf;
  ICON_TYPE it;
  FSTRUCT file;
  SAVECHK *sc;
  
  if( new )
  {
    rsrc_adr( NICINFO, &obj );
    kick( _z->w[wnum].path );
    strcpy( ptr=temp, _z->w[wnum].path );
    strcpy( spathend(temp), obj[NICNAME].ob_spec.tedinfo->te_ptext );
    strcat( temp, ".NIC" );
    if( !Fsfirst( temp, 0x37 ) )
    {
      _f_alert1( _msg_ptr[30] );
      return(0);
    }
  }
  else if( wnum<0 || ewind_type[wnum]==EDW_ICONS )
  {
    if( _z->dflt_path[0] <= 'B' )
    {
      _spf( temp, _msg_ptr[14], _z->dflt_path[0] );
      if( _f_alert1( temp ) == 2 ) return(0);
    }
    strcpy( temp, _z->dflt_path );
    strcat( ptr=temp, "NEOICONS.NIC" );
  }
  else ptr=_z->w[wnum].path;
/*%  else Fdelete( ptr=_z->w[wnum].path ); */
  _bee();
  tmpname[0] = *ptr;
  if( (*_nac->TOS_error)( fhand=(*_icneo->prep_save)(ptr), 0 ) &&
      (*_nac->TOS_error)( tmp_hand=(*_icneo->prep_save)(tmpname), 0 ) )
  {
    key = KEY_START;
    l = ((long)Tgetdate()<<16)|Tgettime();
    if( new )
    {
      encrypt( &l, &iccreate, sizeof(l) );
      encrypt( &l, &icmodify, sizeof(l) );
      encrypt( obj[NICAUTH].ob_spec.tedinfo->te_ptext, icauth, 26 );
      encrypt( obj[NICCOMM1].ob_spec.tedinfo->te_ptext, iccomment, 36 );
      encrypt( obj[NICCOMM1+1].ob_spec.tedinfo->te_ptext, iccomment+36, 36 );
      encrypt( obj[NICCOMM1+2].ob_spec.tedinfo->te_ptext, iccomment+72, 36 );
    }
    else
    {
      icentries = total_icons(_ed_wind_type(wnum));
      if( wnum>=0 ) nicinfo[wnum].modify = l;
      encrypt( wnum<0 ? _icneo->nic_info : &nicinfo[wnum], &iccreate, sizeof(nicinfo[0]) );
    }
    num = icentries;
    key = KEY_START;
    encrypt( &icentries, &icentries, sizeof(icentries) );
    i = (long)&icon_list - (long)extract_icon;
    key = KEY_START;
    encrypt( &i, &iccode, sizeof(i) );
    _fwrite( &fhand, len=(long)&icon_list-(long)&icheader, &icheader );
    checksum( new ? -1 : wnum, 1 );
    for( i=0, sc=chk_start; i<num; i++ )
      if( get_icon( new ? -1 : wnum, i, &file ) )
      {
        _memclr( start=&icf.data[0][0], 12*sizeof(long) );
        strcpy( icf.string, file.nib->ib->ib_ptext );
        if( file.type.p.pexec_mode==NPI )
        {
          icf.size_x = 1;
          icf.size_y = 16;
          icf.type = 0;
          icf.colors = write_patt( file.nib->ib );
          *start = len + sizeof(ICONFILE)*num;
          len += 32 + 3;
        }
        else
        {
          icf.size_x = ICON_WW;
          icf.size_y = ICON_H;
          icf.xchar = file.nib->ib->ib_xchar;
          icf.ychar = file.nib->ib->ib_ychar;
          if( (c=file.nib->ib->ib_char>>8) == 0 ) c = 1;
          icf.colors = c;
          it.type.file = (file.nib->ib->ib_char&1) == 0;
          it.type.folder = (file.nib->ib->ib_char&2) == 0;
          it.type.drive = (file.nib->ib->ib_char&4) == 0;
          it.type.dflt = file.type.p.pexec_mode<D_PROG ? 
              file.type.p.pexec_mode : -1;
          icf.type = it.i;
          for( j=0; j<3; j++, start+=4 )
            if( file.nib->list[j] == 0 )
            {
              if( !j )
              {
                len = find_chk( num, &sc->chk[j][0], start, 1, len, j, &sc->off[j][0], file.nib->ib->ib_pdata );
                len = find_chk( num, &sc->chk[j][1], start+1, 0, len, j, &sc->off[j][1], file.nib->ib->ib_pmask );
              }
              else *(long *)&sc->chk[j][0] = 0L;
              *(long *)&sc->chk[j][2] = 0L;
            }
            else
              for( p=&file.nib->list[j]->col_data, k=0; k<4; k++, p++ )
                if( *p )
                    len = find_chk( num, &sc->chk[j][k], start+k, !(k&1), len, j, &sc->off[j][k], *p );
                else sc->chk[j][k] = 0;
        }
        key = KEY_START;
        encrypt( &icf, &icf, sizeof(icf) );
        _fwrite( &fhand, sizeof(icf), &icf );
        sc++;
      }
    if( tmp_hand>0 )
    {
      Fseek( 0L, tmp_hand, 0 );
      if( (long)lalloc(-1L,-1) < len || (buf=lalloc(l=len,-1)) == 0 )
      {
        buf = (char *)tmppix;
        l = sizeof(tmppix);
      }
      while( fhand>0 && (len=Fread(tmp_hand,l,buf)) > 0 )
        _fwrite( &fhand, l, buf );
    }
    if( fhand>0 ) Fclose(fhand);
    if( tmp_hand>0 ) Fclose(tmp_hand);
    if( fhand<0 || tmp_hand<0 ) Fdelete(ptr);
    Fdelete(tmpname);
    cmfree( (char **)&chk_start );
    if( wnum>=0 )
    {
      if( new && fhand>0 ) (*_icneo->open_to_path)(ptr);
      strcpy( temp, ptr );
      *spathend(temp) = '\0';
      j = wnum;
      /* hack to make sure Neo thinks this is a drive window */
      for( i=0; i<7; i++ )
        if( _z->w[i].place > 0 && ewind_type[i] < EDW_FILE ) j = i;
      (*_icneo->update_wnum)( j, temp );
    }
  }
  _arrow();
  return( fhand>0 );
#else DEMO
  _f_alert1( _msg_ptr[new ? IED_MSGS-1 : IED_MSGS-2] );
  return(1);
#endif
}

int swap_icons( FSTRUCT *fs1, FSTRUCT *fs2, int wnum, int first, int next )
{
  ICONBLK *icb1, *icb2;
  
  if( fs1->type.p.pexec_mode < D_PROG || fs2->type.p.pexec_mode < D_PROG )
  {
    _f_alert1( _msg_ptr[32] );
    return(1);
  }
  /* depends on order of ICONBUF */
  (*_nac->byteswap)( fs1->nib, fs2->nib, sizeof(ICONBUF) );
  wnum = ewind_type[wnum];
  if( patt_nib[wnum] )
  {
    first--;
    next--;
  }
  (*_icneo->reset_icbs)( *iconbuf[wnum], first, first+1 );
  (*_icneo->reset_icbs)( *iconbuf[wnum], next, next+1 );
  return(0);
}

int ic_reorder( FSTRUCT *wfile, int witems, char *reorder_on, int wnum )
{
  int i, j, k, l;
  char buf[13];
  
  if( !*reorder_on ) switch( _f_alert1( _msg_ptr[33] ) )
  {
    case 1:
      for( i=D_PROG-1, l=0; i<witems; i++ )
        for( j=(k=i)+1; j<witems; j++ )
          if( wfile[j].nib->ib->ib_char&ICON_DRIVE &&		/* 003 */
              !(wfile[i].nib->ib->ib_char&ICON_DRIVE) ||		/* 003 */
              (**_icneo->match)( wfile[j].name, wfile[k].name ) )
          {
            swap_icons( &wfile[k], &wfile[j], wnum, j, k );
            k = j;
            l = 1;
          }
      if( l ) update_wind(wnum);
      return(0);	/* never do update_othwind in m_reorder */
    case 3:
      return(0);
  }
  *reorder_on ^= 1;
  return(1);
}

int nt_nic( int wnum )
{
  return(not_nic[wnum]);
}

void im_enable( OBJECT *mnu, int item, int flag )
{
  int *i;

  i = (int *)&mnu[item].ob_state;
  if( *i != (flag ? *i&~DISABLED : *i|DISABLED) ) menu_ienable( mnu, item, flag );
}

void ied_enable( int item, int flag )
{
  im_enable( ed_menu, item, flag );
}

void imenu_enable( int wind, int item, int flag )
{
  im_enable( wind<0 ? imenu : (*_icneo->wmenu)[wind], item, flag );
}

int iwind_enable( int index, int on )
{
  int i;

  for( i=0; i<7; i++ )
    if( _z->w[i].place>0 && ewind_type[i]>=EDW_ICONS )
        imenu_enable( i, index, on );
  return on;
}

void enable_menu( int wnum )
{
  int i, j;
  
  if( iget_top()==ed_handle )
  {
/*%    ied_enable( IEDUNDO, prev_ok ); causes problems with Cancel */
    for( i=j=0; i<3; i++ )
      if( nib_samp.list[i] ) j++;
    ied_enable( IDELREZ, icolors && j>1 && !ed_pat );
    ied_enable( IDELSEL, i_cic[icolors].sel_data!=0 && !ed_pat );
  }
  i = ewind_type[wnum];
  imenu_enable( -1, IRESETI, !*_icneo->reorder_on );
  if( wnum>=0 && i>=EDW_ICONS )
  {
    iwind_enable( IFILCLS, iwind_enable( IDELALL, iwind_enable( IEDNEWIC, !*_icneo->reorder_on &&
        iwind_enable( IREORDER, 1 ) ) ) );
    iwind_enable( ISAVENIC, !not_nic[wnum] && !*_icneo->reorder_on );
  }
}

int trans_icmenu( unsigned char *neo, unsigned char *iced, int num, int from_neo )
{
  char *p;
  
  if( !no_xlate )
    if( from_neo )
    {
      if( (p = strchr(neo,num)) != 0 ) return iced[p-neo];
    }
    else if( (p = strchr(iced,num)) != 0 ) return neo[p-iced];
  return(0);
}

int trans_wmenu( int wnum, int num, int from_neo )
{	/* translate a window menu index */
  static unsigned char neo[] = { WIMOPEN, WIMSHOW, CLOSEFLD, CLOSEWIN,
      CREATE, SELALL, UPDATE, WIMDEL, 0 };
  static unsigned char iced[] = { IWIMOPEN, IWIMSHOW, ICLSFLD, ICLSWIN,
      ICREATEF, ISELALL, IEDUPDAT, IDELITM, 0 };
  static unsigned char neof[] = { WIMOPEN, CLOSEWIN,
      REORDER, SELALL, WIMDEL, 0 };
  static unsigned char icedf[] = { IFILOPEN, IFILCLS,
      IREORDER, IFILALL, IFILDEL, 0 };
  
  if( wnum<0 || ewind_type[wnum]>=EDW_ICONS )
      return trans_icmenu( neof, icedf, num, from_neo );
  return trans_icmenu( neo, iced, num, from_neo );
}

int trans_mmenu( int num, int from_neo )
{	/* translate a desktop menu index */
  static unsigned char neo[] = { OPEN, SHOWINF, 0 };
  static unsigned char iced[] = { IOPEN, ISHOW, 0 };
  
  return trans_icmenu( neo, iced, num, from_neo );
}

void d_to_m(void)
{
  int i;
  
  for( i=0; i<PIXELS; i++ )
    if( pixel[0][i]&0x7f ) pixel[0][i] |= 0x80;
}

unsigned char pix_rot( int i, int j )
{
  return pixel[x_max-j-1][i];
}

unsigned char pix_half( int i, int j )
{
  return i<(x_max>>1) ? pixel[i<<1][j] : 0;
}

unsigned char pix_dbl( int i, int j )
{
  return pixel[i>>1][j];
}

unsigned char pix_hflip( int i, int j )
{
  return pixel[i][x_max-j-1];
}

unsigned char pix_vflip( int i, int j )
{
  return pixel[x_max-i-1][j];
}

void x_pix( unsigned char func( int, int ) )
{
  unsigned char k;
  int i, j;

  redo_prev();
  k = pix_mask();
  for( i=0; i<x_max; i++ )
    for( j=0; j<x_max; j++ )
      tmppix[i][j] = ((*func)( i, j )&k) | (pixel[i][j] & ~k);
  __bytecpy( pixel, tmppix, PIXELS );
  redo_sample();
  do_disp( move, mbox );
}

int use_imenu( int wnum, int num )
{
  int i, j, k, w;
  Rect mbx;
  char temp[200], c;
  FSTRUCT f;

  w = ewind_type[wnum];  
  if( wnum<0 && num!=IDELALL/*Neo Icons to trash */ ) switch( num )
  {
    case IQUIT:
      if( quit_iedit(0) ) return(1);
      break;
    case IABOUT:
      (*gui->start_form)( *_icneo->AES_handle, NAME|MOVER|CLOSER, &forms[1] );
      break;
    case IRESETI:
      if( quit_iedit(1) )
      {
        _bee();
        if( !ic_main( _icneo, 0 ) )
        {
          quitit(0);
          return(1);
        }
        _arrow();
        return(2);
      }
      break;
    case IEDITPAT:
      edit_desk();
      break;
    case ISHRINK:
      menu_icheck( imenu, ISHRINK, (imenu[ISHRINK].ob_state&CHECKED)==0 );
      shrink = imenu[ISHRINK].ob_state&CHECKED;		/* 004 */
      break;
    case -IEDUNDO:
      if( prev_ok )
      {
        (*_nac->byteswap)( pixel, previous, PIXELS );
        i = imode;
        imode = IPBOTH;	/* force redo_sample to do whole icon */
        redo_sample();
        if( i!=IPBOTH )
        {
          imode = i;
          redo_sample();	/* redundant for real mode, but easy to do */
        }
        do_disp( move, mbox );
      }
      break;
    case -IEDCLEAR:
/*%   redo_prev();
      for( j=0; j<PIXELS; j++ )
        pixel[0][j] &= i;
      redo_sample();
      do_disp( move, mbox );
      break; */
      i = 0x7f;
      goto clear;
    case -IEDDELBO:
      i = 0;
      goto clear;
    case -IEDDELIC:
      i = 0x80;
clear:
/*%      i = 0;
      if( imode < IPBOTH && !ed_pat )
      {
        _spf( temp, _msg_ptr[0], editor[IEDVSEL].ob_spec.free_string );
        if( (j = _f_alert1( temp )) == 3 ) break;
        if( j==1 )
          if( imode == IPDATA ) i = 0x80;
          else i = 0x7f;
      } */
      redo_prev();
      for( j=0; j<PIXELS; j++ )
        pixel[0][j] &= i;
      i = imode;
      imode = IPBOTH;	/* force redo_sample to do whole icon */
      redo_sample();
      if( i!=IPBOTH )
      {
        imode = i;
        redo_sample();	/* redundant for real mode, but easy to do */
      }
      do_disp( move, mbox );
      break;
    case -IDELSEL:
      cmfree( (char **)&i_cic[icolors].sel_data );
      cmfree( (char **)&i_cic[icolors].sel_mask );
      set_collist(1);
      if( icolors ) cmfree( (char **)&ic_samp.sel_data );
      else ic_samp.sel_data = 0L;
      ic_samp.sel_mask = 0L;
      if( iselected )
      {
        no_select();
        change_sel();
      }
      sample();
      break;
    case -IDELREZ:
      cmfree( (char **)&i_cic[icolors].col_data );
      cmfree( (char **)&i_cic[icolors].col_mask );
      cmfree( (char **)&i_cic[icolors].sel_data );
      cmfree( (char **)&i_cic[icolors].sel_mask );
      nib_samp.list[icolors] = 0L;
      set_collist(1);
      for( i=0; i<3; i++ )
        if( nib_samp.list[i] )
        {
          goto_colors(i);
          break;
        }
      break;
    case -IEDBLUR:
      redo_prev();
      for( i=0; i<x_max; i++ )
        for( j=0; j<y_max; j++ )
          if( !pixel[j][i] )
          {
            for( c=0, k=i-1; k<=i+1 && !c; k++ )
              for( w=j-1; w<=j+1 && !c; w++ )
                if( k>=0 && w>=0 && k<x_max && w<y_max ) 
                    if( pixel[w][k] ) c++;
            if(!c) tmppix[j][i] = 0;
            else tmppix[j][i] = pixel[j][i] | 0x80;
          }
          else tmppix[j][i] = pixel[j][i] | 0x80;
      __bytecpy( pixel, tmppix, PIXELS );
      redo_sample();
      if( imode != IPDATA ) do_disp( move, mbox );
      break;
    case -IROT:
      x_pix( pix_rot );
      break;
    case -IVFLIP:	/* 002 */
      x_pix( pix_vflip );
      break;
    case -IHFLIP:	/* 002 */
      x_pix( pix_hflip );
      break;
    case -IEDDBL:	/* 003 */
      x_pix( pix_dbl );
      break;
    case -IEDHALF:	/* 003 */
      x_pix( pix_half );
      break;
    case -IEDINTER:
      redo_prev();
      d_to_m();
      __bytecpy( tmppix, pixel, PIXELS );
      j = imode;
      imode = IPDATA+1;
      /* seed fill around the perimeter and take any pixels still
         without a mask and give them one */
      for( i=0; i<x_max; i++ )
      {
        if( !(tmppix[0][i]&0x80) ) seedfill( i, 0, 0x80, 0 );
        if( !(tmppix[x_max-1][i]&0x80) ) seedfill( i, x_max-1, 0x80, 0 );
        if( !(tmppix[i][0]&0x80) ) seedfill( 0, i, 0x80, 0 );
        if( !(tmppix[i][x_max-1]&0x80) ) seedfill( x_max-1, i, 0x80, 0 );
      }
      imode = j;
      for( i=0; i<PIXELS; i++ )
        if( !(tmppix[0][i]&0x80) ) pixel[0][i] |= 0x80;
      redo_sample();
      if( imode != IPDATA ) do_disp( move, mbox );
      else _f_alert1( _msg_ptr[1] );
      break;
    case -IEDDTOM:
      redo_prev();
      d_to_m();
      redo_sample();
      if( imode != IPDATA ) do_disp( move, mbox );
      else _f_alert1( _msg_ptr[1] );
      break;
    case -IEDLET:
      if( idcml != IEDLET )
      {
        to_letter();
        imenu_set( IDRAW, 4, idcml = -num );
      }
      break;
    case -IDRAW:
    case -ICOPY:
    case -IMOVE:
      if( idcml != -num )
      {
        if( move ) xor_box( move=0, mbox.x, mbox.y, mbox.w, mbox.h );
        imenu_set( IDRAW, 4, idcml = -num );
      }
      break;
    case -INONE:
    case -ILR:
    case -ITB:
    case -I4WAY:
      imenu_set( INONE, 4, imirr = -num );
      break;
    case -IEDCUTB:
      cut_mode( CUTB );
      break;
    case -IEDCOPYB:
      cut_mode( COPYB );
      break;
    case -IEDCUT:
      save_clip( 0, 0, x_max, y_max, 1 );
      break;
    case -IEDCOPY:
      save_clip( 0, 0, x_max, y_max, 0 );
      break;
    case -IEDPASTE:
      paste_clip( 0, 0 );
      break;
    case -IEDPASTB:
      paste_clip( -1, -1 );
  }
  else if( w>=EDW_ICONS ) switch(num)
  {
    case ISAVENIC:
      save_nic(wnum);
      break;
    case IEDNEWIC:
      if( add_icon(wnum) )
        if( w == EDW_ICONS ) update_wind( -1 );
        else (*_icneo->first)(0);
      break;
    case IDELALL:
      if( wnum<0 || w == EDW_ICONS )
      {
        if( (i=*num_icons[0]/*%+%num_new[0]*/-D_PROG+1) != 0 )
          if( !_z->conf_del || _f_alert1( _msg_ptr[34] ) == 1 )
          {
            (*_icneo->free_iconbuf)( _icneo->nic_icons, D_PROG-1, num_icons[0],
                icons_rem[0] );
            /*%num_new[0] -= i;*/
            update_wind( -1 );
/*%            get_all_icons();
            update_icons();  replaced by above */
          }
      }
      else if( *num_icons[w] && (!_z->conf_del || _f_alert1( _msg_ptr[35] ) == 1) )
      {
        i = 0;
        while( get_icon( wnum, i, &f ) )
          if( f.type.p.pexec_mode >= D_PROG ) delete_icon( wnum, i );
          else i++;
        (*_icneo->first)(0);
      }
  }
  else if( num==ICREATEN ) nic_info( 0, wnum, 0L, 0L );
  return(0);
}

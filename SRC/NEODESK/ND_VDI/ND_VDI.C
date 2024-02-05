#include "tos.h"
#include "aes.h"
#include "vdi.h"
#include "string.h"
#include "neodesk.h"
#include "neocommn.h"

#define FSMC            0x46534D43L     /* FSMC FSM/Speedo cookie */
#define _SPD            0x5F535044L     /* _SPD cookie value */
#define NVDI		0x4E564449L

long ft_scr_ptr;
int vdi_hand, aes_vdi_hand;
char have_fonts, true_type;
WIND_FONT *fonts;
MFDB fdb1 = { 0L, ICON_W, ICON_H, 2, 0, 1, 0, 0, 0 },
    fdb2;                                       /* default to 0's */
int work_in[] = { 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 2 }, work_out[57],
    /*contrl[12], intin[50], intout[10], ptsin[50], ptsout[50],*/ vxarr[4],
    clip_arr[4], first_char;
static int wr_mode, txt_point, txt_color, gr_font, clip_on, halign, valign,
    ln_pat, ln_type=-1, ln_color, f_style, f_color, f_inter, lnmask, effect,
    lntype, l_start, l_end, lwidth, ch_mode;
static int hw, hh, lw, lh;

static int text_arr[sizeof(_VDIParBlk.intin)/2];
static char text_off[sizeof(_VDIParBlk.intin)/2], char_off[256];

void v_ftext16( int handle, int x, int y, int *wstr, int strlen );
void v_ftext16_mono( int handle, int x, int y, int *wstr, int strlen, int offset );
void vqt_f_extent16( int handle, int *wstr, int strlen, int *extent );

void _colbits( int bit );
void _pats( int *patptr, int patmsk );
void _x1y1arr( int *arr );
void _blit( Rect *box1, Rect *box2, int mode, int tr, long ptr );
void _blitit( Rect *box1, Rect *box2, int tr );
int _set_wmode( int mode );
int _wmode0(void);
int _wmode1(void);
int _wmode2(void);
void _draw_bx( int *box );
void _set_lnmask( int mask );
int _get_lnmask(void);
void _desk_color( int col );
void _blit_init( MFDB *fdb );
void _x1y1val( int x1, int y1, int x2, int y2 );
void _set_pattern( int *box );
void _set_intin12( int i1, int i2 );
void _small_char( int x, int y, int fg, unsigned int c );
void _form_copy( int flag, char *buf, Rect *r );
void _init_scrptr(void);
void _gtext( int x, int y, unsigned char *str, int fnum, int center, int mode, int color );
void _gr_linebox( int *arr );
void _put_pixel( int x, int y, int col );
void _set_butv( int flag );
void _wait_mbut(void);
int _get_mbut(void);
void _set_clip( int *arr, int mode );
void _hide_mouse(void);
void _show_mouse(void);
void gr_box(void);
void gr_hline(void);
void _graph_exit(void);
void _reset_mouse( int grf_mouse( int num, MFORM *addr ) );
int _reinit(void);
void _load_fonts( WIND_FONT *fonts, int size );
void no_fonts(void);
int _vst_point( int size, int *out1, int *out2, int *out3, int *out4 );
int _vst_font( int id, int scale );
void _vst_color( int color );
char check_mono( char scale );
int _vqt_extent( unsigned char *str, int fnum );
void _vst_alignment( int hor, int vert );
void _vst_effects( int eff );
void _vsl_type( int type );
void _vsf_style( int style );
void _vsf_interior( int style );
void text_2_arr( unsigned char *str, int *num );
/*void arb_box( int intcol, int inter, int style, int writ, Rect *r );*/
void _vsl_color( int color );
void _vsl_ends( int start, int end );
void _vsl_width( int width );
void ftext16_mono( int x, int y, int len );
void _vst_charmap( int mode );

GRAPHICS _graphics = {
   _colbits, _pats, _x1y1arr, _blit, _blitit, _set_wmode,
   _wmode0, _wmode1, _wmode2, _draw_bx, _set_lnmask, _get_lnmask, _desk_color,
   _blit_init, _x1y1val, _set_pattern, _set_intin12, _small_char,
   _form_copy, _init_scrptr, _gtext, _hide_mouse, _show_mouse, gr_box,
   gr_hline, _gr_linebox, _put_pixel, _set_butv, _wait_mbut, _get_mbut,
   _set_clip, _graph_exit, 0L,  0, 0, 0, 0, 0, 0,  0L,  0L,  0,
   _reset_mouse, _reinit,  0,  0,  0, 0, 0, 0,  _load_fonts, no_fonts,
   _vst_point, _vst_font, _vst_color, check_mono, _vqt_extent, _vst_alignment,
   _vst_effects, _vsl_type, _vsf_style, _vsf_interior, text_arr, text_2_arr,
   _vsl_color, _vsl_ends, _vsl_width, 0, 0, ftext16_mono, _vst_charmap
};

/* MUST be first!! */
long initialize(void)
{
  if( !_reinit() ) return 0L;
  return( (long)&_graphics );
}

void no_fonts(void)
{
  if( have_fonts )
  {
    vst_unload_fonts( vdi_hand, 0 );
    have_fonts = 0;
    _graphics.total_fonts = work_out[10];
  }
}

int _reinit(void)
{
  int i, dum, work[57], cel_w;
  long *cookie;

  aes_vdi_hand = vdi_hand = graf_handle( &cel_w, &_graphics.cel_ht, &dum, &dum );
  work_in[0] = Getrez() + 2;
  v_opnvwk( work_in, &vdi_hand, work_out );
  if( !vdi_hand )
  {
    form_alert( 1, "[1][Error opening VDI|workstation!][Abort]" );
    return 0;
  }
  vq_extnd( vdi_hand, 1, work );
  _vsf_interior(4);
  _init_scrptr();
  _graphics.v_x_max = work_out[0]+1;
  _graphics.v_y_max = work_out[1]+1;
  _graphics.vplanes = work[4];
  _graphics.has_clut = work[5];
  vq_chcells( vdi_hand, &_graphics.v_cel_my, &_graphics.v_cel_mx );
  _graphics.v_cel_mx--;
  _graphics.v_cel_my--;
/*  _graphics.v_cel_mx = (work_out[0]+1)/cel_w - 1;  003
  _graphics.v_cel_my = (work_out[1]+1)/_graphics.cel_ht - 1; */
  _graphics.screen_ptr = &ft_scr_ptr;
  _graphics.work_out = work_out;
  _graphics.handle = vdi_hand;
  _graphics.has_gdos = vq_gdos();
  _graphics.speedo = (_graphics.scalable = CJar(0,FSMC,&cookie) == CJar_OK)
      != 0 && *cookie == _SPD;
  _graphics.NVDI3 = _graphics.speedo && CJar(0,NVDI,&cookie) == CJar_OK &&
      cookie && *(int *)cookie>=0x300;
  _graphics.total_fonts = work_out[10];
  wr_mode = ln_type = ln_color = f_color = f_style = f_inter,
      gr_font = clip_on = lnmask = halign = valign = effect = ln_type =
      l_start = l_end = lwidth = -1;
  ch_mode = 1;
  return 1;
}

static char xcols[] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14,
    12, 15, 13, 1 };

void _colbits( int bit )
{
  bit = xcols[bit&0xf];
  if( bit != f_color )
  {
    vsf_color( vdi_hand, f_color=bit );
    _vsf_interior(4);
    _vsf_style(1);
  }
}
#pragma warn -par
void _pats( int *patptr, int patmsk )
{
  static int p[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1 };

  if( !patptr ) patptr = p;
  vsf_udpat( vdi_hand, patptr, 1 );
  _vsf_interior(4);
}
#pragma warn +par
void _x1y1arr( int *arr )
{
  vxarr[0] = *arr++;
  vxarr[1] = *arr++;
  vxarr[2] = *arr++;
  vxarr[3] = *arr;
}

int copytran, blitcols[2];
MFDB *blitadr;

void _blit_init( MFDB *fdb )
{
  blitadr = fdb;
  copytran = 0;
}

void _blit( Rect *box1, Rect *box2, int mode, int tr, long ptr )
{
  if( (copytran = mode) != 0 )
  {
    fdb1.fd_addr = (void *)ptr;
    blitadr = &fdb1;
  }
  else blitadr = &fdb2;
  _blitit( box1, box2, tr );
}
void _blitit( Rect *box1, Rect *box2, int tr )
{
  int pts[8];

  pts[2] = (pts[0] = box1->x) + box1->w;
  pts[3] = (pts[1] = box1->y) + box1->h - 1;
  pts[6] = (pts[4] = box2->x) + box2->w;
  pts[7] = (pts[5] = box2->y) + box2->h - 1;
  _hide_mouse();
  if( copytran ) vrt_cpyfm( vdi_hand, tr, pts, blitadr, &fdb2, blitcols );
  else vro_cpyfm( vdi_hand, tr, pts, blitadr, &fdb2 );
  _show_mouse();
}
int _set_wmode( int mode )
{
  int old;
  
  old = wr_mode;
  if( ++mode != old )
  {
    vswr_mode( vdi_hand, mode );
    wr_mode = mode;
  }
  return(old-1);	/* 003: -1 */
}
int _wmode0(void)
{
  return( _set_wmode(0) );
}
int _wmode1(void)
{
  return( _set_wmode(1) );
}
int _wmode2(void)
{
  return( _set_wmode(2) );
}
void _draw_bx( int *box )
{
  int i, new[10], *n=new;

  for( i=8; --i>=0; )
    *n++ = *box++;
  *n++ = *(box-8);
  *n = *(box-7);
  _vsl_type( 7 );
  v_pline( vdi_hand, 5, new );
}
void _set_lnmask( int mask )
{
  if( lnmask != mask ) vsl_udsty( vdi_hand, lnmask=mask );
}
int _get_lnmask(void)
{
  return(lnmask);
}
void _desk_color( int col )
{
  if( col != f_color ) vsf_color( vdi_hand, f_color=col );
  _vsf_interior(4);
}
void _x1y1val( int x1, int y1, int x2, int y2 )
{
  vxarr[0] = x1;
  vxarr[1] = y1;
  vxarr[2] = x2;
  vxarr[3] = y2;
}
void _set_pattern( int *box )
{
  _pats( box, 0 );
}
void _set_intin12( int i1, int i2 )
{
  blitcols[0] = i1;
  blitcols[1] = i2;
}
void _small_char( int x, int y, int fg, unsigned int c )
{
  int dum;
  static char s[]=" ";

  s[0] = c;
  _vst_font( 1, 0 );
  _vst_point( 8, &dum, &dum, &dum, &dum );
  _vst_color( xcols[fg] );
  _vst_alignment( 0, 5 );
  _vst_effects(0);
  v_gtext( vdi_hand, x, y, s );
  if( fg != 1 ) _vst_color(1);
}
void _form_copy( int flag, char *buf, Rect *r )
{
  MFDB fdb0;
  Rect r0, r1;
  int pts[8];
  
  fdb0.fd_addr = (void *)buf;
  fdb0.fd_h = r->h;
  fdb0.fd_wdwidth = ((fdb0.fd_w = r->w+2)+15) >> 4;
  fdb0.fd_nplanes = _graphics.vplanes;
  fdb0.fd_r1 = fdb0.fd_r2 = fdb0.fd_r3 = 0;
  fdb0.fd_stand = 1;
  r0 = r1 = *r;
  if( flag ) r0.x = r0.y = 0;
  else r1.x = r1.y = 0;
  pts[2] = (pts[0] = r0.x) + r0.w;
  pts[3] = (pts[1] = r0.y) + r0.h - 1;
  pts[6] = (pts[4] = r1.x) + r1.w;
  pts[7] = (pts[5] = r1.y) + r1.h - 1;
  _x1y1val( 0, 0, _graphics.v_x_max, _graphics.v_y_max );
  _set_clip( vxarr, 1 );
  _hide_mouse();
  vro_cpyfm( vdi_hand, 3, pts, flag?&fdb0:&fdb2, flag?&fdb2:&fdb0 );
  _show_mouse();
}
void _init_scrptr(void)
{
  ft_scr_ptr = (long)Logbase();
}

int char_xlate[256] = { -256,
    0x0127, 0x0124, 0x0126, 0x0125, 0x0150, 0x022F, 0x0161, 0x012E,
    0x01F9, 0x001F, 0x014A, 0x001F, 0x001F, 0x020A, 0x020B, 0x0010,
    0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018,
    0x0019, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x0000,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
    35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
    67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
    83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
    0x013A, 0x0094, 0x00D7, 0x00FC, 0x0102, 0x0100, 0x0104, 0x0075,
    0x0095, 0x00F8, 0x00F6, 0x00FA, 0x00EC, 0x00EE, 0x00F0, 0x00FF,
    0x0071, 0x00FB, 0x0076, 0x0072, 0x00CB, 0x00CD, 0x00C9, 0x00D5,
    0x00D3, 0x00DD, 0x00CE, 0x00D8, 0x0062, 0x0061, 0x0112, 0x0079,
    0x0063, 0x0106, 0x00F2, 0x00C7, 0x00D1, 0x00C3, 0x00C4, 0x001F,
    0x001F, 0x007F, 0x0136, 0x0135, 0x0099, 0x0097, 0x0080, 0x007D,
    0x007E, 0x00FE, 0x00CF, 0x0073, 0x0077, 0x0078, 0x0074, 0x0103,
    0x00FD, 0x00D0, 0x0088, 0x0082, 0x006C, 0x0117, 0x01F7, 0x01F8,
    0x014E, 0x0113, 0x0114, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
    0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
    0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F,
    0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x006E, 0x001F,
    0x012F, 0x013F, 0x0140, 0x0139, 0x0146, 0x013C, 0x0147, 0x0145,
    0x0148, 0x013D, 0x0144, 0x013E, 0x0141, 0x01AC, 0x0149, 0x012A,
    0x012B, 0x011F, 0x011E, 0x0122, 0x0123, 0x012C, 0x012D, 0x011D,
    0x0121, 0x01F3, 0x01F4, 0x0155, 0x012E, 0x021F, 0x00A0, 0x00A1,
    0x00E6 };

int txt_adv( int ch )
{
  int advx, advy, remx, remy, w;

  vqt_advance( vdi_hand, ch, &advx, &advy, &remx, &remy );
  w = advx; /* + ((unsigned)remx>=16384/2); */
  remx >>= 12;
  if( remx==1 ) w++;
  else if( remx==2 ) w--;
  return w;
}

void text_2_arr( unsigned char *str, int *num )
{
  int *arr = text_arr, n;
  char *off = text_off, o;

  if( *num > sizeof(_VDIParBlk.intin)/2 ) *num = sizeof(_VDIParBlk.intin)/2;
  n = *num;
  if( !_graphics.NVDI3 ) while( n-- && (*arr++ = char_xlate[*str++]+first_char) >= 0 );
  else
    for( ; n--; str++ )
    {
      if( !true_type )
      {
        if( (*arr++ = char_xlate[*str]+first_char) < 0 ) break;
      }
      else if( (*arr++ = *str) == 0 ) break;
      if( (o = char_off[*str]) == (char)0x7f ) o = char_off[*str] =
          (lw - txt_adv(*(arr-1)))>>1;
      *off++ = o;
    }
}
int get_extent( char *str, int len, WIND_FONT *f )
{
  int pts[8];

  if( _graphics.is_scalable&2 ) return f->w * strlen(str);
  else if( _graphics.is_scalable&1 )
      vqt_f_extent16( vdi_hand, text_arr, len, pts );
  else vqt_extent( vdi_hand, str, pts );
  return pts[4];
}
WIND_FONT *get_font( int fnum )
{
  static WIND_FONT temp = { 1, 0, 0, 0, 0 };
  int dum;
  WIND_FONT *f;
  
  if( fnum >= 0 )
  {
    temp.size = fnum + 8;
    f = &temp;
  }
  else f = &fonts[FONT_ICON-fnum];
  _vst_font( f->id, f->scale );
  _vst_point( f->size, &dum, &dum, &f->w, &f->h );
  return f;
}
int _vqt_extent( unsigned char *str, int fnum )
{
  int len, ret;
  WIND_FONT *f;  

  f = get_font(fnum);
  if( _graphics.is_scalable&1 )
  {
    len = strlen(str);
    _vst_charmap( 0 );
    text_2_arr( str, &len );
    ret = get_extent( str, len, f );
    _vst_charmap( 1 );
  }
  else ret = get_extent( str, 0, f );
  return ret;
}
int centered( char *str, int x, int len, WIND_FONT *f )
{
  return x - (get_extent(str,len,f)>>1);
}
void text_box( int x, int y, int len, int *arr )
{
  arr[2] = (arr[0] = x) + lw*len-1;
  arr[3] = (arr[1] = y) + lh-1;
}
int txt_baseline(void)
{
  int dum, dist[5], eff[3];
  
  vqt_fontinfo( vdi_hand, &dum, &dum, dist, &dum, eff );
  return dist[4];
}
void ftext16_mono( int x, int y, int len )
{
  int arr[4], und, *txt;
  char *t;

  if( _graphics.NVDI3 )
  {
    txt = text_arr;
    text_box( x, y, len, arr );
    if( wr_mode==MD_REPLACE )
    {
      vsf_color( vdi_hand, f_color=WHITE );
      vr_recfl( vdi_hand, arr );
    }
    if( effect&8 )	/* underline */
    {
      vst_effects( vdi_hand, effect&~8 );
      und = 1;
    }
    else und = 0;
    t = text_off;
    while( --len>=0 )
    {
      v_ftext16( vdi_hand, x + *t++, y, txt++, 1 );
      x += lw;
    }
    if( und )
    {
      _vsl_type(SOLID);
      _vsl_color(BLACK);
      arr[3] = arr[1] += txt_baseline()+1;
      v_pline( vdi_hand, 2, arr );
    }
  }
  else v_ftext16_mono( vdi_hand, x, y, text_arr, len, lw );
}
void _gtext( int x, int y, unsigned char *str, int fnum, int center, int mode, int color )
{
  int len;
  WIND_FONT *f;
  
  _set_wmode(mode);
  f = get_font(fnum);
  _vst_color( color );
  _vst_alignment( 0, 5 );
  _vst_effects(0);
  if( !(_graphics.is_scalable&1) )
  {
    if( center ) x = centered( str, x, 0, f );
    v_gtext( vdi_hand, x, y, str );
  }
  else
  {
    len = strlen(str);
    _vst_charmap( 0 );
    text_2_arr( str, &len );
    if( center ) x = centered( str, x, len, f );
    if( !(_graphics.is_scalable&2) ) v_ftext16( vdi_hand, x, y, text_arr, len );
    else ftext16_mono( x, y, len );
    _vst_charmap( 1 );
  }
}

void _gr_linebox( int *arr )
{
  _vsl_type( 7 );
  v_pline( vdi_hand, 4, arr );
}

void _put_pixel( int x, int y, int col )
{
  int xy[4];

  xy[0] = xy[2] = x;
  xy[1] = xy[3] = y;
  vsl_color( vdi_hand, xcols[col] );
  _vsl_type( 7 );
  v_pline( vdi_hand, 2, xy );
}

char mouse_but;
int new_but( int buttn )
{
  return(mouse_but = buttn);
}
void _set_butv( int flag )
{
  static int (*old)(void);
  static char level=0;
  int (*vec)(void);
  
  if( !flag )
  {
    if( !--level ) vex_butv( vdi_hand, old, &vec );
  }
  else
  {
    if( !level++ )
    {
      mouse_but = 0;
      vex_butv( vdi_hand, new_but, &old );
    }
  }
  _graphics.have_butv = level;
}
void _wait_mbut(void)
{
  while(mouse_but);
}
int _get_mbut(void)
{
  return(mouse_but);
}
void _set_clip( int *arr, int mode )
{
  static int off[] = { 0, 0, 32767/2, 32767/2 };
  
  if( !mode ) arr = off;
  if( clip_on != mode || *(long *)&clip_arr[0] != *(long *)&arr[0] ||
      *(long *)&clip_arr[2] != *(long *)&arr[2] )
  {
    *(long *)&clip_arr[0] = *(long *)&arr[0];
    *(long *)&clip_arr[2] = *(long *)&arr[2];
    vs_clip( vdi_hand, clip_on=mode, arr );
  }
}
void _hide_mouse(void)
{
  v_hide_c(vdi_hand);
}
void _show_mouse(void)
{
  v_show_c( vdi_hand, 1 );
}
#pragma warn -par
int save_x, save_y;
int my_mov( int x, int y )
{
  y = save_y;
  return x = save_x;
}
#pragma warn +par
int get_pixel( int x, int y )
{
  MFDB pixel = {0L, 16, 1, 1, 0, 1, 0, 0, 0};
  int buf[32], i, *b, pxy[8], out;
  
  pixel.fd_addr = buf;
  pixel.fd_nplanes = _graphics.vplanes;		/* 004 */
  for( i=16, b=buf; --i>=0; )
    *((long *)b)++ = 0L;
  pxy[0] = pxy[2] = x;
  pxy[1] = pxy[3] = y;
  *(long *)&pxy[4] = *(long *)&pxy[6] = 0L;
  vro_cpyfm( aes_vdi_hand, S_ONLY, pxy, &fdb2, &pixel );
  for( i=16, b=buf, out=0; --i>=0; )
  {
    out <<= 1;
    if( *b++ ) out |= 1;
  }
  return out; 
}

void _reset_mouse( int grf_mouse( int num, MFORM *addr ) )
{
  int (*old_mov)();
  int pel, opel, col, ocol, i;
  static MFORM mform = { 0, 0, 1, 0, 0, 1<<15 };
  
  vq_mouse( aes_vdi_hand, &pel, &save_x, &save_y );
  vex_motv( aes_vdi_hand, my_mov, &old_mov );
  vq_mouse( aes_vdi_hand, &pel, &save_x, &save_y );
  v_show_c( aes_vdi_hand, 0 );
  v_hide_c( aes_vdi_hand );
  ocol = get_pixel( save_x, save_y );
  if( (col=ocol+1) >= (1L<<(long)_graphics.vplanes) ) col=0;	/* 004: made long */
  mform.mf_fg = mform.mf_bg = col;
  (*grf_mouse)( 255, &mform );
/*%  vsc_form( aes_vdi_hand, mform ); */
  for( i=18; --i>=0; )
  {
    col = get_pixel( save_x, save_y );
    if( col!=ocol ) break;
    (*grf_mouse)( M_ON, 0L );
  }
  if( i<0 ) for( i=8; --i>=0; )
    (*grf_mouse)( M_OFF, 0L );
  vex_motv( aes_vdi_hand, old_mov, &old_mov );
  (*grf_mouse)( ARROW, 0L );
}
void gr_box(void)
{
  vr_recfl( vdi_hand, vxarr );
}
void gr_hline(void)
{
  vxarr[3] = vxarr[1];
  _vsl_type( 7 );
  v_pline( vdi_hand, 2, vxarr );
}
void _graph_exit(void)
{
  no_fonts();
  v_clsvwk(vdi_hand);
}
/*
void arb_box( int intcol, int inter, int style, int writ, Rect *r )
{
  int arr[4];
  
  vsf_color( vdi_hand, intcol );
  vsf_interior( vdi_hand, inter );
  vsf_style( vdi_hand, style );
  _set_wmode( writ );
  if( r )
  {
    arr[2] = (arr[0] = r->x+1) + r->h-2;
    arr[3] = (arr[1] = r->y+1) + r->w-2;
    vr_recfl( vdi_hand, arr );
  }
}
*/

char check_mono( char scale )
{
  int j, min, max, maxw, dum[5], wid;

  if( !scale || !_graphics.speedo )
  { /* check to see that this bitmapped or FSM font is monospaced */
    vqt_fontinfo( vdi_hand, &min, &max, dum, &maxw, dum );
    if( (unsigned)min<=1 && (unsigned)max>=254 )	/* 003: only go up to 254, to avoid NVDI 3 bug */
    {
      /* go from the first to last character, making sure that each one is
         the full cell width. */
      maxw = -1;
      for( j=1; j<=254; j++ )
      {
        vqt_width( vdi_hand, j, &wid, dum, dum );
        if( maxw>=0 && wid != maxw ) break; /* stop at the first odd char */
        maxw = wid;
      }
      if( j==255 ) return scale | 2;
    }
    return scale & ~2;
  }
  return scale | 2;	/* speedo, so font can be mono */
}

void vst_load(void)
{
  if( !have_fonts && _graphics.has_gdos )
  {
    _graphics.total_fonts = work_out[10] + vst_load_fonts( vdi_hand, 0 );
    have_fonts = 1;
  }
}

void _load_fonts( WIND_FONT *infonts, int size )
{	/* size==0 : just load all fonts */
  int i, j, id, dum;
  char tempname[33], font_ok[10]/*002*/;
  
  if( size ) fonts = infonts;
  for( i=0; i<size; i++ )
    font_ok[i] = 0;
  for( i=0; i<size; i++ )	/* is there a non-system font? */
    if( infonts[i].id != 1 ) break;
  if( !size || i!=size )
  {
    vst_load();
    if( !size ) return;
    for( i=1; i<=_graphics.total_fonts; i++ )
    {
      tempname[32] = 0;
      id = vqt_name( vdi_hand, i, tempname );
      for( j=0; j<size; j++ )
        if( id==infonts[j].id )
        {
          font_ok[j] = 1;
          infonts[j].scale = tempname[32] && _graphics.scalable;
        }
    }
  }
  for( i=j=0; --size>=0; infonts++, j++ )
  {
    if( font_ok[j] ) i++;
    else
    {
      infonts->id = 1;
      infonts->scale = 0;
    }
    gr_font = -1;
    _vst_font( infonts->id, infonts->scale );
    infonts->size = _vst_point( infonts->size, &dum, &dum, &infonts->w,
        &infonts->h );
    infonts->scale = check_mono( infonts->scale );
  }
  gr_font = -1;
  if( !i ) no_fonts();
}

int _vst_font( int id, int scale )
{
  int dum[5];
  
  if( gr_font != id )
  {
    if( id!=1 ) vst_load();
    if( (gr_font = vst_font( vdi_hand, id )) != id ) scale &= ~1;
    txt_point = -1;
  }
  if( (_graphics.is_scalable = scale)&1 )	/* 002: find first ASCII value */
      first_char = id<0 ? 32 : 0;
  true_type = _graphics.NVDI3 && id&0x4000;	/* 003 */
  return gr_font;
}

int _vst_point( int point, int *txt_chw,
                int *txt_chh, int *txt_clw, int *txt_clh )
{
  int i;
  char *co;

  if( point != txt_point )
    if( !_graphics.scalable || !(_graphics.is_scalable&1) ) txt_point =
        vst_point( vdi_hand, point, &hw, &hh, &lw, &lh );
    else
    {
      if( _graphics.NVDI3 )
        for( i=sizeof(char_off), co=char_off; --i>=0; )
          *co++ = 0x7f;
/*      memset( char_off, 0x7f, sizeof(char_off) );*/
      txt_point = vst_arbpt( vdi_hand, point, &hw, &hh, &lw, &lh );
      hw = txt_adv('M');
    }
  *txt_chw = hw;
  *txt_chh = hh;
  *txt_clw = lw;
  *txt_clh = lh;
  return txt_point;
}

void _vst_color( int color )
{
  if( color != txt_color ) vst_color( vdi_hand, txt_color=color );
}

void _vst_alignment( int hor, int vert )
{
  int dum;
  
  if( hor!=halign || vert!=valign ) vst_alignment( vdi_hand,
      halign=hor, valign=vert, &dum, &dum );
}

void _vst_effects( int eff )
{
  if( effect != eff ) vst_effects( vdi_hand, effect=eff );
}

void _vsl_type( int type )
{
  if( ln_type != type ) vsl_type( vdi_hand, ln_type=type );
}

void _vsf_style( int style )
{
  if( style != f_style ) vsf_style( vdi_hand, f_style=style );
}

void _vsf_interior( int style )
{
  if( style != f_inter ) vsf_interior( vdi_hand, f_inter=style );
}

void _vsl_color( int color )
{
  if( color != ln_color ) vsl_color( vdi_hand, ln_color=color );
}

void _vsl_ends( int start, int end )	/* 003 */
{
  if( l_start != start || l_end != end )
      vsl_ends( vdi_hand, l_start=start, l_end=end );
}

void _vsl_width( int width )	/* 003 */
{
  if( width != lwidth ) vsl_width( vdi_hand, lwidth=width );
}

void _vst_charmap( int mode )		/* 003 */
{
  if( true_type ) mode = 1;
  if( mode != ch_mode ) vst_charmap( vdi_hand, ch_mode=mode );
}

/* NeoDesk 3.02 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/
#include "neocntrl.h"
#include "tos.h"
#include "stdlib.h"
#include "string.h"
#include "mwclinea.h"
#include "aes.h"
#include "xwind.h"
#include "..\neodesk.h"
#include "..\neocommn.h"

#define _MCH_COOKIE 0x5F4D4348L
#define _VDO_COOKIE 0x5F56444FL
#define _SND_COOKIE 0x5F534E44L
#define _CPU_COOKIE 0x5F435055L
#define MAGC_COOKIE 0x4d674d63L
#define IDT_cookie      0x5F494454L     /* "_IDT" */
#define Hz_200		*(long *)0x4ba
#define CWIND_TYPE	NAME|CLOSER|MOVER

#define PHYSTOP     *((long *) 0x42E)
#define vblsem     ((int *) 0x452L)
#define _vblqueue  (*(long **)0x456L)
#define nvbls      (*(int *)0x454)
#define _sysbase   (*(long *)0x4F2L)
#define pallreg    ((int *) 0xff8240L)
#define stacy_state *((char *)0xff827EL)
#define falc_mode  *((char *)0xff8267L)
#define sshiftmod  (*(char *)0x44CL)
#define palmode    ((int *) 0x448L)
#define conterm    (*((char *) 0x484L))
#define OS_version (*(int *)((*(long *)0x4F2)+2))
#define MWMask	   (*(int *)0xff8924)
#define MWData	   (*(int *)0xff8922)
#define Getblit()  Blitmode(-1)
#define Setblit(a) Blitmode(a)

extern void cdecl gtext( int x, char *s, unsigned char *la, int h, int pl,
    int nxpl, int plmul );
extern int linea0(void);
extern long clock_tst(void);
void cdecl dopf(char *buf, char *fmt, unsigned int *ap);
char *pn( char *b, unsigned long n, int base, int w, int sign, int pad, int lj);
void set_time(void);
void slide_key( int j );
void set_colors(void);
void set_key( int index, int num );
void get_lc(void);
void set_lc(void);
void reset_keys(void);
void time_in_wind(void);
void get_system(void);
void get_colors(void);
void put_str( char *s /*, int flag*/ );
int make_form( int index, int expand, int edit );
void current_color(int flg);
int is_num(int c);
void swap( char *ptr1, char *ptr2 );
void to_Europe( char *ptr );
void redraw_color(void);
void adjust_hour(void);
void wait(void);

long savecol[MAX_COLOR];
unsigned int hr, min, sec, year, mon, day, ampm=1, failed=1,
    savmins=5, savsecs, clockon, max_w, max_h, screen_size, flopr[2],
    saveon, current, color, key_pause[2], fht, apid,
    bal_vol = (0x14<<8)|0x20, treb_bass = (6<<4)|6, idt_fmt;
unsigned char str[14], curcol[3], get_time, bliton, clickon, sk_ign,
           bellon, pm, planes, top_wind, dc_rate, *fdat, ifmt[]="%d",
           is_121, no_clock, is_mono, gt_512, oldcaps=-1, TT_mono,
           space[]=" ", last_cnt=-1, new_SND, is_MSTe, is_030, is_falc,
           has_Blit, chips, in_slider, has_clut, has_Geneva, falc_vid,
           has_magcmac;
extern char has_adspd;
char snd_dat[] = {
0x00,0x26,0x45,0x57,0x58,0x4B,0x33,0x19,0x04,0xFC,0x00,0x12,0x2F,0x4F,0x6A,0x79,
0x78,0x67,0x49,0x24,0x00,0xE6,0xD8,0xDA,0xE9,0x00,0x15,0x25,0x28,0x1B,0x01,0xDF,
0xB9,0x9B,0x88,0x87,0x95,0xAF,0xCF,0xEC,0x00,0x05,0xFD,0xE8,0xCE,0xB7,0xA8,0xA9,
0xB9,0xD7,0xFD,0x23,0x43,0x56,0x59,0x4C,0x35,0x1B,0x06,0xFC,0xFF,0x10,0x2C,0x4C,
0x68,0x78,0x79,0x69,0x4C,0x27,0x03,0xE8,0xD9,0xD9,0xE8,0xFE,0x14,0x24,0x28,0x1D,
0x03,0xE1,0xBC,0x9D,0x89,0x86,0x93,0xAD,0xCC,0xEA,0xFF,0x05,0xFE,0xEA,0xD0,0xB8,
0xA9,0xA8,0xB7,0xD4,0xFA,0x21,0x41,0x55,0x59,0x4E,0x37,0x1D,0x07,0xFC,0xFF,0x0F,
0x2A,0x4A,0x66,0x78,0x79,0x6B,0x4E,0x2A,0x06,0xE9,0xDA,0xD9,0x00 },
    *sound_data=snd_dat;
long scan_off;
int num_colors, edge_col, bigscr, dum, w_handle=-1, vdi_hand;
Rect windo, center, rsrect;
OBJECT *form, *rsform;
LoadCookie *lc;
char **ctrl_title, is_CD;

typedef union
{
    unsigned long l;
    struct
    {
      unsigned year:7;
      unsigned mon:4;
      unsigned day:5;
      unsigned hr:5;
      unsigned min:6;
      unsigned sec:5;
    } t;
} KTIME;

void spf( char *buf, char *fmt, ... ) {
  dopf(buf, fmt, (unsigned int *)&... );
}
void cdecl dopf(char *buf, char *fmt, unsigned int *ap) {
  char **pp, *ps, pad, lj, sign, larg;
  unsigned long n, *lp;
  register int c, w;
  
  while( (c = *fmt++) != 0 ){
    if (c == '%') {
      if( (lj = ((c=*fmt++) == '-')) != 0 ) c = *fmt++;
      if( c == '0' ){
        pad = '0';
        c = *fmt++;
      }
      else pad = ' ';
      w = 0;
      sign = 1;
      while (c >= '0' && c <= '9'){
        w = w*10 + c-'0';
        c = *fmt++;
      }
      if( (larg = (c == 'l')) != 0 ) c = *fmt++;
      switch (c) {
        case 'c': *buf++ = *ap++; 
            break;
        case 's': pp = (char **) ap;
            ps = *pp++;
            w -= strlen(ps);
            if( !lj ) while (w-- > 0) *buf++ = pad;
            ap = (unsigned int *) pp;
            while (*ps) *buf++ = *ps++;
            if( lj ) while (w-- > 0) *buf++ = pad;
            break;
        case 'x': sign=0; goto do_pn;
        case 'X': sign=0;
        case 'D': larg++;
        case 'd': case 'o':
do_pn:      if (larg) {
            lp = (unsigned long *)ap;
            n = *lp++;
            ap = (unsigned int *)lp;
            }
            else n = (long)(signed) *ap++;
            buf = pn(buf, n, c, w, sign, pad, lj);
            break;
        default:  *buf++ = c; 
            break;
      }
    }
    else  *buf++ = c;
  }
  *buf = 0;
}
char *pn( char *b, unsigned long n, int base, int w, int sign, int pad, int lj )
{
  int i;
  char nb[20];
  switch (base) {
  case 'o':
    base = 8;
    break;
  case 'x':
  case 'X':
    base = 16;
    break;
  default:
    base = 10;
  }
  i = 0;
  if( !n ) nb[i++] = '0';
  else
  {
    if( sign && (n&0x80000000L) ){
      n = (unsigned long)(-(long)n);
    } else sign = 0;
    for (; n; n /= base)
      nb[i++] = "0123456789ABCDEF"[n%base];
    if( sign ) nb[i++] = '-';
  }
  w -= i;
  if( !lj ) while (w-- > 0) *b++ = pad;
  else while (w-- > 0) nb[i++] = pad;
  while (i--) *b++ = nb[i];
  return b;
}
/**********************************************************************/
void pak_rgb( int *rgb, unsigned char *c )
{
  *c = (*rgb * 255L + 499) / 1000;
}
void unpak_rgb( unsigned char *c, int *rgb )
{
  *rgb = (*c * 1000L + 127) / 255;
}
long lsetcolor( int num, long val )
{
  int rgb[4], old[4], i;
  long old_l;
  static char vq_mode=1;
  
  for(;;)
  {
    vq_color( vdi_hand, num, vq_mode, old );
    if( vq_mode && (old[0]>1000 || old[1]>1000 || old[2]>1000) ) vq_mode=0;
    else break;
  }
  old_l = 0L;
  for( i=1; i<4; i++ )
  {
    pak_rgb( old+(i-1), (unsigned char *)&old_l+i );
    unpak_rgb( (unsigned char *)&val+i, rgb+(i-1) );
  }
  if( val>=0 ) vs_color( vdi_hand, num, rgb );
  return old_l;
}
/********
int _setcolor( int num, int val )
{
  int i;
  
  if( TT_mono )
  {
    i = xbios( 83, 255-num, val<0 ? val : ((val&0x888)>>3)|((val&0x777)<<1) );
    return( ((i&0x111)<<3)|((i&0xEEE)>>1) );
  }
  return( Setcolor( num, val ) );
}**********/
/**********************************************************************/
void clear_clock(void)
{
  long stack;
  
  stack = Super(0L);
  put_str( "             " );
  gtext( 0, space, fdat, fht, 1, 0, 1 );
  Super((void *)stack);
}
/**********************************************************************/
void finish(void)
{
  form_dial( FMD_FINISH, 0, 0, 0, 0, rsrect.x, rsrect.y, rsrect.w, rsrect.h );
}
/**********************************************************************/
void reobjo( int num )
{
  objc_draw( rsform, num, 8, rsrect.x, rsrect.y, rsrect.w, rsrect.h );
}
void reobj( int num )
{
  objc_draw( form, num, 1, center.x, center.y, center.w, center.h );
}
/**********************************************************************/
void pause(void)
{
  int j;
  
  for( j=0; j<7; j++ )
    Vsync();
}
/********************************************************************/
void ack( int handl )
{
  int buf[8];
  
  buf[0] = DUM_MSG;
  buf[1] = apid;
  appl_write( handl, 16, buf );
}
/***********************************************************************/
int find_obj( int x, int y )
{
  return( objc_find( form, 0, MAX_DEPTH, x, y ) );
}
/********************************************************************/
void vclock(void)
{
  register int i, n;
  register char c, *ptr;
  static char clock_last;
  long off;
  
  if( !lc ) return;
  if( (c=*lc->clock_cnt) != last_cnt )
  {
    last_cnt = c;
    clock_last++;
    if( ++sec == 60 )
    {
      sec = 0;
      if( ++min == 60 )
      {
        min = 0;
        if( ++hr == 12 ) pm = !pm;
        else if( ampm && hr == 13 ) hr = 1;
        else if( hr == 24 ) hr = 0;
        get_time = 1;
      }
    }
    if( clockon && *(lc->clock_temp) )
    {
      ptr = str + (ampm ? 0 : 3);
      *ptr++ = **lc->kbshift&0x10 ? 'C' : ' ';
      *ptr++ = ' ';
      *ptr++ = (c=hr/10)!=0 ? c+'0' : ' ';
      *ptr++ = (hr%10) + '0';
      *ptr++ = ':';
      *ptr++ = (min/10) + '0';
      *ptr++ = (min%10) + '0';
      *ptr++ = ':';
      *ptr++ = (sec/10) + '0';
      *ptr++ = (sec%10) + '0';
      if( ampm )
      {
        *ptr++ = ' ';
        *ptr++ = pm ? 'p' : 'a';
        *ptr++ = 'm';
      }
      *ptr = '\0';
      put_str( str );
    }
  }
  else if( clock_last )
  {
    clock_last = 0;
    if( saveon && ++*(lc->savcnt)>=savsecs )
    {
      if( *lc->saver )
      {
        if( !*(lc->scrsav) ) (**lc->saver)(1);
      }
      else
      {
        if( !falc_vid || falc_mode&(1<<7) )	/* 003: ST or Falcon in ST mode */
        {
          n = (MAX_COLOR>>1);
          ptr = (char *)0xFF8240L;
          off = 0x11111111L;
        }
        else
        {
          n = (MAX_SAVECOL>>1);
          ptr = (char *)0xFF9800L;
          off = 0x04040004L;
        }
        if( !*(lc->scrsav) )
        {
          for( i=0; i<n; i++ )
            (*lc->pall)[i] = *((long *)ptr + i);
          stacy_state = 0xC;
        }
        if( is_mono ) *pallreg ^= 1;
        else if( !*(lc->scrsav) )
          for( i=0; i<n; i++ )
            *((long *)ptr + i) = off;
      }
      *(lc->savcnt) = 0;
      *(lc->scrsav) = 1;
    }
  }
/*else if( (c=**lc->kbshift&0x10) != oldcaps )
      gtext( 0, (oldcaps=c)!=0 ? "C" : space, fdat, fht, 1, 0, 1 ); */
}
/********************************************************************/
void name_it( int w_handle, char *ptr )
{
  static char *cur;
  
  if( !ptr ) ptr = *ctrl_title;
  if( cur != ptr )
  {
    cur=ptr;
    if( ptr != (char *)-1L ) wind_set( w_handle, WF_NAME, ptr, 0, 0 );
  }
}
/********************************************************************/
int Blitmode( int mode )
{
  if( is_falc ) return 0;
  return xbios( 64, mode );
}
int nf_0(void)	{ return 0; }
int nf_cl(void)	{ return clickon; }
int nf_be(void)	{ return bellon; }
int nf_pa(void)	{ return !is_mono || planes!=1; }
int nf_ch(void)
{
  if( !chips ) return -1;
  if( chips==1 && has_Blit ) return bliton;
  return 2;
}
void choose_name( int w_handle, int num )
{
  int i;
  char **p, *ptr;
  static int (*name_func[])(void) = { nf_cl, nf_be, nf_0, nf_pa,
      nf_0, nf_ch, nf_0, nf_0 };
  
  rsrc_gaddr( 15, num+LABEL1, &p );
  while( (ptr=strrchr(*p,'|')) != 0 ) *ptr=0;
  if( (i=(*name_func[num])()) < 0 ) ptr="";
  else for( ptr=*p; --i>=0; )
    ptr += strlen(ptr)+1;
  name_it( w_handle, ptr );
}
/********************************************************************/
void copyright(int num)
{
  make_form( CABOUT, num, 0 );
  finish();
}
/********************************************************************/
void floprate( int chng )
{
  long stack;
  int *iptr;
  
  stack = Super(0L);
  switch( OS_version )
  {
    case 0x100:
      iptr = (int *)0xa08;
      goto seek;
    case 0x102:
      iptr = (int *)0xa4e;
seek: if( chng )
      {
        *iptr = flopr[0];
        *(iptr+2) = flopr[1];
      }
      else
      {
        flopr[0] = *iptr;
        flopr[1] = *(iptr+2);
      }
      Super((void *)stack);
      break;
    default:
      Super((void *)stack);
      if( chng )
      {
        Floprate( 0, flopr[0] );
        Floprate( 1, flopr[1] );
      }
      else
      {
        flopr[0] = Floprate( 0, -1 );
        flopr[1] = Floprate( 1, -1 );
      }
  }
}
/********************************************************************/
char constat( int chng )
{
  long stack;
  char c=0;
  
  stack = Super((void *)0L);
  if( !chng ) c = conterm;
  else conterm = (conterm & 0xFA) | (bellon<<2) | clickon;
  Super((void *)stack);
  if( !chng )
  {
    clickon = c&1;
    bellon = (c&4) > 0;
    if( !failed && !new_SND )
    {
      form[CLICK].ob_state = DISABLED*!clickon;
      form[BELL].ob_state = DISABLED*!bellon;
    }
  }
  return c;
}
/********************************************************************/
int f_alert( int buttn, int index )
{
  char **ptr;
  
  rsrc_gaddr( 15, index, &ptr );
  return( form_alert( buttn, *ptr ) );
}
/********************************************************************/
void lock_wind( int lock )
{
  wind_update( lock ? BEG_UPDATE : END_UPDATE );
  wind_update( lock ? BEG_MCTRL : END_MCTRL );
}
int make_form( int index, int expand, int edit )
{
  register unsigned int i, j;
  int x, y, st;
  
  rsrc_gaddr( 0, index, &rsform );
  form_center( rsform, &rsrect.x, &rsrect.y, &rsrect.w, &rsrect.h );
  if( expand )
  {
    objc_offset( form, expand, &x, &y );
    rsform[0].ob_x = rsrect.x = (form[0].ob_width-rsrect.w>>1) + form[0].ob_x;
    rsform[0].ob_y = rsrect.y = (form[0].ob_height-rsrect.h>>1) + form[0].ob_y;
    graf_growbox( x, y, form[expand].ob_width, form[expand].ob_height, 
        rsrect.x, rsrect.y, rsrect.w, rsrect.h );
  }
  reobjo(0);
  lock_wind(1);
  for(;;)
  {
    j = 0;
    if( (i = form_do( rsform, edit )) & ((unsigned)1<<15) )
    {
      j++;
      i &= 0x7FFF;
    }
    st = rsform[i].ob_state;
    if( expand == PALLETTE || expand==SOUND )
    {
      lock_wind(0);
      return(i);
    }
    else if( expand == MOUSEI )
      if( j )
      {
        if( !(rsform[i].ob_flags & (1<<11)) )
        {
          objc_change( rsform, i, 0, rsrect.x, rsrect.y, rsrect.w, rsrect.h,
              st|SELECTED, 1 );
          Vsync();
          Vsync();
          Vsync();
          objc_change( rsform, i, 0, rsrect.x, rsrect.y, rsrect.w, rsrect.h,
              st&~SELECTED, 1 );
          wait();
        }
      }
      else if( rsform[i].ob_flags&(1<<11) && i != dc_rate+MSSLOW )
      {
        objc_change( rsform, dc_rate+MSSLOW, 0, rsrect.x, rsrect.y, rsrect.w, 
            rsrect.h, 0, 1 );
        objc_change( rsform, i, 0, rsrect.x, rsrect.y, rsrect.w, rsrect.h,
            SELECTED, 1 );
        evnt_dclick( dc_rate=i-MSSLOW, 1 );
      }
    if( rsform[i].ob_flags & SELECTABLE )
        objc_change( rsform, i, 0, rsrect.x, rsrect.y, rsrect.w, rsrect.h,
        st&~SELECTED, 1 );
    if( expand == COPYRT || rsform[i].ob_flags & (EXIT|(1<<10)) ) break;
  }
  lock_wind(0);
  return(i);
}
/********************************************************************/
void put_str( char *s /* , int flag */ )
{
/*long stack;*/
  register int i, j, m, p;
  
/*if( flag ) stack = Super((void *)0L);*/
  if( (p = is_CD ? bigscr : VPLANES) <= 16 )	/* 003: conditional */
    for( j=0; ;j++ )
      if( (1<<j) == p )
      {
        m=j+1;
        break;
      }
  if( (i=bigscr)<0 )
    if( (i=sshiftmod) < 2 ) i = !i ? 4 : 2;
    else i = p;
  gtext( max_w - (ampm?104:80), ampm?s:s+3, fdat, fht, i, (i-1)<<1, m );
/*if( flag ) Super( (void *)stack );*/
}
/********************************************************************/
void snd_word( int i )
{
  long l;
  
  MWMask = 0x7ff;
  while( MWMask != 0x7ff );
  l = Hz_200 + 3;
  MWData = i;
  while( Hz_200<l );
}
void setsnd(void)
{
  int i;
  
  snd_word( 0x4c0+(char)bal_vol );
  if( (i=bal_vol>>8) > 0x14 ) i = 0x14;
  snd_word( 0x500+i );
  if( (i = 0x28-(bal_vol>>8)) > 0x14 ) i = 0x14;
  snd_word( 0x540+i );	/* 004: was 0x580 */
  snd_word( 0x480+((treb_bass>>4)&0xf) );
  snd_word( 0x440+(treb_bass&0xf) );
}
void set_sound(void)
{
#ifndef NO_SND
  if( new_SND ) Supexec((long (*)())setsnd);
#endif
}
/********************************************************************/
void read_time(void)
{
  KTIME t;
  
  t.l = Gettime();
  sec = t.t.sec << 1;
  min = t.t.min;
  hr = t.t.hr;
  day = t.t.day;
  mon = t.t.mon;
  year = t.t.year + 80;
  if( hr>23 || min>59 || sec>59 || year<80 || year>199 || mon<=0 || mon>12 ||
      day<=0 || day>31 ) get_system();
  else adjust_hour();
  if( no_clock ) set_time();
  get_time = 0;
  if( lc ) --*(lc->clock_cnt);
}
void adjust_hour(void)
{
  pm = 0;
  if( ampm )
    if( !hr ) hr = 12;
    else if( hr>=12 )
    {
      pm++;
      if( hr>12 ) hr-=12;
    }
}
/********************************************************************/
void get_system(void)
{
  register unsigned int t;
  
  get_time = 0;
  t = Tgettime();
  hr = (t>>11) & 0x1F;
  adjust_hour();
  min = (t>>5) & 0x3F;
  sec = (t&0x1F) << 1;
  t = Tgetdate();
  year = ((t>>9) & 0x7F) + 80;
  mon = (t>>5) & 0x0F;
  day = t & 0x1F;
  time_in_wind();
}
/********************************************************************/
void blit_state(void)
{
  if( !failed && has_Blit && chips==1 ) form[CHIPS].ob_state = DISABLED*!bliton;
}
/********************************************************************/
void reset(void)
{
  bliton = Getblit()&1;
  blit_state();
  get_system();
  reset_keys();
  get_colors();
  dc_rate = evnt_dclick( 0, 0 );
  floprate(0);
  constat(0);
  get_lc();
}
/********************************************************************/
void reset_keys(void)
{
  union
  {
    unsigned int i;
    unsigned char c[2];
  } u;
  
  u.i = Kbrate( -1, -1 );
  key_pause[0] = u.c[0];
  if( (key_pause[1] = u.c[1]) > 50 ) key_pause[1] = 50;
  if( !failed )
  {
    slide_key(0);
    slide_key(1);
  }
}
/********************************************************************/
void set_colors(void)
{
  register int i;
  
  for( i=0; i<num_colors; i++ )
    lsetcolor( i, savecol[i] );
}
/********************************************************************/
void get_colors(void)
{
  register int i;
  
  for( i=0; i<num_colors; i++ )
    savecol[i] = lsetcolor( i, -1L );
}
/********************************************************************/
int set_caches( int bits )
{
  int out=0, i;
  extern int cache;
  long set_MSTe(void), set_030(void), set_adspd(void);
  
  cache = bits;
  if( has_Blit )
  {
    out |= (i=Blitmode(-1))&1;
    if( bits>=0 ) Blitmode( (bits&1) ? (i|1) : (i&0xfffe) );
  }
  else out|=1;
  if( is_MSTe ) out |= Supexec( set_MSTe );
  else out |= 3<<2;
  if( is_030 ) out |= Supexec( set_030 );
  else if( has_adspd ) out |= Supexec( set_adspd );
  else out |= 1<<1;
  return out;
}
/********************************************************************/
void set_blit(void)
{
  if( !has_Blit )
  {
    bliton = 0;
    if( !failed && !chips ) form[CHIPS].ob_state = DISABLED;
  }
  else
  {
    set_caches((set_caches(-1)&0xfffe) | bliton);
    blit_state();
  }
/*  if( !failed )
  {
    form[BLITON+!bliton].ob_state = SELECTED;
    form[BLITON+bliton].ob_state &= ~SELECTED;
  }*/
}
/********************************************************************/
void clksav(void)
{
  if( !failed )
  {
    form[SAVERON+!saveon].ob_state = form[CLOCKON+!clockon].ob_state =
        form[TIMEAMPM+!ampm].ob_state = SELECTED;
    form[SAVERON+saveon].ob_state = form[CLOCKON+clockon].ob_state =
        form[TIMEAMPM+ampm].ob_state = 0;
    spf( form[SAVERMIN].ob_spec.tedinfo->te_ptext, ifmt, savmins );
  }
}
/********************************************************************/
void set_buttons(void)
{
  if( savmins > 9 || savmins < 1 ) savmins = 5;
  savsecs = savmins * 60;
  evnt_dclick( dc_rate, 1 );
  clksav();
  constat(1);
  set_blit();
  set_colors();
  set_key( 0, key_pause[0] );
  set_key( 1, key_pause[1] );
  floprate(1);
  set_lc();
}
/********************************************************************/
void set_color( int index, int num )
{
  int i;
  
  if( num != curcol[index] && num>=0 && num<=255 )
  {
    curcol[index] = num;
/*    _setcolor( color, gt_512 ? ((curcol[0]&1)<<11)|((curcol[0]>>1)<<8)|
        ((curcol[1]&1)<<7)|((curcol[1]>>1)<<4)|((curcol[2]&1)<<3)|
        (curcol[2]>>1) : (curcol[0]<<8)|(curcol[1]<<4)|curcol[2] ); */
    lsetcolor( color, ((long)curcol[0]<<16)|((long)curcol[1]<<8)|curcol[2] );
    spf( rsform[COLNUM+index].ob_spec.free_string, ifmt, num );
    i = COLDRAG+index*4;
    if( !in_slider )
    {
      rsform[i].ob_x = (rsform[COLSLID+index*4].ob_width-rsform[i].ob_width)
          * num / 255;
      reobjo( COLSLID+index*4 );
    }
    if( !has_clut ) reobjo( COL0+color-edge_col );
    reobjo( COLNUM-1 );
  }
}
/********************************************************************/
void set_lc(void)
{
  if( lc )
  {
    lc->saveon = saveon;
    lc->ampm = ampm;
    lc->clockon = clockon;
    lc->savmins = savmins;
  }
}
/********************************************************************/
void get_lc(void)
{
  if( lc )
  {
    saveon = lc->saveon;
    ampm = lc->ampm;
    clockon = lc->clockon;
    savmins = lc->savmins;
    savsecs = savmins*60;
  }
}
/********************************************************************/
void set_key( int index, int num )
{
  int i, max;
  
  max = index ? 50 : 255;
  if( num>max ) num = max;
  if( num<0 ) num = 0;
  i = num != key_pause[index];
  key_pause[index] = num;
  Kbrate( key_pause[0], key_pause[1] );
  if( i && !failed ) slide_key( index );
  pause();
}
/********************************************************************/
void set_slid( int drag, int slid, int max, int num )
{
  if( !in_slider )
  {
    form[drag].ob_x = (long)(form[slid].ob_width-form[drag].ob_width) * num / max;
    if( top_wind && !sk_ign ) reobj( slid );
  }
}
/********************************************************************/
void set_time(void)
{
  unsigned int h;
  KTIME t;
  
  t.l = 0;
  t.t.hr = h = ampm&&pm&&hr<12 ? hr+12 : (ampm&&!pm&&hr==12 ? 0 : hr);
  t.t.min = min;
  t.t.sec = sec>>1;
  t.t.year = year-80;
  t.t.mon = mon;
  t.t.day = day;
  Settime( t.l );
  Tsetdate( ((unsigned)year-80<<9) | (mon<<5) | day );
  Tsettime( ((unsigned)h<<11)|(min<<5)|(sec>>1) );
}
/********************************************************************/
void show_color( int flg )
{
  long col;
  int i, j;
  
  col = lsetcolor( color, -1 );
  for( i=2, j=COLSLID+8; i>=0; i--, j-=4 )
  {
    spf( rsform[COLNUM+i].ob_spec.free_string, ifmt, curcol[i] =
        (unsigned char)col );
    col >>= 8L;
    if( !in_slider ) rsform[j+1].ob_x =
        (rsform[j].ob_width-rsform[j+1].ob_width) * curcol[i] / 255;
  }
  if(flg)
  {
    reobjo( COLNUM-1 );
    if( !in_slider ) redraw_color();
  }
}
/********************************************************************/
void redraw_color(void)
{
  register int i;
  
  for( i=0; i<3; i++ )
    reobjo( COLSLID+i*4 );
}
/********************************************************************/
void slide_key( int j )
{
  register int i;
  
  i = j*5;
  spf( form[REPAMS+i].ob_spec.tedinfo->te_ptext, "%4d mS",
      key_pause[j]*20 );
  set_slid( REPADRAG+i, REPASLID+i, j ? 50 : 255, key_pause[j] );
  if( top_wind && !sk_ign ) reobj( REPAMS+i ); 
}
/********************************************************************/
void spf_date( char *ptr, int a, int b, int c, int usep )
{
  char sep;
  
  if( c>99 ) c -= 100;
  if( (sep = (char)idt_fmt) == 0 ) sep = '/';
  switch( (int)idt_fmt&0xf00 )
  {
    case 0x000:
      if( usep ) spf( ptr, "%02d%c%02d%c%02d", a, sep, b, sep, c );
      else spf( ptr, "%02d%02d%02d", a, b, c );
      break;
    case 0x100:
      if( usep ) spf( ptr, "%02d%c%02d%c%02d", b, sep, a, sep, c );
      else spf( ptr, "%02d%02d%02d", b, a, c );
      break;
    default:
      if( usep ) spf( ptr, "%02d%c%02d%c%02d", c, sep, a, sep, b );
      else spf( ptr, "%02d%02d%02d", c, a, b );
      break;
    case 0x300:
      if( usep ) spf( ptr, "%02d%c%02d%c%02d", c, sep, b, sep, a );
      else spf( ptr, "%02d%02d%02d", c, b, a );
  }
}
/********************************************************************/
void time_in_wind(void)
{
  if( !failed )
  {
    spf( form[TIMEEDIT].ob_spec.free_string, "%02d:%02d", hr, min );
    spf_date( form[DATEEDIT].ob_spec.free_string, mon, day, year, 1 );
/*    to_Europe( form[DATEEDIT].ob_spec.free_string ); */
  }
  if( top_wind ) reobj( DATEEDIT-1 );
  current = 1;
}
/********************************************************************/
/*void to_Europe( char *ptr )
{
  if( !US_keybd )
  {
    swap( ptr, ptr+3 );
    swap( ptr+1, ptr+4 );
    *(ptr+2) = *(ptr+5) = '.';
  }
} */
/********************************************************************/
void swap( char *ptr1, char *ptr2 )
{
  register char c;
  
  c = *ptr1;
  *ptr1 = *ptr2;
  *ptr2 = c;
}
/********************************************************************/
void to_int( char *ptr, int *out )
{
  register int i;
  
  for( i=0; i<3; i++, ptr+=2, out++ )
  {
    if( i<2 && (!*ptr || !*(ptr+1)) )
    {
      *out = 999;
      return;
    }
    *out = is_num( *ptr ) ? (is_num( *(ptr+1) ) ? 10*(*ptr-'0') +
        *(ptr+1) - '0' : *ptr-'0') : 999;
  }
}
/**************/
int is_num(int c)
{
  return( c >= '0' && c <= '9' );
}
/********************************************************************/
void use_slider( OBJECT *obj, int slid, int max, void func(int num) )
{
  int i;
  
  if( has_Geneva && graf_slidebox( 0L, max, 1, 0x100 ) >= 0 )
  {
    in_slider = 1;
    i = graf_slidebox( obj, slid-1, slid, 0x200 );
    while( i>=0 )
    {
      (*func)(i);
      i = graf_slidebox( obj, slid-1, slid, 0x300 );
    }
    in_slider = 0;
  }
  else (*func)( (long)graf_slidebox( obj, slid-1, slid, 0 ) * (long)max / 1000 );
}
/********************************************************************/
void wait(void)
{
  int b;
  
  do
    graf_mkstate( &dum, &dum, &b, &dum );
  while( b&1 );
}
/********************************************************************/
void move_boxes(void)
{
  static char from[] = { HBASE, HBOX1, HBOX2, HBOX4, HBOX5, HBOX6 },
                to[] = { 0, CBOX1, CBOX2, CBOX4, CBOX5, CMEMORY },
              pos1[] = { DATESTR, SETDT, DATEEDIT, SAVESTR, SAVERON-1,
                         REPASTR, REPALEFT, REPASLID, REPART, REPAMS },
              pos2[] = { TIMESTR, TIMEEDIT, TIMEAMPM-1, BLITMSG, SAVERPL,
                         SAVERMIN, SAVERMI, SAVEMSTR, REPESTR, REPELEFT,
                         REPELEFT+1, REPELEFT+3, REPELEFT+4 };
  register int i, h;
  OBJECT *obj;
  
  rsrc_gaddr( 0, HIREZ, &obj );
  for( i=0; i<sizeof(from); i++ )
    *(Rect *)&form[to[i]].ob_x = *(Rect *)&obj[from[i]].ob_x;
  form[CMEMORY].ob_spec.tedinfo->te_font = IBM;
  for( i=0; i<=SOUND-CLICK; i++ )
  {
    form[CLICK+i].ob_spec = obj[HCLICK+i].ob_spec;
    *(Rect *)&form[CLICK+i].ob_x = *(Rect *)&obj[HCLICK+i].ob_x;
  }
  h = form[DATESTR].ob_height;
  for( i=0; i<sizeof(pos1); i++ )
    form[pos1[i]].ob_y = 3;
  for( i=0; i<sizeof(pos2); i++ )
    form[pos2[i]].ob_y = 24-16+h;
  form[CLOCKON-1].ob_y = form[CLOCKSTR].ob_y = 45-16*2+(h<<1);
/*  form[REPASLID].ob_y--;
  form[REPELEFT+1].ob_y--; */
  rsrc_gaddr( 0, CHIPCFG, &obj );
  obj[CHIPBLIT].ob_spec.obspec.framesize = 
      obj[CHIPMCHC].ob_spec.obspec.framesize = 2;
  obj[CHIPBLIT].ob_height = obj[CHIPMCHC].ob_height <<= 1;
}
/********************************************************************/
#define DMAsmode  *(char *)0xff8921
#define DMAsctrl  *(char *)0xff8901
#define DMAsbaseh *(char *)0xff8903
#define DMAsbasem *(char *)0xff8905
#define DMAsbasel *(char *)0xff8907
#define DMAsendh  *(char *)0xff890f
#define DMAsendm  *(char *)0xff8911
#define DMAsendl  *(char *)0xff8913
void DMA_sound( int flag )
{
  long l, stack;
  
  objc_change( rsform, SOUNDTST, 0, rsrect.x, rsrect.y, rsrect.w,
      rsrect.h, flag, 1 );
  stack = Super(0L);
#ifndef NO_SND
  DMAsctrl = 0;
#endif
  if( flag )
  {
    conterm &= 0xFA;	/* turn off bell, click */
#ifndef NO_SND
    DMAsmode = (1<<7)|3;
    DMAsbaseh = (long)sound_data>>16;
    DMAsbasem = (long)sound_data>>8;
    DMAsbasel = (long)sound_data;
    l = (long)sound_data+sizeof(snd_dat);
    DMAsendh = l>>16;
    DMAsendm = l>>8;
    DMAsendl = l;
    DMAsctrl = 3;
#endif
  }
  Super((void *)stack);
}
void set_clbel( int cl, int bell, int flag )
{
/*  objc_change( rsform, SOUNDCLK, 0, rsrect.x, rsrect.y, rsrect.w,
      rsrect.h, !cl*DISABLED, flag&1 );
  objc_change( rsform, SOUNDBEL, 0, rsrect.x, rsrect.y, rsrect.w,
      rsrect.h, !bell*DISABLED, flag&2 ); */
  rsform[SOUNDCLK].ob_state = !cl ? DISABLED : 0;
  rsform[SOUNDBEL].ob_state = !bell ? DISABLED : 0;
  if( flag ) objc_draw( rsform, SOUNDBOX, 8, rsrect.x, rsrect.y,
      rsrect.w, rsrect.h );
}
int snd_val[4], min_snd[4]={0,-6,-6,-20}, max_snd[4]={40,6,6,20};
void sound_val( int set )
{
  if( !set )
  {
    snd_val[0] = (char)bal_vol;
    snd_val[1] = treb_bass&0xf;
    snd_val[2] = (treb_bass>>4)&0xf;
    snd_val[3] = (char)(bal_vol>>8);
  }
  else
  {
    bal_vol = (snd_val[3]<<8) | snd_val[0];
    treb_bass = (snd_val[2]<<4) | snd_val[1];
    set_sound();
  }
}
void show_snd( int num, int flag )
{
  int ind = num*6+VOLUME+1, i;
  char *ptr = rsform[ind+3].ob_spec.tedinfo->te_ptext;
  
  i = snd_val[num] + min_snd[num];
  if( min_snd[num]<0 )
    if( i<0 )
    {
      *ptr++ = '-';
      i = -i;
    }
    else if( i==0 ) *ptr++ = ' ';
    else *ptr++ = '+';
  if( num==1||num==2 ) *ptr++ = i+'0';
  else spf( ptr, "%-2d", i );
  if( !in_slider ) rsform[ind+1].ob_x =
      (long)(rsform[ind].ob_width-rsform[ind+1].ob_width) *
      snd_val[num] / (max_snd[num]-min_snd[num]);
  if( flag )
  {
    if( !in_slider ) reobjo( ind );
    reobjo( ind+3 );
  }
}
void snd_incr( int num, int add )
{
  int i;

  num = (num-VOLUME)/6;
  if( (i=snd_val[num]+min_snd[num]+add) >= min_snd[num] && i <= max_snd[num] )
  {
    snd_val[num] = i - min_snd[num];
    show_snd( num, 1 );
    sound_val(1);
  }
}
int func_p1, *func_p2;
void func_snd( int num )
{
  snd_incr( func_p1, num-*func_p2 );
}
void soundcfg(void)
{
  unsigned int old_bv=bal_vol, old_tb=treb_bass, old_cl=clickon,
      old_bl=bellon;
  int i, j, soundon=0;

  rsrc_gaddr( 0, SOUNDCFG, &rsform );
  set_clbel( clickon, bellon, 0 );
  sound_val(0);
  for( i=0; i<4; i++ )
    show_snd( i, 0 );
  set_sound();
  i = make_form( SOUNDCFG, SOUND, 0 );
  lock_wind(1);
  for(;;)
  {
    switch(i)
    {
      case SOUNDTST:
        DMA_sound( soundon = soundon^1 );
        wait();
        break;
      case SOUNDCLK:
        clickon ^= 1;
        goto clbel;
      case SOUNDBEL:
        bellon ^= 1;
clbel:  set_clbel( clickon, bellon, i==SOUNDCLK ? 1 : 2 );
        if( !soundon ) constat(1);
        break;
      case SOUNDOK:
        rsform[SOUNDOK].ob_state &= ~SELECTED;
        DMA_sound(0);
        constat(1);
        lock_wind(0);
        return;
      case VOLUME:
      case VOLUME+6:
      case VOLUME+12:
      case VOLUME+18:
        snd_incr( i, -1 );
        break;
      case VOLUME+3:
      case VOLUME+9:
      case VOLUME+15:
      case VOLUME+21:
        snd_incr( i, 1 );
        break;
      case VOLUME+2:
      case VOLUME+8:
      case VOLUME+14:
      case VOLUME+20:
        j = (i-VOLUME)/6;
        func_p1 = i;
        func_p2 = &snd_val[j];
        use_slider( rsform, i, max_snd[j]-min_snd[j], func_snd );
        break;
      default:
        rsform[i].ob_state &= ~SELECTED;
        DMA_sound(0);
        clickon = old_cl;
        bellon = old_bl;
        constat(1);
        bal_vol = old_bv;
        treb_bass = old_tb;
        set_sound();
        lock_wind(0);
        return;
    }
    i = form_do( rsform, 0 ) & 0x7fff;
  }
}
/********************************************************************/
void func_col( int num )
{
  set_color( func_p1, num );
}
void pallette(void)
{
  register int i, j;
  
  rsrc_gaddr( 0, i = is_121 ? COLS121 : COLS221, &rsform );
  if( planes<4 )
  {
    rsform[COLSLFT].ob_flags |= HIDETREE;
    rsform[COLSRT].ob_flags |= HIDETREE;
    rsform[COL0].ob_flags |= HIDETREE;
    if( planes==1 )
    {
      rsform[COL0+1].ob_flags |= HIDETREE;
      rsform[COL0+4].ob_flags |= HIDETREE;
      rsform[COL0+5].ob_flags |= HIDETREE;
      edge_col = -2;
    }
    else edge_col = -1;
  }
  show_color(0);
  current_color(0);
  i = make_form( i, PALLETTE, 0 );
  lock_wind(1);
  for(;;)
  {
    switch(i)
    {
      case COL0:
      case COL0+1:
      case COL0+2:
      case COL0+3:
      case COL0+4:
      case COL0+5:
        if( color != (j=i-COL0+edge_col) )
        {
          color = j;
          current_color(1);
          show_color(1);
        }
        break;
      case COLDRAG:
      case COLDRAG+4:
      case COLDRAG+8:
        func_p1 = (i-COLDRAG)/4;
        use_slider( rsform, i, 255, func_col );
        break;
      case COLLEFT:
      case COLLEFT+4:
      case COLLEFT+8:
        i = (i-COLLEFT)/4;
        set_color( i, curcol[i]-1 );
        break;
      case COLRT:
      case COLRT+4:
      case COLRT+8:
        i = (i-COLRT)/4;
        set_color( i, curcol[i]+1 );
        break;
      case COLRESET:
        set_colors();
        show_color(1);
        objc_change( rsform, COLRESET, 0, rsrect.x, rsrect.y, 
            rsrect.w, rsrect.h, rsform[COLRESET].ob_state&~SELECTED, 1 );
        break;
      case COLSLFT:
        if( edge_col > 0 )
        {
          edge_col--;
          current_color(1);
        }
        break;
      case COLSRT:
        if( edge_col < num_colors-6 )
        {
          edge_col++;
          current_color(1);
        }
        break;
      case COLQUIT:
        rsform[COLQUIT].ob_state &= ~SELECTED;
        finish();
        lock_wind(0);
        return;
    }
    i = form_do( rsform, 0 );
  }
}
/**********/
void current_color(int flg)
{
  register int i, j;
/*  static char col_tbl[] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };*/
  
  for( i=edge_col, j=COL0; i<num_colors && j<COL0+6; i++, j++ )
    if( i>=0 )
    {
      rsform[j].ob_flags &= ~HIDETREE;
      rsform[j].ob_state = OUTLINED * (i==color);
      rsform[j].ob_spec.obspec.interiorcol = i;
    }
  for( ; j<COL0+6; j++ )
    rsform[j].ob_flags |= HIDETREE;
  if(flg) reobjo( COLSBOX );
}
/********************************************************************/
void bell_toggle(void)
{
  bellon = !bellon;
  constat(1);
  constat(0);
}
/********************************************************************/
void click_toggle(void)
{
  clickon = !clickon;
  constat(1);
  constat(0);
}
/********************************************************************/
int ne0( int i )
{
  return(i!=0);
}
/********************************************************************/
void close_win(void)
{
  if( w_handle >= 0 )
  {
    wind_close( w_handle );
    wind_delete( w_handle );
    w_handle = -1;
  }
}
/********************************************************************/
void make_3d(void)
{
  RSHDR *r = *(RSHDR **)&_GemParBlk.global[7];
  OBJECT *o;
  int i, col, dum, elem;
  unsigned char *c;
  
  for( i=r->rsh_nobs, o=(OBJECT *)((long)r+r->rsh_object); --i>=0; o++ )
  {
    if( !has_Geneva )	/* 004 */
      if( (o->ob_state&X_MAGIC)==X_MAGIC ) o->ob_state &= ~(X_MAGIC|X_PREFER);
    c = (unsigned char *)&o->ob_flags;
    if( *c & 0xC0 )
    {
      *c = (*c&~0xC0) | ((*c>>15-10)&(3<<1));
      switch( o->ob_type )
      {
        case G_BOXTEXT:
        case G_FBOXTEXT:
        case G_TEXT:
        case G_FTEXT:
          o->ob_spec.tedinfo->te_color &= ~0x7F;
          break;
        case G_STRING:
        case G_BUTTON:
          break;
        default:
          o->ob_spec.index &= ~0x7FL;
      }
    }
  }
  elem = W_HSLIDE;
  wind_get( 1, WF_DCOLOR, &elem, &col, &dum, &dum );
  form[REPASLID].ob_spec.index = form[REPASLID+5].ob_spec.index =
      (form[REPASLID+5].ob_spec.index & ~0x7FL) | (long)(col&0x7f);
  elem = W_HELEV;
  wind_get( 1, WF_DCOLOR, &elem, &col, &dum, &dum );
  form[REPADRAG].ob_spec.index = form[REPADRAG+5].ob_spec.index =
      (form[REPADRAG+5].ob_spec.index & ~0x7FL) | (long)(col&0x7f);
  form[REPASLID].ob_x = form[REPASLID+5].ob_x += 2;
  form[REPASLID].ob_width = form[REPASLID+5].ob_width -= 4;
}
void read_vdi(void)
{
  int ex[57];
  
  vq_extnd( vdi_hand, 0, ex );
  is_mono = ex[39]==2;
  vq_extnd( vdi_hand, 1, ex );
  planes = ex[4];
  has_clut = ex[5];
}
/********************************************************************/
long get_idt_fmt(void)
{
  static unsigned int vals[] = { (0<<12) | (0<<8) | 0, 	  /* USA */
  				 (1<<12) | (1<<8) | '.',  /* Germany */
  				 (1<<12) | (1<<8) | 0,	  /* France */
  				 (1<<12) | (1<<8) | '.',  /* UK */
  				 (1<<12) | (1<<8) | 0,	  /* Spain */
  				 (1<<12) | (1<<8) | 0 };  /* Italy */
  unsigned int mode = (*(SYSHDR **)0x4f2)->os_palmode>>1;

  return mode>=sizeof(vals)/2 ? ((1<<12) | (2<<8) | '-') : vals[mode];
}
int conv_idt( int **arr, int *m, int *d, int *y )
{
  switch( idt_fmt&0xf00 )
  {
    case 0x000:
      arr[0] = m;
      arr[1] = d;
      arr[2] = y;
      return 0;
    case 0x100:
      arr[0] = d;
      arr[1] = m;
      arr[2] = y;
      return 1;
    default:
      arr[0] = d;
      arr[1] = y;
      arr[2] = m;
      return 2;
    case 0x300:
      arr[0] = y;
      arr[1] = d;
      arr[2] = m;
      return 3;
  }
}
void conv_date( int *date, int m, int d, int y, char *fmt )
{
  int i, *arr[3];
  char *list, c, **p;
  
  rsrc_gaddr( 15, IDT, &p );
  list = *p + 3*conv_idt( arr, &m, &d, &y );
  if( (c = (char)idt_fmt) == 0 ) c = '/';
  for( i=0; i<3; i++ )
  {
    date[i] = *arr[i];
    *fmt++ = *fmt++ = *list++;
    *fmt++ = c;
  }
  *(fmt-1) = 0;
}
void func_key( int num )
{
  set_key( func_p1, num );
}
EMULTI emulti = { 0, 1, 1, 1,  0, 0, 0, 0, 0,  1, 0, 0, 0, 0,  0L, 999 };

int main(void)
{
  int i, etype=MU_MESAG, select, time[3], date[3], *iptr, j;
  unsigned long stack, buffer[4], l;
  static char dtfmt[] = "%02d%02d%02d";
  char temp[90], *ptr, **p, inside=0,
      no_adspd=0, cjar=0, ge_40, temp2[10], temp3[10], d3=0;
  struct la_font *fontp;
  Rect box;
  extern int _app;
  long is_ads(void);

  if( (long)snd_dat >> 24L )	/* is sound in TT RAM? */
    if( (ptr = Mxalloc(sizeof(snd_dat),0)) != 0 )
      memcpy( sound_data=ptr, snd_dat, sizeof(snd_dat) );
  apid = appl_init();
  if( (ge_40 = _GemParBlk.global[0] >= 0x400) != 0 )
      shel_write( 9, 1, 0, 0L, 0L );
  linea0();
  vdi_hand = graf_handle( time, &i, time, time );
  read_vdi();
  if( (l = 1L<<planes) > MAX_COLOR ) num_colors = MAX_COLOR;
  else num_colors = l;
  fontp = la_init.li_a1[1+(is_121 = i==16)];
  fdat = fontp->font_data;
  fht = i;
  bigscr = (l=(long)Logbase()) >= 0xC00000L && l < 0xE00000L ? planes
      : -1;
  if( l==0xfec00000L )
  {
    bigscr = planes<8 ? 1 : planes;
    is_CD = 1;
    scan_off = V_X_MAX>>3;
    if( planes>=8 ) scan_off *= planes;
  }
  else scan_off = VWRAP;
  screen_size = ((unsigned)scan_off >> 2) * V_Y_MAX;
  wind_get( 0, WF_CURRXYWH, &dum, &dum, &max_w, &max_h );
  has_Geneva = getcookie( GENEVA_COOKIE, &l ) == CJar_OK && l!=0L;
  if( !rsrc_load( "neocntrl.rsc" ) )
  {
    form_alert( 1,
        "[1][NEOCNTRL.RSC is not|in the current|directory!][Ok!]" );
    if( _app ) goto quit;
  }
  else if( getcookie( CJar_cookie, 0L ) != CJar_OK )
  {
    f_alert( 1, NOJAR );
    if( _app ) goto quit;
  }
  else
  {
    cjar++;
    if( CJar( 0, _CPU_COOKIE, &l ) == CJar_OK ) is_030 = (int)l >= 30;
#ifndef NO_SND
    if( CJar( 0, _SND_COOKIE, &l ) == CJar_OK )
    {
      new_SND = ne0( (int)l&2 ) && !ne0( (int)l&4 );	/* !Falcon */
    }
#else
    new_SND = 1;
#endif
    if( CJar( 0, IDT_cookie, &l )==CJar_OK ) idt_fmt = (int)l;
    else idt_fmt = Supexec( get_idt_fmt );
    if( !_app || ge_40 )
    {
      rsrc_gaddr( 15, A1, &p );
      menu_register( apid, *p );
    }
    rsrc_gaddr( 0, REZ0OR1, &form );
    rsrc_gaddr( 15, A9, &ctrl_title );
    if( planes==1 )
    {
      rsrc_gaddr( 15, A11, &p );
      form[BLITMSG].ob_spec.free_string = *p;
    }
    if( is_121 ) move_boxes();
    if( _GemParBlk.global[0] >= 0x340 )
    {
      if( _GemParBlk.global[0] != 0x399/* MagiX */ && _GemParBlk.global[0] < 0x410 ) time[0] = 1;
      else
      {
        time[0] = 0;
        appl_getinfo( 13, time, time+1, time+1, time+1 );
      }
      if( time[0] )
      {
        make_3d();
        d3 = 1;
      }
    }
    if( !d3 && is_121 )
    {
      form[REPELEFT+1].ob_y--;
      form[REPALEFT+1].ob_y--;
    }
    for( i=CLICK; i<=SOUND; i++ )
      form[i].ob_height = form[i].ob_spec.bitblk->bi_hl;
    if( !new_SND ) form[SOUND].ob_flags |= HIDETREE;
    else
    {
      rsrc_gaddr( 0, SOUNDCFG, &rsform );
      rsform[SOUNDCLK].ob_type = rsform[SOUNDBEL].ob_type = G_IMAGE;
      rsform[SOUNDCLK].ob_spec = form[CLICK].ob_spec;
      rsform[SOUNDBEL].ob_spec = form[BELL].ob_spec;
      *(long *)&rsform[SOUNDCLK].ob_width = *(long *)&form[CLICK].ob_width;
      *(long *)&rsform[SOUNDBEL].ob_width = *(long *)&form[BELL].ob_width;
      *(long *)&form[SOUND].ob_x = *(long *)&form[CLICK].ob_x;
      form[CLICK].ob_flags |= HIDETREE;
      form[BELL].ob_flags |= HIDETREE;
    }
    wind_calc( 0, CWIND_TYPE, 0, 0,       /* fit the window around the */
        form[0].ob_width, form[0].ob_height,  /* dialog */
        &dum, &dum, &windo.w, &windo.h );
    windo.x = max_w - windo.w >> 1;
    windo.y = (max_h-windo.h >> 1) + 5;
    failed = 0;
  }
  if( cjar )
    if( getcookie( _VDO_COOKIE, (long *)&l ) == CJar_OK )
    {
      TT_mono = (i=(l>>16))==2 && Getrez()==2;
      falc_vid = i==3;
    }
  no_clock = 0;
  if( cjar )
  {
    if( getcookie( _MCH_COOKIE, (long *)&l ) == CJar_OK )
    {
      is_MSTe = l==0x00010010L;
      is_falc = (int)(l>>16L) == 3;
      if( (l>>16)>2 ) no_adspd=1;
    }
    else if( (l>>16)!=2 ) no_clock = Supexec(clock_tst);
    has_magcmac = CJar( 0, MAGC_COOKIE, &stack ) == CJar_OK;	/* 004 */
  }
  has_Blit = ne0(Blitmode(-1)&2);
  if( is_MSTe && is_030 ) is_MSTe = 0;
  if( !no_adspd ) Supexec( is_ads );
  chips = has_Blit + is_030 + is_MSTe + ne0(has_adspd);
  read_time();
  etype = MU_TIMER|MU_MESAG;
  reset();
  set_buttons();

  if( _app )
  {
    graf_mouse( ARROW, 0L );
    goto open;
  }
  for(;;)
  {
    if( !lc && !_app && cjar )
      if( getcookie( LOAD_COOKIE, (long *)&lc ) == CJar_OK && !has_magcmac/*004*/ )
      {
        stack = Super((void *)0L);
        if( *lc->vclock ) (*lc->clock_off)();
        for( i=1; i<nvbls; i++ )
          if( !*(_vblqueue+i) )
          {
            (*lc->clock_on)( vclock, _vblqueue+i );
            break;
          }
        Super( (void *)stack );
      }
    emulti.type = etype;
    *(Rect *)&emulti.m1x = center;
    *(Rect *)&emulti.m2x = center;
    multi_evnt( &emulti, (int *)buffer );
    wind_get( 0, WF_TOP, &i, &dum, &dum, &dum );
    if( lc && lc->mas && lc->mas->ver>='\7' ) idt_fmt = lc->mas->most->idt_fmt;		/* 004 */
    top_wind = i == w_handle;
    if( sec ) current = 0;
    if( get_time ) get_system();
    else if( !sec && !current ) time_in_wind();
    if( emulti.event & MU_KEYBD && emulti.mouse_k==4 && emulti.key>>8 == 0x11 )
         goto closed;
    if( emulti.event & MU_MESAG )                        /* message event */
      switch( (int)(buffer[0]>>16) )
      {
        case CNTRL_INIT4:
        case CNTRL_INIT:
          i = ampm;
          j = clockon;
          iptr = (int *)buffer[1];
          ampm = ne0(*iptr);
          clockon = ne0(*(iptr+1));
          saveon = ne0(*(iptr+2));
          savmins = *(iptr+3);
          bliton = ne0(*(iptr+4));
          key_pause[0] = *(iptr+5) / 20;
          key_pause[1] = *(iptr+6) / 20;
          clickon = ne0(*(iptr+7));
          bellon = ne0(*(iptr+8));
          dc_rate = *(iptr+9);
          flopr[0] = *(iptr+10);
          flopr[1] = *(iptr+11);
          bal_vol = *(iptr+12);
          treb_bass = *(iptr+13);
          set_lc();
          oldcaps = -1;
          if( i && !ampm || j && !clockon )
          {
            i = ampm;
            ampm = 1;
            clear_clock();
            ampm = i;
          }
          get_colors();
          set_buttons();
          set_caches(*(iptr+4));
          set_sound();
          reset();
          ack( (int)buffer[2] );
          break;
        case CNTRL_REQ:
        case CNTRL_REQ4:
          get_lc();
          spf( (char *)buffer[1], "%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
              ampm, clockon, saveon, savmins, set_caches(-1), key_pause[0]*20,
              key_pause[1]*20, clickon, bellon, dc_rate, flopr[0], flopr[1],
              bal_vol, treb_bass );
          ack( (int)buffer[2] );
          break;
        case NEO_AC_OPEN:
        case AC_OPEN:
          if( failed ) break;
open:     if( w_handle < 0 )                      /* no window open */
          {
            if( (w_handle=wind_create(CWIND_TYPE,windo.x,windo.y,windo.w,
                windo.h)) < 0 )
            {
              f_alert( 1, A3 );
              break;
            }
            name_it( w_handle, 0L );
            wind_open( w_handle, windo.x, windo.y, windo.w, windo.h );
            wind_get( w_handle, WF_WORKXYWH, &center.x, &center.y, &dum,
                &dum );                      /* get its location, area */
            form[0].ob_x = center.x;       /* move the dialog there */
            form[0].ob_y = center.y;
            center.w = form[0].ob_width;
            center.h = form[0].ob_height;
            etype = MU_BUTTON|MU_MESAG|MU_KEYBD|MU_TIMER|MU_M1|MU_M2;
            break;
          }
          if( top_wind )
          {
            copyright(0);
            break;
          }
        case WM_TOPPED:    /* make our window the topmost */
          if( w_handle >= 0 ) wind_set( w_handle, WF_TOP, (int)buffer[1],
              0, 0, 0 );
          break;
        case WM_REDRAW:
          if( w_handle >= 0 )                                 /* just in case */
          {                          /* take the rectangles out for a walk */
            rsrc_gaddr( 15, A4, &p );
            stack = Super((void *)0L);
            spf( form[CMEMORY].ob_spec.tedinfo->te_ptext, *p,
                PHYSTOP, Malloc(-1L) );
            Super( (void *)stack );
            wind_get( w_handle, WF_FIRSTXYWH, &box.x, &box.y, &box.w, &box.h );
            wind_update( BEG_UPDATE );
            graf_mouse( M_OFF, 0L );
            get_system();
            sk_ign++;
            reset();
            clksav();
            sk_ign=0;
            while( box.w && box.h )
            {
              objc_draw( form, 0, MAX_DEPTH, box.x, box.y, box.w, box.h );
              wind_get( w_handle, WF_NEXTXYWH, &box.x, &box.y, &box.w, &box.h );
            }
            graf_mouse( M_ON, 0L );
            wind_update( END_UPDATE );
          }
          break;
        case AP_TERM:
          close_win();
          if( lc && *lc->vclock ) Supexec((long (*)())lc->clock_off);
          goto quit;
        case AC_CLOSE:
          w_handle = -1;
        case WM_CLOSED:
closed:   close_win();
          get_colors();
          etype = MU_MESAG|MU_TIMER;
          name_it( w_handle, (char *)-1L );
          if( _app ) goto quit;
          break;
        case WM_MOVED:        /* move the window somewhere else */
          wind_set( w_handle, WF_CURRXYWH, windo.x = (int)(buffer[2]>>16),
              windo.y = (int)buffer[2], windo.w = (int)(buffer[3]>>16),
              windo.h = (int)buffer[3] );
          wind_get( w_handle, WF_WORKXYWH, &center.x, &center.y, &dum, &dum );
          form[0].ob_x = center.x;
          form[0].ob_y = center.y;
          break;
      }
    if( emulti.event & MU_M2 )
    {
      if( emulti.event&MU_BUTTON ) inside = 0;
      if( w_handle>0 ) name_it( w_handle, 0L );
    }
    if( emulti.event & MU_M1 )
    {
      if( !(emulti.event&MU_BUTTON) ) inside = 1;
      if( wind_find( emulti.mouse_x, emulti.mouse_y ) == w_handle &&
          (select = find_obj( emulti.mouse_x, emulti.mouse_y )) >= CLICK &&
          select <= SOUND ) choose_name( w_handle, select-CLICK );
      else if( w_handle>0 ) name_it( w_handle, 0L );
    }
    if( top_wind && emulti.event&MU_BUTTON && inside )
    {
      select = find_obj( emulti.mouse_x, emulti.mouse_y );
      if( ( form[select].ob_flags & SELECTABLE &&
          !( form[select].ob_state & DISABLED ) ||
          ( form[select].ob_flags & TOUCHEXIT ) )
          && !(form[select].ob_state & SELECTED) ) switch( select )
      {
        case TIMEAMPM:
        case TIME24:
          form[TIMEAMPM+!ampm].ob_state = 0;
          form[select].ob_state = SELECTED;
          reobj(TIMEAMPM-1);
          j = clockon;
          clockon = 0;
          if( select==TIME24 )
          {
            if(j) clear_clock();
            ampm = 0;
          }
          else ampm = 1;
          get_system();
          clockon = j;
          set_lc();
          break;
        case SETDT:
          time_in_wind();
          rsrc_gaddr( 0, EDITDT, &rsform );
          spf( rsform[EDTIME].ob_spec.tedinfo->te_ptext, dtfmt,
              hr, min, sec );
          spf_date( rsform[EDDATE].ob_spec.tedinfo->te_ptext,
              mon, day, year, 0 );
          if( ampm )
          {
            rsform[EDAM].ob_flags = rsform[EDPM].ob_flags |= RBUTTON|SELECTABLE;
            rsform[EDAM+pm].ob_state |= SELECTED;
            rsform[EDAM+pm].ob_state &= ~DISABLED;
            rsform[EDAM+!pm].ob_state &= ~(SELECTED|DISABLED);
          }
          else
          {
            rsform[EDAM].ob_state = rsform[EDPM].ob_state |= DISABLED;
            rsform[EDAM].ob_state = rsform[EDPM].ob_state &= ~SELECTED;
          }
          for(;;)
            if( make_form( EDITDT, SETDT, EDDATE ) != EDOK ) break;
            else
            {
              to_int( rsform[EDTIME].ob_spec.tedinfo->te_ptext, time );
              if( ampm && (time[0]<=0 || time[0]>12) ) f_alert( 1, A5 );
              else if( !ampm && (time[0]<0 || time[0]>23) ) f_alert( 1, A6 );
              else if( time[1]<0 || time[1]>59 || time[2]<0 || time[2]>59 )
                  f_alert( 1, A7 );
              else
              {
                to_int( rsform[EDDATE].ob_spec.tedinfo->te_ptext, date );
                conv_date( date, date[0], date[1], date[2], temp2 );
                if( date[0]<=0 || date[0]>12 || date[1]<=0 || date[1]>31 ||
                    date[2]>99 )
                {
                  rsrc_gaddr( 15, A8, &p );
                  spf_date( temp3, 5, 2, 94, 1 );
                  spf( temp, *p, temp2, temp3 );
                  form_alert( 1, temp );
                }
                else
                {
                  if( ampm ) pm = rsform[EDPM].ob_state & SELECTED;
                  hr = time[0];
                  min = time[1];
                  sec = time[2];
                  year = date[2]<80 ? date[2]+100 : date[2];
                  mon = date[0];
                  day = date[1];
                  set_time();
                  break;
                }
              }
            }
          finish();
          break;
        case CLOCKON:
        case CLOCKOFF:
          if( _app ) f_alert( 1, A12 );
          else if( !clockon && !lc ) f_alert( 1, A10 );
          else
          {
            if( (clockon = !clockon) == 0 ) clear_clock();
            oldcaps = -1;
            form[select].ob_state = SELECTED;
            form[CLOCKON+clockon].ob_state = 0;
            reobj(CLOCKON-1);
            set_lc();
          }
          break;
        case SAVERON:
        case SAVEROFF:
          if( _app ) f_alert( 1, A12 );
          else if( !saveon && !lc ) f_alert( 1, A10 );
          else
          {
            saveon = !saveon;
            set_lc();
            form[select].ob_state = SELECTED;
            form[SAVERON+saveon].ob_state = 0;
            reobj(SAVERON-1);
          }
          break;
        case SAVERPL:
          if( savmins < 9 ) j = savmins + 1;
          else break;
          goto plmi;
        case SAVERMI:
          if( select==SAVERMI )
            if( savmins > 1 ) j = savmins - 1;
            else break;
plmi:     if( j != savmins )
          {
            savmins = j;
            savsecs = j*60;
            spf( form[SAVERMIN].ob_spec.tedinfo->te_ptext, ifmt, savmins );
            reobj(SAVERMIN);
            set_lc();
            pause();
          }
          break;
        case REPASLID:
        case REPASLID+5:
          objc_offset( form, select+1, &i, &dum );
          j=(select-REPASLID)/5;
          set_key( j, emulti.mouse_x<i ? key_pause[j]-5 : key_pause[j]+5 );
          break;
        case REPADRAG:
        case REPADRAG+5:
          func_p1 = (select-REPADRAG)/5;
          use_slider( form, select, func_p1 ? 50 : 255, func_key );
          break;
        case REPALEFT:
        case REPALEFT+5:
          j = (select-REPALEFT)/5;
          set_key( j, key_pause[j]-1 );
          break;
        case REPART:
        case REPART+5:
          j = (select-REPART)/5;
          set_key( j, key_pause[j]+1 );
          break;
        case PALLETTE:
          if( is_mono && planes==1 )
          {
            lsetcolor( 0, lsetcolor(0,-1L) ? 0L : 0xFFFFFFL );
            wait();
          }
          else pallette();
          break;
        case CLICK:
          click_toggle();
          reobj( d3 ? CBOX5 : CLICK );
          wait();
          break;
        case MOUSEI:
          rsrc_gaddr( 0, MOUSESET, &rsform );
          i = dc_rate = evnt_dclick( 0, 0 );
          for( j=0; j<5; j++ )
            rsform[j+MSSLOW].ob_state = j==dc_rate ? SELECTED : 0;
          if( make_form( MOUSESET, MOUSEI, 0 ) != MSOK ) evnt_dclick( i, 1 );
          finish();
          break;
        case BELL:
          bell_toggle();
          reobj( d3 ? CBOX5 : BELL );
          wait();
          break;
        case FLOPPYRT:
          rsrc_gaddr( 0, FLOPCFG, &rsform );
          floprate(0);
          rsform[FLOPA525+!flopr[0]].ob_state =
            rsform[FLOPB525+!flopr[1]].ob_state = 0;
          rsform[FLOPA525+(flopr[0]>0)].ob_state =
            rsform[FLOPB525+(flopr[1]>0)].ob_state = SELECTED;
          if( make_form( FLOPCFG, FLOPPYRT, 0 ) == FLOPOK )
          {
            flopr[0] = rsform[FLOPA35].ob_state ? 3 : 0;
            flopr[1] = rsform[FLOPB35].ob_state ? 3 : 0;
            floprate(1);
          }
          finish();
          break;
        case SOUND:
          soundcfg();
          finish();
          break;
        case CHIPS:
          if( !chips ) break;
          if( chips==1 && has_Blit )
          {
            bliton = bliton^1;
            set_blit();
            reobj(CHIPS);
            wait();
          }
          else
          {
            rsrc_gaddr( 0, CHIPCFG, &rsform );
            i = set_caches(-1);
            if( !has_Blit ) rsform[CHIPBLIT].ob_flags =
                rsform[CHIPBLIT+1].ob_flags |= HIDETREE;
            else rsform[CHIPBLIT].ob_state = i&1;
            if( !is_030 && !has_adspd ) rsform[CHIPMCHC-1].ob_flags = HIDETREE;
            else rsform[CHIPMCHC].ob_state = i&2 ? SELECTED : 0;
            if( !is_MSTe ) rsform[CHIPM8HZ-1].ob_flags = HIDETREE;
            else
            {
              rsform[CHIPMCHC-1].ob_flags = HIDETREE;
              j = (i>>2)&3;
              rsform[CHIPM8HZ].ob_state = !j ? SELECTED : 0;
              rsform[CHIPM8HZ+1].ob_state = j==2 ? SELECTED : 0;
              rsform[CHIPM8HZ+2].ob_state = j==3 ? SELECTED : 0;
            }
            if( make_form( CHIPCFG, CHIPS, 0 ) == CHIPOK )
            {
              i = 0;
              if( rsform[CHIPBLIT].ob_state&SELECTED ) i |= 1;
              if( rsform[CHIPMCHC].ob_state&SELECTED ) i |= 2;
              if( rsform[CHIPM8HZ+1].ob_state&SELECTED ) i |= 8;
              else if( rsform[CHIPM8HZ+2].ob_state&SELECTED ) i |= 12;
              set_caches(i);
            }
            finish();
          }
          break;
        case COPYRT:
          wait();
          copyright(COPYRT);
          wait();
          break;
      }
    }
  }
quit:
  appl_exit();
  return 0;
}


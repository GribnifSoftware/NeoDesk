/* NeoDesk 3.02 by Dan Wilga
   Copyright 1990, Gribnif Software.
   All Rights Reserved.
*/
#include "string.h"
#include "stdlib.h"
#include "stddef.h"
#include "aes.h"
#include "tos.h"
#include "neodesk.h"
#include "mwclinea.h"
#include "neocommn.h"
#include "neod2_id.h"
#include "ctype.h"
#include "xwind.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */
extern GUI *gui;

#define OS_version  (*(int *)((*(long *)0x4F2)+2))
#define index strchr

extern char *msg_ptr[], is_bee, has_mint, ver_gt_10;
extern MOST *z;
extern int w_num, clsizb[7], alt_acc[MAX_NEO_ACC];
extern int use_8x16;
extern GROUP_DESC *group_desc[7];
extern Rect ww[7][2];
extern OBJECT *menu;
extern GRAPHICS *graphics;
extern struct Max_icon
{
  int text_w, data_w, h;
} max_icon;

int auto_twst( int twst, int spt )
{
  if( twst!=3 ) return twst;
  if( ver_gt_10 && (spt==20 || spt==30) ) return 1;	/* 004 */
  if( spt>10 ) return 0;
  return spt==9 ? 2 : (ver_gt_10 ? 1 : 0);
}

long drvmap(void)
{
  return Dsetdrv(Dgetdrv());
}

int fix_coord( int to_coord, COORD *c, int num, int scale )
{
  if( !to_coord )
  {
    c->fix[1] = num - (c->fix[0] = num/scale)*scale;
    return 0;
  }
  else return c->fix[0]*scale + c->fix[1];
}

void group_unit( int wind, int *w, int *h )
{
  struct Max_icon temp;
  int i;
  
  temp = max_icon;
  i = z->showicon[wind];
  z->showicon[wind] = 1;
  get_max_icon(wind);
  z->showicon[wind] = i;
  *w = max_icon.text_w;
  *h = max_icon.h;
  max_icon = temp;
}

void get_widths(void)
{
  txt_menu_copy();
  get_max_icon(w_num);
}

void get_max_icon(int num)
{
  register int icon, large, j, w;
  static int width[3] = { 9, 9, 6 };
  char is_group;

  icon = num>=0 ? z->showicon[num] : 1;
  large = num>=0 ? z->stlgsml[num] : 0;
  is_group = ed_wind_type(num)==EDW_GROUP;
  max_icon.text_w = icon ? (!is_group ? icon_width(12)+2 : icon_width(20)+2) : 14;
  max_icon.h = icon ? ICON_H+z->wind_font[0].h+2 : z->wind_font[1+large].h;
      /*%(large ? (use_8x16?16:8) : (use_8x16?8:6)); */
  w = z->wind_font[1+large].w;
  if( !icon )
  {
    if( num<0 || is_group )
    {
      if( num>=0 )
      {
        max_icon.text_w += 20-14;
        if( group_desc[num]->hdr.opts.s.showpath ) max_icon.text_w += 121;
        if( group_desc[num]->hdr.opts.s.showtype ) max_icon.text_w += 13;
        j = ww[num][0].w / w;
        if( max_icon.text_w > j ) max_icon.text_w = j+1;
      }
    }
    else
    {
      width[0] = z->other_pref.b.long_numbers ? 9 : 6;		/* 005 */
      for( j=0; j<3; j++ )
        if( z->sizdattim[num][j] ) max_icon.text_w += width[j];
      if( z->sizdattim[num][0] && clsizb[num]>1 && z->other_pref.b.consumption )
          max_icon.text_w += width[0];		/* 005: take from width */
    }
    max_icon.text_w *= w;
  }
  max_icon.data_w = icon ? ICON_W : max_icon.text_w;
  if( icon && max_icon.text_w < ICON_W ) max_icon.text_w = ICON_W;
} 
/********************************************************************/
void short_path( char *p, char *s, int len, int maxlen )
{
  char *ptr;
  int max, i, l, plus=3;
  
  if( w_num>=0 || p )
  {
    if( !p )
    {
      p = z->w[w_num].path;
      if( ed_wind_type(w_num)!=EDW_GROUP ) max = pathend(p);
      else max = strlen(p);
    }
    else max = strlen(p);
    if( p[0]==CLIP_LET )
    {
      p += 2;
      max -= 2;
      plus = 1;
    }
    if( len >= maxlen ) len = maxlen-1;
    if( max<len )
    {
      strncpy( s, p, max );
      s[max] = 0;
    }
    else
    {
      ptr = strchr( p+plus, '\\' ) + 1;
      strncpy( s, p, ptr-p );
      strcpy( s+(ptr-p), "..." );
      len -= (l = strlen(s));
      while( ptr && (i=p+max-ptr) > len )
        ptr = strchr( ptr+1, '\\' );
      if( ptr )
      {
        strncpy( s+l, ptr, i );
        s[l+i] = 0;
      }
    }
  }
}

int find_place( int func( int a, int b ) )
{
  register int pl, num, wh;
  extern MOST *z;
  extern int wxref[];
  extern int w_open;
  
  for( pl=w_open; pl>0; pl-- )
    for( num=7; --num>=0; )
      if( (wh=wxref[num]) >= 0 && z->w[num].place == pl ) 
        if( (*func)(wh,num) ) return(1);
        else break;	/* 003: goto next pl */
  return(0);
}
static char nums[] = "0123456789ABCDEF";
char *skip_num( char *buf )
{
  while( strchr(nums,*buf) ) buf++;
  return buf;
}
int cdecl sscnf( char *buf, char *fmt, ... )
{
  return( sscnf_( buf, fmt, (int **)&... ) );
}
int sscnf_( char *buf, char *fmt, int **arg )
{
  register int cnt=0, base;
  register unsigned long i;
  register char *ptr, c, larg;
  char neg;
  
  while( *fmt && *buf )
  {
    base = 10;
    larg = 2;
    neg=0;
    while( *fmt == *buf && *buf )
    {
      fmt++;
      buf++;
    }
    if( isspace(*fmt) ) while( isspace(*buf) ) buf++;
    else
      if( *fmt == '%' )
      {
        switch( *(++fmt) )
        {
          case 'b':
            *(char *)*arg++ = *buf++ == 'T' ? 1 : 0;
            break;
          case 'k':
            sscnf( buf, "%h %h %h", &(*(KEYCODE **)arg)->shift,
               &(*(KEYCODE **)arg)->scan, &(*(KEYCODE **)arg)->ascii );
            arg++;
            buf = skip_num(skip_num(skip_num(buf)+1)+1);
            break;
          case 'h':
            larg = 0;
          case 'X':
            base = 16;
            goto get;
          case 'x':
            base = 16;
          case 'd':
            larg = 1;
          case 'D':
get:        i=0L;
            while( *buf && !isspace(*buf) )
            {
              c = *buf;
              if( !i && c=='-' ) neg++;
              else
              {
                if( c>='a' && c<='f' ) c &= 0x5F;
                if( (ptr=strchr(nums,c)) != 0 && (long)ptr-(long)nums < base )
                    i = i*base + (long)ptr - (long)nums;
                else break;
              }
              buf++;
            }
            if( neg ) i = -i;
            if( larg==2 ) **((long **)arg++) = i;
            else if( larg ) **arg++ = i;
            else **((char **)arg++) = i;
            break;
          case 's':
            ptr = (char *)*arg++;
            while( *buf && !isspace(*buf) ) *ptr++ = *buf++;
            *ptr++ = '\0';
            break;
          case 'S':
            if( !*buf ) break;
            ptr = (char *)*arg++;
            buf++;
            while( *buf && *buf!='`' ) *ptr++ = *buf++;
            if( *buf ) buf++;
            *ptr++ = '\0';
            break;
          case 'c':
            *(char *)*arg++ = *buf++;
            break;
        }
        cnt++;
      }
    fmt++;
  }
  return(cnt);
}

long cFwrite( int handle, long count, void *buf );

void fps(char *s, int fh) {
  cFwrite( fh, (long)strlen(s), s );
}
void fpf(int fh, char *fmt, ...) {
  extern MASTER *mas;
  extern char diskbuff[];
  
  (*mas->dopf)(diskbuff, fmt, (unsigned int *)&...);
  cFwrite( fh, (long)strlen(diskbuff), diskbuff );
}
void spf(char *buf, char *fmt, ...) {
  extern MASTER *mas;
  (*mas->dopf)(buf, fmt, (unsigned int *)&...);
}
void spfcat(char *buf, char *fmt, ...) {
  extern MASTER *mas;
  (*mas->dopf)(buf+strlen(buf), fmt, (unsigned int *)&...);
}
/*****************************************************************/
void bee(void)
{
  graf_mouse( HOURGLASS, 0L );
  is_bee = 1;
}
/*****************************************************************/
void arrow(void)
{
  graf_mouse( ARROW, 0L );
  is_bee = 0;
}
/*****************************************************************/
void _graf_mouse( int shape )
{
  graf_mouse( shape, 0L );
  is_bee = shape == HOURGLASS;
}
/*****************************************************************/
int min( int a, int b )
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
/**********************************************************************/
int f_alert1( char *msg )
{
  char bv;
  int ret;
  
  if( (bv=graphics && graphics->have_butv) != 0 ) (*graphics->set_butv)(0);
  ret = form_alert( 1, msg );
  if( bv ) (*graphics->set_butv)(1);
  return ret;
}
/***************************************************************/
int cdecl _abort(void)
/* abort a copy or delete sequence on Ctrl-C/Undo/both Shift. 1=abort */
{
  register unsigned char key, ret=0, c;
  int l;
  IOREC *io = z->kbio;
  
  if( io->ibufhd != (l=io->ibuftl) )
  {
    if( (l+=3) > io->ibufsiz ) l = 3;
    if( *((char *)io->ibuf+l)=='\03' || *((char *)io->ibuf+l-2)=='\x61' )
    {
      io->ibufhd = io->ibuftl;
      ret = 1;
    }
  }
  if( Getshift()==3 ) ret=1;
  if( ret ) ret = 2 - f_alert1( msg_ptr[0] );
  return(ret);
}
/********************************************************************/
int appl_pwrite( int hand, int len, void *msg )
{
  wind_update( BEG_UPDATE );
  wind_update( END_UPDATE );
  wind_update( BEG_UPDATE );
  wind_update( END_UPDATE );
  wind_update( BEG_UPDATE );
  wind_update( END_UPDATE );
  wind_update( BEG_UPDATE );
  wind_update( END_UPDATE );
/*  evnt_timer( 10, 0 );*/
  return( appl_write( hand, len, msg ) );
}
/********************************************************************/
void wind_lock( int lock )
{
  wind_update( lock ? BEG_UPDATE : END_UPDATE );
  wind_update( lock ? BEG_MCTRL : END_MCTRL );
}
/********************************************************************/
int appl_pfind( char *msg )
{
  char temp[20], *p;

  if( !*msg || *msg==' ' ) return(-1);
  for( p=temp; *msg && *msg!='.' && p-temp<8; )	/* 004: for AV funcs */
    *p++ = *msg++;
  while( p-temp < 8 ) *p++ = ' ';
  *p = 0;
  return( appl_find( temp ) );
}
/***************************************************************/
void cdecl bconws( char *ptr )
{
  extern LoadCookie *lc;

  (*lc->bconws)(ptr);	/* 005: call lc's version, for cyrel */
}
/*********************************************************************/
int slashes( char *ptr )
{
  register int i=0;
  
  while( *ptr )
    if( *ptr++ == '\\' ) i++;
  return(i);
}
/********************************************************************/
int cdecl check_prn(void)
{
  int i = 0;

  while( !i && !Bcostat(0) )
    if( form_alert( 2, msg_ptr[8] ) == 2 ) i=1;
  return(i);
}
/********************************************************************/
int check_q(int alert)           /* is the printer queue installed? 1=yes */
{
  extern char neoq[];
  extern int q_handle;
  
  if( (q_handle = appl_pfind( neoq )) < 0 )
  {
    if( alert ) f_alert1( msg_ptr[9] );
    return( 0 );
  }
  return( 1 );
}
/********************************************************************/
int check_reorder(void)
{
  extern char reorder_on;
  
  if( reorder_on )
  {
    f_alert1( msg_ptr[102] );
    return(0);
  }
  return(1);
}
/********************************************************************/
void error( int num )
{
  char str[80];
  
  spf( str, msg_ptr[16], num );
  Crawio(7);
  f_alert1( str );
}
/********************************************************************/
void to_dirname( char *ptr, char *buf )
{
  register char *ptr2;
  
  ptr2 = buf;
  while( *ptr && *ptr!='\\' && ptr2-buf<11 )
  {
    if( *ptr == '.' ) while( ptr2-buf < 8 ) *ptr2++ = ' ';
    else *ptr2++ = *ptr;
    ptr++;
  }
  while( ptr2-buf < 11 ) *ptr2++ = ' ';
}

int canw( int i )
{
  union
  { 
    char c[2];
    int i;
  } u, v;
  
  u.i = i;
  v.c[1] = u.c[0];
  v.c[0] = u.c[1];
  return( v.i );
}
/*********************************************************************/
void flush(void)
{
  extern IOREC *io;
  
  while( Bconstat(2) ) Bconin(2);              /* flush input buffer */
  z->kbio->ibufhd = z->kbio->ibuftl;
}
int f_writepro(void)
{
  return( f_alert1( msg_ptr[18] ) - 1 );
}
/*********************************************************************/
void from_filename( char *src, char *dest, int flg )
{    /* set flg for no trailing period when no extension */
  register int i=0;
  
  if( !*src ) *dest='\0';
  else
  {
    do
    {
      if( *src != ' ' ) *dest++ = *src;
      if( i++==7 && *(src+1) && *(src+1)!=' ' )
      {
        *dest++ = '.';
        flg++;
      }
    }
    while( *src++ && i<11 );
    if( i==11 ) *dest++ = '\0';
    if( !flg )
    {
      *(dest-1) = '.';
      *dest = '\0';
    }
  }
}
/*******************************************************************
int getcookie( long cookie, long *lc )
{
    long oldssp;
    register long *cookiejar;

    oldssp = Super((void *)0L);
    if( (cookiejar = *(long **)0x5a0) == 0 ) return 0;
    do {
        if (*cookiejar == cookie) {
            *lc = *(cookiejar+1);
            Super((void *)oldssp);
            return 1;
        }
        else cookiejar += 2;
    } while (*cookiejar);
    Super((void *)oldssp);
    return 0;
}
********************************************************************/
int neo_da( char *name )
{
/*	register char *p;*/
	register int i, id;
	char temp[14];

	if( get_acc_name( name, temp ) != 0 )
	{
/*	  while( p-temp < 8 ) *p++ = ' ';	004
	  *p = 0; */
	  if( (id = appl_pfind(temp)) >= 0 )
	    for( i=MAX_NEO_ACC; --i>=0; )
	      if( alt_acc[i]==id ) return id;
	}
	return -1;
}
void acc_init1( int id )
{
  int cmsg[8], i, j;
  extern NEO_ACC nac;
  extern int AES_handle;

  cmsg[0] = NEO_ACC_INI;
  cmsg[2] = 0;
  cmsg[3] = NEO_ACC_MAGIC;
  *(long *)&cmsg[4] = (long)&nac;
  cmsg[6] = cmsg[1] = AES_handle;
  appl_pwrite( id, 16, cmsg );
  for( i=0; i<MAX_NEO_ACC; i++ )
    if( alt_acc[i]<0 )
    {
      alt_acc[i] = id;
      break;
    }
}
void get_ack( int *buf, int dest )
{
  int buffer[8], last[10], sum, i, lnum;
  static EMULTI emulti = { MU_MESAG|MU_TIMER, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 1000, 0 };
  
  buffer[0] = lnum = 0;
  memclr( last, sizeof(last) );
  for(;;)
  {
    multi_evnt( &emulti, buffer );
    if( emulti.event&MU_MESAG )
      if( buffer[0] == DUM_MSG ) return;
      else if( buffer[0] == NEO_ACC_ASK && buffer[3]==NEO_ACC_MAGIC )
          acc_init1( buffer[4] );
      else
      {
        for( sum=0, i=0; i<8; i++ )
          sum += buffer[i]*(i+1);
        for( i=0; i<lnum; i++ )
          if( sum == last[i] ) return;
        if( lnum<10 ) last[lnum++] = sum;
        appl_write( AES_handle, 16, buffer );	/* send back to me */
      }
    if( emulti.event&MU_TIMER )
    {
      if( buf )
      {
        acc_init1( dest );
        appl_pwrite( dest, 16, buf );
        get_ack( 0L, 0 );
      }
      return;
    }
  }
}
/********************************************************************/
void init_screen(void)
{
  extern MOST *z;
  extern MASTER *mas;

  set_clip( z->cliparray, 1 );
  arrow();
  (*mas->clear_mouse)();
}
/********************************************************************/
void iso( char *path )
{
  *spathend(path) = '\0';
}
/********************************************************************/
void isolate(void)
{
  extern char filename[];
  
  iso(filename);
}
/*******************************************************************/
void free_clip(void)
{
  BUF_HDR *b, *b2;
  extern char *last_buf;

  b2 = (BUF_HDR *)last_buf;
  while( (b=b2)!=0 )
  {
    b2 = b->lastbuf;
    lfree(b);
  }
  last_buf = NULL;
  lfreeall(-2);
}
/********************************************************************/
#ifdef PRINT_ALLOCS
void _p_cmfree( char **ptr, char *file, int line )
{
  if( *ptr )
  {
    _p_lfree( *ptr, file, line );
    *ptr = NULL;
  }
}
#else
void cmfree( char **ptr )
{
  if( *ptr )
  {
    lfree(*ptr);
    *ptr = NULL;
  }
}
#endif
/********************************************************************/
void obj_enab( OBJECT *form, int truth, int num, ... )
{
  int *arg = &...;

  while( --num>=0 )
    if( !truth ) form[*arg++].ob_state |= DISABLED;
    else form[*arg++].ob_state &= ~DISABLED;
}
void obj_selec( OBJECT *form, int truth, int num, ... )
{
  int *arg = &...;

  while( --num>=0 )
    if( !truth ) form[*arg++].ob_state &= ~SELECTED;
    else form[*arg++].ob_state |= SELECTED;
}
void obj_true1( OBJECT *form, int truth, int start )
{
  obj_selec( form, truth, 1, start );
}
void obj_true( OBJECT *form, int truth, int start )
{
  truth = truth != 0;
  form[!truth+start].ob_state |= SELECTED;
  form[truth+start].ob_state &= ~SELECTED;
}
void obj_ltrue( OBJECT *form, int truth, int count, int start )
{
  int i;

  for( i=0; i<count; i++ )
    obj_selec( form, i==truth, 1, start++ );
}
/********************************************************************/
int is_sel( int num )
{
  extern OBJECT *form;

  return( form[num].ob_state&SELECTED );
}
int scan_sel( int start, int end )
{
  while( start < end )
    if( is_sel(start) ) return start;
    else start++;
  return 0;
}

/********************************************************************/
char *get_str( OBJECT *o, int num )
{
  return( o[num].ob_spec.tedinfo->te_ptext );
}

/********************************************************************/
int hide_if( OBJECT *form, int num, int truth )
{
  if( truth ) form[num].ob_flags &= ~HIDETREE;
  else form[num].ob_flags |= HIDETREE;
  return truth;
}
/********************************************************************/
/*%
int find_equiv( unsigned char *keys, OBJECT *ob, int obj )
{
  char *s, *e, *s0, k, shift=0;
  int ted, i;
  static char knames[][7]= { "ESC", "TAB", "RET", "RETURN", "BKSP", "BACKSP",
      "DEL", "DELETE", "HELP", "UNDO", "INS", "INSERT", "CLR", "HOME", "F1",
      "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "ENT", "ENTER",
      "KP(", "KP)", "KP/", "KP*", "KP7", "KP8", "KP9", "KP-", "KP4", "KP5",
      "KP6", "KP+", "KP1", "KP2", "KP3", "KP0", "KP.", "",  "",  "" },
              kscans[]   = { 1,     0XF,   0X1C,  0X1C,     0XE,    0XE,
      0X53,  0X53,     0X62,   0X61,   0X52,  0X52,     0X57,  0X57,   0X3B,
      0X3C, 0X3D, 0X3E, 0X3F, 0X40, 0X41, 0X42, 0X43, 0X44,  0X72,   0X72,
      0X63,  0X64,  0X65,  0X66,  0X67,  0X68,  0X69,  0X4A,  0X6A,  0X6B,
      0X6C,  0X4E,  0X6D,  0X6E,  0X6F,  0X70,  0X71,  0x50, 0x4d, 0x4b };

  if( !obj ) return 1;
  ob += obj;
  if( ob->ob_flags&HIDETREE ) return 0;
  if( (char)(ob->ob_type)==G_TITLE ) return 1;
  if( (ob->ob_state&X_MAGMASK) == X_MAGIC ) return 1;
  ob->ob_state &= 0xff;
  if( (s=s0=ob->ob_spec.free_string)!=0 && (e = s+strlen(s))!=s0 )
  {
    while( *--e==' ' )
      if( e==s0 ) return 1;
    s=e;
    while( *--s!=' ' )
      if( s==s0 ) return 1;
    if( s==s0+1 ) return 1;
    ob->ob_flags &= ~(7<<13);
    while( s<e ) switch( *++s )
    {
      case ' ':
      case 0:
        return 1;
      case '':
        shift |= 1;
        break;
      case '^':
        shift |= 2;
        break;
      case '':
      case '~':
        shift |= 4;
        break;
      default:
        if( s==e )
        {
          if( (k = *s)>='a' && k<='z' ) k&=0xdf;
          for( i=0; i<128; i++ )
            if( keys[i]==k )
            {
              ob->ob_state |= i<<8;
              ob->ob_flags |= shift<<13;
              return 1;
            }
        }
        else
        {
          k = *(e+1);
          *(e+1) = 0;
          for( i=sizeof(kscans); --i>=0; )
            if( !strcmpi( s, knames[i] ) ) break;
          *(e+1) = k;
          if( i>=0 )
          {
            i = kscans[i];
            if( i>=2 && i<=0xd && shift&4 ) i += 0x78-2;
            else if( i>=0x3b && i<=0x44 && shift&1 ) i += 0x54-0x3b;
            ob->ob_state |= i<<8;
            ob->ob_flags |= shift<<13;
          }
          return 1;
        }
    }
  }
  return 1;
}

void set_equivs( OBJECT *o, int parent )
{
  unsigned char *keys;
  int i;
  
  keys = Keytbl( (void *)-1L, (void *)-1L, (void *)-1L )->shift;
  for( i=o[parent].ob_head; i!=parent; i=o[i].ob_next )
    find_equiv( keys, o, i );
}
*/

static unsigned char mnk, mnush, mnfound;

int find_equiv(OBJECT *ob, int obj)
{
  if( mnfound ) return 0;
  if( !obj ) return 1;
  ob += obj;
  if( ob->ob_flags&HIDETREE ) return 0;
  if( (char)(ob->ob_type)==G_TITLE ) return 1;
  if( (ob->ob_state&X_MAGMASK) == X_MAGIC ) return 1;
  if( !(ob->ob_flags&DISABLED) && (ob->ob_state>>8) == mnk &&
      (unsigned)(ob->ob_flags)>>13 == mnush ) mnfound = obj;
  return 1;
}

int main_equivs( int *event, int sh, int key, int *buf )
{
  int parent, i, j, e;
  
  mnk = (unsigned char)key;
  mnush = ((sh&3) != 0) | ((sh&12)>>1);
  mnfound = 0;
  map_tree( menu, 0, -1, find_equiv );
  if( mnfound )
  {
    parent = find_parent(menu,mnfound);
    for( i=menu[e=menu[menu[0].ob_head].ob_next].ob_head, j=0; i!=e;
        i=menu[i].ob_next, j++ )
      if( i==parent )
      {
        for( i=2; --j>=0; i=menu[i].ob_next );
        buf[0] = MN_SELECTED;
        buf[1] = 0;
        buf[4] = mnfound;
        menu_tnormal( menu, buf[3]=i, 0 );
        *event |= MU_MESAG;
        return 1;
      }
  }
  return 0;
}

/********************************************************************/
#ifdef PRINT_ALLOCS
  int prn_alloc_hand;
  long _timer(void)
  {
    return *(long *)0x4ba;
  }
  void prnall( char *str, ... )
  {
    char temp[150];
    long in, all;

    if( !prn_alloc_hand )
    {
      Cconws( "Save alloc list? (y/N)" );
      if( ((char)Bconin(2)&0x5f) != 'Y' || (prn_alloc_hand = cFcreate("ALLOCS.TXT",0)) < 0 )
      {
        prn_alloc_hand = -1;
        return;
      }
    }
    if( prn_alloc_hand==-1 ) return;
    memstat( &in, &all );
    spf( temp, "%X ", Supexec(_timer) );
    (*mas->dopf)(temp+strlen(temp), str, (unsigned int *)&...);
    spfcat( temp, " \t%D of %D in use\r\n", in, all );
    cFwrite( prn_alloc_hand, (long)strlen(temp), temp );
  }
#endif

#ifdef PRINT_ALLOCS
void *_p_lalloc( long size, int id, char *file, int line )
{
  extern int AES_handle;
  void *out;
  
  if( id==-1 ) id = AES_handle-500;
  out = (void *)((*mas->lalloc)( size, id, 1 ));
  prnall( "%s %d: alloc(%D,%d) \tret=$%X", file, line, size, id, out );
  return out;
}
#else
void *lalloc( long size, int id )
{
#ifdef DEBUG
  extern int AES_handle;
  
  if( id==-1 ) id = AES_handle-500;
#endif
  return (void *)((*mas->lalloc)( size, id, 1 ));
}
#endif

#ifdef PRINT_ALLOCS
void *_p_lalloc2( long size, int id, char *file, int line )
{
  extern int AES_handle;
  void *out;
  
  if( id==-1 ) id = AES_handle-500;
  out = (void *)((*mas->lalloc)( size, id, 0 ));
  prnall( "%s %d: alloc(%D,%d) \tret=$%X", file, line, size, id, out );
  return out;
}
#else
void *lalloc2( long size, int id )
{
#ifdef DEBUG
  extern int AES_handle;
  
  if( id==-1 ) id = AES_handle-500;
#endif
  return (void *)((*mas->lalloc)( size, id, 0 ));
}
#endif

#ifdef PRINT_ALLOCS
int _p_lfree( void *xfb, char *file, int line )
{
  int out = (*mas->lfree)( xfb );

  prnall( "%s %d: free($%X) \tret=%d", file, line, xfb, out );
  return out;
}
#else
int lfree( void *xfb )
{
  return (*mas->lfree)( xfb );
}
#endif

int lfreeall( int id )
{
#ifdef DEBUG
  extern int AES_handle;
  
  if( id==-1 ) id = AES_handle-500;
#endif
  return (*mas->lfreeall)( id );
}
void memstat( long *in_use, long *allocated )
{
  (*mas->memstat)( in_use, allocated );
}
#ifdef PRINT_ALLOCS
int _p_lshrink( void *block, long newsiz, char *file, int line )
{
  int out = (*mas->lshrink)( block, newsiz );

  prnall( "%s %d: lshrink($%X,%D) \tret=%d", file, line, block, newsiz, out );
  return out;
}
#else
int lshrink( void *block, long newsiz )
{
  return (*mas->lshrink)( block, newsiz );
}
#endif

#ifdef PRINT_ALLOCS
int _p_lrealloc( void **xfb, long size, char *file, int line )
{
  void *old = *xfb;
  int out = (*mas->lrealloc)( xfb, size, 1 );

  prnall( "%s %d: realloc(%X(%X->%X),%D) \tret=%d", file, line, xfb, old, *(long *)xfb, size, out );
  return out;
}
#else
int lrealloc( void **xfb, long size )
{
  return (*mas->lrealloc)( xfb, size, 1 );
}
#endif
int add_thing( void **start, int *total, int *remain, void *add,
    int addinc, long size, int id )
{
  if( !*start )
  {
    *total = 0;
    if( (*start = lalloc( (*remain=addinc)*size, id )) == 0 ) return 0;
  }
  else if( !*remain )
    if( lrealloc( start, ((*remain=addinc)+*total)*size ) ) return 0;
  if( add ) memcpy( (char *)(*start) + (*total * size), add, size );
  ++(*total);
  --(*remain);
  return 1;
}
int add_string( void **start, int *total, int *remain, void *add,
    int addinc, long len, int id )
{
  if( !*start )
  {
    *total = 0;
    if( (*start = lalloc( *remain=addinc, id )) == 0 ) return 0;
  }
  else if( *total + len > 32767L ) return 0;	/* goes past 32K */
  if( *remain < len )
    if( lrealloc( start, (*remain=addinc+len)+*total ) ) return 0;
  if( add ) memcpy( (char *)(*start) + *total, add, len );
  *total += len;
  *remain -= len;
  return 1;
}

/********************************************************************/
void cat_date( char *ptr, int a, int b, int c, int zero )
{
  char sep, *str;
  
  if( c>99 ) c -= 100;
  if( (sep = (char)z->idt_fmt) == 0 ) sep = '/';
  ptr += strlen(ptr);
  str = "%02d%c%02d%c%02d ";
  switch( (int)z->idt_fmt&0xf00 )
  {
    case 0x000:
      if( !zero ) str = "%d%c%d%c%02d";
      spf( ptr, str, a, sep, b, sep, c );
      break;
    case 0x100:
      if( !zero ) str = "%d%c%d%c%02d";
      spf( ptr, str, b, sep, a, sep, c );
      break;
    default:
      if( !zero ) str = "%02d%c%d%c%d";
      spf( ptr, str, c, sep, a, sep, b );
      break;
    case 0x300:
      if( !zero ) str = "%02d%c%d%c%d";
      spf( ptr, str, c, sep, b, sep, a );
  }
}
/********************************************************************/
void lock_drive( int drive, int handle, int lock )
{
  extern LoadCookie *lc;
  static int lock_hand[4];
  char *ptr;
  int *lh = lock_hand, i;
  
  ptr = (*lc->lock_drive);
  if( lock )
  {
    for( i=0; i<4; i++, ptr++, lh++ )
    {
      if( *ptr<0 ) break;
      if( *ptr==drive ) return;
    }
    if( i==4 ) return;
    *ptr = drive;
    *lh = handle;
  }
  else for( i=0; i<4; i++, ptr++, lh++ )
    if( *lh==handle ) *ptr = -1;
}
int is_locked( int drv )
{
  char *ptr;
  int i;
  
  if( drv>='A' ) drv -= 'A';
  ptr = (*lc->lock_drive);
  for( i=4; --i>=0; ptr++ )
    if( *ptr==drv ) return 1;
  return 0;
}
/********************************************************************/
  #define MAX_PLANES4 4    /* 4 data planes */
  #define MAX_W       2    /* width in words */
  #define MAX_H       32   /* height in pixels */
  #define MAX_DATA4   ((MAX_PLANES4+1)*MAX_W*MAX_H*2)
         /* (4 data planes + 1 mask plane) * (normal + seleced) */
  #define OLD_PLANES 2	   /* planes in a Neo 3 file */
  #define MAX_SIZE4   ((MAX_PLANES4<<4)|MAX_PLANES4)
  typedef struct
  {
    unsigned char size_x, size_y;
    unsigned char readicon;	/* set to 0, 1, or 2 (2, 4, 16 planes) */
    unsigned char norm_read, sel_read;	/* changed to number of planes read */
    unsigned char unused;
    unsigned char xchar, ychar;
    unsigned char planes;      /* set to MAX_SIZE4 before calling */
    unsigned char type;              /* use ICON_TYPE to convert */
    char text[12];            /* see old icon format for details */
    unsigned char colors[OLD_PLANES];
    int data[MAX_DATA4];
  } NEO_ICON;

static char n2pl[] = { 1, 2, 4 };

static int plane_size( int pl )
{
  return pl*DATASIZ;
}

int alloc_im( int **p, int pl )
{
  return (*p = (int *)lalloc( plane_size(pl), -1 )) != 0;
}

/* copy a CICON, allocating memory for copies of all elements */
int copy_cicon( int pl, CICON *from, CICON *to )
{
  int i, **p, **q;

  memclr( to, sizeof(CICON) );
  to->num_planes = pl;
  if(from)
  {
    if( from->col_data && !alloc_im( &to->col_data, pl ) ||
        from->col_mask && !alloc_im( &to->col_mask, 1 ) ||
        from->sel_data && !alloc_im( &to->sel_data, pl ) ||
        from->sel_mask && !alloc_im( &to->sel_mask, 1 ) )
    {
      cmfree( (char **)&to->col_data );
      cmfree( (char **)&to->col_mask );
      cmfree( (char **)&to->sel_data );
      cmfree( (char **)&to->sel_mask );
      return 0;
    }
    for( i=4, p=&to->col_data, q=&from->col_data; --i>=0; p++, q++ )
      if( *p ) memcpy( *p, *q, plane_size(i&1 ? pl : 1) );
  }
  return 1;
}

int dflt_rgb[16][3];

int fix_rez( int *data, int **out, int pl, int dplanes, int (*rgb)[16][3], int devspef )
{
  int s;
  PICTURE pic = { { 0L, 32, 32, 2, 1, 0, 0, 0, 0 }, 16 };  /* NOT static! */

  if( data )
  {
    if( (pic.mfdb.fd_addr = lalloc( s=plane_size(pl), -1 )) == 0 ) return 0;
    memcpy( pic.mfdb.fd_addr, data, s );
    set_temps(dplanes<0?pl:dplanes);
    pic.intens = !rgb ? dflt_rgb : rgb;
    pic.mfdb.fd_nplanes = pl;
    if( transform_pic( &pic, devspef ) )
    {
      lfree( pic.mfdb.fd_addr );
      return 0;
    }
    *out = pic.mfdb.fd_addr;
    return 1;
  }
  return 1;
}

int fix_icon( ICONBUF *ib )
{
  CICON *c, **ci;
  int i;
  
  for( ci=&ib->nicb.list[2], i=3; --i>=0; ci-- )
    if( *ci && (*ci)->num_planes <= graphics->vplanes )
    {
      if(i)
      {
        if( (c=lalloc( sizeof(CICON), -1 )) == 0 ) return 0;
        memcpy( c, *ci, sizeof(CICON) );
        if( c->col_data )
          if( !fix_rez( c->col_data, &c->col_data, c->num_planes,
              graphics->vplanes, 0L, -1 ) ) return 0;
        if( c->sel_data )
          if( !fix_rez( c->sel_data, &c->sel_data, c->num_planes,
              graphics->vplanes, 0L, -1 ) ) return 0;
        c->num_planes = graphics->vplanes;
        ib->nicb.ci = c;
      }
      else ib->nicb.ci = *ci;
      return 1;
    }
  return 1;
}

#define KEY_START 0x37
char key;
static void encrypt( void *from, void *to, long size )
{
  while( --size >= 0 )
  {
    *((char *)to)++ = *((char *)from)++ ^ key;
    key += 0x21;
  }
}

int read_nic_header( int hand, unsigned int *entries, void *ni, 
                 unsigned int *codelen )
{
  char buf[sizeof(NIC_INFO)+4];
  
  if( cFread( hand, sizeof(buf), buf ) == sizeof(buf) )
  {
    key = KEY_START;
    encrypt( buf, entries, 2 );
    key = KEY_START;
    encrypt( buf+2, ni, sizeof(NIC_INFO) );
    key = KEY_START;
    encrypt( buf+2+sizeof(NIC_INFO), codelen, 2 );
    return(1);
  }
  return(0);
}

int *resize_icon( int sx, int sy, int *dptr, int *from, int pl )
{
  int i, j, h;
  
  h = sy >= ICON_H ? ICON_H : sy;
  while( --pl>=0 )
  {
    for( j=h; j--; )
    {
      for( i=0; i<sx; i++ )
        *dptr++ = *from++;
      if( sx==1 ) *dptr++ = 0;
    }
    memclr( dptr, i=(ICON_H-h)<<2 );
    dptr += i;
  }
  return from;
}

void reset_icbs( ICONBUF *icb, int i, int max )
{
  icb += i;
  for( ; i<max; i++, icb++ )
  {
    icb->nicb.ib = &icb->icb;
    icb->icb.ib_ptext = icb->text;
  }
}

extern NIC_INFO nic_info;
extern int num_icons, icons_rem, user_icons;
extern ICONBUF *nic_icons;
extern OBJECT *icons, *deskpat;
extern char *icon_buf;
extern ICIC *icic;

ICONBUF *add_icon( ICONBUF **start, int *num_icons, int *icons_rem )
{
  ICONBUF *icb, *old;
  ICONBLK *icon;

  old = *start;
  if( !add_thing( (void **)start, num_icons, icons_rem,
      0L, 5, sizeof(ICONBUF), -1 ) ) return 0L;
  if( old != *start ) reset_icbs( *start, 0, *num_icons );
  icb = *start + *num_icons - 1;
  memclr( icb, sizeof(ICONBUF) );
  icb->nicb.ib = &icb->icb;
  icb->type = NOT_DFLT;
  memcpy( &icb->icb, icons[1].ob_spec.iconblk, sizeof(ICONBLK) );
  strcpy( icb->icb.ib_ptext = icb->text, "           " );
  icb->icb.ib_pdata = icb->icb.ib_pmask = 0L;
  return icb;
}

char *nic_rbuf;
long nic_start, nic_pos, nic_len;

void nic_jmp( char *code, int off, long jmp )
{
  code += off;
  *(int *)code = 0x4ef9;
  *(long *)(code+2) = jmp;
}
#pragma warn -par
long nic_Fseek( long off, int hand, int mode )
{
  if( off < nic_start ) off = nic_start;
  if( off > nic_len ) off = nic_len;
  return nic_pos = off;
}
long nic_Fread( int hand, long count, char *buf )
{
  if( count+nic_pos > nic_len ) count = nic_len-nic_pos;
  if( count )
  {
    memcpy( buf, nic_rbuf+nic_pos-nic_start, count );
    nic_pos += count;
  }
  return count;
}
#pragma warn +par

int read_nic( int hand, int some, NIC_INFO *nif, ICONBUF **start, int *num_icons, int *icons_rem )
{
  int err=1, softerr=0, i, j, *from, rez, pl;
  int buf[4];	/* must be word-aligned */
  unsigned int codelen, entries, entry;
  long l, l2, len;
  char *code, neo4, fast=0;
  ICONBLK *icb;
  NEO_ICON ni;
  ICON_TYPE it;
  ICONBUF *ib;
  CICON ci, *new;

  for( j=16; --j>=0; )
  {
    l = setcolor( j, -1L );
    for( i=1; i<4; i++ )
      unpak_rgb( (unsigned char *)&l+i, &dflt_rgb[j][i-1] );
  }
  cFread( hand, 8L, buf );
  if( *(long *)buf != 0x2E4E4943 || buf[2] != NIC_VER ) return -1;
  i = *(char *)&buf[3] - 1;	/* -1 because version byte of copyright already read */
  if( (neo4 = *((char *)&buf[3]+1) == '\x4') != 0 )
  {
    cFread( hand, 6L, buf );	/* buf[1]=Fseek, buf[2]=Fread */
    if( (char)buf[0]==0 ) fast++;
    i -= 6;
  }
  cFseek( i, hand, 1 );
  if( read_nic_header( hand, &entries, nif, &codelen ) )
  {
    err=0;
    if( (code=lalloc(codelen,-1)) == 0 ) return(0);
    else
    {
      if( cFread( hand, codelen, code ) != codelen ) err++;
      else
      {
        if( fast && (long)lalloc(-1L,-1) > (l=(long)entries*	/* 004: fixed this calc */
            (sizeof(ICONBUF)+sizeof(CICON)*2		/* read CICON + fixed CICON */
            +(long)DATASIZ*
            (some?(graphics->vplanes>4?4:graphics->vplanes)*2+2:4+6+10))) &&
            (from = lalloc(l,-1)) != 0 )	/* 004: grab memory temporarily to avoid a hole later on */
        {
          l = cFseek( 0L, hand, 1 );
          len = cFseek( 0L, hand, 2 ) - l;
          if( (nic_rbuf = lalloc(len,-1)) != 0 )
          {
            cFseek( l, hand, 0 );
            if( !TOS_error( l2=cFread( hand, len, nic_rbuf ), 0 ) ||
                l2!=len ) err++;
            else
            {
              nic_jmp( code, buf[1], (long)nic_Fseek );
              nic_jmp( code, buf[2], (long)nic_Fread );
              nic_pos = nic_start = l;
              nic_len = len+l;
            }
          }
          lfree(from);		/* 004 */
        }
        for( entry=0; entry<entries && !err; entry++ )
        {
          for( ib=0L, rez=3; --rez>=0; )  /* go in this order so that best rez will be hit first if some */
          {
            pl = n2pl[rez];
            if( some && pl > graphics->vplanes ) continue;
            ni.size_x = MAX_W;
            ni.size_y = MAX_H;
            ni.planes = neo4 ? MAX_SIZE4	/* Neo 4 format: use 0x44 */
                  : OLD_PLANES;			/* Otherwise use 2 */
            ni.readicon = rez;
            if( (i=(*(int (*)( int h, int ent, NEO_ICON *ni ))code)
                ( hand, entry, &ni )) == -2 ) softerr=1;       	/* used to set err++ */
            else if( !i )
              if( !ni.type )	/* desktop pattern */
              {
                if( (j=graphics->vplanes)>8 ) j = 8;
                j = 1<<j;
                if( ni.colors[1]>=j )
                  if( ni.colors[0]>=j )
                  {
                    ni.colors[1] = 0;
                    ni.colors[0] = 1;
                  }
                  else ni.colors[1] = 1;
                else if( ni.colors[0]>=j ) ni.colors[0] = 1;
                i = (ni.colors[1]<<4)|ni.colors[0];
                if( nif==&nic_info )	/* default NIC file */
                {
                  memcpy( deskpat->ob_spec.bitblk->bi_pdata, ni.data, 32 );
                  deskpat->ob_spec.bitblk->bi_color = i;
                }
                else if( !(*icic->add_nic_patt)( ni.data, i ) ) err++;
                break;
              }
              else if( ib || (ib = add_icon( start, num_icons, icons_rem )) != 0 )
              {
                icb = &ib->icb;
                from = ni.data;
                if( !neo4 )
                {	/* rez is not correct if !neo4 */
                  if( !alloc_im( &icb->ib_pdata, 1 ) || !alloc_im( &icb->ib_pmask, 1 ) )
                  {
                    err++;
                    break;
                  }
                  from = resize_icon( ni.size_x, ni.size_y, icb->ib_pmask, from, 1 );
                  resize_icon( ni.size_x, ni.size_y, icb->ib_pdata, from, 1 );
                }
                else if( (new = ib->nicb.list[rez] = lalloc( sizeof(CICON), -1 )) == 0 )
                {
                  err++;
                  break;
                }
                else
                {
                  memclr( &ci, sizeof(CICON) );
                  j = ni.size_x * ni.size_y;
                  if( ni.norm_read & 0xF0 )
                  {	/* has a normal mask */
                    ci.col_mask = from;
                    from += j;
                  }
                  if( ni.norm_read & 0x0F )
                  {	/* has normal data */
                    ci.col_data = from;
                    from += n2pl[rez]*j;
                  }
                  if( ni.sel_read & 0xF0 )
                  {	/* has a selected mask */
                    ci.sel_mask = from;
                    from += j;
                  }
                  if( ni.sel_read & 0x0F )
                  {	/* has selected data */
                    ci.sel_data = from;
                  }
                  copy_cicon( n2pl[rez], &ci, new );
                  if( !rez )
                  {
                    icb->ib_pdata = new->col_data;
                    icb->ib_pmask = new->col_mask;
                  }
                }
                it.i = ni.type;
                ib->type = it.type.dflt>=0 ? it.type.dflt : NOT_DFLT;
                strcpy( icb->ib_ptext, ni.text );
                icb->ib_char = (it.type.file?0:1)|(it.type.folder?0:2)|
                    (it.type.drive?0:4)/*003*/|(ni.colors[0]<<8)|
                    (ni.colors[1]<<12);
                icb->ib_xchar = ni.xchar;
                icb->ib_ychar = ni.ychar;
                if( some ) break;	/* got the best icon */
              }
              else
              {
                err++;
                break;
              }
            if( !neo4 ) break;	/* go to next icon */
          }
          if( ib && !fix_icon(ib) ) err++;  /* ib==0L if pattern */
        }
        cmfree( &nic_rbuf );
      }
      lfree(code);
    }
  }
  if( err || softerr ) f_alert1( msg_ptr[174] );
  return(1);
}

void free_icon( CICON **ci, CICON *curr, int root )
{
  int j, **p, **q;

  if( *ci )
  {
    if( curr )
      for( j=4, p=&curr->col_data, q=&(*ci)->col_data; --j>=0; p++, q++ )
        if( *p == *q ) *q = 0L;	/* don't free ci element if used in curr */
    for( j=4, p=&(*ci)->col_data; --j>=0; p++ )
      cmfree( (char **)p );
    if( root ) cmfree((char **)ci);
  }
}

void free_nib_icon( NICONBLK *n )
{
  int rez;
  
  for( rez=3; --rez>=0; )
    if( !rez && n->list[0]==n->ci )
      if( !n->ci )		/* 003 */
      {
        cmfree( (char **)&n->ib->ib_pdata );
        cmfree( (char **)&n->ib->ib_pmask );
      }
      else n->list[0] = 0L;
    else free_icon( &n->list[rez], n->ci, 1 );
  free_icon( &n->ci, 0L, 1 );
}

void free_iconbuf( ICONBUF **ib, int start, int *num, int *rem )
{
  int count=0;
  ICONBUF *i = *ib;
  
  i += start;
  while( start++<*num )
  {
    free_nib_icon( &i->nicb );
    count++;
    i++;
  }
  if( (*num -= count) == 0 ) cmfree( (char **)ib );
  else if( rem ) *rem += count;
}

void free_nic( ICONBUF **i, int *num )
{
  free_iconbuf( i, 0, num, 0L );
  cmfree( (char **)i );
}

void get_icon_names(void)
{
  int j;
  char *p;
  ICONBUF *ib;

  cmfree( &icon_buf );
  if( (j = num_icons-D_PROG+1) > 0 && (p = icon_buf = lalloc(13*j,-1)) == 0 ) return;
  for( j=D_PROG-1, ib=nic_icons+D_PROG-1; j<num_icons; j++, ib++, p+=13 )
    from_filename( ib->text, p, 0 );
  user_icons = num_icons-D_PROG+1;	/* 002: moved here */
}

int read_dflt_nic( int some )
{
  int h, i, j;
  char temp[120];
  ICONBUF *ib;
  
  strcpy( temp, z->dflt_path );
  strcat( temp, "NEOICONS.NIC" );
  user_icons = 0;
  free_nic( &nic_icons, &num_icons );
  if( (h = cFopen( temp, 0 )) > 0 )
  {
    i = read_nic( h, some, &nic_info, &nic_icons, &num_icons, &icons_rem );
    cFclose(h);		/* 002: used to set user_icons here */
  }
  else i=0;
  /* remember: nic_icons[NPI] is the first user icon! */
  if( num_icons<D_PROG )
  {
    free_nic( &nic_icons, &num_icons );
    for( j=0; j<D_PROG-1; j++ )
    {
      if( (ib = add_icon( &nic_icons, &num_icons, &icons_rem )) == 0 ) error(99);
      memcpy( &ib->icb, icons[j+1].ob_spec.iconblk, sizeof(ICONBLK) );
      strcpy( ib->icb.ib_ptext = ib->text, icons[j+1].ob_spec.iconblk->ib_ptext );
      ib->type = j;
    }
  }
  else
    for( j=0, ib=nic_icons; j<num_icons; j++, ib++ )
    {
      if( ib->type==NPI ) ib->type++;
/*%      h = j>=D_PROG-1 ? TEXT+1 : j+1;
      memcpy( &ib->icb, icons[h].ob_spec.iconblk, sizeof(ICONBLK) );
      ib->icb.ib_ptext = ib->text; */
      if( ib->nicb.list[0] )	/* set mono color icon's data into iconblk, if avail */
      {
        ib->icb.ib_pdata = ib->nicb.list[0]->col_data;
        ib->icb.ib_pmask = ib->nicb.list[0]->col_mask;
      }
    }
  get_icon_names();
  return i;
}


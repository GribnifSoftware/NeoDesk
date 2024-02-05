/* NeoDesk 3.02 by Dan Wilga
   Copyright � 1990, Gribnif Software.
   All Rights Reserved.
*/
#include "new_aes.h"
#include "tos.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "lerrno.h"
#include "ierrno.h"
#include "neodesk.h"
#include "neocommn.h"
#include "xwind.h"
#include "about.rsh"
#define EXTERN extern
#include "about.rh"
#include "neod1_id.h"
#include "mwclinea.h"
#include "cattgios.h"

#define ABOUT_VER 6		/* in About */

#define _VDO_COOKIE 0x5F56444FL
#define _MCH_COOKIE 0x5F4D4348L
#define _CPU_COOKIE 0x5F435055L
#define MiNT_COOKIE 0x4D694E54L
#define MagX_COOKIE 0x4D616758L

extern void memclr( void *ptr, unsigned long num );
void breakpoint(void) 0x4afc;

#define rindex strrchr
#define index  strchr
#define about  rs_object

#define LINE_BUF_LEN 258        /* must be long enough for table+1 */

char *pathend( char *ptr );
int TOS_error( long num, int errs, ERRSTRUC errstruc[] );
int redirect( int *fh, int *sh );
int execute( int s_h, int f_h, int o_h );

MOST z;
GRAPHICS *graphics;
TEDINFO ted = { "                                                ",
     "", "", IBM, 0, TE_CNTR, 0x1180,
  /* (BLACK<<12)|(BLACK<<8)|(1<<7)|(7<<4), */  0, -1, 12, 1 };
OBJECT blank[] = { { -1, 1, 1, G_BOX, 0, 0, (long)(4<<4)|3 },
                   { 0, -1, -1, G_BOXTEXT, 32, 0, (long)&ted } };
char *new_msgs;

ERRSTRUC dflt_errors[DFLT_ERRORS] = {
        { IERROR, 0L }, { IEDRVNR, 0L }, { IEWRPRO, 0L }, { IE_CHNG, 0L },
        { IEUNDEV, 0L }, { IEBADSF, 0L }, { IEINVFN, 0L }, { IEFILNF, 0L },
        { IEPTHNF, 0L }, { IENHNDL, 0L }, { IEACCDN, 0L }, { IENSMEM, 0L },
        { IEDRIVE, 0L }, { IEPLFMT, 0L } };

char line_buf[LINE_BUF_LEN], diskbuf[1304], neo[120], not_arcd;

int dum;

ERRSTRUC errstruc[2] = { { IEPTHNF }, { IEFILNF } };

long fpos;

LoadCookie *lc;

char is_MSTe, is_030, is_falc, has_mint, has_gnva, *gr_start, aes_ge_40, have_about,
    is_magx_shel, has_catt, gnva_ge004;
int char_w, char_h, sw_bits, sw0, sw_argv;
Rect about_rect;

struct
{
  int num;
  ERRSTRUC *errstruc;
} noerrs;

/********************************************************************/
void cdecl spf( char *buf, char *fmt, ... ) {
  (*lc->dopf)( buf, fmt, (unsigned int *)&... );
}
/********************************************************************/
void shorten_bb( BITBLK *bb )
{
  int i, j, *ptr, wb, wb2, extra, carry;

  /* clear-out every second line in the image data */
  wb2 = (wb = bb->bi_wb)>>1;
  extra = bb->bi_hl&1;
  for( j=bb->bi_hl>>1, ptr=(int *)((long)bb->bi_pdata+wb); --j>=0; ptr+=wb )
  {
    if( extra || j ) for( i=0, carry=0; i<wb2; i++ )
    {
      *(ptr+wb2+i) |= (*(ptr+i)>>1) | carry;
      carry = (*(ptr+i)&1)<<15;
    }
    memclr( ptr, wb );
  }
  /* now, expand the image description so that every second (used)
     line becomes tacked onto the end of the previous line */
/*  bb->bi_x += bb->bi_wb<<3;*/
  bb->bi_hl >>= 1;
  bb->bi_wb <<= 1;
}
void fixx2( int *i )
{
  *i = ((*i&0xFF)*char_w) + (*i>>8);
}
void fixy2( int *i )
{
  *i = ((*i&0xFF)*char_h) + (*i>>8);
}
int obfix( OBJECT *o, int count )
{
  int *i;

  for( ; --count>=0; o++ )
  {
    i = &o->ob_x;
    fixx2( i++ );
    fixy2( i++ );
    fixx2( i++ );
    fixy2( i );
  }
  return(1);
}
void init_about(void)
{
  int i, j, mult=3;

  rs_tedinfo[0].te_ptext = msg_ptr[27];
  rs_tedinfo[2].te_ptext = msg_ptr[28];
  rs_object[COPDATE-1].ob_spec.free_string = msg_ptr[29];
  rs_object[ABHELP].ob_spec.free_string = msg_ptr[24];
  j = about[COPLOGO].ob_height;
  fixy2(&j);
  if( j < about[COPLOGO].ob_spec.bitblk->bi_hl )
  {
    shorten_bb(&rs_bitblk[0]);
    shorten_bb(&rs_bitblk[1]);
    mult = 2;
  }
  obfix( rs_object, NUM_OBS );
  /* center all lines of About box */
  j = (about[COPLOGO].ob_width = about[COPLOGO].ob_spec.bitblk->bi_wb<<mult) + 8;
  if( (i=about[0].ob_width) > j ) j = i;
  if( (i=about[5].ob_width) > j ) j = i+8;
  about[0].ob_width = j;
  for( i=2; i<ABHELP-1; i++ )
    if( about[i].ob_next==i+1 ) about[i].ob_x = (j-about[i].ob_width)>>1;
/*  about[ABHELP-1].ob_x = about[ABHELP-2].ob_x;*/
  about[1].ob_x = j-(about[1].ob_width=16);
#ifndef DEMO
  spf( about[COPDATE].ob_spec.tedinfo->te_ptext, "4.%02x %s", ABOUT_VER, __DATE__ );
#else
  about[COPDATE-1].ob_spec.free_string[0] = 0;
  about[COPDATE].ob_spec.tedinfo->te_ptext[0] = 0;
#endif DEMO
  form_center( about, &about_rect.x, &about_rect.y, &about_rect.w, &about_rect.h );
  if( about_rect.x>2 )
  {
    about_rect.x -= 3;
    about_rect.y -= 3;
    about_rect.w += 6;
    about_rect.h += 6;
  }
  about[COPMOVE].ob_flags |= HIDETREE;
  about[ABHELP].ob_flags |= HIDETREE;
  wind_update( BEG_UPDATE );
  form_dial( FMD_START, 0, 0, 0, 0, Xrect(about_rect) );
  objc_draw( about, 0, 8, Xrect(about_rect) );
  have_about = 1;
  about[COPMOVE].ob_flags &= ~HIDETREE;
  about[ABHELP].ob_flags &= ~HIDETREE;
}
void kill_about(void)
{
  if( have_about )
  {
    wind_update( END_UPDATE );
    form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect(about_rect) );
    have_about = 0;
  }
}
/********************************************************************/
void bconws( char *string )
{
  (*lc->bconws)(string);
}
/********************************************************************/
void cdecl clear_mouse( void )
{
  register int i;
  extern linea0(void);

  if( has_gnva ) graf_mouse( X_MRESET, 0L );
  else /* 004 if( !has_mint || !aes_ge_40 )*/ /* !MultiTOS */
  {
    wind_update( BEG_UPDATE );
    for( i=10; --i>=0; )
      graf_mouse( M_OFF, 0L );
/*%    {
      linea0();
      mousehidden = 1;
      for( i=20; mousehidden && --i>=0; )
        graf_mouse( M_ON, 0L );
    }
    else*/ (*graphics->reset_mouse)( graf_mouse );
    wind_update( END_UPDATE );
  }
}
/********************************************************************/
int cdecl dsetpath( char *path )
{
  char temp[120];
  register long ret;
  DTA dma, *old;

  old = Fgetdta();
  Fsetdta(&dma);
  strcpy( temp, path );
  strcpy( temp+3, "*.*" );
  if( (ret=Fsfirst( temp, 0x37 )) == 0 || ret==AENMFIL )
      ret = Dsetpath( path+2 );
  Fsetdta(old);
  return(ret);
}
/********************************************************************/
long readlen, buflen, maxread;
int readerr, readhand;
char *readbuf, *iptr;
#define N	 4096	/* size of ring buffer */
#define F	   18	/* upper limit for match_length */
#define THRESHOLD   2   /* encode string into position and length
						   if match_length is greater than this */
#define NIL	N	/* index for root of binary search trees */

unsigned char *text_buf;
#define TBUFSIZ		(N + F - 1)	/* ring buffer of size N,
  with extra F-1 bytes to facilitate string comparison */
/*%
int _getc(void)
{
  if( !maxread ) return EOF;
  if( !readlen )
    if( (readlen = Fread( readhand, maxread>buflen?buflen:maxread,
        iptr=readbuf )) == 0L ) return AEPLFMT;
    else if( readlen<0 )
    {
      readerr = readlen;
      return EOF;
    }
  readlen--;
  maxread--;
  return *((unsigned char *)iptr)++;
}
int decomp( long size, int hand, char *optr )
{
  int  i, j, k, r, c;
  unsigned int  flags;

  if( (buflen=(long)Malloc(-1)) > sizeof(diskbuf) ) readbuf = Malloc(buflen);
  else
  {
    readbuf = diskbuf;
    buflen = sizeof(diskbuf);
  }
  readlen = 0;
  readerr = 0;
  readhand = hand;
  maxread = size;
  memset( text_buf, 0, N-F );
  r = N - F;  flags = 0;
  for ( ; ; ) {
    if (((flags >>= 1) & 256) == 0) {
        if ((c = _getc()) < 0) break;
	flags = c | 0xff00;		/* uses higher byte cleverly */
    }							/* to count eight */
    if (flags & 1) {
	if ((c = _getc()) < 0) break;
	*optr++ = c;
	text_buf[r++] = c;
	r &= (N - 1);
    } else {
	if ((i = _getc()) < 0) break;
	if ((j = _getc()) < 0) break;
	i |= ((j & 0xf0) << 4);
	j &= 0x0f;
	j += THRESHOLD + 1;
	for (k = 0; --j >= 0; k++) {
	    *optr++ = text_buf[r++] = text_buf[(i + k) & (N - 1)];
	    r &= (N - 1);
	}
    }
  }
  if( readbuf != diskbuf ) Mfree(readbuf);
/*%  if(!l) *optr++ = 0; */
  return(readerr);
}  ****/
int decomp( long size, int hand, char *optr );
int exe_hand;

void execlose( int open )
{
  if( !open && exe_hand>0 )
  {
    Fclose(exe_hand);
    exe_hand = 0;
  }
}

void close_exe(void)
{
  execlose(0);
}

void cond_mouse( int mode )
{
  if( !z.multitask ) graf_mouse( mode, 0L );
}

long _exe_load( int mode, char *name, void *a0, int d0, char **strt, int open );

/********************************************************************/
void prep_exec(void)
{
  static char kick[]="x:\\*.*";
  DTA dma, *old;

  if( exe_hand<=0 )
  {
    old = Fgetdta();
    Fsetdta(&dma);
    strcpy( neo, z.dflt_path );
    strcat( neo, "NEODESK.EXE" );
    kick[0] = neo[0];
    not_arcd = Fsfirst(kick,0x37);
    not_arcd = Fsfirst(neo,0x37);
    Dsetdrv( (neo[0]&0x5F) - 'A' );
    Fsetdta(old);
  }
}

int test_err( long err )
{  
  char filename[150];
  
  if( (err == AEFILNF || err == AEPTHNF) && neo[0]<'C' )
  {
    clear_mouse();
    spf( filename, msg_ptr[MAS0+8], neo[0] );
    if( f_alert1( filename ) == 1 ) return 1;
  }
  else TOS_error( err, 2, errstruc );
  return 0;
}

long exe_load( int mode, char *name, void *a0, int d0, char **strt, int open )
{
  long l;
  
  do
  {
    prep_exec();
    l = _exe_load( mode, name, a0, d0, strt, open );
  }
  while( test_err(l) );
  return l;
}

long _exe_load( int mode, char *name, void *a0, int d0, char **strt, int open )
/* mode:  0: run and remove (NEODESK2), 1: allocate extra (ICONEDIT),
          2: run and keep (graphics) */
{
  int hand, *table;
  char *start, was_open, *prog, *extra;
/*  unsigned char *uptr;*/
  long l, len;
  struct
  {
    int magic;
    long text, data, bss, sym;
  } prghdr;
  struct
  {
    char name[12];
    long next, text, data, bss, fixup, total;
  } archdr;
  extern long _StkLim;
  BASPAG *bp;
  void fixup( unsigned char *uptr, long *l, long start );

  if( not_arcd )
  {
    strcpy( rindex(neo,'\\')+1, name );
    if( mode )
    {
      bp=(BASPAG *)(l=Pexec( 3, neo, (COMMAND *)"\0", 0L ));
      if( l > 0 )
      {
        Mshrink( 0, bp, bp->p_tlen+bp->p_dlen+bp->p_blen+sizeof(BASPAG) );
        l = ((long(*)(void))bp->p_tbase)();
      }
    }
    else
    {
      l = Pexec( 0, neo, (COMMAND *)"\0", 0L );
    }
    return l;
  }
  if( (was_open = exe_hand > 0) != 0 ||
      (exe_hand=hand=Fopen( neo, 0 )) > 0 )
  {
    cond_mouse( BUSYBEE );
    hand = exe_hand;	/* in case exe_hand>0 */
    if( was_open || (l=Fread( hand, sizeof(prghdr), &prghdr )) == sizeof(prghdr) &&
        prghdr.sym && (l=Fseek( len=prghdr.text+prghdr.data+0x1c,
        hand, 0 )) == len )
      for(;;)
      {
        if( (l=Fread( hand, sizeof(archdr), &archdr )) != sizeof(archdr) )
            break;
        if( !strncmp( archdr.name, name, 12 ) )
        {
          l = archdr.text+archdr.data+2+(archdr.fixup<archdr.bss?archdr.bss:
              archdr.fixup) + TBUFSIZ+3;
          if( (l&1) ) l++;
          if( mode==1 )
          {
            len = (long)Malloc(-1L)-l;
            if( len<0 ) goto nomem;
            else if( len>32 )
            {
              if( (extra = Malloc(len-32)) == 0 ) goto nomem;
            }
            else extra = 0L;
          }
          start = prog=malloc_small(l);		/* 004: small */
          if( mode==1 && extra ) Mfree(extra);
          if( start == 0L )
          {
nomem:      l = AENSMEM;
            break;
          }
          start = (char *)((long)start+1&0xFFFFFFFEL);	/* probably not needed */
          text_buf = (unsigned char *)(start+l-TBUFSIZ-3);
          if( (l=decomp( archdr.total, hand, start )) == 0L )
          {
            Mshrink( 0, prog, text_buf-prog );	/* throw away text_buf */
            if( (l=*(long *)(len=archdr.text+archdr.data+(long)start)) != 0 )
            {
              *((char *)len+archdr.fixup) = '\0';
              *(long *)(l+=(long)start) += (long)start;
              fixup( (unsigned char *)(len+4), (long *)l, (long)start );
/*	      uptr = (unsigned char *)(len+4);
              while( *uptr )
              {
                if( *uptr == 1 ) l+=254;
                else *(long *)(l+=*uptr) += (long)start;
                uptr++;
              } */
            }
            memclr( (char *)((long)start+archdr.text+archdr.data), archdr.bss );
            execlose( open );
            if( !mode ) kill_about();
            l = (*(long (*)(void *a0, int d0, long stk ))start)
                ( a0, d0, _StkLim );
            if( mode==1 || mode==2 )
            {
              *strt = (char *)start;
              cond_mouse( ARROW );
              return(l);
            }
          }
          else close_exe();
          Mfree(prog);
          if( !mode ) kill_about();
          cond_mouse( ARROW );
          return(l);
        }
        if( !archdr.next || (l=Fseek( archdr.next, hand, 0 )) !=
            archdr.next ) break;
      }
    execlose( open && l>=0 );
    if( !mode ) kill_about();
    cond_mouse( ARROW );
    return( l<0 ? l : AEPLFMT );
  }
  else if( !mode ) kill_about();
  return(hand);
}
/********************************************************************/
GRAPHICS *init_graphics(void)
{
  long l;

  l = exe_load( 2, "ND_VDI.EXE", 0L, 0, &gr_start, 1 );
  if( l>0 ) return (GRAPHICS *)l;
  return 0L;
}
/********************************************************************/
char *pathend( char *ptr )
{
  register char *ch;

  if( (ch=strrchr(ptr,'\\')) == 0 ) return ptr;
  return ch+1;
}
/********************************************************************/
int cdecl get_line( int file, char *diskbuff, int ignore, int new )
{
  static char retried;
  static char *ptr;
  static int len, hex;
  int quit=0, strt=1, hex_flg=0, ign_eol=0, com=0;
  int from_hex( char *ptr );

  if(new)retried=len=fpos=0;
  while(!quit)
  {
    if( !len )
    {
      if( retried ) return(1);
      retried++;
      if( !TOS_error( (len = Fread(file,(long)LINE_BUF_LEN,ptr=line_buf)),
          0, 0L ) ) return(1);
      if( len ) retried = 0;
    }
    if( (!len && !strt) || *ptr=='\r' )
    {
      if( !com && !ign_eol )
      {
        *diskbuff = '\0';
        quit++;
      }
      strt++;
      com=ign_eol=0;
    }
    else if( strt && *ptr!='\n' )
    {
      com = *ptr==';';
      strt=0;
    }
    if( !com && !strt )
      if( hex_flg )
      {
        hex = hex*16 + from_hex(ptr);
        if( hex_flg == 2 )
        {
          *diskbuff++ = hex;
          hex_flg = 0;
        }
        else hex_flg++;
      }
      else if( *ptr=='^' && !ignore && !ign_eol ) ign_eol++;
      else if( *ptr=='\\' && !ignore )
      {
        hex=0;
        hex_flg++;
      }
      else *diskbuff++ = *ptr;
    if( len )
    {
      ptr++;
      len--;
      fpos++;
    }
  }
  return(0);
}
/**********/
int from_hex( char *ptr )
{
  if( *ptr > 0x60 ) *ptr -= 0x20;             /* convert a-f to A-F */
  return( *ptr - (*ptr >= 'A' ? 0x37 : '0') );   /* convert hex digit to dec */
}
/********************************************************************/
int f_alert1( char *msg )
{
  char bv=0;
  int ret;
  
  if( graphics && (bv=graphics->have_butv) != 0 ) (*graphics->set_butv)(0);
  ret = form_alert( 1, msg );
  if( bv ) (*graphics->set_butv)(1);
  return ret;
}
/**********************************************************************/
int cdecl read_messages( char *name, int end_num, char *start_mem,
                         char *end_mem, char *ptrs[], char **new_msgs )
{
  register int hand, i, l;
  int err=0, p_ind=0;
  register long flen, mlen;
  register char *ptr;

  if( (hand = Fopen(name,0)) > 0 )
  {
    ptr = start_mem;
    mlen = (long)end_mem - (long)start_mem;
    flen = Fseek( 0L, hand, 2 );
    Fseek( 0L, hand, 0 );
    for( i=0; i<end_num && !err; i++ )
      if( (err = get_line( hand, diskbuf, 0, !i )) == 0 )
      {
        if( (l=strlen(diskbuf)+1) > mlen )
          if( (ptr = *new_msgs = (char *)lalloc( mlen = flen -
              fpos + l, -3, 0 )) == 0 )
          {
            form_error(8);
            return(1);
          }
        strcpy( ptrs[p_ind++] = ptr, diskbuf );
        ptr = (char *)((long)ptr+l);
        mlen -= l;
      }
    Fclose(hand);
  }
  if( err )
  {
    spf( diskbuf, "[1][|Fatal error in %s!!][Ok]", name );  /* not a msg */
    f_alert1( diskbuf );
  }
  return(err);
}
/********************************************************************/
int redirect( int *f_h, int *s_h )
{
  register int i, j, err=0;
  register char *min, *max, *ptr;
  char *ptr2, *ptr3;
  ERRSTRUC errstruc[1];

  errstruc[0].num = IEACCDN;
  errstruc[0].str = msg_ptr[MAS0+1];
  min = index( z.long_tail, '\'' );
  max = rindex( z.long_tail, '\'' );
  ptr = !max ? z.long_tail+1 : max+1;
  j = 0;
  *s_h = 1;
  if( (ptr2=index( ptr, '>' )) != NULL )
  {
    j++;
    if( *(ptr2+1) == '>' ) j++;
  }
  else if( (ptr2=index( ptr, '<' )) != NULL )
  {
    j=3;
    *s_h = 0;
  }
  else ptr2 = index( ptr, '\0' );
  if( j )
  {
    ptr = ptr2 + (j==2);
    while( *++ptr == ' ' );
    ptr3 = ptr;
    do
    {
      if( *ptr3 >= 'a' && *ptr3 <= 'z' ) *ptr3 -= 32;
      else if( *ptr3 == ' ' ) *ptr3 = '\0';
    }
    while( *ptr3++ );
    if( j==1 ) Fdelete( ptr );
    if( !strcmp( ptr, "PRN:" ) ) *f_h = Fdup(3);
    else if( (*f_h = Fopen( ptr, *s_h )) < 0 )    /* s_h is the mode here */
      if( !*s_h || (*f_h = Fcreate( ptr, 0 )) < 0 ) err =
          !TOS_error( (long) *f_h, 1, errstruc );
  }
  if( !err )
  {
    if( j==2 ) Fseek( 0L, *f_h, 2 );
    for( i=0, ptr=z.long_tail; *ptr; ptr++ )
      if( (ptr!=min && ptr!=max || ptr==min && min==max) && ptr<ptr2 )
          z.long_tail[i++] = *ptr;
    z.long_tail[i] = '\0';
  }
  return(err);
}
/********************************************************************/
int TOS_error( long num, int errs, ERRSTRUC errstruc[] )
{
  register int i;
  char temp[150];

  if( num < 0L )
  {
    temp[0] = 0;
    for( i=0; i<errs; i++ )
      if( num == (long)errstruc[i].num ) spf( temp, msg_ptr[MAS0+2],
          errstruc[i].str, num );
    if( !temp[0] && num >= -12L ) num = IERROR;
    if( !temp[0] )
      for( i=0; i<DFLT_ERRORS; i++ )
        if( num == (long)dflt_errors[i].num ) spf( temp, msg_ptr[MAS0+2],
            dflt_errors[i].str + (dflt_errors[i].str[0]=='|'), num );
    if( !temp[0] ) spf( temp, msg_ptr[MAS0+3], num );
    f_alert1( temp );
    return(0);
  }
  return(1);
}
/********************************************************************/
int Blitmode( int mode )
{
  if( is_falc ) return 0;	/* avoid bug in Falcon TOS */
  return xbios( 64, mode );
}
char cattnum[] = { _CaTT_s16_mhz, _CaTT_s32_mhz, _CaTT_s48_mhz, _CaTT_s56_mhz, _CaTT_s64_mhz },
  cattmodes[] = { 16, 32, 48, 56, 64 };
int cdecl set_caches( int bits )
{
  int out=0, i, out2=0;
  extern int cache;
  long set_MSTe(void), set_030(void), set_adspd(void), set_CaTT(void);
  extern char has_adspd;

  cache = ((unsigned int)bits>>8)|(bits<<8);
  if( ((i=Blitmode(-1))&2) != 0 )
  {
    out |= i&1;
    if( cache>=0 ) Blitmode( (cache&1) ? (i|1) : (i&0xfffe) );
  }
  else out|=1;
  if( is_MSTe ) out |= Supexec( set_MSTe );
  else out |= 3<<2;
  if( is_030 )
  {
    if( has_catt )
      if( cache>=0 )
      {
        i = (cache>>12)&7;
        if( i && cattmodes[i-1] ) out2 |= Supexec( set_CaTT );
      }
      else out2 |= Supexec( set_CaTT );
    out |= Supexec( set_030 );
  }
  else if( has_adspd ) out |= Supexec( set_adspd );
  else out |= 1<<1;
  if( cache<0 && !z.multitask && (z.has_magx || gnva_ge004) ) out2 |= (1<<3);
  return (out<<8) | out2;
}
void init_catt(void)
{
  int i, j;
  CaTT_GIOS cg;

  if( has_catt ) CaTT_inquire( &cg, sizeof(cg) );
  else cg.max_function_code = 0;
  for( i=0; i<sizeof(cattnum); i++ )
    if( cattnum[i] > cg.max_function_code ) cattmodes[i] = 0;
    else for( j=0; ; j++ )
      if( j==8 || !cg.catt_modes_table[j] )
      {
        cattmodes[i] = 0;
        break;
      }
      else if( cg.catt_modes_table[j]==cattmodes[i] ) break;
}
/********************************************************************/
int cdecl wait_key(void)
{
  int ret=0, dum;

  (*graphics->set_butv)(1);
  while(!ret)
  {
    switch( (*graphics->get_mbut)()&3 )
    {
      case 1:
        ret = 0x39;     /* Space bar */
        break;
      case 2:
        ret = 0x48;     /* Up arrow */
        break;
      case 3:
        ret = 1;        /* Esc key */
    }
    if( Bconstat(2) ) ret = (Bconin(2)&0xFF0000)>>16;
  }
  (*graphics->set_butv)(0);
  return(ret);
}
void get_shel_path( char *filename )
{
  char *p;
  struct
  {
    int dummy;
    long magic;
    char tail[128];
  } sheltail;
  
  shel_read( filename, (char *)&sheltail );
  is_magx_shel = sheltail.magic==0x5348454CL;
  strcpy( z.dflt_path, filename );
  if( (p = strrchr(z.dflt_path,'\\')) != 0 ) *(p+1) = 0;
  else z.dflt_path[0] = 0;
  if( *(p=z.dflt_path) != 0 )
  {
    if( *(p+1)==':' )
    {
      Dsetdrv( (z.dflt_path[0]&=0x5f)-'A' );
      p += 2;
    }
    Dsetpath(p);
  }
}
/********************************************************************/
void add_ver( int num )		/* 002 */
{
  char *ptr=msg_ptr[MAS0-1];
  
  if( (ptr = strchr( ptr, '?' )) != 0 ) *ptr = num+'0';
}
/********************************************************************/
int is_alt(void)
{
  return (Kbshift(-1)&0xf)==8;
}
/********************************************************************/
MASTER mas = { NEO_VER, 0, { 0 }, &z, &dflt_errors[0], MINIT, &blank[0],
               exe_load, &redirect, &execute,
               &clear_mouse, &wait_key, &dsetpath, (void cdecl (*)())0L,
               &get_line, &read_messages, 0L, 0L, &set_caches, &rs_trindex[0],
               lalloc, lfree, lfreeall, memstat, lshrink, lrealloc, 0L, close_exe };

#pragma warn -par
int main( int argc, char *argv[], char *envp[] )
{
  int i, err, s_hand, f_handle=-1, old_hand, status=0, rez, dum,
      AES_handle, msgbuf[8], old_av;
  long bad_exec = -1L, l;
  char filename[150], no_adspd=0, *env,
      diskbuff[300];                  /* 150 because of form_alert */
  MASTER *old_mas;
  static char lets[]="LMH";
  static int tbl[3][2] = { 320, 200, 640, 200, 640, 400 };
  long is_ads(void), get_bp_addr(void);

  z.is_acc = !_app;
  if( is_alt() )		/* 005: function */
    if( z.is_acc )
    {
      appl_init();
      goto quit;
    }
    else return(-1);
  mas.parent_env = _BasPag->p_env;
  if( !z.is_acc )
  {
    /* remove any ARGV passed to this app */
    for( i=0; envp[i]; i++ )
      if( !strcmp( envp[i], "ARGV=" ) )
      {
        *envp[i] = 0;
        break;
      }
  }
  AES_handle = appl_init();
  aes_ge_40 = _GemParBlk.global[0]>=0x400;
  if( aes_ge_40 || z.is_acc ) z.menu_id = menu_register( AES_handle, "  NeoDesk" );
  /* must happen before anything that might goto quit or return so that
     MagX gets the right return value */
  if( _GemParBlk.global[0] >= 0x339 ) get_shel_path( filename );
  /* read_messages uses spf (lc->dopf) if error */
  if( CJar( 0, LOAD_COOKIE, (long *)&lc ) != CJar_OK || lc->ver != LOADER_VER )
  {
    add_ver( LOADER_VER>>8 );
    add_ver( LOADER_VER&0xFF );
    f_alert1( msg_ptr[MAS0-1] );
    goto quit;
  }
  if( lc->mas && lc->mas->singletask )		/* 006 */
      magx_unsingle( &lc->mas->singletask, _BasPag );
  err = (int)Supexec(get_ver) > 0x102 ? MEM_NONE : MEM_ALWAYS;
  if( CJar( 0, GENEVA_COOKIE, &l ) == CJar_OK )
  {
    has_gnva = 1;
    if( ((G_COOKIE *)l)->ver >= 0x104 ) gnva_ge004 = 1;
  }
  for( i=0; i<11; i++ )
  {
    z.mem_limit[i].mem_mode = err;
    z.mem_limit[i].limit_to = z.mem_limit[i].take_always = has_gnva ?
        100*1024L : 250*1024L;
  }
  if( CJar( 0, MagX_COOKIE, &l ) == CJar_OK )
  {
    z.has_magx = z.multitask = 1;
  }
  z.dflt_path[0] = (i=Dgetdrv()) + 'A';
  z.dflt_path[1] = ':';
  if( z.is_acc )
    if( shel_envrn( &env, "NEOPATH=" ) && env && *env )
    {
      if( env[1]==':' ) strcpy( z.dflt_path, env );
      else strcpy( z.dflt_path+2, env );
      dsetpath( z.dflt_path );
    }
    else if( !Dsetpath( env="\\NEODESK4\\" ) ) strcpy( z.dflt_path+2, env );
  Dgetpath( z.dflt_path+2, i+1 );
  strcat( z.dflt_path, "\\" );
  z.rezvdi = -1;
  strcpy( z.drezname, "DEFAULT" );
  if( (i = Fopen("NEO_INF.DAT",0)) < 0 )
  {
    for( i=0; i<3; i++ )
    {
      spf( z.rezname[i], "NEODESK%c", lets[i] );
      z.rezes[i][0] = tbl[i][0];
      z.rezes[i][1] = tbl[i][1];
    }
  }
  else
  {
    Fread( i, 1L, filename );
    if( filename[0]>=INFDAT_VER-2 && filename[0]<=INFDAT_VER )
    {
      Fread( i, 40L, z.rezes[0] );
      Fread( i, 90L, z.rezname[0] );
      Fread( i, (long)(MAX_NEO_ACC*9+1), z.neo_acc );
      if( filename[0]>=INFDAT_VER-1 )
      {
        Fread( i, 2L, &z.rezvdi );
        Fread( i, 9L, z.drezname );
        if( filename[0]==INFDAT_VER ) Fread( i, sizeof(z.mem_limit),
            z.mem_limit );
      }
    }
    else f_alert1( msg_ptr[MAS0+10] );
    Fclose(i);
  }
  wind_get( 0, WF_FULLXYWH, &z.maximum.x, &z.maximum.y, &z.maximum.w,
      &z.maximum.h );
  z.inf_name = z.drezname;
  for( i=0; i<10; i++ )
    if( z.rezes[i][0] == z.maximum.w && z.rezes[i][1] == z.maximum.y+z.maximum.h )
    {
      z.inf_name = z.rezname[i];
      break;
    }
  /* i is either correct rez or 10 for "Default" */
  z.rez_num = i;
  z.mem_mode = z.new_mem_mode = z.mem_limit[i].mem_mode;
  z.limit_to = z.mem_limit[i].limit_to;
  z.take_always = z.mem_limit[i].take_always;

  if( read_messages( "NEODESK1.MSG", EXE1_MSGS, msg_ptr[0], msg_ptr[EXE1_MSGS],
      &msg_ptr[0], &new_msgs ) ) goto quit;
  graf_handle( &char_w, &char_h, &dum, &blank[1].ob_height );
  init_about();
  if( CJar( 0, _MCH_COOKIE, &l ) == CJar_OK )
  {
    is_MSTe = l==0x00010010L;
    is_falc = (int)(l>>16L)==3;
    if( (l>>16)>2 ) no_adspd=1;
  }
  if( CJar( 0, _CPU_COOKIE, &l ) == CJar_OK )
  {
    is_030 = (int)l >= 30;
    no_adspd = 1;
  }
  has_catt = !Init_CaTT_GIOS();
  init_catt();
#ifdef MMAC
  no_adspd = 1;
  is_030 = 1;
  is_MSTe = 0;
#else
  if( !no_adspd ) Supexec( is_ads );
#endif
  if( CJar( 0, MiNT_COOKIE, &l ) == CJar_OK ) has_mint = 1;
  Supexec(get_bp_addr);		/* relies on has_mint */
  if( is_MSTe && is_030 ) is_MSTe = 0;
  if( !has_mint && !has_gnva &&
      (z.has_multigem=gemdos(112,1,1,0x5aaf)==0L) != 0 )
  {
    gemdos(112,-1,-1,0x5aaf);
    z.multitask=1;
  }
  if( _GemParBlk.global[1]==-1 ) z.multitask=1;
  if( _GemParBlk.global[0] >= 0x340 )
  {
    i = _GemParBlk.global[0] >= 0x410 && appl_getinfo( 10, &sw_bits, &sw0, msgbuf, &sw_argv );
    if( has_gnva || !i && has_mint && z.multitask )	/* avoid lack of getinfo in AES 4.00 */
    {
      sw0 = 0;
      sw_argv = 1;
      sw_bits = -1;
    }
    else sw_bits &= (1<<11);		/* only bit I care about */
  }
  for( i=0; i<DFLT_ERRORS; i++ )
    dflt_errors[i].str = msg_ptr[i];
  errstruc[0].str = msg_ptr[MAS0+4];
  errstruc[1].str = msg_ptr[MAS0+5];
  init_always();		/* 004: grab always block before exe loads */
  if( (z.graphics = graphics = init_graphics()) /*%z.rezvdi&(1<<z.rez_num)))*/ == 0 )
  {
    kill_about();
    goto quit;
  }
  for( i=0; i<3; i++ )
    strcpy( z.new_inf_name[i], z.inf_name );
  blank[0].ob_height = graphics->v_y_max;
  blank[1].ob_height--;
  mas.rez = /*%rez =*/ char_h==16 && (CJar( 0, _VDO_COOKIE, &l ) != CJar_OK ||
      (l>>16)!=2 || Getrez()!=7) ? 2 : (graphics->v_x_max>=640 ? 1 : 0);
/*%  z.rez_let = lets[rez]; */
  mas.dopf = lc->dopf;
  mas.bad_media = lc->bad_media;
  mas.open_wind = lc->open_wind;
  wind_calc( 1, WIND_TYPE, z.maximum.x, z.maximum.y, z.maximum.w,
      z.maximum.h, &z.max_area.x, &z.max_area.y, &z.max_area.w,
      &z.max_area.h );
  z.cliparray[0] = z.cliparray[2] = z.maximum.x;
  z.cliparray[1] = z.cliparray[3] = z.maximum.y;
  z.cliparray[2] += (blank[0].ob_width = blank[1].ob_width = z.maximum.w) - 1;
  z.cliparray[3] += z.maximum.h - 1;
  z.pallette[0] = 0x777;
  z.pallette[1] = 0x700;
  z.pallette[2] = 0x035;

  old_mas = lc->mas;

try_again:                                   /* ok, so I used a goto... */

  prep_exec();
  lc->mas = &mas;
  old_av = *lc->avserver;	/* 003 */
  if( (bad_exec = dsetpath(z.dflt_path)) == 0 )
  {
/* 002    appl_exit(); */
    bad_exec = exe_load(0,"NEODESK2.EXE",0L,0,0L,1);
    AES_handle = appl_init();
/* 002   if( aes_ge_40 || && !z.is_acc ) menu_register( AES_handle, "  NeoDesk" ); */
  }
  else
  {
    kill_about();
    if( test_err(bad_exec) ) goto try_again;
  }
  *lc->avserver = old_av;	/* 003 */
  if( bad_exec == MMAGIC )
  {
    err = 0;
    wind_set( 0, WF_NEWDESK, &blank[0], 0, 0 );
    if( z.set_path )
    {
      strcpy( filename, mas.path );
      *pathend(filename) = '\0';
      Dsetdrv( filename[0]-'A' );
    }
    else
    {
      filename[0] = z.old_drv+'A';
      filename[1] = ':';
      strcpy( filename+2, z.old_path );
      strcat( filename, "\\" );
      Dsetdrv(z.old_drv);
    }
    if( TOS_error( dsetpath( filename ), 2, errstruc ) )
    {
      if( z.exec_type.p.tos ) err = redirect( &f_handle, &s_hand );
      else if( z.exec_type.p.clear_screen )
      {
        graf_mouse( HOURGLASS, 0L );
        objc_draw( &blank[0], 0, 1, blank[0].ob_x, blank[0].ob_y,
            blank[0].ob_width, blank[0].ob_height );
      }
      if( !err )
      {
        if( z.exec_type.p.tos && z.exec_type.p.clear_screen )
        {
          (*graphics->hide_mouse)();
          bconws( "\033E\033e\033v\033b\057\033c\040\r" );
        }
        appl_exit();
        if( f_handle>0 )
        {
          old_hand = Fdup( s_hand );
          if( (i = Fforce( s_hand, f_handle )) < 0 )
          {
            appl_init();
            err = !TOS_error( (long) i, 2, errstruc );
            Fclose( f_handle );
            Fclose( old_hand );
            f_handle = -1;
            appl_exit();
          }
        }
        if( !err ) status = execute( s_hand, f_handle, old_hand );
        appl_init();
single_ret:
        (*graphics->show_mouse)();
        if( status && z.exec_type.p.show_status || z.exec_type.p.tos &&
            z.tos_pause )
        {
          bconws( msg_ptr[MAS0+6] );
          wait_key();
        }
      }
    }
    wind_set( 0, WF_NEWDESK, &blank[0], 0, 0 );
    bconws( "\033H\033K\033B\033K" );
    strcpy( ted.te_ptext, msg_ptr[MAS0+7] );
    objc_draw( &blank[0], 0, 1, blank[0].ob_x, blank[0].ob_y,
        blank[0].ob_width, blank[0].ob_height );
    goto try_again;
  }
quit:
  if( graphics ) (*graphics->graph_exit)();
  if( gr_start ) Mfree(gr_start);
  if( new_msgs ) lfree(new_msgs);
  lfreeall( ALLOC_MAS );
  if( lc ) lc->mas = old_mas;
  if( z.is_acc )
    for(;;)
    {
      evnt_mesag(msgbuf);
      if( msgbuf[0] == AP_TERM ) break;
    }
  appl_exit();
  return( is_magx_shel && is_alt()/*005*/ ? -1 : 0 );
}
#pragma warn +par
/*********************************************************************/
/***void cdecl magic_exec( BASPAG *bp )
{
  int r;

  appl_init();
  mas.singletask = bp->p_parent;
  breakpoint();
  bp->p_parent = _BasPag->p_parent;
  r = shel_write( 1, !z.exec_type.p.tos, 101, mas.path, z.tail );
  appl_exit();
  Ptermres( -1L, r );
}
int magic_single(void)
{
  BASPAG *bp;
  int r;

  bp=(BASPAG *)Pexec( 5, 0L, (COMMAND *)"\0", 0L );
  if( (long)bp > 0 )
  {
    Mshrink( 0, bp, sizeof(BASPAG) );
    bp->p_tbase = magic_exec;
    r = Pexec( 6, 0L, (COMMAND *)bp, 0L );
    Mfree(bp);
    return r+1;
  }
  return 1;
} ****/
/*********************************************************************/
int execute( int s_h, int f_h, int o_h )
{
  int i, j, l, status;
  char temp[120], *env;
  APP_FLAGS af;
  APPFLAGS a;
  SHWRCMD sh;

  for( i=16; --i>=0; )
    z.pallette[i] = Setcolor( i, -1 );
  af.i = z.new_cache;
  if( !af.p.reserved )
  {
    i = set_caches( af.i );
    if( !af.p.clock ) *(lc->clock_temp) = 0;
  }
  if( has_mint ) z.exec_type.p.pexec_mode |= 0x100;
  env = z.env_ptr[0] ? z.env_ptr : 0L;
  status = 0;	/* 004 moved here for MagiX */
  if( z.multitask && sw_bits && !sw0 )
  {
    sh.name = mas.path;
    sh.environ = env;
    z.long_tail[0] = z.use_argv>0 ? -1 : strlen(z.long_tail+1);
    if( gnva_ge004 && !af.p.reserved && af.p.singletask )
    {
      strcpy( a.name, pathend(mas.path) );
      x_appl_flags( X_APF_SEARCH, 0, &a );
      a.flags.s.multitask = 0;
      sh.app_flags = a.flags.l;
      j = XSHD_FLAGS|XSHW_RUNANY|SHD_ENVIRON;
    }
    else j = SHW_RUNANY|SHD_ENVIRON;
    if( shel_write( j, !z.exec_type.p.tos, sw_argv && z.use_argv>0,
        (char *)&sh, z.long_tail ) < 0 ) f_alert1( msg_ptr[26] );
  }
  else if( z.has_magx )
  {
    if( !shel_write( 1, !z.exec_type.p.tos, j = 
        !af.p.reserved && af.p.singletask && is_magx_shel ? 101 : 100, mas.path, z.tail ) )
        f_alert1( msg_ptr[26] );
    else if( j==101 )
    {
      (*graphics->graph_exit)();
      magx_single( &mas.singletask );
      if( !(*graphics->reinit)() ) exit(-1);
    }
  }
  else status = Pexec( z.exec_type.p.pexec_mode, mas.path, (COMMAND *)z.tail, env );
  if( z.free_env )
  {
    lfree( z.free_env );
    z.free_env = 0L;
  }
  if( !af.p.reserved )
  {
    set_caches(i);
    *(lc->clock_temp) = 1;
    z.new_cache = -1;
  }
  for( i=16; --i>=0; )
    Setcolor( i, z.pallette[i] );
  if( f_h>0 )
  {
    Fforce( s_h, o_h );
    Fclose( o_h );
    Fclose( f_h );
  }
  if( z.exec_type.p.return_status && z.stat_return )
  {
    *z.stat_return = (unsigned)status>>8;
    *(z.stat_return+1) = status;
  }
  if( z.exec_type.p.clear_screen ) bconws( "\033f\033b\x2f\033c\x20" );
  if( status && (!z.status_report || status<0 && z.status_report==1) &&
      z.exec_type.p.show_status )
  {
    spf( temp, msg_ptr[MAS0], status );
    bconws( temp );
    if( status<0 )
    {
      for( i=j=0; i<DFLT_ERRORS; i++ )
        if( status == dflt_errors[i].num ) j = i;
      if(j)
      {
        bconws( "\r\n(" );
        i=0;
        while( (l=dflt_errors[j].str[i]) != 0 )
        {
          if( i || l!='|' ) Crawio( l=='|' ? ' ' : l );
          i++;
        }
        Crawio( ')' );
      }
    }
  }
  else status=0;
  return(status);
}

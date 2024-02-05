/* NeoDesk 3.02 by Dan Wilga
   Copyright � 1990, Gribnif Software.
   All Rights Reserved.
*/
#include "tos.h"
#include "stdio.h"
#include "ierrno.h"
#include "aes.h"
#include "neodesk.h"
struct linea_init { int dum; };	/* included for neocommn.h */
#include "neocommn.h"
#include "neold_id.h"
#include "string.h"

/* SET STACK SIZE TO 1024!!! */

#define VER_STR	     "\r\nNeoDesk � Loader v. 4.06 resident\r\n"
#define hdv_mediach  *((long **)0x47E)
#define hdv_rw       *((long **)0x476)
#define hdv_bpb      *((long **)0x472)
#define prv_lsto     *((long **)0x55e)
#define prv_lst      *((long **)0x57e)
#define _p_cookies   (*(long **)0x5a0)
#define TIMER_VEC    *((long *) 0x114L)
#define _sysbase     (*(long *)0x4F2L)
#define OS_version   (*(int *)((*(long *)0x4F2)+2))
#define MiNT_COOKIE 0x4D694E54L
#define _VDO_COOKIE 0x5F56444FL
#define MAGC_COOKIE  0x4d674d63L
#define CYREL_COOKIE  0x434D3136L

char errmsg[] =
    "\033f\033b\057\033c\040\033H\aA%s error has occurred (%d bombs).\033K\r\n\
Press \"R\" for a list of the system\'s Registers.\033K\r\n\
Press \"P\" to list to Printer.\033K\r\n\
Any other key exits.\033K",
     *errcodes[11] = {
    " Bus", "n Address", "n Illegal Instruction", " Divide by Zero",
    " CHK Instruction", " TRAPV Instruction", " Priviledge Violation",
    " Trace", " Reserved Vector", "n Uninitialized Interrupt Vector",
    "n <Unknown>" },
     regstr[]="\033H\r\n\033J\r\n",
     regfmt[]="\
    d%c: %08X   a%c: %08X\r\n",
     other[] ="\r\n\
   ssp: %08X   %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\r\n\
   usp: %08X   %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\r\n\
    pc: %08X\r\n\
instr.: %04x %04x %04x %04x\r\n\
    sr: %04x = {t:%c  s:%c  interrupt:%c%c%c  x:%c  n:%c  z:%c  v:%c  c:%c}",
    not_68k,
    space[300]/* must be big enough for other[] */, path[120];
     
TEDINFO ted = { path, "", "", IBM, 0, TE_CNTR, 0x1180,
  /* (BLACK<<12)|(BLACK<<8)|(1<<7)|(7<<4), */  0, -1, 12, 1 };
OBJECT blank[] = { { -1, 1, 1, G_BOX, 0, 0, (long)((4<<4)|3) },
                   { 0, -1, -1, G_BOXTEXT, 32, 0, (long)&ted } };

long pr_count, pr_bufsiz=8191L;
int bad_media=-1, outhand, savcnt=0, mdwa, avserver=-1, w_colors[19][2] =
    { 0, 0, 0, 0, 0x1100, 0x1100, 0x1100, 0x11A1, 0x1100, 0x1100,
      0x1180, 0x1180, 0, 0, 0, 0, 0x1100, 0x1100, 0x1100, 0x1100,
      0x1100, 0x1100, 0x1100, 0x1100, 0x1111, 0x1111, 0x1100, 0x1100,
      0x1100, 0x1100, 0x1100, 0x1100, 0x1100, 0x1100, 0x1111, 0x1111,
      0x1100, 0x1100 };
char top, **bufptr, scrsav=0, do_malloc=0, *dm_ptr, clock_cnt,
    ldpath[80], clock_temp=1, lock_drive[4]={-1,-1,-1,-1}, *pr_bufmax,
    *pr_bufstart, *pr_buftail, no_bcon, num_sep, has_magcmac, long_numbers,
    has_cyrel;

extern long old_mediach, new_mediach, old_rw, new_rw, timvec, do_prt, kbdv,
    key_orig, old_bpb, new_bpb, old_prv, new_prv, old_pri, new_pri;
extern int global[];
extern char prgflg;
void clock_on( void vclock(void), long *vbladr );
void clock_off(void);
void (*vclock)(void);
extern void shrink( BASPAG *bp, long stack );
extern char *kbshift, inittab[], t2tabl[];
long *new_jar, open_wind, old_hitpa, baspag, *pgmsize, pall[MAX_SAVECOL>>1];
void (*saver)( int onoff );
int (*fselect)( int apid, char *path, char *file, int *button, char *title );
BASPAG new_bp;

LoadCookie new_cookie = { LOADER_VER, &dopf, &bconws, &bad_media, &top,
    &outhand, &bufptr, ((int *)&pr_count)+1, &savcnt, &scrsav,
    '\0', '\1', '\0', '\5', &kbshift, &pall, &clock_cnt,
    &open_wind, t2tabl, (MASTER *)0L, &do_malloc, &baspag, &pgmsize,
    inittab, &saver, shrink, &fselect, &vclock, clock_on, clock_off,
    &mdwa, &ldpath[0], &clock_temp, &w_colors, &lock_drive, &pr_bufsiz,
    &pr_count, &pr_bufmax, &pr_bufstart, &pr_buftail, &no_bcon, PR_VALID,
    &num_sep, &avserver, &long_numbers, &has_cyrel };
LoadCookie *lc=&new_cookie;
char *tabptr=inittab, *t2tab=t2tabl, falc_vid;

void quit( int stat )
{
  Pterm(stat);
}

/********************************************************************/
void cdecl spf(char *buf, char *fmt, ...) {
  dopf(buf, fmt, (unsigned int *)&...);
}
void cdecl dopf(char *buf, char *fmt, ...) {
  char **pp, *ps, pad, lj, sign, larg;
  unsigned long n, *lp;
  register int c, w;
  unsigned int *ap = *(unsigned int **)...;
  
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
        case 'b': *buf++ = *ap++ != 0 ? 'T' : 'F';
            break;
        case 'k':
            spf( buf, "%02x %02x %02x", (*(KEYCODE **)ap)->shift,
                (*(KEYCODE **)ap)->scan, (*(KEYCODE **)ap)->ascii );
            ((KEYCODE **)ap)++;
            buf += strlen(buf);
            break;
        case 'v':
            spf( buf, "%x.%02x", *ap>>8, *ap&0xff );
            ap++;
            buf += strlen(buf);
            break;
        case 'S': *buf++ = '`';
        case 's': pp = (char **) ap;
            ps = *pp++;
            w -= strlen(ps);
            if( !lj ) while (w-- > 0) *buf++ = pad;
            ap = (unsigned int *) pp;
            if( c!='S' ) while (*ps) *buf++ = *ps++;
            else while (*ps)
              if( *ps=='`' ) ps++;
              else *buf++ = *ps++;
            if( lj ) while (w-- > 0) *buf++ = pad;
            if( c=='S' ) *buf++ = '`';
            break;
        case 'x':
            sign=0;
            n = (unsigned long) *ap++;
            goto do_pn;
        case 'h':
            sign=0;
            n = (unsigned long)(unsigned char)*ap++;
            goto do_pn;
        case 'N': larg++;
        case 'n': if( (sign = num_sep) == 0 && !long_numbers ) sign = -1;
            goto dpn;
        case 'X': sign=0;
        case 'D': larg++;
        case 'd': case 'o':
dpn:        if (larg) {
            lp = (unsigned long *)ap;
            n = *lp++;
            ap = (unsigned int *)lp;
            }
            else n = (long)(signed) *ap++;
do_pn:      buf = pn(buf, n, c, w, sign, pad, lj);
            break;
        default:  *buf++ = c; 
            break;
      }
    }
    else  *buf++ = c;
  }
  *buf = 0;
}
/*****************
char *pn( char *b, unsigned long n, int base, int w, int sign, int pad, int lj )
{
  int i, j;
  char nb[20], sep;
  switch (base) {
  case 'o':
    base = 8;
    break;
  case 'x':
  case 'h':
  case 'X':
    base = 16;
    break;
  default:
    base = 10;
  }
  sep = sign==1 ? 0 : (char)sign;
  i = 0;
  if( !n ) nb[i++] = '0';
  else
  {
    if( sign && (n&0x80000000L) ){
      n = (unsigned long)(-(long)n);
    } else sign = 0;
    if( !sep ) for (; n; n /= base)
      nb[i++] = "0123456789ABCDEF"[n%base];
    else for (j=1; ; j++)
    {
      nb[i++] = "0123456789ABCDEF"[n%base];
      if( (n /= base) == 0 ) break;
      if( !(j%3) ) nb[i++] = sep;
    }
    if( sign ) nb[i++] = '-';
  }
  w -= i;
  if( !lj ) while (w-- > 0) *b++ = pad;
  while (i--) *b++ = nb[i];
  if( lj ) while (w-- > 0) *b++ = pad;
  return b;
}
**********/
char *pn( char *b, unsigned long n, int base, int w, char sign, int pad, int lj )
{
  int i, j, k;
  char nb[20], sep;
  static unsigned long divs[] =
      { 1L<<30,    1L<<20,     1L<<20,    1L<<20,   1L<<10,  1L<<10, 1L<<10 },
         ranges[] =
      { 1L,        100L,       10L,       1L,       100L,    10L,    1L },
         mults[] =
      { 1000L,     10L,        100L,      1000L,    10L,     100L,   1000L };
  static char lts[] =
      { 'G',         'M',        'M',       'M',      'K',     'K',    'K' },
              dps[] =
      {  2,           0,          1,         2,        0,       1,      2,    0 };

  switch (base) {
  case 'o':
    base = 8;
    break;
  case 'x':
  case 'h':
  case 'X':
    base = 16;
    break;
  default:
    base = 10;
  }
  sep = sign==1 ? 0 : sign;
  i = 0;
  if( !n ) nb[i++] = '0';
  else
  {
    if( sign && (n&0x80000000L) ){
      n = (unsigned long)(-(long)n);
    } else sign = 0;
    if( !sep ) for (; n; n /= base)
      nb[i++] = "0123456789ABCDEF"[n%base];
    else if( !long_numbers )
    {
      for( j=0; j<7; j++ )
        if( n/divs[j] >= ranges[j] )
        {
          if( j>=4 ) n = n*mults[j]/divs[j];
          else n /= divs[j]/mults[j];
          n = (n+5) / 10;
          break;
        }
      if( j<7 ) nb[i++] = lts[j];
      for ( j=dps[j]; n; n /= 10)
      {
        if( (k=n%10) != 0 || j<=0 || i>1 ) nb[i++] = "0123456789ABCDEF"[k];
        if( !--j && i>1 ) nb[i++] = '.';
      }
    }
    else for (j=1; ; j++)
    {
      nb[i++] = "0123456789ABCDEF"[n%10];
      if( (n /= 10) == 0 ) break;
      if( !(j%3) ) nb[i++] = sep;
    }
    if( sign ) nb[i++] = '-';
  }
  w -= i;
  if( !lj ) while (w-- > 0) *b++ = pad;
  while (i--) *b++ = nb[i];
  if( lj ) while (w-- > 0) *b++ = pad;
  return b;
}
/********************************************************************/
void cdecl bconws( char *ptr )
{
  while( *ptr )
    if( *ptr=='\033' && has_cyrel && (*(ptr+1)=='b' || *(ptr+1)=='c') ) ptr += 3;	/* 005 */
    else Crawio( *ptr++ );
}
/******************************************************************
int getcookie( long cookie, long *ptr )
{
    register long *cookiejar;

    if( (cookiejar = *(long **)0x5a0) == 0 ) return 0;
    do {
        if (*cookiejar == cookie)
        {
          if( ptr ) *ptr = *(cookiejar+1);
          return 1;
        }
        else cookiejar += 2;
    } while (*cookiejar);
    return 0;
}
********************************************************************/
void ptermrs(void)
{
/**  char temp[100];
  spf( temp, "Terminate at %X size=%X\r\n", _BasPag, _PgmSize );
  bconws(temp);
  Bconin(2); **/
  Ptermres( _PgmSize, 0 );
}
/********************************************************************/
void get_dflt(void)
{
  *(lc->ldpath) = Dgetdrv()+'A';
  *(lc->ldpath+1) = ':';
  Dgetpath( lc->ldpath+2, 0 );
  strcat( lc->ldpath+2, "\\" );
}
void get_shel_path(void)
{
  char *p, sheltail[150], filename[150];

  shel_read( filename, sheltail );
  if( (p = strrchr(filename,'\\')) != 0 ) *(p+1) = 0;
  else filename[0] = 0;
  if( filename[0] != 0 )
  {
    strcpy( p=lc->ldpath, filename );
    if( *(p+1)==':' )
    {
      Dsetdrv( (p[0]&=0x5f)-'A' );
      p += 2;
    }
    Dsetpath(p);
  }
  else get_dflt();
}
void get_path(void)
{
  if( global[0] >= 0x339 ) get_shel_path();
  else get_dflt();
/*%  Cconws( "\033H" );
  Cconws( lc->ldpath );
  Cconws( "path" ); */
}
/********************************************************************/
void run_master(void)
{
  static char name[] = "NEODESK.EXE";
  int dum, i, j, flag;
  char *p, temp[120];
  extern void no_exe(void), new_aes(void), test_magic(void);
  extern int second_run;
  
  if( second_run || global[0] )
  {
    if( global[0] < 0x399 )	/* 004: was 0x400 */
    {
      *dm_ptr = 1;
      for( i=flag=0; i<20 || flag; )
      {
        for( j=flag=0; j<6; j++ )
          flag += tabptr[j];
        flag += *(lc->mdwa);
        graf_mkstate( &dum, &dum, &dum, &dum );
        if( !flag ) i++;
      }
      *dm_ptr = 0;
    }
    else new_aes();
    test_magic();
    /* appl_init      evnt_mesag       evnt_timer       evnt_multi */
    *(t2tab+10-10) = *(t2tab+23-10) = *(t2tab+24-10) = *(t2tab+25-10) = -1;
    p=lc->ldpath;
    strcpy( temp, p );
    strcat( temp, name );
    if( p[1]==':' )
    {
      Dsetdrv(*p-'A');
      p += 2;
    }
    Dsetpath(p);
    if( Fsfirst( temp, 0x37 ) ) no_exe();
    else shel_write( 1, 1, 1, temp, "\0" );
  }
  else bconws( VER_STR );
  *dm_ptr = 0;
  if( old_hitpa != _PgmSize ) ptermrs();
}
/********************************************************************/
/**************void test( long l )
{
  char temp[50];
  
  num_sep = 0xff;
  spf( temp, "%D = %N\r\n", l, l );
  bconws(temp);
  test(22L);
  test(1000L);
  test(1023L);
  test(1024L);
  test(1030L);
  test(1023200L);
  test(1026200L);
  test(1048576L);
  test(1948576L);
  test(999999999L);
  test( 3L<<15L );
  test( 0x41000000L );
  Crawcin();
  Pterm0();
} **********/
int main(void)
{
  long stack;
  KBDVBASE *kbp;
  extern int shel_fix(void);
  extern void unjar(void), bad_message(void), bad_version(void);
  extern IOREC *iorec;
  extern char *kbshift;

  if( (Kbshift(-1)&0xf) == 8 )
  {
    Cconws( "\r\nNEOLOAD: Installation aborted due\r\n\
to Alternate key being held.\r\n\n" );
    return(0);
  }
  if( CJar( 0, CJar_cookie, 0L ) != CJar_OK )
  {
    Cconws( "\033E\aYou must run JARxxx before NEOLOAD\r\n" );
    Crawcin();
    return 1;
  }
  has_cyrel = CJar( 0, CYREL_COOKIE, 0L ) == CJar_OK;	/* 005 */
  if( CJar( 0, _VDO_COOKIE, &stack ) == CJar_OK )
      falc_vid = (int)(stack>>16L)==3;
  has_magcmac = CJar( 0, MAGC_COOKIE, &stack ) == CJar_OK;
  old_hitpa = _PgmSize;
  iorec = Iorec(1);
  stack = Super((void *)0L);
  kbshift = (char *)(OS_version>0x0100 ? *(long *)(_sysbase+0x24) : 0xE1BL);
  if( CJar( 0, LOAD_COOKIE, (long *)&lc ) != CJar_OK )
  {
    /* lc points to new_cookie */
    if( CJar( 1, LOAD_COOKIE, &lc ) != CJar_OK )
    {
      Cconws( "\r\n\aNo more room in Cookie Jar.\r\nUse a higher number in JARxxx.\r\n" );
      Crawcin();
      return 1;
    }
    Super((void *)stack);
    appl_init();
    get_path();
    stack = Super((void *)0L);
    if( shel_fix() )
    {
      *((long *)0x380L) = 0L;
      old_mediach = (long)hdv_mediach;
      hdv_mediach = &new_mediach;
      old_rw = (long)hdv_rw;
      hdv_rw = &new_rw;
      old_bpb = (long)hdv_bpb;
      hdv_bpb = &new_bpb;
      timvec = TIMER_VEC;
      TIMER_VEC = (long) &do_prt;
      old_pri = (long)prv_lsto;
      prv_lsto = &new_pri;
      old_prv = (long)prv_lst;
      prv_lst = &new_prv;
      kbp = Kbdvbase();
      key_orig = (long) kbp->kb_kbdsys;
      kbp->kb_kbdsys = (void (*)())&kbdv;
      Super((void *)stack);
      dm_ptr = &do_malloc;
      *lc->pgmsize = &_PgmSize;
      *lc->baspag = (long)_BasPag;
      run_master();
      ptermrs();
    }
    else
    {
      Super((void *)stack);
      bad_message();
      return(0);
    }
  }
  if( lc->ver != LOADER_VER )
  {
    Super((void *)stack);
    bad_message();
    return(0);
  }
  Super((void *)stack);
  appl_init();
  get_path();	/* needs appl_init() first */
  if( CJar( 0, MiNT_COOKIE, 0L ) != CJar_OK )
  {
    stack = Super((void *)0L);
    *lc->baspag = (long)_BasPag;
    if( old_hitpa == _PgmSize )
        (*lc->shrink)( _BasPag, stack );   /* never returns */
    Super((void *)stack);
  }
  t2tab = lc->t2table;
  tabptr = lc->inittab;
  dm_ptr = lc->do_malloc;
  *lc->pgmsize = &_PgmSize;
  run_master();
  return(0);
}

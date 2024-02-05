#include "string.h"
#include "stdlib.h"
#include "tos.h"
#include "stdio.h"
#include "ctype.h"
#include "new_aes.h"
#include "vdi.h"
#include "xwind.h"
#include "..\neocommn.h"
#include "..\guidefs.h"
#include "graphics.h"
#include "help.h"

#define LINES   100
#define LINELEN 100
#define JUMPS   20

#define MODE_ALL -1
#define MODE_TEXT 1
#define MODE_SEL  2

#define HELP_ID		0x476E0100	/* Gn 1.0 */

static int hand, nyb, wind, dum;
static long *tbl1, *tbl2;
static int char_w, char_h, bl_rows, bl_cols, apid, line_num, line_cnt, line_len,
    sel_row, sel_col, sele_row, sele_col, draw_mode=MODE_ALL, jumps[JUMPS], jnum,
    jstart, min_wid, min_ht, search_ret, corn_x, corn_y, count;
static char path[120], fname[13], info_txt[100], info_max, read_err,
    **title, search_top[30], know_cols, savepth[120], savename[13], name[40],
    geneva_fmt, in_rev, hlpfile[120], modal_open;
static unsigned char line[LINES][LINELEN+2], *line_pos;  /*, llen[LINES];*/

#ifdef DEBUG
extern int vdi_hand;
#else
int vdi_hand;
#endif

static int
    work_in[] = { 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
    work_out[57];       /* returns from v_opnvwk() */

static struct		/* for Pure C format files */
{
  char info[84];
  long id;
  long tbl1_len,
       tbl2_start,
       tbl2_len;
  char fast[12];
  long caps_start,
       caps_len,
       caps_entries,
       sens_start,
       sens_len,
       sens_entries;
} help;

typedef struct header	/* for Geneva-format files */
{
  long id;
  int hdsize;		/* sizeof(HEADER) */
  long tbl_len,		/* longword, alphabetical pointers */
       caps_start,	/* 6-byte table, relative to entry */
       caps_len,
       caps_entries,
       sens_start,
       sens_len,
       sens_entries;
} HEADER;

HEADER header;

int cdecl draw_text( PARMBLK *pb );
void new_screen( int draw );
void do_index(void);
int prev_jump( int num );
void jump_tbl( int i, char *s );
int open_help( char *s );
void get_corner(void);
void do_move( int *buf );
void do_full( int *buf );
static void use_menu( int num, int title );

static GRECT winner, wsize, max;
static USERBLK ub_main = { draw_text };
static OBJECT blank[] = { -1, -1, -1, G_USERDEF, TOUCHEXIT, 0, (long)&ub_main },
    *form, *menu, *modal;
static Rect clip_rect;

#define HELP_SET_VER 0x0100
#define HELP_VER     0x0101
#define WIN_TYPE NAME|MOVER|CLOSER|FULLER|SIZER|UPARROW|DNARROW|VSLIDE|LFARROW|RTARROW|HSLIDE|INFO

static HELP_SET set;
static HELPCMD cmd;
static RSHDR *rshdr;
static MOST *z;
static GUI *gui;

void help_main(void);
void new_topic( HELPCMD *h );
int i_help( OBJECT *o, FORM *f );
int t_help( OBJECT *o, int num, FORM *f );
int xx_help( OBJECT *o, int num, FORM *f );
int u_help( OBJECT *o, FORM *f );
int i_set( OBJECT *o, FORM *f );
int t_set( OBJECT *o, int num, FORM *f );
int x_set( OBJECT *o, int num, FORM *f );
int i_find( OBJECT *o, FORM *f );
int x_find( OBJECT *o, int num, FORM *f );

static FORM_TYPE forms[] = {
    { -1, NO_POS,   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 }, 0L, i_help, t_help,
      xx_help, u_help, 0L },	/* windowed */
    { -1, NO_POS-1, { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1 }, 0L, i_help, t_help,
      xx_help, u_help, 0L },	/* modal */
    { -1, NO_POS-2, { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, 0L, i_set, t_set,
      x_set, 0L, 0L },		/* font */
    { -1, NO_POS-3, { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, 0L, i_find, 0L,
      x_find, 0L, 0L }		/* find */
    };

void help_load( HELPCMD *h )	/************* MUST BE FIRST!! *************/
{
  (*h->nac->bytecpy)( &cmd, h, sizeof(HELPCMD) );
  (*h->nac->bytecpy)( &set, &(z=cmd.nac->mas->most)->help, sizeof(HELP_SET) );
  gui = z->gui;
  z->help.fulled = do_full;
  z->help.moved = do_move;
  z->help.use_menu = use_menu;
  z->help.new_topic = new_topic;
  z->help.wind = 0;
/*#ifdef DEBUG
  cmd.modal = 1;
#endif */
  help_main();
}

#include "fontlist.c"

static char *pathend( char *ptr )
{
  char *s;
  
  if( (s=strrchr(ptr,'\\')) == 0 ) return(ptr);
  return(s+1);
}

void set_if( int num, int true )
{
  if( true ) form[num].ob_state |= SELECTED;
  else form[num].ob_state &= ~SELECTED;
}

void modal_draw( int num )
{
  if( modal_open ) objc_draw( modal, num, 8, wsize.g_x, wsize.g_y, wsize.g_w, wsize.g_h );
}

void prev_chg( int state )
{
  if( modal[MOPREV].ob_state != state )
  {
    modal[MOPREV].ob_state = state;
    modal_draw( MOPREV );
  }
}

void enab_prev(void)
{
  int i, ok;
  
  if( menu )
  {
    i = prev_jump( jnum );
    menu_ienable( menu, MPREV, ok = hand>0 && jnum!=jstart && i!=jstart );
    if( cmd.modal ) 
      if( ok ) prev_chg( modal[MOPREV].ob_state & ~DISABLED );
      else prev_chg( modal[MOPREV].ob_state | DISABLED );
  }
}

void set_entries(void)
{
  int i;
  
  if( menu )
  {
    menu_ienable( menu, MFIND, i=hand>0 );
    menu_ienable( menu, MINDEX, i );
    menu_ienable( menu, MTEXT, i );
    menu_ienable( menu, MBLOCK, i=sel_row!=-99 );
    menu_ienable( menu, MCLIP, i );
    enab_prev();
  }
}

static void _spf(char *buf, char *fmt, ...) {
  (*cmd.nac->mas->dopf)(buf, fmt, (unsigned int *)&...);
}

void do_name( char *name )
{
  if( cmd.modal )
  {
    modal[MTITLE].ob_spec.free_string = name;
    modal_draw( MTITLE );
  }
  else wind_set( wind, WF_NAME, name );
}

void set_name( char *s )
{
  if( wind>0 || cmd.modal )
    if( hand>0 && s )
    {
      _spf( name, "%s: %s", *title, s );
      do_name(name);
    }
    else do_name(*title);
}

void set_info(void)
{
  if( cmd.modal )
  {
    modal[MSTAT].ob_spec.tedinfo->te_ptext = info_txt;
    modal_draw( MSTAT );
  }
  else wind_set( wind, WF_INFO, info_txt );
  enab_prev();
}

void clear_info( int set )
{
  info_txt[0] = info_max = 0;
  jnum = jstart = 0;
  if( set ) set_info();
}

void add_info( char *s )
{
  if( !info_max )
    if( strlen(info_txt) + strlen(s) >= sizeof(info_txt) ) info_max=1;
    else strcat( info_txt, s );
}

void init_line(void)
{
  memset( line[line_num]+1, ' ', LINELEN );
}

static void rsrc_adr( int type, int obj, void **tree )
{
  (*gui->Nrsc_gaddr)( type, obj, tree, rshdr );
}

void no_help( int draw )
{
  char **p;
  int i;
  
  for( line_num=bl_cols=0; line_num<5; line_num++ )
  {
    init_line();
    rsrc_adr( 15, EMPLINE+line_num, (void **)&p );
    strcpy( line[line_num]+1, *p );
    if( (i=strlen( line[line_num]+1 )) > bl_cols ) bl_cols = i;
    line[line_num][i+1] = ' ';
  }
  bl_rows = 5;
/*  know_cols = 0;*/
  new_screen(draw);
  clear_info(draw);
  set_name(0L);
}

static char *_lalloc( long len )
{
  return (*cmd.nac->mas->lalloc)( len, -1, 1 );
}

static void _lfree( void *p )
{
  (*cmd.nac->mas->lfree)(p);
}

static int _lrealloc( void *p, long size )
{
  return (*cmd.nac->mas->lrealloc)( p, size, 1 );
}

void close_help(void)
{
  if( hand>0 )
  {
    Fclose(hand);
    hand=0;
  }
  if( tbl1 )
  {
    _lfree(tbl1);
    tbl1 = 0L;
  }
  no_help(0);
  set_entries();
}

int alert( int num, int close )
{
  char **p;
  
  if( close )
  {
    close_help();
    no_help(1);
  }
  rsrc_adr( 15, num, (void **)&p );
  return form_alert( 1, *p );
}

static int buflen;

long _read( int hand, long size, void *out )
{
  static char buf[1024], *bufptr;
  int size0=size;
  
  while( size )
  {
    if( buflen<=0 )
      if( (buflen=Fread(hand,sizeof(buf),bufptr=buf)) < 0 ) return -1L;
      else if( !buflen ) return size0-size;
    *((char *)out)++ = *bufptr++;
    buflen--;
    size--;
  }
  return size0;
}

unsigned char get_nyb(void)
{
  static unsigned char byte;
  
  if( !nyb )
  {
    if( read_err ) return -1;
    if( _read( hand, 1L, &byte ) != 1L )
    {
      read_err++;
      return -1;
    }
    nyb++;
    return byte>>4;
  }
  nyb--;
  return byte&0xF;
}

unsigned char get_byte(void)
{
  return get_nyb()<<4 | get_nyb();
}

void new_line( void )
{
/*  *line_pos = 0;*/
  if( (line[line_num++][0] = line_cnt) > bl_cols ) bl_cols = line_cnt;
  count = in_rev = 0;
  line_pos = line[bl_rows=line_num]+1;
  line_cnt = line_len = 0;
  init_line();
}

void add_line( unsigned char c )
{
  if( c == '\n' && !count ) new_line();
  else if( (c != '\r' || count) && line_len<LINELEN )
  {
    if( count ) count--;
    else if( c==0x1d )
      if( !in_rev ) count = in_rev = 2;
      else in_rev = 0;
    else line_cnt++;
    if( line_num>=LINES ) read_err++;
    else
    {
      line_len++;
      *line_pos++ = c;
    }
  }
}

void init_unpack(void)
{
  line_num=line_cnt=line_len=0;
  line_pos=line[0]+1;
  count = in_rev = 0;
  bl_cols = 0;
/*  know_cols = 0;*/
  init_line();
}

#define N		 4096	/* size of ring buffer */
#define F		   18	/* upper limit for match_length */
#define THRESHOLD	2   /* encode string into position and length
						   if match_length is greater than this */
#define NIL			N	/* index for root of binary search trees */

unsigned long int
		textsize = 0,	/* text size counter */
		codesize = 0,	/* code size counter */
		printcount = 0;	/* counter for reporting progress every 1K bytes */
unsigned char
		text_buf[N + F - 1];	/* ring buffer of size N,
			with extra F-1 bytes to facilitate string comparison */
int		match_position, match_length,  /* of longest match.  These are
			set by the InsertNode() procedure. */
		lson[N + 1], rson[N + 257], dad[N + 1];  /* left & right children &
			parents -- These constitute binary search trees. */
FILE	*infile, *outfile;  /* input & output files */

long get_len;

int _getc(void)
{
  unsigned char c;
  
  if( !get_len ) return EOF;
  _read( hand, 1L, &c );
  if( read_err ) return EOF;
  get_len--;
  return c;
}

void Decode(void)	/* Just the reverse of Encode(). */
{
	int  i, j, k, r, c;
	unsigned int  flags;
	
	init_unpack();
	memset( text_buf, ' ', N-F );
/*	for (i = 0; i < N - F; i++) text_buf[i] = ' '; */
	r = N - F;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = _getc()) == EOF) break;
			flags = c | 0xff00;		/* uses higher byte cleverly */
		}							/* to count eight */
		if (flags & 1) {
			if ((c = _getc()) == EOF) break;
			add_line(c);  text_buf[r++] = c;  r &= (N - 1);
		} else {
			if ((i = _getc()) == EOF) break;
			if ((j = _getc()) == EOF) break;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				add_line(c);  text_buf[r++] = c;  r &= (N - 1);
			}
		}
	}
	if( read_err ) alert( READERR, 1 );
}

void unpack(void)
{
  unsigned char dat, *ptr;
  int i;

  nyb = 0;
  init_unpack();
  while( !read_err )
  {
    dat = get_nyb();
    if( dat<12 ) add_line( help.fast[dat] );
    else if( dat==12 ) add_line( get_byte() );
    else if( dat==13 )
    {
      i = (get_nyb()<<8) | get_byte();
      i = *(tbl2+1+i) - (long)(ptr = *(char **)(tbl2+i));
      ptr += (long)tbl2;
      while( i-- )
        add_line( *ptr++ ^ 0xA3 );
    }
    else if( dat==14 ) new_line();
    else
    {
      new_line();
      return;
    }
  }
  if( read_err ) alert( READERR, 1 );
}

static int search( char *topic, long start, long entries, long len, int cmp( const char *s1, const char *s2 ) )
{
  long l, min, max, ll, min0, max0;
  int *ent, *e, i, ret=0;
  char *s;

  if( !entries ) return 0;  
  if( (ent = (int *)_lalloc(len)) == 0 )
  {
    alert( ALNOMEM, 0 );
    return 0;
  }
  buflen = 0;
  if( Fseek( start, hand, 0 ) == start && _read( hand, len, ent ) == len )
      for( l=-1, max=entries, min=0;; )
  {
    ll = l;
    if( (l = (max+min+1)>>1) >= entries ) return 0;
    if( l == ll ) l--;
    e = (int *)((long)ent + 6*l);
    s = (char *)e + *(long *)e;
    min0 = min;
    max0 = max;
    if( (i = (*cmp)(s,topic)) < 0 ) min=l;
    else if( !i )
    {
      search_ret = geneva_fmt ? *(e+2) : (((*(e+2) & 0x7FF8) >> 1) - 1) >> 2;
      strcpy( search_top, s );
      ret = 1;
      break;
    }
    else max=l;
/*    if( max==min || !l || l==ll ) break; */
    if( min0==min && max0==max || !l ) break;
  }
  else alert( READERR, 1 );
  _lfree(ent);
  return ret;
}

void draw_sel(void)
{
  if( cmd.modal )
  {
    draw_mode = MODE_SEL;
    modal_draw( MDISP );
    draw_mode = MODE_ALL;
  }
  else if( wind>0 )
  {
    draw_mode = MODE_SEL;
    x_wdial_draw( wind, 0, 8 );
    draw_mode = MODE_ALL;
  }
}

void unselect(void)
{
  if( sel_row!=-99 ) draw_sel();
  sel_row = sel_col = sele_row = sele_col = -99;
  set_entries();
}

void list_tbl( long *tbl )
{
  int i;
  unsigned char dat, *ptr;
  
  sel_row=-99;	/* 003 */
  unselect();	/* 003 */
  buflen = 0;
  if( Fseek( *tbl, hand, 0 ) != *tbl )
  {
    alert( READERR, 1 );
    return;
  }
  if( geneva_fmt )
  {
    get_len = *(tbl+1) - *tbl;
    Decode();
  }
  else unpack();
  if( line_cnt ) new_line();
}

static char upcase;

static int match( const char *str, const char *pat )
{
  char s, p /*, per=0*/;
  
  for(;;)
  {
    if( (p = *pat++) == '\0' )
      if( *str ) return(-1);
      else return(0);
    s = *str++;
    if( p == '*' )
    {
      if( !s ) return(0);
      str--;
      do
      {
/*        if( *str == '.' ) per=1;*/
        if( !match( str, pat ) ) return(0);
      }
      while( *str++ );
/*      if( *pat++ != '.' ) return(0); */
      if( *pat == '*' )
        if( !*(pat+1) ) return(0);
        else return(1);
      else if( *pat /*|| per*/ ) return(1);
      return(0);
    }
    else if( p == '?' )
    {
      if( !s ) return(1);
    }
    else
    {
      if( upcase )
      {
        p = toupper(p);
        s = toupper(s);
      }
      if( p > s ) return(-1);
      else if( p < s ) return(1);
    }
  }
}

void find( char *topic, int sens, int revert, int all )
{
  int ret=0, first=1;
  int (*func)( const char *str, const char *pat ),
      (*func2)( const char *str, const char *pat );
  char temp[120], **p;
  DTA dta, *old=0L;

  if( strcspn( "*?", topic ) < 2 ) func = func2 = match;
  else
  {
    func = strcmpi;
    func2 = strcmp;
  }
  for(;;)
  {
    upcase = 1;
    if( !geneva_fmt )
    {
      if( (ret=search( topic, help.sens_start, help.sens_entries, help.sens_len, func ))==0 )
      {
        upcase = 0;
        if( (ret=search( topic, help.caps_start, help.caps_entries, help.caps_len, func2 ))==0 && !sens )
        {
          upcase = 1;
          ret=search( topic, help.caps_start, help.caps_entries, help.caps_len, func );
        }
      }
    }
    else if( (ret=search( topic, header.sens_start, header.sens_entries, header.sens_len, func ))==0 )
    {
      upcase = 0;
      if( (ret=search( topic, header.caps_start, header.caps_entries, header.caps_len, func2 ))==0 && !sens )
      {
        upcase = 1;
        ret=search( topic, header.caps_start, header.caps_entries, header.caps_len, func );
      }
    }
    if( ret || !all ) break;
    if( first )
    {
      first=0;
      old = Fgetdta();
      Fsetdta(&dta);
      revert = 1;
      if( Fsfirst(set.hlp_path,0x31) ) break;
    }
    else if( Fsnext() ) break;
    strcpy( temp, set.hlp_path );
    strcpy( pathend(temp), dta.d_fname );
    if( !open_help(temp) ) break;
  }
  if( old ) Fsetdta(old);
  if( !ret )
  {
    rsrc_adr( 15, ALNOTOP, (void **)&p );
    _spf( temp, *p, topic );
    form_alert( 1, temp );
    if( revert ) do_index();
  }
  else jump_tbl( search_ret, search_top );
}

int prev_jump( int num )
{
  return --num<0 ? JUMPS-1 : num;
}

int next_jump( int num )
{
  return ++num==JUMPS ? 0 : num;
}

void jump_tbl( int i, char *s )
{
  int j;
  
  if( i==-1 )
    if( jnum != jstart && prev_jump(jnum) != jstart )
    {
      jnum = prev_jump(jnum);
      i = jumps[prev_jump(jnum)];
      for( j=jstart, s=info_txt; j!=jnum; )
      {
        j = next_jump(j);
        if( (s = strchr(s,'|')) == 0 ) break;
        s++;
      }
      if( s && s!=info_txt ) *(s-2) = 0;
    }
    else return;
  else
  {
    add_info( jnum==jstart ? " " : " | " );
    add_info(s);
    jumps[jnum] = i;
    jnum = next_jump(jnum);
    if( jnum==jstart )
    {
      jstart = next_jump(jstart);
      if( info_max ) info_txt[0] = 0;
    }
  }
  set_info();
  list_tbl( tbl1+i );
  new_screen(1);
}

void do_index(void)
{
  char **p;
  
  unselect();
  clear_info(0);
  rsrc_adr( 15, INDEX, (void **)&p );
  jump_tbl( 1, *p );
}

static int select( int titl, char *path, int pthlen, char *name )
{
  int b, r;
  char **p;
  
  rsrc_adr( 15, titl, (void **)&p );
  if( cmd.has_Geneva ) r = x_fsel_input( path, pthlen, name, 1, &b, *p );
  else if( cmd.aes_ver >= 0x104 ) r = fsel_exinput( path, name, &b, *p );
  else r = fsel_input( path, name, &b );
  if( cmd.modal ) modal_draw(0);
  if( r && b ) return 1;
  return 0;
}

int open_help( char *s )
{
  long l;
  char dflt=0, temp[13];
  int ret=0;
  
  if( !s )
  {
    strcpy( temp, pathend(path) );
    strcpy( pathend(path), fname );
    dflt++;
    s = path;
  }
  close_help();
  buflen = 0;
  if( (hand = Fopen( s, 0 )) > 0 )
  {
    set_name( pathend(s) );
    if( tbl1 )
    {
      _lfree(tbl1);
      tbl1 = 0L;
    }
    geneva_fmt = 0;
    if( _read( hand, sizeof(header), &header ) == sizeof(header) )
      if( header.id == HELP_ID )
      {
        geneva_fmt=1;
        if( (tbl1 = (long *)_lalloc( l = header.tbl_len +
            header.caps_len + header.sens_len )) != 0 )
          if( _read( hand, l, tbl1 ) == l )
          {
            read_err=0;
            ret = 1;
          }
          else
          {
            _lfree(tbl1);
            tbl1 = 0L;
            alert( READERR, 1 );
          }
        else alert( ALMEMHLP, 1 );
      }
      else if( Fseek( 0L, hand, 0 ) == 0L )
      {
        buflen = 0;
        if( _read( hand, sizeof(help), &help ) != sizeof(help) ||
            help.id != 0x48322E30L ) alert( ALNOTHLP, 1 );
        else if( (tbl1 = (long *)_lalloc( l = help.tbl1_len + help.tbl2_len +
            help.caps_len + help.sens_len )) != 0 )
          if( _read( hand, l, tbl1 ) == l )
          {
            tbl2 = (long *)(help.tbl2_start - sizeof(help) + (long)tbl1);
            read_err=0;
            ret = 1;
          }
          else
          {
            _lfree(tbl1);
            tbl1 = 0L;
            alert( READERR, 1 );
          }
        else alert( ALMEMHLP, 1 );
      }
      else alert( READERR, 1 );
    else alert( ALNOTHLP, 1 );
  }
  else alert( ALOPEN, 0 );
  if( dflt ) strcpy( pathend(path), temp );
  set_entries();
  if( !ret ) no_help(1);
  return ret;
}

void do_quit(void)
{
  modal_open = 0;
  z->help.wind = wind = 0;
  if( rshdr ) close_help();
#ifndef DEBUG
  if( vdi_hand )
  {
    no_fonts();
    v_clsvwk(vdi_hand);
  }
#endif
  (*gui->Nrsc_free)( rshdr );
  if( cmd.modal )
  {
    strcpy( z->help.topic, set.topic );
    z->help.match = set.match;
    z->help.all = set.all;
    z->help.font = set.font;
    z->help.point = set.point;
    strcpy( z->help.hlp_path, set.hlp_path );
  }
  else (*cmd.nac->bytecpy)( &z->help, &set, sizeof(HELP_SET) );
  (*cmd.signal_exit)();
}

static void gtext( int x, int y, char *t, int len, int rev )
{
  int arr[4];
  void cur_point(void);
  
  if( draw_mode&MODE_TEXT )
  {
#ifdef DEBUG
    cur_point();
#endif
    _vswr_mode( MD_REPLACE );
    if( is_scalable )
    {
      text_2_arr( t, &len );
      ftext16_mono( x, y, len );
    }
    else v_gtext( vdi_hand, x, y, t );
  }
  if( rev && draw_mode&MODE_SEL )
  {
    arr[2] = (arr[0] = x) + char_w*len-1;
    arr[3] = (arr[1] = y) + char_h-1;
    _vswr_mode( MD_XOR );
    vr_recfl( vdi_hand, arr );
  }
}

static int rmin( int a, int b )
{
   if( a > b )
      return( b );
   else
      return( a );
}

static int rmax( int a, int b)
{
   if( a < b )
      return( b );
   else
      return( a );
}

static int rc_intersect(Rect *r1, Rect *r2)
{
   int xl, yu, xr, yd;                      /* left, upper, right, down */

   xl      = rmax( r1->x, r2->x );
   yu      = rmax( r1->y, r2->y );
   xr      = rmin( r1->x + r1->w, r2->x + r2->w );
   yd      = rmin( r1->y + r1->h, r2->y + r2->h );

   r2->x = xl;
   r2->y = yu;
   r2->w = xr - xl;
   r2->h = yd - yu;

   return( r2->w > 0 && r2->h > 0 );
}

static int cdecl draw_text( PARMBLK *pb )
{
  char *s, *t, dum, rev=0, und=0;
  int i, y, x, col, max, j;
  
  if( cmd.modal && !rc_intersect( (Rect *)&winner, (Rect *)&pb->pb_xc ) )
      return pb->pb_currstate;
  _vs_clip( 1, (Rect *)&pb->pb_xc );
  if( (i = (pb->pb_yc - pb->pb_y)/char_h) < 0 ) i=0;
  if( (col = bl_rows) < set.rows ) col = set.rows;
  if( (max = i + (pb->pb_hc+(char_h<<1)-1)/char_h) > col ) max = col;
  get_corner();
  if( draw_mode&MODE_TEXT && is_scalable ) _vst_charmap( 0 );	/* 003 */
  for( y=pb->pb_y+cmd.modal+i*char_h; i<max; i++, y+=char_h )
  {
    s=t=line[i]+1;
    x = pb->pb_x+cmd.modal;
    col=0;
    rev = (i>sel_row || i==sel_row && sel_col<=0) &&
          (i<sele_row || i==sele_row && sele_col>0);
    while(*s)
    {
      while( *s && *s!=0x1d &&
          (rev || i!=sel_row || col!=sel_col) &&
          (!rev || i!=sele_row || col!=sele_col) )
      {
        s++;
        col++;
      }
      if( draw_mode&MODE_TEXT ) _vst_effects( und ? 8 : 0 );
      dum = *s;
      *s = 0;
      if( *t ) gtext( x, y, t, s-t, rev );
      *s = dum;
      x += (s-t)*char_w;
      if( dum==0x1d )
        if( (und ^= 1) != 0 ) s+=3;
        else s++;
      else if( dum ) rev^=1;
      t = s;
    }
    if( (j=set.cols-col+corn_x) > 0 ) gtext( x, y,
        "                                 ", j, rev );
  }
  if( draw_mode&MODE_TEXT && is_scalable ) _vst_charmap( 1 );	/* 003 */
  return pb->pb_currstate;
}

int is_sep( unsigned char c )
{
  return strchr( " ,.!?/*&%$\"\';:|[]{}\t\x1d", c ) != 0;
}

int get_sel( int mode, char *out, int len, int full )
{
  static char *s, und, end;
  static int i, y, ymax;
  int olen;
  
  olen = 0;
  if( !mode )
  {
    und = end = 0;
    y = full ? 0 : sel_row;
    ymax = full ? bl_rows-1 : sele_row;
    i = 0;
    s = line[y]+1;
  }
  if( end ) return 0;
  for(;;)
  {
    while( i<=line[y][0] )
      if( *s!='\x1d' )
        if( full || y>sel_row || i>=sel_col )
        {
          if( !full && y==sele_row && i==sele_col )
          {
            *out = 0;
            end = 1;
            return olen;
          }
          *out++ = *s++;
          olen++;
          i++;
          if( !--len ) return olen;
        }
        else
        {
          i++;
          s++;
        }
      else if( (und ^= 1) != 0 ) s += 3;
      else s++;
    i = 0;
    s = line[++y] + 1;
    if( y>ymax ) break;
    *out++ = '\r';
    *out++ = '\n';
    olen += 2;
    if( (len-=2) <= 0 ) return olen;
  }
  if( !full ) *out = 0;
  end = 1;
  return olen;
}

void get_coords( int *x, int *y )
{
  if( cmd.modal )
  {
    *x -= winner.g_x + modal[MDISP].ob_x;
    *y -= winner.g_y+1 + modal[MDISP].ob_y;
  }
  else
  {
    *x -= blank[0].ob_x;
    *y -= blank[0].ob_y-1;
  }
  if( *x < 0 ) *x = -1;
  else *x /= char_w;
  if( *y < 0 ) *y = -1;
  else *y /= char_h;
}

int select_word( int x, int y )
{
  int i, ret, li;
  unsigned char *s, und, *last;
  
  get_coords( &x, &y );
  unselect();
  if( x<0 || y>=bl_rows ) return -1;
  for( i=li=0, und=0, s=last=line[y]+1; *s; s++ )
    if( *s!='\x1d' )
    {
      if( !und && is_sep(*s) )
      {
        last=s;
        li = i;
      }
      if( !x-- ) break;
      i++;
    }
    else if( (und ^= 1) != 0 )
    {
      last = s;
      li = i;
      s += 2;
    }
  if( !*s || !und && is_sep(*s) ) return -1;
  if( *last++ == '\x1d' && und )
  {
    ret = (*last++<<8) | *last;
    if( !geneva_fmt )
    {
      ret = (ret&0x7fff)/2 - 2;
      if( !(ret&1) ) ret -= 4;
      ret >>= 2;
    }
    else if( ret==-1 ) ret = 0xfff;
  }
  else ret = -1;
  sel_row = sele_row = y;
  if( und ) sel_col = li;
  else sel_col = li ? li+1 : 0;
  while( *s && (und || !is_sep(*s)) )
  {
    if( *s=='\x1d' ) break;
    i++;
    s++;
  }
  sele_col = i;
  draw_sel();
  return ret;
}

void but_up(void)
{
  int b, dum;
  
  do
    graf_mkstate( &dum, &dum, &b, &dum );
  while(b&1);
}

void calc_blank(void)
{
  int i;
  
  blank[0].ob_width = ((i=set.cols)>bl_cols ? i : bl_cols)*char_w;
  blank[0].ob_height = ((i=set.rows)>bl_rows ? i : bl_rows)*char_h;
  if( cmd.modal && blank[0].ob_width )
  {
    *(long *)&modal[MDISP].ob_width = *(long *)&blank[0].ob_width;
    if( blank[0].ob_height <= winner.g_h ) i = modal[BIGSL].ob_height;
    else if( (i = (long)modal[BIGSL].ob_height * winner.g_h / blank[0].ob_height ) <
        modal[MBLANK].ob_height ) i = modal[MBLANK].ob_height;
    modal[SMLSL].ob_height = i;
    if( blank[0].ob_width <= winner.g_w ) i = modal[BIGSLH].ob_width;
    else if( (i = (long)modal[BIGSLH].ob_width * winner.g_w / blank[0].ob_width ) <
        modal[MBLANK].ob_width ) i = modal[MBLANK].ob_width;
    modal[SMLSLH].ob_width = i;
  }
  else if( wind>0 )
  {
    wind_set( wind, X_WF_DIALWID, char_w );
    wind_set( wind, X_WF_DIALHT, char_h );
  }
}

static void get_outer(void)
{
  int w, h;

  if( !cmd.modal )
  {  
    winner.g_x = (winner.g_x+(char_w>>1))/char_w*char_w;
    if( (w = (winner.g_w+(char_w>>1))/char_w) > LINELEN ) w = LINELEN;
    if( (h = (winner.g_h+(char_h>>1))/char_h) > LINES ) h = LINES;
    winner.g_w = w*char_w;
    winner.g_h = h*char_h;
    x_wind_calc( WC_BORDER, WIN_TYPE, X_MENU, winner.g_x, winner.g_y,
        winner.g_w, winner.g_h, &wsize.g_x, &wsize.g_y, &wsize.g_w,
        &wsize.g_h );
    if( wsize.g_w < min_wid ) winner.g_w += char_w;
    if( wsize.g_h < min_ht ) winner.g_h += char_h;
    x_wind_calc( WC_BORDER, WIN_TYPE, X_MENU, winner.g_x, winner.g_y,
        winner.g_w, winner.g_h, &wsize.g_x, &wsize.g_y, &wsize.g_w,
        &wsize.g_h );
  }
  set.cols = winner.g_w/char_w;
  set.rows = winner.g_h/char_h;
  set.xoff = winner.g_x/char_w;
  set.yoff = wsize.g_y - max.g_y;
  calc_blank();
}

void get_inner(void)
{
  int min, bw, bh;
  OBJECT *o;
  
  if( !cmd.modal )
  {
    x_wind_calc( WC_WORK, WIN_TYPE, X_MENU, wsize.g_x, wsize.g_y,
        wsize.g_w, wsize.g_h, &winner.g_x, &winner.g_y, &winner.g_w,
        &winner.g_h );
  }
  else
  {
    o = modal;
    bw = o[MBLANK].ob_width;
    min = (320-16-bw+char_w-1) / char_w * char_w;
    if( (winner.g_w = (max.g_w-16-bw) / char_w * char_w) > char_w*80 )
        winner.g_w = char_w*80;
    if( winner.g_w < min ) winner.g_w = min;
    o[MSTAT].ob_width = winner.g_w + bw - 2 - o[MOPREV].ob_width;
    o[MMOVE].ob_x = (o[0].ob_width = winner.g_w+1 + bw + 16) -
        o[MMOVE].ob_width;
    o[MTITLE].ob_width = o[MMOVE].ob_x - 8;
    o[MBOX].ob_width = (o[MDISP].ob_width = winner.g_w) + 1;
    o[RIGHT].ob_x = (o[UP].ob_x = o[BIGSL].ob_x = o[DOWN].ob_x =
        o[MBLANK].ob_x = winner.g_w+1 + 8) - bw;
    o[BIGSLH].ob_width = winner.g_w+1 - (bw<<1) + 2;
    bh = o[MBLANK].ob_height;
    o[MDISP].ob_height = winner.g_h =
        (max.g_h - bh - o[MBOX].ob_y - 8) / char_h * char_h;
    o[0].ob_height = o[MBOX].ob_y + winner.g_h+1 + bh + 8;
    o[BIGSL].ob_height = (o[MBOX].ob_height = winner.g_h+1) - (bh<<1) + 2;
    o[DOWN].ob_y = (o[LEFT].ob_y = o[BIGSLH].ob_y = o[RIGHT].ob_y =
        o[MBLANK].ob_y = o[MBOX].ob_y + winner.g_h+1) - bh;
    o[BIGSL].ob_y = (o[UP].ob_y = o[MBOX].ob_y) + bh - 1;
  }
  get_outer();
}

void read_string( char *s, int max, int size )
{
  char dummy;

  max--;
  if( size<=max )        /* the whole string fits */
      appl_read( apid, size, s );
  else
  {
    /* read what we can */
    appl_read( apid, max-1, s );
    s[max-1] = '*';     /* terminate the string */
    s[max] = 0;
    /* now, get the remaining bytes of the string */
    for( size=size-(max-1); size>0; size-- )
      appl_read( apid, 1, &dummy );
  }
}

void sliders( int hv, int draw )
{
  int i;
  
  if( modal_open ) modal[SMLSL].ob_flags = modal[SMLSLH].ob_flags &= ~HIDETREE;
  if( hv&1 )
  {
    i = blank[0].ob_width-winner.g_w;
    modal[SMLSLH].ob_x = i>0 ? -(long)modal[MDISP].ob_x *
        (modal[BIGSLH].ob_width-modal[SMLSLH].ob_width) / i : 0;
    if( draw ) modal_draw( BIGSLH );
  }
  if( hv&2 )
  {
    i = blank[0].ob_height-winner.g_h;
    modal[SMLSL].ob_y = i>0 ? -(long)modal[MDISP].ob_y *
        (modal[BIGSL].ob_height-modal[SMLSL].ob_height) / i : 0;
    if( draw ) modal_draw( BIGSL );
  }
}

void redraw_txt( Rect *r )
{
  PARMBLK pb;
  
  *(Rect *)&pb.pb_xc = *r;
  objc_offset( modal, MDISP, &pb.pb_x, &pb.pb_y );
  *(long *)&pb.pb_w = *(long *)&modal[MDISP].ob_width;
  graf_mouse( M_OFF, 0L );
  draw_text( &pb );
  graf_mouse( M_ON, 0L );
}

void scroll_mdial( int flag, int val, int realtime )
{
  int i, j, imax, jmax, a;
  Rect r1, r2, redraw;

    i = modal[MDISP].ob_x;
    j = modal[MDISP].ob_y;
    if( (imax=modal[MDISP].ob_width-winner.g_w) <= 0 ) imax = 0;
    if( (jmax=modal[MDISP].ob_height-winner.g_h) <= 0 ) jmax = 0;
    redraw = r1 = r2 = *(Rect *)&winner;
    switch( flag )
    {
      case WA_LFPAGE:
        i += winner.g_w;
        goto hor;
      case WA_RTPAGE:
        i -= winner.g_w;
        goto hor;
      case WA_LFLINE:
        i += char_w;
        goto hor;
      case WA_RTLINE:
        i -= char_w;
        goto hor;
      case WM_HSLID:
        i = !realtime ? (long)imax * -val / 1000L : -val;
hor:    i = i/char_w*char_w;
        if( i < -imax ) i = -imax;
        else if( i>0 ) i = 0;
        if( modal[MDISP].ob_x==i ) return;
        if( (a=abs(j=i-modal[MDISP].ob_x)) < winner.g_w )
        {
          r1.w = r2.w -= (redraw.w=a);
          if( j>0 ) r2.x += j;
          else
          {
            r1.x -= j;
            redraw.x = r2.x+r2.w;
          }
          x_graf_blit( (GRECT *)&r1, (GRECT *)&r2 );
        }
        modal[MDISP].ob_x = blank[0].ob_x = i;
        break;
      case WA_UPPAGE:
        j += winner.g_h;
        goto vert;
      case WA_DNPAGE:
        j -= winner.g_h;
        goto vert;
      case WA_UPLINE:
        j += char_h;
        goto vert;
      case WA_DNLINE:
        j -= char_h;
        goto vert;
      case WM_VSLID:
        j = !realtime ? (long)jmax * -val / 1000L : -val;
vert:   j = j/char_h*char_h;
        if( j < -jmax ) j = -jmax;
        else if( j>0 ) j = 0;
        if( modal[MDISP].ob_y==j ) return;
        if( (a=abs(i=j-modal[MDISP].ob_y)) < winner.g_h )
        {
          r1.h = r2.h -= (redraw.h=a);
          if( i>0 ) r2.y += i;
          else
          {
            r1.y -= i;
            redraw.y = r2.y+r2.h;
          }
          x_graf_blit( (GRECT *)&r1, (GRECT *)&r2 );
        }
        modal[MDISP].ob_y = blank[0].ob_y = j;
        break;
    }
    sliders( flag<WA_LFPAGE ? 2 : 1, !realtime );
    redraw_txt( &redraw );
}

int mslider( int dir )
{
  int i, big, small, mult;

  mult = !dir ? char_w : char_h;
  if( graf_slidebox( 0L, !dir ? modal[MDISP].ob_width/mult :
      modal[MDISP].ob_height/mult, !dir ? winner.g_w/mult :
      winner.g_h/mult, 0x100|dir ) >= 0 )
  {
    small = !dir ? SMLSLH : SMLSL;
    modal[small].ob_state |= SELECTED;
    modal_draw( small );
    big = !dir ? BIGSLH : BIGSL;
    i = graf_slidebox( modal, big, small, 0x200|dir );
    while( i>=0 )
    {
      scroll_mdial( !dir ? WM_HSLID : WM_VSLID, i*mult, 1 );
      i = graf_slidebox( modal, big, small, 0x300|dir );
    }
    modal[small].ob_state &= ~SELECTED;
    modal_draw( small );
  }
  return 1;
}

void new_screen( int draw )
{
  int i;

/*  if( !know_cols )
  {  
    know_cols = 1;
    for( i=bl_cols=0; i<bl_rows; i++ )
    {
      if( line[i][0] > bl_cols ) bl_cols = line[i][0];
      if( llen[i] < LINELEN ) line[i][llen[i]+1] = ' ';
    }
  } */
  for( line_num=bl_rows; line_num<LINES; line_num++ )
  {
    line[line_num][0] = 0;
    init_line();
  }
  if( draw && (cmd.modal || wind>0) )
  {
    calc_blank();
    if( cmd.modal )
    {
      modal[MDISP].ob_x = 0;
      modal[MDISP].ob_y = 0;
      sliders( 3, 1 );
      modal_draw( MDISP );
    }
    else
    {
      blank[0].ob_x = winner.g_x;
      blank[0].ob_y = winner.g_y;
      wind_set( wind, X_WF_DIALOG, blank );
    }
  }
}

int start_hform( long type, int num )
{
  forms[num].memory = 0L;
  forms[num].tree = 0L;
  return (*gui->start_form)( cmd.AES_handle, type, &forms[num] );
}

int open_wind(void)
{
  int i, dum;
  
  if( !cmd.modal && (i=x_wind_create( WIN_TYPE, X_MENU, wsize.g_x,
      wsize.g_y, wsize.g_w, wsize.g_h )) > 0 )
  {
    wind_delete(i);
    set_name(0L);
    clear_info(1);
    if( !start_hform( WIN_TYPE|((long)X_MENU<<16), 0 ) ) return 0;
    return 1;
  }
  cmd.modal = 1;
  set_name(0L);
  clear_info(1);
  modal[SMLSL].ob_flags = modal[SMLSLH].ob_flags |= HIDETREE;
  do
  {
    get_inner();
    modal[MDISP].ob_spec.userblk = &ub_main;
    x_form_center( modal, &wsize.g_x, &wsize.g_y, &wsize.g_w, &wsize.g_h );
    objc_offset( modal, MBOX, &winner.g_x, &winner.g_y );
    winner.g_x++;
    winner.g_y++;
    i = start_hform( 0L, 1 );
    *(long *)&modal[MDISP].ob_x = 0L;
    *(long *)&blank[0].ob_x = 0L;
    sliders( 3, 0 );
  }
  while( i==MSET );
  return i;
}

#pragma warn -par
int t_set( OBJECT *o, int num, FORM *f )
{
  fontup( o, num );
  return 0;
}

void new_size(void)
{
  get_inner();
  if( wind>0 ) wind_set( wind, WF_CURRXYWH, wsize.g_x, wsize.g_y, wsize.g_w, wsize.g_h );
  new_screen(1);	/* 003: moved here */
}

int x_set( OBJECT *o, int num, FORM *f )
{
  finish_font(num);
  if( !cmd.modal )
  {
    (*gui->prev_blit)( f, 1 );
    new_size();
/*    new_screen(1);  003 */
    (*gui->prev_blit)( f, 0 );
  }	/* else, let reinit handle it */
  return 1;
}

int i_set( OBJECT *o, FORM *f )
{
  int dum;

  if( !o )
  {
    rsrc_adr( 0, WINDOPTS, (void **)&f->tree );
    x_form_center( f->tree, &dum, &dum, &dum, &dum );
    return 1;
  }
  return font_sel( o, SYSTEM, &set.font, &set.point );
}
#pragma warn +par

void initialize(void)
{
  int i;
  
  init_font( &set.font, &set.point );
  _vst_alignment( 0, 5 );
  if( !cmd.modal )
  {
    winner.g_x = set.xoff<<3;
    winner.g_w = char_w*set.cols;
    winner.g_h = char_h*set.rows;
    i = set.yoff;
    get_outer();
    wsize.g_y = max.g_y + i;
    get_inner();
    if( wsize.g_w > max.g_w )
    {
      wsize.g_w = max.g_w;
      get_inner();
    }
    if( wsize.g_h > max.g_h )
    {
      wsize.g_h = max.g_h;
      get_inner();
    }
  }
}

void sel_region( int x, int y, int ex, int ey )
{
  if( x==sel_col && y==sel_row )
  {
    if( ey==sele_row && ex==sele_col ) return;
    else if( ey<sele_row || ey==sele_row && ex<sele_col )
    { /* un-xor the tail end */
      sel_col = ex;
      sel_row = ey;
    }
    else
    { /* add the tail end */
      sel_col = sele_col;
      sel_row = sele_row;
      sele_col = ex;
      sele_row = ey;
    }
    draw_sel();
    sel_col = x;
    sel_row = y;
    sele_col = ex;
    sele_row = ey;
  }
  else if( ex==sele_col && ey==sele_row )
  {
    if( y<sel_row || y==sel_row && x<sel_col )
    { /* add the tail end */
      sele_col = sel_col;
      sele_row = sel_row;
      sel_col = x;
      sel_row = y;
    }
    else
    { /* un-xor the tail end */
      sele_col = x;
      sele_row = y;
    }
    draw_sel();
    sel_col = x;
    sel_row = y;
    sele_col = ex;
    sele_row = ey;
  }
  else
  {
    sel_col = x;
    sel_row = y;
    sele_col = ex;
    sele_row = ey;
    draw_sel();
  }
}

void get_corner(void)
{
  corn_x = winner.g_x;
  corn_y = winner.g_y;
  get_coords( &corn_x, &corn_y );
  if( corn_x<0 ) corn_x = 0;
  if( corn_y<0 ) corn_y = 0;
}

void udlr( int num, int msg )
{
  int x, y, b, k;

  graf_mouse( M_OFF, 0L );
  if( num>=0 )
  {
    modal[num].ob_state |= SELECTED;
    modal_draw(num);
  }
  do
  {
    scroll_mdial( msg, 0, 0 );
    graf_mkstate( &x, &y, &b, &k );
  }
  while( num>=0 && b&1 );
  if( num>=0 )
  {
    modal[num].ob_state &= ~SELECTED;
    modal_draw(num);
  }
  graf_mouse( M_ON, 0L );
}

void scroll( int dir )
{
  int msg[8];

  if( cmd.modal ) udlr( -1, dir );
  else
  {  
    msg[0] = WM_ARROWED;
    msg[3] = wind;
    msg[4] = dir;
    (*gui->xtern.arrow_dial)( msg );
  }
  get_corner();
}

void unsel( int x1, int y1, int x2, int y2 )
{
  int t1, t2, t3, t4;
  
  t1 = sel_col;
  t2 = sel_row;
  t3 = sele_col;
  t4 = sele_row;
  sel_col = x1;
  sel_row = y1;
  sele_col = x2;
  sele_row = y2;
  draw_sel();
  sel_col = t1;
  sel_row = t2;
  sele_col = t3;
  sele_row = t4;
}

void sel_mouse( int x, int y, int b, int k )
{
  int first=1, shift, anc_x, anc_y, dir=0, xmax, ymax, dum;
  
  if( (xmax = set.cols) < bl_cols ) xmax = bl_cols;
  if( (ymax = set.rows) < bl_rows ) ymax = bl_rows;
  for(;;)
  {
    if( !first ) graf_mkstate( &x, &y, &b, &k );
    shift = k&3;
    get_corner();
    if( !x )
    {	/* mouse at left edge of screen: special case */
      x = corn_x-1;
      get_coords( &dum, &y );
    }
    else get_coords( &x, &y );
    if( !(b&1) ) break;
    if( x-corn_x>set.cols )
    {
      scroll( shift ? WA_RTPAGE : WA_RTLINE );
      if( (x = corn_x+set.cols) > xmax ) x = xmax;
    }
    else if( x<corn_x )
    {
      scroll( shift ? WA_LFPAGE : WA_LFLINE );
      x = corn_x;
    }
    if( y-corn_y>=set.rows )
    {
      scroll( shift ? WA_DNPAGE : WA_DNLINE );
      if( (y = corn_y+set.rows) >= ymax ) y = ymax-1;
      x = xmax;
    }
    else if( y<corn_y )
    {
      scroll( shift ? WA_UPPAGE : WA_UPLINE );
/*      if( (y = corn_y-1) < 0 ) corn_y = 0; */
      y = corn_y;
      x = -1;
    }
    if( first )
    {
      graf_mouse( TEXT_CRSR, 0L );
      if( shift && sel_row!=-99 )
        if( y<sel_row || y==sel_row && x<sel_col ) sel_region( x, y, anc_x=sel_col, anc_y=sel_row );
        else sel_region( anc_x=sel_col, anc_y=sel_row, x, y );
      else
      {
        anc_x = x;
        anc_y = y;
        unselect();
      }
    }
    else if( y<anc_y || y==anc_y && x<anc_x )
    {
      if( dir>0 ) unsel( anc_x, anc_y, sele_col, sele_row );
      sel_region( x, y, anc_x, anc_y );
      dir = -1;
    }
    else if( y>anc_y || y==anc_y && x>anc_x )
    {
      if( dir<0 ) unsel( sel_col, sel_row, anc_x, anc_y );
      sel_region( anc_x, anc_y, x, y );
      dir = 1;
    }
    first=0;
  }
  if( first )
    if( shift && sel_row!=-99 )
    {
      if( y<sel_row || y==sel_row && x<sel_col ) sel_region( x, y, sele_col, sele_row );
      else if( y>sel_row || y==sel_row && x>sel_col ) sel_region( sel_col, sel_row, x, y );
    }
    else unselect();
  graf_mouse( ARROW, 0L );
  evnt_button( 2, 1, 0, &x, &y, &b, &k );
}

static int get_clip( char *out )
{
  if( x_scrp_get( out, 1 ) )
  {
    strcat( out, "SCRAP.TXT" );
    return 1;
  }
  return 0;
}

void save_txt( int full, int clip )
{
  int h, i, l;
  char temp[120+13];
  
  if( clip || select( SAVEPATH, savepth, sizeof(savepth), savename ) )
  {
    if( clip )
    {
      if( !get_clip(temp) ) return;
    }
    else
    {
      strcpy( temp, savepth );
      strcpy( pathend(temp), savename );
    }
    if( (h=Fcreate(temp,0)) < 0 ) alert( NOCREAT, 0 );
    else
    {
      i = 0;
      while( (l=get_sel( i, temp, sizeof(temp)-1, full )) > 0 )
      {
        if( Fwrite( h, l, temp ) != l )
        {
          alert( NOWRITE, 0 );
          break;
        }
        i = 1;
      }
      Fclose(h);
    }
  }
}

void chop_pull( int start, int end )
{
  char *p;
  int i;
  OBJECT *o;
  
  if( (p = strchr( (o=&menu[start])->ob_spec.free_string, '^' )) != 0 )
  {
    i = p - o->ob_spec.free_string;
    while( start<=end )
    {
      if( *(p=o->ob_spec.free_string)==' ' )
      {
        while( p[i]!=' ' ) i--;
        p[i] = 0;
      }
      start++;
      o++;
    }
  }
}

#pragma warn -par
int i_help( OBJECT *o, FORM *f )
{
  if( !o )
  {
    if( cmd.modal ) f->tree = modal;
    else
    {
      blank[0].ob_x = winner.g_x;
      blank[0].ob_y = winner.g_y;
      f->tree = blank;
    }
    return 1;
  }
  unselect();
  calc_blank();
  o[1].ob_flags |= HIDETREE;
  if( f->handle>0 )
  {
    wind_set( z->help.wind=wind=f->handle, X_WF_MENU, menu );
    wind_get( f->handle, X_WF_MINMAX, &min_wid, &min_ht, &dum, &dum );
  }
  else
  {
    o[MFILE].ob_spec.free_string = menu[TFILE].ob_spec.free_string+1;
    o[MTOPIC].ob_spec.free_string = menu[TTOPIC].ob_spec.free_string+1;
    chop_pull( MHELP, MQUIT );
    chop_pull( MFIND, MUSING );
  }
  return 1;
}

void do_cmd(void)
{
  if( cmd.file && cmd.file[0] )
  {
    if( cmd.file[1] != ':' )
    {
      strcpy( hlpfile, z->dflt_path );
      strcat( hlpfile, cmd.file );
    }
    else strcpy( hlpfile, cmd.file );
    if( Fsfirst( hlpfile, 0x37 ) )
    {
      strcpy( hlpfile, pathend(cmd.file) );
      shel_find( hlpfile );
    }
    cmd.file = 0L;
    if( open_help(hlpfile) )
      if( cmd.topic && cmd.topic[0] )
      {
        find( cmd.topic, 0, 1, 1 );
        cmd.topic = 0L;
      }
      else do_index();
  }
}

void new_topic( HELPCMD *h )
{
  cmd.file = h->file;
  cmd.topic = h->topic;
  cmd.caps = h->caps;
  do_cmd();
}

int u_help( OBJECT *o, FORM *f )
{
  if( cmd.modal ) modal_open = 1;
/*%  else wind_set( wind, X_WF_DIALOG, blank ); */
  do_cmd();
  f->update = 0L;
  return 1;
}
char font_quit;
int xx_help( OBJECT *o, int num, FORM *f )
{
  if( cmd.modal && num==MSET )
  {
    use_menu( MFONT, -1 );
    return font_quit;
  }
  do_quit();
  return 1;
}
int fake_menu( int num, int parent )
{
  int i;
  
  if( (i = do_popup( modal, num, menu, parent, 0 )) >= 0 )
    if( (i+=parent+1)==MQUIT )
    {
      do_quit();	/* 004 */
      return 1;
    }
    else use_menu( i, -1 );
  return 0;
}
int t_help( OBJECT *o, int num, FORM *f )
{
  int x, y, b, k, i;
  
  if( cmd.modal ) switch( i=num&0x7ff )
  {
    case MFILE:
      return fake_menu( i, MHELP-1 );
    case MTOPIC:
      return fake_menu( i, MFIND-1 );
    case MOPREV:
      if( !(o[MOPREV].ob_state & DISABLED) ) use_menu( MPREV, -1 );
      if( o[MOPREV].ob_state & SELECTED )
      {		/* Undo might be pressed when disabled */
        form = o;
        set_if( MOPREV, 0 );
        modal_draw( MOPREV );
      }
      return 0;
    case BIGSL:
      objc_offset( o, SMLSL, &x, &y );
      i = y;
      graf_mkstate( &x, &y, &b, &k );
      scroll_mdial( y < i ? WA_UPPAGE : WA_DNPAGE, 0, 0 );
      return 0;
    case BIGSLH:
      objc_offset( o, SMLSLH, &x, &y );
      i = x;
      graf_mkstate( &x, &y, &b, &k );
      scroll_mdial( x < i ? WA_LFPAGE : WA_RTPAGE, 0, 0 );
      return 0;
    case SMLSL:
      mslider(1);
      return 0;
    case SMLSLH:
      mslider(0);
      return 0;
    case LEFT:
      udlr( LEFT, WA_LFLINE );
      return 0;
    case RIGHT:
      udlr( RIGHT, WA_RTLINE );
      return 0;
    case UP:
      udlr( UP, WA_UPLINE );
      return 0;
    case DOWN:
      udlr( DOWN, WA_DNLINE );
      return 0;
  }
  graf_mkstate( &x, &y, &b, &k );
  if( num<0 )
  {
    i = select_word( x, y );
    but_up();
    if( i>=0 )
    {
      get_sel( 0, hlpfile, sizeof(hlpfile), 0 );
      unselect();
      if( i==0xfff ) find( hlpfile, 0, 0, 1 );
      else jump_tbl( i, hlpfile );
    }
  }
  else sel_mouse( x, y, b, k );
  set_entries();
  return 0;
}

int i_find( OBJECT *o, FORM *f )
{
  int dum;
  
  if( !o )
  {
    rsrc_adr( 0, FIND, (void **)&f->tree );
    x_form_center( f->tree, &dum, &dum, &dum, &dum );
    return 1;
  }
  strcpy( o[FTOPIC].ob_spec.tedinfo->te_ptext, set.topic );
  form = o;
  set_if( FMATCH, set.match );
  set_if( FALL, set.all );
  return 1;
}

int x_find( OBJECT *o, int num, FORM *f )
{
  if( num==FOK )
  {
    strcpy( set.topic, o[FTOPIC].ob_spec.tedinfo->te_ptext );
    set.match = o[FMATCH].ob_state&SELECTED;
    set.all = o[FALL].ob_state&SELECTED;
  }
  return 1;
}
#pragma warn +par

void do_move( int *buf )
{
  wsize = *(GRECT *)&buf[4];
  new_size();
}

void do_full( int *buf )
{
  static char fulled;
  
  if( !fulled ) *(GRECT *)&buf[4] = max;
  else wind_get( wind, WF_PREVXYWH, &buf[4], &buf[5], &buf[6], &buf[7] );
  fulled ^= 1;
  do_move(buf);
}

static void use_menu( int num, int title )
{
  char **p, topic[21];
  
  switch( num )
  {
    case MHELP:
      if( !select( HELPTITL, path, sizeof(path), fname ) || !open_help(0L) ) break;
    case MINDEX:
      do_index();
      break;
    case MQUIT:
      (*gui->use_form)( wind, -1 );
      break;
    case MFONT:
      font_quit = start_hform( 0L, 2 ) == FOOK;
      break;
    case MFIND:
      if( start_hform( 0L, 3 ) == FOK && set.topic[0]/* 003 */ )
          find( set.topic, set.match, 0, set.all );
      break;
    case MPREV:
      unselect();
      jump_tbl( -1, 0L );
      break;
    case MUSING:
      strcpy( hlpfile, z->dflt_path );
      strcat( hlpfile, "NEODESK.HLP" );
      if( open_help(hlpfile) )
      {
        rsrc_adr( 15, HELPSTR, (void **)&p );
        find( *p, 0, 1, 0 );
      }
      break;
    case MHPATH:
      topic[0] = 0;
      strcpy( hlpfile, set.hlp_path );
      strcpy( pathend(hlpfile), "*.HLP" );
      if( select( HELPPATH, hlpfile, sizeof(set.hlp_path), topic ) )
      {
        strcpy( set.hlp_path, hlpfile );
        strcpy( pathend(set.hlp_path), "*.HLP" );
      }
      break;
    case MBLOCK:
      save_txt( 0, 0 );
      break;
    case MTEXT:
      save_txt( 1, 0 );
      break;
    case MCLIP:
      save_txt( 0, 1 );
      break;
  }
  if( title>=0 ) menu_tnormal( menu, title, 1 );
}

void help_main(void)
{
  char rsc[120];
  
  /* default item selector path */
  strcpy( path, z->dflt_path );
  strcpy( savepth, path );
  strcat( path, "*.HLP" );
  strcpy( set.hlp_path, path );		/* default aux help path */
  strcat( savepth, "*.TXT" );
  strcpy( savename, "HELP.TXT" );
#ifndef DEBUG
  strcpy( rsc, z->dflt_path );
  strcat( rsc, "HELP.RSC" );
#else
  strcpy( rsc, "H:\\SOURCE\\NEODESK\\HELP\\HELP.RSC" );
#endif
  switch( (*gui->Nrsc_load)( rsc, Fgetdta(), &rshdr ) )
  {
    case 0:
      form_alert( 1, "[1][|HELP.RSC not found!][Cancel]" );
      return;
    case -1:
    case -2:
      return;
  }
  (*gui->Nrsc_rcfix)( rshdr );
#ifndef DEBUG
  vdi_hand = graf_handle( &dum, &dum, &dum, &dum );
  work_in[0] = Getrez() + 2;
  vdi_reset();
  v_opnvwk( work_in, &vdi_hand, work_out );
  if( !vdi_hand )
  {
    do_quit();
    return;
  }
#endif
  *(Rect *)&max = z->maximum;
  _vswr_mode( MD_REPLACE );
  _vsf_color( 0 );
  rsrc_adr( 15, TITLE, (void **)&title );
  rsrc_adr( 0, HMENU, (void **)&menu );
  rsrc_adr( 0, MODAL, (void **)&modal );
  menu[0].ob_state |= X_MAGIC;
  has_gdos = vq_gdos();
  initialize();
  set_entries();
  no_help(0);
  graf_mouse( ARROW, 0L );
  open_wind();
}

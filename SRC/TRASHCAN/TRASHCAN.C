#define TURBO_C

#include "tos.h"
#include "new_aes.h"
#include "string.h"
#include "stdlib.h"
#include "..\neocommn.h"
#include "trashcan.h"

#define BUSY_BEE HOURGLASS

typedef struct F_Type
{
  unsigned char menu_num;		/* menu string index */
  int treenum;				/* rsc file index */
  FORMFLAGS flags;
  long memory;				/* amount of memory */
  int (*init)( OBJECT *o, struct F_Type *f );             /* function to initialize it */
  int (*touch)( OBJECT *o, int num, struct F_Type *f );   /* called when TOUCHEXIT is clicked */
  int (*exit)( OBJECT *o, int num, struct F_Type *f );    /* called when EXIT is clicked */
  OBJECT *tree;                         /* dial's object tree */
} F_TYPE;

void sort(void);
void set_buttons( OBJECT *o, F_TYPE *f );
int read_fat( BPB *bp, unsigned int *cluster, int drv );
int check_folders(void);
int writ( int hand, long n, char *buf );
void mediach(void);
void update(void);
void dialog(void);
void read_data( OBJECT *o, F_TYPE *f );
void write_rsc( OBJECT *o );
void ack(void);
void pause(void);
int f_alert( int num, char *s, int mode );

#define rindex  strrchr
#define index   strchr

#define VERSION   0x0100
#define SEC_SIZE  2048

typedef struct ddnd DND;
typedef struct ddmd DMD;
struct ddnd          /* directory node descriptor */
{
        char    d_name[11];
        char    d_fill;
        int     d_flag;
        int     d_strtcl;
        int     d_time;
        int     d_date;
        long    *d_ofd;   /* OFD * */
        DND     *d_parent;
        DND     *d_left; /* child */
        DND     *d_right; /* sib-link */
        DMD     *d_drv;
        long    *d_dirfil;  /* OFD * */
        long    d_dirpos;
        long    d_scan; /* current position in dir for DND tree */
        long    *d_files; /* open files on this node OFD * */
};

struct ddmd           /* drive media block */
{
        int     m_recoff[3]; /* record offsets for fat,dir,data */
        int     m_drvnum;
        int     m_fsiz;
        int     m_clsiz;
        int     m_clsizb;
        int     m_recsiz;

        int     m_numcl;
        int     m_clrlog; /* clsiz in rec, log2 is shift */
        int     m_clrm;  /* clsiz in rec, mask */
        int     m_rblog;  /* recsiz in bytes, shift */
        int     m_rbm;   /* recsiz in bytes, mask */
        int     m_clblog;
        long    *m_fatofd;      /* OFD * */

        long    *m_ofl;         /* OFD * */
        DND     *m_dtl; /* directory tree list */
        int     m_16; /* 16 bit fat ? */
};

#define dmd_12(d) ((DMD *)((long *)0x8380L)[d])
#define dnd_12(d) dmd_12(d) ? dmd_12(d)->m_dtl : 0L;
#define dnd_10(d) ((DND *)((long *)0x5650L)[d])
#define OS_version  (*(int *)((*(long *)0x4F2)+2))

/* first two must be in this order */
char sec_buf[SEC_SIZE], fat[SEC_SIZE*2], data_name[] = "x:\\trashcan.dat", 
    root[]="x:\\*.*", asked, update_list[16], dflt_drive,
    rsc_name[]="TRASHCAN.RSC", fake[]="x:\\000.XXX", wdraw_ok;
int ver=VERSION, window, items, num_sel, first, longest, dts, xoff, idt_fmt,
    w_handle, blit_ok, delta, retries, apid;
unsigned long table_items;
unsigned int ents_sec, fat_sec;
BPB *bp0;
GRAPHICS *graphics;

int dum,
    conf_on=1,
    drive,
    vals,
    text_h,
    neo_apid;           /* NeoDesk's AES application ID */
                           
extern char *index(), *rindex();

typedef struct
{
  unsigned char name[8], ext[3], att, unused[10];
  unsigned int time, date, cluster;
  unsigned long len;
} Dirent;

typedef struct
{
  char path[118];
  Dirent dir;
  unsigned int next_cl;
} Tablent;

typedef struct
{
  unsigned long len;
  unsigned int time, date;
  char hidden, selected, dir, space;
  char path[118];
  int link;
} Fname;

NEO_ACC *neo_acc=0L;                    /* points to NeoDesk's functions */
Fname *fname;
Rect max;

int i_main( OBJECT *o, F_TYPE *f );
int t_main( OBJECT *o, int num, F_TYPE *f );
int x_main( OBJECT *o, int num, F_TYPE *f );
int i_dum( OBJECT *o, F_TYPE *f );
int x_dum( OBJECT *o, int num, F_TYPE *f );

F_TYPE forms[] = {
   { 0,  MAIN,   { 0, 1, 0, 0, 0, 0, 0 }, 0L,
         i_main, t_main, x_main },	     /* Main */
   { 0,  INFO1,  { 0, 1, 0, 0, 0, 1, 0 }, 0L,
         i_dum,  x_dum,  x_dum },	     /* About */
   { 0,  CONFL,  { 0, 1, 0, 0, 0, 1, 0 }, 0L,
         i_dum,  0L,  x_dum },	     	     /* Name Conflict */
   { 0,  WARNINGS,  { 0, 1, 0, 0, 0, 1, 0 }, 0L,
         i_dum,  x_dum,  x_dum } };          /* Warn */

Rect center, form_rect;
F_TYPE *curform;
GUI *gui;

int hide_if( OBJECT *form, int num, int truth )
{
  if( truth ) form[num].ob_flags &= ~HIDETREE;
  else form[num].ob_flags |= HIDETREE;
  return truth;
}

static char *old_title;

void form_y( F_TYPE *f, int sign )
{
  int i, h;
  OBJECT *o = f->tree;

  if( (o[1].ob_type>>8) != X_MOVER )
  {
    old_title = "";
    return;
  }
    h = o[2].ob_height*sign;  /* get title height */
    for( i=o[0].ob_head; i; i = o[i].ob_next )
      o[i].ob_y += h;
    o[0].ob_height += h;
    if( sign<0 ) old_title = o[2].ob_spec.free_string;
    else o[2].ob_spec.free_string = old_title;
    if( hide_if( o, 2, sign>0 ) ) o[0].ob_state |= X_PREFER;
    else o[0].ob_state &= ~X_PREFER;
    o[0].ob_spec.obspec.framesize = sign<0 ? 0 : -2;
    hide_if( o, 1, sign>0 );
}

void calc_bord( long type, OBJECT *tree, Rect *g )
{
  wind_calc( WC_BORDER, type, tree[0].ob_x, tree[0].ob_y,
      tree[0].ob_width, tree[0].ob_height, &g->x, &g->y, &g->w, &g->h );
  if( g->y < text_h )
  {
    tree[0].ob_y += text_h-g->y;
    g->y = text_h;
  }
}

void free_fname(void)
{
  if( fname )
  {
    (*neo_acc->mas->lfree)( fname );
    fname = 0L;
  }
}

void close_winds( int ac_close )
{
  free_fname();
  if( w_handle>0 )
  {
    form_y( curform, 1 );
    if( !ac_close )
    {
      wind_close(w_handle);
      wind_delete(w_handle);
    }
    w_handle = -1;
  }
}

void form_draw( F_TYPE *f, int num )
{
  int x, y, w, h;
  
  if( curform->flags.modal ) (*gui->xtern.objc_draw)( f->tree, num,
      8, form_rect.x, form_rect.y, form_rect.w, form_rect.h );
  else
  {
    wind_get( w_handle, WF_FIRSTXYWH, &x, &y, &w, &h );
    while( w && h )
    {
      (*gui->xtern.objc_draw)( f->tree, num, 8, x, y, w, h );
      wind_get( w_handle, WF_NEXTXYWH, &x, &y, &w, &h );
    }
  }
}

int _use_form( F_TYPE *f, int num )
{
  int but, ret=0, i;

  if( num==-1 )
  {
    if( f->exit ) (*f->exit)( f->tree, -1, f );
    close_winds(0);
    return 1;
  }
  but = num&0x7FFF;	    /* treat double-clicks as singles */
  if( f->tree[but].ob_flags & TOUCHEXIT )
  {
    if( f->touch ) ret = (*f->touch)( f->tree, but, f );
  }
  else if( f->tree[but].ob_flags&EXIT )
  {
    if( f->exit ) ret = (*f->exit)( f->tree, num, f );
    i = f->tree[but].ob_state & ~SELECTED;
    if( !ret )
      if( f->flags.modal ) (*gui->xtern.objc_change)( f->tree, but, 0, Xrect(form_rect), i, 1 );
      else (*gui->xtern.objc_change)( f->tree, but, 0, Xrect(center), i, 1 );
    else f->tree[but].ob_state = i;
  }
  if( ret && !f->flags.modal ) close_winds(0);
  return ret;
}

void use_form( int hand, int num )
{
  if( hand==w_handle ) _use_form( curform, num );
}

void blit_form( F_TYPE *f, int unblit )
{
  OBJECT *o = f->tree;
  
  if( !unblit )
  {
    delta = o[0].ob_x - form_rect.x;
    blit_ok = (*gui->xtern.form_dial)( X_FMD_START,  0, 0, 0, 0,  Xrect(form_rect) );
  }
  else
  {
    form_rect.x = o[0].ob_x - delta;
    form_rect.y = o[0].ob_y - delta;
    (*gui->xtern.form_dial)( blit_ok ? X_FMD_FINISH : FMD_FINISH,  0, 0, 0, 0,
	Xrect(form_rect) );
  }
}

void wind_lock( int lock )
{
  wind_update( lock ? BEG_UPDATE : END_UPDATE );
  wind_update( lock ? BEG_MCTRL : END_MCTRL );
}

int modal_formdo( F_TYPE *f )
{
  int ret;
  
  wind_lock(1);
  blit_form( f, 0 );
  hide_if( f->tree, 1, blit_ok );	/* mover */
  (*gui->xtern.objc_draw)( f->tree, 0, 8, Xrect(form_rect) );
  for(;;)
  {
    if( _use_form( f, ret=(*gui->xtern.form_do)( f->tree, 0 ) ) ) break;
  }
  blit_form( f, 1 );
  wind_lock(0);
  return ret;
}

void form_pos( F_TYPE *f )
{
  int x, y, delta;
  OBJECT *form = f->tree;

  x = form[0].ob_x;
  y = form[0].ob_y;
  (*gui->xtern.x_form_center)( form, &form_rect.x, &form_rect.y,
      &form_rect.w, &form_rect.h );
  delta = form[0].ob_x - form_rect.x;
  form_rect.x = (form[0].ob_x = x) - delta;
  form_rect.y = (form[0].ob_y = y) - delta;
  form[0].ob_x = form_rect.x + delta;
  form[0].ob_y = form_rect.y + delta;
}

int start_form( int type, F_TYPE *f )
{
  int ret = 0, dum, hand;
  
  if( f->flags.modal ) hand = 0;
  else if( (w_handle = hand =
      wind_create( type, Xrect(neo_acc->mas->most->maximum) )) < 0 )
  {
    f_alert( A22, 0L, 0 );
    return 0;
  }
  if( hand>0 )
  {
    form_y( curform=f, -1 );
    calc_bord( type, f->tree, &center ); /* fit a window around it */
    wind_calc( WC_WORK, type, Xrect(center),
        &f->tree[0].ob_x, &f->tree[0].ob_y, &dum, &dum );
  }
  form_pos(f);
  if( (*f->init)( f->tree, f ) )	 /* initialize the dialog */
    if( hand>0 )
    {
      wind_set( hand, WF_NAME, old_title );
      wind_open( hand, Xrect(center) );
      ret = -1;
    }
    else ret = modal_formdo( f );
  else close_winds(0);
  return ret;
}

int make_form( int num )
{
  F_TYPE *f;
  int i;
  Rect frect;
  
  f = forms+num;
  if( !f->tree )
  {
    rsrc_gaddr( 0, f->treenum, &f->tree );
    f->treenum = -1;
    (*gui->xtern.x_form_center)( f->tree, &frect.x, &frect.y, &frect.w, &frect.h );
  }
/*%  if( num==2 ) f->old_title = "";*/
  i = start_form( NAME|MOVER|CLOSER, f );
  return i;
}

void cdecl spf( char *buf, char *fmt, ... )     /* call NeoMaster's sprintf() */
                        /* ANSI C compilers probably won't like this one! */
{
  (*neo_acc->mas->dopf)( buf, fmt, &... );
}

void ob_stat( OBJECT *o, int num, int true, int bit )
{
  o += num;
  if( true ) o->ob_state |= bit;
  else o->ob_state &= ~bit;
}

void enab_if( OBJECT *o, int num, int true )
{
  ob_stat( o, num, !true, DISABLED );
}

void sel_if( OBJECT *o, int num, int true )
{
  ob_stat( o, num, true, SELECTED );
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
/* -------------------------------------------------------------------- */
/*    min()                                                             */
/*                                                                      */
/*    Minimum zweier Zahlen berechnen.                                  */
/* -------------------------------------------------------------------- */

int min( int a, int b)
{
   if( a > b )
      return( b );
   else
      return( a );
}

/* -------------------------------------------------------------------- */
/*    max()                                                             */
/*                                                                      */
/*    Maximum zweier Zahlen bestimmen.                                  */
/* -------------------------------------------------------------------- */

int _max( int a, int b)
{
   if( a < b )
      return( b );
   else
      return( a );
}

/* -------------------------------------------------------------------- */
/*    rc_intersect()                                                    */
/*                                                                      */
/*    Schnittfl„che zweier Rechtecke berechnen.                         */
/* -------------------------------------------------------------------- */

int rc_intersect(Rect *r1, Rect *r2)
{
   int xl, yu, xr, yd;                      /* left, upper, right, down */

   xl      = _max( r1->x, r2->x );
   yu      = _max( r1->y, r2->y );
   xr      = min( r1->x + r1->w, r2->x + r2->w );
   yd      = min( r1->y + r1->h, r2->y + r2->h );

   r2->x = xl;
   r2->y = yu;
   r2->w = xr - xl;
   r2->h = yd - yu;

   return( r2->w > 0 && r2->h > 0 );
}

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

void spf_date( char *ptr, int a, int b, int c )
{
  char sep, *str;
  
  if( c>99 ) c -= 100;
  if( (sep = (char)idt_fmt) == 0 ) sep = '/';
  str = "%02d%c%02d%c%02d";
  switch( (int)idt_fmt&0xf00 )
  {
    case 0x000:
      spf( ptr, str, a, sep, b, sep, c );
      break;
    case 0x100:
      spf( ptr, str, b, sep, a, sep, c );
      break;
    default:
      spf( ptr, str, c, sep, a, sep, b );
      break;
    case 0x300:
      spf( ptr, str, c, sep, b, sep, a );
  }
}

void to_date( char *buf, int date )
{
  register int yr;

  if( (yr = ((date>>9) & 0x7F) + 80) > 99 ) yr -= 100;
  spf_date( buf, (date>>5) & 0xf, date&0x1f, yr );
}

void to_time( char *buf, unsigned int time )
{
  spf( buf, "%02d:%02d:%02d", time>>11 & 0x1f, time>>5 & 0x3f, time&0x1f );
}

void from_filename( char *src, char *dest )
{
  register int i=0;
  
  while( i<11 )
  {
    if( *src != ' ' ) *dest++ = *src;
    if( ++i == 8 ) *dest++ = '.';
    src++;
  }
  *dest = '\0';
}

int f_alert( int num, char *s, int mode )
{
  char **ptr, buf[200], buf2[13], *ptr3;
  
  rsrc_gaddr( 15, num, &ptr );
  if( s )
  {
    if( !mode ) from_filename( s, ptr3=buf2 );
    else ptr3 = rindex( s, '\\' ) + 1;
    spf( buf, *ptr, ptr3 );
    return( form_alert( 1, buf ) );
  }
  return( form_alert( 1, *ptr ) );
}

void mediach()
{
/*  int h, buf[8];*/
  
  *(neo_acc->mas->bad_media) = drive;
  Fsfirst( root, 0x37 );
  update_list[drive] = 1;
/*  buf[0] = SH_WDRAW;  003
  buf[1] = apid;
  buf[2] = 0;
  buf[3] = drive;
  if( wdraw_ok ) shel_write( 7, 0, 0, (char *)buf, "\0" );
  else if( (h = appl_find( "NEODESK " )) >= 0 )
      appl_write( h, 16, buf ); */
}

void pause()
{
  int x;
  
  do
    graf_mkstate( &dum, &dum, &x, &dum );
  while( x&1 );
}

int check_bounds( int x, int y )
{
  return( y>=0 && y<10*text_h && x>=0 && x<30*8 && y<(items-window)*text_h );
}

void update()
{
  static char upd[]="x:\\";
  
  *upd = drive+'A';
  if( neo_acc->nac_ver >= 0x300 ) (*neo_acc->update_drive)( upd );
/*%  else (*(int (*)())((long)(neo_acc->trash_files)+0x55A))( upd );*/
}

void set_buttons( OBJECT *o, F_TYPE *f )
{
  register int i;
  
  i = num_sel ? 0 : DISABLED;
  if( i != (o[DELPERM].ob_state&DISABLED) )
  {
    o[DELPERM].ob_state = (o[DELPERM].ob_state&~DISABLED) | i;
    o[UNDEL].ob_state = (o[UNDEL].ob_state&~DISABLED) | i;
    form_draw( f, DELPERM );
    form_draw( f, UNDEL );
  }
}

void slider( OBJECT *o, F_TYPE *f, int flg )
{
  register int i;

  if( flg<=0 )
  {
    if( (i = items-10) < 1 ) i=1;
    o[SMLSL].ob_y = (long)(o[BIGSL].ob_height - 
        o[SMLSL].ob_height) * window / i;
    form_draw( f, BIGSL );
  }
  if( flg )
  {
    if( (i = longest-30) < 1 ) i=1;
    o[SMLSLH].ob_x = (long)(o[BIGSLH].ob_width - 
        o[SMLSLH].ob_width) * xoff / i;
    form_draw( f, BIGSLH );
  }
}

void write_rsc( OBJECT *o )
{
  RSHDR rsh;
  int hand, i;
  long l;
  
  Dsetdrv(dflt_drive);
  Dsetpath( "\\" );
  if( (hand = Fopen(rsc_name,2)) > 0 )
  {
    if( f_alert( A18, 0L, 0 ) == 1 )
    {
      Fread( hand, (long)sizeof(rsh), &rsh );
      Fseek( (long)rsh.rsh_trindex, hand, 0 );
      Fread( hand, 4L, &l );
      Fseek( l+CONFON*sizeof(OBJECT)+10, hand, 0 );
      i = o[CONFON].ob_state;
      Fwrite( hand, 2L, &i );
/*%      Fseek( (long)sizeof(OBJECT)-2, hand, 1 );
      i = !i;
      Fwrite( hand, 2L, &i ); */
    }
    Fclose(hand);
  }
}

int find_link( int num )
{
  register int i;
  
  i = first;
  while( i>=0 && num-- ) i = fname[i].link;
  return(i);
}

void display( OBJECT *o, F_TYPE *ft, int mode, int firsty, int lasty )
{
  register int i, j;
  register char *ptr, *txt;
  char buf[31], ok;
  OBJECT *f, *d;

  idt_fmt = neo_acc->mas->most->idt_fmt;  
  buf[30] = '\0';
  if( firsty<0 )
  {
    firsty = 0;
    lasty = 9;
  }
  f = o+NAME0+firsty;
  d = o+DATE0+firsty;
  for( ok=1, i=firsty, j=find_link(i+window); i<=lasty; i++, f++, d++ )
  {
    if( i>=items-window ) ok = 0;
    if( mode <= 0 )
    {
      txt = f->ob_spec.tedinfo->te_ptext;
      if( !ok ) *txt = 0;
      else if( xoff < strlen(ptr=&fname[j].dir) )
          strncpy( txt, ptr+xoff, 30 );
      else *txt = 0;
      if( ok && fname[j].hidden ) d->ob_state = f->ob_state |= DISABLED;
      else d->ob_state = f->ob_state &= ~DISABLED;
      if( ok && fname[j].selected ) d->ob_state = f->ob_state |= SELECTED;
      else d->ob_state = f->ob_state &= ~SELECTED;
      form_draw( ft, NAME0+i );
    }
    if( mode )
    {
      txt = d->ob_spec.tedinfo->te_ptext;
      if( !ok ) strcpy( txt, "        " );
      else
      {
        switch( dts )
        {
          case 0:
            to_date( buf, fname[j].date );
            break;
          case 1:
            to_time( buf, fname[j].time );
            break;
          case 2:
            spf( buf, "%8D", fname[j].len );
            break;
        }
        buf[8] = '\0';
        strncpy( txt, buf, 8 );
      }
      form_draw( ft, DATE0+i );
    }
    if(ok) j = fname[j].link;
  }
}

void move( OBJECT *o, F_TYPE *f, int num, int dir )
{
  Rect box1, box2, textr;
  register int n, i, ow, oh;
  char no_blit;
  
  wind_update( BEG_UPDATE );
  objc_offset( o, NAME0, &textr.x, &textr.y );
  textr.w = ow = 39*8 - 1;
  textr.h = oh = 10*text_h - 1;
  rc_intersect( &max, &textr );
  box1 = box2 = textr;
  box1.h = box2.h++;
  no_blit = textr.w != ow || textr.h != oh;
  if( !dir )
  {
    if( num+window > items-10 ) num = items-10-window;
    if( num+window < 0 ) num = -window;
    if( num )
    {
      window += num;
      if( (n=abs(num)) >= 10 || no_blit ) display( o, f, -1, -1, 0 );
      else
      {
        i = n*text_h;
        box1.h = box2.h -= i;
        box1.y += i;
        if( num<0 )
        {
          (*neo_acc->blit)( &box2, &box1, 0, 3, 0L );
          display( o, f, -1, 0, n-1 );
        }
        else
        {
          (*neo_acc->blit)( &box1, &box2, 0, 3, 0L );
          display( o, f, -1, 10-n, 9 );
        }
      }
      slider( o, f, 0 );
    }
  }
  else
  {
    if( num+xoff > longest-30 ) num = longest-30-xoff;
    if( num+xoff < 0 ) num = -xoff;
    if( num )
    {
      xoff += num;
      if( (n=abs(num)) >= 30 || no_blit ) display( o, f, 0, -1, 0 );
      else
      {
        i = n<<3;
        box1.w = box2.w = 30*8-i-1;
        box1.x += i;
        if( num<0 )
        {
          (*neo_acc->blit)( &box2, &box1, 0, 3, 0L );
          display( o, f, 0, -1, 0 );
        }
        else
        {
          (*neo_acc->blit)( &box1, &box2, 0, 3, 0L );
          display( o, f, 0, -1, 0 );
        }
      }
      slider( o, f, 1 );
    }
  }
  wind_update( END_UPDATE );
}

void unsel( OBJECT *o, F_TYPE *f )
{
  register int j;
  
  if( num_sel && !(Kbshift(-1)&3) )
  {
    j = first;
    while( j>=0 )
    {
      fname[j].selected = 0;
      j = fname[j].link;
    }
    display( o, f, -1, -1, 0 );
    num_sel = 0;
  }
}

int check_ver( int hand )
{
  int ver2;
  
  Fread( hand, 2L, &ver2 );
  if( ver2 != VERSION )
  {
    f_alert( A1, 0L, 0 );
    return(0);
  }
  Fread( hand, 4L, &table_items );
  return(1);
}

void sort()
{
  register int i, j, prev_max, max, new_items;
  register Fname *fn;
  char buf[120];
  
  Dsetdrv(drive);
  new_items=0;
  prev_max = -3;
  while( new_items<items )
    for( i=0; i<items; i++ )
      if( fname[i].link == -1 )
      {
        max = i;
        for( j=0; j<items; j++ )
          if( j != i )
          {  
            fn = &fname[j];
/*          if( !(k=pthcmp(fn->path,fname[max].path)) )
                if( Dsetpath(fname[max].path) ) fn->hidden=1;*/
            if( fn->link == -1 && strcmp( fn->path, fname[max].path ) > 0 )
                max = j;
          }
        fn = &fname[max];
        strcpy( buf, fn->path );
        *(rindex(buf,'\\')+1) = '\0';
        if( Dsetpath(buf) ) fn->hidden = 1;
        fn->link = prev_max;
        prev_max = max;
        new_items++;
      }
  first = prev_max;
}

void read_data( OBJECT *o, F_TYPE *f )
{
  register unsigned long l;
  register int hand, i, j;
  Dirent dir;
  
  *data_name = drive + 'A';
  items = window = num_sel = longest = xoff = 0;
  graf_mouse( BUSY_BEE, 0L );
  free_fname();
  if( (hand = Fopen( data_name, 0 )) > 0 )
  {
    if( check_ver( hand ) )
    {
      if( (fname = (Fname *)((*neo_acc->mas->lalloc)(
          (items=table_items)*sizeof(Fname), -42, 1 ))) == 0L )
      {
        f_alert( MOREITMS, 0L, 0 );
        items = 0;
      }
      for( i=0; i<items; i++ )
      {
        fname[i].selected = 0;
        fname[i].link = -1;
        fname[i].space = ' ';
        fname[i].hidden = 0;
        Fread( hand, 118L, fname[i].path );
        Fread( hand, (long)sizeof(dir), &dir );
        Fread( hand, 2L, &dum );
        if( !fname[i].path[0] ) i--;
        else
        {
          fname[i].dir = dir.att & S_IJDIR ? '\7' : ' ';
          fname[i].len = ((unsigned)canw((int)dir.len)<<16L) | 
              (unsigned)canw((int)(dir.len>>16));
          fname[i].time = canw(dir.time);
          fname[i].date = canw(dir.date);
          if( (j=strlen(fname[i].path)+2) > longest ) longest = j;
        }
      }
    }
    Fclose( hand );
  }
  sort();
  graf_mouse( ARROW, 0L );
  i = o[BIGSL].ob_height;
  if( items > 10 )
    if( (i = i * 10 / items) < 10 ) i = 10;
  o[SMLSL].ob_height = i;
  i = o[BIGSLH].ob_width;
  if( longest > 30 )
    if( (i = i * 30 / longest) < 10 ) i = 10;
  o[SMLSLH].ob_width = i;
  slider( o, f, -1 );
  set_buttons( o, f );
  display( o, f, -1, -1, 0 );
}

void ack()                                   /* acknowledge NeoDesk's message */
{
  int buf[8];
  
  buf[0] = DUM_MSG;                     /* always ack with this message */
  appl_write( buf[1]=neo_apid, 16, buf );
}

int test_neo(void)
{
  if( !neo_acc || (long)neo_acc & 1 || neo_acc->nac_ver < 0x0400 )
  {
    f_alert( BADNEO, 0L, 0 );
    neo_acc = 0L;
    return 0;
  }
  if( neo_apid<0 ) neo_apid = appl_find("NEODESK ");
  gui = neo_acc->mas->most->gui;
  return 1;
}

void find_neo( int *buf )
{
  /* throw away if too many tries */
  if( ++retries>10 )
  {
    f_alert( BADNEO, 0L, 0 );
    return;
  }
  /* try to find NeoDesk */
  if( (neo_apid = appl_find("NEODESK ")) >= 0 )
  {
    /* put the message back in my queue */
    appl_write( apid, 16, buf );
    buf[0] = NEO_ACC_ASK;         /* 89 */
    buf[1] = apid;
    buf[3] = NEO_ACC_MAGIC;
    buf[4] = apid;
    appl_write( neo_apid, 16, buf );
  }
  else f_alert( BADNEO, 0L, 0 );
}

int find( char *name, unsigned int *sec, unsigned int *drv, Dirent **de, int flag )
{
  register BPB *bp;
  char *ptr;
  register unsigned int cl, max_cl;
  char buf[12], found, last, *ptr2;
  unsigned int cluster, dir_sec;
  
  buf[11] = '\0';
  if( (bp0=bp=Getbpb( *drv = (unsigned int)(*name-'A') )) == 0 )
  {
    f_alert( A2, 0L, 0 );
    return(-1);
  }
  if( bp->recsiz > SEC_SIZE )
  {
    f_alert( A3, 0L, 0 );
    return(-1);
  }
  ptr = name+3;
  ents_sec = ((bp->recsiz+1)<<3) / (bp->bflags&1?16:12);
  *sec = bp->fatrec + bp->fsiz;
  last = cluster = 0;
  max_cl = bp->rdlen;
  fat_sec=0;
  while( !last )
  {
    to_dirname( ptr, buf );
    if( (ptr2 = index(ptr,(int)'\\')) != 0 ) ptr = ptr2+1;
    else last++;
    dir_sec = *sec;
again:
    found=0;
    while( !found )
    {
      for( cl=0; cl<max_cl && !found; cl++ )
      {
        if( Rwabs( 2, sec_buf, 1, *sec, *drv ) < 0 )
        {
          f_alert( A4, 0L, 0 );
          return(-1);
        }
        *de = (Dirent *) sec_buf;
        while( (char *)*de-sec_buf < bp->recsiz )
          if( !strncmp( buf, (*de)->name, 11 ) )
          {
            found++;
            break;
          }
          else if( last && flag>0 && ((*de)->name[0]==(unsigned char)'\xE5' || 
              !(*de)->name[0])) return(1);
          else if( !(*de)->name[0] ) goto nfnd;
          else (*de)++;
        if( !found ) (*sec)++;
      }
      if( !found&&cluster )
      {
        if( read_fat( bp, &cluster, *drv ) < 0 ) return(-1);
        if( cluster==0xffff ) goto nfnd;            /* end of file */
        *sec = (cluster-2) * bp->clsiz + bp->datrec;
      }
    }
nfnd:
    if( found )
    {
      if( !last )
        if( (*de)->att & S_IJDIR )
        {
          *sec = ( (cluster=canw((*de)->cluster))-2) * 
              bp->clsiz + bp->datrec;
          max_cl = bp->clsiz;
        }
        else
        {
          f_alert( A5, buf, 0 );
          return(-1);
        }
      else return(1);
    }
    else if( !last )
    {
      if( flag>=0 ) f_alert( A6, buf, 0 );
      else if( flag==-2 ) return(-3);
      else if( (cl=f_alert( A23, buf, 0 )) == 1 )
      {
        strcpy( sec_buf, name );
        *(ptr2-name+sec_buf) = '\0';
        if( Dcreate( sec_buf ) < 0 ) f_alert( A10, 0L, 0 );
        else
        {
          *sec = dir_sec;
          goto again;
        }
      }
      else if( cl==2 ) return(-2);
      return(-1);
    }
  }
  return(0);
}

int read_fat( BPB *bp, unsigned int *cluster, int drv )
{
  register unsigned int ncl;
  int f;
  union
  {
    unsigned char c[2];
    unsigned int i;
  } u;
  
  if( (f = *cluster/ents_sec + bp->fatrec) != fat_sec )
    if( Rwabs( 2, fat, 2, f, drv ) < 0 )
    {
      f_alert( A4, 0L, 0 );
      return(-1);
    }
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

int find_table( int hand, char *name, int ignore )
{
  Tablent tbl;
  unsigned long pos=0L;
  int i=table_items;

  while( i && Fread( hand, (long)sizeof(tbl), &tbl ) == sizeof(tbl) )
    if( !tbl.path[0] )
    {
      if( !ignore )
      {
        pos = Fseek( 0L, hand, 1 ) - sizeof(tbl);
        break;
      }
    }
    else
    {
      i--;
      if( !strcmp( name+2, tbl.path ) ) return(1);
    }
  if( pos ) Fseek( pos, hand, 0 );
  if( i>0 && ignore )
  {
    f_alert( A16, 0L, 0 );
    table_items -= i;
  }
  return(0);
}

int delete( char *name )
{
  unsigned int sec, drv;
  int i;
  Dirent *de;
  register int hand;
  register char *ptr;
  
  ptr = rindex( name, '\\' );
  if( !strcmp( name, PASSED_TRASH ) || !strcmp( name, PASSED_PRN ) ||
      !strncmp( name, PASSED_CLIP, sizeof(PASSED_CLIP)-1 ) ||
      strlen( name ) < 4 || !ptr ) return(0);
  if( !asked )
  {
    asked=1;
    if( conf_on )
      if( f_alert( DELTEMP, 0L, 0 ) != 1 ) return(-1);
  }
  graf_mouse( BUSY_BEE, 0L );
  drive = (*data_name = *root = *name) - 'A';
  if( !check_folders() ) return(-1);
  if( !*(ptr+1) ) *ptr = '\0';
  if( (i = find( name, &sec, &drv, &de, 0 )) == 0 )
  {
    f_alert( A7, 0L, 0 );
    return(-1);
  }
  else if( i<0 ) return(-1);
  if( (hand = Fopen(data_name,2)) < 0 )
  {
    if( (hand = Fcreate( data_name, S_IJHID )) < 0 )
    {
      f_alert( A8, 0L, 0 );
      return(-1);
    }
    else
    {
      if( writ( hand, 2L, (char *)&ver ) < 0 )
      {
retm1:  Fclose(hand);
        Fdelete(data_name);
        return(-1);
      }
      table_items=0;
      if( writ( hand, 4L, (char *)&table_items ) < 0 ) goto retm1;
    }
  }
  else if( !check_ver( hand ) )
  {
    Fclose(hand);
    return(-1);
  }
  if( find_table( hand, name, 0 ) )
  {
    f_alert( A9, 0L, 0 );
    return(-1);
  }
  if( writ( hand, 118L, name+2 ) < 0 ) return(-1);
  if( writ( hand, (long)sizeof(Dirent), (char *)de ) < 0 ) return(-1);
  i = canw(de->cluster);
  if( read_fat( bp0, (unsigned int *)&i, drv ) < 0 ) return(-1);
  if( writ( hand, 2L, (char *)&i ) < 0 ) return(-1);
  table_items++;
  Fseek( 2L, hand, 0 );
  if( writ( hand, 4L, (char *)&table_items ) < 0 ) return(-1);
  Fclose(hand);           /* must close before changing name */
  if( Rwabs( 2, sec_buf, 1, sec, drv ) < 0 )
  {
    f_alert( A4, 0L, 0 );
    return(-1);
  }
  de->name[0] = '\xE5';
  if( Rwabs( 1, sec_buf, 1, sec, drv ) < 0 )
  {
    f_alert( A10, 0L, 0 );
    return(-1);
  }
  mediach();
  return(0);
}

int writ( int hand, long n, char *buf )
{
  if( Fwrite( hand, n, buf ) != n )
  {
    f_alert( A11, 0L, 0 );
    Fclose(hand);
    return(-1);
  }
  return(0);
}

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

int undelete( char *name, int delperm )
{
  unsigned int sec, drv, cmp;
  register int i;
  register char *ptr, *nam;
  long pos;
  Dirent *de;
  register int hand;
  Tablent tbl;
  OBJECT *obj;
  
  *data_name = *root = *name;
  if( !check_folders() ) return(-1);
  if( (hand = Fopen(data_name,2)) < 0 )
  {
    f_alert( A12, 0L, 0 );
    return(-1);
  }
  if( !check_ver( hand ) )
  {
    Fclose(hand);
    return(-1);
  }
  if( !find_table( hand, name, 1 ) )
  {
    f_alert( A13, 0L, 0 );
    return(-1);
  }
  pos = Fseek( -(long)sizeof(tbl), hand, 1 );
  Fread( hand, (long)sizeof(tbl), &tbl );
  Fclose( hand );
  if( delperm && tbl.dir.att & S_IJRON )
    switch( f_alert( A24, tbl.dir.name, 0 ) )
    {
      case 1:
        tbl.dir.att &= ~S_IJRON;
        break;
      case 2:
        return(0);
      case 3:
        return(-1);
    }
  mediach();
  rsrc_gaddr( 0, CONFL, &obj );
  to_filename( rindex(name,'\\')+1, obj[CONFORIG].ob_spec.tedinfo->te_ptext );
  *fake = *name;
  *(fake+3) = '0';
again:
  if( (i=find( name, &sec, &drv, &de, delperm ? -2 : -1 )) > 0 )
  {
incr:
    nam = tbl.dir.name;
    if( delperm )
      if( (*((name=fake)+3))++ > 0 )
      {
        ptr = name+3;
        for( i=0; i<11; i++ )
        {
          if( *ptr=='.' && i==8 ) ptr++;
          if( *ptr && *ptr != '.' ) nam[i] = *ptr++;
          else nam[i] = ' ';
        }
        goto again;
      }
      else
      {
        f_alert( A10, 0L, 0 );
        return(-1);
      }
    for( ptr=obj[CONFNEW].ob_spec.tedinfo->te_ptext, i=0; i<11; i++ )
      if( i<8 || nam[i] != ' ' ) *ptr++ = nam[i];
    *ptr='\0';
    graf_mouse( ARROW, 0L );
    if( (i = make_form(2)) == NEWNAME &&
        *(ptr=obj[CONFNEW].ob_spec.tedinfo->te_ptext) )
    {
      from_filename( ptr, rindex(name,'\\')+1 );
      for( i=0; i<11; i++ )
        if( *ptr ) nam[i] = *ptr++;
        else nam[i] = ' ';
      graf_mouse( BUSY_BEE, 0L );
      goto again;
    }
    else if( i==CONFSKIP ) return(0);
    return(-1);
  }
  else if( i==-2 ) return(0);
  else if( i==-3 ) goto incr;
  else if( i<0 ) return(-1);
  if( (i=find( name, &sec, &drv, &de, 1 )) == 0 )
  {
    strcpy( sec_buf, name );
    *(rindex(sec_buf,'\\')+1) = '\345';
    if( (hand=Fcreate(sec_buf,0)) < 0 )
    {
      f_alert( A10, 0L, 0 );
      return(-1);
    }
    else
    {
      Fclose( hand );
      if( !find( name, &sec, &drv, &de, 1 ) )
      {
        f_alert( A10, 0L, 0 );
        return(-1);
      }
    }
  }
  else if( i<0 ) return(-1);
  cmp = canw( tbl.dir.cluster );
  if( read_fat( bp0, &cmp, drv ) < 0 ) return(-1);
  if( cmp != tbl.next_cl )
  {
    if( f_alert( A15, 0L, 0 ) == 1 ) goto remove;
    return(-1);
  }
  memcpy( de, &tbl.dir, sizeof(Dirent) );
  if( Rwabs( 1, sec_buf, 1, sec, drv ) < 0 )
  {
    f_alert( A10, 0L, 0 );
    return(-1);
  }
  mediach();
remove:
  if( !--table_items ) Fdelete( data_name );
  else
  {
    if( (hand = Fopen(data_name,1)) < 0 )
    {
      f_alert( A12, 0L, 0 );
      return(-1);
    }
    Fseek( pos, hand, 0 );
    drv=0;
    Fwrite( hand, 1L, &drv );
    Fseek( 2L, hand, 0 );
    Fwrite( hand, 4L, &table_items );
    Fclose(hand);
  }
  if( delperm )
  {
    if( Fattrib(name,0,0) & S_IJDIR )
    {
      i = *(neo_acc->status_on);
      *(neo_acc->status_on) = 0;
      hand = (*neo_acc->trash_files)( name, &dum, &dum ) ? -1 : 0;
      *(neo_acc->status_on) = i;
      return(hand);
    }
    if( Fdelete(name) < 0 )
    {
      f_alert( A17, name, 1 );
      return(-1);
    }
  }
  return(0);
}

int add_val( int *valptr, int i, DND *dnd, int flag )
{
  register int *v;
  
  if( ++vals > 98 ) return(1);
  if( valptr < (int *)&sec_buf[SEC_SIZE*3] )
  {
    if( !flag )
    {
      v = (int *)sec_buf;
      while( v<valptr )
        if( *(DND **)(v+1) == dnd ) return(1);
        else v+=3;
    }
    *valptr++ = i;
    *(DND **)valptr = dnd;
    return(0);
  }
  else return(2);
}

int check_folders()
{
  register int *valptr, *val2=0L;
  register DND *dnd;
  register int i;
  DND *dnd2;
  long stack;
  int err=0;
  
  vals = 0;
  stack = Super(0L);
  if( (i=OS_version) <= 0x100 ) dnd = dnd_10(drive);
  else if( i<=0x102 ) dnd = dnd_12(drive);
  if( i>0x102 || !dnd )
  {
    Super((void *)stack);
    return(1);
  }
  valptr = (int *)sec_buf;
  i=0;
  do
  {
    for( ; i<2 && !err; i++ )
      if( (dnd2 = i?dnd->d_right : dnd->d_left) != 0 )
        if( (err=add_val( valptr, i, dnd, val2 && dnd==*(DND **)(val2+1) )) == 0 ) 
        {
          i = -1;
          valptr+=3;
          dnd = dnd2;
        }
    if( val2 ) *val2 = 2;
    val2 = (int *)sec_buf;
    while( val2 < valptr )
      if( (i = *val2 + 1) < 2 )
      {
        dnd = *(DND **)(val2+1);
        break;
      }
      else val2 += 3;
  }
  while( val2 < valptr && !err );
  Super((void *)stack);
  if( err ) f_alert( err==1 ? A20 : A19, 0L, 0 );
  else mediach();
  return(!err);
}

void do_quit(void)
{
  rsrc_free();
  appl_exit();
  exit(0);
}

#pragma warn -par
int i_main( OBJECT *o, F_TYPE *f )
{
  unsigned long bits;
  int i;

  bits = Drvmap();
  if( !(bits&(1<<drive)) ) drive = 0;
  for( i=0; i<26; i++ )
  {
    sel_if( o, DRIVEA+i, i==drive );
    enab_if( o, DRIVEA+i, bits&(1<<i) );
    if( text_h<16 ) o[DRIVEA+i].ob_state &= ~(X_MAGIC|X_PREFER|X_DRAW3D);	/* 003 */
  }
  read_data( o, f );
  return 1;
}
int t_main( OBJECT *o, int num, F_TYPE *f )
{
  int x, y, y2;
  int i, j, state, text_x, text_y;

      switch(num)
      {
        case UP:
        case LEFT:
          move( o, f, -1, num==LEFT );
          break;
        case DOWN:
        case RIGHT:
          move( o, f, 1, num==RIGHT );
          break;
        case BIGSL:
          graf_mkstate( &dum, &y, &dum, &dum );
          objc_offset( o, SMLSL, &dum, &y2 );
          move( o, f, y<=y2 ? -10 : 10, 0 );
          break;
        case BIGSLH:
          graf_mkstate( &x, &dum, &dum, &dum );
          objc_offset( o, SMLSLH, &y2, &dum );
          move( o, f, x<=y2 ? -30 : 30, 1 );
          break;
        case SMLSL:
          if( (i=items-9) < 0 ) i = 0;
          move( o, f, (int)((long)i * 
              graf_slidebox( o, BIGSL, SMLSL, 1 ) / 1000L) - window, 0 );
          break;
        case SMLSLH:
          if( (i=longest-29) < 0 ) i = 0;
          move( o, f, (int)((long)i * graf_slidebox( o, BIGSLH, SMLSLH, 0 ) 
              / 1000L) - xoff, 1 );
          break;
        case SELALL:
          i = first;
          j = num_sel!=items;
          while( i>=0 )
          {
            fname[i].selected = j;
            i = fname[i].link;
          }
          num_sel = j ? items : 0;
          set_buttons( o, f );
          display( o, f, -1, -1, 0 );
          evnt_button( 1, 1, 0, &dum, &dum, &dum, &dum );
          break;
        case NAMEBOX2:
        case NAMEBOX:
            unsel( o, f );
/*******    if( (*neo_acc->rubber_box)( x+text_x, y+text_y, &box, 0 ) )
            {
              j = find_link(window);
              for( i=0, y=text_y; i<10 && j>=0; i++, j=fname[j].link )
              {
                box2.x = text_x;
                box2.y = y;
                box2.w = 30*8;
                box2.h = text_h;
                if( rc_intersect( &box, &box2 ) )
                  if( !fname[j].selected ** && !fname[j].hidden ** )
                  {
                    fname[j].selected = 1;
                    num_sel++;
                    display( o, f, 0, i, i );
                  }
                y += text_h;
              }
            }  ***********/
            set_buttons( o, f );
          break;
        case DTSD:
        case DTST:
        case DTSS:
          if( num-DTSD != dts )
          {
            dts = num - DTSD;
            display( o, f, 1, -1, 0 );
          }
          pause();
          break;
        default:
          i = -1;
          if( num>=NAME0 && num<NAME0+10 ) i = num-NAME0;
          else if( num>=DATE0 && num<DATE0+10 ) i = num-DATE0;
          if( i>=0 )
          {
            objc_offset( o, NAME0, &text_x, &text_y );
            unsel( o, f );
            if( i+window<items )
            {
            state = fname[find_link(i+window)].selected ^ 1;
            do
            {
              j=find_link(i+window);
              if( /*   !fname[j].hidden && */
                  fname[j].selected != state )
              {
                if( (fname[j].selected = (char)state) != 0 ) num_sel++;
                else num_sel--;
                display( o, f, -1, i, i );
              }
              do
              {
                graf_mkstate( &x, &y, &y2, &dum );
                j = (y-=text_y)/text_h;
              }
              while( y2&1 && (j==i || !check_bounds( x-=text_x, y )) );
              i = y/text_h;
            }
            while( y2&1 );
            }
            set_buttons( o, f );
          }
          else
          {
            drive = num - DRIVEA;
            pause();
            read_data( o, f );
          }
      }
  return 0;
}
int x_main( OBJECT *o, int num, F_TYPE *f )
{
  int ret=0, i, j, x;
  char buf[120] = "x:";
  
  switch(num)
  {
        case -1:
        case QUIT:
          ret = 1;
          break;
        case WARN:
          make_form(3);
          break;
        case GETINFO:
          make_form(1);
          break;
        case DELPERM:
          if( f_alert( DELPER, 0L, 0 ) != 1 ) break;
        case UNDEL:
          graf_mouse( BUSY_BEE, 0L );         /* read_data does the arrow */
          buf[0] = drive + 'A';
          j = first;
          x = 0;
          while( j>=0 && x>=0 )
          {
            if( fname[j].selected )
            {
              strcpy( buf+2, fname[j].path );
              o[FILESTR].ob_flags &= ~HIDETREE;
              o[FILESTR].ob_spec.tedinfo->te_ptext = 
                   rindex(buf,'\\')+1;
              form_draw( f, FILESTR );
              x = undelete( buf, num==DELPERM );
            }
            j = fname[j].link;
          }
          o[FILESTR].ob_flags |= HIDETREE;
          form_draw( f, FILESTR-1 );
/*          update(); 003 */
          read_data( o, f );
          break;
  }
  if( ret )
  {
    if( (i=o[CONFON].ob_state & SELECTED) != conf_on )
    {
      conf_on = i;
      write_rsc(o);
    }
    free_fname();
  }
  return ret;
}
int i_dum( OBJECT *o, F_TYPE *f )
{
  return 1;
}
int x_dum( OBJECT *o, int num, F_TYPE *f )
{
  return 1;
}
#pragma warn +par

char auto_update( int mode )
{
  int i;
  static int old_drive;
  char up;
  
  if( !mode )
  {
    old_drive = drive;
    for( i=0; i<16; i++ )
      update_list[i] = 0;
    return 0;
  }
  else
  {
    up = 0;
    for( i=0; i<16; i++ )
      if( update_list[i] )
      {
        if( i==old_drive ) up=1;
        drive = i;
        update();
      }
    drive = old_drive;
    return up;
  }
}

EMULTI emulti = { 0, 1, 1, 1,  0, 0, 0, 0, 0,  1, 0, 0, 0, 0  };

main( int argc, char *argv[] )
{
  int ignore=0, i, buffer[8], x_max, dum, edit_obj, next_obj;
  char *ptr, **ptr2, aes_40, inside=0, cont, discard=0;                   /* pointer modified by list_files() */
  extern int _app;
  DTA dta;
  char temp[120];
  OBJECT *o;

  drive = dflt_drive = Dgetdrv();
  apid = appl_init();
  aes_40 = _GemParBlk.global[0] >= 0x400;
  if( aes_40 )
  {
    appl_getinfo( 10, buffer, buffer+1, buffer+1, buffer+1 );
    wdraw_ok = buffer[0] >= 7;
  }
  graf_handle( &dum, &text_h, &dum, &dum );
  if( !rsrc_load( rsc_name ) )
  {
    form_alert( 1, "[1][|Could not find TRASHCAN.RSC!][Bleh]" );
    if( aes_40 || _app )
    {
      appl_exit();
      return(0);
    }
    ignore++;
    goto loop;
  }
  if( _app )
  {
    if( argc>=2 )
    {
      neo_acc = (NEO_ACC *)atol(argv[1]);
      neo_apid = -1;
    }
    if( !test_neo() ) do_quit();
    graphics = neo_acc->mas->most->graphics;
    if( argc>=3 )
    {
      Fsetdta(&dta);
      temp[0] = dflt_drive+'A';
      temp[1] = ':';
      Dgetpath( temp+2, 0 );
      strcat( temp, "\\" );
      graf_mouse( BUSY_BEE, 0L );
      auto_update(0);
      for( i=2; i<argc; i++ )
      {
        if( (ptr=index(argv[i],'\\')) == 0 ) strcpy(
            rindex(ptr=temp,'\\')+1, argv[i] );
        else ptr = argv[i];
        if( delete( ptr ) < 0 ) break;
      }
      auto_update(1);
      graf_mouse( ARROW, 0L );
      do_quit();
    }
  }
  if( aes_40 ) shel_write( 9, 1, 0, 0L, 0L );
  wind_get( 0, WF_WORKXYWH, &max.x, &max.y, &max.w, &max.h );
  if( !ignore )
  {
    rsrc_gaddr( 0, MAIN, &o );
    conf_on = o[CONFON].ob_state & SELECTED;
    o[FILESTR].ob_flags |= HIDETREE;
    if( text_h<16 )	/* 003 */
        o[DTSD].ob_state = (o[DTST].ob_state = o[DTSS].ob_state &= ~(X_MAGIC|X_PREFER|X_DRAW3D)) | SELECTED;
    if( !_app || aes_40 )
    {
      rsrc_gaddr( 15, ACCNAME, &ptr2 );
      menu_register( apid, *ptr2 );
    }
  }
  wind_get( 0, WF_CURRXYWH, &dum, &dum, &x_max, &dum );

loop:
  if( _app ) goto open;
  for(;;)
  {
    if( ignore ) emulti.type = MU_MESAG;
    else if( w_handle > 0 ) emulti.type = MU_MESAG|MU_KEYBD|MU_M1|MU_M2|MU_BUTTON;
    else emulti.type = MU_MESAG|MU_KEYBD|MU_BUTTON;
    *(Rect *)&emulti.m1x = center;
    *(Rect *)&emulti.m2x = center;
    multi_evnt( &emulti, buffer );
    if( emulti.event&MU_MESAG )
      switch( buffer[0] )
      {
        case NEO_ACC_INI:                       /* NeoDesk knows we are here */
          if( buffer[3]==NEO_ACC_MAGIC )
          {
            retries = 0;
            neo_acc = *(NEO_ACC **)&buffer[4];  /* set pointer to Neo's fncns */
            graphics = neo_acc->mas->most->graphics;
            neo_apid = buffer[6];               /* NeoDesk's AES ID */
            if( !test_neo() ) discard = 1;
          }
          break;  
        case NEO_CLI_RUN:                       /* run a batch file */
          if( buffer[3]==NEO_ACC_MAGIC )
            if( !neo_acc ) find_neo(buffer);
            else ack();
          break;
        case NEO_ACC_PAS:
          /* the user dragged icons to the desktop icon for this DA */
          if( buffer[3]==NEO_ACC_MAGIC /* && neo_acc  003 */ )
            if( !neo_acc ) find_neo(buffer);
            else
            {
              graf_mouse( BUSY_BEE, 0L );
              asked=0;
              auto_update(0);
              while( (*neo_acc->list_files)( &ptr ) && delete( ptr )>=0 );
              cont = auto_update(1);
              graf_mouse( ARROW, 0L );
              ack();                                /* you MUST ack NeoDesk!! */
              if( cont && w_handle>0 ) read_data( forms[0].tree, &forms[0] );	/* reread this drive's data */
            }
          break;
        case NEO_ACC_BAD:
          /* NEODESK.EXE has exited, so you can't use any of its functions
             or send it messages anymore */
          if( buffer[3] == NEO_ACC_MAGIC )
          {
            close_winds(0);
            neo_acc=0L;
            retries = 0;
          }
          break;
        case NEO_AC_OPEN:
        case AC_OPEN:                           /* user wants DA open */
          if( discard || ignore )
          {
            discard = 0;
            break;
          }
open:     if( x_max < 480 )
          {
            f_alert( BADREZ, 0L, 0 );
            if( _app ) do_quit();
          }
          else if( !neo_acc ) find_neo(buffer);
          else if( test_neo() )
            if( w_handle<=0 ) make_form(0);
  	    else
	    {
	      buffer[3] = w_handle;
	      goto topped;
	    }
          break;
        case AC_CLOSE:
          if( !ignore && neo_acc ) close_winds(1);
          neo_acc = 0L;
          break;
	case AP_TERM:
	  do_quit();
	case WM_REDRAW:
	  form_draw( curform, 0 );
	  break;
	case WM_TOPPED:
topped:   wind_set( buffer[3], WF_TOP, buffer[3], 0, 0, 0 );
          break;
        case WM_MOVED:
          wind_set( buffer[3], WF_CURRXYWH, buffer[4], buffer[5], buffer[6], buffer[7] );
          wind_get( buffer[3], WF_WORKXYWH, &center.x, &center.y, &dum, &dum );
          curform->tree[0].ob_x = center.x;
          curform->tree[0].ob_y = center.y;
          break;
        case WM_CLOSED:
	  use_form( buffer[3], -1 );
	  break;
      }
    if( emulti.event & MU_M2 )
    {
      if( emulti.event&MU_BUTTON ) inside = 0;
    }
    if( emulti.event & MU_M1 )
    {
      if( !(emulti.event&MU_BUTTON) ) inside = 1;
    }
    if( emulti.event&MU_BUTTON && inside )
    {
      wind_get( 0, WF_TOP, &i, &dum, &dum, &dum );
      if( i == w_handle && (next_obj =
          (*gui->xtern.objc_find)( curform->tree, 0, 8, emulti.mouse_x, emulti.mouse_y )) >= 0 )
      {
        cont = (*gui->xtern.form_button)( curform->tree, next_obj, emulti.times, &next_obj );
        if( !cont && next_obj>=0 ) use_form( w_handle, next_obj );
      }
    }
    if( emulti.event&MU_KEYBD )
    {
      edit_obj = next_obj = -1;  /* change me */
      cont = (*gui->xtern.form_keybd)( curform->tree, edit_obj, next_obj, emulti.key,
          &next_obj, &emulti.key );
      if( !cont && next_obj>=0 ) use_form( w_handle, next_obj );
    }
    if( w_handle<=0 && _app ) do_quit();
  }
}


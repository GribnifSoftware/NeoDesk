#define TURBO_C

typedef struct { int x, y, w, h; } Rect;

#include "tos.h"
#include "aes.h"
#include "string.h"
#include "stdlib.h"
#include "e:\compiler.c\include\stat.h"
#include "d:\neo_acc.h"
#include "d:\sources\trashcan\trashcan.h"
#include "e:\compiler.c\include\linea.h"

#define BUSY_BEE HOURGLASS

extern linea0( void ), linea9( void ), lineaa( void ), linea5( void );
void sort(void);
void set_buttons(void);
int read_fat( BPB *bp, unsigned int *cluster, int drv );
int check_folders(void);
int writ( int hand, long n, char *buf );
void mediach(void);
void unsel(void);
void update(void);
int check_neo(void);
void dialog(void);
void read_data(void);
void write_rsc(void);
void ack(void);
void pause(void);

#define rindex  strrchr
#define index   strchr

#define VERSION   0x0100
#define SEC_SIZE  1024

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
char sec_buf[SEC_SIZE], fat[SEC_SIZE*2], *data_name = "x:\\trashcan.dat", 
    *root="x:\\*.*", asked, update_list[16], dflt_drive, US_keybd,
    *rsc_name="TRASHCAN.RSC", *fake="x:\\000.XXX";
int ver=VERSION, window, items, num_sel, first, longest, dts, xoff;
unsigned long table_items;
unsigned int ents_sec, fat_sec;
BPB *bp0;

int dum,
    conf_on=1,
    drive,
    vals,
    text_x, text_y, text_h,
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
Rect r, textr;
OBJECT *mainobj;

void cdecl spf( char *buf, char *fmt, ... )     /* call NeoMaster's sprintf() */
                        /* ANSI C compilers probably won't like this one! */
{
  call_w_save( neo_acc->mas->dopf, buf, fmt, &... );
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

int max( int a, int b)
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

void to_date( char *buf, int date )
{
  register int yr;
  static char *fmt="%02d/%02d/%02d";

  if( (yr = ((date>>9) & 0x7F) + 80) > 99 ) yr -= 100;
  if( US_keybd ) spf( buf, fmt, (date>>5) & 0xf, date&0x1f, yr );
  else spf( buf, fmt, date&0x1f, (date>>5) & 0xf, yr );
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
  *(neo_acc->mas->bad_media) = drive;
  Fsfirst( root, 0x37 );
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
  static char *upd="x:\\";
  
  *upd = drive+'A';
  if( neo_acc->nac_ver >= 0x210 ) call_w_save( neo_acc->update_drive, upd );
  else call_w_save( (int (*)())((long)(neo_acc->trash_files)+0x55A), upd );
}

void set_buttons()
{
  register int i;
  
  if( (i = !num_sel ? DISABLED : 0) != mainobj[DELPERM].ob_state )
  {
    mainobj[DELPERM].ob_state = mainobj[UNDEL].ob_state = i;
    objc_draw( mainobj, DELPERM, 0, r.x, r.y, r.w, r.h );
    objc_draw( mainobj, UNDEL, 0, r.x, r.y, r.w, r.h );
  }
}

void slider( int flg )
{
  register int i;

  if( flg<=0 )
  {
    if( (i = items-10) < 1 ) i=1;
    mainobj[SMLSL].ob_y = (long)(mainobj[BIGSL].ob_height - 
        mainobj[SMLSL].ob_height) * window / i;
    objc_draw( mainobj, BIGSL, 1, r.x, r.y, r.w, r.h );
  }
  if( flg )
  {
    if( (i = longest-30) < 1 ) i=1;
    mainobj[SMLSLH].ob_x = (long)(mainobj[BIGSLH].ob_width - 
        mainobj[SMLSLH].ob_width) * xoff / i;
    objc_draw( mainobj, BIGSLH, 1, r.x, r.y, r.w, r.h );
  }
}

void write_rsc()
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
      i = conf_on;
      Fwrite( hand, 2L, &i );
      Fseek( (long)sizeof(OBJECT)-2, hand, 1 );
      i = !i;
      Fwrite( hand, 2L, &i );
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

void display( int mode, int firstx, int firsty, int lastx, int lasty )
{
  static long patptr[] = { 0x00008888L, 0x00002222L };
  register int i, y, j;
  register char *ptr;
  int arr[4], h, x;
  char buf[31];
  
  buf[30] = '\0';
  if( firsty<0 )
  {
    firsty = 0;
    lasty = 9;
  }
  if( firstx<0 )
  {
    firstx = 0;
    lastx = 29;
  }
  lineaa();
  call_w_save( neo_acc->set_clip_rect, &textr, 1 );
  arr[3] = (arr[1] = y = text_y+firsty*text_h) + (lasty-firsty+1) * text_h - 1;
  WMODE = 0;
  if( mode )
  {
    arr[2] = ( arr[0] = textr.x + 31*8 ) + 8*8 - 1;
    call_w_save( neo_acc->blank_box, arr );
  }
  if( mode <= 0 )
  {
    arr[2] = (arr[0] = textr.x+(firstx<<3)) + ((lastx-firstx+1)<<3) - 1;
    call_w_save( neo_acc->blank_box, arr );
  }
  if( lasty+window >= items ) lasty = items-window-1;
  h = text_h>>3;
  x = text_x + (firstx<<3);
  for( i=firsty, j=find_link(i+window); i<=lasty; i++, y+=text_h )
  {
    if( mode <= 0 )
    {
      if( xoff+firstx < strlen(ptr=&fname[j].dir) )
      {
        strncpy( buf, ptr+xoff+firstx, 30 );
        buf[lastx-firstx+1] = '\0';
        call_w_save( neo_acc->gtext, x, y, buf, h, 0 );
      }
      if( fname[j].hidden )
      {
        X1 = arr[0];
        Y2 = (Y1 = y) + text_h - 1;
        X2 = arr[2] - 1;
        WMODE = COLBIT0 = 1;
        COLBIT1 = COLBIT2 = COLBIT3 = 0;
        PATMSK = 3;
        PATPTR = (int *)patptr;
        linea5();
      }
      /* else */ if( fname[j].selected )
      {
        arr[3] = (arr[1]=y) + text_h - 1;
        WMODE = 2;
        call_w_save( neo_acc->blank_box, arr );
      }
    }
    if( mode )
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
      call_w_save( neo_acc->gtext, text_x+31*8, y, buf, h, 0 );
    }
    j = fname[j].link;
  }
  linea9();
}

void move( int num, int dir )
{
  Rect box1, box2;
  register int n, i;
  
  box1 = box2 = textr;
  box1.h = box2.h++;
  if( !dir )
  {
    if( num+window > items-10 ) num = items-10-window;
    if( num+window < 0 ) num = -window;
    if( num )
    {
      window += num;
      if( (n=abs(num)) >= 10 ) display( -1, -1, -1, 0, 0 );
      else
      {
        i = n*text_h;
        box1.h = box2.h -= i;
        box1.y += i;
        if( num<0 )
        {
          call_w_save( neo_acc->blit, &box2, &box1, 0, 3, (char *)0L );
          display( -1, -1, 0, -1, n-1 );
        }
        else
        {
          call_w_save( neo_acc->blit, &box1, &box2, 0, 3, (char *)0L );
          display( -1, -1, 10-n, -1, 9 );
        }
      }
      slider( 0 );
    }
  }
  else
  {
    if( num+xoff > longest-30 ) num = longest-30-xoff;
    if( num+xoff < 0 ) num = -xoff;
    if( num )
    {
      xoff += num;
      if( (n=abs(num)) >= 30 ) display( 0, -1, -1, 0, 0 );
      else
      {
        i = n<<3;
        box1.w = box2.w = 30*8-i-1;
        box1.x += i;
        if( num<0 )
        {
          call_w_save( neo_acc->blit, &box2, &box1, 0, 3, (char *)0L );
          display( 0, 0, -1, n-1, 0 );
        }
        else
        {
          call_w_save( neo_acc->blit, &box1, &box2, 0, 3, (char *)0L );
          display( 0, 30-n, -1, 29, 0 );
        }
      }
      slider( 1 );
    }
  }
}

void unsel()
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
    display( 0, -1, -1, 0, 0 );
    num_sel = 0;
  }
}

int make_form( int num )
{
  OBJECT *obj;
  Rect rec;
  register int i;
  
  rsrc_gaddr( 0, num, &obj );
  form_center( obj, &rec.x, &rec.y, &rec.w, &rec.h );
  rec.w += 3;
  rec.h += 3;
  objc_draw( obj, 0, 8, rec.x, rec.y, rec.w, rec.h );
  i = form_do( obj, 0 );
  if( obj>=0 && obj[i].ob_flags & EXIT ) obj[i].ob_state=0;
  objc_draw( mainobj, 0, 8, rec.x, rec.y, rec.w, rec.h );
  display( -1, -1, -1, 0, 0 );
  return(i);
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

void read_data()
{
  register unsigned long l;
  register int hand, i, j;
  Dirent dir;
  
  *data_name = drive + 'A';
  items = window = num_sel = longest = xoff = 0;
  fname = (Fname *)*(neo_acc->c_buf);
  graf_mouse( BUSY_BEE, (MFORM *)&dum );
  if( (hand = Fopen( data_name, 0 )) > 0 )
  {
    if( check_ver( hand ) )
    {
      if( (items=table_items) > (l = *(neo_acc->c_buflen)/sizeof(Fname)) )
      {
        items = l;
        f_alert( MOREITMS, 0L, 0 );
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
  graf_mouse( ARROW, (MFORM *)&dum );
  i = mainobj[BIGSL].ob_height;
  if( items > 10 )
    if( (i = i * 10 / items) < 10 ) i = 10;
  mainobj[SMLSL].ob_height = i;
  i = mainobj[BIGSLH].ob_width;
  if( longest > 30 )
    if( (i = i * 30 / longest) < 10 ) i = 10;
  mainobj[SMLSLH].ob_width = i;
  slider(-1);
  set_buttons();
  display( -1, -1, -1, 0, 0 );
}

void ack()                                   /* acknowledge NeoDesk's message */
{
  int buf[8];
  
  buf[0] = DUM_MSG;                     /* always ack with this message */
  appl_write( buf[1]=neo_apid, 16, buf );
}

int check_neo()                         /* does NeoDesk know we are here? */
{
  if( !neo_acc ) f_alert( BADNEO, 0L, 0 );
  return( neo_acc!=0 );                 /* return 1 if initialized */
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
      strlen( name ) < 4 || !ptr ) return(0);
  if( !asked )
  {
    asked=1;
    if( conf_on )
      if( f_alert( DELTEMP, 0L, 0 ) != 1 ) return(-1);
  }
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
  update_list[drv] = 1;
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
  strcpy( obj[CONFORIG].ob_spec.free_string, rindex(name,'\\')+1 );
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
    graf_mouse( ARROW, (MFORM *)&dum );
    if( (i = make_form( CONFL )) == NEWNAME &&
        *(ptr=obj[CONFNEW].ob_spec.tedinfo->te_ptext) )
    {
      from_filename( ptr, rindex(name,'\\')+1 );
      for( i=0; i<11; i++ )
        if( *ptr ) nam[i] = *ptr++;
        else nam[i] = ' ';
      graf_mouse( BUSY_BEE, (MFORM *)&dum );
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
      hand = call_w_save( neo_acc->trash_files, name, &dum, &dum ) ? -1 : 0;
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

void dialog()
{
  int done=0, x, y, y2, w_hand;
  int i, j, state;
  unsigned long bits;
  Rect max;
  char buf[120] = "x:";
  
  form_center( mainobj, &r.x, &r.y, &r.w, &r.h );
  objc_offset( mainobj, NAME0, &textr.x, &textr.y );
  textr.w = 39*8 - 1;
  textr.h = 10*text_h - 1;
  wind_get( 0, WF_WORKXYWH, &max.x, &max.y, &max.w, &max.h );
  rc_intersect( &max, &r );             /* clip for gtext */
  rc_intersect( &r, &textr );
  bits = Drvmap();
  if( !(bits&(1<<drive)) ) drive = 0;
  for( i=0; i<16; i++ )
    mainobj[DRIVEA+i].ob_state = (i==drive ? SELECTED : 0) | 
        (bits&(1<<i) ? 0 : DISABLED);
  mainobj[NAME0].ob_flags |= HIDETREE;
  objc_offset( mainobj, NAME0, &text_x, &text_y );
  if( (w_hand = wind_create( 0, r.x, r.y, r.w, r.h )) < 0 )
  {
    f_alert( A22, 0L, 0 );
    return;
  }
  wind_update( BEG_UPDATE );
  wind_open( w_hand, r.x, r.y, r.w, r.h );
  objc_draw( mainobj, 0, 8, r.x, r.y, r.w, r.h );
  call_w_save( neo_acc->copy_init );
  read_data();
  while( !done )
  {
    if( (i = form_do(mainobj,0)) >= 0 && !(mainobj[i].ob_state & DISABLED) )
    {
      if( mainobj[i].ob_flags & EXIT )
      {
        mainobj[i].ob_state = 0;
        objc_draw( mainobj, i, 0, r.x, r.y, r.w, r.h );
      }
      switch(i)
      {
        case QUIT:
          done++;
          break;
        case WARN:
          make_form( WARNINGS );
          break;
        case GETINFO:
          make_form( INFO1 );
          make_form( INFO2 );
          break;
        case UP:
        case LEFT:
          move(-1,i==LEFT);
          break;
        case DOWN:
        case RIGHT:
          move(1,i==RIGHT);
          break;
        case BIGSL:
          graf_mkstate( &dum, &y, &dum, &dum );
          objc_offset( mainobj, SMLSL, &dum, &y2 );
          move( y<=y2 ? -10 : 10, 0 );
          break;
        case BIGSLH:
          graf_mkstate( &x, &dum, &dum, &dum );
          objc_offset( mainobj, SMLSLH, &y2, &dum );
          move( x<=y2 ? -30 : 30, 1 );
          break;
        case SMLSL:
          if( (i=items-9) < 0 ) i = 0;
          move( (int)((long)i * 
              graf_slidebox( mainobj, BIGSL, SMLSL, 1 ) / 1000L) - window, 0 );
          break;
        case SMLSLH:
          if( (i=longest-29) < 0 ) i = 0;
          move( (int)((long)i * graf_slidebox( mainobj, BIGSLH, SMLSLH, 0 ) 
              / 1000L) - xoff, 1 );
          break;
        case SELALL:
          i = first;
          while( i>=0 )
          {
            /*  if( !fname[i].hidden ) */      fname[i].selected = 1;
            i = fname[i].link;
          }
          num_sel = items;
          set_buttons();
          display( 0, -1, -1, 0, 0 );
          break;
        case DELPERM:
          if( f_alert( DELPER, 0L, 0 ) != 1 ) break;
        case UNDEL:
          graf_mouse( BUSY_BEE, (MFORM *)&dum );         /* read_data does the arrow */
          buf[0] = drive + 'A';
          j = first;
          x = 0;
          while( j>=0 && x>=0 )
          {
            if( fname[j].selected )
            {
              strcpy( buf+2, fname[j].path );
              mainobj[FILESTR].ob_flags = 0;
              mainobj[FILESTR].ob_spec.tedinfo->te_ptext = 
                   rindex(buf,'\\')+1;
              objc_draw( mainobj, FILESTR, 0, r.x, r.y, r.w, r.h );
              x = undelete( buf, i==DELPERM );
            }
            j = fname[j].link;
          }
          mainobj[FILESTR].ob_flags = HIDETREE;
          objc_draw( mainobj, FILESTR-1, 0, r.x, r.y, r.w, r.h );
          update();
          read_data();
          break;
        case NAMEBOX2:
        case NAMEBOX:
          graf_mkstate( &x, &y, &dum, &dum );
          y -= text_y;
          x -= text_x;
          if( check_bounds( x, y ) )
          {
            unsel();
            state = fname[find_link(y/text_h+window)].selected ^ 1;
            do
            {
              i = y/text_h;
              j=find_link(i+window);
              if( /*   !fname[j].hidden && */
                  fname[j].selected != state )
              {
                if( (fname[j].selected = (char)state) != 0 ) num_sel++;
                else num_sel--;
                display( 0, -1, i, 0, i );
              }
              do
              {
                graf_mkstate( &x, &y, &y2, &dum );
                j = (y-=text_y)/text_h;
              }
              while( y2&1 && (j==i || !check_bounds( x-=text_x, y )) );
            }
            while( y2&1 );
            set_buttons();
          }
          else
          {
            unsel();
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
                    display( 0, -1, i, 0, i );
                  }
                y += text_h;
              }
            }  ***********/
            set_buttons();
          }
          break;
        case DTSD:
        case DTST:
        case DTSS:
          if( i-DTSD != dts )
          {
            dts = i - DTSD;
            display( 1, -1, -1, 0, 0 );
          }
          pause();
          break;
        default:
          drive = i - DRIVEA;
          pause();
          read_data();
      }
    }
  }
  call_w_save( neo_acc->copy_free );
  wind_close(w_hand);
  wind_delete(w_hand);
  wind_update( END_UPDATE );
  if( mainobj[CONFON+conf_on].ob_state & SELECTED )
  {
    conf_on = mainobj[CONFON].ob_state & SELECTED;
    write_rsc();
  }
}

#ifndef NOT_ACC
main()
#else
main( int argc, char *argv[] )
#endif
{
  int ignore=0, i, buffer[8], apid;
  char *ptr, **ptr2;                   /* pointer modified by list_files() */

#ifdef NOT_ACC
  extern long atol();
  neo_acc = (NEO_ACC *) atol(argv[1]);
  linea9();
#endif
  linea0();
  apid = appl_init();
  text_h = V_CEL_HT;
  if( !rsrc_load( rsc_name ) )
  {
    form_alert( 1, "[1][|Could not find TRASHCAN.RSC!][Bleh]" );
    ignore++;
  }
  rsrc_gaddr( 0, MAIN, &mainobj );
  conf_on = mainobj[CONFON].ob_state & SELECTED;
  mainobj[FILESTR].ob_flags = HIDETREE;
#ifndef NOT_ACC
  rsrc_gaddr( 15, ACCNAME, &ptr2 );
  if( V_X_MAX >= 480 ) menu_register( apid, *ptr2 );
#endif
  drive = dflt_drive = Dgetdrv();
  US_keybd = *((char *)Keytbl( (void *)-1L, (void *)-1L, (void *)-1L )->shift + 0x28)
      == '\"';

#ifndef NOT_ACC
  for(;;)
  {
    evnt_mesag( buffer );
#else
    buffer[0] = AC_OPEN;
#endif
    if( !ignore )
      switch( buffer[0] )
      {
        case NEO_ACC_INI:                       /* NeoDesk knows we are here */
          if( buffer[3]==NEO_ACC_MAGIC )
          {
            neo_acc = *(NEO_ACC **)&buffer[4];  /* set pointer to Neo's fncns */
            neo_apid = buffer[6];               /* NeoDesk's AES ID */
          }
          break;  
        case NEO_CLI_RUN:                       /* run a batch file */
          if( buffer[3]==NEO_ACC_MAGIC ) ack();
          break;
        case NEO_ACC_PAS:
          /* the user dragged icons to the desktop icon for this DA */
          if( buffer[3]==NEO_ACC_MAGIC && neo_acc )
          {
            graf_mouse( BUSY_BEE, (MFORM *)&dum );
            asked=0;
            for( i=0; i<16; i++ )
              update_list[i] = 0;
            while( call_w_save( neo_acc->list_files, &ptr ) && delete( ptr )>=0 );
            for( i=0; i<16; i++ )
              if( update_list[i] )
              {
                drive = i;
                update();
              }
            graf_mouse( ARROW, (MFORM *)&dum );
          }
          ack();                                /* you MUST ack NeoDesk!! */
          break;
        case NEO_ACC_BAD:
          /* NEODESK.EXE has exited, so you can't use any of its functions
             or send it messages anymore */
          if( buffer[3] == NEO_ACC_MAGIC ) neo_acc=0L;
          break;
        case NEO_AC_OPEN:
        case AC_OPEN:                           /* user wants DA open */
          if( check_neo() ) dialog();
          break;
        case AC_CLOSE:
          neo_acc = 0L;
      }
#ifndef NOT_ACC
  }
#else
  return(0);
#endif
}


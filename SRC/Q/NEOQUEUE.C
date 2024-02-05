/* NeoDesk 3.01 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/
#include "neoq_c.h"
#include "..\neodesk.h"
#include "aes.h"
#include "vdi.h"
#include "tos.h"
#include "string.h"
#include "stdlib.h"
#include "..\neocommn.h"

#define QWIND_TYPE  (CLOSER|MOVER|NAME)
#define PRBUFSIZ   8192
#define BC_PRINTER 0
#define BC_AUXPORT 1
#define WIDTH      40
#define OS_version  (*(int *)((*(long *)0x4F2)+2))
#define OS_bp_ptr   (*(long *)((*(long *)0x4F2)+40))

void set_timer(void);
void redraw( int xoff, int l, Rect rect );
void form_feed(void);
void set_trash( int state );
int f_alert( int buttn, int index );
int find_obj( int mx, int my );

int work_in[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
    work_out[57],
    cliparray[4] = { 0, 0 },
    handle,
    apid,
    neo_apid,
    w_handle = -1;
    
int text_h,
    files,
    len,
    pos,
    retries,
    f_hand = -1,
    outhand = BC_PRINTER,
    dum;
    
EMULTI emulti = { MU_MESAG, 1, 1, 1,  0, 0, 0, 0, 0,  1, 0, 0, 0, 0,  500 };
long bp_ptr = 0x602C;
NEO_ACC *neo_acc=0L;                    /* points to NeoDesk's functions */
LoadCookie *lc;

char filename[10][120], prbuff[PRBUFSIZ], *bufptr, multitask, **extdata;
OBJECT *form, *cnfg, *copyrt;
Rect form_rect, cnfg_rect, cr_rect;
PRN_PARAM prn_param;

extern long cdecl bFopen( char *path, int mode );
extern long cdecl bFread( int hand, long size, void *buf );
extern int cdecl bFclose( int hand );

/********************************************************************/
void ack(void)
{
  int buf[8];
  
  buf[0] = DUM_MSG;
  buf[1] = apid;
  appl_write( neo_apid, 16, buf );
}
/***************************************************************/
void blit( Rect *box1, Rect *box2 )
{
  int array[8];
  static MFDB fdb = { 0L, 0, 0, 0, 0, 0, 0, 0, 0 };

  array[2] = (array[0] = box1->x) + box1->w;
  array[3] = (array[1] = box1->y) + box1->h - 1;
  array[6] = (array[4] = box2->x) + box2->w;
  array[7] = (array[5] = box2->y) + box2->h - 1;
  graf_mouse( M_OFF, 0L );
  vro_cpyfm( handle, 3, array, &fdb, &fdb );
  graf_mouse( M_ON, 0L );
}
/********************************************************************/
int check_prn(void)
{
  while( !Bcostat(outhand) )
    if( f_alert( 2, A2 ) == 2 )
        return(0);
  return(1);
}
/********************************************************************/
int check_files(void)
{
  if( !files && !check_prn() ) return(0);
  if( files == 10 )
  {
    f_alert( 1, A1 );
    return(0);
  }
  return(1);
}
/********************************************************************/
void stop_q(void)
{
  *lc->pr_count = 0L;
  *lc->pr_buftail = bufptr = *lc->pr_bufstart;
}
/********************************************************************/
void close_file(void)
{
  bFclose( f_hand );
  f_hand = -1;
  stop_q();
  if( prn_param.ffd ) form_feed();
  *lc->no_bcon = 0;
}
/********************************************************************/
void copyright(void)
{
  wind_update( BEG_UPDATE );
  objc_draw( copyrt, ROOT, MAX_DEPTH, cr_rect.x, cr_rect.y, cr_rect.w, 
      cr_rect.h );
  form_do( copyrt, 0 );
  form_dial( FMD_FINISH, 0, 0, 0, 0, cr_rect.x, cr_rect.y, cr_rect.w, 
      cr_rect.h );
  wind_update( END_UPDATE );
}
/********************************************************************/
void delete( int index )
{
  register int i;
  
  for( i=index+1; i<files; i++ )
    strcpy( filename[i-1], filename[i] );
  files--;
  set_timer();
}
/********************************************************************/
int has_data(void)
{
  return f_hand<0 && lc && *(lc->pr_count);
}
void xor_box( int *box, int ox, int oy, int index )
{
  box[2] = (box[0] = ox + 2) + (WIDTH*8);
  box[3] = (box[1] = oy + text_h*index) + text_h;
  v_bar( handle, box );
}
void drag( int oldy )
{
  int x, y, b, ox, oy, dbx[10], index, newin, top, max[2], box[4], f;
  register int i, j, state=0, dy, oldst=0, lnewin=-1;
  char temp[120], is_data;

  objc_offset( form, QBOX, &ox, &oy );
  max[0] = top = oy += 1;
  max[1] = form_rect.y + form_rect.h - text_h - 8;
  index = (oldy-top) / text_h;
  if( index >= (f=files+(is_data=has_data())) ) return;
  graf_mouse( M_OFF, 0L );
  vswr_mode( handle, 3 );
  xor_box( box, ox, oy, index );
  dbx[6] = dbx[8] = dbx[0] = box[0];
  dbx[3] = dbx[9] = dbx[1] = box[1];
  dbx[2] = dbx[4] = box[2];
  dbx[5] = dbx[7] = box[3];
  graf_mkstate( &x, &y, &b, &dum );
  while( b & 1 )
  {
    if( state )
    {
      graf_mouse( M_OFF, 0L );
      v_pline( handle, 5, dbx );
    }
    dy = y - oldy;
    if( dbx[1] + dy < max[0] ) dy = max[0] - dbx[1];
    if( dbx[5] + dy > max[1] ) dy = max[1] - dbx[5];
    dbx[1] = dbx[3] = dbx[9] += dy;
    dbx[5] = dbx[7] += dy;
    state = 1;
    newin = (dbx[1]-top+(text_h>>1)) / text_h;
    if( newin<=f-1 && newin!=index && (index!=f-1||newin<f) &&
        (!is_data||index&&newin) )	/* don't drag to/from Spooled */
    {
      state = 2;
    }
    else if( find_obj( x, y ) == QTRASH )
    {
      state = 3;
      if( oldst != 3 ) set_trash( SELECTED );
    }
    if( oldst==3 && state!=3 ) set_trash( 0 );
    if( lnewin!=newin )
    {
      if( oldst==2 ) xor_box( box, ox, oy, lnewin );
      if( state==2 ) xor_box( box, ox, oy, newin );
    }
    v_pline( handle, 5, dbx );
    graf_mouse( M_ON, 0L );
    oldy = y;
    oldst = state;
    lnewin = newin;
    do
      graf_mkstate( &x, &y, &b, &dum );
    while( b&1 && y==oldy );
  }
  if( state )
  {
    graf_mouse( M_OFF, 0L );
    v_pline( handle, 5, dbx );
    graf_mouse( M_ON, 0L );
    if( state > 1 )
    {
      strcpy( temp, filename[index] );
      switch( state )
      {
        case 2:            /* re-order */
          if( is_data )
          {
            newin--;
            index--;
          }
          if( (!newin||!index) && f_hand > 0 )
          {
            if( f_alert( 1, A4 ) == 2 ) break;
            close_file();
          }
          if( newin>index )
          {
            for( i=j=0; i<files; i++ )
            {
              if( j == newin ) strcpy( filename[j++], temp );
              if( i != index ) strcpy( filename[j++], filename[i] );
            }
            if( j == files-1 ) strcpy( filename[j], temp );
          }
          else
          {
            for( i=j=files-1; i>=0; i-- )
            {
              if( j == newin ) strcpy( filename[j--], temp );
              if( i != index ) strcpy( filename[j--], filename[i] );
            }
            if( !j ) strcpy( filename[0], temp );
          }
          break;
        case 3:            /* delete */
          set_trash(0);
          if( !index && is_data ) stop_q();
          else
          {
            if( !index && f_hand > 0 )
            {
              if( f_alert( 1, A5 ) == 2 ) break;
              close_file();
            }
            delete( index );
          }
      }
      vswr_mode( handle, 1 );
      redraw( 0, WIDTH, form_rect );
    }
  }
  if( state<2 )
  {
    if( state ) graf_mouse( M_OFF, 0L );
    xor_box( box, ox, oy, index );
    vswr_mode( handle, 1 );
    graf_mouse( M_ON, 0L );
  }
}
/*********/
void set_trash( int state )
{
  form[QTRASH].ob_state = state;
  objc_draw( form, QTRASH, 0, form_rect.x, form_rect.y, form_rect.w, 
      form_rect.h );
}
/********************************************************************/
int f_alert( int buttn, int index )
{
  char **ptr;
  
  rsrc_gaddr( 15, index, &ptr );
  return( form_alert( buttn, *ptr ) );
}
/***********************************************************************/
int find_obj( int mx, int my )
{
  return( objc_find( form, 0, MAX_DEPTH, mx, my ) );
}
/********************************************************************/
void form_feed(void)
{
  int cnt=0;
  long *i;
  
  i = lc->pr_count;
  while( *i && --cnt );
  if( !cnt )
  {
    *(bufptr = *lc->pr_buftail = *(lc->pr_bufstart)) = '\f';
    *i = 1L;
    while( *i && --cnt );
  }
}
/******************************************************************
int getcookie( long cookie, LoadCookie **ptr )
{
    register long *cookiejar;
    long stack;

    stack = Super((void *)0L);
    if( (cookiejar = *(long **)0x5a0) != 0 ) 
      do {
          if (*cookiejar == cookie)
          {
            if( ptr ) *ptr = (LoadCookie *)*(cookiejar+1);
            Super((void *)stack);
            return 1;
          }
          else cookiejar += 2;
      } while (*cookiejar);
    Super((void *)stack);
    return 0;
}
**********************************************************************/
void move( int num )
{
  register int new_pos, dx, i;
  Rect box1, box2;
  
  wind_update( BEG_UPDATE );
  new_pos = pos + num;
  if( new_pos > len - WIDTH ) new_pos = len - WIDTH;
  if( new_pos < 0 ) new_pos = 0;
  if( new_pos != pos )
  {
    if( abs( new_pos - pos ) >= WIDTH )
    {
      redraw( 0, WIDTH, form_rect );
      pos = new_pos;
    }
    else
    {
      box1.x = box2.x = form[QBOX].ob_x + form_rect.x + 5;
      box1.y = box2.y = form[QBOX].ob_y + form_rect.y;
      box1.w = (WIDTH*8);
      box1.h = box2.h = form[QBOX].ob_height;
      i = new_pos - pos;
      dx = abs(i) << 3;
      box2.w = box1.w -= dx;
      box2.x += dx;
      pos = new_pos;
      if( i<0 )
      {
        blit( &box1, &box2 );
        redraw( 0, -i, form_rect );
      }
      else
      {
        blit( &box2, &box1 );
        redraw( WIDTH - i, i, form_rect );
      }
    }
  }
  wind_update( END_UPDATE );
}
/********************************************************************/
void only_files(void)
{
  f_alert( 1, A6 );
}
/**********************************************************************/
void redraw( int xoff, int l, Rect rect )
{
  int x, y;
  register int i, j;
  int array[4];
  char str[61], is_data, *ptr;
  
  if( w_handle > 0 )
  {
    objc_offset( form, QBOX, &x, &y );
    graf_mouse( M_OFF, 0L );
    is_data = has_data();
    len = 1;
    if( is_data ) len = strlen(*extdata);
    for( i=0; i<files; i++ ) 
      if( (j=strlen(filename[i])) > len ) len = j;
    if( pos > len-WIDTH ) pos = len - WIDTH;
    if( pos < 0 ) pos = 0;
    form[QSMALL].ob_width = len <= WIDTH ? form[QBIG].ob_width :
        (long)WIDTH * form[QBIG].ob_width / len;
    form[QSMALL].ob_x = form[QBIG].ob_x + (long) pos *
        form[QBIG].ob_width / len;
    objc_draw( form, QBIG, 0, rect.x, rect.y, rect.w, rect.h );
    objc_draw( form, QSMALL, 0, rect.x, rect.y, rect.w, rect.h );
    x += (xoff << 3) + 3;
    array[0] = x;
    array[2] = x + (l<<3);
    array[1] = ++y;
    array[3] = y + form[QBOX].ob_height - 4;
    v_bar( handle, array );
    y++;
    for( i=0; i<files+is_data; i++, y += text_h )
    {
      ptr = is_data ? (!i ? *extdata : filename[i-1]) : filename[i];
      if( (j=pos+xoff)<strlen(ptr) )
      {
        strncpy( str, ptr+j, l );
        str[l] = '\0';
        v_gtext( handle, x, y, str );
      }
    }
    graf_mouse( M_ON, 0L );
  }
}
/**********************************************************************/
void redraw_all( int flag )
{
  int pxarray[4];
  Rect rect;
  
  if( w_handle > 0 )
  {
    wind_update( BEG_UPDATE );
    wind_get( w_handle, WF_FIRSTXYWH, &pxarray[0], &pxarray[1],
        &pxarray[2], &pxarray[3] );
    graf_mouse( M_OFF, 0L );
    while( pxarray[2] && pxarray[3] )
    {
      rect.w = pxarray[2];
      rect.h = pxarray[3];
      pxarray[2] += (rect.x = pxarray[0]) - 1;
      pxarray[3] += (rect.y = pxarray[1]) - 1;
      vs_clip( handle, 1, pxarray );
      if( flag ) objc_draw( form, 0, MAX_DEPTH, rect.x, rect.y, 
          rect.w, rect.h );
      redraw( 0, WIDTH, rect );
      wind_get( w_handle, WF_NEXTXYWH, &pxarray[0], &pxarray[1],
          &pxarray[2], &pxarray[3] );
    }
    graf_mouse( M_ON, 0L );
    vs_clip( handle, 1, cliparray );
    wind_update( END_UPDATE );
  }
}
/**********************************************************************/
char *rind( char *ptr )
{
  char *res;
  
  return( (res=strrchr(ptr,'\\')) == (char *)0 ? ptr : res+1 );
}
/********************************************************************/
void set_ptr(void)
{
  if( OS_version >= 0x0102 ) bp_ptr = OS_bp_ptr;
}
/********************************************************************/
void set_timer(void)
{
  if( files > 0 || w_handle>0 /* for Spool update */ ) emulti.type |= MU_TIMER;
  else emulti.type &= ~MU_TIMER;
}
/********************************************************************/
void wait(void)
{
  int b;
  
  do
    graf_mkstate( &dum, &dum, &b, &dum );
  while( b&1 );
}
/**********************************************************************/
void reinit(void)
{
  long c;
  char *o;
  
  if( lc )
  {
    if( *(lc->pr_bufsiz) < PRBUFSIZ )
    {
      c = *(lc->pr_count);
      *(lc->pr_count) = 0L;
      if( (o=*(lc->pr_bufstart)) != 0 )
      {
        *(lc->pr_count) = 0L;
        memcpy( prbuff, o, c );
        bufptr = **(lc->bufptr) - o + prbuff;
        *(lc->pr_buftail) = *(lc->pr_buftail) - o + prbuff;
      }
      else bufptr = *(lc->pr_buftail) = prbuff;
      *(lc->pr_bufmax) = (*(lc->pr_bufstart)=prbuff) + (*(lc->pr_bufsiz)=PRBUFSIZ);
      *(lc->outhand) = outhand;
      *(lc->pr_count) = c;
    }
    *(lc->bufptr) = &bufptr;
  }
}
/**********************************************************************/
void find_neo( int *buf )
{
  /* throw away if too many tries */
  if( ++retries>10 )
  {
/*%    f_alert1( A3 );*/
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
}
/**********************************************************************/
void clear_top(void)
{
  if( lc ) *lc->top = 0;
}
void close_it(void)
{
  if( w_handle>0 )
  {
    clear_top();		/* 003 */
    wind_close( w_handle );
    wind_delete( w_handle );
    emulti.type = MU_MESAG;
    w_handle = -1;
    set_timer();
  }
}
/**********************************************************************/
int get_load(void)
{
  if( !lc )
    if( getcookie( LOAD_COOKIE, &lc ) == CJar_OK )
      if( lc->pr_valid != PR_VALID ) lc = 0L;
      else reinit();
  return lc != 0L;
}
/**********************************************************************/
int main(void)
{
  unsigned int buffer[8];
  int err, i, sx, sy, bx, by, is_121;
  char temp[120], name[13]="", failed=1, **ptr, *ptr2, top=0, fsel[120],
      inside=0, last_data=0;
  Rect wind;
  register int j;
  long l;

  apid = appl_init();
  multitask = _GemParBlk.global[1]==-1;
  if( _GemParBlk.global[0] >= 0x400 ) shel_write( 9, 1, 0, 0L, 0L );
  handle = graf_handle( &dum, &text_h, &dum, &dum );
  is_121 = text_h>8;
  if( rsrc_load( is_121 ? "NEOQ_M.RSC" : "NEOQ_C.RSC" ) ) failed=0;
  else form_alert( 1,"[1][|NeoQueue could not find|its resource file!][Hm!]");
  if( !failed )
  {
    rsrc_gaddr( 0, QUEUE, &form );
    rsrc_gaddr( 0, PRNCNFG, &cnfg );
    rsrc_gaddr( 0, INFORM, &copyrt );
    rsrc_gaddr( 15, EXTDATA, &extdata );
    form_center( form, &form_rect.x, &form_rect.y, &form_rect.w,
        &form_rect.h );
    form_center( cnfg, &cnfg_rect.x, &cnfg_rect.y, &cnfg_rect.w,
        &cnfg_rect.h );
    form_center( copyrt, &cr_rect.x, &cr_rect.y, &cr_rect.w,
        &cr_rect.h );
    wind_calc( 0, QWIND_TYPE, form_rect.x, form_rect.y, form_rect.w,
        form_rect.h, &wind.x, &wind.y, &wind.w, &wind.h );
    if( _GemParBlk.global[0] >= 0x340 && _GemParBlk.global[0] != 0x399 )  /* MagiX */
    {
      if( _GemParBlk.global[0] < 0x410 ) sx = 1;
      else
      {
        sx = 0;
        appl_getinfo( 13, &sx, &sy, &sy, &sy );
      }
      if( sx ) make_3d();
    }
  }
  work_in[0] = Getrez()+2;
  v_opnvwk( work_in, &handle, work_out );
  if( handle<0 )
  {
    form_alert( 1, "[1][|Error opening|VDI workstation!][OK]" );
    failed = 1;
  }
  wind_get( 0, WF_CURRXYWH, &dum, &dum, cliparray+2, cliparray+3 );
  cliparray[2]--;
  cliparray[3]--;
  if( !failed )
  {
    vs_clip( handle, 1, cliparray );
    vswr_mode( handle, 1 );
    vst_point( handle, 9 + is_121, &dum, &dum, &dum, &dum );
/*%  vst_color( handle, 1 ); */
    vst_alignment( handle, 0, 5, &dum, &dum );
    vsl_udsty( handle, 0x3333 );
    vsl_type( handle, 7 );
    vsf_interior( handle, 1 );
    vsf_color( handle, 0 );
  }
  Supexec( (long (*)())set_ptr );
  fsel[0] = (i=Dgetdrv()) + 'A';
  fsel[1] = ':';
  Dgetpath( fsel+2, i+i );
  strcat( fsel, "\\*.*" );
  if( !failed && (!_app || multitask) )
  {
    rsrc_gaddr( 15, A7, &ptr );
    menu_register( apid, *ptr );
  }
  if( _app )
    if( failed ) goto term;
    else goto open;
  
  for(;;)
  {
    get_load();
    *(Rect *)&emulti.m1x = form_rect;
    *(Rect *)&emulti.m2x = form_rect;
    multi_evnt( &emulti, (int *)buffer );
/*    if( (emulti.event&MU_TIMER) && w_handle<0 )
    {
      emulti.event = MU_TIMER;
      top = 0;
    }
    else */
    if( w_handle<0 ) top = 0;
    else
    {
      wind_get( 0, WF_TOP, &i, &dum, &dum, &dum );
      top = (i == w_handle);
    }
    if( lc )
    {
      *lc->top = top;
      if( (i=has_data()) != last_data )
      {
        if( w_handle>0 ) redraw_all(0);
        last_data = i;
      }
    }
    if( emulti.event & MU_KEYBD && emulti.mouse_k==4 &&
        emulti.key>>8 == 0x11 ) goto closed;
    if( lc && !top && emulti.event&(MU_BUTTON|MU_TIMER|MU_M1|MU_M2) )
    {
      if( f_hand < 0 && !*lc->pr_count )
        while( files && (f_hand=bFopen(filename[0],0)) < 0 )
        {
          rsrc_gaddr( 15, A8, &ptr );
          strcpy( temp, *ptr );
          strcpy( ptr2=strchr(temp,'X'), rind(filename[0]) );
          strcat( temp, ptr2-temp+*ptr+12 );
          if( form_alert( 1, temp ) == 2 )
          {
            delete(0);
            redraw_all(0);
          }
        }
      if( f_hand > 0 )
      {
        *lc->no_bcon = 1;
reread: if( (l=*lc->pr_count)==0 ) l = bFread( f_hand, 
            *(lc->pr_bufsiz), bufptr=*(lc->pr_buftail)=*(lc->pr_bufstart) );
        if( l < 0 )
        {
          *lc->pr_count=0L;
          if( f_alert( 1, A10 ) == 1 ) goto reread;
          /* else fall through to next if */
        }
        else *lc->pr_buftail = *lc->pr_bufstart + (*lc->pr_count = l);
        if( !*lc->pr_count )
        {
          close_file();
          delete(0);
          redraw_all(0);
        }
      }
    }
    if( emulti.event & MU_MESAG ) switch( buffer[0] )
    {
      case NEO_ACC_INI:
        if( buffer[3]==NEO_ACC_MAGIC )
        {
          retries = 0;
          neo_acc = *(NEO_ACC **)&buffer[4];
          neo_apid = buffer[6];
        }
        break;  
      case NEO_ACC_BAD:
        if( buffer[3] == NEO_ACC_MAGIC ) neo_acc=0L;
        break;
      case NEO_CLI_RUN:
        ack();
        break;
      case NEO_ACC_PAS:
        if( buffer[3]==NEO_ACC_MAGIC )
        {
          if( !neo_acc )
          {
            find_neo( (int *)buffer );
            break;
          }
          j = files;
          if( !failed )
            if( !lc ) f_alert( 1, A16 );
            else
            {
              err=0;
              while( (*neo_acc->list_files)(&ptr2) && !err )
                if( *(ptr2+strlen(ptr2)-1) != ':' && *(ptr2+strlen(ptr2)-1) !=
                    '\\' )
                  if( check_files() ) strcpy( filename[files++], ptr2 );
                  else err++;
                else
                {
                  only_files();
                  err++;
                }
            }
          ack();
          if( files != j ) redraw_all(0);
          set_timer();
        }
        break;
      case PR_INIT:
        prn_param.prn_set = (*(PRN_PARAM **)&buffer[6]) -> prn_set;
        prn_param.ffd = (*(PRN_PARAM **)&buffer[6]) -> ffd;
        outhand = (prn_param.prn_set&0x10) ? BC_AUXPORT : BC_PRINTER;
        Setprt( prn_param.prn_set );
        neo_apid = buffer[1];
        ack();
        emulti.type = MU_MESAG;
        set_timer();
        if( w_handle > 0 ) emulti.type |= MU_BUTTON|MU_KEYBD|MU_M1|MU_M2;
        break;
      case PR_REQ:
        if( !neo_acc )
        {
          find_neo( (int *)buffer );
          break;
        }
        (*(PRN_PARAM **)&buffer[2])->prn_set = prn_param.prn_set;
        (*(PRN_PARAM **)&buffer[2])->ffd = prn_param.ffd;
        ack();
        break;
      case PR_SCRN:
        if( f_hand > 0 || lc && *lc->pr_count > 0 )
        {
          f_alert( 1, A11 );
          break;
        }
        if( check_prn() ) v_hardcopy( handle );
        break;
      case PR_LIST:
      case NEO_AC_OPEN:
      case AC_OPEN:
open:   reinit();
        if( failed ) break;
        Setprt( prn_param.prn_set );
        graf_mouse( ARROW, 0L );
        if( w_handle < 0 )
        {
          if( (w_handle = wind_create( QWIND_TYPE, wind.x, wind.y, 
              wind.w, wind.h )) < 0 ) f_alert( 1, A12 );
          else
          {
            rsrc_gaddr( 15, A13, &ptr );
            wind_set( w_handle, WF_NAME, *ptr, 0, 0 );
            pos = 0;
            wind_open( w_handle, wind.x, wind.y, wind.w, wind.h );
            emulti.type |= MU_BUTTON|MU_KEYBD|MU_M1|MU_M2;
          }
          break;
        }
        if( top )
        {
          copyright();
          break;
        }
      case WM_TOPPED:
        if( w_handle > 0 ) wind_set( w_handle, WF_TOP, w_handle, 0, 0, 0 );
        break;
      case WM_MOVED:
        wind_set( w_handle, WF_CURRXYWH, wind.x=buffer[4], wind.y=buffer[5],
           wind.w, wind.h );
        wind_get( w_handle, WF_WORKXYWH, &form_rect.x, &form_rect.y, &dum,
            &dum );
        form[0].ob_x = form_rect.x + 3;
        form[0].ob_y = form_rect.y + 3;
        break;
      case WM_REDRAW:
        redraw_all(1);
        break;
      case AC_CLOSE:
        neo_acc = 0L;
        emulti.type = MU_MESAG;
        set_timer();
        w_handle = -1;
        clear_top();	/* 003 */
        break;
      case WM_CLOSED:
closed: if( !_app )
        {
          close_it();
          break;
        }	/* term if app */
      case AP_TERM:
term:   if( lc && *(lc->pr_bufstart)==prbuff )
        {
          *(lc->pr_bufmax) = 0L;
          *(lc->bufptr) = 0L;
          *(lc->pr_buftail) = 0L;
          *(lc->pr_bufstart) = 0L;
          if( f_hand>0 ) *lc->no_bcon = 0;
          *(lc->pr_bufsiz) = PRBUFSIZ-1L;
        }
        if( !failed ) v_clsvwk(handle);
        close_it();
        rsrc_free();
        appl_exit();
        return 0;
    }
    if( emulti.event & MU_M2 && emulti.event&MU_BUTTON ) inside = 0;
    if( emulti.event & MU_M1 && !(emulti.event&MU_BUTTON) ) inside = 1;
    if( top && emulti.event&MU_BUTTON && inside )
    {
      switch( find_obj( emulti.mouse_x, emulti.mouse_y ) )
      {
        case QLEFT:
          move( -1 );
          break;
        case QRIGHT:
          move( 1 );
          break;
        case QBIG:
          if( emulti.mouse_x < form[QSMALL].ob_x + form_rect.x ) move( -WIDTH );
          else move( WIDTH );
          break;
        case QSMALL:
          objc_offset( form, QSMALL, &sx, &sy );
          objc_offset( form, QBIG, &bx, &by );
          if( graf_dragbox( form[QSMALL].ob_width, form[QSMALL].ob_height,
              sx, sy, bx, by, form[QBIG].ob_width, form[QBIG].ob_height,
              &i, &dum ) ) move( (int)(((long)i-bx+1)*len/form[QBIG].ob_width
              - pos ) );
          break;
        case QBOX:
          drag( emulti.mouse_y );
          break;
        case QFILES:
          wait();
          if( !lc ) f_alert( 1, A16 );
          else if( check_files() )
          {
            if( !fsel_input( fsel, name, &i ) ) break;
            if( !i || !name[0] ) break;
            if( !files ) if( !check_prn() ) break;
            strcpy( ptr2=filename[files++], fsel );
            *rind( ptr2 ) = '\0';
            strcat( ptr2, name );
            redraw_all(0);
          }
          break;
        case QPRNCNFG:
          wait();
          Setprt( i = prn_param.prn_set );
          for( j=PRNBUT0; j<=PRNBUT0+15; j+=3, i=i>>1 )
          { 
            cnfg[j+!(i&1)].ob_state |= SELECTED;
            cnfg[j+(i&1)].ob_state &= ~SELECTED;
          }
          cnfg[PRNFFD+!(prn_param.ffd)].ob_state |= SELECTED;
          cnfg[PRNFFD+prn_param.ffd].ob_state &= ~SELECTED;
          wind_update( BEG_UPDATE );
          objc_draw( cnfg, ROOT, MAX_DEPTH, cnfg_rect.x, cnfg_rect.y,
              cnfg_rect.w, cnfg_rect.h );
          cnfg[i = form_do( cnfg, 0 )].ob_state &= ~SELECTED;
          form_dial( FMD_FINISH, 0, 0, 0, 0, cnfg_rect.x, cnfg_rect.y,
              cnfg_rect.w, cnfg_rect.h );
          wind_update( END_UPDATE );
          if( i==PRNQOK )
          {
            for( i=0, j=PRNBUT0+15; j>=PRNBUT0; j-=3 )
            {
              i <<= 1;
              if( cnfg[j].ob_state & SELECTED ) i |= 1;
            }
            outhand = (i&0x10) ? BC_AUXPORT : BC_PRINTER;
            reinit();
            Setprt( prn_param.prn_set = i );
            prn_param.ffd = cnfg[PRNFFD].ob_state & SELECTED ? 1 : 0;
          }
          break;
        case QFFD:
          wait();
          if( f_hand >= 0 )
            if( f_alert( 2, A14 ) == 2 ) break;
          if( check_prn() ) 
            if( !lc ) Bconout( outhand, '\f' );
            else
            {
              *lc->top = 0;
              form_feed();
            }
          wait();
          break;
        case COPYRT:
          wait();
          copyright();
          break;
      }
    }
  }
}


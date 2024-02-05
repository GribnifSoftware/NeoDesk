/* #define MINT_DEBUG */

#include "stdio.h"
#include "new_aes.h"
#include "xwind.h"
#include "stdlib.h"
#include "string.h"
#include "tos.h"
#include "neocommn.h"
#include "vdi.h"
#include "linea.h"
#include "multevnt.h"
#define _WINDOWS
#include "win_var.h"
#include "win_inc.h"
#include "windows.h"
#include "ierrno.h"

#define DEBUGGER(x,y,z)

/* for now, the wind_set( X_WF_DIALFLGS ) map to bits in treeflag */
#define X_WTFL_ACTIVE	8
#define X_WTFL_BLITSCRL	16
#define X_WTFL_KEYS     32
#define X_WTFL_MAXIMIZE 64

void test_msg( APP *ap, char *msg );

Rect norect = { 0, 0, 0, 0 };

int last_handle=-1, last_gadget, last_gad_m,
    last_gad_w, gadget_ok;
int nobuf[8];
extern int _aes_ver;
unsigned long last_tic, gadget_tic;
/*%int new_un, new_ptr, cur_last, not_taken, old_un;*/
/*%long play_key;*/
char no_set;
APP *wind_app;
Rect wclip_rec, wclip_big;
Window *wclip_win;
void dial_sliders( Window *w, int draw );

void close_del( Window *w )
{
  wind_app = 0L;		/* don't consider old windows */
  w->apid = curapp->id;         /* so that no error occurs */
  if( w->place > 0 ) _wind_close( w->handle );
  _wind_delete( w->handle );
}

void _wind_new(void)
{
  Window *w, *w2;
  APP *ap;

  update.i = cnt_update.i = 0;
  has_update = 0L;
  for( ap=app0; ap; ap=ap->next )
  {
    ap->update.i = 0;
    ap->old_update = 0L;
  }
  for( w2=desktop->next; (w=w2)!=0; )
  {
    w2 = w->next;
    if( w->apid==curapp->id ) close_del(w);
  }
}

void get_top(void)
{
  int top, dum;
  
  wind_get( 0, WF_TOP, &top, &dum, &dum, &dum );
  top_wind = find_window(top);
}

APP *key_owner(void)
{
  APP *ap;

  if( update.i ) return has_update;
  else if( no_top || top_wind==ascii_w || has_menu && top_wind==desktop )
      return has_menu;
  else
  {
    for( ap=app0; ap; ap=ap->next )
      if( ap->id==top_wind->apid ) return ap;
    return 0L;
  }
}

int owns_key( APP *ap )
{
  if( !update.i ) return 1;
  else return ap==has_update;
}

int _is_key( KEYCODE *k, unsigned char shift, unsigned int key )
{
  if( shift&3 ) shift |= 3;     /* 1 shift key becomes both */
  if( k->shift!=shift ) return 0;
  if( k->ascii ) return (unsigned char)key == k->ascii;
  return (unsigned char)(key>>8) == k->scan;
}

int is_key( KEYCODE *k, unsigned char shift, unsigned int key )
{
  APP *ap;
  int i;

  if( _is_key( k, shift, key ) )
  {
    if( (ap=key_owner()) != 0 )
      for( i=0; i<3; i++ )
        if( _is_key( &ap->flags.reserve_key[i], shift, key ) ) return 0;
    return 1;
  }
  return 0;
}

void set_dc( int r )
{
  static int clicks[]={ 113*5, 83*5, 69*5, 55*5, 42*5 };

  if( r>=0 && r<=4 ) dc_pause = clicks[dc_rate=r]/ticcal;
}

int _evnt_dclick( int new, int getset )
{
  int ret;

  ret = dc_rate;
  if( getset ) set_dc( new );
  return(ret);
}

int un_update( int new )
{
  int i, ret;
  char j;
  UPDATE up;
  
  up.i = new;
  ret = cnt_update.i;
  for( i=0; i<2; i++ )
    while( (j = cnt_update.c[i] - up.c[i]) != 0 )
      _wind_update( i ? (j>0 ? END_UPDATE : BEG_UPDATE) :
          (j>0 ? END_MCTRL : BEG_MCTRL) );
  return ret;
}

void _appl_write( int id, int count, int *buf )
{
  int old;
  
  old = un_update(0);
  appl_write( id, count, buf );
  un_update(old);
}

void newtop( int type, int hand, int id )
{
  int buf[8];

  if( id>=0 )
  {
    buf[0] = type;
    buf[2] = 0;
    buf[3] = hand;
    _appl_write( buf[1]=id, 16, buf );
  }
}

static void no_memory(void)
{
  form_error( IENSMEM );
}

static void draw_desk(void)
{
  redraw_window( 0, &desktop->working, 0 );
}

Window *find_window( int handle )
{
  Window *wind;

  if( (wind=desktop) == 0L ) return 0L;
  do
    if( wind->handle == handle ) return(wind);
  while( (wind=wind->next) != 0 );
  return(0L);
}


void get_min( Window *wind )
{
  int type=wind->type, xtype=wind->xtype, w=cel_w-1, h=cel_h-1, has_v, has_h;
  Rect r = { 0, 0, 0, 0 }, out;

  /* min inner size, considering just scroll bars first */
  has_v = type & (VSLIDE|UPARROW|DNARROW) || type&SIZER && xtype&X_HSPLIT;
  has_h = type & (HSLIDE|LFARROW|RTARROW) || type&SIZER && xtype&X_VSPLIT;
  if( !(wind->treeflag&X_WTFL_MAXIMIZE) )
  {
    if( type&SIZER || has_h || has_v )
    {
      r.w += w;
      r.h += h;
    }
  }
  else if( type&SIZER )
    if( !has_v && has_h ) r.w += w;
    else if( has_v && !has_h ) r.h += h;
  if( (type&RTARROW) || type&(NAME|MOVER|CLOSER|FULLER|SMALLER) && type&FULLER ) r.w += w;
  if( type & (CLOSER|LFARROW) ) r.w += w;
  if( type & HSLIDE ) r.w += w;
  if( type & UPARROW ) r.h += h;
  if( type & VSLIDE ) r.h += h;
  if( type & DNARROW ) r.h += h;
  if( xtype & X_VSPLIT )
  {
    wind->vsp_min1 = wind->vsp_min2 = r.h;
    r.h += dflt_wind[WVSPLIT].ob_height-1;
  }
  if( xtype & X_HSPLIT )
  {
    wind->hsp_min1 = wind->hsp_min2 = r.w;
    r.w += dflt_wind[WHSPLIT].ob_width-1;
  }
  recalc_outer( r, &out, type, xtype );
  wind->min_w = out.w;
  wind->min_h = out.h;
}

static int check_split( int *split, int val, int min1, int min2, int max, int siz )
{
  int m1, m2;

  m1 = min1>>1;
  m2 = min2>>1;
  if( val!=-1 )
    if( max-siz+2 <= min1+min2 )
    {
      if( val ) val = -1;	/* don't change if already at 0 */
    }
    else if( val < m1 ) val = 0;
    else if( val < min1 ) val = min1;
    else if( val > max-siz-m2+2 ) val = -1;
    else if( val > max-siz-min2+2 ) val = max-siz-min2+2;
  if( val != *split )
  {
    *split = val;
    return 1;
  }
  return 0;
}

int both_splits( Window *wind )
{
  /* use logical OR, so both happen */
  return check_split( &wind->hsplit, wind->hsplit, wind->hsp_min1,
      wind->hsp_min2, wind->working.w, wind->tree[WHSPLIT].ob_width ) |
      check_split( &wind->vsplit, wind->vsplit, wind->vsp_min1,
      wind->vsp_min2, wind->working.h, wind->tree[WVSPLIT].ob_height );
}

int x_wind_create( int type, int xtype, int x, int y, int w, int h )
{
  Rect r;
  
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return create_window( type, xtype, &r );
}

int create_window( int type, int xtype, Rect *r )
{
  Window *wind, *new;
  int h, *old=0L;

  wind = desktop;
  while( wind->next ) wind = wind->next;
  if( wind_app && wind_app->flags.flags.s.limit_handles )
  {
    for( old=wind_app->old_handles, h=0; *old && ++h<=7; old++ );
    if( h>7 ) return -1;
  }
  if( (h = wind_create( 0, Xrect(*r) )) >= 0 )
    if( (new = (Window *)lalloc( sizeof(Window), -1 )) == 0 )
    {
      no_memory();
      return(-1);
    }
    else
    {
      if( old ) *old = h;
      memset( new, 0, sizeof(Window) );
      wind->next = new;
      new->handle=h;
      new->tree = dflt_wind;
      new->treecnt = TREECNT;
      new->apid = curapp->id;
      new->outer = new->full = *r;
      new->dirty = norect;
      new->prev = new->outer;
      new->type = type;
      new->xtype = xtype;
      new->top_bar = "";
      new->place = new->hslide = new->hslidesz = new->vslide =
          new->vslidesz = new->treeflag = new->hsplit = new->vsplit = -1;
      if( !curapp->flags.flags.s.maximize_wind )
          new->treeflag &= ~X_WTFL_MAXIMIZE;
      new->dial_swid = new->dial_sht = 1;
      memcpy( new->colors, settings->dwcolors[settings->wcolor_mode], sizeof(new->colors) ); /* 003 */
      recalc_window( new->handle, new, -1L );
      recalc_inner( new->outer, &new->working, type, xtype );
      get_min( new );
      both_splits(new);
      new->max_w = desktop->outer.w;
      new->max_h = desktop->outer.h;
    }
  else DEBUGGER(WNCREATE,NOHAND,MAX_HANDLE);
  return h;  /* -1 if none */
}

void do_wclip(void)
{
  _vs_clip( 1, &wclip_rec );
}

#pragma warn -par
void wded_clip_ini( Rect *r )
{
  _v_mouse(0);	/* 003 */
}
#pragma warn +par

int wded_clip(void)
{
  if( wclip_rec.w )
  {
    do_wclip();
    wclip_rec.w = 0;
    return 1;
  }
  return 0;
}

void wd_edit( Window *w, Rect *r )
{
  if( w && w->dialog && w->place == place && w->treeflag&X_WTFL_ACTIVE )
  {
    if( r )
    {
      wclip_rec = *r;
      clip_ini = wded_clip_ini;
      clip_it = wded_clip;
    }
    if( w->dial_edit>0 ) objc_edit( w->dialog, w->dial_edit, 0, &w->dial_edind, ED_END );
    else if( w->dial_edit<0 && w->dial_obj != 0 )
    {
      objc_edit( w->dialog, w->dial_edit=w->dial_obj, 0, &w->dial_edind, ED_INIT );
      w->dial_obj = 0;
    }
    if( r ) reset_clip();
  }
}

int _wind_open( int handle, int x, int y, int w, int h )
{
  Window *wind;

  if( (wind = find_window(handle)) == 0 )
  {
    DEBUGGER(WINDOP,INVHAND,handle);
    return 0;
  }
  if( wind==desktop )
  {
    DEBUGGER(WINDOP,OPDESK,0);
    return 0;
  }
  if( wind->place >= 0 )
  {
    DEBUGGER(WINDOP,ALROPEN,handle);
    return(0);
  }
  if( wind->apid>=0 && wind->apid != curapp->id && wind->dial_obj != IS_TEAR )
  {
    DEBUGGER(WINDOP,NOTOWN,handle);
    return(0);
  }
  wind->outer.x = x;
  wind->outer.y = y;
  wind->outer.w = w;
  wind->outer.h = h;
  opn_wind(wind);
  return(1);
}

void opn_wind( Window *wind )
{
  Window *top;
  APP *ap;

  get_top();
  top = top_wind;
/*%  if( top_wind && !no_top && top_wind->handle )
      newtop( WM_UNTOPPED, top->handle, top->apid ); */
  no_top = 0;
  top_wind = wind;
  wd_edit(top,0L);
  wind->place = ++place;
  if( top ) all_gadgets( top->place );
  recalc_window( wind->handle, wind, -1L );
  recalc_inner( wind->outer, &wind->working, wind->type, wind->xtype );
  dial_sliders( wind, 0 );
  wind_open( wind->handle, Xrect(wind->outer) );
/*  _gad_redraw( wind->handle, 0L, 0 );	/* 003 */*/
  redraw_window( wind->handle, &wind->outer, 0 );	/* 003: instead */
  wind->dirty = norect;
  wind->chop_redraw = 1;
}

void msg_redraw( Window *w, int *buf )
{
  buf[0] = WM_REDRAW;
  buf[1] = w->apid;
  buf[2] = 0;
  buf[3] = w->handle;
  *(Rect *)&buf[4] = w->dirty;
  w->dirty = norect;
}

int _wind_close( int handle )
{
  Window *wind, *w, *top=0L;
  int pl, was_top;
  APP *ap, *ap2;

  if( (wind = find_window(handle)) == 0 )
  {
    DEBUGGER(WNCLOSE,INVHAND,handle);
    return 0;
  }
  if( wind==desktop )
  {
    DEBUGGER(WNCLOSE,CLDESK,0);
    return 0;
  }
  if( (pl=wind->place) < 0 )
  {
    DEBUGGER(WNCLOSE,NOTOPEN,handle);
    return(0);
  }
  if( wind->apid>=0 && wind->apid != curapp->id )
  {
    DEBUGGER(WNCLOSE,NOTOWN,handle);
    return(0);
  }
  free_rects(wind);
  ap2 = curapp;         /* only current app can own window */
  wind->place = -1;     /* so that redraw_wind() in appl_write()
                           will fail below */
  if( wind->dirty.w )
  {                     /* MultiDesk compatibility */
    int buf[8];
    msg_redraw( wind, buf );
    _appl_write( wind->apid, 16, buf );
  }
  w = desktop;
  ap2->has_wind=0;
  get_top();
  was_top = top_wind==wind;
  do
    if( w!=wind )
    {
/*%      if( was_top && w->place == place-1 )
      {
        top = w;
        for( ap=app0; ap; ap=ap->next )
          if( ap->id == w->apid )
          {
            if( w->place>0 && (no_top = ap->menu && has_menu != ap) == 0 )
            {
              newtop( WM_ONTOP, w->handle, ap->id );
              if( ap->id != wind->apid ) switch_mouse(ap,0);
            }
            break;
          }
      }*/
      if( ap2->id == w->apid ) ap2->has_wind=1;
      if( w->place > pl ) w->place--;   /* above window being closed */
/*%      else redraw_window( w->handle, &wind->outer, 1 ); */
    }
  while( (w = w->next) != 0 );
  if( --place < 0 ) place = 0;	/* just in case; should never happen */
  if( was_top )
  {
/*%    if( !no_top ) all_gadgets( place ); */
    if( top ) wd_edit(top_wind=top,0L);
    else top_wind = desktop;
  }
  for( w=desktop->next; w; w=w->next )
    if( w->tear_parent==wind ) close_del(w);
  wind_close(handle);
  get_top();
  if( top_wind ) all_gadgets( top_wind->place );
  return(1);
}

void redraw_all( Rect *r )
{
  Window *w;

  w = desktop;
  do
    redraw_window( w->handle, r, 1 );
  while( (w = w->next) != 0 );
}

void delete_wind( Window *wind )
{
  Window *w;
  int h, handle;
  
  handle = wind->handle;
  w = desktop;
  do
    if( w->next == wind )
    {
      w->next = wind->next;
      break;
    }
  while( (w = w->next) != 0 );
  if( wind_app && wind_app->flags.flags.s.limit_handles )
    for( h=0; h<7; h++ )
      if( wind_app->old_handles[h]==handle ) wind_app->old_handles[h]=0;
  free_rects(wind);
  lfree(wind);
}

int _wind_delete( int handle )
{
  Window *wind;

  if( (wind = find_window(handle)) == 0 )
  {
    DEBUGGER(WNDEL,INVHAND,handle);
    return 0;
  }
  if( wind==desktop )
  {
    DEBUGGER(WNDEL,DELDESK,0);
    return 0;
  }
  if( wind->place>=0 )
  {
    DEBUGGER(WNDEL,DECLOSE,handle);
    wind_app = 0L;
    _wind_close(handle);       /* del w/o close */
  }
  if( wind->apid>=0 && wind->apid != curapp->id )
  {
    DEBUGGER(WNDEL,NOTOWN,handle);
    return(0);
  }
  delete_wind( wind );
  wind_delete(handle);
  return(1);
}

void find_oldup(void)
{
  APP *ap;

  if( curapp->update.i == 0 )
    if( curapp == has_update )
    {
      has_update = curapp->old_update;
      curapp->old_update = 0L;
    }
    else
    {
      for( ap=app0; ap; ap=ap->next )
        if( ap->old_update == curapp )
        {
          ap->old_update = curapp->old_update;
          break;
        }
      curapp->old_update = 0L;
    }
}

void set_upid(void)
{
  APP *ap;

  if( has_update != curapp )
  {
    for( ap=has_update; ap; ap=ap->old_update )
      if( ap->old_update == curapp )
      {
        ap->old_update = curapp->old_update;
        break;
      }
    curapp->old_update = has_update;
    has_update = curapp;
  }
}

void set_update(void)
{
  update.c[0] = cnt_update.c[0] > 0;
  update.c[1] = cnt_update.c[1] > 0;
}

int _wind_update( int flag )
{
  if( flag&0x100 && update.i && has_update!=curapp ) return 0;
  wind_update(flag);
  switch( flag&0xff )
  {
    case BEG_UPDATE:
      cnt_update.c[1]++;
      curapp->update.c[1]++;
      set_upid();
      break;
    case END_UPDATE:
      cnt_update.c[1]--;
      curapp->update.c[1]--;
      find_oldup();
      break;
    case BEG_MCTRL:
      cnt_update.c[0]++;
      curapp->update.c[0]++;
      set_upid();
      break;
    case END_MCTRL:
      cnt_update.c[0]--;
      curapp->update.c[0]--;
      find_oldup();
      break;
    default:
      DEBUGGER(WUPDATE,UNKTYPE,flag);
      return(0);
  }
  set_update();
  return(1);
}

void free_rects( Window *wind )
{
  Rect_list *r, *rn;

  if( (r = wind->rects) == 0L || update.i && wind->rectptr ) return;
  do
  {
    rn = r->next;
    lfree(r);
  }
  while( (r=rn) != 0 );
  wind->rects = wind->rectptr = 0L;
}

void wrecalc( Rect *r, int type, int xtype, int sign )
{
  int h, has_v, has_h;
  char max;

  h = sign*(cel_h-1);
  max = curapp->flags.flags.s.maximize_wind;
  if( !max ) has_h = has_v =
      type & (VSLIDE|UPARROW|DNARROW|HSLIDE|LFARROW|RTARROW|SIZER);
  else
  {
    has_v = type & (VSLIDE|UPARROW|DNARROW);
    has_h = type & (HSLIDE|LFARROW|RTARROW);
  }
  if( has_v || type&SIZER && (!has_h || xtype&X_HSPLIT) ) r->w += sign*(cel_w-1);
  if( has_h || type&SIZER && (!has_v || xtype&X_VSPLIT) ) r->h += h;
  if( type & (NAME|MOVER|CLOSER|FULLER|SMALLER) )
  {
    r->y -= h;
    r->h += h;
  }
  if( type & INFO )
  {
    r->y -= h;
    r->h += h;
  }
  if( xtype & X_MENU )
  {
    r->y -= (h=sign*(menu_h-1));
    r->h += h;
  }
}

void recalc_outer( Rect inner, Rect *outer, int type, int xtype )
{
  *outer = inner;
  outer->x--;
  outer->w += 2;
  outer->y--;
  outer->h += 2;
  wrecalc( outer, type, xtype, 1 );
}

void recalc_inner( Rect outer, Rect *inner, int type, int xtype )
{
  *inner = outer;
  inner->x++;
  inner->w -= 2;
  inner->y++;
  inner->h -= 2;
  wrecalc( inner, type, xtype, -1 );
}

int x_wind_calc( int type, int kind, int xkind, int inx, int iny,
    int inw, int inh, int *outx, int *outy, int *outw, int *outh )
{
  Rect r, out;
  
  r.x = inx;
  r.y = iny;
  r.w = inw;
  r.h = inh;
  if( type ) recalc_inner( r, &out, kind, xkind );
  else recalc_outer( r, &out, kind, xkind );
  *outx = out.x;
  *outy = out.y;
  *outw = out.w;
  *outh = out.h;
  return(1);
}

void cond_redraw( Window *w, int num, unsigned int rnum, int flag )
{
  Rect r;
  int i, j, nx, ny;

  if( w->place > 0 )
  {
    wind_update( BEG_UPDATE );
    regenerate_rects(w);
    if( w->tree!=dflt_wind ) flag = 0;
    if( flag ) objc_offset( w->tree, ++num, &r.x, &r.y ); /* old slider coords */
    recalc_window( w->handle, w, (long)rnum );
    /* avoid redraw problem with Neo 4 and real-time scrolling */
    if( flag )
    {
      objc_offset( w->tree, num, &nx, &ny );       /* new coords */
      if( nx != r.x || ny != r.y )
      {
        r.w = w->tree[num].ob_width+1;
        r.h = w->tree[num].ob_height+1;
        if( num==WHSMLSL ) r.w += abs(nx-r.x);
        else r.h += abs(ny-r.y);
        if( flag<0 )
        {
          r.x = nx;
          r.y = ny;
        }
        gredraw_obj( w, num-1, &r );
      }
    }
    else gredraw_obj( w, num, 0L );
    free_rects(w);
    wind_update( END_UPDATE );
  }
}

void redraw_info( Window *w )
{
  recalc_window( w->handle, w, INFO );
  _v_mouse(0);	/* 003 */
  gredraw_obj( w, WILEFT, 0L );
  gredraw_obj( w, WINFO, 0L );
  gredraw_obj( w, WIRT, 0L );
  _v_mouse(1);	/* 003 */
}

void draw_wmenu( Window *w )
{
  if( w!=desktop && w->menu )
  {
    _v_mouse(0);	/* 003 */
    recalc_window( w->handle, w, (long)X_MENU<<8 );
    gredraw_obj( w, WMNLEFT, 0L );
    gredraw_obj( w, WMENU, 0L );
    gredraw_obj( w, WMNRT, 0L );
    _v_mouse(1);	/* 003 */
  }
}

int check_slider( Window *w, int *i, int val )
{
  int r;

  recalc_window( w->handle, w, 0L );
  if( val < -1 ) val = -1;
  else if( val>1000 ) val = 1000;
  if( *i != val )
  {
    r = *i<val ? 1 : -1;
    *i = val;
    return(r);
  }
  return(0);
}

void _wind_set( Window *w, int item, int num )
{
  _set_window( w->handle, item, num );
}

void dial_sliders( Window *w, int draw )
{
  int i;

  if( w->dialog && w->treeflag&X_WTFL_SLIDERS )
  {
    if( w->type&HSLIDE )
    {
      _wind_set( w, WF_HSLSIZE, w->working.w*1000L/w->dialog[0].ob_width );
      if( (i = w->dialog[0].ob_x+w->dialog[0].ob_width - (w->working.x+w->working.w)) < 0 )
        if( (w->dialog[0].ob_x -= i) < w->working.x ) w->dialog[0].ob_x = w->working.x;
      if( (i=w->dialog[0].ob_width-w->working.w) <= 0 ) i = 0;
      else i = (w->working.x-w->dialog[0].ob_x)*1000L / i;
      if( draw ) _wind_set( w, WF_HSLIDE, i );
      else w->hslide = i;
    }
    if( w->type&VSLIDE )
    {
      _wind_set( w, WF_VSLSIZE, w->working.h*1000L/w->dialog[0].ob_height );
      if( (i = w->dialog[0].ob_y+w->dialog[0].ob_height - (w->working.y+w->working.h)) < 0 )
        if( (w->dialog[0].ob_y -= i) < w->working.y ) w->dialog[0].ob_y = w->working.y;
      if( (i=w->dialog[0].ob_height-w->working.h) <= 0 ) i = 0;
      else i = (w->working.y-w->dialog[0].ob_y)*1000L / i;
      if( draw ) _wind_set( w, WF_VSLIDE, i );
      else w->vslide = i;
    }
  }
}

void set_desk( APP *ap )
{
  desktop->apid = ap->id;
  desktop->tree = ap->desk;
  desk_obj = ap->desk_obj;
  if( !place )
  {
    if( has_desk ) has_desk->has_wind = 0;
    ap->has_wind = 1;
  }
  has_desk = ap;
}

void no_desk_own(void)
{
  if( !place && has_desk ) has_desk->has_wind = 0;
  has_desk = 0L;
}

void new_desk( int mode, APP *ap )
{       /* mode:  -1: draw if different  0: no draw  1: draw if new is 0L */
  OBJECT *o;
  APP *ap2, *ap3;
  int flag;

  o = desktop->tree;
  if( ap && ap->desk ) set_desk(ap);
  else if( has_menu && has_menu->desk ) set_desk(has_menu);
  else if( !has_desk || !has_desk->desk || has_desk->asleep )
  {
    if( has_desk && !has_desk->asleep ) set_desk( ap2=has_desk );	/* 003: added sleep checks */
    else if( has_menu ) set_desk( ap2=has_menu );
    else
    {
      no_desk_own();
      if( (ap2 = app0) == 0 ) return;
    }
    desktop->tree = dflt_desk;
    desk_obj = 0;
    for( ap3=0L, ap=ap2->next; ap!=ap2; )
      if( !ap ) ap=app0;
      else if( ap->desk && !ap->asleep )
      {
        set_desk(ap);
        break;
      }
      else
      {
        if( !ap3 && !ap->asleep ) ap3=ap;
        ap=ap->next;
      }
    if( (!has_desk || has_desk->asleep/*003*/) && ap3 ) set_desk(ap3);
  }
  if( mode>0 && desktop->tree==dflt_desk ||
      mode<0 && o != desktop->tree )
  {
    flag=0;
    if( !curapp )
    {
      curapp = app0;
      flag++;
    }
    draw_desk();
    if( flag ) curapp = 0L;
  }
}

void set_dfltdesk( OBJECT *tree, int count )
{
  char was_mine;

  was_mine = desktop->tree == dflt_desk;
  if( (dflt_desk = tree) == 0 )
  {
    dflt_desk = dflt_desk0;
    dtree_cnt = DTREECNT;
  }
  else dtree_cnt = count;
  if( was_mine ) desktop->tree = dflt_desk;
}

void change_if( int *i, int num )
{
  if( num!=-1 ) *i = num;
}

int next_iconify( int *out )
{
  Window *w;
  unsigned int i, j;
  int x, y;
  
  for( i=1; i; i++ )
  {
    for( w=desktop; (w=w->next) != 0; )
      if( w->icon_index == i ) break;
    if( !w ) break;
  }
  if( out )
  {
    j = i-1;	/* 003 */
    x = desktop->working.w/(out[2]=ICON_WID);
    y = desktop->working.h/(out[3]=ICON_HT);
    out[0] = (j%x)*ICON_WID;	/* 003: was /x */
    out[1] = desktop->working.y+desktop->working.h - ICON_HT -
       (j%(x*y))/x*ICON_HT;	/* 003: added *ICON_HT */
  }
  return i;
}

char wcol_xref[] = { W_BOX, W_CLOSER, W_NAME, W_FULLER, W_FULLER, W_FULLER,
      W_INFO, W_INFO, W_INFO, -1/*tool*/, W_INFO, W_INFO/*menu*/, W_INFO, W_UPARROW,
      W_VSLIDE, W_VELEV, W_DNARROW, -1/*split*/, W_UPARROW, W_VSLIDE, W_VELEV,
      W_DNARROW, W_LFARROW, W_HSLIDE, W_HELEV, W_RTARROW, -1,
      W_LFARROW, W_HSLIDE, W_HELEV, W_RTARROW, W_SIZER };

void change_oldcol( int colors[2][WGSIZE+1], int num, int i1, int i0 )
{
  int i;

  for( i=0; i<=WGSIZE; i++ )
    if( wcol_xref[i] == num )
    {
      change_if( &colors[1][i], i1 );
      change_if( &colors[0][i], i0 );
    }
}

int _set_window( int handle, int field, ... )
{
  static int set_window( SWT *t );
  SWT t;
  
  t.handle = handle;
  t.change = field;
  t.i1 = *(int *)&...;
  t.i2 = *((int *)&...+1);
  t.i3 = *((int *)&...+2);
  t.i4 = *((int *)&...+3);
  wind_app = 0L;
  return set_window(&t);
}

static int set_window( SWT *t )
{
  Window *wind, *w, *w2;
  int pl, handle, i, buf[8];
  Rect r, r2;
  APP *ap;

  handle = t->handle;
  wind = find_window(handle);
  if( t->change!=WF_NEWDESK && t->change!=WF_DCOLOR &&
      t->change!=X_WF_DFLTDESK/*003*/ && t->change!=X_WF_DCOLSTAT/*003*/ )
  {
    if( wind==desktop )
    {
      DEBUGGER(WNSET,SETDESK,t->change);
      return 0;
    }
    if( !wind )
    {
      DEBUGGER(WNSET,INVHAND,handle);
      return 0;
    }
  }
  switch( t->change )
  {
    case WF_CURRXYWH:
new_xywh:
      wind->prev = wind->outer;
      r = *(Rect *)&t->i1;
      if( wind->dialog )
      {
        wind->dialog[0].ob_x += r.x - wind->outer.x;
        wind->dialog[0].ob_y += r.y - wind->outer.y;
      }
      if( t->change!=WF_CURRXYWH ||
          *(long *)&r != *(long *)&wind->outer ||
          *((long *)&r+1) != *((long *)&wind->outer+1) )
      {
        wind->outer = r;
        recalc_inner( r, &wind->working, wind->type, wind->xtype );
        if( wind->place>=0 )
        {
          dial_sliders( wind, 1 );
          both_splits(wind);
          recalc_window( handle, wind, -1L );
/*%          pl=0;
          r = wind->prev;
          r.w += 2;
          r.h += 2;
          /* always redraw whole thing if WM_(UN)ICONIFY */
          if( t->change==WF_CURRXYWH && *(long *)&wind->prev.w == *(long *)&wind->outer.w &&
              wind->place==place && gintersect( desktop->working, r, &r2 ) &&
              *(long *)&r == *(long *)&r2 &&
              *((long *)&r+1) == *((long *)&r2+1) && wind->outer.x>=0 )
          {
            blit(wind);
            pl=1;
          }
          else if( *(long *)&r == *(long *)&wind->outer &&
              wind->outer.w <= wind->prev.w && wind->outer.h <=
              wind->prev.h )
          {
            regenerate_rects(wind);
            gredraw_obj( wind, 0, 0L );
            free_rects(wind);
            pl=1;
          }
          redraw_window( 0, &wind->prev, 1 );
          for( w=desktop->next; w; w=w->next )
            if( w->place <= wind->place-pl ) redraw_window( w->handle,
                w==wind ? &wind->outer : &wind->prev, 1 ); */
          wind_set( handle, WF_CURRXYWH, Xrect(wind->outer) );
          if( *(long *)&wind->prev == *(long *)&wind->outer &&
              wind->outer.w <= wind->prev.w && wind->outer.h <=
              wind->prev.h )
          {
            wind_update( BEG_UPDATE );
            regenerate_rects(wind);
            gredraw_obj( wind, 0, 0L );
            wind_update( END_UPDATE );
            free_rects(wind);
          }
          else if( *(long *)&wind->prev.w != *(long *)&wind->outer.w )
          {	/* 002: if w or h changed, force redraw of whole thing */
            wind->dirty = wind->outer;
            msg_redraw( wind, buf );
            _appl_write( wind->apid, 16, buf );
          }
        }
      }
      break;
    case WF_FULLXYWH:
      wind->full = *(Rect *)&t->i1;
      break;
    case WF_WORKXYWH:
/*      wind->working = *va_arg( argpt, Rect * );
      wind->prev = wind->outer;
      recalc_outer( wind->working, &wind->outer, wind->type );
      recalc_window( handle, wind, -1L );*/
      return(0);
    case WF_NAME:
      wind->top_bar = *(char **)&t->i1 ? *(char **)&t->i1 : 0L;
      cond_redraw( wind, WMOVE, NAME, 0 );
      break;
    case WF_INFO:
      if( *(char **)&t->i1 )
      {
        strncpy( wind->info_bar, *(char **)&t->i1, INFO_LEN );
        wind->info_bar[INFO_LEN] = '\0';
      }
      else wind->info_bar[1] = wind->info_bar[0] = '\0';
      wind->info_pos = 0;
      if( wind->place > 0 )
      {
        regenerate_rects(wind);
        redraw_info(wind);
      }
      break;
    case WF_HSLIDE:
      if( (pl=check_slider( wind, &wind->hslide, t->i1 )) != 0 )
          cond_redraw( wind, WHBIGSL, HSLIDE, pl );
      break;
    case X_WF_HSLIDE2:
      if( (pl=check_slider( wind, &wind->hslide2, t->i1 )) != 0 )
          cond_redraw( wind, WHBIGSL2, HSLIDE, pl );
      break;
    case WF_VSLIDE:
      if( (pl=check_slider( wind, &wind->vslide, t->i1 )) != 0 )
          cond_redraw( wind, WVBIGSL, VSLIDE, pl );
      break;
    case X_WF_VSLIDE2:
      if( (pl=check_slider( wind, &wind->vslide2, t->i1 )) != 0 )
          cond_redraw( wind, WVBIGSL2, VSLIDE, pl );
      break;
    case WF_HSLSIZE:
      if( check_slider( wind, &wind->hslidesz, t->i1 ) )
          cond_redraw( wind, WHBIGSL, HSLIDE, 0 );
      break;
    case X_WF_HSLSIZE2:
      if( check_slider( wind, &wind->hslidesz2, t->i1 ) )
          cond_redraw( wind, WHBIGSL2, HSLIDE, 0 );
      break;
    case WF_VSLSIZE:
      if( check_slider( wind, &wind->vslidesz, t->i1 ) )
          cond_redraw( wind, WVBIGSL, VSLIDE, 0 );
      break;
    case X_WF_VSLSIZE2:
      if( check_slider( wind, &wind->vslidesz2, t->i1 ) )
          cond_redraw( wind, WVBIGSL2, VSLIDE, 0 );
      break;
    case WF_TOP:
      get_top();
      w = top_wind;
      wind_set( handle, WF_TOP, handle );
      top_wind = wind;
      i = wind->place;
      if( w && w != wind )
      {
        all_gadgets( w->place );
        w = desktop;	/* added next 5 lines for 002 */
        while( (w=w->next) != 0 )
        {
          if( w->place == place ) wd_edit(w,0L);
          if( w->place >= i ) --w->place;
        }
        wind->place = place;
        all_gadgets( place );
      }
      else all_gadgets(i);
      break;
    case WF_NEWDESK:
      wind_set( handle, WF_NEWDESK, t->i1, t->i2, t->i3 );
      break;
    case WF_COLOR:
      if( wind!=desktop )
      {
        change_oldcol( wind->colors, t->i1, t->i2, t->i3 );
        if( last_handle==wind->handle ) last_handle = -1;	/* 003 */
        break;
      }
    case WF_DCOLOR:
      change_oldcol( settings->dwcolors[settings->wcolor_mode], t->i1, t->i2, t->i3 );
      last_handle = -1;	/* 003 */
      break;
    case X_WF_DCOLSTAT:	/* 003 */
      change_if( &settings->dwcolors[i=settings->wcolor_mode][1][t->i1], t->i2 );
      change_if( &settings->dwcolors[i][0][t->i1], t->i3 );
      for( wind=desktop; (wind=wind->next)!=0; )         /* 003 */
      {
        change_if( &wind->colors[1][t->i1], t->i2 );
        change_if( &wind->colors[0][t->i1], t->i3 );
      }
      change_if( (int *)&dflt_wind[t->i1].ob_state, t->i4 );
      change_if( &settings->wstates[i][t->i1], t->i4 );
      last_handle = -1;	/* 003 */
      break;
    case WF_BEVENT:
      wind->bevent = t->i1;
      if( _aes_ver >= 0x400 ) wind_set( handle, WF_BEVENT, t->i1, t->i2, t->i3 );
      break;
/*%    case WF_BOTTOM:
      if( wind->place>0 && place>1 )
      {
        pl = wind->place;
        for( w=desktop->next; w; w=w->next )
        {
          if( w->place<pl && w->place>0 ) w->place++;
          if( w->place==place ) w2=w;
        }
        all_gadgets(wind->place=1);
        if( pl==w2->place ) all_gadgets(pl);
        for( w=desktop->next; w; w=w->next )
          if( w->place<=pl && w!=wind ) redraw_window( w->handle,
              &wind->outer, 1 );
        for( ap=app0; ap; ap=ap->next )
          if( ap->id==w2->apid )
          {
            newtop( WM_ONTOP, w2->handle, ap->id );
            /* need to set no_top here, too */
          }
        top_wind = w2;
      }
      break; */
    case WF_ICONIFY:
      if( wind->icon_index )
      {
        DEBUGGER(WNSET,ALRIC,wind->handle);
        return 0;
      }
      wind->icon_index = next_iconify(0L);
      wind->old_type = wind->type;
      wind->type = NAME|MOVER;
      wind->old_xtype = wind->xtype;
      wind->xtype = 0;
      wind->iconify = wind->outer;
      goto new_xywh;
    case WF_UNICONIFY:
      if( !wind->icon_index )
      {
        DEBUGGER(WNSET,NOTIC,wind->handle);
        return 0;
      }
      wind->icon_index = 0;
      wind->type = wind->old_type;
      wind->xtype = wind->old_xtype;
      goto new_xywh;
    case WF_UNICONIFYXYWH:
      wind->iconify = *(Rect *)&t->i1;
      break;
    case X_WF_MENU:
      if( (wind->menu = *(OBJECT **)&t->i1) != 0 )
      {
        *(char *)&wind->menu[0].ob_type = X_GRAYMENU;	/* 003 */
        set_equivs(wind);
      }
      wind->menu_tA = wind->menu_tZ = 0;
      if( wind->place >= 0 ) draw_wmenu(wind);
      break;
    case X_WF_DIALOG:
      wind->dial_obj = wind->dial_edit = 0;
      if( (wind->dialog = *(OBJECT **)&t->i1) != 0 )
      {
        if( !(wind->type&(HSLIDE|VSLIDE)) )
            *(Rect *)&(wind->dialog[0].ob_x) = *(Rect *)&wind->working;
        dial_sliders( wind, 1 );
        next_obj = 0;
        form_init( wind->dialog );
        wind->dial_obj = next_obj;
        wind->dial_edit = edit_obj;
        redraw_window( wind->handle, &wind->working, 0 );
      }
      break;
    case X_WF_DIALWID:
      wind->dial_swid = t->i1;
      break;
    case X_WF_DIALHT:
      wind->dial_sht = t->i1;
      break;
    case X_WF_DIALFLGS:
      i = wind->treeflag & ~(X_WTFL_ACTIVE|X_WTFL_BLITSCRL) |
          ((t->i1&3)<<3);
      if( (i&X_WTFL_ACTIVE) != (wind->treeflag&X_WTFL_ACTIVE) )
      {
        wind->treeflag |= X_WTFL_ACTIVE;
        wd_edit( wind, 0L );
      }
      wind->treeflag = i;
      break;
    case X_WF_DIALEDIT:
      if( wind->dialog )
      {
        if( t->i1>=0 )
        {
          init_win_dial( wind );
          if( edit_obj>0 ) objc_edit( wind->dialog, edit_obj, 0, &edit_idx, ED_END );
          edit_obj = t->i1;
          edit_idx = t->i2;
          if( edit_obj>0 ) objc_edit( wind->dialog, edit_obj, 0, &edit_idx,
              edit_idx>=0 ? ED_END : ED_INIT );
          next_obj = 0;
          exit_win_dial( wind );
        }
        break;
      }
      DEBUGGER(WNSET,NOTDIAL,wind->handle);
      return 0;
    case X_WF_DFLTDESK:
      set_dfltdesk( *(OBJECT **)&t->i1, t->i3 );
/*%      ddesk_app = *(OBJECT **)&t->i1!=0 ? curapp : 0L;*/
      break;
    case X_WF_MINMAX:
      change_if( &wind->min_w, t->i1 );
      change_if( &wind->min_h, t->i2 );
      change_if( &wind->max_w, t->i3 );
      change_if( &wind->max_h, t->i4 );
      break;
    case X_WF_SPLMIN:
      change_if( &wind->hsp_min1, t->i1 );
      change_if( &wind->hsp_min2, t->i2 );
      change_if( &wind->vsp_min1, t->i3 );
      change_if( &wind->vsp_min2, t->i4 );
      if( both_splits(wind) )
      {
        r = wind->working;
        r.h += cel_h;
        r.w += cel_w;
        goto draw;
      }
      break;
    case X_WF_HSPLIT:
      if( check_split( &wind->hsplit, t->i1, wind->hsp_min1,
          wind->hsp_min2, wind->working.w, wind->tree[WHSPLIT].ob_width ) )
      {
        r = wind->working;
        r.h += cel_h;
        goto draw;
      }
      break;
    case X_WF_VSPLIT:
      if( check_split( &wind->vsplit, t->i1, wind->vsp_min1,
          wind->vsp_min2, wind->working.h, wind->tree[WVSPLIT].ob_height ) )
      {
        r = wind->working;
        r.w += cel_w;
draw:   recalc_window( handle, wind, -1L );
        redraw_window( wind->handle, &r, 0 );
      }
      break;
    case X_WF_OBJHAND:
      wind->objhand = *(int cdecl(**)(int hand, int obj))&t->i1;
      break;
    default:
      DEBUGGER(WNSET,UNKTYPE,t->change);
      return 0;
  }
  return(1);
}

static int hide_if( OBJECT *tree, int truth, int idx )
{
  if( !truth ) tree[idx].ob_flags |= HIDETREE;
  else tree[idx].ob_flags &= ~HIDETREE;
  return truth;
}

void position_info( Window *w )
{
  int i, wid, t, cw, ch;
  char *ptr;
  TEDINFO *ted;

  if( !(w->treeflag&X_WTFL_SLIDERS) ) w->info_pos = w->info_end = 1;
  else
  {
    i = w->outer.w-2;
    if( w->info_pos ) i -= cel_w;
/*    w->info_end = strlen(w->info_bar+w->info_pos)*char_w > i;  003 */
    ptr = w->info_bar+w->info_pos;
    if( (t = (ted=w->tree[WINFO].ob_spec.tedinfo)->te_font) != SMALL && t != IBM )
    {
      ted_font( t, ted->te_junk1, ted->te_junk2, &cw, &ch );
      wid = prop_extent( t, ptr, &cw );
    }
    else wid = strlen(ptr)*char_w;
    w->info_end = wid > i;
  }
}

void position_menu( Window *w )
{
  int mx, i, wid, tail;
  OBJECT *m=w->menu;

  wid = w->outer.w;
  if( (i = w->menu_tA) <= 0 ) i = m[2].ob_head;
  else wid -= cel_w;
  tail = m[2].ob_tail;
  w->menu_tZ=-1;
  for(;;)
  {
    wid -= m[i].ob_width;
    if( wid < cel_w )
      if( i!=tail || wid < 0 ) break;
    w->menu_tZ=i;
    if( i==tail ) break;
    i = m[i].ob_next;
  }
  if( w->menu_tZ == tail ) w->menu_tZ=0;
  else if( w->menu_tZ<0 ) w->menu_tA = -1;
  else if( w->menu_tA<0 ) w->menu_tA = 0;
}

void calc_slid( int i, OBJECT *tree, int smlsl, int siz, int cel, int off, int pos )
{
  int *ip;

  if( (*(ip=&tree[smlsl].ob_width+off) = siz <= 0 ? cel :
      (long)siz*i/1000) < cel ) *ip=cel;
  *(ip-2) = (long)(i - *ip) * pos / 1000;
}

int *gad_color( OBJECT *tree, int i )
{
  int t, u;
  
  tree += i;
  u = is_xusrdef( 0L, tree );
  if( (t=tree->ob_type) == G_BOX || t==G_IBOX || t==G_BOXCHAR ||
      t==G_BUTTON )
    if(u) return (int *)&tree->ob_spec.userblk->ub_parm+1;  /* was ub_code in 003 */
    else return (int *)&tree->ob_spec+1;
  if( t==G_FTEXT || t==G_FBOXTEXT || t==G_TEXT || t==G_BOXTEXT )
    if(u) return &((TEDINFO *)(tree->ob_spec.userblk->ub_code))->te_color;
    else return &tree->ob_spec.tedinfo->te_color;
  return 0L;
}
void recalc_window( int handle, Window *wind, long flag )
{
  int type=wind->type, typex=wind->xtype, w, w_1, w_2, h, h_1, h_2, i,
      top, has_h, has_v, ch_1, cw_1, ybase, mx, mh_1, wsp_ht, wsp_wd, *iptr;
  char max;
  OBJECT *tree;
/*  static char cols[] = { W_BOX, W_CLOSER, -1, W_FULLER, W_FULLER, W_FULLER,
      W_INFO, -1, W_INFO, -1/*tool*/, W_INFO, W_INFO/*menu*/, W_INFO, W_UPARROW,
      W_VSLIDE, W_VELEV, W_DNARROW, -1/*split*/, W_UPARROW, W_VSLIDE, W_VELEV,
      W_DNARROW, W_LFARROW, W_HSLIDE, W_HELEV, W_RTARROW, -1,
      W_LFARROW, W_HSLIDE, W_HELEV, W_RTARROW, W_SIZER }; */
  int dummy;

  if( wind==desktop ) return;
  if( !(wind->treeflag&X_WTFL_RESIZE) ) return;
  max = wind->treeflag&X_WTFL_MAXIMIZE;
  tree = wind->tree;
  if( last_handle != handle || flag<0L ) /* new window or flag says force it */
  {
    flag = -1L;
    last_handle = handle;
    *(Rect *)&tree[0].ob_x = wind->outer;
  }
  else if( !flag ) return;
  top = wind==top_wind;
  if( flag==-1L )
    for( i=0; i<=WGSIZE/*003*/; i++ )
      if( i!=WGMOVE )
        if( (iptr = gad_color( tree, i )) != 0 ) *iptr = wind->colors[top][i];
  if( (iptr = gad_color( tree, WGMOVE )) != 0 )
  {
    *iptr = wind->colors[top][WGMOVE];
    iptr = &u_tedinfo(tree,WGMOVE)->te_font;
    if( *iptr==SMALL || *iptr==IBM ) *iptr = wind->icon_index ? SMALL : IBM;
  }
/* 003  if( flag==-1L )
    for( i=0; i<sizeof(cols); i++ )
      if( cols[i]>=0 )
        if( (iptr = gad_color( tree, i )) != 0 ) *iptr = wind->colors[top][cols[i]];
  if( (iptr = gad_color( tree, WGINFO )) != 0 ) *iptr = wind->colors[top][W_INFO];
  if( (iptr = gad_color( tree, WGMOVE )) != 0 )
  {
    *iptr = wind->colors[top][W_NAME];
    iptr = &tree[WGMOVE].ob_spec.tedinfo->te_font;
    if( *iptr==SMALL || *iptr==IBM ) *iptr = wind->icon_index ? SMALL : IBM;
  }
  /* 002 */
  for( i=WSIZE, iptr=settings->winob_state; --i>=0; )
  {
    ++tree;
    tree->ob_state = (tree->ob_state&SELECTED) | *iptr++;
  } */
  tree = wind->tree;
  if( !(type&VSLIDE) ) tree[WVBIGSL].ob_spec.obspec.fillpattern =
      tree[WVBIGSL2].ob_spec.obspec.fillpattern = 0;
  if( !(type&HSLIDE) ) tree[WHBIGSL].ob_spec.obspec.fillpattern =
      tree[WHBIGSL2].ob_spec.obspec.fillpattern = 0;
  w_2 = (w_1 = (w = wind->outer.w) - cel_w) - cel_w + 1;
  h_2 = (h_1 = (h = wind->outer.h) - cel_h) - cel_h + 1;
  top = type & (NAME|MOVER|CLOSER|FULLER|SMALLER);
  if( !max ) has_h = has_v = type&(HSLIDE|LFARROW|RTARROW|VSLIDE|UPARROW|DNARROW|SIZER);
  else
  {
    has_v = type&(VSLIDE|UPARROW|DNARROW);
    has_h = type&(HSLIDE|LFARROW|RTARROW);
  }
  if( type&SIZER )
  {
    if( typex&X_HSPLIT ) has_v = 1;
    if( typex&X_VSPLIT ) has_h = 1;
  }
  ch_1 = cel_h-1;
  cw_1 = cel_w-1;
  mh_1 = menu_h-1;
  ybase=0;
  if( top ) ybase = ch_1;
  if( type&INFO ) ybase += ch_1;
  if( typex&X_MENU ) ybase += mh_1;
  if( wind->tool ) ybase += wind->tool[0].ob_height;
  wsp_ht = typex&X_VSPLIT ? wind->tree[WVSPLIT].ob_height-1 : 0;
  wsp_wd = typex&X_HSPLIT ? wind->tree[WHSPLIT].ob_width-1 : 0;

  if( flag & (NAME|MOVER|CLOSER|FULLER|SMALLER) )	/* show send to back */
    if( hide_if( tree, top, WBACK ) ) tree[WBACK].ob_x = (type&FULLER) ? w_2 : w_1;

  if( flag & CLOSER ) hide_if( tree, type&CLOSER, WCLOSE );

  if( flag & SMALLER )
    if( hide_if( tree, type&SMALLER, WICONIZ ) )
        tree[WICONIZ].ob_x = ((type&FULLER) ? w_2-cel_w+1 : w_2);

  if( flag & FULLER )
    if( hide_if( tree, type&FULLER, WFULL ) ) tree[WFULL].ob_x = w_1;

  if( flag & (MOVER|NAME) )
    if( hide_if( tree, top, WMOVE ) )
    {
      tree[WMOVE].ob_x = (type&CLOSER) ? cw_1 : 0;
      tree[WMOVE].ob_width = w_1 - ((type&CLOSER) ? cw_1 : 0) -
          ((type&FULLER) ? cw_1 : 0) - ((type&SMALLER) ? cw_1 : 0) + 1;
    }

  if( flag & NAME ) tree[WMOVE].ob_spec.tedinfo->te_ptext = wind->top_bar;

  if( flag & INFO )
  {
    if( (i = type&INFO) != 0 ) position_info(wind);
    else wind->info_pos=wind->info_end=0;
    hide_if( tree, i && wind->info_pos>0, WILEFT );
    hide_if( tree, i && wind->info_end>0, WIRT );
    if( hide_if( tree, i, WINFO ) )
    {
      tree[WIRT].ob_y = tree[WILEFT].ob_y = tree[WINFO].ob_y =
          top ? ch_1 : 0;
      tree[WIRT].ob_x = w_1;
      tree[WINFO].ob_x = 0;
      i = w;
      if( wind->info_pos>0 )
      {
        i -= cel_w-1;
        tree[WINFO].ob_x += cel_w-1;
      }
      if( wind->info_end>0 ) i -= cel_w-1;
      tree[WINFO].ob_width = i;
      tree[WINFO].ob_spec.tedinfo->te_ptext = wind->info_bar +
          wind->info_pos;
    }
  }

  if( flag & ((long)X_MENU<<8) )
  {
    if( (i = (typex&X_MENU) && wind->menu) != 0 &&
        wind->menu[2].ob_width > w ) position_menu(wind);
    else wind->menu_tA=wind->menu_tZ=0;
    hide_if( tree, i && wind->menu_tA>0, WMNLEFT );
    hide_if( tree, i && wind->menu_tZ>0, WMNRT );
    if( hide_if( tree, i, WMENU ) )
    {
      tree[WMNRT].ob_y = tree[WMNLEFT].ob_y = tree[WMENU].ob_y = ybase-mh_1;
      tree[WMNRT].ob_x = w_1;
      tree[WMENU].ob_x = 0;
      i = w;
      if( wind->menu_tA>0 )
      {
        i -= cel_w-1;
        tree[WMENU].ob_x += cel_w-1;
      }
      if( wind->menu_tZ>0 ) i -= cel_w-1;
      tree[WMENU].ob_width = i;
      if( wind->menu )
      {
        wind->menu[0].ob_x = 0;
        if( (i = wind->menu_tA) <= 0 ) i = wind->menu[2].ob_head;
        objc_offset( wind->menu, i, &mx, &dummy );
        wind->menu[0].ob_x = wind->outer.x+tree[WMENU].ob_x-mx;
        wind->menu[0].ob_y = wind->outer.y+ybase-mh_1;
      }
    }
  }

  if( flag & SIZER )
  {
    if( !(type&SIZER) && (!max && has_h || (type&(LFARROW|HSLIDE|RTARROW)) &&
        (type&(UPARROW|VSLIDE|DNARROW))) )
    {
      hide_if( tree, 1, WSIZE );
      i = ' ';
    }
    else
    {
      hide_if( tree, type&SIZER, WSIZE );
      i = '';
    }
    tree[WSIZE].ob_spec.obspec.character = i;
    tree[WSIZE].ob_x = w_1;
    tree[WSIZE].ob_y = h_1;
  }

  if( flag & UPARROW )
  {
    hide_if( tree, (type&UPARROW)!=0 && wind->vsplit, WUP );
    tree[WUP2].ob_x = tree[WUP].ob_x = w_1;
    tree[WUP].ob_y = ybase + (!wind->vsplit?wsp_ht:0);
    if( hide_if( tree, (type&UPARROW) && (typex&X_VSPLIT) && wind->vsplit>=0, WUP2 ) )
        tree[WUP2].ob_y = ybase + wind->vsplit + wsp_ht;
  }

  if( flag & DNARROW )
  {
    hide_if( tree, (type&DNARROW)!=0 && wind->vsplit, WDOWN );
    tree[WDOWN2].ob_x = tree[WDOWN].ob_x = w_1;
    tree[WDOWN].ob_y = (max && !(type & (HSLIDE|RTARROW|LFARROW|SIZER)) ? h_1 :
        h_2) - ((typex&X_VSPLIT)!=0 && wind->vsplit<0 ? wsp_ht : 0);
    if( hide_if( tree, (type&DNARROW) && (typex&X_VSPLIT) && wind->vsplit>=0, WDOWN2 ) )
    {
      tree[WDOWN2].ob_y = tree[WDOWN].ob_y;
      tree[WDOWN].ob_y = ybase + wind->vsplit - ch_1;
    }
  }

  if( flag & VSLIDE )
  {
    hide_if( tree, type&VSLIDE && wind->vsplit, WVSMLSL );
    mx = wind->vsplit>=0 ? wind->vsplit : h-ybase-(has_h||type&SIZER ? cel_h :1)-wsp_ht;
    if( hide_if( tree, wind->vsplit!=0 && (has_v || type&SIZER && !has_h), WVBIGSL ) )
    {
      tree[WVBIGSL].ob_x = w_1;
      tree[WVBIGSL].ob_y = ybase + ((type&UPARROW) ? ch_1 : 0) + (!wind->vsplit ? wsp_ht : 0);
      calc_slid( tree[WVBIGSL].ob_height = mx + 1 -
          ((type&UPARROW) ? ch_1 : 0) -
          ((type&DNARROW) ? ch_1 : 0),
          tree, WVSMLSL, wind->vslidesz, cel_h, 1, wind->vslide );
    }
    hide_if( tree, i=(type&VSLIDE) && (typex&X_VSPLIT) && wind->vsplit>=0, WVSMLSL2 );
    if( hide_if( tree, i || has_v || type&SIZER && !has_h, WVBIGSL2 ) )
    {
      tree[WVBIGSL2].ob_x = w_1;
      tree[WVBIGSL2].ob_y = ybase + mx + wsp_ht + ((type&UPARROW) ? ch_1 : 0);
      calc_slid( tree[WVBIGSL2].ob_height = h - ybase -
          wsp_ht -
          mx -
          ((type&UPARROW) ? ch_1 : 0) -
          (!max || (type&(SIZER|LFARROW|RTARROW|HSLIDE)) ? ch_1 : 0) -
          ((type&DNARROW) ? ch_1 : 0),
          tree, WVSMLSL2, wind->vslidesz2, cel_h, 1, wind->vslide2 );
    }
  }

  if( flag & LFARROW )
  {
    hide_if( tree, (type&LFARROW)!=0 && wind->hsplit, WLEFT );
    tree[WLEFT2].ob_y = tree[WLEFT].ob_y = h_1;
    tree[WLEFT].ob_x = !wind->hsplit ? wsp_wd : 0;
    if( hide_if( tree, (type&LFARROW) && (typex&X_HSPLIT) && wind->hsplit>=0, WLEFT2 ) )
        tree[WLEFT2].ob_x = wsp_wd + wind->hsplit;
  }

  if( flag & RTARROW )
  {
    hide_if( tree, (type&RTARROW)!=0 && wind->hsplit, WRT );
    tree[WRT2].ob_y = tree[WRT].ob_y = h_1;
    tree[WRT].ob_x = (!max || (type&(SIZER|UPARROW|DNARROW|VSLIDE)) ? w_2 :
        w_1) - ((typex&X_HSPLIT)!=0 && wind->hsplit<0 ? wsp_wd : 0);
    if( hide_if( tree, (type&RTARROW) &&(typex&X_HSPLIT) && wind->hsplit>=0, WRT2 ) )
    {
      tree[WRT2].ob_x = tree[WRT].ob_x;
      tree[WRT].ob_x = wind->hsplit - cw_1;
    }
  }

  if( flag & HSLIDE )
  {
    hide_if( tree, type&HSLIDE && wind->hsplit, WHSMLSL );
    mx = wind->hsplit>=0 ? wind->hsplit : w-(has_v||type&SIZER ? cel_w :1)-wsp_wd;
    if( hide_if( tree, wind->hsplit!=0 && (has_h || type&SIZER && !has_v), WHBIGSL ) )
    {
      tree[WHBIGSL].ob_x = (!wind->hsplit ? wsp_wd : 0) + ((type&LFARROW) ? cw_1 : 0);
      tree[WHBIGSL].ob_y = h_1;
      calc_slid( tree[WHBIGSL].ob_width = mx + 1 -
          ((type&LFARROW) ? cw_1 : 0) -
          ((type&RTARROW) ? cw_1 : 0),
          tree, WHSMLSL, wind->hslidesz, cel_w, 0, wind->hslide );
    }
    hide_if( tree, i=(type&HSLIDE) && (typex&X_HSPLIT) && wind->hsplit>=0, WHSMLSL2 );
    if( hide_if( tree, i || has_h || type&SIZER && !has_v, WHBIGSL2 ) )
    {
      tree[WHBIGSL2].ob_y = h_1;
      tree[WHBIGSL2].ob_x = mx + wsp_wd + ((type&LFARROW) ? cw_1 : 0);
      calc_slid( tree[WHBIGSL2].ob_width =
          ((type&LFARROW) ? w_1+1 : w) -
          wsp_wd -
          mx -
          (!max || (type&(SIZER|UPARROW|DNARROW|VSLIDE)) ? cw_1 : 0) -
          ((type&RTARROW) ? cw_1 : 0),
          tree, WHSMLSL2, wind->hslidesz2, cel_w, 0, wind->hslide2 );
    }
  }

  if( flag & ((long)X_HSPLIT<<8) )
    if( hide_if( tree, typex&X_HSPLIT, WHSPLIT ) )
    {
      tree[WHSPLIT].ob_x = wind->hsplit>=0 ? wind->hsplit :
          (type&SIZER || has_v ? w_1 : w) - wsp_wd;
      tree[WHSPLIT].ob_height = h-(tree[WHSPLIT].ob_y=ybase);
    }

  if( flag & ((long)X_VSPLIT<<8) )
    if( hide_if( tree, typex&X_VSPLIT, WVSPLIT ) )
    {
      tree[WVSPLIT].ob_y = wind->vsplit>=0 ? ybase + wind->vsplit :
          (type&SIZER || has_h ? h_1 : h) - wsp_ht;
      tree[WVSPLIT].ob_width = w;
    }
}

void drw_win_menu( Window *w, Rect *r, int alts )
{
  int i, end;
  OBJECT *m = w->menu;

  if( !w->menu || !(w->xtype&X_MENU) ) return;
  if( (i=w->menu_tA) == 0 ) i = m[2].ob_head;
  if( (end=w->menu_tZ) == 0 ) end = m[2].ob_tail;
  else if( end<0 ) return;
  for(;;)
  {
    _objc_draw( (OBJECT2 *)m, menu_owner, i, 0, Xrect(*r) );
    if( alts ) drw_alt( m, -i, 0 );
    if( i==end ) break;
    i = m[i].ob_next;
  }
}

char add_err;

void add_rects( Rect *curr, Rect *add )
{
  int i;

  if( !curr->w || !curr->h )
  {
    *curr = *add;
    return;
  }
  if( (i=curr->x-add->x) > 0 )
  {
    curr->w += i;
    curr->x -= i;
  }
  if( (i=curr->y-add->y) > 0 )
  {
    curr->h += i;
    curr->y -= i;
  }
  if( (i=add->x+add->w-(curr->x+curr->w)) > 0 ) curr->w += i;
  if( (i=add->y+add->h-(curr->y+curr->h)) > 0 ) curr->h += i;
}

int make_rect( int i, Rect_list **r, int x, int y, int w, int h,
    Rect_list **root )
{
  Rect rect;
  Rect_list *l;

  rect.x = x;
  rect.y = y;
  if( (rect.w = w) == 0 || (rect.h = h) == 0 ) return(i);
  if( !i ) (*r)->r = rect;
  else if( (l = add_rect( *r, root )) != 0 ) (*r=l)->r = rect;
  return(++i);
}

/* necessary for wind_get */
int gen_rect( Rect *in, Rect_list **root )
{
  Rect rect1, rect2;
  Rect_list *r, *prev, *rn;
  int i;

      r = *root;
      prev = 0L;
      do
      {
        if( gintersect( *in, r->r, &rect2 ) )
          if( *(long *)&r->r == *(long *)&rect2 &&
              *((long *)&r->r+1) == *((long *)&rect2+1) )
          {
            rn = r->next;
            if( prev ) prev->next = rn;
            else *root = rn;
            lfree(r);
            r = rn;
          }
          else
          {
            rect1 = r->r;
            i = 0;
            if( rect2.x > rect1.x ) i = make_rect( i, &r, rect1.x, rect1.y,
                rect2.x-rect1.x, rect1.h, root );
            if( rect2.x+rect2.w < rect1.x+rect1.w ) i = make_rect( i, &r,
                rect2.x+rect2.w, rect1.y, rect1.x+rect1.w-rect2.x-rect2.w,
                rect1.h, root );
            if( rect2.y > rect1.y ) i = make_rect( i, &r, rect2.x, rect1.y, rect2.w,
                rect2.y-rect1.y, root );
            if( rect2.y+rect2.h < rect1.y+rect1.h ) i = make_rect( i, &r, rect2.x,
                rect2.y+rect2.h, rect2.w, rect1.y+rect1.h-rect2.y-rect2.h,
                root );
            prev = r;
            r = r->next;
            if( add_err )
            {
              add_err = 0;
              return 0;
            }
          }
        else
        {
          prev = r;
          r = r->next;
        }
      }
      while( r );
  return 1;
}

int redraw_window( int handle, Rect *ar, int add )
{
  Window *wind, *w;
  Rect rect, area;
  Rect_list *r;
  int obj;

  if( (wind = find_window(handle)) == 0 || wind->place<0 ) return(0);
  area = *ar;
  if(add)
  {
    area.w += 2;
    area.h += 2;
  }
  wind_update( BEG_UPDATE );		/* 002: added */
  regenerate_rects(wind);
  recalc_window( handle, wind, 0L );
  if( wind->tree && gintersect( desktop->outer, area, &area ) )
  {
    obj = wind==desktop ? desk_obj : 0;
    r = wind->rects;
    while( r )
    {
      if( gintersect( area, r->r, &rect ) )
      {
        _objc_draw( (OBJECT2 *)wind->tree, 0L, obj, 8, Xrect(rect) );
        drw_win_menu( wind, &rect, 0 );
        if( wind != desktop )
          if( gintersect( wind->working, rect, &rect ) )
            if( !wind->dialog ) add_rects( &wind->dirty, &rect );
            else
            {
              _objc_draw( (OBJECT2 *)wind->dialog, 0L, 0, 8, Xrect(rect) );
              wd_edit( wind, &rect );
            }
      }
      r = r->next;
    }
  }
  free_rects(wind);
  wind_update( END_UPDATE );		/* 002: added */
  return(1);
}

Rect_list *add_rect( Rect_list *r, Rect_list **root )
{
  Rect_list *ret;

  if( add_err ) return(0);
  if( (ret = (Rect_list *)lalloc( sizeof(Rect_list), -1 )) == 0L )
  {
    add_err++;
    no_memory();
  }
  else if(r)
  {
    ret->next = r->next;
    r->next = ret;
  }
  else
  {
    *root = ret;
    ret->next = 0L;
  }
  return(ret);
}

void regenerate_rects( Window *wind )
{
  Window *w;
  Rect_list **root, *r;
  Rect rect1;

  if( *(root = &wind->rects) != 0 && update.i && wind->rectptr ) return;
  free_rects(wind);
  r = 0L;
  wind_update( BEG_UPDATE );	/* 003 */
  wind_get( wind->handle, WF_FIRSTXYWH, &rect1.x, &rect1.y, &rect1.w, &rect1.h );
  while( rect1.w && rect1.h )
  {
    if( (r=add_rect( r, root )) != 0 ) r->r = rect1;
    wind_get( wind->handle, WF_NEXTXYWH, &rect1.x, &rect1.y, &rect1.w, &rect1.h );
  }
  wind_update( END_UPDATE );	/* 003 */
}

int gintersect( Rect r1, Rect r2, Rect *res )
{
  res->x = r1.x < r2.x ? r2.x : r1.x;
  res->y = r1.y < r2.y ? r2.y : r1.y;
  res->w = r1.x+r1.w < r2.x+r2.w ? r1.x+r1.w-res->x : r2.x+r2.w-res->x;
  res->h = r1.y+r1.h < r2.y+r2.h ? r1.y+r1.h-res->y : r2.y+r2.h-res->y;
  return( res->w > 0 && res->h > 0 );
}

/*%int _find_wind( int x, int y )
{
  Window *w;
  int pl=-1, h=-1;

  w = desktop;
  do
    if( x >= w->outer.x && x < w->outer.x+w->outer.w &&
        y >= w->outer.y && y < w->outer.y+w->outer.h &&
        w->place > pl )
    {
      pl = w->place;
      h = w->handle;
    }
  while( (w=w->next) != 0 );
  return h;
}*/

int x_wind_tree( int mode, WIND_TREE *wt )
{
  Window *w;
  int handle;

  handle = wt->handle;
  if( (w = find_window(handle)) == 0 )
  {
    DEBUGGER(XWTRE,INVHAND,handle);
    return 0;
  }
  switch( mode )
  {
    case X_WT_GETCNT:
      wt->count = w->treecnt;
      wt->flag = w->treeflag;
      break;
    case X_WT_READ:
      memcpy( wt->tree, w->tree, w->treecnt*sizeof(OBJECT) );
      if( w->tree == dflt_wind ) wt->tree[WINFO].ob_spec.tedinfo =
          wt->tree[WMOVE].ob_spec.tedinfo = 0L;
      break;
    case X_WT_SET:
      if( wt->count>=0 )
      {
        w->tree = wt->tree;
        w->treecnt = wt->count;
      }
      w->treeflag = wt->flag;
      break;
    default:
      DEBUGGER(XWTRE,UNKTYPE,mode);
      return 0;
  }
  return 1;
}

void get_split( Window *wind, int **i, int dir )
{
  int h, h1;

  h1 = (dir ? wind->working.h : wind->working.w) -
      (dir ? wind->tree[WVSPLIT].ob_height :
      wind->tree[WHSPLIT].ob_width) + 2;
  *(*i++) = (h = dir ? wind->vsplit : wind->hsplit);
  *(*i++) = !h ? 0 : (h>0 ? h : h1)-1;	/* added -1 for rel 3 */
  **i     = h>=0 ? h1-h-1 : 0;
}

void _get_split( Window *wind, int *buf, int dir )
{
  int *i[3];
  
  i[0] = buf;
  i[1] = buf+1;
  i[2] = buf+2;
  get_split( wind, i, dir );
}

void get_oldcol( int colors[2][WGSIZE+1], int num, int *out ) /* 003 */
{
  int i;

  for( i=0; i<=WGSIZE; i++ )
    if( wcol_xref[i] == num )
    {
      *out++ = colors[1][i];
      *out = colors[0][i];
      return;
    }
}

#define Args   *i, *(i+1), *(i+2), *(i+3)

int _wind_get( int handle, int type, ... )
{
  Window *wind, *w;
  int h, h1, pl, id;
  Rect *r, rect1;
  int **i = (int **)&...;
  OBJECT **o;
  long l;

  if( type==WF_SCREEN )
  {
    h = wind_get( handle, type, Args );
    if( !**(i+2) && !**(i+3) )
    {
      /* default is 1/4 screen */
      l = (long)desktop->outer.w * desktop->outer.h * vplanes >> 2;
      **(i+2) = l>>16;
      **(i+3) = l;
    }
    return h;
  }
  if( ((wind = find_window(handle)) == 0 || wind==desktop) /*&& type!=WF_DCOLOR 002 */ )
      return wind_get( handle, type, Args );
  switch( type )
  {
    case WF_CURRXYWH:
      r = &wind->outer;
      goto rec;
    case WF_PREVXYWH:
      r = &wind->prev;
      goto rec;
    case WF_FULLXYWH:
      r = &wind->full;
      goto rec;
    case WF_WORKXYWH:
      r = &wind->working;
rec:  *(*i++) = r->x;
      *(*i++) = r->y;
      *(*i++) = r->w;
      **i = r->h;
      break;
    case WF_FIRSTXYWH:
      if( wind->place>=0 && (wind!=desktop || curapp==has_desk) )
      {
        regenerate_rects( wind );
        recalc_window( handle, wind, -1L&~((long)X_MENU<<8) );
        if( wind->rects )
        {
          if( wind->xtype&X_VSPLIT )
          {
            objc_xywh( (long)wind->tree, WVSPLIT, &rect1 );
            if( !gen_rect( &rect1, &wind->rects ) ) goto norec;
          }
          if( wind->xtype&X_HSPLIT )
          {
            objc_xywh( (long)wind->tree, WHSPLIT, &rect1 );
            if( !gen_rect( &rect1, &wind->rects ) ) goto norec;
          }
        }
        wind->rectptr = wind->rects;
      }
      else
      {
norec:  r = &norect;
        goto rec;
      }
    case WF_NEXTXYWH:
      for(;;)
        if( !wind->rects || !wind->rectptr ) goto norec;
        else
        {
          r = &(wind->rectptr->r);
          wind->rectptr = wind->rectptr->next;
          if( gintersect( wind->working, *r, r ) ) goto rec;
        }
    case WF_HSLIDE:
      **i = wind->hslide;
      break;
    case WF_VSLIDE:
      **i = wind->vslide;
      break;
    case WF_HSLSIZE:
      **i = wind->hslidesz;
      break;
    case WF_VSLSIZE:
      **i = wind->vslidesz;
      break;
    case X_WF_HSPLIT:
      get_split( wind, i, 0 );
      break;
    case X_WF_VSPLIT:
      get_split( wind, i, 1 );
      break;
    case X_WF_HSLIDE2:
      **i = wind->hslide2;
      break;
    case X_WF_VSLIDE2:
      **i = wind->vslide2;
      break;
    case X_WF_HSLSIZE2:
      **i = wind->hslidesz2;
      break;
    case X_WF_VSLSIZE2:
      **i = wind->vslidesz2;
      break;
    case X_WF_SPLMIN:
      *(*i++) = wind->hsp_min1;
      *(*i++) = wind->hsp_min2;
      *(*i++) = wind->vsp_min1;
      **i     = wind->vsp_min2;
      break;
    case WF_TOP:
    case WF_NEWDESK:
    case WF_BOTTOM:
      return wind_get( handle, type, Args );
    case WF_COLOR:
      get_oldcol( wind->colors, **i, *(i+1) );
      break;
    case WF_DCOLOR:
      get_oldcol( settings->dwcolors[settings->wcolor_mode], **i, *(i+1) );
      break;
    case X_WF_DCOLSTAT:	/* 003 */
      h = *(*i++);
      *(*i++) = settings->dwcolors[settings->wcolor_mode][1][h];
      *(*i++) = settings->dwcolors[settings->wcolor_mode][0][h];
      **i = u_object(dflt_wind,h)->ob_state;
      break;
    case WF_OWNER:
      *(*i++) = wind->apid;
      *(*i++) = wind->place>=0;
      w = desktop;
      **i = **(i+1) = 0;
      do
        if( w->place == wind->place+1 ) **i = w->handle;
        else if( w->place == wind->place-1 ) **(i+1) = w->handle;
      while( (w=w->next) != 0 );
      break;
    case WF_BEVENT:
      **i = wind->bevent;
      break;
    case WF_ICONIFY:
      *(*i++) = wind->icon_index!=0;
      *(*i++) = ICON_WID;
      **i     = ICON_HT;
      break;
    case WF_UNICONIFY:
      r = &wind->iconify;
      goto rec;
    case X_WF_MENU:
      o = (OBJECT **)(((long)**i<<8)|**(i+1));
      *o = wind->menu;
      break;
    case X_WF_DIALOG:
      o = (OBJECT **)(((long)**i<<8)|**(i+1));
      *o = wind->dialog;
      break;
    case X_WF_DIALWID:
      **i = wind->dial_swid;
      break;
    case X_WF_DIALHT:
      **i = wind->dial_sht;
      break;
    case X_WF_DIALFLGS:
      **i = (wind->treeflag & (X_WTFL_ACTIVE|X_WTFL_BLITSCRL)) >> 3;
      break;
    case X_WF_MINMAX:
      *(*i++) = wind->min_w;
      *(*i++) = wind->min_h;
      *(*i++) = wind->max_w;
      **i     = wind->max_h;
      break;
    case X_WF_OBJHAND:
      o = (OBJECT **)(((long)**i<<8)|**(i+1));
      *(int cdecl (**)(int hand, int obj))o = wind->objhand;
      break;
    case X_WF_DIALEDIT:
      *(*i++) = wind->dial_edit;
      **i     = wind->dial_edind;
      break;
    default:
      DEBUGGER(WNGET,UNKTYPE,type);
      return 0;
  }
  return(1);
}

void _gad_redraw( int w_handle, Rect *r, int obj )
{
  Window *w;
  
  if( (w = find_window(w_handle)) != 0 )
  {
    recalc_window( w->handle, w, 0L );
    wind_update( BEG_UPDATE );
    regenerate_rects(w);
    gredraw_obj( w, obj, r );
    free_rects(w);	/* 002 */
    wind_update( END_UPDATE );
  }
}

void gredraw_obj( Window *w, int num, Rect *ir )
{
  Rect r, r2;
  Rect_list *p;

  if( w->tree[num].ob_flags&HIDETREE ) return;
  if( !ir )
  {
    objc_xywh( (long)w->tree, num, &r );
    r.w += 2;
    r.h += 2;
    ir = &r;
  }
  p = w->rects;
  _v_mouse(0);	/* 003 */
  while( p )
  {
    if( gintersect( *ir, p->r, &r2 ) )
    {
      _objc_draw( (OBJECT2 *)w->tree, 0L, num, 8, Xrect(r2) );
      if( !num || num==WMENU ) drw_win_menu( w, &r2, draw_menu_alts );
    }
    p = p->next;
  }
  _v_mouse(1);	/* 003 */
}

int still_pressed( Window *w, int num )
{
  Mouse m;
  register int i, last, first;
  OBJECT *o = &w->tree[num];

  if( (i=o->ob_flags)&SELECTABLE )
    if( i&TOUCHEXIT )
    {
      if( !(o->ob_state&SELECTED) )
      {
        o->ob_state |= SELECTED;
        gredraw_obj( w, num, 0L );
      }
    }
    else for( last=num, first=1;;)
    {
      mks_graf( &m, 1 );
      if( !(m.b&1) )
      {
        o->ob_state &= ~SELECTED;
        if( last==num ) gredraw_obj( w, num, 0L );
        return(last==num);
      }
      i = objc_find( w->tree, 0, 8, m.x, m.y );
      if( i!=last && (i==num || last==num) || first )
      {
        first=0;
        if( i != num ) o->ob_state &= ~SELECTED;
        else o->ob_state |= SELECTED;
        gredraw_obj( w, num, 0L );
        last = i;
      }
    }
  return(1);
}

void all_gadgets( int pl )
{
  Window *w;

  for( w=desktop->next; w; w=w->next )
    if( w->place == pl )
    {
      wind_update( BEG_UPDATE );
      recalc_window( w->handle, w, -1L );
      regenerate_rects(w);
      gredraw_obj( w, 0, 0L );
      free_rects(w);	/* 002 */
      wind_update( END_UPDATE );
      return;
    }
}

/*%void reset_butq(void)
{
  but_q[bq_last].x = g_mx;
  but_q[bq_last].y = g_my;
  but_q[bq_last].k = kbshift();
  bq_ptr=bq_last;
  unclick=not_taken=0;
}*/

int get_int( unsigned char *buf )
{
  return (*buf<<8) | *(buf+1);
}

long get_long( unsigned char *buf )
{
  int i;

  i = get_int(buf);
  return ((long)i<<16) | get_int(buf+2);
}

/*%void set_butq(void)
{
  unclick = new_un;
  bq_ptr = new_ptr;
  cur_last = bq_last;
  not_taken=0;
}*/

void gadget_off(void)
{
  Window *w;
  unsigned int *i;

  if( last_gadget )
  {
    if( (w = find_window( last_gad_w ))->place >= 0 &&
        *(i=&w->tree[last_gadget].ob_state)&SELECTED )
    {
      recalc_window( last_gad_w, w, 0L );
      wind_update( BEG_UPDATE );
      regenerate_rects(w);
      *i &= ~SELECTED;
      gredraw_obj( w, last_gadget, 0L );
      free_rects(w);	/* 002 */
      wind_update( END_UPDATE );
    }
    last_gadget = 0;
  }
}

void set_gadget(void)
{
  Mouse m;
  
  if( last_gadget )
  {
    mks_graf( &m, 0 );
    if( !(m.b&1) ) gadget_off();
    else
    {
      if( !gadget_ok ) gadget_tic = tic();
      gadget_ok = 1;
    }
  }
}

void multi( int *buf, EMULTI *emulti )
{
  memcpy( &curapp->type, emulti, 16*sizeof(int) );
  curapp->buf = buf;
  curapp->apread_cnt = 16;
  curapp->apread_id = curapp->id;
  curapp->state &= curapp->mask;
}

int has_key(void)
{
  return lastkey;
}

long getkey(void)
{
/*%  char sh;
  long k;

  k = Bconin(2)&0xFFFFFFL;
  sh = kbshift();
  k |= (long)sh<<24L;
  return k; */
  long ret;
  
  if( !lastkey )
  {
    lastkey = evnt_keybd();
    lastsh = kbshift();
  }
  ret = ((long)lastsh<<24L) | (((long)lastkey<<8)&0xff0000L) | (unsigned char)lastkey;
  lastkey = 0;
  return ret;
}

int prev_obj( OBJECT *o, int num, int i )
{
  if( (i = o[i].ob_head) == num ) return num;
  while( o[i].ob_next != num ) i = o[i].ob_next;
  return i;
}

/*%void eat_clicks( APP *ap )
{
  while( bq_ptr != bq_last && !(but_q[bq_ptr].b&ap->mask) )
    if( ++bq_ptr == MAX_BUTQ ) bq_ptr = 0;
}*/

int wind_menu(void)
{
  Window *w;

  if( (w = top_wind) == 0 || w==desktop ||
      w->dial_obj==IS_TEAR || !w->menu ) w = 0L;
  next_menu=w;
  return next_menu!=0;
}

void menu_right( Window *w )
{
  if( w->menu && w->menu_tZ>0 )
  {
    if( !w->menu_tA ) w->menu_tA = w->menu[2].ob_head;
    w->menu_tA = w->menu[w->menu_tA].ob_next;
    if( w->menu_tZ )
      if( (w->menu_tZ = w->menu[w->menu_tZ].ob_next) ==
          w->menu[2].ob_tail ) w->menu_tZ = 0;
    draw_wmenu(w);
  }
}

void menu_left( Window *w )
{
  if( w->menu && w->menu_tA>0 )
  {
    if( (w->menu_tA=prev_obj(w->menu,w->menu_tA,2)) ==
        w->menu[2].ob_head ) w->menu_tA=0;
    if( w->menu_tZ ) w->menu_tZ = prev_obj(w->menu,w->menu_tZ,2);
    draw_wmenu(w);
  }
}

int mouse_ok( APP *ap, EMULTI *e, int use_ap, Window **w )
{
  int id;

  *w = find_window( wind_find( ap->mouse_x, ap->mouse_y ) );
  if( !*w ) return 0;
  if( !use_ap )
  {
    if( !(*w)->dialog && !(*w)->icon_index || (*w)->place<0 ) return 0;
  }
  else if( !update.i && (*w)->dialog ) return 0;	/* added for 003 */
  if( !(e->event&MU_BUTTON) || e->times<=0 || !(ap->type&(MU_BUTTON|X_MU_DIALOG)) ) return 0;
  else return 1;
}

void win_clip_ini( Rect *r )
{
  if( !r || (wclip_big.w = r->w)==0 || (wclip_big.h = r->h)==0 )
      wclip_big = desktop->outer;
  else
  {
    wclip_big.x = r->x;
    wclip_big.y = r->y;
  }
  wind_update( BEG_UPDATE );	/* 003 */
  wind_get( wclip_win->handle, WF_FIRSTXYWH, &wclip_rec.x,
      &wclip_rec.y, &wclip_rec.w, &wclip_rec.h );
  _v_mouse(0);
}

int win_clip(void)
{
  int ok=0;

  for(;;)
  {
    if( !wclip_rec.w )
    {
      wind_update( END_UPDATE );	/* 003 */
      return 0;
    }
    if( gintersect( wclip_big, wclip_rec, &wclip_rec ) )
    {
      do_wclip();
      ok++;
    }
    wind_get( wclip_win->handle, WF_NEXTXYWH, &wclip_rec.x,
        &wclip_rec.y, &wclip_rec.w, &wclip_rec.h );
    if( ok ) return 1;
  }
}

int _x_wdial( int hand, int start, int parm, int flag )
{
  Window *w;
  int i;

  wind_app = 0L;
  w = find_window(hand);
  if( w && w->apid == curapp->id && w->dialog && w->place>=0 )
  {
    wclip_win = w;
    clip_ini = win_clip_ini;
    clip_it = win_clip;
    if( !flag ) i = _objc_draw( (OBJECT2 *)w->dialog, 0L, start, parm, 0, 0, 0, 0 );
    else i = change_objc( w->dialog, 0L, start, &norect, parm, 1 );
    reset_clip();
    return i;
  }
  DEBUGGER(flag?XWDIALC:XWDIALD,INVHAND,hand);
  return 0;
}

int x_wdial_draw( int hand, int start, int depth )
{
  return _x_wdial( hand, start, depth, 0 );
}

int x_wdial_change( int hand, int start, int newstate )
{
  return _x_wdial( hand, start, newstate, 1 );
}

void init_win_dial( Window *w )
{
  wclip_win = w;
  clip_ini = win_clip_ini;
  clip_it = win_clip;
  form_reinit( w->dial_obj, w->dial_edit, w->dial_edind, w->place==place );
}

void exit_win_dial( Window *w )
{
  w->dial_obj = next_obj;
  w->dial_edit = edit_obj;
  w->dial_edind = edit_idx;
  reset_clip();
}

int win_dial( Window *w, int *buf, APP *ap )
{
  int i;

  if( w && w->apid == ap->id && w->dialog && w->place>=0 )
  {
    if( !(w->treeflag&X_WTFL_ACTIVE) )
    {
      Bconout(2,7);
      return 0;
    }
    init_win_dial( w );
    edit_curs( w->dialog, 0, 0 );
    if( (i = form_event( (long)w->dialog, wind_dial, 0 )) != 0 )
    {
      edit_curs( w->dialog, 1, 1 );
      edit_curs( w->dialog, 0, 0 );
    }
    exit_win_dial( w );
    if( !i )
    {
      ap->event |= X_MU_DIALOG;
      *(OBJECT **)&buf[0] = w->dialog;
      buf[2] = next_obj;
      buf[3] = w->handle;
      w->dial_obj = 0;
      return 1;
    }
    return i==2/*003*/ ? -2 : -1;
  }
  return 0;
}

int test_wdial( Window *w )
{
  return w->dialog && 
      (w->treeflag&(X_WTFL_CLICKS|X_WTFL_ACTIVE)) == (X_WTFL_CLICKS|X_WTFL_ACTIVE);
}

int scroll_wdial( Window *w, int flag, int val, int realtime )
{
  int i, j, imax, jmax, a;
  Rect r1, r2, r3, redraw;

  if( test_wdial(w) )
  {
    i = w->dialog[0].ob_x;
    j = w->dialog[0].ob_y;
    if( (imax=w->dialog[0].ob_width-w->working.w) <= 0 ) imax = 0;
    if( (jmax=w->dialog[0].ob_height-w->working.h) <= 0 ) jmax = 0;
    redraw = r1 = r2 = w->working;
    switch( flag )
    {
      case WA_LFPAGE:
        i += w->working.w;
        goto hor;
      case WA_RTPAGE:
        i -= w->working.w;
        goto hor;
      case WA_LFLINE:
        i += w->dial_swid;
        goto hor;
      case WA_RTLINE:
        i -= w->dial_swid;
        goto hor;
      case WM_HSLID:
        i = w->working.x - (!realtime ? (long)imax*val/1000L : val);
hor:    i = (i-w->working.x)/w->dial_swid*w->dial_swid + w->working.x;
        if( i<w->working.x-imax ) i = w->working.x - imax;
        else if( i>w->working.x ) i = w->working.x;
        if( w->dialog[0].ob_x==i ) return 1;
        gintersect( desktop->outer, w->working, &r3 );
        if( w->treeflag&X_WTFL_BLITSCRL && w->place==place &&
            (a=abs(j=i-w->dialog[0].ob_x)) < w->working.w &&
            *(long *)&r3.x == *(long *)&w->working.x &&
            *(long *)&r3.w == *(long *)&w->working.w )
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
        w->dialog[0].ob_x = i;
        break;
      case WA_UPPAGE:
        j += w->working.h;
        goto vert;
      case WA_DNPAGE:
        j -= w->working.h;
        goto vert;
      case WA_UPLINE:
        j += w->dial_sht;
        goto vert;
      case WA_DNLINE:
        j -= w->dial_sht;
        goto vert;
      case WM_VSLID:
        j = w->working.y - (!realtime ? (long)jmax*val/1000L : val);
vert:   j = (j-w->working.y)/w->dial_sht*w->dial_sht + w->working.y;
        if( j<w->working.y-jmax ) j = w->working.y - jmax;
        else if( j>w->working.y ) j = w->working.y;
        if( w->dialog[0].ob_y==j ) return 1;
        gintersect( desktop->outer, w->working, &r3 );
        if( w->treeflag&X_WTFL_BLITSCRL && w->place==place &&
            (a=abs(i=j-w->dialog[0].ob_y)) < w->working.h &&
            *(long *)&r3.x == *(long *)&w->working.x &&
            *(long *)&r3.w == *(long *)&w->working.w )
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
        w->dialog[0].ob_y = j;
        break;
    }
    dial_sliders( w, !realtime );
    redraw_window( w->handle, &redraw, 0 );
    return 1;
  }
  return 0;
}

int wslider( Window *w, int dir )
{
  int i, big, small, mult;

  if( !test_wdial(w) || !(w->type&(HSLIDE|VSLIDE)) ) return 0;
  mult = !dir ? w->dial_swid : w->dial_sht;
  if( graf_slidebox( 0L, !dir ? w->dialog[0].ob_width/mult :
      w->dialog[0].ob_height/mult, !dir ? w->working.w/mult :
      w->working.h/mult, 0x100|dir ) >= 0 )
  {
    wind_update( BEG_UPDATE );
    small = !dir ? WHSMLSL : WVSMLSL;
    w->tree[small].ob_state |= SELECTED;
    gredraw_obj( w, small, 0L );
    big = !dir ? WHBIGSL : WVBIGSL;
    i = graf_slidebox( w->tree, big, small, 0x200|dir );
    while( i>=0 )
    {
      scroll_wdial( w, !dir ? WM_HSLID : WM_VSLID, i*mult, 1 );
      i = graf_slidebox( w->tree, big, small, 0x300|dir );
    }
    w->tree[small].ob_state &= ~SELECTED;
    regenerate_rects(w);
    gredraw_obj( w, small, 0L );
    free_rects(w);
    wind_update( END_UPDATE );
  }
  return 1;
}

void fix_msg( APP *ap, int *buf )
{
  int *f;
  static int fix[] = { WM_REDRAW, WM_TOPPED, WM_CLOSED, WM_FULLED,
      WM_ARROWED, WM_HSLID, WM_VSLID, WM_SIZED, WM_MOVED, WM_UNTOPPED,
      WM_ONTOP, X_WM_HSPLIT, X_WM_VSPLIT, X_WM_ARROWED2, X_WM_HSLID2,
      X_WM_VSLID2, X_WM_OBJECT, 0 };

  if( ap->flags.flags.s.limit_handles )
    if( ap->event&X_MU_DIALOG ) goto fix_it;
    else
      for( f=fix; *f; f++ )
        if( *f==buf[0] )
        {
fix_it:   wind_app = ap;
          buf[3] = buf[3];	/* let set_stack clear wind_app */
          return;
        }
}

int cycle_wind(void)
{
  APP *ap, *ap2;

  if( place && !no_top && top_wind->apid>=0 )
      for( ap=app0; ap && ap->id!=top_wind->apid; ap=ap->next );
  else if( (ap=has_menu)==0L ) return 0;
  if( ap ) for( ap2=ap->next; ap2!=ap; )
    if( !ap2 ) ap2 = app0;
    else if( !ap2->asleep && ap2->menu && ap2!=has_menu )
    {
      switch_menu(ap2);
      return 1;
    }
    else if( !ap2->asleep && ap2->has_wind &&
        (no_top || top_wind->apid!=ap2->id) && cycle_top(ap2) ) return 1;
    else ap2 = ap2->next;
  return 0;
}

/*%int sw_next_app( APP *curr )
{
  APP *ap2;

  if( !has_menu || !multitask ) return 0;
  if( curr && curr->parent_id != 1 )	/* added parent search for 003 */
    for( ap2=app0; ap2; ap2=ap2->next )
      if( ap2->id == curr->parent_id )
      {
        if( ap2->menu && !ap2->asleep )
        {
          switch_menu(ap2);
          return 1;
        }
        break;
      }
  for( ap2=has_menu->next; ap2!=has_menu; )
    if( !ap2 ) ap2 = app0;
    else if( ap2->menu && !ap2->asleep )
    {
      switch_menu(ap2);
      return 1;
    }
    else ap2 = ap2->next;
  return 0;
} */

APP *dis_init( int set, int no_evnt, long stack )
{
  APP *ap;

  if( curapp )
  {
    if( set )
    {
      curapp->stack = stack;
      if( (curapp->no_evnt = no_evnt) != 0 ) curapp->type = 0;
    }
    if( !(curapp->type&MU_MESAG) ) curapp->buf = nobuf;
    ap = curapp;
/*%    if( !no_evnt && apps_initial && curapp->start_end ) /* is DA initializing? */
    {
      curapp->start_end = 0;
      apps_initial--;
    }
    if( !preempt )
    {
      if( loading ) loading = load_acc(0);  /* only returns when out of DA's */
      if( !apps_initial ) shel_exec();
    }
    ap=curapp->next; */
  }
  else ap = 0L;
/*%  _x_appl_free();*/
  return ap;
}

int has_kgad( Window *w, int k )
{
  static int trans[] = { CLOSER, MOVER, SMALLER, CLOSER|MOVER|NAME|FULLER|SMALLER, FULLER,
      INFO, INFO, INFO,
      0,
      0, 0, 0,	/* can't fit X_MENU here */
      UPARROW, VSLIDE, VSLIDE, DNARROW,
      0,
      UPARROW, VSLIDE, VSLIDE, DNARROW,
      LFARROW, HSLIDE, HSLIDE, RTARROW,
      0,
      LFARROW, HSLIDE, HSLIDE, RTARROW,
      SIZER };

  return (w->type & trans[k-WCLOSE])!=0 || (w->xtype&X_MENU) &&
      k >= WMNLEFT && k<= WMNRT || (w->xtype&X_VSPLIT) && k==WVSPLIT ||
      (w->xtype&X_HSPLIT) && k==WHSPLIT;
}

int check_gadget(void)
{
  Mouse m;
  int ret = 0;

  if( gadget_ok )
  {
    mks_graf( &m, 0 );
    if( !(m.b&1) )
    {
      gadget_ok = 0;
      if( last_gadget==99 ) last_gadget=0;
      else
      {
        /*%reset_butq();*/
        gadget_off();
      }
    }
    else if( last_gadget!=99 ) ret = 1;
  }
  return ret;
}

char no_topmsg = 1;

void wait_up( APP *ap )
{
  do
    get_mks();
  while( ap->mouse_b&1 );
}

void top_all( APP *ap, Window *w, int *buf )
{
  int p;
  Window *w2;

  wait_up(ap);
  buf[0] = WM_TOPPED;
  buf[1] = w->apid;
  buf[2] = 0;
  for( p=0; p<=place; p++ )
    for( w2=desktop; (w2=w2->next) != 0; )
      if( w2->place==p )
      {
        if( w2->apid==w->apid )
        {
          buf[3] = w2->handle;
          _appl_write( buf[1], 16, buf );
        }
        break;
      }
}

int cycle( Window *w )
{
  Window *w2;
  int h;

  if( w->place != 1 )
    for( h=1; h<place; h++ )	/* 003 */
      for( w2=desktop->next; w2; w2=w2->next )
        if( w2->place == h && has_kgad( w2, WBACK )/*003*/ )
          return w2->handle;
  return 0;
}

int gad_zone( int gadget )	/* 004 */
{
  return gadget>=WUP2 && gadget<=WDOWN2 || gadget>=WRT2 && gadget<=WLEFT2 ?
      X_WM_ARROWED2 : WM_ARROWED;
}

long _dispatch( APP *ap )
{
  int h1, h2, h3, h4, x, y, buf[8], buftl;
  Window *w, *w2, *oldtop;
  char *ptr;
  static unsigned char eq_tran[] = { WVBIGSL, WVBIGSL, WUP, WDOWN, WHBIGSL,
      WHBIGSL, WLEFT, WRT, WCLOSE, WBACK, WFULL, WILEFT, WIRT };
  Rect r;
  APP *ap2;
  long l;
  MSGQ *msg, *msg2;
  EMULTI e;

  ap->event = 0;
  memcpy( &e, &ap->type, 16*2 );
  e.type |= MU_MESAG|MU_BUTTON|MU_KEYBD;
  e.type &= ~X_MU_DIALOG;
  e.clicks = 2;
  e.mask = e.state = 1;
  for(;;)
  {
    if( (!update.i || has_update==ap) && ap->type&MU_MESAG/*003*/ )
      for( w=desktop->next; w; w=w->next )      /* redraws */
        if( w->dirty.w && (ap->id==w->apid || w->dial_obj==IS_TEAR) &&
            w->place>=0 )
        {
          msg_redraw( w, buf );
          ap->event |= MU_MESAG;
          if( w->dial_obj==IS_TEAR ) menu_evnt( ap, buf );
          else break;
        }
/*    if( no_topmsg ) get_top();  002: removed */
    if(io) buftl = (*io)->ibuftl+4;	/* 003: moved buftl checking into here */
    if( check_gadget() )
    {
      mks_graf( (Mouse *)&e.mouse_x, 0 );
      e.event = MU_BUTTON;
      e.times = 1;
      e.mouse_b = 3;
    }
    else if( !ap->event ) multi_evnt( &e, buf );
    else
    {
      mks_graf( (Mouse *)&e.mouse_x, 0 );
      e.event = 0;
    }
    if( e.event&MU_KEYBD && io )	/* 003 */
      do
      {
	if( buftl>=(*io)->ibufsiz ) buftl=0;
	ptr = (char *)(*io)->ibuf+buftl;
	if( *(ptr+1)==(unsigned)e.key>>8 && *(ptr+3)==(char)e.key )
	{
	  e.mouse_k = *ptr & 0xf;
	  break;
	}
	buftl+=4;
      }
      while( buftl != (*io)->ibuftl );
    curapp = ap;
    oldtop = top_wind;
    get_top();
    if( no_topmsg )
      if( !oldtop && top_wind ) all_gadgets( top_wind->place );
      else if( oldtop && !top_wind ) all_gadgets( oldtop->place );
    if( e.event & MU_MESAG )
    {
/*%      if( ap->type&MU_MESAG && !update.i )
      { */
      memcpy( ap->buf, buf, 16 );
      switch( buf[0] )
      {
        case WM_REDRAW:
          if( (w=find_window(buf[3])) != 0 )
          {
            if( w->chop_redraw )	/* 003 */
            {
              w->chop_redraw = 0;
              if( w->dialog || !gintersect( w->working,
                  *(Rect *)&buf[4], (Rect *)&buf[4] ) ) break;
            }
            redraw_window( buf[3], (Rect *)&buf[4], 0 );
          }
          else ap->event |= MU_MESAG;
          break;
        case AC_CLOSE:
          for( w2=desktop->next; (w=w2)!=0; )
          {
            w2 = w->next;
            if( w->apid == ap->id )
            {
              delete_wind(w);	/* 002: used to call _wind_delete */
              place--;
            }
          }
          ap->event |= MU_MESAG;
          break;
        case WM_UNTOPPED:
          if( top_wind && top_wind->handle==buf[3] ) break;		/* 003: ignore useless buffered msg */
        case WM_ONTOP:
          if( (w=find_window(buf[3])) != 0 ) all_gadgets(w->place);
          if( !w || w->dial_obj!=IS_TEAR ) ap->event |= MU_MESAG;
          no_topmsg = 0;
          break;
        case WM_TOPPED:
          if( (w=find_window(buf[3])) != 0 && w->dial_obj==IS_TEAR )
              _wind_set( w, WF_TOP, w->handle );
          else ap->event |= MU_MESAG;
          break;
        case DUM_MSG:	/* 003: avoid too many appl_writes in top_bar */
          if( --dum_msg < 0 ) dum_msg = 0;
        default:
          ap->event |= MU_MESAG;
      }
      if( ap->event&MU_MESAG && (!(ap->type&MU_MESAG) || update.i && has_update!=ap) )
      {
        _appl_write( ap->id, 16, buf );
        ap->event &= ~MU_MESAG;
        e.type &= ~MU_MESAG;	/* 003: prevent infinite loop */
      }
    }
     /*% else _appl_write( ap->id, 16, buf ); */
next:
    if( !lastkey && e.event&MU_KEYBD )
    {
      lastkey = e.key;
      lastsh = e.mouse_k;
    }
    if( !update.i && lastkey )
      if( is_key( &settings->menu_start, lastsh, lastkey ) )
      {
        wind_menu();
        lastkey=0;
      }
      else if( is_key( &settings->redraw_all, lastsh, lastkey ) )
      {
        form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect(desktop->outer) );
        lastkey=0;
      }
      else if( ap->type&MU_MESAG && owns_key(ap) && menu_equiv( buf, lastsh, lastkey ) )
      {
        ap->event |= MU_MESAG;
        lastkey=0;
      }
    if( !update.i && ap->type&MU_MESAG && next_menu && next_menu->apid==ap->id )
    {
      choose_menu( next_menu );
      next_menu=0L;
      mks_graf( (Mouse *)&ap->mouse_x, 0 );
      domenu( ap, buf, 1 );
    }
    if( !update.i && place && lastkey && top_wind && (top_wind->apid<0 || ap->type&MU_MESAG
        && ap->id==top_wind->apid) )    /* window kbd equivs */
    {
      h1 = (w=top_wind)->handle;
      recalc_window( h1, w, 0L );
      regenerate_rects(w);
      if( is_key( &settings->cycle_in_app, lastsh, lastkey ) )	/* 003 */
      {
        lastkey = 0;
        if( (h1=cycle(w)) > 0 ) goto topped;
      }
      else if( is_key( &settings->iconify, lastsh, lastkey ) )	/* 003 */
      {
        if( !w->icon_index )
        {
          h4 = WM_ICONIFY;
          next_iconify( buf+4 );
        }
        else
        {
          h4 = WM_UNICONIFY;
          *(Rect *)&buf[4] = w->iconify;
        }
        h3 = buf[4];
        lastkey = 0;
        goto msg;
      }
      else if( is_key( &settings->alliconify, lastsh, lastkey ) )	/* 003 */
      {
        if( !w->icon_index )
        {
          h4 = WM_ALLICONIFY;
          next_iconify( buf+4 );
        }
        else
        {
          h4 = WM_UNICONIFY;
          *(Rect *)&buf[4] = w->iconify;
        }
        h3 = buf[4];
        lastkey = 0;
        goto msg;
      }
      for( h2=0; h2<sizeof(settings->wind_keys)/sizeof(KEYCODE); h2++ )
        if( is_key( &settings->wind_keys[h2], lastsh, lastkey ) )
        {
          h3 = eq_tran[h2];
          if( !w->vsplit && (h3==WUP || h3==WDOWN || h3==WVBIGSL) ) h3+=WUP2-WUP;	/* 004 */
          else if( !w->hsplit && (h3==WLEFT || h3==WRT || h3==WHBIGSL) ) h3+=WLEFT2-WLEFT;	/* 004 */
          if( h3==WBACK )	/* 003: separated */
          {
            lastkey=0;
            if( (h1=cycle(w)) > 0 )
            {
              free_rects(w);
              goto topped;
            }
          }
          else if( has_kgad( w, h3 ) )
          {
            lastkey=0;
            if( h2<XS_CLOSE )
            {
              h4 = gad_zone(h3);		/* 004 */
              h3 = h2;
              goto armsg2;			/* 004: was armsg */
            }
            goto sw;
          }
        }
      free_rects(w);
    }
/*%    cur_last = bq_last;*/
    if( !update.i && e.event&MU_BUTTON )  /* window ops */
    {
      mks_graf( (Mouse *)&e.mouse_x, 0 );  /* in case b&2 */
      if( gadget_ok )
      {
        if( last_gadget!=99 && tic()-gadget_tic < settings->gadget_pause ) goto butn;
        h1 = last_gad_w;
      }
      else
      {
        if( last_gadget )
          if( last_gadget==99 ) last_gadget=0;
          else
          {
            gadget_off();
/*%            set_butq();*/
            goto butn;
          }
        memcpy( &ap->mouse_x, &e.mouse_x, 8 );
        h1 = wind_find( x=ap->mouse_x, y=ap->mouse_y );
      }
/*%      h2 = no_top ? 0 : top_wind->handle; */
      w = find_window( h1 );
      if( !h1 || !w )
      {
        if( (ap->times = e.times) > ap->clicks ) ap->times = ap->clicks;
        if( ap->type&MU_BUTTON ) ap->event |= MU_BUTTON;
        goto thru;
      }
      else if( /*%h1 == h2 ||*/ !top_wind || top_wind->handle==h1 || last_gadget || (e.mouse_b&2) || w->bevent&1 )
      {
        if( w->apid>=0 )
        {
          if( w->apid != ap->id )
          {
            if( (ap->times = e.times) > ap->clicks ) ap->times = ap->clicks;
            if( ap->type&MU_BUTTON ) ap->event |= MU_BUTTON;
            goto thru;
          }
          if( !(ap->type&MU_MESAG) ) goto butn;
        }
        last_gad_w = h1;
        recalc_window( h1, w, 0L );
        regenerate_rects(w);
        if( (h3=last_gadget ? last_gadget : objc_find( w->tree, 0, 8, x, y )) >= 0
            && (!h3 || has_kgad( w, h3 )) )
        {
/*%          if( h3 && h3!=99 ) set_butq();*/
sw:       if( !(w->treeflag&X_WTFL_CLICKS) && h3 && h3!=99 && h3!=WUP && h3!=WVBIGSL &&
              h3!=WDOWN && h3!=WLEFT && h3!=WHBIGSL && h3!=WRT )
          {
            h4 = X_WM_SELECTED;
            goto msg;
          }
          if( h3==last_gadget || still_pressed( w, h3 ) )
          {
            for( h4=e.times; --h4>=0; )
              if( w->objhand && !gobjhand( w->objhand, h1, h3 ) )
              {
                h4 = X_WM_OBJECT;
                goto msg;		/* leave h3 */
              }
            switch(h3)
            {
              case WCLOSE:
                h4 = WM_CLOSED;
                goto msg;
              case WMOVE:
                if( dragbox_graf( 0, &buf[4], &buf[5],
                    buf[6]=w->outer.w, buf[7]=w->outer.h,
                    w->outer.x, w->outer.y, desktop->working.x-
                    (ap->flags.flags.s.off_left ? w->outer.w : 0),
                    desktop->working.y, desktop->working.x+desktop->working.w+
                    (ap->flags.flags.s.off_left ? (w->outer.w<<1) : w->outer.w),
                    desktop->working.y+desktop->working.h+w->outer.h ) )
                {
/*%                  if( tic()-ngad_tic < dc_pause ) goto topped;*/
                  h4 = WM_MOVED;
                  h3 = buf[4];
                  goto msg;
                }
                goto topped;	/* clicked just once on name */
              case WBACK:
                if( w->place != 1 )
                {
                  for( w2=desktop->next; w2; w2=w2->next )
                    if( w2->place == 1 )
                    {
                      h1 = w2->handle;
                      free_rects(w);
                      goto topped;
                    }
                }
                break;
              case WICONIZ:
                h4 = (ap->mouse_k&4) ? WM_ALLICONIFY : WM_ICONIFY;
                next_iconify( buf+4 );
                h3 = buf[4];
                goto msg;
              case WFULL:  /* backward compat.: return current size */
                h4 = WM_FULLED;
                *(Rect *)&buf[4] = w->outer;
                h3 = buf[4];
                goto msg;
              case WIRT:
                if( w->info_end && w->treeflag&X_WTFL_SLIDERS )
                {
                  w->info_pos += !w->info_pos ? cel_w/char_w+1 : 1;
                  redraw_info(w);
                }
                last_gadget = h3;
                set_gadget();
                break;
              case WILEFT:
                if( w->info_pos > 0 && w->treeflag&X_WTFL_SLIDERS )
                {
                  if( w->info_pos <= cel_w/char_w+1 ) w->info_pos=0;
                  else w->info_pos--;
                  redraw_info(w);
                }
                last_gadget = h3;
                set_gadget();
                break;
              case WMNRT:
                menu_right(w);
                last_gadget = h3;
                set_gadget();
                break;
              case WMNLEFT:
                menu_left(w);
                last_gadget = h3;
                set_gadget();
                break;
              case WMENU:
                if( w->menu )
                {
                  /* wait for mouse to maybe come up */
/*%                  while( tic()-ngad_tic < settings->gadget_pause ); */
                  choose_menu( w );
                  get_mks();
                  domenu( ap, buf, -1 );
                }
                break;
              case WUP:
              case WUP2:
                last_gadget = h3;
                h3 = WA_UPLINE;
                goto armsg;
              case WVBIGSL:
              case WVBIGSL2:
                if( last_gadget == h3 ) h3 = last_gad_m;
                else
                {
                  objc_offset( w->tree, h3+1, &x, &y );
                  last_gadget = h3;
                  h3 = last_gad_m = ap->mouse_y < y ? WA_UPPAGE : WA_DNPAGE;
                }
                goto armsg;
              case WVSMLSL:
                if( w==top_wind && wslider(w,1) ) goto thru;
                h4 = WM_VSLID;
                goto vslid;
              case WVSMLSL2:
                h4 = X_WM_VSLID2;
vslid:          h3 = graf_slidebox( w->tree, h3-1, h3, 1 );
                goto msg;
              case WDOWN:
              case WDOWN2:
                last_gadget = h3;
                h3 = WA_DNLINE;
                goto armsg;
              case WVSPLIT:
                objc_xywh( (long)w->tree, WVSPLIT, &r );
                if( w->working.h-r.h <= w->vsp_min1+w->vsp_min2 )
                    dragbox_graf( 0, &h4, &h3, r.w, r.h, r.x, r.y,
                    r.x, r.y, r.w, r.h );
                else if( dragbox_graf( 0, &h4, &h3, r.w, r.h,
                    w->outer.x, r.y, w->working.x, h2=w->working.y-1,
                    r.w, w->working.h+2 ) )
                {
                  h4 = w->vsplit;
                  if( check_split( &w->vsplit, h3-h2, w->vsp_min1,
                      w->vsp_min2, w->working.h, r.h ) )
                  {
                    _get_split( w, buf+4, 1 );
                    h3 = w->vsplit;
                    w->vsplit = h4;
                    h4 = X_WM_VSPLIT;
                    goto msg;
                  }
                }
                break;
              case WLEFT:
              case WLEFT2:
                last_gadget = h3;
                h3 = WA_LFLINE;
                goto armsg;
              case WHBIGSL:
              case WHBIGSL2:
                if( last_gadget == h3 ) h3 = last_gad_m;
                else
                {
                  objc_offset( w->tree, h3+1, &x, &y );
                  last_gadget = h3;
                  h3 = last_gad_m = ap->mouse_x < x ? WA_LFPAGE : WA_RTPAGE;
                }
                goto armsg;
              case WHSMLSL:
                if( w==top_wind && wslider(w,0) ) goto thru;
                h4 = WM_HSLID;
                goto hslid;
              case WHSMLSL2:
                h4 = X_WM_HSLID2;
hslid:          h3 = graf_slidebox( w->tree, h3-1, h3, 0 );
                goto msg;
              case WRT:
              case WRT2:
                last_gadget = h3;
                h3 = WA_RTLINE;
armsg:          h4 = gad_zone(last_gadget);
armsg2:         free_rects(w);
                goto msg;
              case WHSPLIT:
                objc_xywh( (long)w->tree, WHSPLIT, &r );
                if( w->working.w-r.w <= w->hsp_min1+w->hsp_min2 )
                    dragbox_graf( 0, &h4, &h3, r.w, r.h, r.x, r.y,
                    r.x, r.y, r.w, r.h );
                else if( dragbox_graf( 0, &h4, &h3, r.w, r.h, r.x,
                    w->working.y, w->outer.x, w->working.y,
                    w->working.w+2, r.h ) )
                {
                  h3 = h4-w->outer.x;
                  h4 = w->hsplit;
                  if( check_split( &w->hsplit, h3,
                      w->hsp_min1, w->hsp_min2, w->working.w, r.w ) )
                  {
                    h3 = w->hsplit;
                    _get_split( w, buf+4, 0 );
                    w->hsplit = h4;
                    h4 = X_WM_HSPLIT;
                    goto msg;
                  }
                }
                break;
              case WSIZE:
                last_gadget = h3;
                *(Rect *)&buf[4] = w->outer;
                if( x_graf_rubberbox( (GRECT *)&buf[4], (GRECT *)&desktop->working,
                    w->min_w, w->min_h, w->max_w, w->max_h, 1, 1 ) )
                {
                  h4 = WM_SIZED;
                  h3 = buf[4];
                  gadget_off();
                  goto msg;
                }
                gadget_off();
                break;
              case WTOOLBOX:
                last_gadget = h3;
                break;
              case 0:
                free_rects(w);
                if( w->icon_index && (h4=mouse_ok( ap, &e, 0, &w2 )) != 0 &&
                    e.times==2 )
                {
/*%                  set_butq();*/
                  h4 = WM_UNICONIFY;
                  *(Rect *)&buf[4] = w->iconify;
                  h3 = buf[4];
                  goto msg;
                }
                else if( w->dial_obj == IS_TEAR )
                {
/*%                  set_butq();*/
                  buf[3] = h1;
                  ap->event |= MU_BUTTON;
                  memcpy( &ap->mouse_x, &e.mouse_x, 8 );
                  menu_evnt( ap, buf );
                }       /* eat clicks if not looking for this event */
                else if( !ap->no_evnt && !(ap->type&MU_BUTTON) &&
                    (!w->icon_index || h4 && e.times) &&
                    (w->apid>=0 && !(ap->type&X_MU_DIALOG) ||
                    !w->dialog || !(w->treeflag&X_WTFL_ACTIVE)) ) {} /*% set_butq(); */
                else if( ap->mouse_b&1 /*%&& cur_last==bq_last*/ )
                {
                  last_gadget = 99;
                  gadget_ok = 1;
                }
            }
          }
        }
        free_rects(w);
      }
      else
      {
        if( ap->mouse_k&3 )	/* 003 */
        {
          top_all( ap, w, buf );
          goto thru;
        }
topped: h4 = WM_TOPPED;
        /*%do
          get_mks();
        while( ap->mouse_b&1 ); */
msg:    set_gadget();
	e.event &= ~MU_BUTTON;
        if( h4==WM_ARROWED )
        {
          if( scroll_wdial( w, h3, 0, 0 ) ) goto thru;
        }
        else if( h4==WM_HSLID || h4==WM_VSLID )
          if( scroll_wdial( w, h4, h3, 0 ) ) goto thru;
        buf[0] = h4;
        w = find_window( h1 );
        buf[1] = w->apid;
        buf[2] = 0;
        buf[3] = h1;
        buf[4] = h3;
        ap->event |= MU_MESAG;
        if( w->dial_obj == IS_TEAR )
        {
          menu_evnt( ap, buf );
          w = desktop;
        }
      }
      if( ap->event&MU_MESAG )
      {
	e.event &= ~MU_BUTTON;
        if( w->apid != ap->id || !(ap->type&MU_MESAG) )
        {
          _appl_write( w->apid, 16, buf );
          ap->event &= ~MU_MESAG;
        }
        goto thru;
      }
    }
butn:
    if( !gadget_ok || last_gadget==99 )
    {
      if( ap->type&X_MU_DIALOG )
      {
        memcpy( &ap->mouse_x, &e.mouse_x, 8 );
        ap->times = e.times;
        if( mouse_ok( ap, &e, 0, &w2 ) && win_dial( w2, buf, ap ) )
        {
/*%          set_butq();*/
          goto thru;
        }
        if( lastkey && owns_key(ap) &&
           ((h1=win_dial( top_wind, buf, ap )) > 0 || h1==-2) )	/*003*/
        {
/*          if( h1>0 || edit_obj>=0 || !(top_wind->treeflag&X_WTFL_KEYS) )
          { 003 */
            lastkey=0;
            goto thru;
/*          }
          goto keybd; */
        }
      }
      if( ap->type&MU_BUTTON )
        if( mouse_ok( ap, &e, 1, &w2 ) && (!w2->icon_index || e.times<2) )
        {
          ap->event |= MU_BUTTON;
/*%          set_butq();*/
          memcpy( &ap->mouse_x, &e.mouse_x, 8 );
          if( ap->clicks <= 0 ) ap->times = 0;
          else
          {
/*%            if( !unclick && ap->mask ) ap->mouse_b = ap->state;*/
            ap->times = e.times;
          }
        }
    }
keybd:
    if( lastkey && owns_key(ap) )
    {
      ap->key = lastkey;
      h1 = ap->mouse_k;
      ap->mouse_k = lastsh;
      ap->event |= MU_KEYBD;
      if( !update.i && !no_top && top_wind && top_wind->dial_obj == IS_TEAR )
      {
        buf[3] = top_wind->handle;
        menu_evnt( ap, buf );
        if( !(ap->event&MU_KEYBD) )
        {
          ap->mouse_k = h1;
          lastkey = 0;  /* tear ate key */
        }
      }
      if( !ap->no_evnt )
      {
        if( !(ap->type&MU_KEYBD) )
        {
          ap->event &= ~MU_KEYBD;
          ap->mouse_k = h1;
          if( ap->type&MU_MESAG ) lastkey = 0;
        }
        else lastkey = 0;
      }
      else ap->event &= ~MU_KEYBD;
    }
thru:
    if( ap->type & (MU_M1|MU_M2|MU_TIMER) )
        ap->event |= e.event & (MU_M1|MU_M2|MU_TIMER);
    if( ap->event )
    {
      if( ap->event&(X_MU_DIALOG|MU_MESAG) )
      {
        fix_msg( ap, buf );
        memcpy( ap->buf, buf, 8*sizeof(int) );
      }
      set_curapp(ap);
      return(ap->stack);
    }
  }
}

long dispatch( int no_evnt, long stack )
{
  APP *ap;

  ap = dis_init( 1, no_evnt, stack );
  return _dispatch(ap);
}

void _multi_evnt( EMULTI *a, int *msgbuf )
{
  multi( msgbuf, a );
  dispatch( 0, 0L );
  memcpy( &a->event, &curapp->event, 7*sizeof(int) );
}

int _evnt_timer( int locount, int hicount )
{
/*%  int msgbuf[8];
  EMULTI e;
  
  e.type = MU_TIMER;
  e.low = locount;
  e.high = hicount;
  multi( msgbuf, &e );
  dispatch( 0, 0L ); */
  evnt_timer( locount, hicount );
  return 1;
}

void _arrow_dial( int *msg )
{
  Window *w;
  
  if( (w = find_window( msg[3] )) != 0 ) scroll_wdial( w, msg[4], 0, 0 );
}

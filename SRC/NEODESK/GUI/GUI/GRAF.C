#include "new_aes.h"
#include "vdi.h"
#include "tos.h"
#include "..\neocommn.h"
#include "win_var.h"
#include "win_inc.h"
#include "xwind.h"
#include "string.h"
#include "stdlib.h"

#define DEBUGGER(x,y,z)

void init_xor(void)
{
  _vsl_color( 1 );
  _vsl_type( 7 );
  _vswr_mode( 3 );
  _vsl_udsty( 0x5555 );
}

int kbshift(void)
{
  return Kbshift(-1)&0xf;
}

int _mks_graf( Mouse *out )
{
/*%  vq_mouse( vdi_hand, &out->b, &out->x, &out->y );
  out->k = kbshift(); */
/*%  graf_mkstate( &out->x, &out->y, &out->b, &out->k ); */
  EMULTI e;
  int buf[8];
  
  e.type = MU_TIMER|MU_KEYBD;
/*%  e.m1flags = 0;
  *(Rect *)&e.m1x = desktop->outer; */
  e.low = 5;
  e.high = 0;
  multi_evnt( &e, buf );
  if( e.event & MU_KEYBD )
  {
    lastkey = e.key;
    lastsh = e.mouse_k;
  }
  *out = *(Mouse *)&e.mouse_x;
  return(1);
}

#pragma warn -par
int mks_graf( Mouse *out, int flag )
{
  void reset_butq(void);

  /*%if( flag>0 ) reset_butq();*/
  return _mks_graf( out );
/*%  *out = *(Mouse *)&but_q[bq_ptr].x;
  if( flag<=0 )
  {
    if( !flag && bq_ptr != bq_last )
    {
      if( ++bq_ptr == MAX_BUTQ ) bq_ptr=0;
      unclick=0;
    }
    but_q[bq_ptr].x = g_mx;
    but_q[bq_ptr].y = g_my;
  }
  return(1); */
}
#pragma warn +par

void objc_xywh( long tree, int obj, Rect *p )
{
  objc_offset((OBJECT *)tree, obj, &p->x, &p->y);
  *(long *)&(p->w) = *(long *)&(((OBJECT *)tree)[obj].ob_width);
}

void clip_desk(void)
{
  _vs_clip( 1, &desktop->outer );
}

void draw_xor( Rect *r )
{
  to_larr( Xrect(*r) );
  _v_mouse(0);
  pline_5();
  _v_mouse(1);
}

OBJECT2 *gr_tree;
static int gr_par, gr_obj, gr_all;
static Mouse m;
static Rect obr, par;

void move_slid( Rect *r, int nx, int ny )
{
  Rect s;

  s = *r;
  s.x--;
  s.y--;
  s.w+=2;
  s.h+=2;
  if( !ny )
    if( nx<0 ) s.w = -nx+2;
    else
    {
      s.x = r->x+r->w-nx;
      s.w = nx+2;
    }
  else if( !nx )
    if( ny<0 ) s.h = -ny+2;
    else
    {
      s.y = r->y+r->h-ny;
      s.h = ny+2;
    }
  _objc_draw( gr_tree, 0L, gr_par, 0, Xrect(s) );
  s = *r;
  s.x -= nx;
  s.y -= ny;
  mblit( 0x101, &s );
  gr_tree[gr_obj].ob_x -= nx;
  gr_tree[gr_obj].ob_y -= ny;
}

int _graf_dragbox( GDBO *go, GDB *g, int mode )
{
  int nx, ny, bw, bh;
  Rect r;

  go->x = r.x = g->x;
  go->y = r.y = g->y;
  r.w = g->w;
  r.h = g->h;
  bw = g->bx+g->bw;
  bh = g->by+g->bh;
  if( !mode ) mks_graf( &m, 1 );
  if( r.x < g->bx ) r.x = g->bx;
  else if( r.x+r.w > bw ) r.x = bw-r.w;
  if( r.y < g->by ) r.y = g->by;
  else if( r.y+r.h > bh ) r.y = bh-r.h;
  if( mode ) mblit( 0x101, &r );
  if( !(m.b&1) ) return(0);
  if( !mode )
  {
    init_xor();
    clip_desk();
  }
  do
  {
    if( !mode ) draw_xor( &r );
    else mblit( 0x101, &r );
    do
    {
      ny = m.y;
      nx = m.x;
      mks_graf( &m, 1 );
      if( m.x < g->bx ) m.x = g->bx;
      else if( m.x > bw ) m.x = bw;
      if( m.y < g->by ) m.y = g->by;
      else if( m.y > bh ) m.y = bh;
      ny -= m.y;
      nx -= m.x;
      if( r.x-nx < g->bx ) nx = r.x-g->bx;
      else if( r.x+r.w-nx > bw ) nx = -(bw-r.w-r.x);
      if( r.y-ny < g->by ) ny = r.y-g->by;
      else if( r.y+r.h-ny > bh ) ny = -(bh-r.h-r.y);
    }
    while( (m.b&1) && !nx && !ny );
    if( !mode ) draw_xor( &r );
    else move_slid( &r, nx, ny );
    r.x -= nx;
    r.y -= ny;
  }
  while( m.b&1 && !mode );
  go->x = r.x;
  go->y = r.y;
  return(1);
}

int cdecl dragbox_graf( int blit, int *ox, int *oy, int gr_dwidth, ... )
{
  int i;
  GDBO o;

  i = _graf_dragbox( &o, (GDB *)&gr_dwidth, blit );
  *ox = o.x;
  *oy = o.y;
  return(i);
}

int sl_pos( int slvh, int x, int y )
{
  int i;

  if( !(slvh&0xff) )
    if( (i = par.w-obr.w)<=0 ) return(0);
    else return( (long)((x-par.x)*1000L+i-1)/i );
  else if( (i=par.h-obr.h)<=0 ) return(0);
    else return( (long)((y-par.y)*1000L+i-1)/i );
}

int conv_pos( int pos )
{
  return (long)pos * gr_all / 1000L;
}

int graf_slidebox( OBJECT *tree, int parent, int object, int slvh )
{
  int old, new, x, y, mode;

  mode = slvh>>8;
  lock_menu(1);
  if( mode==1 )
  {
    mks_graf( &m, 1 );
    lock_menu(0);
    if( !(m.b&1) ) return -1;
    if( (gr_all = parent - object + 1) < 0 ) gr_all = 0;
    return 0;
  }
  objc_xywh( (long)tree, object, &obr );
  objc_xywh( (long)tree, parent, &par );
  form_app = curapp;
  adjust_rect( &tree[object], &obr, 1 );
  adjust_rect( &tree[parent], &par, 1 );
  if( mode==2 )
  {
    if( !mblit( 0x100, &obr ) )
    {
      lock_menu(0);
      return 0;
    }
    _objc_draw( gr_tree=(OBJECT2 *)tree, 0L, gr_par=parent, 0, par.x, par.y, par.w, par.h );
    gr_obj = object;
  }
  if( mode ) old = conv_pos( sl_pos( slvh, obr.x, obr.y ) );
  x = obr.x;
  y = obr.y;
  do
  {
    if( !dragbox_graf( mode, &x, &y, obr.w, obr.h, x, y, par.x, par.y, par.w, par.h ) && mode )
    {
      lock_menu(0);
      return -1;
    }
    new = sl_pos( slvh, x, y );
    if( mode ) new = conv_pos(new);
    else break;
  }
  while( new==old );
  lock_menu(0);
  return new;
}

void get_mpos( Mouse *m, GRECT *area, int snap )
{
  mks_graf( m, 1 );
  if( snap>1 )
  {
    m->x = (m->x-area->g_x) / snap * snap + area->g_x;
    m->y = (m->y-area->g_y) / snap * snap + area->g_y;
  }
}

int x_graf_rubberbox( GRECT *r, GRECT *outer, int minwidth, int minheight,
    int maxwidth, int maxheight, int snap, int lag )
{
  int x, y, xo, yo, dx, dy, i, negx=0, negy=0, lagx, lagy, no_neg;
  register int state=0;
  Mouse m;
  GRECT r0;

  if( snap<1 ) snap = 1;
  if( !outer ) outer = (GRECT *)&desktop->working;
  lock_menu(1);
  get_mpos( &m, r, snap );
  no_neg = lag&0xff00;
  lag = (unsigned char)lag;
  if( !lag )
  {
    r->g_w = m.x-r->g_x;
    r->g_h = m.y-r->g_y;
    lagx=lagy=0;
  }
  else
  {
    lagx = r->g_x+r->g_w-m.x;
    lagy = r->g_y+r->g_h-m.y;
  }
  r0 = *r;
  clip_desk();
  init_xor();
  while( m.b & 1 )
  {
    xo = negx ? r->g_x : r->g_x+r->g_w;
    yo = negy ? r->g_y : r->g_y+r->g_h;
    draw_xor( (Rect *)r );
    do
    {
      get_mpos( &m, r, snap );
      dx = m.x + lagx - xo;
      dy = m.y + lagy - yo;
      if( negx )
      {
        dx = -dx;
        if( (i=r->g_w+dx) < minwidth )
        {
          if( i<0 ) negx = -1;
          dx = minwidth-r->g_w;
        }
        else if( i > maxwidth ) dx = maxwidth-r->g_w;
        if( r->g_x-dx < outer->g_x ) dx = r->g_x-outer->g_x;
      }
      else
      {
        if( (i=r->g_w+dx) < minwidth )
          if( i<0 && minwidth>0 && !no_neg )
          {
            negx = 2;
            dx = -i;
            if( r0.g_w+dx > maxwidth ) dx = maxwidth-r0.g_w;
          }
          else dx = minwidth-r->g_w;
        else if( i > maxwidth ) dx = maxwidth-r->g_w;
        if( r->g_x+r->g_w+dx > outer->g_x+outer->g_w )
            dx = outer->g_x+outer->g_w-r->g_x-r->g_w;
      }
      if( negy )
      {
        dy = -dy;
        if( (i=r->g_h+dy) < minheight )
        {
          if( i<0 ) negy = -1;
          dy = minheight-r->g_h;
        }
        else if( i > maxheight ) dy = maxheight-r->g_h;
        if( r->g_y-dy < outer->g_y ) dy = r->g_y-outer->g_y;
      }
      else
      {
        if( (i=r->g_h+dy) < minheight )
          if( i<0 && minheight>0 && !no_neg )
          {
            negy = 2;
            dy = -i;
            if( r0.g_h+dy > maxheight ) dy = maxheight-r0.g_h;
          }
          else dy = minheight-r->g_h;
        else if( i > maxheight ) dy = maxheight-r->g_h;
        if( r->g_y+r->g_h+dy > outer->g_y+outer->g_h )
            dy = outer->g_y+outer->g_h-r->g_y-r->g_h;
      }
    }
    while( (m.b&1) && !dx && !dy && (!negx || negx==1) && (!negy || negy==1) );
    draw_xor( (Rect *)r );
    if( negx==2 )
    {
      r->g_w = r0.g_w;
      negx=1;
    }
    if( negx ) r->g_x -= dx;
    r->g_w += dx;
    if( negy==2 )
    {
      r->g_h = r0.g_h;
      negy=1;
    }
    if( negy ) r->g_y -= dy;
    r->g_h += dy;
    if( negx<0 )
    {
      r->g_x = r0.g_x;
      negx=0;
    }
    if( negy<0 )
    {
      r->g_y = r0.g_y;
      negy=0;
    }
    state = 1;
  }
  lock_menu(0);
  return( state );
}

static Rect norect = { 0, 0, 0, 0 };
APP *gw_app;

int gwatch( Rect *g, OBJECT2 *tree, int obj, int in, int out,
    unsigned int ex )
{
  int state, x, y, i, mx, my, dum;

  graf_mkstate( &mx, &my, &dum, &dum );
  x = mx - g->x;
  y = my - g->y;
  i = (state = x >= 0 && x < g->w && y >= 0 && y < g->h) != 0 ? in : out;
  if( tree[obj].ob_state != i ) change_objc( (OBJECT *)tree, gw_app, obj,
      &norect, ex|i, 1 );
  return(state);
}

int graf_watchbox( OBJECT *tree, int obj, int in, int out )
{
  return _graf_watchbox( tree, curapp, obj, in, out );
}

int _graf_watchbox( OBJECT *tree, APP *ap, int obj, int in, int out )
{
  unsigned int ex;
  Rect g;
  int state;
  OBJECT *tree2;
  Mouse m;

  gw_app = ap;
  objc_xywh( (long)tree, obj, &g );
  tree2 = &tree[obj];
  ex = tree2->ob_state&0xFF00;
  _mks_graf(&m);
  if( !(m.b&1) )
  {
    state = 1;
    if( tree2->ob_state != in ) change_objc( tree, gw_app, obj, &norect,
        ex|in, 1 );
  }
  else
  {
    while( m.b&1 )
    {
      state = gwatch( &g, (OBJECT2 *)tree, obj, in, out, ex );
      _mks_graf(&m);
    }
    state = gwatch( &g, (OBJECT2 *)tree, obj, in, out, ex );
  }
  return(state);
}

int x_graf_blit( GRECT *r1, GRECT *r2 )
{
  MFDB mfdb = { 0L };
  int px[8];

  if( !r1 ) mblit( 0x101, (Rect *)r2 );
  else if( !r2 ) return mblit( 0x100, (Rect *)r1 );
  else
  {
    _vs_clip( 0, 0L );
    px[2] = (px[0] = r1->g_x) + r1->g_w - 1;
    px[3] = (px[1] = r1->g_y) + r1->g_h - 1;
    px[6] = (px[4] = r2->g_x) + r2->g_w - 1;
    px[7] = (px[5] = r2->g_y) + r2->g_h - 1;
    _v_mouse(0);
    vro_cpyfm( vdi_hand, 3, px, &mfdb, &mfdb );
    _v_mouse(1);
  }
  return 1;
}

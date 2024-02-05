#include "aes.h"
#include "vdi.h"
#include "xwind.h"
#include "tos.h"
#include "neocommn.h"
#include "win_var.h"
#include "win_inc.h"
#define _MENUS
#include "wind_str.h"
#include "string.h"
#include "stdlib.h"
#include "windows.h"

#define DEBUGGER(x,y,z)

static Rect pull_rect, max;
OBJECT *ptree;
OBJECT blank[] = { -1, -1, -1, G_BOX, 32, 0, 0xFF1070L };
int title, m_head, title_alts, next_pull;
static int bar_h;
int pull=0, entry=0;
int old_mx, old_my;
static int keybd, state;
char in_sub, neg_alts, is_menu;
Window *menu_wind;
MN_SET user_mset = { 200, 10000, 250, 0, 16 }, menu_set;
unsigned char mnuk, mnush;
int mnuk_out, *menu_buf;
MENU sub_pop;
unsigned long entry_tic, scroltic1, scroltic2, pop_tic;
static ACC_LIST *sorted_acc;
static int m_obj, up, down, first, last, sfirst, sprev /* last non-scrollable obj */, stop, sbot;
static OBJECT *m_start;
char have_mvec, g_mb, did_mclick;
typedef struct
{
  Rect r;
  OBJECT *tree;
  int pull, entry, up, down, first, last, sfirst, sprev, stop, sbot;
} SUB_DAT;
SUB_DAT sub_dat[4];

int find_pull( int title )
{
  int x, y;

  y = guimenu[guimenu[guimenu[0].ob_head].ob_next].ob_head;
  for( x=m_head; x!=title; y=guimenu[y].ob_next, x++ /* 003 x=guimenu[x].ob_next*/ );
  return y;
}

unsigned char *keys;

static int find_equiv(OBJECT *ob, int obj)
{
  char *s, *e, *s0, k, shift=0;
  int ted, i;
  static char knames[][7]= { "ESC", "TAB", "RET", "RETURN", "BKSP", "BACKSP",
      "DEL", "DELETE", "HELP", "UNDO", "INS", "INSERT", "CLR", "HOME", "F1",
      "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "ENT", "ENTER",
      "KP(", "KP)", "KP/", "KP*", "KP7", "KP8", "KP9", "KP-", "KP4", "KP5",
      "KP6", "KP+", "KP1", "KP2", "KP3", "KP0", "KP.", "",  "",  "" },
              kscans[]   = { 1,     0XF,   0X1C,  0X1C,     0XE,    0XE,
      0X53,  0X53,     0X62,   0X61,   0X52,  0X52,     0X47,  0X47,   0X3B,
      0X3C, 0X3D, 0X3E, 0X3F, 0X40, 0X41, 0X42, 0X43, 0X44,  0X72,   0X72,
      0X63,  0X64,  0X65,  0X66,  0X67,  0X68,  0X69,  0X4A,  0X6A,  0X6B,
      0X6C,  0X4E,  0X6D,  0X6E,  0X6F,  0X70,  0X71,  0x50, 0x4d, 0x4b };

  if( !obj ) return 1;
  ob += obj;
  if( ob->ob_flags&HIDETREE ) return 0;
  if( (char)(ob->ob_type)==G_TITLE ) return 1;
  if( (ob->ob_state&X_MAGMASK) == X_MAGIC ) return 1;
  ob->ob_state &= 0xff;
  if( (s=s0=get_butstr((long)ob,0,&ted,0))!=0 && (e = s+strlen(s))!=s0 )
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
            else if( shift&2 )	/* 003 */
              if( i==0x47 ) i = 0x77;
              else if( i==0x4b ) i = 0x73;
              else if( i==0x4d ) i = 0x74;
            ob->ob_state |= i<<8;
            ob->ob_flags |= shift<<13;
          }
          return 1;
        }
    }
  }
  return 1;
}

void set_equivs( Window *w )
{
  choose_menu( w );
  if( guimenu && (guimenu[0].ob_state&X_MAGMASK)==X_MAGIC )
  {
    keys = Keytbl( (void *)-1L, (void *)-1L, (void *)-1L )->shift;
    fn_dir = 0;
    map_tree( guimenu, 0, -1, find_equiv );
  }
}

int gmenu_bar( OBJECT *tree, int flag )
{
  extern char is_acc;

  if( !flag )
  {
    has_menu = 0L;
    desktop->menu = curapp->menu = 0L;
  }
  else if( flag>0 )
  {
    has_menu = curapp;
    desktop->menu = curapp->menu = tree;
    tree[0].ob_x = 0;
    tree[0].ob_y = 0;
    tree[0].ob_width = tree[1].ob_width = desktop->outer.w;
    set_equivs(desktop);
  }
  if( is_acc ) return 0;		/* 002 */
  return menu_bar( tree, flag );
}

void menu_rec( Window *w, int i, int title )
{
  Rect r1;

  objc_xywh( (long)guimenu, i, &r1 );
  if( title )
    if( w != desktop )
    {
      if( w->place > 0 )	/* 003: test */
      {
        recalc_window( w->handle, w, (X_MENU<<8L)/*003*/ );
        regenerate_rects(w);
        gredraw_obj( w, WMENU, &r1 );
      }
    }
    else _objc_draw( (OBJECT2 *)guimenu, menu_owner, i, 8, Xrect(max) );
  else add_rects( &w->dirty, &r1 );
}

int change_menu( OBJECT *tree, int obj, int flag, int val, int maintest )
{
  Window *w;
  int i, j, *p, notmain=0;
  Rect_list *r;

  i = *(p=(int *)&tree[obj].ob_state);
  if( flag ) *p |= val;
  else *p &= ~val;
  if( *p != i || flag<0 )
    for( w=desktop->next; w; w=w->next )
      if( w->dial_obj==IS_TEAR && w->menu == tree )
      {
        move_menu( 0, w );
        i = j = guimenu[w->vslide].ob_head;
        do
          if( i==obj )
          {
            menu_rec( w, i, 0 );
            break;
          }
        while( (i=guimenu[i].ob_next) != j && i>0 );
        move_menu( 1, w );
        notmain++;
      }
      else if( w->menu==tree )
      {
        choose_menu( w );
        if( (char)tree[obj].ob_type == G_TITLE )  menu_rec( w, obj, 1 );
/*%        if( w==desktop && obj == guimenu[find_pull(tree[2].ob_head)].ob_head )
            accs_obj(0); */
        notmain++;
      }
  if( maintest && !notmain )
  {
    *p = i;
    return 1;
  }
  return 0;
}

int menu_icheck( OBJECT *tree, int obj, int check )
{
  if( !tree )
  {
    DEBUGGER(MNUICH,NULLTREE,0);
    return(0);
  }
  change_menu( tree, obj, check, CHECKED, 0 );
  return(1);
}

int menu_ienable( OBJECT *tree, int obj, int check )
{
  if( !tree )
  {
    DEBUGGER(MNUIEN,NULLTREE,0);
    return(0);
  }
  change_menu( tree, obj, !check, DISABLED, 0 );
  return(1);
}

int _menu_tnormal( OBJECT *tree, int obj, int check )
{
  if( !tree )
  {
    DEBUGGER(MNUTNO,NULLTREE,0);
    return(0);
  }
  check = !check;
  if( (tree[obj].ob_state&SELECTED) != check )
    if( change_menu( tree, obj, -1, 0, 1 ) ) menu_tnormal( tree, obj, !check );
    else change_menu( tree, obj, check, SELECTED, 0 );
  return(1);
}

int menu_text( OBJECT *tree, int obj, const char *text )
{
  if( !tree )
  {
    DEBUGGER(MNUTEX,NULLTREE,0);
    return(0);
  }
  strcpy( tree[obj].ob_spec.free_string, text );
  change_menu( tree, obj, -1, 0, 0 );
  return(1);
}

void no_clip(void)
{
  _vs_clip( 0, 0L );
}

int mblit( int flag, Rect *r2 )
{
  int px[8];
  long size;
  Rect r = *r2;
  static PULL pb[4];
  static int blit_lev;
  static char first_too;	/* set if blit_lev=0 was too small */

  if( flag==-1 ) goto free;
  if( !(flag&0xff00) )
  {
    if(r.x) r.x--;
    r.y--;
    r.w+=2;
    r.h+=2;
  }
  else flag = (char)flag;
  gintersect( desktop->outer, r, &r );
  fdb2.fd_h = r.h;
  fdb2.fd_wdwidth = (fdb2.fd_w = r.w+16) >> 4;
  fdb2.fd_nplanes = vplanes;
  fdb2.fd_r1 = fdb2.fd_r2 = fdb2.fd_r3 = 0;
  size = (long)r.h*(fdb2.fd_wdwidth<<1)*vplanes;
  if( !flag && !blit_lev && size>pull_siz.l )
  {
    first_too++;
    blit_lev++;
  }
  if( !flag && blit_lev++ )
  {
    pb[blit_lev-2].l = pull_buf.l;
    if( (pull_buf.l = (long)lalloc( size, -1 )) == 0 )
    {
      while( --blit_lev > 1 )
        lfree((void *)pb[blit_lev-1].l);
      pull_buf.l = pb[0].l;
      blit_lev = 0;
      no_memory();
      return 0;
    }
  }
  fdb2.fd_addr = (char *)pull_buf.l;
  no_clip();
  px[2] = (px[0] = !flag ? r.x : 0) + r.w - 1;
  px[3] = (px[1] = !flag ? r.y : 0) + r.h - 1;
  px[6] = (px[4] = !flag ? 0 : r.x) + r.w - 1;
  px[7] = (px[5] = !flag ? 0 : r.y) + r.h - 1;
  _v_mouse(0);
  vro_cpyfm( vdi_hand, 3, px, !flag ? &fdb0 : &fdb2, !flag ? &fdb2 : &fdb0 );
  _v_mouse(1);
free:
  if( flag )
  {
    if( blit_lev>1 )
    {
      lfree((void *)pull_buf.l);
      pull_buf.l = pb[blit_lev-2].l;
    }
    if( blit_lev )
    {
      blit_lev--;
      if( first_too )
      {
        first_too--;
        blit_lev--;
      }
    }
  }
  return 1;
}

void drw_alt( OBJECT *tree, int num, int undraw )
{
  _v_mouse(0);
  alt_redraw( (long)tree, num, undraw );
  _v_mouse(1);
}

void draw_title(void)
{
  menu_rec( menu_wind, title, 1 );
  if( menu_wind != desktop )
  {
    no_clip();
    drw_alt( guimenu, -title, 0 );
  }
}

INT_MENU *find_sub( APP *ap, OBJECT2 *tree, int item, unsigned int *index )
{
  INT_MENU *im;
  unsigned char num;
  unsigned int ii;

  if( !ap || (im=ap->menu_att)==0 ) return 0L;
  tree += item;
  if( (num = tree->ob_typex)<=127 ) return 0L;
  im += (ii = num-128);
  if( index ) *index = ii;
  if( im->parent!=tree ) return 0L;
  return im;
}

int add_attach( OBJECT *tree, int num )
{
  unsigned int ii;

  if( (char)tree[num].ob_type == G_STRING && find_sub( menu_owner,
      (OBJECT2 *)tree, num, &ii ) != 0 )
      tree[num].ob_spec.free_string[strlen(tree[num].ob_spec.free_string)-2] =
      fn_dir ? ' ' : '';
  return 1;
}

void set_attaches( OBJECT *tree, int i, int pull )
{
  if( menu_owner && menu_owner->menu_att )
  {
    fn_dir = i;
    map_tree( tree, pull, tree[pull].ob_next, add_attach );
  }
}

void undraw( int title, int flag )
{
  if( pull_rect.w )
  {
    mblit( 1, &pull_rect );
    if( entry ) ptree[entry].ob_state &= ~SELECTED;
  }
  if( flag )
  {
    draw_title();
    guimenu[title].ob_state &= ~SELECTED;
    draw_title();
  }
  set_attaches( guimenu, 1, pull );
  ptree[pull].ob_x = old_mx;
  ptree[pull].ob_y = old_my;
}

int in_rect( int x, int y, Rect *r )
{
  x -= r->x;
  y -= r->y;
  return( x>=0 && x<r->w && y>=0 && y<r->h );
}

void entry_off(void)
{
  if( entry )
  {
    change_objc( ptree, menu_owner, entry, &max, ptree[entry].ob_state&~SELECTED, 1 );
    entry=0;
  }
}

void get_mks(void)
{
  mks_graf( (Mouse *)&curapp->mouse_x, 1 );
  if( have_mvec ) curapp->mouse_b = g_mb;
}

char *center_title( int title )
{
  char *p, *p2, *p3;
  int x;

  p2 = p = guimenu[title].ob_spec.free_string;
  x = 0;
  while( *p2 == ' ' )
  {
    p2++;
    x++;
  }
  p3 = p2;
  while( *p3 ) p3++;
  while( p3>p2 && *--p3==' ' && x>0 ) x--;
  return(p+x);
}

void new_entry( int x, int force )
{
  if( x!=entry )
  {
    entry_off();
    if( force || !(ptree[x].ob_state&DISABLED) )
    {
      change_objc( ptree, menu_owner, entry=x, &max, ptree[x].ob_state|SELECTED, 1 );
      entry_tic = tic();
    }
  }
}

int check_title( int x )
{
  while( guimenu[x].ob_state&DISABLED ) x = guimenu[x].ob_next;
  if( x<m_head ) return 0;
  return x;
}

void get_menu_alts( OBJECT *tree, int start, int draw )
{
  int i, neg;

  if( keybd )
  {
    neg = num_keys;	/* this is the title bar, so negate them */
    no_clip();
    fn_dir = 0;		/* scan all items */
    fn_last = draw;
    form_app = menu_owner;
    map_tree( tree, start, tree[start].ob_next, find_alt );
    if( !neg )
    {
      for( i=0; i<num_keys; i++ )
        alt_obj[i] = -alt_obj[i];
      neg_alts=1;
    }
    else neg_alts=0;
  }
}

void undraw_menu_alts(void)
{
  if( (num_keys=title_alts) != 0 )
  {
    no_clip();
    if( menu_wind==desktop ) form_redraw_all( (long)guimenu, 1 );
    else gredraw_obj( menu_wind, WMENU, 0L );
  }
}

int new_pull( int x )
{
  int y;

  if( /*%is_acc ||*/ in_sub )
  {
    if( !(guimenu[x].ob_state&DISABLED) ) next_pull = x;
    return 0;
  }
  if( title ) undraw( title, 1 );
  y = find_pull(title=x);
  objc_offset( guimenu, y, &pull_rect.x, &pull_rect.y );
  old_mx = guimenu[y].ob_x;
  old_my = guimenu[y].ob_y;
  if( pull_rect.x < 0 )
  {
    guimenu[y].ob_x -= pull_rect.x;
    pull_rect.x = 0;
  }
  if( pull_rect.x+(pull_rect.w=guimenu[y].ob_width) > (x=max.x+max.w) )
  {
    guimenu[y].ob_x -= (x=pull_rect.x+pull_rect.w-x+1);
    pull_rect.x -= x;
  }
  if( pull_rect.y+(pull_rect.h=guimenu[pull=y].ob_height) > (x=max.y+max.h) )
  {
    guimenu[y].ob_y -= (x=pull_rect.y+pull_rect.h-x+1);
    pull_rect.y -= x;
  }
  if( !(guimenu[title].ob_state&SELECTED) )
  {
    guimenu[title].ob_state |= SELECTED;
    draw_title();
  }
  guimenu[y].ob_x -= pull_rect.x&7;		/* 003 */
  pull_rect.x &= 0xFFF8;			/* 003 */
  if( menu_wind!=desktop || title!=m_head )
  {
    set_attaches( guimenu, 0, y );
    mblit( 0, &pull_rect );
    _objc_draw( (OBJECT2 *)guimenu, menu_owner, y, 8, Xrect(max) );
    num_keys = title_alts;
    stop = guimenu[y].ob_head;
    sbot = guimenu[y].ob_tail;
    sprev = -1;
    get_menu_alts( guimenu, y, 1 );
    entry=0;
  }
  else	/* Desk menu */
  {
/*%    acc_pop.mn_tree = acc_tree;
    acc_pop.mn_menu = 0;
    acc_pop.mn_item = 1;
    acc_pop.mn_scroll = 1;
    is_acc++;
    next_pull=0;
    y = pull;
    x = menu_popup( &acc_pop, pull_rect.x, pull_rect.y, &acc_pop );
    pull = y;
    is_acc=0;
    if( x>=0 ) pull_rect.w=0;
    if( next_pull )
    {
      acc_pop.mn_item = -1;
      new_pull(next_pull);
      return 1;
    }
    if( x<=0 ) acc_pop.mn_item = 0;
    if( x<0 ) return -1; */
    return 0;
  }
  return 1;
}

int last_entry( int horiz )
{
  int i, l;

  for( l=0, i=ptree[pull].ob_head; i!=pull; i=ptree[i].ob_next )
    if( !(ptree[i].ob_flags&HIDETREE) ) l=i;
  if( !horiz ) return l;
  if(l) new_entry(l,1);
  return 0;
}

void pull_it( int i )
{
  int f, l;

  if( (i = check_title(i)) == 0 ) i = check_title(m_head);
  if( (f=menu_wind->menu_tA) == 0 ) f=m_head;
  if( (l=menu_wind->menu_tZ) == 0 ) l=guimenu[2].ob_tail;
  if( i!=title )
  {
    entry_off();
    if( i < f || i > l )
    {
      if( title )
      {
        undraw( title, 1 );
        title=0;
      }
      menu_wind->menu_tA = i==m_head ? 0 : i;
      regenerate_rects(menu_wind);
      draw_menu_alts = menu_wind!=desktop;
      draw_wmenu(menu_wind);
      draw_menu_alts = 0;
    }
    if( new_pull(i) && pull ) new_entry(ptree[pull].ob_head,1);
  }
}

int test_sub( int flag )
{
  return menu_owner && menu_owner->menu_att &&
      ((OBJECT2 *)ptree)[entry].ob_typex>=128 && (!flag ||
      tic()-entry_tic > menu_set.Display) &&
      !(((OBJECT2 *)ptree)[entry].ob_state&DISABLED);
}

int menu_keybd( int horiz )
{  /* return:  0: disabled or err,  -2: menu start,  -3: new pull in sub
              -1: Esc (entry=0) CR or Space,  other: new entry */
  unsigned long k;
  int i, j, l;

  if( !entry ) return 0;
  k = getkey();
/*%  if( is_key( &settings->menu_start, k>>24, (k&0xff)|((k>>16L)&0xff00) ) ) return -2; */
  switch( (char)k )
  {
    case '\r':
    case ' ':
cr:   if( ptree[entry].ob_state&DISABLED ) return 0;
      return -1;
    case '\033':
      entry_off();
      return -1;
    default:
      i = (char)(k>>16L);
      if( i>=0x78 && i<=0x81 ) i-=0x78-0x2;
      j = Keytbl((void *)-1L, (void *)-1L, (void *)-1L)->unshift[i];
      if( j>='a' && j<='z' ) j &= 0xdf;
      for( i=0; i<num_keys; i++ )
        if( alt[i]==j )
          if( !title_alts )
          {
            new_entry( -(char)alt_obj[i], 1 );
            return -1;
          }
          else if( i<title_alts )
          {
            if( (horiz || /*%is_acc ||*/ in_sub) && -(char)alt_obj[i] != title )
            {
              entry_off();
              pull_it( -(char)alt_obj[i] );
              if( in_sub ) return -3;
              /*%if( is_acc ) return -1;*/
            }
            return 0;
          }
          else
          {
            new_entry( alt_obj[i], 1 );
            return -1;
          }
      i=ptree[pull].ob_head;
      switch( (char)(k>>16) )
      {
        case 0x1C:
        case 0x72:
          goto cr;
        case 0x61:	/* Undo */
esc:      entry_off();
          return -1;
        case 0x48:    /* up */
          if( k&(3L<<24) )
          {
            if( !horiz ) return i;
            new_entry(i,1);
          }
          else if( entry!=i )
          {
            while( (j=ptree[i].ob_next) != entry ) i=j;
            if( !horiz ) return i;
            new_entry(i,1);
          }
          else return last_entry(horiz);
          break;
        case 0x50:    /* down */
          if( k&(3L<<24) ) return last_entry(horiz);
          else if( (j=ptree[entry].ob_next) != pull &&
              !(ptree[j].ob_flags&HIDETREE) )
          {
            if( !horiz ) return j;
            new_entry(j,1);
          }
          else
          {
            if( !horiz ) return i;
            new_entry(i,1);
          }
          break;
        case 0x4b:    /* left */
          if( !horiz )
            if( in_sub ) goto esc;
            else /*%if( !is_acc )*/ break;
          for( l=0, i=m_head; i>=m_head; i=j )
          {
            j = guimenu[i].ob_next;
            if( !(guimenu[i].ob_state&DISABLED) ) l = i;
            if( j==title&&l>0 || j<m_head )
            {
              pull_it(l);
              break;
            }
          }
/*%          if( is_acc ) return -1; */
          break;
        case 0x4d:    /* right */
          if( test_sub(0) ) goto cr;
          else if( !horiz /*%&& !is_acc*/ ) break;
          if( (i=guimenu[title].ob_next) > m_head ) pull_it(i);
          else pull_it(m_head);
/*%          if( is_acc ) return -1;*/
      }
  }
  return 0;
}

int test_pop(void)
{
  if( next_menu ) return 1;
/*%  if( acc_pop.mn_item>=0 )
  {
    entry = acc_pop.mn_item;
    guimenu = acc_tree;
    return 1;
  } */
  return 0;
}

void menu_sel( OBJECT *tree, int ent )
{
  curapp->event = MU_MESAG;
  menu_buf[0] = menu_wind==desktop ? MN_SELECTED : X_MN_SELECTED;
  menu_buf[3] = title;
  menu_buf[4] = ent;
  *(OBJECT **)&menu_buf[5] = tree;
  menu_buf[7] = menu_wind==desktop ? pull : menu_wind->handle;
}

void drw_pop_al( int undraw, int top, int bot )
{
  int i, j;

  j=top;
  do
  {
    i = j;
    if( i != up && i != down ) drw_alt( ptree, neg_alts ? -i : i, undraw );
    j = ptree[i].ob_next;
  } while( i!=bot && i>=0 );
}

void draw_pop_alts( int undraw )
{
  int i, j;

  if( num_keys )
  {
    if( sprev>0 && stop != first ) drw_pop_al( undraw, first, sprev );
    drw_pop_al( undraw, stop, sbot );
  }
}

void alts_off(void)
{
  if( num_keys )
  {
    draw_pop_alts(1);
    undraw_menu_alts();
    num_keys = title_alts = 0;
  }
  keybd = 0;
}

int submenu( Rect *r, MENU *out )
{ /* return:  1: continue,  0: get out */
  int x, y, i, key;
  MENU *sub;
  SUB_DAT *sd;

  if( in_sub<4 )
  {
    pop_tic = tic();
    sub = &find_sub( menu_owner, (OBJECT2 *)ptree, entry, 0L )->menu;
    x = (ptree[pull].ob_x + ptree[pull].ob_width - char_w) & 0xfff8;
    if( x + (i=sub->mn_tree[sub->mn_item].ob_width) > max.x+max.w )
      if( (x = ptree[pull].ob_x - i) < 0 ) x = 0;
      else x &= 0xfff8;
    y = r->y + ptree[entry].ob_y;
    sd = &sub_dat[in_sub];
    sd->r = *r;
    sd->tree = ptree;
    sd->pull = pull;
    sd->entry = entry;
    sd->up = up;
    sd->down = down;
    sd->first = first;
    sd->sfirst = sfirst;
    sd->stop = stop;
    sd->sprev = sprev;
    sd->sbot = sbot;
    sd->last = last;
    key = keybd;
    in_sub++;
    next_pull=0;
    draw_pop_alts(1);
    num_keys = title_alts;
    i = menu_popup( sub, x, y, &sub_pop );
    in_sub--;
    if( !key ) keybd = 0;
    ptree = sd->tree;
    pull = sd->pull;
    entry = sd->entry;
    up = sd->up;
    down = sd->down;
    first = sd->first;
    sfirst = sd->sfirst;
    stop = sd->stop;
    sprev = sd->sprev;
    sbot = sd->sbot;
    last = sd->last;
    if( next_pull && !in_sub )
    {
      if( keybd ) pull_it(next_pull);
      else new_pull(next_pull);
      return pull;	/* continue if there is still a pull */
    }
    else if(i)
    {
      if( i>0 )
        if( is_menu )
        {
          menu_sel( sub->mn_tree, sub_pop.mn_item );
          menu_buf[7] = sub->mn_menu;
        }
        else if( out ) memcpy( out, &sub_pop, sizeof(MENU) );
      entry_off();
      return 0;
    }
    num_keys = title_alts;
    get_menu_alts( ptree, pull, 1 );
  }
  return 1;
}

int (*old_mbutv)( int b );

int butv( int buts );
/*%	in asm
{
  return (*old_mbutv)(g_mb = buts);
} */

void lock_menu( int lock )
{
  Mouse m;
  EMULTI e;
  int buf[8];
  
  wind_update( lock ? BEG_UPDATE : END_MCTRL );
  wind_update( lock ? BEG_MCTRL : END_UPDATE );
  if( lock<0 )
  {
    mks_graf( &m, 0 );
    g_mb = m.b;
    did_mclick = 0;
    vex_butv( vdi_hand, butv, &old_mbutv );
    have_mvec = 1;
  }
  else if( !lock && have_mvec )
  {
    vex_butv( vdi_hand, old_mbutv, &old_mbutv );
    if( did_mclick )
    {
      e.type = MU_BUTTON|MU_TIMER;
      e.clicks = e.mask = e.state = 1;
      e.low = 500;
      e.high = 0;
      do
        multi_evnt( &e, buf );
      while( !e.event );
    }
    have_mvec = 0;
  }
}

void domenu( APP *ap, int *buf, int keyybd )
{ /* keybd: 0=use settings->pulldown; <0=use current button; >0=keyboard */
  int x, y, first, last;
  Window *w;
  OBJECT *o;
  Rect drag;
  char has_mouse=0;

  if( !guimenu || menu_wind->menu_tZ<0/*003*/ ) return;
  lock_menu(-1);
  entry=title=0;
  keybd = keyybd;
  num_keys = title_alts = 0;	/* zero-out kbd equivs */
  menu_buf = buf;
  if( keybd>0 )
  {
    if( (x=check_title(m_head))==0 )
    {
      lock_menu(0);
      return;
    }
    get_menu_alts( guimenu, 2, menu_wind==desktop );  /* get title alts */
    title_alts = num_keys;
    if( menu_wind!=desktop )
    {
      regenerate_rects(menu_wind);
      draw_menu_alts = 1;
      undraw_menu_alts();
      draw_menu_alts = 0;
    }
/*    pulldown=1; */
    state=0;
/*%    force_mouse(0);*/
    has_mouse++;
    pull_it(x);
    if( test_pop() ) goto finish;
    get_mks();
  }
  else
  {
    pull=0;
    state = ap->mouse_b&1;
/*    pulldown = !keybd ? settings->flags.s.pulldown : state;*/
    keybd=0;
  }
  if( (first=menu_wind->menu_tA) == 0 ) first = guimenu[2].ob_head;
  last=menu_wind->menu_tZ;
/*  if( (last=menu_wind->menu_tZ) == 0 ) last = guimenu[2].ob_tail; 003 */
  if( keybd || in_rect( ap->mouse_x, ap->mouse_y, &guimin ) )
  {
    is_menu = 1;
    ap->event &= ~MU_BUTTON;
/*%    if( !has_mouse ) force_mouse(0);*/
    for(;;)
    {
      if( (ap->mouse_b&1) == state )
      {
        if( has_key() )
        {
          keybd = 1;
          if( (x=menu_keybd(1)) == -2 )
          {
            if( wind_menu() )
            {
              undraw( title, entry==0 );
              entry=0;
              goto finish;
            }
          }
          else if( test_pop() || x ) goto finish;
        }
        else if( !keybd && pull && (x=objc_find( guimenu, pull, 8, ap->mouse_x, ap->mouse_y )) > 0 )
        {
          alts_off();
          if( x != entry ) new_entry(x,0);
          else if( test_sub(1) )
            if( !submenu( &pull_rect, 0L ) ) goto finish;
        }
        else if( !keybd && (x=objc_find( guimenu, 2, 1, ap->mouse_x, ap->mouse_y )) >= first &&
            (!last/*003*/ || x<=last) )
        {     /* in bar */
          alts_off();
          if( x != title )
            if( guimenu[x].ob_state&DISABLED )
            {
              if( title )
              {
                undraw( title, 1 );
                pull=title=entry=0;	/* added pull for 002 */
              }
              goto get;
            }
            else
            {
              y = new_pull(x);
              if( test_pop() && y>=0 ) goto finish;
            }
          else entry_off();
          if( ap->mouse_k&4 && ap->mouse_b&1 )
          {
            drag = pull_rect;
            drag.w += 2;
            drag.h += 2;
            graf_mouse( FLAT_HAND, 0L );
            if( dragbox_graf( 0, &drag.x, &drag.y, drag.w,
                drag.h, drag.x, drag.y, max.x-drag.w, menu_h,
                max.x+max.w+(drag.w<<1), max.y+max.h+drag.h ) )
            {
              o = guimenu;
              guimenu = menu_wind->menu;
              undraw( title, 1 );
              guimenu = o;
              drag.h += cel_h;
/*%              if( guimenu==acc_tree ) drag.h -= char_h<<1;*/
              if( (drag.y-=cel_h) < desktop->working.y )
                  drag.y = desktop->working.y;
              if( (y=create_window( NAME|MOVER|CLOSER, 0, &drag )) > 0 )
              {
                w = find_window(y);
                w->dial_obj = IS_TEAR;
                w->bevent = settings->flags.s.tear_aways_topped;
                w->menu = guimenu;
                w->top_bar = center_title( title );
                w->apid = (menu_wind==desktop?has_menu->id:menu_wind->apid);
                w->hslide = title;
                w->vslide = pull;
                w->tear_parent = menu_wind;
                opn_wind(w);
              }
              title=pull=0;
            }
            graf_mouse( ARROW, 0L );
          }
        }
        else if( !keybd )
        {
/*          keybd = state^1; */
          entry_off();
        }
      }
      else if( keybd || !state && in_rect( ap->mouse_x, ap->mouse_y, &guimin ) )
      {
        alts_off();
        state=1;
      }
      else if( title )
      {
        if( keybd ) entry_off();
finish: o = menu_wind->menu;
        if( test_sub(0) )
          if( submenu( &pull_rect, 0L ) ) goto get;
/*%        if( entry && title==m_head )
          if( check_acc() ) entry = 0; */
        guimenu = o;
        if( menu_wind->menu==o )
        {
          undraw( title, entry==0 );
          undraw_menu_alts();
          title_alts=num_keys=0;	/* added for rel 003 */
        }
        else guimenu[title].ob_state &= ~SELECTED;
        while( ap->mouse_b&1 ) get_mks();
/*%        reset_butq();*/
        if( entry ) menu_sel( guimenu, entry );
        title=pull=num_keys=0;
/*%        force_mouse(1);*/
        is_menu = 0;
/*%        test_unload();                /* won't return if term worked */*/
        lock_menu(0);
        return;
      }
      else if( state )
      {
        undraw_menu_alts();
        title_alts=num_keys=0;
        is_menu = 0;
/*%        force_mouse(1);*/
        lock_menu(0);
        return;
      }
get:  get_mks();
    }
  }
  lock_menu(0);
}

void draw_entry( Rect *r )
{
  _objc_draw( (OBJECT2 *)ptree, menu_owner, entry, 0, Xrect(*r) );
}

void undraw2( Rect_list *r )
{
  Rect r2;

  while( r )
  {
    if( gintersect( r->r, pull_rect, &r2 ) )
    {
      ptree[entry].ob_state |= SELECTED;
      draw_entry( &r2 );
      ptree[entry].ob_state &= ~SELECTED;
      draw_entry( &r2 );
    }
    r = r->next;
  }
}

void move_menu( int flag, Window *w )
{
  static int x[3], y[3], opull[3], lev;
  static OBJECT *tree[3];
  int mx, my;
  OBJECT *o;

  if( !flag )
  {
    tree[lev] = ptree;
    choose_menu(w);
    opull[lev] = pull;
    x[lev] = (o=&ptree[pull=w->vslide])->ob_x;
    y[lev] = o->ob_y;
    objc_offset( ptree, pull, &mx, &my );
    o->ob_x += w->working.x-mx;
    o->ob_y += w->working.y-my;
    lev++;
  }
  else if( lev )
  {
    ptree[pull].ob_x = x[--lev];
    ptree[pull].ob_y = y[lev];
    ptree = tree[lev];
    pull = opull[lev];
  }
}

Window *menu_w( int *buf, int *opull )	/* 003 */
{
  Window *w;

  if( (w = find_window( buf[3] )) == 0L || w->dial_obj!=IS_TEAR ) return 0L;
  else
  {
    recalc_window( buf[3], w, 0L );
    regenerate_rects( w );
    move_menu( 0, w );  /* sets sd->pull */
    *opull = pull;
    return w;
  }
}

void menu_evnt( APP *ap, int *buf )
{
  SWT swt;
  Window *w;
  Rect_list *r;
  Rect r2;
  int i;

  w = 0L;
  if( ap->event & MU_KEYBD )
  {
    if( (char)ap->key == '\033' )   /* Esc key */
    {
      *(Rect *)&buf[4] = desktop->working;
      ap->event &= ~MU_KEYBD;
      goto redraw;
    }
  }
  if( ap->event & MU_MESAG )
  {
    switch( buf[0] )
    {
      case WM_REDRAW:
redraw:
        if( (w=menu_w( buf, &i )) == 0 ) return;	/* 003 */
        r = w->rects;
        while( r )
        {
          if( gintersect( r->r, *(Rect *)&buf[4], &r2 ) )
              _objc_draw( (OBJECT2 *)guimenu, menu_owner, i, 8, Xrect(r2) );
          r = r->next;
        }
        set_attaches( guimenu, 1, i );	/* 003 */
        break;
      case WM_TOPPED:
        if( (w=menu_w( buf, &i )) == 0 ) return;	/* 003 */
        _wind_set( w, WF_TOP, buf[3] );
        break;
      case WM_CLOSED:
        if( (w=menu_w( buf, &i )) == 0 ) return;	/* 003 */
        close_del(w);
        break;
      case WM_MOVED:
        if( (w=menu_w( buf, &i )) == 0 ) return;	/* 003 */
        _set_window( buf[3], WF_CURRXYWH, buf[4], buf[5], buf[6], buf[7] );
        break;
      default:		/* 003 */
        return;
    }
    ap->event &= ~MU_MESAG;
  }
  else if( ap->event & MU_BUTTON )
  {
    if( (w=menu_w( buf, &i )) == 0 ) return;	/* 003 */
    set_attaches( guimenu, 0, i );	/* 003 */
    pull_rect = w->working;
    entry = 0;
    for(;;)
    {
/*%      get_mks();   not for neo */
      if( ap->mouse_b&1 )
      {
        i = objc_find( guimenu, pull, 8, ap->mouse_x, ap->mouse_y );
        if( i != entry )
        {
          r = w->rects;
          if( entry ) undraw2(r);
          entry=0;
          if( i>0 )
            if( !(guimenu[i].ob_state&DISABLED) )
            {
              entry = i;
              while( r )
              {
                if( gintersect( r->r, pull_rect, &r2 ) )
                {
                  guimenu[i].ob_state |= SELECTED;
                  draw_entry(&r2);
                }
                r = r->next;
              }
/*%              if( test_sub(0) )  /* 003 */
              {
                is_menu = 1;
                i = submenu( &pull_rect, 0L, 1 );
                is_menu = 0;
                if( !i )
                {
                  undraw2( w->rects );
                  if( ap->event )	/* was it set by submenu? */
                  {
                    memcpy( buf, menu_buf, 16 );
                    buf[7] = sub_pop.mn_menu;
                    goto finish;
                  }
                  else break;
                }
              }  **/
            }
        }
      }
      else if( entry )
      {
        undraw2( w->rects );
        ap->event = MU_MESAG;
        buf[0] = menu_wind->tear_parent==desktop ? MN_SELECTED : X_MN_SELECTED;
        buf[1] = w->apid;
        buf[3] = w->hslide;
        buf[4] = entry;
        *(OBJECT **)&buf[5] = guimenu;
        buf[7] = menu_wind->tear_parent==desktop ? pull : menu_wind->tear_parent->handle;
        _menu_tnormal( guimenu, buf[3], 0 );
        set_attaches( guimenu, 1, i );	/* 003 */
        break;
      }
      else break;
      get_mks();   /* for neo */
    }
    ap->event &= ~MU_BUTTON;
/*%    reset_butq();*/
  }
  if(w) free_rects(w);		/* 003 */
  set_attaches( guimenu, 1, i );	/* 003 */
  move_menu( 1, menu_wind );	/* 003: was w */
}

void choose_menu( Window *w )
{
  int x;
  APP *ap;

  if( (menu_wind = w) == 0L )
  {
    guimenu = 0L;
    return;
  }
  ptree = guimenu = w->menu;
  max = desktop->outer;
  bar_h = menu_h - 1;
  if( w!=desktop )
  {
    objc_offset( w->tree, WMENU, &guimin.x, &guimin.y );
    *(long *)&guimin.w = *(long *)&w->tree[WMENU].ob_width;
    menu_owner = 0L;
    for( ap=app0; ap; ap=ap->next )
      if( ap->id==w->apid ) menu_owner = ap;
  }
  else
  {
    guimin.x = desktop->outer.x;
    guimin.y = desktop->outer.y;
    guimin.w = desktop->outer.w;
    guimin.h = bar_h;
    menu_owner = has_menu;
  }
  if( guimenu ) m_head = guimenu[2].ob_head;
}

int menu_k(OBJECT *ob, int obj)
{
  ob += obj;
  if( ob->ob_flags&HIDETREE || mnuk_out ) return 0;
  if( ob->ob_state&DISABLED ) return 1;
  if( (ob->ob_state>>8) == mnuk && (unsigned)(ob->ob_flags)>>13 == mnush )
  {
    mnuk_out=obj;
    return 0;
  }
  return 1;
}

int _menu_equiv( Window *w, int *buf, int sh, int key )
{
  int parent, i, j, e;

  if( !w || !w->menu || (w->menu[0].ob_state&X_MAGMASK)!=X_MAGIC ||
      w->icon_index || (mnuk = key>>8)==0/*006*/ ) return 0;
  if( w->dial_obj==IS_TEAR && w->menu==desktop->menu ) w=desktop;	/* 003 */
  choose_menu( w );
  mnush = ((sh&3) != 0) | ((sh&12)>>1);
  mnuk_out=0;
  map_tree( guimenu, 0, -1, menu_k );
  if( mnuk_out )
  {
    parent = gfind_parent( guimenu, mnuk_out );
    for( i=guimenu[e=guimenu[guimenu[0].ob_head].ob_next].ob_head, j=0; i!=e;
        i=guimenu[i].ob_next, j++ )
      if( i==parent )
      {
/*        for( i=m_head; --j>=0; i=guimenu[i].ob_next );
        title=i; 003 */
        title = i = m_head+j;
        if( menu_wind!=desktop )
        {
          guimenu[i].ob_state |= SELECTED;
          draw_title();
        }
        else menu_tnormal( guimenu, i, 0 );
        buf[0] = menu_wind==desktop ? MN_SELECTED : X_MN_SELECTED;
        buf[3] = i;
        buf[4] = mnuk_out;
        *(OBJECT **)&buf[5] = guimenu;
        buf[7] = menu_wind==desktop ? parent :
            (menu_wind->dial_obj==IS_TEAR ? menu_wind->tear_parent->handle :	/* 003 */
            menu_wind->handle);
        return 1;
      }
  }
  return 0;
}

int menu_equiv( int *buf, int sh, int key )
{
  if( _menu_equiv( top_wind, buf, sh, key ) ) return 1;
  if( has_menu == curapp )
      return _menu_equiv( desktop, buf, sh, key );
  return 0;
}

void user_menu(void)
{
  int i;
  long *l1, *l2;

  for( i=4, l1=&user_mset.Display, l2=&menu_set.Display; --i>=0; )
    *l2++ = *l1++ / ticcal;             /* convert to 50 Hz timer tics */
  menu_set.Height = user_mset.Height*char_h;
}

int menu_settings( int flag, MN_SET *values )
{
  if( flag==0 ) memcpy( values, &user_mset, sizeof(MN_SET) );
  else if( values )
  {
    memcpy( &user_mset, values, sizeof(MN_SET) );
    user_menu();
  }
  return 1;
}

TEDINFO upted =
{
  "", "", "", IBM, 0, 2, 1<<8, 0, 0, 2, 0
},
dwnted =
{
  "", "", "", IBM, 0, 2, 1<<8, 0, 0, 2, 0
};

void pop_text( OBJECT *tree, int num, int ind, int reset )
{
  static int old[2][5];

  tree += num;
  if( !reset )
  {
    /* save type, flags, state, spec */
    memcpy( old[ind], &tree->ob_type, 10 );
    tree->ob_type = G_BOXTEXT;
    tree->ob_flags = tree->ob_state = 0;
    tree->ob_spec.tedinfo = ind ? &dwnted : &upted;
  }
  else memcpy( &tree->ob_type, old[ind], 10 );
}

void part_draw( OBJECT *tree, int root, int first, int last )
{
  Rect r;

  objc_offset( tree, first, &r.x, &r.y );
  *(long *)&r.w = *(long *)&tree[first].ob_width;
  r.h += tree[last].ob_y - tree[first].ob_y;
  form_app = menu_owner;
  adjust_rect( tree+first, &r, 1 );
  _objc_draw( (OBJECT2 *)tree, menu_owner, root, 8, Xrect(r) );
  draw_pop_alts(0);
}

void set_mpos( OBJECT *tree, int root, Rect *r,
    int downscroll, int *itemoff, int offset, int draw )
{
  int i, end, h;
  Rect r2, r3;

  h = tree[sfirst].ob_height;
  if( downscroll )
  {
    i = *itemoff;
    end = last-downscroll;
    if( (i += offset) < 0 ) i = 0;
    else if( i > end ) i = end;
    if( draw )
    {
      if( up ) pop_text( tree, up, 0, 1 );
      if( down ) pop_text( tree, down, 1, 1 );
      up=down=0;
    }
    if( i ) pop_text( tree, up=sfirst+i, 0, 0 );
    if( i<end ) pop_text( tree, down=downscroll+i, 1, 0 );
    tree[root].ob_y += (*itemoff-i) * h;
    if( i==*itemoff ) return;   /* no draw */
    *itemoff = i;
  }
  if( draw )
    if( offset>1 || offset<-1 || !offset )
    {
      _objc_draw( (OBJECT2 *)tree, menu_owner, root, 8, Xrect(*r) );
      draw_pop_alts(0);
    }
    else
    {
      r2 = *r;
      r2.h -= h*3-1;
      r2.y += h;
      r3 = r2;
      r3.y += h;
      first += *itemoff;
      sbot = downscroll += *itemoff;
      stop = sfirst + *itemoff;
      if( offset>0 )
      {
        part_draw( tree, root, stop, stop );
        x_graf_blit( (GRECT *)&r3, (GRECT *)&r2 );
        part_draw( tree, root, downscroll-1, downscroll );
      }
      else
      {
        part_draw( tree, root, downscroll, downscroll );
        x_graf_blit( (GRECT *)&r2, (GRECT *)&r3 );
        part_draw( tree, root, stop, stop+1 );
      }
    }
}

int pop_pause(void)
{
  unsigned long t = tic();
  
  if( !scroltic1 ) scroltic1 = t+menu_set.Delay;
  if( t < scroltic1 ) return 0;
  if( !scroltic2 ) scroltic2 = t+menu_set.Speed;
  if( t < scroltic2 ) return 0;
  scroltic2 = 0;
  return 1;
}

int pop_obj( APP *ap, Rect *r, OBJECT *tree )
{
  int i;

  if( in_rect( ap->mouse_x, ap->mouse_y, r ) &&
      (i=objc_find( tree, pull, 8, ap->mouse_x, ap->mouse_y )) > 0 &&
      !(tree[i].ob_state&DISABLED) )
  {
    pop_tic = tic() - menu_set.Display - 1;
    return i;
  }
  return 0;
}

int sub_obj( int x, int y )
{
  int j, i;

  if( tic()-pop_tic > menu_set.Display )
    for( j=in_sub; --j>=1; )		/* 003: changed order */
      if( in_rect( x, y, &sub_dat[j].r ) &&
          (i=objc_find( sub_dat[j].tree, sub_dat[j].pull, 8, x, y )) > 0 )
        if( i==sub_dat[j].entry )
        { /* returned to a parent sub starter */
          pop_tic = tic();
          return 1;
        }
        else return 2;
  return 0;
}

int menu_popup( MENU *mnu, int xpos, int ypos, MENU *mdata )
{  /* return: -1: drag Desk menu, -2: exit w/o entry (in_sub), 0: no entry, 1: OK */
  Rect r, r2, r3;
  OBJECT *tree, *old;
  int downscroll=0;
  int i, xy[2], pxy[2], itemoff=0, root, ret=0, top;
  APP *ap = curapp;
  char ok=0;
  static OBJECT *menu_root;

  lock_menu(-1);
  tree = mnu->mn_tree;
  root = mnu->mn_menu;
  if( /*%!is_acc &&*/ !in_sub ) menu_owner = curapp;
  set_attaches( tree, 0, root );
  first = tree[root].ob_head;
  /* 003: avoid problem where tree order is weird */
  for( i=top=first; i!=root; i=tree[i].ob_next )
    if( tree[i].ob_y < tree[i].ob_y ) top = i;
  if( (sfirst = mnu->mn_scroll)==0 ) sfirst=first;
  r.h = 0;
  for( last=i=first; i!=root; last=i, i=tree[i].ob_next )
  {
    /* make sure sfirst is a child of root (avoid mn_scroll==1 falsely) */
    if( i==sfirst ) ok=1;
    if( mnu->mn_scroll && r.h >= menu_set.Height )
    {
      if( !downscroll ) downscroll = last;
    }
    else if( (xy[0] = tree[i].ob_y + tree[i].ob_height) > r.h ) r.h = xy[0];  /* 003: find last item */
  }
  if( !ok ) sfirst = first;
  for( sprev=-1, i=first; i!=root; i=tree[i].ob_next )
    if( tree[i].ob_next == sfirst ) sprev = i;	/* find last non-scrollable obj */
  first = top;
  if( downscroll && mnu->mn_item > downscroll )
    if( (last-sfirst+1) - (itemoff = mnu->mn_item-sfirst-1) < downscroll-sfirst+1 )
        itemoff = last-downscroll;
  tree[root].ob_x = tree[root].ob_y = 0;
  objc_offset( tree, root, pxy, pxy+1 );
  r.x = xpos;
  tree[root].ob_x = xpos - pxy[0];
  objc_offset( tree, mnu->mn_item-itemoff, xy, xy+1 );
  if( (i = ypos-xy[1])+pxy[1] < desktop->working.y ) i =
      desktop->working.y-pxy[1];
  else if( i+r.h > desktop->working.y+desktop->working.h-4 ) i =
      desktop->working.y+desktop->working.h-4-r.h;
  tree[root].ob_y = i;
  r.y = i + pxy[1];
  r.w = tree[root].ob_width;
  r2=r;
  r2.x--; r2.y--;
  r2.w+=2; r2.h+=2;
  form_app = menu_owner;
  adjust_rect( tree+root, &r2, 1 );
  if( !mblit( 0, &r2 ) ) goto quit;
  i = tree[root].ob_height;
  tree[root].ob_height = r.h;
  _objc_draw( (OBJECT2 *)tree, menu_owner, root, 0, Xrect(r2) );
  tree[root].ob_height = i;
  if( (i=tree[root].ob_spec.obspec.framesize)>0 ) r.h += i;
  r3 = r;
  r.y += tree[sfirst].ob_y;
  r.h -= tree[sfirst].ob_y;
  up=down=0;
  i = itemoff;
  itemoff = 0;
  get_mks();
  if( /*%!is_acc &&*/ !in_sub )
  {
    keybd = 0;
    state = ap->mouse_b&1;
  }
  num_keys = /*%is_acc ||*/ in_sub ? title_alts : 0;
  get_menu_alts( tree, root, 0 );
  old = ptree;
  if( /*%is_acc ||*/ in_sub==1 ) menu_root = ptree;
  ptree = tree;
  stop = sfirst+itemoff;
  sbot = downscroll ? downscroll+itemoff : last;
  if( sfirst!=first ) part_draw( tree, root, first, sbot );
  set_mpos( tree, root, &r, downscroll, &itemoff, i, 0 );
  _objc_draw( (OBJECT2 *)tree, menu_owner, root, 8, Xrect(r) );
  draw_pop_alts(0);
  entry=0;
  pull = mnu->mn_menu;
  if( keybd != 0 ) new_entry(mnu->mn_item,1);
  for(;;)
  {
    if( !is_menu && has_key()/*003*/ || (ap->mouse_b&1) == state )
    {
      if( has_key() )
      {
        if( !keybd )	/* 003 */
        {
          keybd=1;
          get_menu_alts( tree, root, 0 );
          draw_pop_alts(0);
        }
        ok = 1;		/* 003 */
        if( !entry )
        {
          new_entry( sfirst+itemoff, 1 );
          ok = 0;	/* 003: go to first item in list if dn arrow */
        }
        switch( i=menu_keybd(0) )
        {
          case -1:
            goto finish;
          case -2:
/*%            if( is_acc )
              if( wind_menu() )
              {
                entry_off();
                goto finish;
              } */
            break;
          case -3:
            ret = -2;
            goto finish;
          case 0:
            break;
          default:
            if( up && i<=up || down && i>=down )
            {
              xy[0] = i-entry;
              entry_off();
              set_mpos( tree, root, &r, downscroll, &itemoff, xy[0], 1 );
              new_entry(i,1);
            }
            else if( ok || i!=entry+1 /* 003 */ ) new_entry(i,1);
        }
      }  /* over an entry in the same popup */
      else if( (!keybd || !is_menu/*003*/) && (i=pop_obj(ap,&r3,tree)) > 0 )
      {
        alts_off();
        if( i != entry )
        {
          scroltic1 = scroltic2 = 0;
          new_entry(i,0);
        }
        else if( (i==down||i==up) && pop_pause() )
        {
          entry_off();
          i = i==up ? -1 : 1;
          set_mpos( tree, root, &r, downscroll, &itemoff, i, 1 );
          if( (i = i==-1?up:down) != 0 ) new_entry(i,0);
        }
        else if( test_sub(1) )  /* has a submenu */
          if( !submenu( &r3, mdata ) )
          {
            if( is_menu ) ret = -2;
            else
            {
              ret = 1;
              mdata->mn_keystate = ap->mouse_k;
            }
            goto finish;
          }
      }
      else if( !keybd && is_menu && (i=objc_find( menu_root, 2, 1, ap->mouse_x,
          ap->mouse_y )) > 0 )
/*%        if( i==m_head && ap->mouse_k&4 && ap->mouse_b&1 && acc_count )
        { /* drag Desk menu */
          alts_off();
          ret = -1;
          entry_off();
          break;
        }
        else if( i!=m_head || !is_acc ) */
        { /* new sub */
          alts_off();
          new_pull(i);
          entry_off();
          break;
        }
/*%        else entry_off();*/
      else if( /* 003 !keybd &&*/ sub_obj( ap->mouse_x, ap->mouse_y ) == 2 )
      { /* on another entry in a parent submenu */
        alts_off();
        entry_off();
        goto finish;
      }
      else if( !keybd )
      { /* not on anything */
/*        keybd = state^1; */
        entry_off();
      }
    }
    else if( keybd || !state && in_rect( ap->mouse_x, ap->mouse_y, &guimin ) )
    { /* returned to menu bar */
      alts_off();
      state=1;
    }
    else if( entry && (entry==up || entry==down) )	/* 003 */
    {	/* clicked on up/down arrow */
      state = keybd = 0;
    }
    else
    {
      if( sub_obj( ap->mouse_x, ap->mouse_y ) == 1 ) goto get;
      if( !pop_obj( ap, &r3, tree ) /* || keybd*/ ) entry_off();
      if( !entry && in_sub ) ret = -2;
finish:
      if( entry && test_sub(0) )	/* clicked on sub -> */
      {
        if( submenu( &r3, mdata ) ) goto get;
        if( is_menu ) ret = -2;
        else
        {
          ret = 1;
          mdata->mn_keystate = ap->mouse_k;
        }
      }
      if( entry ) tree[entry].ob_state &= ~SELECTED;
      if( !in_sub )
      {
        while( ap->mouse_b&1 ) get_mks();
/*%        reset_butq();*/
      }
      break;
    }
get:get_mks();
  }
  if( in_sub || ret>=0 ) mblit( 1, &r2 );
  else pull_rect = r2;
  if( downscroll )
  {
    if( up ) pop_text( tree, up, 0, 1 );
    if( down ) pop_text( tree, down, 1, 1 );
  }
  if( entry && !(ptree[entry].ob_state&DISABLED) )
  {
    memcpy( mdata, mnu, sizeof(MENU) );
    mdata->mn_item = entry>=stop || entry<sfirst ? entry : entry-itemoff;
    mdata->mn_keystate = ap->mouse_k;
    ret = 1;
  }
quit:
  set_attaches( ptree, 1, root );
  entry=num_keys=0;
  ptree = old;
  lock_menu(0);
  return ret;
}

#if 0
int menu_attach( int flag, OBJECT *tree, int item, MENU *mdata )
{
  INT_MENU *im;
  unsigned int ii;

  im = find_sub(curapp,(OBJECT2 *)tree,item,&ii);
  switch( flag )
  {
    case 0:     /* inquire */
      if( !im ) return 0;
      if( mdata ) memcpy( mdata, &im->menu, sizeof(MENU) );
      return 1;
    case 1:     /* new */
      if( mdata )
      {
        if( !im )
          if( !curapp->menu_att && (curapp->menu_att =
              (INT_MENU *)lalloc(64*sizeof(INT_MENU), curapp->id)) == 0 )
          {
            no_memory();
            return 0;
          }
          else if( curapp->attaches>=64 ) return 0;     /* limit attaches */
          else im = &curapp->menu_att[ii=curapp->attaches++];
        else im->parent->ob_typex = 0;
        memcpy( &im->menu, mdata, sizeof(MENU) );
        (im->parent = (OBJECT2 *)tree + item)->ob_typex = ii+128;
        return 1;
      }
    case 2:     /* delete */
      if( !im ) return 0;
      if( !--curapp->attaches )
      {
        lfree(curapp->menu_att);
        curapp->menu_att=0;
      }
      else
      {
        memcpy( im, im+1, (ii=curapp->attaches-ii)*sizeof(INT_MENU) );
        for( ; ii-- > 0; im++ )
          im->parent->ob_typex--;
      }
  }
  return 0;
}

int menu_istart( int flag, OBJECT *tree, int imenu, int item )
{
  INT_MENU *im;
  int i;

  if( (im=curapp->menu_att) == 0 ) return 0;
  for( i=curapp->attaches; --i>=0; im++ )
    if( im->menu.mn_tree==tree && im->menu.mn_menu==imenu )
    {
      if( flag ) im->menu.mn_item = item;
      return im->menu.mn_item;
    }
  return 0;
}

#endif

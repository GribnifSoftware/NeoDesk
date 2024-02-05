#include "vdi.h"
#include "new_aes.h"
#include "tos.h"
#include "xwind.h"
#include "string.h"
#include "..\neocommn.h"
#include "graphics.h"
#define _APPLIC
#include "win_var.h"
#include "win_inc.h"
#include "windows.h"
#include "windows.rsh"

#ifndef DEBUG
  GRAPHICS *graphics;
  MASTER *mas;
#else
  extern GRAPHICS *graphics;
  extern MASTER *mas;
#endif

#include "initial.h"

int gui_init( GEMPARBLK *blk );
void gui_exit( int hand, int apexit );
int nx_settings( int getset, int length, NSETTINGS *user );

static EXEGUI exe_gui = { gui_init, gui_exit, _multi_evnt, _evnt_timer, rsrc_rcfix,
    _obj_draw, objc_change, objc_offset, _form_dial, form_center, form_do,
    form_button, x_form_center, x_form_error, x_wdial_draw, x_wdial_change,
    menu_popup, menu_text, menu_icheck, menu_ienable, _menu_tnormal,
    graf_slidebox, x_graf_rubberbox, x_graf_blit,
    x_wind_calc, x_wind_create, _wind_open, _wind_close, _wind_delete, _wind_get,
    _set_window, _wind_update, _wind_new, x_wind_tree, set_dc, _gad_redraw,
    gmenu_bar, _arrow_dial, form_keybd, objc_find, nx_settings,
    &dum_msg, x_scrp_get };

int ind_move=1, ind_change=1, act_move=1, act_change=0;
int ind_col, act_col, bkgrnd_col, add3d_h=2, add3d_v=2, _aes_ver;
unsigned char alt_obj[MAX_KEYS], alt[MAX_KEYS], alt_off[MAX_KEYS];
char is_acc;
static NEO_ACC *nac;

long gui_load( NEO_ACC *_nac, int aes_ver )	/* must be first! */
{
  int dum[10], i, work_out[57], color_mode;
  static char mod[] = { 3, 2, 3, 3, 3,
                        11, 14, 11,
                        0,
                        11, 14, 11,
                        3, 1, 1, 3,
                        0,
                        3, 1, 1, 3,
                        3, 6, 2, 3,  0,  3, 6, 2, 3,  3 };
  OBJECT *o;
  char *m, dcolor;
  LoadCookie *lc;
  
  if( getcookie( LOAD_COOKIE, (long *)&lc ) != CJar_OK ) return 0L;
  clip_ini = dflt_clip_ini;
  clip_it = dflt_clip;
  vdi_hand = (graphics=(mas=(nac=_nac)->mas)->most->graphics)->handle;
#ifndef DEBUG
  io = &mas->most->kbio;
#endif
  vdi_reset();
  clear_mouse = mas->clear_mouse;
  (*_nac->set_temps/*002*/)( vplanes = graphics->vplanes );
  v_bpp = (*_nac->test_rez)();
  drw_image = _nac->draw_image;
  if( vplanes>=8 ) color_mode = 3;
  else if( vplanes>=4 ) color_mode = 2;
  else if( vplanes>=2 ) color_mode = 1;
  else color_mode = 0;
  wcolor_mode = color_mode>2 ? 2 : color_mode;
  undo_ptr = mas->most->msg_ptr[178];
  unkn_err = mas->most->msg_ptr[179];
  is_acc = mas->most->is_acc;	/* 002 */
  settings = &mas->most->gui_settings;	/* 002 */
  dflt_errors = (ERRSTRUCT *)(mas->dflt_errors);
  graf_handle( &char_w, &char_h, &cel_w, &cel_h );
  if( (font_mode = Getrez()) > 8 ) font_mode = 8;	/* 004: was Getrez()-2 !! */
  font_id = fontinfo[font_mode].font_id = 1;
  ptsiz = fontinfo[font_mode].point_size = char_h > 8 ? 10 : 9;
  font_scalable = 0;
  ind_col = act_col = bkgrnd_col = color_mode>=2 ? 0x1178 : 0x1170;
  dcolor = (_aes_ver=aes_ver)/*003*/ >= 0x330;
  if( aes_ver >= 0x340 && aes_ver != 0x399 )  /* MagiX */
  {
    dum[0] = 0;
    dum[1] = 0;
    if( aes_ver >= 0x410 )
    {
      if( appl_getinfo( 11, dum, dum+2, dum+2, dum+2 ) ) dcolor = dum[0]&(1<<3);
      appl_getinfo( 13, dum, dum+1, dum+2, dum+2 );
    }
    if( dum[1] )
    {
      objc_sysvar( 0, LK3DIND, 0, 0, &ind_move, &ind_change );
      objc_sysvar( 0, LK3DACT, 0, 0, &act_move, &act_change );
      objc_sysvar( 0, INDBUTCOL, 0, 0, dum, dum+1 );
      ind_col = 0x1170 | dum[0];
      objc_sysvar( 0, ACTBUTCOL, 0, 0, dum, dum+1 );
      act_col = 0x1170 | dum[0];
      objc_sysvar( 0, BACKGRCOL, 0, 0, dum, dum+1 );
      bkgrnd_col = 0x1170 | dum[0];
      objc_sysvar( 0, AD3DVALUE, 0, 0, &add3d_h, &add3d_v );
    }
/*    if( aes_ver > 0x340 ) appl_getinfo( 0, &fontinfo[font_mode].point_size, &fontinfo[font_mode].font_id, dum, dum );  003: removed because it's not right */
  }
  if( mas->state == MINIT )	/* 004 if */
    for( i=0; i<19; i++ )
  {
    if( dcolor )
    {
      dum[0] = i;
      wind_get( 0, WF_DCOLOR, dum, &dum[2], &dum[1] );
    }
    else *(long *)&dum[1] = *(long *)&(*lc->w_colors)[i][0];
    _set_window( 0, WF_DCOLOR, i, dum[2], dum[1] );
  }
  mas->most->wind_font[5].id = fontinfo[font_mode].font_id;
  mas->most->wind_font[5].size = fontinfo[font_mode].point_size;
  (*graphics->load_fonts)( mas->most->wind_font, 6 );
  ptsiz = _vst_point( fontinfo[font_mode].point_size, dum, dum,
      &char_w, &char_h );
  fontinfo[font_mode].gadget_wid = cel_w - char_w;
  fontinfo[font_mode].gadget_ht = cel_h - char_h;
  menu_h = char_h + 3;
  ticcal = 5;
  set_dc( dc_rate = evnt_dclick( 0, 0 ) );
  user_menu();		/* always re-fix menu height */
  dflt_desk = dflt_desk0 = rs_trindex[DESK];
  dtree_cnt = DTREECNT;
  dflt_wind = rs_trindex[WIND];
  for( o=&dflt_wind[WCLOSE], m=mod, i=0; i<sizeof(mod); i++, o++ )
  {
    if( *m&1 ) o->ob_width = cel_w;
    if( *m&2 ) o->ob_height = i>=9 && i<=11 ? menu_h : cel_h;
    if( *m&4 ) o->ob_x = cel_w-1;
    if( *m++&8 ) o->ob_y = cel_h-1;
  }
  vq_extnd( vdi_hand, 0, work_out );
  dflt_wind[WHSPLIT].ob_width = 5 * work_out[4] / work_out[3];
  dflt_wind[WVSPLIT].ob_height = 5 * work_out[3] / work_out[4];
  if( (desktop = (Window *)lalloc( sizeof(Window), -1 )) == 0L ) return 0;
  memset( top_wind=desktop, 0, sizeof(Window) );
  desktop->outer.w = work_out[0]+1;
  desktop->outer.h = work_out[1]+1;
  desktop->working.y = menu_h;
  desktop->working.w = work_out[0]+1;
  desktop->working.h = work_out[1]+1-menu_h;
  desktop->full = desktop->working;
  desktop->prev = desktop->outer;
  desktop->tree = dflt_desk;
  desktop->treeflag = -1;
  desktop->treecnt = DTREECNT;
  *(Rect *)&desktop->tree->ob_x = desktop->outer;
  desk_obj = 0;
  _wind_get( 0, WF_SCREEN, &pull_buf.i[0], &pull_buf.i[1],
      &pull_siz.i[0], &pull_siz.i[1] );		/* may need desktop */
  if( (pull_buf.l = (long)lalloc( pull_siz.l, -1 )) == 0L ) return 0;
  fix_trees();
  if( init_aph( mas->most->gemparblk->global[2] ) ) return (long)&exe_gui;
  return 0L;
}

int ofixw, ofixh;

void fixx2( int *i )
{
  *i = ((*i&0xFF)*ofixw) + (*i>>8);
}

void fixy2( int *i )
{
  *i = ((*i&0xFF)*ofixh) + (*i>>8);
}

int odd_obfix( OBJECT *tree, int ind )
{
  int *i = &tree[ind].ob_x;

  fixx2( i++ );
  fixy2( i++ );
  fixx2( i++ );
  fixy2( i );
  if( ofixw==6 && (char)tree[ind].ob_type==G_STRING )
      tree[ind].ob_state |= (X_MAGIC|X_SMALLTEXT);
  return(1);
}

void fix_trees(void)
{
  int i, j;

  if( desktop->outer.w > char_w*51 )
  {
    ofixw = char_w;
    ofixh = char_h;
  }
  else ofixw=ofixh=6;
  for( i=0; i<sizeof(ascii_tbl)/sizeof(OBJECT); i++ )
    odd_obfix( ascii_tbl, i );
  ob_fixspec();
}

int init_aph( int apno )
{
  APP *aph, *aph2, *prev;

  if( (aph2=app0) != 0 )
  {
    for( prev=0L; aph2 && aph2->id<apno; prev=aph2,
        aph2=aph2->next );
    aph2 = prev;
  }
ok:
  if( (aph = (APP *)lalloc(sizeof(APP),-1)) == 0 ) return(0);
  else
  {
    memset( aph, 0, sizeof(APP) );
    aph->id = apno;
    memcpy( &aph->flags, &dflt_flags, sizeof(APPFLAGS) );
    if( !aph2 )
    {					/* first app in list */
      if( (aph->next = app0) != 0 )	/* if there was a first */
          app0->prev = aph;		/* point it back to me */
      app0 = aph;			/* set first to me */
    }
    else if( (aph->next = aph2->next) != 0 )	/* insert into list */
        aph->next->prev = aph;			/* if next, point to me */
    if( (aph->prev = aph2) != 0 ) aph2->next = aph;
    aph->ap_type = 2;
    aph->waiting = -1;	/* initial value */
    set_curapp(aph);
    return(1);
  }
}

int set_asc_cur( int x, int y )
{
  int ret, w;

  y--;
  ret = (x-ascii_tbl[0].ob_x)/(w=ascii_tbl[0].ob_width/51);
  ascii_tbl[6].ob_x = ret*w;
  ascii_tbl[6].ob_y = y*ascii_tbl[1].ob_height;
  ascii_tbl[6].ob_flags &= ~HIDETREE;
  return ret + y*51 + 1;
}

int x_key( unsigned char num, char *tbl )
{
  int i;

  for( i=128; --i>=0; )
    if( *tbl++==num )
    {
      lastkey = ((127-i)<<8) | num;
      return 1;
    }
  return 0;
}

void trans_key( unsigned char num )
{
  KEYTAB *kt=Keytbl((void *)-1L, (void *)-1L, (void *)-1L);

  lastsh = 0;
  if( x_key( num, kt->unshift ) ) return;
  if( x_key( num, kt->shift ) || x_key( num, kt->capslock ) )
  {
    lastsh = 1;
    return;
  }
  lastkey = num;
}

#ifdef PRINT_ALLOCS
void *__p_lalloc( long size, int id, char *file, int line )
{
  void *out;
#ifdef DEBUG
  extern int AES_handle;
  
  if( id==-1 ) id = AES_handle-500;
#endif
  out = (void *)((*mas->lalloc)( size, id, 1 ));
#ifdef DEBUG
  prnall( "%s %d: alloc(%D,%d) \tret=$%X", file, line, size, id, out );
#endif
  return out;
}
#else
void *_lalloc( long size, int id )
{
#ifdef DEBUG
  extern int AES_handle;
  
  if( id==-1 ) id = AES_handle-500;
#endif
  return (void *)((*mas->lalloc)( size, id, 1 ));
}
#endif

#ifdef PRINT_ALLOCS
int __p_lfree( void *xfb, char *file, int line )
{
  int out = (*mas->lfree)( xfb );

#ifdef DEBUG
  prnall( "%s %d: free($%X) \tret=%d", file, line, xfb, out );
#endif
  return out;
}
#else
int _lfree( void *xfb )
{
  return (*mas->lfree)( xfb );
}
#endif

int _lfreeall( int id )
{
  return (*mas->lfreeall)( id<0 ? id : 1000+id );
}

static void close_all( APP *ap, int all )
{
  Window *w, *w2;	/* added look ahead for 003 */

  for( w2=desktop->next; (w=w2)!=0; )
  {
    w2 = w->next;
    if( !ap && w->place>=-1 || ap && w->apid==ap->id )  /* 003: added ap && */
      if( all || w->place>=0 ) close_del(w);	/* 003: was >= -1 */
  }
}

int gui_init( GEMPARBLK *blk )
{
  int ret;
  
  ret = appl_init();
  if( blk ) memcpy( blk, &_GemParBlk, sizeof(GEMPARBLK) );
  if( !init_aph( ret ) ) return -1;
  return ret;
}

APP *find_curapp( int hand )
{
  APP *ap;
  
  for( ap=app0; ap; ap=ap->next )
    if( ap->id == hand ) return curapp=ap;
  return curapp=0L;
}

void gui_exit( int hand, int apexit )
{
  if( find_curapp(hand) ) close_all( curapp, 0 );
  lfreeall(hand);
  if( apexit ) appl_exit();
}

void set_curapp( APP *ap )
{
  curapp=ap;
}

long get_tic(void)
{
  return *(long *)0x4ba;
}

unsigned long tic(void)
{
  return Supexec( get_tic );
}

int nx_settings( int getset, int len, NSETTINGS *user )
{
  if( (unsigned)len>(unsigned)sizeof(NSETTINGS) ) len=sizeof(NSETTINGS);
  if( getset==-1 ) return 0;
  else if( getset )
  {
    if( len>4 ) memcpy( (int *)&settings->struct_len+1,
        (int *)&user->struct_len+1, len-4 );
  }
  else if( user ) memcpy( user, settings, len );
  return 1;
}

#ifndef DEBUG
void map_tree( OBJECT *tree, int this, int last, int func( OBJECT *tree, int tmp ) )
{
  int tmp1;

  tmp1 = 0;
  while (this != last && this != -1)
    if (tree[this].ob_tail != tmp1)
    {
      tmp1 = this;
      this = -1;
      if( (*func)( tree, tmp1 ) ) this = tree[tmp1].ob_head;
      if (this == -1) this = tree[tmp1].ob_next;
    }
    else
    {
      tmp1 = this;
      this = tree[tmp1].ob_next;
    }
}

void no_memory(void)
{
  Bconout( 2, 7 );
}

void spf(char *buf, char *fmt, ...) {
  (*mas->dopf)(buf, fmt, (unsigned int *)&...);
}
#endif

void fix_cicon( unsigned int *col_data, long len, int old_planes, int new_planes, MFDB *s )
{
  (*nac->set_temps)( new_planes );
  (*nac->xfix_cicon)( col_data, len, old_planes, new_planes, s, 1 );
}

static char *pathend( char *ptr )
{
  char *s;
  
  if( (s=strrchr(ptr,'\\')) == 0 ) return(ptr);
  return(s+1);
}

int x_scrp_get( char *out, int delete )
{
  long map;
  int ret;
  char ok=0, *e;
  DTA dta, *old;

  old = Fgetdta();
  Fsetdta(&dta);
  scrp_read(out);
  map = (*nac->drvmap)();
  if( *out && *(out+1)==':' && (map & (*out&=0x5f)-'A')!=0 )
    if( (e=pathend(out))<=out+3 && !*e )
    {
      ok=1;
      if( e<out+3 ) strcat(out,"\\");
    }
    else
    {
      if( !*e ) *(e-1) = 0;
      if( !Fsfirst(out,FA_SUBDIR) ) ok=1;
      strcat(out,"\\");
    }
  if( !ok )
  {
    out[0] = (map&(1<<2)) ? 'C' : 'A';
    strcpy( out+1, ":\\CLIPBRD" );
    if( Fsfirst(out,0x37) && Dcreate( out ) )
    {
      Fsetdta(old);
      return 0;
    }
    strcat( out, "\\" );
  }
  scrp_write(out);
  if( delete )
  {
    strcat( out, "SCRAP.*" );
    ret = Fsfirst( out, 0x23 );
    while( !ret )
    {
      strcpy( pathend(out), dta.d_fname );
      Fdelete( out );
      ret = Fsnext();
    }
  }
  *pathend(out) = 0;
  Fsetdta(old);
  return 1;
}

/* NeoDesk 3.02 by Dan Wilga
   Copyright ½ 1990, Gribnif Software.
   All Rights Reserved.
*/
#include "aes.h"
#include "string.h"
#include "neodesk.h"
#include "tos.h"
#include "neocommn.h"
#include "stdlib.h"
#include "inf_load.h"
#include "xwind.h"

#undef DEBUG

#define index strchr
#define rindex strrchr

NEO_ACC *neo_acc;
GUI *gui;
INF_CONV *infc;
MOST *z;
char path[120], fname[13], buf[200], tmpf[130], exin, multitask, info, discard,
    **argv, bad;
int hand, ver, curxmax, curymax, curcelht, xmax, ymax, AES_handle, stage,
    neo_apid, retries, old_stage, argc, w_handle, blit_ok, delta;
/* stage 0:info  1:select  2:convert  3:quit */
Rect frect;

void test_stage( int *buf );

void cdecl spf( char *buf, char *fmt, ... ) {
  (*neo_acc->mas->dopf)( buf, fmt, (unsigned int *)&... );
}

/*%void get_ack(void)
{
  int buffer[8];
  
  do evnt_mesag(buffer);
  while( buffer[0] != DUM_MSG );
} */

void ack(void)                          /* acknowledge NeoDesk's message */
{
  int buf[8];
  
  buf[0] = DUM_MSG;                     /* always ack with this message */
  buf[1] = AES_handle;
  appl_write( neo_apid, 16, buf );
}

void sel_if( OBJECT *form, int num, int truth )
{
  unsigned int *i = &form[num].ob_state;
  
  if( truth ) *i |= SELECTED;
  else *i &= ~SELECTED;
}

int is_sel( OBJECT *o, int num )
{
  return o[num].ob_state & SELECTED;
}

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

int i_main( OBJECT *o, F_TYPE *f );
int t_main( OBJECT *o, int num, F_TYPE *f );
int x_main( OBJECT *o, int num, F_TYPE *f );
int i_rez( OBJECT *o, F_TYPE *f );
int x_rez( OBJECT *o, int num, F_TYPE *f );
int i_info( OBJECT *o, F_TYPE *f );
int x_info( OBJECT *o, int num, F_TYPE *f );

F_TYPE forms[] = {
   { 0,  MAIN,   { 0, 1, 0, 0, 0, 0, 0 }, 0L,
         i_main, t_main, x_main },	     /* Main */
   { 0,  REZ,    { 0, 1, 0, 0, 0, 1, 0 }, 0L,
         i_rez,  0L,     x_rez },	     /* Rez */
   { 0,  CINFO,  { 0, 1, 0, 0, 0, 0, 0 }, 0L,
         i_info, x_info, x_info } };         /* Info */

Rect center, form_rect;
F_TYPE *curform;

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
  if( g->y < curcelht )
  {
    tree[0].ob_y += curcelht-g->y;
    g->y = curcelht;
  }
}

void close_winds( int ac_close )
{
  if( neo_acc ) infc->is_ok = 0L;
  if( hand>0 )
  {
    Fclose(hand);
    hand = 0;
  }
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

int _use_form( F_TYPE *f, int num )
{
  int but, ret=0, i;
  Rect r;

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

int f_alert1( int num )
{
  char **p;
  
  rsrc_gaddr( 15, num, &p );
  return( form_alert( 1, *p ) );
}

int start_form( int type, F_TYPE *f )
{
  int ret = 0, dum, hand;
  Rect outer;
  
  if( f->flags.modal ) hand = 0;
  else if( (w_handle = hand =
      wind_create( type, Xrect(neo_acc->mas->most->maximum) )) < 0 )
  {
    f_alert1( NOWINDS );
    return 0;
  }
  if( hand>0 )
  {
    form_y( curform=f, -1 );
    calc_bord( type, f->tree, &outer ); /* fit a window around it */
    wind_calc( WC_WORK, type, Xrect(outer),
        &center.x, &center.y, &center.w, &center.h );
    *(long *)&f->tree[0].ob_x = *(long *)&center.x;
  }
  form_pos(f);
  if( (*f->init)( f->tree, f ) )	 /* initialize the dialog */
    if( hand>0 )
    {
      wind_set( hand, WF_NAME, old_title );
      wind_open( hand, Xrect(outer) );
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
  
  f = forms+num;
  if( !f->tree )
  {
    rsrc_gaddr( 0, f->treenum, &f->tree );
    f->treenum = -1;
    (*gui->xtern.x_form_center)( f->tree, &frect.x, &frect.y, &frect.w, &frect.h );
  }
/*%  if( num==2 ) f->old_title = "";*/
  if( (i=start_form( NAME|MOVER|CLOSER, f )) == 0 ) stage = 3;
  return i;
}

char *pathend( char *ptr )
{
  char *s;
  
  if( (s=rindex(ptr,'\\')) == 0 ) return(ptr);
  return(s+1);
}

int fopen( char *name )
{
  int b;
  
  if( (b=Fopen(name,0)) < 0 )
  {
    f_alert1( A1 );
    return(0);
  }
  return(b);
}

int select( int flag, char *name )
{
  int b, r;
  char inf_name[120], **p;

  if( flag )
    if( (b=fopen(name)) > 0 )
    {
      strcpy( path, name );
      strcpy( pathend(path), "*.INF" );
      strcpy( fname, pathend(name) );
      return(b);
    }
  rsrc_gaddr( 15, A7, &p );
  for(;;)
  {
    wind_lock(1);
    r = exin ? fsel_exinput( path, fname, &b, *p ) :
        fsel_input( path, fname, &b );
    wind_lock(0);
    if( !r || !b ) return 0;
    strcpy( inf_name, path );
    strcpy( pathend(inf_name), fname );
    if( (b=fopen(inf_name)) > 0 ) return(b);
  }
}

void read_err(void)
{
  f_alert1( A2 );
}

int get_line(void)
{
  return( (*neo_acc->mas->get_line)(hand,buf,1,0) );
}

/*%
char *find_extn( char *p )
{
  char *p2;
  
  if( (p2 = strrchr(p,'\\')) == 0 ) p2=p;
  if( (p = strchr(p2,'.')) == 0 ) return p2+strlen(p2);
  return p;
}

int iprog_type( char *path )
{
  int i;
  PROG_TYPE pt;
  EXTENSION *e;

  pt.i = 0;
  path = find_extn(path);
  for( e=z->extension, i=0; ++i<=z->num_ext; e++ )
    if( e->type.c && !strcmp( path, e->extns ) )
    {
      pt.p.batch = e->type.s.bat;
      pt.p.npg = e->type.s.npg;
      pt.p.tos = e->type.s.tos;
      pt.p.takes_params = e->type.s.parm;
      pt.p.set_me = pt.p.show_status = pt.p.clear_screen = 1;
      break;
    }
  if( pt.i == 0 ) pt.p.pexec_mode = TEXT;
  else if( pt.p.batch ) pt.p.pexec_mode = BATCH;
  else pt.p.pexec_mode = PROG;
  return pt.i;
} */

int prog_type( int i )
{
  PROG_TYPE pt;
  
  pt.i = 0;
  if( !i ) return 0;
  pt.p.set_me = 1;
  pt.p.clear_screen = 1;
  pt.p.show_status = 1;
  switch( i )
  {
    case 1:
      break;
    case 3:
      pt.p.takes_params = 1;
    case 2:
      pt.p.tos = 1;
      break;
    case 4:
      pt.p.batch = 1;
      pt.p.takes_params = 1;
      break;
  }
  return( pt.i );
}

void convert( int *x, int *y, int *w, int *h /*, int dial */ )
{
/*  if( dial )
  {
    *x = (long)*x * curxmax / xmax;
    *y = (long)*y * curymax / ymax * curcelht / celht;
  }
  else
  {  */
    *x = (long)*x * curxmax / xmax;
    *y = (long)*y * curymax / ymax;
    *w = (long)*w * curxmax / xmax;
    *h = (long)*h * curymax / ymax;
/*  }*/
}

void check( int *num, int max )
{
  if( *num > max ) *num=max;
}

#define TYPES1	37
#define TYPES2	5
char oks1[TYPES1], oks2[TYPES2];

int is_ok( int index )
{
  if( index>=100 )
  {
    if( (index-=100)>=TYPES2 ) return 0;
    return oks2[index];
  }
  if( index>=TYPES1 ) return 0;
  return oks1[index];
}

int get_ack(void)
{
  int buffer[8], last[10], sum, i, lnum;
  static EMULTI emulti = { MU_MESAG|MU_TIMER, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 3000, 0 };
  
  buffer[0] = lnum = 0;
  memset( last, 0, sizeof(last) );
  for(;;)
  {
/*%    if( gui ) (*gui->xtern.multi_evnt)( &emulti, buffer );
    else*/ multi_evnt( &emulti, buffer );
    if( emulti.event&MU_MESAG )
      if( buffer[0] == NEO_INF2_RES )
      {
#ifdef DEBUG
        Cconws( "got " );
        Crawio( buffer[4]+'0' );
        Crawio( ' ' );
#endif
        return buffer[4];
      }
      else
      {
        for( sum=0, i=0; i<8; i++ )
          sum += buffer[i]*(i+1);
        for( i=0; i<lnum; i++ )
          if( sum == last[i] )
          {
#ifdef DEBUG
            Cconws( "fail1 " ); 
#endif
            return 1;
          }
        if( lnum<10 ) last[lnum++] = sum;
        appl_write( AES_handle, 16, buffer );	/* send back to me */
      }
    if( emulti.event&MU_TIMER )
    {
#ifdef DEBUG
      Cconws( "fail2 " ); 
#endif
      return 1;
    }
  }
}

int wrap_load( int after )
{
/*#ifndef DEBUG*/
  int set[8];

  set[0] = NEO_ACC_INF2;
  set[1] = set[5] = AES_handle;
  set[2] = 0;
  set[3] = NEO_ACC_MAGIC;
  set[4] = after;
  appl_write( neo_apid, 16, set );
  return get_ack();
/*#else
  return after=0;
#endif */
}

struct
{
  int type;
  char path[120];
} programs[15];
struct
{
  int type, x, y, state;
  char c[2], label[13];
} icons[32];

void load_it( OBJECT *form )
{
  int *r, i, j, k, dum, progs, set[20], icon_cnt;
  char err, *ptr, c, temp[130];
  struct Wstruct *ws;
  PRN_PARAM prn_param;
/*%  unsigned long qmsg[4];*/
  static char xoks1[TYPES1] = { AUTOEXC, PRINTERQ, CPANEL, COLORS, SNAPGR,
      PRTDIR, PREF, MISCSET, FLOP, BATCHPRO, TXTDISP, WINDOWS, WINDOWS,
      WINDOWS, WINDOWS, WINDOWS, WINDOWS, WINDOWS, TEMPL, TEMPL, TEMPL,
      TEMPL, TEMPL, TEMPL, TEMPL, CPYDELF, SRCHFL, TEMPL, DFLTPARM, DFLTPARM,
      DFLTPARM, DFLTPARM, DFLTPARM, ENVSTR, DIALSET, DESKSET, WINDSET },
      xoks2[TYPES2] = { DTOPICON, INSTALAP, EXTENSS, ENVSTR, DIALOGS };
  
/*%  (*neo_acc->save_desktop)();*/
  for( i=0; i<TYPES1; i++ )
    oks1[i] = is_sel( form, xoks1[i] ) && (ver==3 || xoks1[i]<TEMPL ||
        ver && xoks1[i]<=EXTENSS);
  for( i=0; i<TYPES2; i++ )
    oks2[i] = is_sel( form, xoks2[i] );
  infc->is_ok = is_ok;
  if( ver<3 )
  {
  z = neo_acc->mas->most;
  err = 0;
  progs = 0;
  if( (err = get_line()) == 0 )	/* skip alt path */
    if( !ver )
    {
      if( make_form( 1 ) != OTHCONV ) return;
    }
    else if( (err = get_line()) == 0 )
        (*neo_acc->sscnf)( buf, "%d %d %d", &xmax, &ymax, &dum ); /*&celht );*/

  if( wrap_load(0) ) return;

  graf_mouse( HOURGLASS, 0L );
  if( !err )
    if( (err = get_line()) == 0 )
      if( is_ok(INF_AUTOEXC) ) strcpy( z->autoexec, buf );
      
  if( !err )
    if( (err = get_line()) == 0 && is_ok(INF_CONTROL) )
    {
      r = z->cntrl_set;
      (*neo_acc->sscnf)( buf, ver ? "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d" :
          "%d %d %d %d %d %d %d %d %d %d %d %d", &prn_param.prn_set, &prn_param.ffd,
          r, r+1, r+2, r+3, r+4, r+5, r+6, r+7, r+8, r+9, r+10, r+11, r+12, r+13 );
          /* sscnf stops if file is a 3.00-3.02 */
      if( ver==1 ) *(r+4) |= 0xE;	/* turn other caches on */
    }

  if( !err )
    if( (err = get_line()) == 0 && is_ok(INF_PALETTE) )
      for( i=0, ptr=buf; i<(ver?16:4); i++ )
      {
        (*neo_acc->sscnf)( ptr, "%x", &j );
        z->pallette[i] = 0L;
        for( k=4; --k; )
          *((unsigned char *)&z->pallette[i]+k) = (((j&7)<<1) | (j&8>>3)) * 255 / 15;
        ptr += 4;
      }

  if( !err )
    if( ver && (err = get_line()) == 0 && is_ok(INF_SRCHTMP) )
      for( i=0, ptr=buf; i<6; i++ )
      {
        (*neo_acc->sscnf)( ptr, "%s",  z->template[i] );
        if( z->template[i][0] == '^' ) z->template[i][0] = '\0';
        ptr = index(ptr,' ') + 1;
      }

  if( !err )
  {
    if( (err = get_line()) == 0 )
    {
      if( ver )
      {
        if( ver==1 ) (*neo_acc->sscnf)( buf, 
            "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s",
            set, set+1, set+2, set+3, set+4, set+5, set+6, set+7, set+8,
            set+9, set+10, set+11, set+12, set+13, set+14, set+15, set+16,
            set+17, set+18, temp );
        else (*neo_acc->sscnf)( buf, 
            "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s",
            set, set+1, set+2, set+3, set+4, set+5, set+6, set+7, set+8,
            set+9, set+10, set+11, set+12, set+13, set+14, set+15, set+16,
            set+17, set+18, set+19, temp );
        if( is_ok(INF_PRNTDIR) ) z->dir_prn.c = set[2];
        if( is_ok(INF_FORMAT) )
        {
          z->tracks = set[11];
          z->sides = set[12];
          z->spt = set[13];
          z->twst = set[14];
        }
      }
      else
      {
        (*neo_acc->sscnf)( buf, 
            "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s", 
            set+11, set+12, set+13, set+14, set+15, set+16, set+17, set, 
            set+1, set+3, set+4, set+5, set+7, set+8, set+9, set+10, temp );
        if( is_ok(INF_WINDOW1) )
        {
          z->mshowicon = set[11];
          z->mstlgsml = set[12];
          z->mstcolumn = set[13];
          z->msizdattim[0] = set[14];
          z->msizdattim[1] = set[15];
          z->msizdattim[2] = set[16];
          z->msort_type = set[17];
        }
      }
      if( is_ok(INF_SNAP) )
      {
        z->snapx = set[0] + (ver ? 0 : 2) - 79;
        z->snapy = set[1] + (ver ? 0 : 4) - 41;
      }
      if( is_ok(INF_COPYMOV) )
      {
        z->move_mode = set[3];
        z->conf_copy = set[4];
        z->conf_del = set[5];
        if( ver ) z->conf_over = set[6];
        z->status_on = set[7];
        z->tos_pause = set[8];
        /* ignore files_fold */
        z->use_master = set[10] != 0;
        if( ver )
        {
          z->ctrlZ = set[15];
          z->status_report = set[16];
          z->quit_alert = set[17];
          z->saveconf = set[18];
        }
        if( ver>1 && is_ok(INF_MISCPRF) ) z->other_pref.i = set[19];
      }
      if( is_ok(INF_BATCH) ) strcpy( z->batch_name, temp );
    }
  }
  
  if( !err && ver )
    if( (err = get_line()) == 0 && is_ok(INF_TXTREAD) )
        strcpy( z->text_reader, buf );
  
  for( i=0; i<7 && !err; i++ )
    if( (err = get_line()) == 0 && is_ok(INF_WINDOW1) )
    {
      r = &((ws = &z->w[i])->x);
      if( ver )
      {
        (*neo_acc->sscnf)( buf, "%d %d %d %d %d %d %c %D %d %d %d %d %d %d %d %d %s",
            &ws->place, r, r+1, r+2, r+3, &ws->split, set,
            &ws->f_off.l, &z->showicon[i], &z->stlgsml[i], &z->stcolumn[i],
            z->sizdattim[i], z->sizdattim[i]+1, z->sizdattim[i]+2, z->sort_type+i,
            &z->filter[i].mask, ws->path );
        if( z->filter[i].mask )
        {
          z->filter[i].flags.s.templates = 1;
          z->filter[i].flags.s.use_long = 0;
        }
      }
      else
      {
        (*neo_acc->sscnf)( buf, "%d %d %d %d %d %c %D %s", &ws->place, 
            r, r+1, r+2, r+3, set, &ws->f_off.l, ws->path );
        z->showicon[i] = z->mshowicon;
        z->stlgsml[i] = z->mstlgsml;
        z->stcolumn[i] = z->mstcolumn;
        z->sizdattim[i][0] = z->msizdattim[0];
        z->sizdattim[i][1] = z->msizdattim[1];
        z->sizdattim[i][2] = z->msizdattim[2];
        z->sort_type[i] = z->msort_type;
        ws->split = 0L;
      }
      if( !i )	/* 003 */
      {
        z->mshowicon = z->showicon[0];
        z->mstlgsml = z->stlgsml[0];
        z->mstcolumn = z->stcolumn[0];
        z->msizdattim[0] = z->sizdattim[0][0];
        z->msizdattim[1] = z->sizdattim[0][1];
        z->msizdattim[2] = z->sizdattim[0][2];
        z->msort_type = z->sort_type[i];
      }
      ws->place = 0;	/* 003: causes a crash otherwise */
      convert( r, r+1, r+2, r+3 );
      j = curcelht==8 ? 73+5 : 137+6;
      if( r[3] < j ) r[3] = j;
      check( &r[2], curxmax );
    }

    for( i=icon_cnt=0; i<32 && !err; i++ )
      if( (err = get_line()) == 0 )
        if( is_ok(INF_DESKICN) )
        {
          (*neo_acc->sscnf)( buf, "%d %d %d %c %s", &icons[i].type,
              &icons[i].x, &icons[i].y, &c, tmpf );
          if( icons[i].type >= 0 )
          {
            convert( &icons[i].x, &icons[i].y, &dum, &dum );
            check( &icons[i].x, curxmax-6*12 );
            check( &icons[i].y, curymax-curcelht-2-32-8 );
            icons[i].c[0] = c == '^' ? '\0' : c;
            icons[i].c[1] = 0;
            for( j=0; tmpf[j]; j++ )
              if( tmpf[j] == '^' ) tmpf[j] = ' ';
            if( !ver && icons[i].type >= CLIPBRD-2 ) icons[i].type++;
            if( icons[i].type >= D_PROG-2 ) progs++;
            strcpy( icons[i].label, tmpf );
            icon_cnt++;
          }
        }
        else if( atoi(buf) >= (ver ? D_PROG-2 : D_PROG-2-1) ) progs++;

  for( i=0; i<15 && !err; i++ )
    if( ver || i<10 )
    {
      if( (err = get_line()) == 0 && is_ok(INF_APPLIC) )
      {
        if( ver )
        {
          (*neo_acc->sscnf)( buf, "%d %d %s %s %s %s", set, &j,
              temp, tmpf, temp+40, temp+50 );
          if( ver==1 ) j |= 0x1f;	/* new flags for 3.03 */
        }
        else
        {
          (*neo_acc->sscnf)( buf, "%d %s %s %s %s", &j, 
              temp, tmpf, temp+40, temp+50 );
          j = 0x1f;	/* all new flags on */
          set[0] = prog_type(j);
        }
        if( set[0] )
        {
          if( temp[51] )
          {
            if( temp[41] ) strcat( temp+41, "," );
            strcat( temp+41, temp+51 );
          }
          spf( buf, "%d %h %S %S %S", set[0], j, temp+41,		/* 004: new applic format */
              temp, tmpf );
          err = (*infc->scan_inf_line)( buf, INF_APPLIC );
        }
      }
    }
  
  if( ver ) for( i=0; i<10 && !err; i++ )
    if( (err = get_line()) == 0 && is_ok(INF_EXTENSN) )
    {
      (*neo_acc->sscnf)( buf, "%d %s", &j, temp );
      spf( buf, "%x %S", j, temp );
      err = (*infc->scan_inf_line)( buf, INF_EXTENSN );
    }

  if( !err )
  {
    for( i=0, ptr=z->env; i<10 && !err; i++ )
      if( (err = get_line()) == 0 && is_ok(INF_ENVIRON) )
      {
        j = strlen(buf);
        strcpy( ptr, buf );
        *(ptr+=j+1) = '\0';
      }
    if( is_ok(INF_ENVIRON) ) while( ptr < z->env+620 ) *ptr++ = '\0';
  }
  
  for( i=0, temp[0]=0; i<2 && !err; i++ )
    if( (err = get_line()) == 0 && is_ok(INF_TTP1) ) 
        strcat( temp, buf );
  if( is_ok(INF_TTP1) )
  {
    for( i=0; i<5; i++ )
      z->ttp_params[i][0] = '\0';
    ptr = buf;
    i = 0;
    while( *ptr )
    {
      if( (j=strlen(ptr))>38 ) j = 38;
      strncpy( z->ttp_params[i], ptr, j );
      z->ttp_params[i++][j] = 0;
      ptr += j;
    }
  }
  if( !err )
    for( i=0; i<progs && !err; i++ )
      if( (err = get_line()) == 0 && is_ok(INF_DESKICN) )
      {
        (*neo_acc->sscnf)( buf, "%d %d %s", &j, &k, tmpf );
        programs[j].type = k + !ver;
        strcpy( programs[j].path, tmpf );
      }
  if( is_ok(INF_DESKICN) )
    for( i=0; i<icon_cnt && !err; i++ )
    {
      if( (k=icons[i].type) >= D_PROG-2 )
      {
        j = k - (D_PROG - 2);
        k = 99;
      }
      else j = -1;
/*%      if( j>=0 )
        if( programs[j].type==FOLDER ) set[0] = FOLDER<<8;
        else set[0] = iprog_type( programs[j].path );
      else set[0] = 0; */
      spf( buf, "%d %d %d %S %S %d %S", k, icons[i].x,
          icons[i].y, icons[i].c, icons[i].label, j>=0 ? programs[j].type<<8 : 0,
          j>=0 ? programs[j].path : "" );
      err = (*infc->scan_inf_line)( buf, INF_DESKICN );
    }

    wrap_load(1);
  }
  else
  {
/*%    if( multitask )
    { */
      set[0] = NEO_ACC_INF;
      set[1] = AES_handle;
      set[2] = 0;
      set[3] = NEO_ACC_MAGIC;
      strcpy( tmpf, path );
      strcpy( pathend(tmpf), fname );
      *(char **)&set[4] = tmpf;
      appl_write( neo_apid, 16, set );
/*%    }
    else
    {
      Fseek( 0L, hand, 0 );
      (*infc->inf_load)( 0, hand, 1 );
    } */
  }
  graf_mouse( ARROW, 0L );
  Fseek( 0L, hand, 0 );
  stage = 2;
  old_stage = 0;	/* rewind & re-read version */
}

#pragma warn -par
int i_main( OBJECT *o, F_TYPE *f )
{
  o[MINVIS1].ob_flags = ver ? 0 : HIDETREE;
  o[MINVIS2].ob_flags = ver==3 ? 0 : HIDETREE;
/*%  if( !ver )
    if( !make_form(1) ) return 0; */
  return 1;
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

int t_main( OBJECT *o, int num, F_TYPE *f )
{
  int i, j, max, dum;

  if( num==CSELALL )
  {
    for( j=0, max=DFLTPARM-CPANEL+1, i=CPANEL; i<=DFLTPARM; i++ )
      if( is_sel(o,i) ) j++;
    if( ver ) for( max+=EXTENSS-TEMPL+1, i=TEMPL; i<=EXTENSS; i++ )
      if( is_sel( o, i ) ) j++;
    if( ver==3 ) for( max+=DIALSET-DIALOGS+1, i=DIALOGS; i<=DIALSET; i++ )
      if( is_sel( o, i ) ) j++;
    j = j!=max;
    for( i=CPANEL; i<=DFLTPARM; i++ )
      sel_if( o, i, j );
    form_draw( f, CPANEL-1 );
    if( ver )
    {
      for( i=TEMPL; i<=EXTENSS; i++ )
        sel_if( o, i, j );
      form_draw( f, MINVIS1 );
    }
    if( ver==3 )
    {
      for( i=DIALOGS; i<=DIALSET; i++ )
        sel_if( o, i, j );
      form_draw( f, MINVIS2 );
    }
    evnt_button( 1, 1, 0, &dum, &dum, &dum, &dum );
  }
  return 0;
}
int x_main( OBJECT *o, int num, F_TYPE *f )
{
  int buf[8];
  
  if( num==CONVERT )
  {
#ifdef DEBUG
    Cconws( "\033HConv" );
#endif
    load_it( o );
/*%    if( f->handle<=0 && multitask )
    {
      wind_lock(0);
      evnt_timer( 250, 0 );
      buf[0] = NEO_AC_OPEN;
      buf[2] = 0;
      appl_write( buf[1]=AES_handle, 16, buf );
      wind_lock(1);
      return 1;
    } */
    return 0;
  }
  else if( num==SELINF ) stage = 1;
  else stage = 3;
/*%  if( f->handle<=0 ) test_stage(0L); */
  return 1;
}
int i_rez( OBJECT *o, F_TYPE *f )
{
  int dum;
  
  sel_if( o, REZMED, dum = curxmax==640 && curymax==200 && curcelht==8 );
  sel_if( o, REZHIGH, dum |= curxmax==640 && curymax==400 && curcelht==16 );
  sel_if( o, REZOTH, !dum );
  spf( o[OTHWID].ob_spec.tedinfo->te_ptext, "%d", curxmax );
  spf( o[OTHHT].ob_spec.tedinfo->te_ptext, "%d", curymax );
  return 1;
}
int x_rez( OBJECT *o, int num, F_TYPE *f )
{
  if( is_sel( o, REZMED ) )
  {
    xmax = 640;
    ymax = 200;
/*          celht = 8;*/
  }
  else if( is_sel( o, REZHIGH ) )
  {
    xmax = 640;
    ymax = 400;
/*         celht = 16; */
  }
  else
  {
    xmax = atoi(o[OTHWID].ob_spec.tedinfo->te_ptext);
    ymax = atoi(o[OTHHT].ob_spec.tedinfo->te_ptext);
/*          celht = form[OTH8].ob_state ? 8 : 16; */
  }
  return 1;
}
int i_info( OBJECT *o, F_TYPE *f )
{
  return 1;
}
int x_info( OBJECT *o, int num, F_TYPE *f )
{
  stage = 1;
  return 1;
}
#pragma warn +par

int test_neo(void)
{
  if( !neo_acc || (long)neo_acc & 1 || neo_acc->nac_ver < 0x0400 ||
      neo_acc->nac_ver >= 0x0500 )
  {
    f_alert1( A4 );
    neo_acc = 0L;
    return 0;
  }
  infc = neo_acc->mas->most->inf_conv;
  if( infc->ver != INFCONV_VER )
  {
    f_alert1( BADNEO );
    neo_acc = 0L;
    return 0;
  }
  if( neo_apid<0 ) neo_apid = appl_find("NEODESK ");
  gui = neo_acc->mas->most->gui;
/*%  if( (*gui->xtern.gui_init)( 0L ) < 0 ) return 0; */
  curxmax = neo_acc->mas->most->graphics->v_x_max;
  curymax = neo_acc->mas->most->graphics->v_y_max;
  return 1;
}

int get_ver(void)
{	/* return: 1 for continue, -1 for retry */
  int i;
  static char vers[][5] = { "2.02", "3.00", "3.03", "4.00" };

  if( !(*neo_acc->mas->get_line)( hand, buf, 1, 1 ) )
  {
    for( i=0; i<4; i++ )
      if( !strcmp( buf, vers[i] ) )
      {
        ver = i;
        return 1;
      }
    f_alert1( A5 );
    Fclose(hand);
    hand = 0;
  }
  else read_err();
  return -1;
}

void find_neo( int *buf )
{
  /* throw away if too many tries */
  if( ++retries>10 )
  {
    f_alert1( A3 );
    return;
  }
  /* try to find NeoDesk */
#ifdef DEBUG
  Cconws( "find" );
  Crawio( retries+'0' );
#endif
  if( (neo_apid = appl_find("NEODESK ")) >= 0 )
  {
#ifdef DEBUG
    Cconws( "F " );
#endif
    /* put the message back in my queue */
    appl_write( AES_handle, 16, buf );
    buf[0] = NEO_ACC_ASK;         /* 89 */
    buf[1] = AES_handle;
    buf[3] = NEO_ACC_MAGIC;
    buf[4] = AES_handle;
    appl_write( neo_apid, 16, buf );
  }
  else
  {
#ifdef DEBUG
    Cconws( "NF " );
#endif
    f_alert1( A3 );
  }
}

void do_quit(void)
{
/*%  if( neo_acc ) (*gui->close_all_fwind)( AES_handle, 0 ); */
  rsrc_free();
/*%  if( gui ) (*gui->xtern.gui_exit)( AES_handle, 1 );
  else*/ appl_exit();
  if( neo_acc ) infc->is_ok = 0L;
  exit(0);
}

int do_open( int *buf )
{
  if( bad )
    if( _app ) do_quit();
    else return 0;
  if( buf && discard ) discard = 0;
  else if( !neo_acc )
    if( buf ) find_neo(buf);
    else return 0;
  else if( !stage )
    if( argc<3 )
      if( !info )
      {
        make_form(2);
        info = 1;
      }
      else stage = 1;
    else
    {
      argc = 0;
      if( (hand = select( 1, argv[2] )) != 0 ) stage = 2;
      else stage = 1;
    }
  return 1;
}

void test_stage( int *buf )
{
  int i;
  char **p;
  
  if( neo_acc && old_stage != stage )
  {
again:
    if( stage<3 ) do
    {
      if( stage==1 )
        if( (hand = select( 0, 0L )) != 0 ) stage = 2;
        else stage = 3;
      if( stage==2 )
        if( (i = get_ver())==0 ) stage = 3;
        else if( i<0 ) stage = 1;
    }
    while( stage==1 );
    if( stage==2 && w_handle<=0 ) make_form(0);
    if( stage==3 && _app )
    {
      rsrc_gaddr( 15, A6, &p );
      if( form_alert( 2, *p ) == 1 )
      {
        stage = 0;
        if( do_open(buf) ) goto again;
      }
    }
  }
  if( stage==3 )
    if( _app ) do_quit();
    else stage = 0;
  old_stage = stage;
}

void _wind_set( int h, int n, int i1, int i2, int i3, int i4 )
{
/*%  if( gui ) (*gui->xtern.wind_set)( h, n, i1, i2, i3, i4 );
  else*/ wind_set( h, n, i1, i2, i3, i4 );
}

EMULTI emulti = { 0, 1, 1, 1,  0, 0, 0, 0, 0,  1, 0, 0, 0, 0  };

int main( int largc, char *largv[] )
{
  int buf[8], dum, i, edit_obj, next_obj;
  char aes_40, **p, *ptr2, inside=0, cont;
  
  argc = largc;
  argv = largv;
  AES_handle = appl_init();
  exin = _GemParBlk.global[0] >= 0x140;
  aes_40 = _GemParBlk.global[0] >= 0x400;
  multitask = _GemParBlk.global[1] == -1;
  if( aes_40 ) shel_write( 9, 1, 0, 0L, 0L );
  if( !rsrc_load( "INF_LOAD.RSC" ) )
  {
    form_alert( 1, "[1][Could not open|INF_LOAD.RSC!][Ok]" );
    bad = 1;
    if( aes_40 || _app )
    {
      appl_exit();
      return(0);
    }
    goto loop;
  }
#ifndef DEBUG
  if( !multitask && _app )
  {
    f_alert1( NOTMULTI );
    do_quit();
  }
#endif
  if( aes_40 || !_app )
  {
    rsrc_gaddr( 15, A8, &p );
    menu_register( AES_handle, *p );
  }
  if( _app )
  {
    if( argc>=2 )
    {
      neo_acc = (NEO_ACC *)atol(argv[1]);
      neo_apid = -1;
    }
    if( !test_neo() ) do_quit();
  }
  graf_mouse( ARROW, 0L );
  graf_handle( &dum, &curcelht, &dum, &dum );
  path[0] = Dgetdrv()+'A';
  path[1] = ':';
  Dgetpath( path+2, 0 );
  strcat( path, "\\*.INF" );
loop:
  if( _app ) goto open;
  for(;;)
  {
/*%    if( gui )
    {
      emulti.type = X_MU_DIALOG|MU_MESAG;
      (*gui->xtern.multi_evnt)( &emulti, buf );
    }
    else
    { */
      if( w_handle > 0 ) emulti.type = MU_MESAG|MU_KEYBD|MU_M1|MU_M2|MU_BUTTON;
      else emulti.type = MU_MESAG|MU_KEYBD|MU_BUTTON;
      *(Rect *)&emulti.m1x = center;
      *(Rect *)&emulti.m2x = center;
      multi_evnt( &emulti, buf );
/*%    } */
    if( emulti.event & MU_MESAG )
      switch( buf[0] )
      {
        case NEO_ACC_INI:                       /* NeoDesk knows we are here */
          if( buf[3]==NEO_ACC_MAGIC )
          {
#ifdef DEBUG
	    Cconws( "ACC_INI " );
#endif
            retries = 0;
            neo_acc = *(NEO_ACC **)&buf[4];  /* set pointer to Neo's fncns */
            neo_apid = buf[6];               /* NeoDesk's AES ID */
            if( !test_neo() ) discard = 1;
          }
          break;  
        case NEO_CLI_RUN:                       /* run a batch file */
          if( buf[3]==NEO_ACC_MAGIC )
            if( !neo_acc ) find_neo(buf);
            else ack();
          break;
        case NEO_ACC_PAS:
          if( buf[3]==NEO_ACC_MAGIC )
          {
#ifdef DEBUG
	    Cconws( "ACC_PAS " );
#endif
            if( !neo_acc ) find_neo(buf);
            else
            {
              while( (*neo_acc->list_files)(&ptr2) )
                if( *(ptr2+strlen(ptr2)-1) != ':' && *(ptr2+strlen(ptr2)-1) !=
                    '\\' )
                {
#ifdef DEBUG
                  Cconws(ptr2);
 	          Cconws(" ");
#endif
                }
              ack();
            }
          }
          break;
        case NEO_ACC_BAD:
          if( buf[3] == NEO_ACC_MAGIC )
          {
#ifdef DEBUG
	    Cconws( "ACC_BAD " );
#endif
            if( neo_acc )
            {
/*%              (*gui->close_all_fwind)( AES_handle, 0 );
              (*gui->xtern.gui_exit)( AES_handle, 0 ); */
              close_winds(0);
            }
            neo_acc = 0L;
            retries = 0;
            stage = 0;
          }
          break;
        case NEO_AC_OPEN:
#ifdef DEBUG
	  Cconws( "AC_OPEN " );
#endif
        case AC_OPEN:
open:     if( neo_acc && stage==2 )
	    if( w_handle<=0 ) make_form(0);  /* reopen modal main dial */
	    else
	    {
	      buf[3] = w_handle;
	      goto topped;
	    }
          else do_open(buf);
          break;
        case AC_CLOSE:
	  if( !bad && neo_acc ) /*% (*gui->close_all_fwind)( AES_handle, 1 ); */
              close_winds(1);
	  stage = 0;
	  neo_acc = 0L;
	  break;
	case AP_TERM:
	  do_quit();
	case WM_REDRAW:
	  form_draw( curform, 0 );
	  break;
	case WM_TOPPED:
topped:   _wind_set( buf[3], WF_TOP, buf[3], 0, 0, 0 );
          break;
        case WM_MOVED:
          _wind_set( buf[3], WF_CURRXYWH, buf[4], buf[5], buf[6], buf[7] );
          wind_get( buf[3], WF_WORKXYWH, &center.x, &center.y, &dum, &dum );
          curform->tree[0].ob_x = center.x;
          curform->tree[0].ob_y = center.y;
          break;
        case WM_CLOSED:
	  use_form( buf[3], -1 );
	  stage = _app ? 3 : 0;
	  break;
      }
/*%    if( emulti.event & X_MU_DIALOG ) use_form( buf[3], buf[2] ); */
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
    test_stage(0L);
  }
}

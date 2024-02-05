#include "tos.h"
#include "new_aes.h"
#include "neocommn.h"
#include "stdlib.h"
#include "string.h"
#include "settings.h"
#include "guidefs.h"

int i_wset( OBJECT *o, FORM *f );
int t_wset( OBJECT *o, int num, FORM *f );
int x_wset( OBJECT *o, int num, FORM *f );
int i_dset( OBJECT *o, FORM *f );
int t_dset( OBJECT *o, int num, FORM *f );
int x_dset( OBJECT *o, int num, FORM *f );

static FORM_TYPE forms[] = {
    { -1, NO_POS-10,   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      13*sizeof(KEYCODE)+(WGSIZE+1)*5*2, i_wset, t_wset, x_wset, 0L, 0L },
    { -1, NO_POS
    -11, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      4*sizeof(OB_PREFER), i_dset, t_dset, x_dset, 0L, 0L }
    };

static int start_dial( int num );

static SET_STRUCT *set_struct;
static GUI *gui;
static MOST *z;
static RSHDR *rshdr;
static NSETTINGS *set;
static int dum, dattnum=1, forms_open;
static NEO_ACC *neo_acc;

#ifdef TEST
int main( int argc, char *argv[] )
#else
int set_start( SET_STRUCT *s )	/* must be first */
#endif
{
  char rsc[120];
#ifdef TEST
  NEO_ACC *nac;
  int dial;
  SET_STRUCT s = { start_dial };
  
  if( argc<3 ) return 1;
  nac = (NEO_ACC *)atol( argv[1] );
  dial = atoi( argv[2] );
  set_struct = &s;
#endif
  gui = (z=(neo_acc=s->nac)->mas->most)->gui;
  set = &z->gui_settings;
#ifndef TEST
  strcpy( rsc, z->dflt_path );
  strcat( rsc, "SETTINGS.RSC" );
#else
  strcpy( rsc, "H:\\SOURCE\\NEODESK\\SETTINGS.RSC" );
#endif
  switch( (*gui->Nrsc_load)( rsc, Fgetdta(), &rshdr ) )
  {
    case 0:
#ifndef TEST
      form_alert( 1, "[1][|SETTINGS.RSC not found!][Cancel]" );
#else
      Cconws( "SETTINGS.RSC not found!" );
#endif
      return 0;
    case -1:
    case -2:
      return 0;
  }
  (*gui->Nrsc_rcfix)( rshdr );
#ifdef TEST
  forms[0].flags.modal = 1;
  forms[1].flags.modal = 1;
  start_dial( dial );
  return 0;
#else
  set_struct = s;
  s->start_dial = start_dial;
  start_dial( s->first_form );
  return 1;
#endif
}

static void rsrc_adr( int type, int obj, void **tree )
{
  (*gui->Nrsc_gaddr)( type, obj, tree, rshdr );
}

static void set_if( OBJECT *form, int num, int true )
{
  if( true ) form[num].ob_state |= SELECTED;
  else form[num].ob_state &= ~SELECTED;
}

static int is_sel( OBJECT *o, int i )
{
  return o[i].ob_state&SELECTED;
}

struct
{
  char obj, gobj, key;
} inds[] = { XWCLOSE,  WGCLOSE,  XS_CLOSE,
             XWMOVE,   WGMOVE,   99,
             XWBACK,   WGBACK,   XS_CYCLE,
             XWFULL,   WGFULL,   XS_FULL,
             XWILEFT,  WGILEFT,  XS_LFINFO,
             XWINFO,   WGINFO,   99,
             XWIRT,    WGIRT,    XS_RTINFO,
             XWUP,     WGUP,     XS_UPLINE,
             XWVBIGSL, WGVBIGSL, 98,
             XWVSMLSL, WGVSMLSL, 99,
             XWDOWN,   WGDOWN,   XS_DNLINE,
             XWLEFT,   WGLEFT,   XS_LFLINE,
             XWHBIGSL, WGHBIGSL, 97,
             XWHSMLSL, WGHSMLSL, 99,
             XWRT,     WGRT,     XS_RTLINE,
             XWSIZE,   WGSIZE,   99 };

static int ind=0, top=1, key=XS_CLOSE;

#define KEYS   ((KEYCODE *)f->mem_ptr)
#define STATES ((int *)&KEYS[13])
#define COLORS ((int (*)[WGSIZE+1])&STATES[WGSIZE+1])
#define COLOR2 ((int (*)[WGSIZE+1])&COLORS[2][0])

void gadcolor( OBJECT *o, FORM *f, int i )
{
  int col;
  
  o += inds[i].obj;
  col = COLORS[top][inds[i].gobj];
  if( (char)o->ob_type==G_BOXTEXT ) o->ob_spec.tedinfo->te_color = col;
  else *((int *)&o->ob_spec.index+1) = col;
  o->ob_state = (o->ob_state&OUTLINED) | STATES[inds[i].gobj];
}

void all_colors( OBJECT *o, FORM *f )
{
  int i;
  
  for( i=0; i<sizeof(inds)/4; i++ )
    gadcolor( o, f, i );
}

void add_ob( OBJECT *o, int ob )
{
  if( ob!=XWVSMLSL && ob!=XWHSMLSL ) objc_add( o, XWOUTER, ob );
}

void order( OBJECT *o )
{
  int i, ob;
  
  o[XWOUTER].ob_head = o[XWOUTER].ob_tail = -1;
  objc_add( o, XWOUTER, XWSPEC );
  for( i=0; i<sizeof(inds)/4; i++ )
  {
    ob = inds[i].obj;
    if( i!=ind )
    {
      add_ob( o, ob );
      o[ob].ob_state &= ~OUTLINED;
    }
    else o[ob].ob_state |= OUTLINED;
  }
  add_ob( o, inds[ind].obj );
}

void opaq_3d( OBJECT *o, FORM *f )
{
  if( is_sel( o, KOCPX ) /*|| inds[ind].gobj<0 003 */ )
      o[XWI1].ob_flags = o[XWI2].ob_flags |= HIDETREE;
  else o[XWI1].ob_flags = o[XWI2].ob_flags &= ~HIDETREE;
  set_if( o, XWOPAQ, COLORS[top][inds[ind].gobj]&(1<<7) );
  set_if( o, XW3D, STATES[inds[ind].gobj]&X_DRAW3D );
  set_if( o, XWSHADO, STATES[inds[ind].gobj]&X_SHADOWTEXT );
}

static void form_draw( FORM *f, int num )
{
  if( f->handle>0 ) x_wdial_draw( f->handle, num, 8 );
  else objc_draw( f->tree, num, 8, 0, 0, 0, 0 );
}

static void spf(char *buf, char *fmt, ...) {
  (*neo_acc->mas->dopf)(buf, fmt, (unsigned int *)&...);
}

void k_set( OBJECT *o, FORM *f, int draw )
{
  int i;
  unsigned char *ptr, c, c2;
  char **p;
  KEYCODE *k;
  static unsigned char
      keycode[] = { 0, 1, 0xf, 0xe, 0x53, 0x52, 0x62, 0x61, 0x47, 0x48, 0x50,
          0x4d, 0x4b, 0x72, 0x60, 0x39 },
      keynam[][6] = { "???", "Esc", "Tab", "Bksp", "Del", "Ins", "Help", "Undo",
          "Home", "", "", "", "", "Enter", "ISO", "Space" }, fmt[]="F%d",
          kpfmt[]="kp %c";

  if( key==99 )
  {
    o[KOBOX].ob_flags |= HIDETREE;
    if( draw ) form_draw( f, KOINVIS );
    return;
  }
  o[KOBOX].ob_flags &= ~HIDETREE;
  k = &KEYS[key];
  set_if( o, KOSHIFT, k->shift&3 );
  set_if( o, KOCNTRL, k->shift&4 );
  set_if( o, KOALT, k->shift&8 );
  *(ptr = o[KOKEY].ob_spec.free_string) = '\0'; /* blank by default */
  if( (c = k->ascii) != 0 )   /* ASCII value is used instead of scan code */
    if( c == ' ' ) strcpy( ptr, "Space" );
    else
    {
      *ptr++ = c;
      *ptr = 0;
    }
  else
  {
    c = k->scan;
    if( c==0x74 ) c=0x4d;                       /* ^right -> right */
    else if( c==0x73 ) c=0x4b;                  /* ^left  -> left */
    else if( c==0x77 ) c=0x47;                  /* ^home  -> home */
    else if( c>=0x78 && c<=0x83 ) c -= 0x76;    /* ^F1-10 -> F1-10 */
    for( i=0; i<sizeof(keycode); i++ )
      if( keycode[i] == c ) strcpy( ptr, keynam[i] );
    if( c >= 0x3b && c <= 0x44 ) spf( ptr, fmt, c-0x3a ); /* F1-10 */
    else if( c >= 0x54 && c <= 0x5d )
        spf( ptr, fmt, c-0x53 );                    /* shift F1-10 */
    if( !*ptr )
    {
      c2 = *(Keytbl( (void *)-1L, (void *)-1L, (void *)-1L )->unshift + c);
      if( c >= 0x63 && c <= 0x72 || c == 0x4a || c == 0x4e )
          spf( ptr, kpfmt, c2 );                    /* keypad key */
      else
      {
        *ptr++ = c2;                                      /* unshifted char */
        *ptr = '\0';
      }
    }
  }
/***  /* get a description for the key type */
  rsrc_gaddr( 15, is_main ? S0+xkey : FLK0+fknum, &p );
  o[KOBOX].ob_spec.free_string = *p;
  if( !is_main )
  {
    /* set either the flag's name or description in the dialog */
    o[KOMENU-1].ob_spec.free_string = flagmode ? app[tasknum-1].name+2 :
        apf.desc[0] ? apf.desc : apf.name;
    /* Activate is always disabled for the default flags */
    enab_if( o, KOMENU, flagmode || flagnum );
  } ******/
  if( draw ) form_draw( f, KOBOX );
}

void k_mode( OBJECT *o, FORM *f, int mode, int draw )
{
  static char *old;

  if( mode )
  {
    o[KOBOX1].ob_flags |= HIDETREE;
    old = o[KOBOX].ob_spec.free_string;
    o[KOBOX].ob_spec.free_string = o[KOPRESS].ob_spec.free_string;
  }
  else
  {
    o[KOBOX1].ob_flags &= ~HIDETREE;
    if( old ) o[KOBOX].ob_spec.free_string = old;
  }
  if( draw ) form_draw( f, KOBOX );
}

unsigned char k_shift( OBJECT *o )
{
  return ((o[KOALT].ob_state&SELECTED)<<3) |
         ((o[KOCNTRL].ob_state&SELECTED)<<2) |
         ((o[KOSHIFT].ob_state&SELECTED) ? 3 : 0);
}

static void bytecpy( void *dest, void *src, long size )
{
  (*neo_acc->bytecpy)( dest, src, size );
}

void set_bit( int *i, OBJECT *o, int ob, int bit )
{
  if( is_sel(o,ob) ) *i |= bit;
  else *i &= ~bit;
}

void gad_set( OBJECT *o, FORM *f, int i )
{
  gadcolor( o, f, i );
  form_draw( f, inds[i].obj );
}

void draw_gad( OBJECT *o, FORM *f, int others, int ind )
{
  int i;
  
  if( others ) for( i=0; i<sizeof(inds)/4; i++ )
    if( i!=ind && inds[i].gobj == inds[ind].gobj )
        gad_set( o, f, i );
  gad_set( o, f, ind );
}

void add_bits( FORM *f, int mask, int num )
{
  unsigned int *c;
  
  c = (unsigned int *)&COLORS[top][inds[ind].gobj];
  *c = (*c & ~mask) | (((*c & mask)+num) & mask);
}

int find_cc( int new )
{
  int i;

  for( i=0; i<sizeof(inds)/4; i++ )
    if( inds[i].obj==new ) return i;
  return -1;
}

static int cc_obj( OBJECT *o, int mx, int my )
{
  return find_cc( objc_find( o, 0, 8, mx, my ) );
}

static void copy_color( OBJECT *o, FORM *f, int num )
{
  int mx, my, mw, mh, new, bx, by, in;
  
  graf_mkstate( &mx, &my, &mw, &mh );
  if( (in = find_cc(num)) < 0 ) return;
  if( !(mw&1) || cc_obj( o, mx, my )<0 ) return;
  objc_offset( o, num, &mx, &my );
  mw = o[num].ob_width;
  mh = o[num].ob_height;
  objc_offset( o, XWOUTER, &bx, &by );
  wind_update( BEG_UPDATE );
  graf_mouse( POINT_HAND, 0L );
  if( graf_dragbox( mw, mh, mx, my, bx, by, o[XWOUTER].ob_width,
      o[XWOUTER].ob_height, &mx, &my ) )
  {
    graf_mkstate( &mx, &my, &mw, &mh );
    if( (new = cc_obj( o, mx, my )) >= 0 && new != in )
    {
      if( !is_sel( o, KOCPX ) )
          COLORS[top][inds[new].gobj] = COLORS[top][inds[in].gobj];
      STATES[inds[new].gobj] = STATES[inds[in].gobj];
      draw_gad( o, f, 1, new );
      form_draw( f, inds[ind].obj );	/* redraw current */
    }
  }
  graf_mouse( ARROW, 0L );
  wind_update( END_UPDATE );
}

#pragma warn -par
int i_wset( OBJECT *o, FORM *f )
{
  if( !o )
  {
    rsrc_adr( 0, KEYOPTS, (void **)&f->tree );
    x_form_center( f->tree, &dum, &dum, &dum, &dum );
    return 1;
  }
  set_if( o, KOCPX, set->flags.s.use_wcolors_cpx );
  set_if( o, KOTOP, top );
  bytecpy( KEYS, set->wind_keys, 13*sizeof(KEYCODE) );
  bytecpy( STATES, set->wstates[set->wcolor_mode], (WGSIZE+1)*2 );
  bytecpy( COLORS, set->dwcolors[set->wcolor_mode], (WGSIZE+1)*2*2 );
  if( is_sel( o, KOCPX ) ) bytecpy( COLOR2, set->dwcolors[set->wcolor_mode], (WGSIZE+1)*2*2 );
  all_colors( o, f );
  order( o );
  opaq_3d( o, f );
  o[KOPRESS].ob_flags |= HIDETREE;
  k_mode( o, f, 0, 0 );
  k_set( o, f, 0 );
  forms_open++;
  return 1;
}

int pop_bits( int type, OBJECT *o, FORM *f, int ob, int bit )
{
  unsigned int *c, val, ret, mask;
  
  mask = type ? 7 : 15;		/* 004 */
  c = (unsigned int *)&COLORS[top][inds[ind].gobj];
  val = (*c>>bit)&mask;
  if( !type ) ret = (*set_struct->popup_col)( o, ob, val );
  else ret = (*set_struct->popup_fill)( o, ob, val );
  if( ret==val ) return 0;
  *c = (*c & ~(mask<<bit)) | (ret<<bit);
  return 1;
}

int t_wset( OBJECT *o, int num, FORM *f )
{
  int i, nk, mx, my, x, gobj;

  gobj = inds[ind].gobj;
  switch( num )
  {
    case KOCPX:
      if( is_sel( o, KOCPX ) )
      {
        bytecpy( COLOR2, COLORS, (WGSIZE+1)*2*2 );
        i = set->flags.s.use_wcolors_cpx;
        set->flags.s.use_wcolors_cpx = 1;
        (*set_struct->get_wcolors)();
        set->flags.s.use_wcolors_cpx = i;
        bytecpy( COLORS, set->dwcolors[set->wcolor_mode], (WGSIZE+1)*2*2 );
      }
      else bytecpy( COLORS, COLOR2, (WGSIZE+1)*2*2 );
      all_colors( o, f );
      opaq_3d( o, f );
      form_draw( f, XWI0 );
      break;
    case XWBCOL-1:
      add_bits( f, 15<<12, 15<<12 );
      draw_gad( o, f, 1, ind );
      break;
    case XWBCOL+1:
      add_bits( f, 15<<12, 1<<12 );
      draw_gad( o, f, 1, ind );
      break;
#ifndef TEST
    case XWBCOL:
      if( pop_bits( 0, o, f, XWBCOL-1, 12 ) ) draw_gad( o, f, 1, ind );
      break;
#endif
    case XWFILL-1:
      add_bits( f, 7<<4, 7<<4 );
      draw_gad( o, f, 1, ind );
      break;
    case XWFILL+1:
      add_bits( f, 7<<4, 1<<4 );
      draw_gad( o, f, 1, ind );
      break;
#ifndef TEST
    case XWFILL:
      if( pop_bits( 1, o, f, XWFILL-1, 4 ) ) draw_gad( o, f, 1, ind );
      break;
#endif
    case XWFCOL-1:
      add_bits( f, 15, -1 );
      draw_gad( o, f, 1, ind );
      break;
    case XWFCOL+1:
      add_bits( f, 15, 1 );
      draw_gad( o, f, 1, ind );
      break;
#ifndef TEST
    case XWFCOL:
      if( pop_bits( 0, o, f, XWFCOL-1, 0 ) ) draw_gad( o, f, 1, ind );
      break;
#endif
    case XWTCOL-1:
      add_bits( f, 15<<8, 15<<8 );
      draw_gad( o, f, 1, ind );
      break;
    case XWTCOL+1:
      add_bits( f, 15<<8, 1<<8 );
      draw_gad( o, f, 1, ind );
      break;
#ifndef TEST
    case XWTCOL:
      if( pop_bits( 0, o, f, XWTCOL-1, 8 ) ) draw_gad( o, f, 1, ind );
      break;
#endif
    case KOTOP:
      top = is_sel( o, KOTOP );
      all_colors( o, f );
      opaq_3d( o, f );
      form_draw( f, XWI0 );
      break;
    case XW3D:
      STATES[gobj] |= X_MAGIC;
      set_bit( &STATES[gobj], o, XW3D, X_DRAW3D );
      draw_gad( o, f, 0, ind );
      break;
    case XWOPAQ:
      set_bit( &COLORS[top][gobj], o, XWOPAQ, (1<<7) );
      draw_gad( o, f, 1, ind );
      break;
    case XWSHADO:
      STATES[gobj] |= X_MAGIC;
      set_bit( &STATES[gobj], o, XWSHADO, X_SHADOWTEXT );
      draw_gad( o, f, 0, ind );
      break;
    default:
      if( num>=XWCLOSE && num<=XWSIZE )
  {
    for( i=0; i<sizeof(inds)/4; i++ )
      if( inds[i].obj == num )
      {
        graf_mkstate( &mx, &my, &dum, &dum );
        if( (nk=inds[i].key) == 97 )
        {
          objc_offset( o, XWHSMLSL, &x, &dum );
          nk = mx<=x ? XS_LFPAGE : XS_RTPAGE;
        }
        else if( nk == 98 )
        {
          objc_offset( o, XWVSMLSL, &dum, &x );
          nk = my<=x ? XS_UPPAGE : XS_DNPAGE;
        }
        if( i==ind && nk==key ) copy_color( o, f, num );
        else
        {
          if( i != ind )
          {
            ind = i;
            order(o);
            opaq_3d( o, f );
            form_draw( f, XWI0 );
          }
          key = nk;
          k_set( o, f, 1 );
          return 0;
        }
      }
  }
      else if( num>=KOCNTRL && num<=KOALT ) KEYS[key].shift = k_shift(o);
  }
  return 0;
}

static void close_form( int num )
{
  forms[num].tree = 0L;
  if( !--forms_open )
  {
    (*gui->Nrsc_free)( rshdr );
#ifndef TEST
    (*set_struct->signal_xset)();
#endif
  }
}

static void dup_split( int *i )
{
  bytecpy( &i[WGUP2], &i[WGUP], 4*2 );
  bytecpy( &i[WGLEFT2], &i[WGLEFT], 4*2 );
}

int x_wset( OBJECT *o, int num, FORM *f )
{
  int sh=0, ky=0, dum, is_main;
  KEYCODE *k;

  switch( num )
  {
    case KOREAD:        /* read a keypress from user */
      k_mode( o, f, 1, 1 );   /* display "Press key" message */
      wind_update( BEG_UPDATE );
      evnt_multi( MU_KEYBD,  0,0,0,  0,0,0,0,0,
          0,0,0,0,0,  &dum,  0,0,  &dum, &dum, &dum, &sh, &ky, &dum );
      wind_update( END_UPDATE );
      k_mode( o, f, 0, 1 );   /* undraw message */
      if( sh&3 ) sh |= 3;               /* one shift key->both shift keys */
      /* fall through to process key */
    case KOCLEAR:       /* key=sh=0 already for clear */
      k = &KEYS[key];
      k->shift = sh&0xf;
      k->scan = ky>>8;
      k->ascii = 0;
      k_set( o, f, 1 );
      return 0;
    case KOOK:
      set->flags.s.use_wcolors_cpx = is_sel( o, KOCPX );
      bytecpy( set->wind_keys, KEYS, 13*sizeof(KEYCODE) );
      dum = set->wcolor_mode;
      dup_split( STATES );
      dup_split( COLORS[0] );
      dup_split( COLORS[1] );
      for( num=WGSIZE+1; --num>=0; )
        wind_set( 0, X_WF_DCOLSTAT, num, COLORS[1][num],
            COLORS[0][num], STATES[num] );
/*      bytecpy( set->wstates[set->wcolor_mode], STATES, (WGSIZE+1)*2 );
      bytecpy( set->dwcolors[set->wcolor_mode], COLORS, (WGSIZE+1)*2*2 ); */
      if( f->handle>0 ) (*set_struct->new_colors)();	/* 003 */
    default:            /* cancel */
      close_form(0);
      return 1;
  }
}

#define OBPREFS ((OB_PREFER *)f->mem_ptr)

int get_prefer( OB_PREFER **op, FORM *f )
{
  int ret;

  *op = &OBPREFS[dattnum];
  switch( dattnum )
  {
    case 0:	/* reversed from TASKMAN */
      ret = DOS3D;
      break;
    case 1:	/* reversed from TASKMAN */
      ret = DOSBOX;
      break;
    case 2:
      ret = DOSEXIT;
      break;
    case 3:
      ret = DOSOTHER;
      break;
  }
  return ret;
}

/* set one bit of the state of a sample object, and also set the
   state of the appropriate checkbox */
void d_obj( int truth, unsigned int *state, int bit, OBJECT *o, int obj )
{
  if( truth ) *state |= bit;
  set_if( o, obj, truth );
}

/* show a color/fill pattern sample */
void d_sample( OBJECT *o, int num, int fill, int val )
{
#ifndef TEST
  o[num].ob_spec.index = (o[num].ob_spec.index&0xFFFF0000L) |
      (unsigned int)(fill ? set_struct->fills[val].ob_spec.index :
      set_struct->colors[val].ob_spec.index);
#endif
}

/* hide or show an object */
static void hide_if( OBJECT *o, int num, int truth )
{
  o += num;
  if( !truth ) o->ob_flags |= HIDETREE;
  else o->ob_flags &= ~HIDETREE;
}

/* update all Dialog Options objects */
/* hide text and fill boxes based on Atari 3D state */
void d_hide( OBJECT *o, FORM *f, OB_PREFER *op, int draw )
{
  int on, show;

  on = (o[DOFBOX+1].ob_flags&HIDETREE) == 0;
  show = !op->s.atari_3D;
  if( show != on )
  {
    hide_if( o, DTBOX+1, show );
    hide_if( o, DOFBOX+1, show );
    if( draw )
    {
      form_draw( f, DTBOX );
      if( !(draw&4) ) form_draw( f, DOFBOX );
    }
  }
}

void d_stat( OBJECT *o, FORM *f, int draw )
{
  OB_PREFER *op;
  unsigned int obj, *state, *flags;
  OBJECT *ch;

  /* get the currently chosen type of object */
  obj = get_prefer( &op, f );
  ch = &o[obj];
  /* default state: everything off */
  *(state = &ch->ob_state) = X_MAGIC;
  *(flags = &ch->ob_flags) = TOUCHEXIT;
  /* add each attribute */
  d_obj( op->s.outlined,    state, OUTLINED,     o, DOUTLINE );
  d_obj( op->s.shadowed,    state, SHADOWED,     o, DSHADOW );
  d_obj( op->s.draw_3D,     state, X_DRAW3D,     o, DGNVA3D );
  d_obj( op->s.rounded,     state, X_ROUNDED,    o, DROUND );
  d_obj( op->s.shadow_text, state, X_SHADOWTEXT, o, DTSHAD );
  d_obj( op->s.bold_shadow, flags, X_BOLD,       o, DTBOLD );
  d_obj( op->s.atari_3D,    flags, obj==DOSBOX ? FL3DBAK : FL3DACT,
      o, DATARI3D );
  set_if( o, DOPAQUE, op->s.textmode );
  /* and set the correct sample with the current state */
  if( op->s.atari_3D )
  { /* even though these are G_BOXTEXT objects, emulate G_BUTTONs */
    if( obj==DOSBOX ) ch->ob_spec.index =
        (ch->ob_spec.index&0xFFFF0000L) | 0x1000;
    else
    {
      ch->ob_spec.tedinfo->te_color = 0x1000;
      if( ch->ob_height >= z->graphics->cel_ht )
      { /* adjust size for expanded border */
        ch->ob_x += 2;
        ch->ob_y += 2;
        ch->ob_width -= 4;
        ch->ob_height -= 4;
      }
    }
  }
  else
  {
    if( obj==DOSBOX ) ch->ob_spec.index =
        (ch->ob_spec.index&0xFFFF0000L) | (unsigned int)op->l;
    else
    {
      ch->ob_spec.tedinfo->te_color = (unsigned int)op->l;
      if( ch->ob_height < z->graphics->cel_ht )
      { /* it was an Atari 3D, so reset size */
        ch->ob_y -= 2;
        ch->ob_x -= 2;
        ch->ob_width += 4;
        ch->ob_height += 4;
      }
    }
  }
  /* set the popup samples */
  d_sample( o, DOFLEFT+1, 1, op->s.fillpattern );
  d_sample( o, DOPLEFT+1, 0, op->s.interiorcol );
  d_sample( o, DOTLEFT+1, 0, op->s.textcol );
  d_sample( o, DOBLEFT+1, 0, op->s.framecol );
  d_hide( o, f, op, draw );
  if( draw&1 ) form_draw( f, obj==DOSBOX ? DOSBOX-1 : DOSBOX );
  if( draw&2 ) form_draw( f, DOBBOX );
  if( draw&4 )
  {
    form_draw( f, DOTBOX );
    form_draw( f, DOFBOX );
  }
  if( draw&8  ) form_draw( f, DOFLEFT+1 );
  if( draw&16 ) form_draw( f, DOPLEFT+1 );
  if( draw&32 ) form_draw( f, DOTLEFT+1 );
  if( draw&64 ) form_draw( f, DOBLEFT+1 );
}

int i_dset( OBJECT *o, FORM *f )
{
  int old;
  
  if( !o )
  {
    rsrc_adr( 0, DIALOPTS, (void **)&f->tree );
    x_form_center( f->tree, &dum, &dum, &dum, &dum );
    return 1;
  }
  bytecpy( OBPREFS, &set->color_3D, 4*sizeof(OB_PREFER) );
  old = dattnum;
  for( dattnum=0; dattnum<4; dattnum++ )
    if( dattnum!=old ) d_stat( o, f, 0 );  /* skip the current one */
  dattnum = old;
  d_stat( o, f, 0 );       /* do the current one last */
  forms_open++;
  return 1;
}

void touch_it( OBJECT *o, int num, FORM *f )
{
  int dum;
  
  form_button( o, num, 1, &dum );
  t_dset( o, num, f );
}

int t_dset( OBJECT *o, int num, FORM *f )
{
  OB_PREFER *op;
  int update;
  static char xl[] = { 1, 0, 2, 3 };

  if( num>=DOOUT && num<=DOOTHER )      /* new object type */
  {
    if( (num=xl[num-DOOUT]) != dattnum )       /* and it really is new */
    {
      dattnum = num;
      update = 4|2;
    }
  }
  else
  {
    update = 1;
    get_prefer( &op, f );                  /* get current state */
    switch( num )
    {
      case DOSBOX:
      case DOSBOXT:
        touch_it( o, DOOUT, f );		/* switch to outer box */
        return 0;
      case DOS3D:
        touch_it( o, DO3DBUT, f );		/* switch to 3D button */
        return 0;
      case DOSEXIT:
        touch_it( o, DOEXIT, f );		/* switch to exit button */
        return 0;
      case DOSOTHER:
        touch_it( o, DOOTHER, f );		/* switch to other */
        return 0;
      case DOUTLINE:                    /* toggle outline */
        op->s.outlined ^= 1;
        break;
      case DSHADOW:                     /* toggle shadow */
        op->s.shadowed ^= 1;
        break;
      case DTSHAD:                      /* toggle text shadow */
        op->s.shadow_text ^= 1;
        break;
      case DTBOLD:                      /* toggle bold shadow */
        op->s.bold_shadow ^= 1;
        break;
      case DATARI3D:                    /* toggle Atari 3D effect */
        if( (op->s.atari_3D ^= 1) != 0 )
        {
          op->s.draw_3D = 0;
          o[DGNVA3D].ob_state&=~SELECTED;
          form_draw( f, DGNVA3D );
        }
        break;
      case DGNVA3D:                     /* toggle Geneva 3D effect */
        if( (op->s.draw_3D ^= 1) != 0 )
        {
          op->s.atari_3D = 0;
          o[DATARI3D].ob_state&=~SELECTED;
          form_draw( f, DATARI3D );
        }
        break;
      case DROUND:                      /* toggle round */
        op->s.rounded ^= 1;
        break;
      case DOPAQUE:                     /* toggle text transparent/opaque */
        op->s.textmode ^= 1;
        break;
      case DOFLEFT:                     /* previous fill pattern */
        op->s.fillpattern--;
        update |= 8;
        break;
      case DOFLEFT+2:                       /* next fill pattern */
        op->s.fillpattern++;
        update |= 8;
        break;
#ifndef TEST
      case DOFLEFT+1:                       /* fill pattern popup */
        op->s.fillpattern = (*set_struct->popup_fill)( o, DOFLEFT+1, op->s.fillpattern );
        update |= 8;
        break;
#endif
      case DOPLEFT:                   /* previous interior color */
        op->s.interiorcol--;
        update |= 16;
        break;
      case DOPLEFT+2:                     /* next interior color */
        op->s.interiorcol++;
        update |= 16;
        break;
#ifndef TEST
      case DOPLEFT+1:                     /* interior color popup */
        op->s.interiorcol = (*set_struct->popup_col)( o, DOPLEFT+1, op->s.interiorcol );
        update |= 16;
        break;
#endif
      case DOTLEFT:                   /* previous text color */
        op->s.textcol--;
        update |= 32;
        break;
      case DOTLEFT+2:                     /* next text color */
        op->s.textcol++;
        update |= 32;
        break;
#ifndef TEST
      case DOTLEFT+1:                     /* text color popup */
        op->s.textcol = (*set_struct->popup_col)( o, DOTLEFT+1, op->s.textcol );
        update |= 32;
        break;
#endif
      case DOBLEFT:                   /* previous frame color */
        op->s.framecol--;
        update |= 64;
        break;
      case DOBLEFT+2:                     /* next frame color */
        op->s.framecol++;
        update |= 64;
        break;
#ifndef TEST
      case DOBLEFT+1:                     /* frame color popup */
        op->s.framecol = (*set_struct->popup_col)( o, DOBLEFT+1, op->s.framecol );
        update |= 64;
        break;
#endif
    }
  }
  d_stat( o, f, update );
  return 0;
}

int x_dset( OBJECT *o, int num, FORM *f )
{
  if( num==DOOK )
  {
    bytecpy( &set->color_3D, OBPREFS, 4*sizeof(OB_PREFER) );
    if( f->handle>0 ) form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect(z->maximum) );
  }
  close_form(1);
  return 1;
}

#pragma warn +par

static int start_dial( int num )
{
  return set_struct->modal_ret = (*gui->start_form)( set_struct->AES_handle, NAME|CLOSER|MOVER, &forms[num] );
}

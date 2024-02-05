#define NO_FDI  0x71

#include "new_aes.h"
#include "vdi.h"
#include "tos.h"
#include "xwind.h"
#include "ierrno.h"
#include "string.h"
#include "stdlib.h"
#include "multevnt.h"
#define _FORMS
#include "..\neocommn.h"
#include "win_var.h"
#include "win_inc.h"

#define NIL     -1
#define ROOT    0
#define FALSE   0
#define TRUE    1
#define OB_NEXT(x)      (((OBJECT2 *)tree)[x].ob_next)
#define OB_HEAD(x)      (((OBJECT2 *)tree)[x].ob_head)
#define OB_TAIL(x)      (((OBJECT2 *)tree)[x].ob_tail)
#define OB_TYPE(x)      (((OBJECT2 *)tree)[x].ob_type)
#define OB_FLAGS(x)     (((OBJECT2 *)tree)[x].ob_flags)
#define OB_STATE(x)     (((OBJECT2 *)tree)[x].ob_state)

#define M1_ENTER        0x0000
#define M1_EXIT         0x0001

#define BS      0x0E00
#define TAB     0x0F00
#define CR      0x1C00
#define KCR     0x7200
#define ESC     0x0100
#define UP      0x4800
#define DOWN    0x5000
#define DEL     0x5300
#define UNDO    0x6100
#define HELP    0x6200

int fn_obj;                 /* Found tabable object     */
int fn_prev;                /* Last EDITABLE obj seen   */
int asc_x2, asc_y2;
Rect asc;
char mouse_curs=1, fn_alt;

#define hide_mouse() graf_mouse( M_OFF, 0L )
#define show_mouse(a) graf_mouse( M_ON, 0L )

        void                            /* If the object is not already */
objc_sel(long tree, int obj);                  /* SELECTED, make it so.*/
        void                            /* If the object is SELECTED,   */
objc_dsel(long tree, int obj);                 /* deselect it.         */
        void                            /* Return the object's GRECT    */
objc_xywh(long tree, int obj, Rect *p);                /* through 'p' */
        int                            /* Find the parent object of    */
get_parent(long tree, int obj);            /* by traversing right until*/
        int
inside(int x, int y, GRECT *pt);      /* determine if x,y is in rectangle*/
        int
rc_intersect(GRECT *p1, GRECT *p2);     /* compute intersection of two GRECTs*/
        void
rc_copy(GRECT *psbox, GRECT *pdbox);  /* copy source to destination rectangle*/
        int
break_x(int *pxy);
        int
break_y(int *pxy);
        int
break_obj(long tree, int obj);                 /* Called once per object to*/
        int                            /* Manages mouse rectangle events */
form_hot(long tree, int hot_obj, int mx, int my, GRECT *rect, int *mode);
        void
do_radio(long tree, int obj, int wait);
int gform_dial( int flag, Rect *small, Rect *big );

#define do_alts   curapp->flags.flags.s.kbd_equivs
#define undo_alts curapp->flags.flags.s.undo_equivs

void fix_int( int *i, int cel )
{
  *i = (*i&0xFF)*cel + (*i>>8);
}

void line_init(int bp)
{
  _vsl_color( bp );
  _vsl_type( 1 );
  _vswr_mode( 1 );
}

static void cdecl draw_line( int i, ... )
{
  v_pline( vdi_hand, 2, &i );
}

void adjust_rect( OBJECT *obj, Rect *r, int frame )
{
  int i, j;
  char round=0;
  OBDESC od;

  i = j = frame ? (char)(((char)obj->ob_type==G_BUTTON ? but_spec(obj) :
      get_spec(obj))>>16) : 0;
  get_obdesc( (OBJECT2 *)obj, r, &od );
  /* rounded, outlined is a special case */
  if( (!form_app || form_app->flags.flags.s.round_buttons) &&
      od.magic && (od.state&(X_ROUNDED|OUTLINED)) == (X_ROUNDED|OUTLINED) )
  {
    i = j-2;
    round++;
  }
  else if( i>-3 && od.state&OUTLINED ) i = -3;
  if( i<0 )
  {
    r->x += i;
    r->y += i;
    r->w -= (i<<1);
    r->h -= (i<<1);
  }
  if( od.state&SHADOWED )
  {
    i = (abs(j)<<1);
    if( j>1 ) i -= j-1;
    if( round )
      if( (i-=2) <= 0 ) return;
    r->w += i;
    r->h += i;
  }
}

int is_xusrdef( APP *ap, OBJECT *tree )
{
  int typ;

  return (tree->ob_state&X_MAGMASK) == X_MAGIC && 
      ((typ=get_typex(ap,(OBJECT2 *)tree)) == X_USRDEFPRE ||
      typ == X_USRDEFPOST);
}

long get_spec( OBJECT *tree )
{
  long ptr;

  ptr = tree->ob_spec.index;
  if( tree->ob_flags & INDIRECT ) return *(long *)ptr;
  if( is_xusrdef(form_app,tree) ) return ((USERBLK *)ptr)->ub_parm;
  return( ptr );
}

long but_spec( OBJECT *o )
{
  if( (o->ob_flags&(DEFAULT|EXIT) ) == (DEFAULT|EXIT) )
      return( 0x00FD1170L );
  if( o->ob_flags&(DEFAULT|EXIT) ) return( 0x00FE1170L );
  return( 0x00FF1170L );
}

void objc_toggle( long tree, int obj )
{
  Rect root;

  objc_xywh(tree, ROOT, &root);
  adjust_rect( (OBJECT *)tree, &root, 1 );
  change_objc((OBJECT *)tree, form_app, obj, &root, OB_STATE(obj)^SELECTED, 1);
}

void objc_sel( long tree, int obj )
{
  if ( !(OB_STATE(obj) & SELECTED) ) objc_toggle(tree, obj);
}

void objc_dsel( long tree, int obj )
{
  if (OB_STATE(obj) & SELECTED) objc_toggle(tree, obj);
}

/************* Keyboard manager and subroutines ***************/

int find_def( OBJECT *ob, int obj )
{
  ob += obj;
  if (HIDETREE & ob->ob_flags) return (FALSE);
    if ( ob->ob_flags&DEFAULT )
      if ( !(DISABLED & ob->ob_state) )
            fn_obj = obj;   /* Record object number                 */
  return (TRUE);
}

int find_ndef( OBJECT *ob, int obj )
{
  ob += obj;
  if (HIDETREE & ob->ob_flags) return (FALSE);
    if ( fn_obj==NIL && (ob->ob_flags&(DEFAULT|EXIT|SELECTABLE)) ==
        (EXIT|SELECTABLE) && !(DISABLED & ob->ob_state) )
        fn_obj = obj;   /* Record object number                 */
  return (TRUE);
}

int edit_ok( OBJECT *ob, int num )
{
  return (ob[num].ob_flags&EDITABLE) && !(ob[num].ob_state&DISABLED);
}

int find_tab( OBJECT *ob, int obj ) /* Look for target of TAB operation.    */
{                       /* Check for hidden subtree.           */
  int i;

  ob += obj;
  if (HIDETREE & ob->ob_flags) return (FALSE);
                          /* If not EDITABLE, who cares?          */
  if ( !edit_ok(ob,0) /*||
      ((i=ob->ob_type) != G_FTEXT && i != G_FBOXTEXT)*/ )
          return (TRUE);
                          /* Check for forward tab match          */
  if (fn_dir && fn_prev == fn_last)
          fn_obj = obj;
                          /* Check for backward tab match         */
  if (!fn_dir && obj == fn_last)
          fn_obj = fn_prev;
  fn_prev = obj;          /* Record object for next call.         */
  return (TRUE);
}

int find_edit( OBJECT *ob, int obj )   /* Look for next editable field.*/
{                       /* Check for hidden subtree.           */
  int i;

  ob += obj;
  if (HIDETREE & ob->ob_flags) return (FALSE);
                          /* If not EDITABLE, who cares?          */
  if ( !edit_ok(ob,0) /*||
      ((i=ob->ob_type) != G_FTEXT && i != G_FBOXTEXT)*/ )
          return (TRUE);
  if( fn_obj==NIL ) fn_obj = obj;
  return (TRUE);
}

char *get_butstr( long tree, int obj, int *is_ted, int disab )
{
  char *ptr=0L;
  static char gchar[2]="x";
  OBJECT *ob = &(((OBJECT *)tree)[obj]);

  *is_ted = 0;
  if ( !fn_dir || fn_dir & ob->ob_flags )
    if ( !disab || !(DISABLED & ob->ob_state) )
    {
      switch( (char)ob->ob_type )
      {
        case G_BOXCHAR:
          *(ptr = gchar) = get_spec(ob)>>24;
          break;
        case G_BOXTEXT:
        case G_TEXT:
          ptr=(char *)((TEDINFO *)get_spec(ob))->te_ptext;
          *is_ted = 1;
          break;
        case G_FTEXT:
        case G_FBOXTEXT:
          ptr=(char *)((TEDINFO *)get_spec(ob))->te_ptmplt;
          *is_ted = 1;
          break;
        case G_BUTTON:
        case G_STRING:
        case G_TITLE:
          ptr=(char *)get_spec(ob);
          break;
      }
    }
  return(ptr);
}

int strxcmp( char *ptr1, char *ptr2 )
{
  if( *ptr1=='[' ) ptr1++;
  while( (*ptr1&0xdf) == (*ptr2&0xdf) )
  {
    if( !*ptr1 ) return(1);
    ptr1++;
    ptr2++;
    if( *ptr1=='[' ) ptr1++;
  }
  if( !*ptr2 )
  {
    while( *ptr1 == ' ' ) ptr1++;
    if( !*ptr1 ) return(1);
  }
  return(0);
}

int find_exit( OBJECT *ob, int obj )
{
  char *ptr, *ptr2;
  int dum;
  long tree = (long)ob;

  ob += obj;
  if (HIDETREE & ob->ob_flags)
          return (FALSE);
  if( fn_obj==NIL )
    if( get_typex(form_app,(OBJECT2 *)ob) == X_RADCHKUND && ob->ob_flags&(1<<11) ) fn_obj = obj;
    else if( (ptr=get_butstr( tree, obj, &dum, 1 )) != 0 )
    {
      while( *ptr==' ' ) ptr++;
      for( ptr2=undo_ptr; *ptr2; ptr2+=strlen(ptr2)+2 )
        if( strxcmp(ptr,ptr2) ) fn_obj = obj;
    }
  return (TRUE);
}

int fnd_hu( OBJECT *ob, int obj, int type )
{
  ob += obj;

  if( ob->ob_flags&HIDETREE ) return (FALSE);
  if( fn_obj==NIL )
    if( get_typex(form_app,(OBJECT2 *)ob) == type ) fn_obj = obj;
  return (TRUE);
}

int find_help( OBJECT *ob, int obj )
{
  return fnd_hu( ob, obj, X_HELP );
}

int find_undo( OBJECT *ob, int obj )
{
  return fnd_hu( ob, obj, X_UNDO );
}

void draw_alt( long tree, int obj, char *ptr, int off, int is_ted,
    int undraw )
{
  TEDINFO *ted;
  int w, j, type;
  unsigned long spec;
  Rect g;
  unsigned int c;
  OBJECT *ob = u_object((OBJECT *)tree,obj);
  OBDESC od;

  objc_xywh( tree, obj, &g );
  ted = (TEDINFO *)(spec=get_spec(ob));
  j = !is_ted || ted->te_font==IBM;
  get_obdesc( (OBJECT2 *)ob, 0L, &od );
  if( (od.state&(X_MAGMASK|X_SMALLTEXT)) == (X_MAGIC|X_SMALLTEXT) ) j=0;
  g.x += off*(w=j?char_w:6);
  if( (type=(char)ob->ob_type) != G_STRING && type!=G_TITLE )
      switch( is_ted ? ted->te_just : TE_CNTR )
  {
    case TE_RIGHT:
      g.x += g.w - (off+strlen(ptr))*w;
      break;
    case TE_CNTR:
      g.x += (g.w - (off+strlen(ptr))*w+1)>>1;
      break;
  }
  g.y += (g.h>>1) + (!j ? 3 : (char_h-2)>>1 /* 003: was (char_h>10 ? (char_h-4)>>1 : (char_h-2)>>1) */ );
  txt_app = form_app;
  /*c = od.color.l; 003 */
  if( od.atari3d != 0 && od.atari_move && od.state&SELECTED ||
      od.magic && (od.state&(X_DRAW3D|SELECTED)) == (X_DRAW3D|SELECTED) )
  {
    g.x++;
    g.y++;
    od.state &= ~SELECTED;
  }
  txt_app = 0L;
  if( type==G_TITLE )
  {
    _vsl_color(1);
    _vsl_type(1);
    _vswr_mode(3);
  }
  else
  {
    if( !od.atari3d )
    {
      if( is_ted ) c = ted->te_color;
      else if( type==G_BOXCHAR ) c = spec;
      else c = (od.color.l&0xF0F0) | (undraw ? 0 : 0x101);  /* 003: added | */
      od.color.l = c;	/* 003 */
      if( od.magic && od.state&X_SHADOWTEXT && (!od.color.b.textcol ||
          od.color.b.interiorcol && od.color.b.fillpattern) ) c = c ? 0 : 0x101;  /* 004 */
      if( od.state&SELECTED ) xor_col(&c);
    }
    else c = od.atari_col;	/* 003 */
    if( !undraw ) c >>= 8;
    line_init( c&0xf );
  }
  draw_line( g.x, g.y, g.x+w-1, g.y );
}

int find_alt( OBJECT *ob, int obj )
{
  char *ptr, c;
  int j, i, is_ted;
  long tree = (long)ob;

  ob = ob+obj;
  if (HIDETREE & ob->ob_flags) return (FALSE);
  if( /*(fn_dir && (ob->ob_type&fn_dir)==0) ||*/ (ob->ob_state&(X_MAGMASK|X_KBD_EQUIV)) ==
      (X_MAGIC|X_KBD_EQUIV) || ((i=get_typex(form_app,(OBJECT2 *)ob)) == X_RADCHKUND ||
      i == X_HELP) && ((char)ob->ob_type==G_STRING || (char)ob->ob_type==G_BUTTON) ) return(TRUE);
  for( j=0; j<num_keys; j++ )
    if( alt_obj[j] == obj ) return(TRUE);
  if( num_keys < MAX_KEYS && (ptr=get_butstr( tree, obj, &is_ted, 1 )) != 0 )
    for( i=0; *ptr; i++, ptr++ )
      if( (c=*ptr&0xdf)>='A' && c<='Z' ||
          (c=*ptr)>='0' && c<='9' )
      {
        for( j=0; j<num_keys; j++ )
          if( c == alt[j] ) break;
        if( j==num_keys )
        {
          alt[j] = c;
          alt_off[j] = i;
          alt_obj[num_keys++] = obj;
          if( fn_last ) draw_alt( tree, obj, ptr, i, is_ted, 0 );
          break;
        }
      }
  return (TRUE);
}

int find_nalt( OBJECT *ob, int obj )
{
  char *ptr, c;
  int is_ted;
  long tree = (long)ob;

  if( fn_obj > 0 ) return(FALSE);
  ob = ob+obj;
  if (HIDETREE & ob->ob_flags) return (FALSE);
  if( (ob->ob_state&(X_MAGMASK|X_KBD_EQUIV)) !=
      (X_MAGIC|X_KBD_EQUIV) && (get_typex(form_app,(OBJECT2 *)ob) != X_RADCHKUND ||
      (char)ob->ob_type!=G_STRING && (char)ob->ob_type!=G_BUTTON) ) return(TRUE);
  if( (ptr=get_butstr( tree, obj, &is_ted, 1 )) == 0 ) return TRUE;  /* 003: lots of changes */
  if( fn_alt )
  {
    if( (ptr=strchr(ptr,'[')) != 0 )
    {
      if( (c = *(ptr+1))>='a' && c<='z' ) c&=0xdf;
      if( c==fn_last )
      {
        fn_obj = obj;
        return(FALSE);
      }
    }
  }
  else if( *ptr==fn_last && !*(ptr+1) )
  {
    fn_obj = obj;
    return(FALSE);
  }
  return(TRUE);
}

int scan_alts( OBJECT *tree, char ch, char is_alt )
{
  fn_last = ch;
  fn_obj = 0;
  fn_dir = SELECTABLE|TOUCHEXIT|EXIT;
  fn_alt = is_alt;	/* 004 */
  map_tree(tree, ROOT, NIL, find_nalt);
  return( fn_obj );
}

void alt_redraw( long tree, int obj, int undraw )
{
  int i, ob, is_ted;
  char *ptr;

  fn_dir = 0;
  for( i=0; i<num_keys; i++ )
    if( (char)alt_obj[i]==(char)obj )
    {
      ob = abs((char)obj);
      if( (ptr=get_butstr(tree,ob,&is_ted,1))!=0 ) draw_alt( tree,
          ob, ptr+alt_off[i], alt_off[i], is_ted, undraw );
      return;
    }
}

void form_redraw_all( long tree, int undraw )
{
  int i, is_ted;
  char *ptr;

  fn_dir = 0;
  for( i=0; i<num_keys; i++ )
    if( (ptr=get_butstr(tree,abs((char)alt_obj[i]),&is_ted,1))!=0 )
      draw_alt( tree, abs((char)alt_obj[i]), ptr+alt_off[i], alt_off[i], is_ted, undraw );
}

void form_search( OBJECT *tree, int func( OBJECT *tree, int tmp ) )
{
  fn_dir = EXIT|TOUCHEXIT;
  map_tree( tree, ROOT, fn_obj = NIL, func );
}

int __form_keybd( long tree, int edit_obj, int kr, int *out_obj, int *okr )
{
  *okr = kr;
  fn_dir = 0;             /* Default tab direction is backward.   */
  switch (kr&0xFF00) {
    case HELP:
      *okr = 0;
      form_search( (OBJECT *)tree, find_help );
      goto test;
    case UNDO:
      *okr = 0;
      form_search( (OBJECT *)tree, find_undo );
      if( fn_obj == NIL ) form_search( (OBJECT *)tree, find_exit );
test: if( fn_obj != NIL )
      {
sel:    objc_sel(tree, fn_obj);
        *out_obj = fn_obj;
        return (FALSE);
      }
      break;
    case KCR:
    case CR:        /* Zap character.                       */
      if((char)kr != '\r') break;
      *okr = 0;
              /* Look for a DEFAULT object.           */
      map_tree((OBJECT *)tree, ROOT, fn_obj = NIL, find_def);
              /* If found, SELECT and force exit.     */
      if (fn_obj != NIL) goto sel;
                                    /* Falls through to     */
    case TAB:                       /* tab if no default    */
    case DOWN:
      fn_dir = 1;             /* Set fwd direction    */
    case UP:
      *okr = 0;               /* Zap character        */
      fn_last = edit_obj;
      map_tree((OBJECT *)tree, ROOT, fn_prev = fn_obj = NIL, find_tab);
      if (fn_obj == NIL)      /* try to wrap around   */
          map_tree((OBJECT *)tree, ROOT, NIL, find_tab);
      if (fn_obj != NIL) *out_obj = fn_obj;
      break;
    default:                        /* Pass other chars     */
      return (TRUE);
  }
  return (TRUE);
}

#pragma warn -par
int form_keybd( OBJECT *tree, int edit_obj, int next, int kr, int *out_obj, int *okr )
{
  return __form_keybd( (long)tree, edit_obj, kr, out_obj, okr );
}
#pragma warn +par

/************* Mouse button manager and subroutines ***************/
void do_radio( long tree, int obj, int wait )
{
  int pobj, sobj, state;
  Mouse m;

  pobj = gfind_parent( (OBJECT *)tree, obj );  /* Get the object's parent */

  for (sobj = OB_HEAD(pobj); sobj != pobj; sobj = OB_NEXT(sobj) )
  {                               /* Deselect all but...     */
    if (sobj != obj && (OB_FLAGS(sobj)&RBUTTON) )
      objc_dsel(tree, sobj);
  }
  objc_sel(tree, obj);                    /* the one being SELECTED  */
  if( wait )
    do
      mks_graf( &m, 0 );
    while( m.b&1 );
}

int form_button( OBJECT *tree, int obj, int clicks, int *next_obj )
{
  return _form_button( tree, curapp, obj, clicks, next_obj );
}

int _form_button( OBJECT *tree, APP *ap, int obj, int clicks, int *next_obj )
{
  unsigned char flags, state, texit, sble, dsbld;
  int hibit, ret=TRUE;

  flags = u_object(tree,obj)->ob_flags;           /* Get flags and states   */
  state = u_object(tree,obj)->ob_state;
  texit = flags & TOUCHEXIT;
  sble = flags & SELECTABLE;
  dsbld = state & DISABLED;

  if( state&SELECTED && !sble && (flags&(EXIT|TOUCHEXIT))==EXIT && !dsbld )
  {	/* 003: special case: fool into touchexit */
    texit = 1;
    clicks = clicks!=0;
  }
  
  if ( !(flags&(TOUCHEXIT|EDITABLE|EXIT)) && (!sble || dsbld) ) /* This is not an      */
  {                                /* interesting object  */
    *next_obj = 0;
    return (TRUE);
  }

  if (texit && clicks == 2)               /* Preset special flag  */
          hibit = 0x8000;
  else
          hibit = 0x0;

  if (sble && !dsbld)                     /* Hot stuff!           */
  {
    ret = 2;  /* 003: return a unique value so X_WTFL_KEYS works right */
    if (flags & RBUTTON)            /* Process radio buttons*/
    {
      do_radio((long)tree, obj, !sble||!texit);    /* immediately!         */
                  /* selectable, touchexit, radio button is special */
    }
    else if (!_graf_watchbox((OBJECT *)tree, ap, obj, state^SELECTED, state ))
    {                       /* He gave up...  */
      *next_obj = 0;
      return ret;	/* 003 */
    }
  }

  if (texit || flags&EXIT && sble&&!dsbld&&!(state&SELECTED) ) /* checks old state */
          /* Exit conditions.             */
  {
    *next_obj = obj | hibit;
    return (FALSE);         /* Time to leave!               */
  }
  else if (!(flags&EDITABLE) )    /* Clear object unless tabbing  */
          *next_obj = 0;
  else *next_obj = obj;           /* EDITABLE */

  return ret;
}

void move_curs( long tree, int edit_obj, int *idx, int nidx )
{
  if( nidx != *idx )
  {
    objc_edit((OBJECT *)tree, edit_obj, 0, idx, ED_END );
    *idx = nidx;
    objc_edit((OBJECT *)tree, edit_obj, 0, idx, ED_END );
  }
}

/*%
int last_but;

int _multi( long tree )
{
  long l;
  int up, pos=0/*%, buffer[8]*/;
  Mouse m;

  br=1;
  kr=0;
  for(;;)
  {
/*%    multi_evnt( &emulti, buffer ); */
    mks_graf( &m, 0 );
/*%    ks = emulti.mouse_k; */
    ks = m.k;
    if( m.b&1  /*%emulti.mouse_b&1*/ )
    {
      mx = m.x;  /*%emulti.mouse_x;*/
      my = m.y;  /*% emulti.mouse_y; */
      br = 1;
      if( (m_obj = objc_find((OBJECT *)tree, ROOT, MAX_DEPTH, mx, my)) >= 0
          && !(last_but&1) && ((up=OB_FLAGS(m_obj))&TOUCHEXIT ||
          up&EXIT && OB_STATE(m_obj)&SELECTED) )
      {
        l = tic()+dc_pause;
        /*%reset_butq();*/
        up = 0;
        while( tic() < l )
        {
          mks_graf( &m, 0 );
          /*%graf_mkstate( &emulti.mouse_x, &emulti.mouse_y, &emulti.mouse_b, &emulti.mouse_k );*/
          if( !(m.b&1) /*% !(emulti.mouse_b&1)*/ ) up=1;
          else
          {
            if( up ) br=2;
            up=0;
          }
        }
      }
      mb = last_but = m.b;  /*%emulti.mouse_b;*/
      return(MU_BUTTON);
    }
    else last_but = 0;
    if( has_key() /*%emulti.event&MU_KEYBD*/ )
    {
      l = getkey();  /*%emulti.key; */
      if( ks == 8 )
        if( (char)l >= '0' && (char)l <= '9' )
        {
          kr = kr*10 + (char)l - '0';
          if( ++pos==3 && kr>255 ) kr=0;
        }
      if( !pos ) kr = (l>>8L) | l;
      if( !pos || pos==3 ) return(MU_KEYBD);
    }
    else if( pos && ks != 8 ) return MU_KEYBD;
  }
}
*/

static EMULTI emulti = { MU_KEYBD | MU_BUTTON, 2, 1, 1 };

int _multi( long tree )
{
  long l;
  int pos=0, buffer[8];

  br=1;
  kr=0;
  for(;;)
  {
    multi_evnt( &emulti, buffer );
/*%    mks_graf( (Mouse *)&emulti.mouse_x, 0 );*/
    ks = emulti.mouse_k;
    if( emulti.event&MU_BUTTON )
    {
      mx = emulti.mouse_x;
      my = emulti.mouse_y;
      br = emulti.times;
      m_obj = objc_find((OBJECT *)tree, ROOT, MAX_DEPTH, mx, my);
      mb = emulti.mouse_b;
      return(MU_BUTTON);
    }
    if( emulti.event&MU_KEYBD )
    {
      l = emulti.key;
      if( ks == 8 )
        if( (char)l >= '0' && (char)l <= '9' )
        {
          kr = kr*10 + (char)l - '0';
          if( ++pos==3 && kr>255 ) kr=0;
        }
      if( !pos ) kr = l;
      if( !pos || pos==3 ) return(MU_KEYBD);
    }
    else if( pos && ks != 8 ) return MU_KEYBD;
  }
}

int wind_dial( long tree )
{
  if( lastkey )
  {
    kr = lastkey;
    ks = lastsh;
    /* lastkey = 0;   causes WTFL_KEYS to fail */
    return MU_KEYBD;
  }
  m_obj = objc_find((OBJECT *)tree, ROOT, MAX_DEPTH,
      mx=curapp->mouse_x, my=curapp->mouse_y);
  br = curapp->times;
  return MU_BUTTON;
}

void reblit( long addr, int save )
{
  int px[8];

  fdb2.fd_addr = (char *)addr;
  _vs_clip( 0, 0L );
  if( !save )
  {
    px[0] = px[1] = 0;
    px[2] = (px[6]=asc_x2) - (px[4]=asc.x);
    px[3] = (px[7]=asc_y2) - (px[5]=asc.y);
    vro_cpyfm( vdi_hand, 3, px, &fdb2, &fdb0 );
  }
  else
  {
    px[4] = px[5] = 0;
    px[6] = (px[2]=asc_x2) - (px[0]=asc.x);
    px[7] = (px[3]=asc_y2) - (px[1]=asc.y);
    vro_cpyfm( vdi_hand, 3, px, &fdb0, &fdb2 );
  }
}

int fblit( int flag )
{
  return mblit( flag, &asc );
}

void blit_drag( OBJECT *tree, int clicks )
{
  int x, y, nx, ny, gx, dif, gmx, gmy, gmb, dum;
  long under;
  Mouse m;

  hide_mouse();
  under = pull_buf.l;
  if( !fblit(0xff00) ) return;	/* save dialog */
  mks_graf( &m, 0 );
  if( m.b&2 )
  {
    reblit( under, 0 );		/* blank */
    do
      mks_graf( &m, 0 );
    while( m.b&1 );
    reblit( pull_buf.l, 0 );	/* draw dialog */
  }
  else if( clicks==2 )
  {
    reblit( under, 0 );		/* blank */
    _form_center( tree, &asc, 1 );
    asc_x2 = asc.x + asc.w - 1;
    asc_y2 = asc.y + asc.h - 1;
    reblit( under, 1 );		/* save new blank */
    do
      mks_graf( &m, 0 );
    while( m.b&1 );
    reblit( pull_buf.l, 0 );	/* draw dialog */
  }
  else
  {
    graf_mkstate( &gmx, &gmy, &gmb, &dum );
    x = gmx;
    y = gmy;
    dif = tree[0].ob_x - asc.x;
    while( gmb&1 )
    {
      ny = gmy-y;
      y = gmy;
/*    if( (gx=gmx) == 0 ) gx = 1;*/
      gx = gmx;
      nx = gx-x;
      x = gx;
      if( asc.x+nx < 0 ) nx = -asc.x;
      else if( asc_x2+nx >= desktop->outer.w ) nx = desktop->outer.w-asc_x2-1;
      if( asc.y+ny < desktop->working.y ) ny = desktop->working.y-asc.y;
      else if( asc_y2+ny >= desktop->outer.y+desktop->outer.h ) ny =
          desktop->outer.y+desktop->outer.h-asc_y2-1;
      if( nx || ny )
      {
        reblit( under, 0 );		/* blank */
        tree[0].ob_x = (asc.x += nx) + dif;
        tree[0].ob_y = (asc.y += ny) + dif;
        asc_x2 += nx;
        asc_y2 += ny;
        reblit( under, 1 );		/* save new blank */
        reblit( pull_buf.l, 0 );	/* draw dialog */
      }
      graf_mkstate( &gmx, &gmy, &gmb, &dum );
    }
  }
  fblit(-1);				/* just free */
  show_mouse(1);
}

void draw_ascii( int flag )
{
  static int ox, oy;
  static Rect old;

  hide_mouse();
  if( !flag )
  {
    ox = ascii_tbl[0].ob_x;
    oy = ascii_tbl[0].ob_y;
    old = asc;
    asc.w = ascii_tbl[0].ob_width;
    asc_x2 = (asc.x = ascii_tbl[0].ob_x = (desktop->outer.w-asc.w)>>1) +
        asc.w;
    asc_y2 = (asc.y = ascii_tbl[0].ob_y = char_h+3) +
        (asc.h=ascii_tbl[0].ob_height);
    fblit(0);
    _objc_draw( (OBJECT2 *)ascii_tbl, 0L, 0, 8, 0, 0, 0, 0 );
  }
  else
  {
    ascii_tbl[0].ob_x = ox;
    ascii_tbl[0].ob_y = oy;
    fblit(1);
    asc = old;
    asc_x2 = asc.x+asc.w-1;
    asc_y2 = asc.y+asc.h-1;
  }
  show_mouse(1);
}

int wind_get( int wi_ghandle, int wi_gfield, ... );

static Rect frect;
static char asc_on=0;

void form_init( OBJECT *tree )
{
  tree[0].ob_next = -1;
  if( next_obj<0 ) next_obj = 0;
  if( !edit_ok(tree,next_obj) /*||
      ((oidx=OB_TYPE(next_obj)) != G_FTEXT &&
      oidx != G_FBOXTEXT)*/ )
  {
    map_tree(tree, ROOT, fn_obj = NIL, find_edit);
    next_obj = fn_obj == NIL ? 0 : fn_obj;
  }
  edit_obj = -1;
}

void form_reinit( int next, int edit, int idx, int curs )
{
  int i;

  next_obj = next;
  edit_idx = idx;
  edit_obj = edit;
  mouse_curs = curs;
  num_keys=0;
  form_app = curapp;
}

int udlr_equiv( OBJECT *o )
{
  int i, ret=0;
  static char wk[] = { XS_UPLINE, XS_DNLINE, XS_LFLINE, XS_RTLINE },  /* 003 */
      wch[] = { '', '', '', '' };

  for( i=0; i<4; i++ )
    if( is_key( &settings->wind_keys[wk[i]], ks, kr ) &&
        (ret = scan_alts(o,wch[i],0)) > 0 ) return ret;
  return ret;
}

unsigned char key_2ascii(void)
{
  unsigned int i;
  unsigned char ch;

  i = (unsigned)kr>>8;
  if( i>=0x78 && i<=0x81 ) i-=0x78-0x2;
  ch = Keytbl((void *)-1L, (void *)-1L, (void *)-1L)->unshift[i];
  if( ch>='a' && ch<='z' ) ch &= 0xdf;
  return ch;
}

void edit_curs( OBJECT *tree, int type, int cont )
{
  if( !type )  /* possible new edit */
  {
    if( edit_obj != next_obj && next_obj != 0 )
    {
      objc_edit(tree, edit_obj=next_obj, 0, &edit_idx, ED_INIT);
      next_obj = 0;
    }
  }    /* show/erase current */
  else if( edit_obj && (!cont || next_obj != edit_obj && next_obj != 0) )
      objc_edit(tree, edit_obj, 0, &edit_idx, ED_END);
}

void edit_key( long tree, int cont )
{
  int max;

  if( kr )
  {
    if( (max = _xobjc_edit((OBJECT *)tree, form_app, edit_obj, kr|((long)ks<<16), &edit_idx, ED_CHAR)) == 2 )
    {
      edit_obj--;
      edit_curs( (OBJECT *)tree, 1, 0 );  /* turn on */
    }
    else if( max==3 )
    {
      edit_obj++;
      edit_curs( (OBJECT *)tree, 1, 0 );  /* turn on */
    }
  }
  else if( next_obj && cont &&
      u_tedinfo((OBJECT *)tree,next_obj<edit_obj?next_obj:edit_obj)->te_tmplen==X_LONGEDIT )
  {
    edit_curs( (OBJECT *)tree, 1, 0 );  /* turn off */
    edit_obj = next_obj;
    next_obj = 0;
    if( edit_idx > (max=strlen(u_ptext((OBJECT *)tree,edit_obj))) ) edit_idx = max;
    edit_curs( (OBJECT *)tree, 1, 0 );  /* turn on */
  }
}

int scrp_txt( char *p, int del )
{
  if( x_scrp_get( p, del ) )
  {
    strcat( p, "SCRAP.TXT" );
    return 1;
  }
  return 0;
}

void edit_to_clip( long tree )
{
  char temp[120], *p;
  int h;

  if( scrp_txt( temp, 1 ) && (h = Fcreate(temp,0)) > 0 )
  {
    p = u_ptext((OBJECT *)tree,edit_obj);
    Fwrite( h, strlen(p), p );
    Fwrite( h, 2, "\r\n" );
    Fclose(h);
  }
}

void edit_from_clip( long tree )
{
  char temp[120];
  int h;

  if( scrp_txt( temp, 0 ) && (h = Fopen(temp,0)) > 0 )
  {
    kr = 0;
    ks = 0;
    for(;;)
    {
      if( Fread( h, 1L, (char *)&kr + 1 ) <= 0 ) break;
      if( (char)kr=='\r' || (char)kr=='\n' ) break;
      edit_key( tree, 1 );
    }
    Fclose(h);
  }
}

int form_event( long tree, int event( long tree ), int outside )
{
  int which;
  int cont=TRUE;
  int max, oidx;
  TEDINFO *ted;
  Mouse m;

  which = event(tree);
                                         /* handle button event  */
  if (which == MU_BUTTON)
  {
    if( asc_on )
    {
      asc_on=0;
      if( (oidx=objc_find( ascii_tbl, 0, 8, mx, my )) > 0 )
      {
        kr = set_asc_cur( mx, oidx );
        ascii_tbl[6].ob_state |= SELECTED;
        _objc_draw( (OBJECT2 *)ascii_tbl, 0L, 6, 0, 0, 0, 0, 0 );
        do
          mks_graf( &m, 0 );
        while( m.b&1 );
        ascii_tbl[6].ob_state &= ~SELECTED;
        ascii_tbl[6].ob_flags |= HIDETREE;
        draw_ascii(1);
        goto do_ascii;
      }
      draw_ascii(1);
      do
        mks_graf( &m, 0 );
      while( m.b&1 );
    }
    else
    {                               /* Which object hit?    */
      next_obj = m_obj;
do_button:
      if (next_obj == NIL)
      {
        next_obj = 0;
        if( !outside ) return TRUE;
        Bconout(2,7);
      }
      else                            /* Process a click      */
      {
        cont = _x_form_mouse( (OBJECT *)tree, form_app, mx, my, br, &edit_obj,
            &next_obj, &edit_idx );
        if( !cont && (oidx=next_obj&0x7fff)!=0x7fff &&
            get_typex(form_app,((OBJECT2 *)tree)+oidx) == X_MOVER &&
            ((OBJECT2 *)tree)[oidx].ob_type == G_BUTTON )
        {
          objc_edit( (OBJECT *)tree, edit_obj, 0, &edit_idx, ED_END );
          blit_drag( (OBJECT *)tree, br );
          objc_edit( (OBJECT *)tree, edit_obj, 0, &edit_idx, ED_END );
          next_obj=0;
          cont=1;
        }
      }
    }
  }
  else if (which == MU_KEYBD)
    if( outside && (kr==0x4700 || kr==0x4737) )  /* was Insert, now Clr-Home with or without Shift */
    {
      if( edit_obj && !asc_on )
      {
        draw_ascii(0);
        asc_on = 1;
      }
    }
    else
    {                               /* Control char filter  */
      if( asc_on )
      {
        asc_on=0;
        draw_ascii(1);
      }
      /* 003 if( ks==8 )*/
        if( ks==8 && kr>>8 == 0xf ) /* Alt-Tab */
        {
          map_tree((OBJECT *)tree, ROOT, fn_obj=NIL, find_def);
          if( fn_obj != NIL )
          {
            max = fn_obj;
            map_tree( (OBJECT *)tree, max, fn_obj=NIL, find_ndef );
            if( fn_obj==NIL ) map_tree( (OBJECT *)tree, ROOT, max, find_ndef );
            if( fn_obj != NIL )
            {
              objc_xywh(tree, max, &frect);
              adjust_rect( (OBJECT *)tree+max, &frect, 1 );
              ((OBJECT2 *)tree)[max].ob_flags &= ~DEFAULT;
              ((OBJECT2 *)tree)[fn_obj].ob_flags |= DEFAULT;
              _objc_draw( (OBJECT2 *)tree, form_app, ROOT, MAX_DEPTH, frect.x,
                  frect.y, frect.w, frect.h );
              hide_mouse();
              alt_redraw( tree, max, 0 );
              _objc_draw( (OBJECT2 *)tree, form_app, fn_obj, MAX_DEPTH, 0, 0,
                  0, 0 );
              alt_redraw( tree, fn_obj, 0 );
              show_mouse(1);
            }
          }
          goto end;
        }
        else if( ks==8 || edit_obj<=0 && !(ks&0xC) &&
            (outside && settings->flags.s.no_alt_modal_equiv ||
            !outside && settings->flags.s.no_alt_modeless_eq) )	/* 003 */
        {
          max = key_2ascii();	/* 003: function now */
          for( oidx=0; oidx<num_keys; oidx++ )
            if( alt[oidx]==max )
            {
              next_obj = alt_obj[oidx];
              which=0;
              br=1;
              goto do_button;
            }
          if( (next_obj = scan_alts((OBJECT *)tree,max,1)) > 0 ||
              (next_obj = udlr_equiv( (OBJECT *)tree )) > 0 )	/* 003 */
          {
            which = 0;
            br = 1;
            goto do_button;
          }
        }
        else if( (next_obj = udlr_equiv( (OBJECT *)tree )) > 0 )	/* 003 */
        {
          which = 0;
          br = 1;
          goto do_button;
        }
        else if( edit_obj>0 && (max=ks&0xb)&3 && max&8 )	/* 003: clipboard ops */
          if( (max = key_2ascii()) == 'X' )
          {
            edit_to_clip(tree);
            ks = 0;
            kr = '\033';	/* clear line, below */
          }
          else if( max == 'C' )
          {
            edit_to_clip(tree);
            goto end;
          }
          else if( max == 'V' )
          {
            edit_from_clip(tree);
            goto end;
          }
      cont = __form_keybd(tree, edit_obj, kr, &next_obj, &kr);
      if( cont && edit_obj>0 ) cont = 2;	/* 003: prevent passing key to app */
do_ascii:
      if( edit_obj>0 )	/* was !=0 in 003 */
      {
        if( kr )
        {
          if( (max = _xobjc_edit((OBJECT *)tree, form_app, edit_obj, kr|((long)ks<<16), &edit_idx, ED_CHAR)) == 2 )
          {
            edit_obj--;
            edit_curs( (OBJECT *)tree, 1, 0 );  /* turn on */
          }
          else if( max==3 )
          {
            edit_obj++;
            edit_curs( (OBJECT *)tree, 1, 0 );  /* turn on */
          }
        }
        else if( next_obj && cont &&
            ((OBJECT *)tree)[next_obj<edit_obj?next_obj:edit_obj].ob_spec.tedinfo->
            te_tmplen==X_LONGEDIT )
        {
          edit_curs( (OBJECT *)tree, 1, 0 );  /* turn off */
          edit_obj = next_obj;
          next_obj = 0;
          if( edit_idx > (max=strlen(((OBJECT *)tree)[edit_obj].ob_spec.tedinfo->te_ptext)) ) edit_idx = max;
          edit_curs( (OBJECT *)tree, 1, 0 );  /* turn on */
        }
      }
    }
end:
  mouse_curs = 1;
  return( cont );
}

int form_do( OBJECT *tr, int next )
{
  long tree = (long)tr;
  int cont;
                                          /* Init. editing        */
  _wind_update(BEG_MCTRL);
  next_obj = next;
  form_init( tr );
  form_reinit( next_obj, 0, 0, 1 );

  /* find alt key equivs */
  hide_mouse();
  if( (((OBJECT2 *)tree)->ob_statex&((X_MAGMASK|X_KBD_EQUIV)>>8)) !=
      ((X_MAGIC|X_KBD_EQUIV)>>8) && do_alts )
  {
    fn_last = 1;	/* draw */
    fn_dir = EXIT;
    map_tree((OBJECT *)tree, ROOT, NIL, find_alt);
    fn_dir = SELECTABLE|RBUTTON|TOUCHEXIT;
    map_tree((OBJECT *)tree, ROOT, NIL, find_alt);
  }
  show_mouse(1);
                                          /* Main event loop      */
  do
  {
    edit_curs( (OBJECT *)tree, 0, 0 );
    cont = form_event( tree, _multi, 1 );
    edit_curs( (OBJECT *)tree, 1, cont );
  }
  while( cont );

  hide_mouse();
  if( undo_alts ) form_redraw_all( tree, 1 );
  show_mouse(1);
  /*%reset_butq();*/
  _wind_update(END_MCTRL);
  num_keys = 0;
  return(next_obj);
}

int spf_alert( char *s, int num )
{
  char buf[200];

  spf( buf, s, num );
  return( form_alert( 1, buf ) );
}

int sspf_alert( char *tmpl, char *s )
{
  char buf[200];

  spf( buf, tmpl, s );
  return( form_alert( 1, buf ) );
}

int _form_center( OBJECT *tree, Rect *r, int frame )
{
  objc_xywh( (long)tree, ROOT, r );
  tree[0].ob_x = r->x = desktop->working.x + (desktop->working.w-r->w)/2;
  tree[0].ob_y = r->y = desktop->working.y + (desktop->working.h-r->h)/2;
  adjust_rect( tree, r, frame );
  return(1);
}

int form_center( OBJECT *tree, int *x, int *y, int *w, int *h )
{
  Rect r;

  _form_center( tree, &r, 0 );
  *x = r.x;
  *y = r.y;
  *w = r.w;
  *h = r.h;
  return(1);
}

int x_form_center( OBJECT *tree, int *x, int *y, int *w, int *h )
{
  Rect r;

  _form_center( tree, &r, 1 );
  *x = r.x;
  *y = r.y;
  *w = r.w;
  *h = r.h;
  return(1);
}

int _form_dial( int fo_diflag, int fo_dilittlx, int fo_dilittly, int fo_dilittlw,
    int fo_dilittlh, int fo_dibigx, int fo_dibigy, int fo_dibigw, int fo_dibigh )
{
  Rect s, b;
  
  s.x = fo_dilittlx;
  s.y = fo_dilittly;
  s.w = fo_dilittlw;
  s.h = fo_dilittlh;
  b.x = fo_dibigx;
  b.y = fo_dibigy;
  b.w = fo_dibigw;
  b.h = fo_dibigh;
  return gform_dial( fo_diflag, &s, &b );
}

int gform_dial( int flag, Rect *small, Rect *big )
{
  switch( flag )
  {
/*%**    case FMD_GROW:
      growbox( small, big );
      break;
    case FMD_SHRINK:
      shrinkbox( big, small );
      break;
    case FMD_FINISH:
      redraw_all( big );
      break; *****/
    case X_FMD_START:
      asc = *big;
      asc_x2 = big->x+big->w-1;
      asc_y2 = big->y+big->h-1;
      return mblit( 0x100, big );
    case X_FMD_FINISH:
      mblit( 0x101, big );
      break;
    default:
      return form_dial( flag, small->x, small->y, small->w,
          small->h, big->x, big->y, big->w, big->h );
  }
  return(1);
}

int _form_mouse( OBJECT *tree, int mx, int my, int clicks, int *out )
{
  return _x_form_mouse( tree, curapp, mx, my, clicks, out, out+1, out+2 );
}

int _x_form_mouse( OBJECT *tree, APP *ap, int mx, int my, int clicks, int *edit_obj,
    int *next_obj, int *ed_idx )
{
  int cont, max, oidx, w;
  char *ptr, *ptr2, *ptr3;
  Rect frect;
  TEDINFO *ted;

  my++;
  cont = _form_button( tree, ap, *next_obj, clicks, next_obj );
  if( cont && *next_obj && mouse_curs && edit_ok(tree,*next_obj) )
  {
    ptr = (char *)(ted=(TEDINFO *)get_spec(&tree[*next_obj]))->te_ptmplt;
    if( (ptr2 = strchr( ptr, '_' )) != 0 )
    {
      oidx = *ed_idx;
      if( ted->te_font==GDOS_PROP || ted->te_font==GDOS_MONO || ted->te_font==GDOS_BITM )
          *ed_idx = strlen(ted->te_ptext);
      else
      {
        w = ted->te_font==SMALL ? 6 : char_w;
        objc_xywh( (long)tree, *next_obj, &frect );
        switch( ted->te_just )
        {
          case TE_RIGHT:
            max = frect.w - strlen(ptr)*w;
            break;
          case TE_CNTR:
            max = (frect.w - strlen(ptr)*w) >> 1;
            break;
          case TE_LEFT:
          default:
            max=0;
        }
        mx -= frect.x + max + w*(ptr2-ptr);
        *ed_idx = 0;
        ptr3 = (char *)ted->te_ptext;
        while( *ptr3 && mx>=w-1 )
        {
          if( *ptr2++=='_' )
          {
            ++*ed_idx;
            ptr3++;
          }
          mx -= w;
        }
      }
      if( *edit_obj != *next_obj || oidx != *ed_idx )
      {
        objc_edit(tree, *edit_obj, 0, &oidx, ED_END);
        objc_edit(tree, *edit_obj=*next_obj, 0, ed_idx, ED_END);
      }
    }
  }
  return(cont);
}

int x_form_error( char *fmt, int num )
{
  ERRSTRUCT *err;
  char *msg, temp[50];
  int i;

  if( num<0 )
  {
    for( err=dflt_errors; err->num; err++ )
      if( err->num==num ) return sspf_alert( fmt, err->str );
    if( num >= -12 ) return sspf_alert( fmt, dflt_errors[0].str );
  }
  if( num>=0 ) return form_error(num);
  spf( temp, unkn_err, num );
  return sspf_alert( fmt, temp );
}


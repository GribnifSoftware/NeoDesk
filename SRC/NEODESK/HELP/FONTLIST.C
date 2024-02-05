/* this file gets #included in HELP.C */
#define FONT_SEL
#include "font_sel.h"

#define CJar_xbios      0x434A          /* "CJ" */
#define CJar_OK         0x6172          /* "ar" */
#define CJar(mode,cookie,value)         (int)xbios(CJar_xbios,mode,cookie,value)
#define FSMC            0x46534D43L     /* FSMC FSM/Speedo cookie */
#define _SPD            0x5F535044L     /* _SPD cookie value */

static int font_num, point_size, fonts, cw, ch;
static OBJECT *wfontpop;
static char **sys_font, have_fonts;
typedef struct
{
  int id;
  char name[34],
       scale;           /* is the font a scalable Speedo/FSM font? */
} FONT_DESC;
FONT_DESC *fontlist;

void cur_point(void)
{
  int dum;

  point_size = _vst_point( point_size, &dum, &dum, &char_w, &char_h );
  max_ht = 32767/char_h;
}

void init_font( int *id, int *point )
{
  int i, dum, scale = 0;
  char temp[34];
  long *cookie;

  total_fonts = work_out[10];
  has_gdos = vq_gdos();         /* is GDOS present? */
  speedo = CJar(0,FSMC,&cookie) == CJar_OK && *cookie == _SPD;
  if( *id != 1 )
    if( !has_gdos ) *id = 1;
    else
    {
      load_fonts();
      i = total_fonts;
      while(i)
      {
        temp[32] = 0;
        if( vqt_name( vdi_hand, i, temp ) == *id )
        {
          scale = temp[32];
          break;
        }
        else i--;
      }
      if( !i )
      {
        *id = 1;
        no_fonts();
      }
    }
  else no_fonts();
  _vst_font( *id, scale );
  point_size = *point;
  cur_point();
  *point = point_size;
}

static int hide_if( OBJECT *tree, int truth, int idx )
{
  if( !truth ) tree[idx].ob_flags |= HIDETREE;
  else tree[idx].ob_flags &= ~HIDETREE;
  return truth;
}

void font_samp( OBJECT *o, int draw )
{
  TEDINFO *ted;
  int i, y, len, dx, dy;

  o[FOSAMP1+3].ob_y = char_h + (o[FOSAMP1+2].ob_y = char_h +
      (o[FOSAMP1+1].ob_y = char_h + o[FOSAMP1].ob_y));
  /* set the TEDINFOs up to display the correct font */
  len = strlen(o[FOSAMP1].ob_spec.tedinfo->te_ptext) * char_w;
  for( i=FOSAMP1, y=o[FOSAMP1].ob_y; i<FOSAMP1+4; i++, y+=char_h )
  {
    o[i].ob_width = len;
    o[i].ob_y = y;
    o[i].ob_height = char_h;
    ted = o[i].ob_spec.tedinfo;
    ted->te_font = fontlist[font_num].scale ? GDOS_MONO : GDOS_BITM;
    /* some bindings may be redefined for this */
    ted->te_junk1 = fontlist[font_num].id;
    ted->te_junk2 = point_size;
  }
  if( draw )
  {
    objc_offset( o, FOSAMP, &dx, &dy );
    objc_draw( o, FOSAMP, 8, dx, dy, o[FOSAMP].ob_width, o[FOSAMP].ob_height );
  }
}

/* a new point size has been chosen */
static void new_point( OBJECT *o, int draw )
{
  cur_point();
  /* set the new number */
  _spf( o[FOPT].ob_spec.free_string, "%d", point_size );
  /* reset and draw the sample */
  if( draw ) objc_draw( o, FOPT, 0, 0, 0, 0, 0 );
  font_samp( o, draw );
}

static unsigned int do_popup( OBJECT *o, int obj, OBJECT *pop, int parent, unsigned int val )
{
  MENU m, out;
  int x, y;
  
  m.mn_tree = pop;
  m.mn_menu = parent;
  m.mn_item = parent+val+1;
  m.mn_scroll = 1;
  objc_offset( o, obj, &x, &y );
  if( menu_popup( &m, x, y, &out ) ) return out.mn_item-parent-1;
  return -1;
}

/* choose another font to use */
static void set_fnum( OBJECT *o, int i, int draw )
{
  int dum;

  font_num = i;
  /* set the name of the current font in the dialog */
  o[FOFONT].ob_spec.free_string = fonts ?
      wfontpop[i+1].ob_spec.free_string+1 : *sys_font;
  if( fonts ) _vst_font( fontlist[i].id, fontlist[i].scale );
  else _vst_font( 1, 0 );
  /* reset point size because the previous point size might not be possible */
  if( draw ) objc_draw( o, FOFONT, 8, 0, 0, 0, 0 );
  new_point( o, draw );
}

/* allocate an item, tacking it onto the end of an existing list */
static int add_thing( void **ptr, int num, void *new, int size )
{
  int ok;

  num *= size;
  if( !*ptr )
  {
    *ptr = _lalloc( num );
    ok = *ptr != 0;
  }
  else ok = !_lrealloc( ptr, num );
  if( ok )
  {
    if(new) memcpy( (char *)(*ptr)+num-size, new, size );
    return 1;
  }
  return 0;
}

/* add a new font name to the popup list */
static int add_font( int id, char *name, char scale )
{
  int i, w;
  char *ptr;
  static OBJECT new = { -1, -1, -1, G_STRING, 0, 0, 0L, 0, 0, 0, 0 },
      root = { -1, -1, -1, G_BOX, 1<<10, SHADOWED, 0x00FF1100L };

  if( !wfontpop )
  {
    if( !add_thing( (void **)&wfontpop, 1, &root, sizeof(OBJECT) ) )
        return 0;
    wfontpop[0].ob_width = cw*24+4;
    wfontpop[0].ob_next = wfontpop[0].ob_head = wfontpop[0].ob_tail = -1;
    new.ob_height = ch;
    fonts = 0;
  }
  if( !add_thing( (void **)&wfontpop, fonts+2, &new, sizeof(OBJECT) ) ) return 0;
  if( !add_thing( (void **)&fontlist, fonts+1, 0L, sizeof(FONT_DESC) ) ) return 0;
  fontlist[fonts].id = id;
  fontlist[fonts].scale = scale;
  wfontpop[fonts+1].ob_y = fonts*ch;
  wfontpop[fonts+1].ob_width = wfontpop[0].ob_width;
  ptr = fontlist[fonts].name;
  *ptr = ' ';
  /* if the font id is 1, then it's the default system font, otherwise
     use its real name */
  strncpy( ptr+1, id==1 ? *sys_font : name, 32 );
  *(ptr+33) = 0;
  /* add this object to the fonts popup */
  objc_add( wfontpop, 0, fonts+1 );
  if( (w = strlen(ptr)*cw) > wfontpop[0].ob_width )
      wfontpop[0].ob_width = w;
  fonts++;
  for( i=0; i<fonts; i++ )
  {     /* always do this whole loop so that names get reset after add_thing */
    wfontpop[i+1].ob_spec.free_string = fontlist[i].name;
    wfontpop[i+1].ob_width = wfontpop[0].ob_width;
  }
  return 1;
}

void no_fonts(void)
{
  if( have_fonts )
  {
    have_fonts = 0;
    _vst_font( 1, 0 );
    vst_unload_fonts( vdi_hand, 0 );
    total_fonts = work_out[10];
  }
}

static void load_fonts(void)
{
  static int old;

  total_fonts = work_out[10];         /* number of system fonts */
  if( !have_fonts )
  {
    if( has_gdos )                /* yes, load fonts */
    {
      total_fonts += vst_load_fonts( vdi_hand, 0 );   /* system fonts + loaded fonts */
      have_fonts = 1;
    }
  }
  else if( old ) total_fonts = old;
  old = total_fonts;
}

void fontup( OBJECT *o, int num )
{
  int pt, np, dum;

  switch( num&0x7fff )
  {
    case FOFONT:
      if( fonts>1 ) set_fnum( o, do_popup( o, FOFONT, wfontpop, 0, font_num ), 1 );
      break;
    case FOPUP:
      for( pt=point_size+1; pt<=25; pt++ )
        if( _vst_point( pt, &dum, &dum, &dum, &dum ) == pt )
        {
          point_size = pt;
          new_point( o, 1 );
          break;
        }
      break;
    case FOPDWN:
      if( (pt=point_size-1) >= 4/*003*/ &&
          (np = _vst_point( pt, &dum, &dum, &dum, &dum )) <= pt )
        if( np < 4 ) cur_point();
        else
        {
          point_size = pt;
          new_point( o, 1 );
        }
      break;
  }
}

int *out_id, *out_pt;

int font_sel( OBJECT *o, int sys, int *oid, int *pt )
{
  int i, j, min, max, id, dum[5], maxw, wid;
  char namebuf[33];

  out_id = oid;
  out_pt = pt;
  graf_handle( &cw, &ch, dum, dum );
  rsrc_adr( 15, sys, (void **)&sys_font );
  graf_mouse( BUSYBEE, 0L );
  load_fonts();
  font_num = 0;
  for( i=1; i<=total_fonts; i++ )
  {
    namebuf[32] = 0;
    id = vqt_name( vdi_hand, i, namebuf );      /* font name */
    if( id != -1 )                              /* not a font to skip */
    {
      _vst_font( id, namebuf[32] );                 /* choose it */
      if( !namebuf[32] || !speedo )
      { /* check to see that this bitmapped or FSM font is monospaced */
        vqt_fontinfo( vdi_hand, &min, &max, dum, &maxw, dum );
        /* for 003, avoid a bug in NVDI 3 by only checking first 254 chars */
        if( (unsigned)min>1 || (unsigned)max<254 ) continue;
        /* go from the first to last character, making sure that each one is
           the full cell width */
        maxw = -1;
        for( j=1; j<=254; j++ )
        {
          vqt_width( vdi_hand, j, &wid, dum, dum );
          if( maxw>=0 && wid != maxw ) break; /* stop at the first odd char */
          maxw = wid;
        }
        if( j<255 ) continue;                   /* is the font monospaced? */
      }
      if( !add_font( id, namebuf, namebuf[32] && id!=1 ) )
      {
        if( !wfontpop )
        {
          graf_mouse( ARROW, 0L );
          if( *oid==1 ) no_fonts();
          return 0;
        }
        total_fonts = i;
        break;
      }
      if( id == *oid ) font_num = fonts-1;
    }
  }
  graf_mouse( ARROW, 0L );
  if( !wfontpop )
    if( !add_font( 1, namebuf, 0 ) ) return 0;
  wfontpop[0].ob_height = fonts*ch;
  point_size = *pt;
  set_fnum( o, font_num, 0 );
  return 1;
}

void finish_font( int num )
{
  if( num != FOOK ) init_font( out_id, out_pt );
  else
  {
    *out_id = fontlist[font_num].id;
    *out_pt = point_size;
  }
  if( wfontpop )
  {
    _lfree(wfontpop);
    wfontpop = 0L;
    _lfree(fontlist);
    fontlist = 0L;
  }
  if( *out_id==1 ) no_fonts();
}

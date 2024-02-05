#include "vdi.h"
#include "aes.h"
#include "tos.h"
#define _GRAPHICS
#include "..\neocommn.h"
#include "graphics.h"
#include "xwind.h"

extern int vdi_hand;
extern GRAPHICS *graphics;
extern Rect clip_rect;
static int ln_color;

void vdi_reset(void)
{
  speedo = graphics->speedo;
  scalable = graphics->scalable;
  is_scalable = &graphics->is_scalable;
  text_arr = graphics->text_arr;
  gr_font = ln_color = -1;
  NVDI3 = graphics->NVDI3;
}

void _v_mouse( int flag )
{
  switch( flag )
  {
    case 0:
      (*graphics->hide_mouse)();
      break;
    case 1:
      (*graphics->show_mouse)();
      break;
    case 2:
      (*clear_mouse)();
      break;
  }
}

void _vs_clip( int flag, Rect *r )
{
  int p[4];
  static Rect off = { 0, 0, 32767/2, 32767/2 };
  
  if( !flag ) r = &off;
  p[2] = (p[0] = r->x) + r->w - 1;
  p[3] = (p[1] = r->y) + r->h - 1;
  (*graphics->set_clip)( p, flag );
  clip_rect = *r;
}

void _vswr_mode( int mode )
{
  (*graphics->set_wmode)(mode-1);
}

int _vst_point( int point, int *char_width,
                int *char_height, int *cell_width, int *cell_height )
{
  int ret;
  
  ret = (*graphics->vst_point)( point, char_width, char_height, cell_width, &txt_clh );
  *cell_height = txt_clh;
  return ret;
}

void _vst_color( int color )
{
  (*graphics->vst_color)( color );
}

void _vst_alignment( int hor, int vert )
{
  (*graphics->vst_alignment)( hor, vert );
}

void _vst_effects( int eff )
{
  (*graphics->vst_effects)( eff );
}

void _vsl_udsty( int pat )
{
  (*graphics->set_lnmask)( pat );
}

void _vsl_type( int type )
{
  (*graphics->vsl_type)( type );
}

void _vsl_color( int color )
{
  (*graphics->vsl_color)( color );
}

void _vsf_style( int style )
{
  (*graphics->vsf_style)( style );
}

void _vsf_color( int color )
{
  (*graphics->desk_color)( color );
}

void _vsf_interior( int style )
{
  (*graphics->vsf_interior)( style );
}

int _vst_font( int id, int scale )
{
  return gr_font=(*graphics->vst_font)( id, scale );
}

void text_2_arr( unsigned char *str, int *num )
{
  (*graphics->text_2_arr)( str, num );
}

void _vsl_ends( int start, int end )	/* 003 */
{
  (*graphics->vsl_ends)( start, end );
}

void _vsl_width( int width )	/* 003 */
{
  (*graphics->vsl_width)( width );
}

void ftext16_mono( int x, int y, int len )
{
  (*graphics->ftext16_mono)( x, y, len );
}

void _vst_charmap( int mode )	/* 003 */
{
  (*graphics->vst_charmap)(mode);
}

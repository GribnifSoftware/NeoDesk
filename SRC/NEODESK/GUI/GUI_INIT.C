#include "neodesk.h"   /* moving folder same device tries to delete if Use Ex*/
#include "new_aes.h"
#include "xwind.h"
#include "string.h"
#include "tos.h"
#include "stdlib.h"
#include "stdio.h"
#include "neocommn.h"
#include "neod2_id.h"

#define GENEVA_EXE  0x104	/* version of Geneva in EXE */

int Nrsc_gaddr( int type, int index, void *addr, RSHDR *rsc );
int Nrsc_rcfix( RSHDR *rsc );
void Nrsc_free( RSHDR *rsc );
int Nrsc_load( char *name, DTA *dta, RSHDR **out );
int gui_init( GEMPARBLK *blk );
void gui_exit( int hand, int apexit );
void set_dclick( int rate );
void gad_redraw( int w_handle, Rect *r, int obj );
void arrow_dial( int *msg );

EXEGUI exe_gui = { gui_init, gui_exit, multi_evnt, evnt_timer, rsrc_rcfix,
    objc_draw, objc_change, objc_offset, form_dial, form_center, form_do,
    form_button, x_form_center, x_form_error, x_wdial_draw, x_wdial_change,
    menu_popup, menu_text, menu_icheck, menu_ienable, menu_tnormal,
    graf_slidebox, x_graf_rubberbox, x_graf_blit,
    x_wind_calc, x_wind_create, wind_open, wind_close, wind_delete, wind_get,
    wind_set, wind_update, wind_new, x_wind_tree, set_dclick, gad_redraw,
    menu_bar, arrow_dial, form_keybd, objc_find, 0L/*x_settings*/, 0L/*dum_msg*/,
    x_scrp_get };
GUI _gui = { start_ext_form, use_form, form_draw, close_all_fwind, prev_blit,
            Nrsc_load, Nrsc_rcfix, Nrsc_gaddr, Nrsc_free }, *gui;
extern char has_Geneva, *external_gui, *free_help;
extern GRAPHICS *graphics;
extern TREE maintree;
extern OBJECT *wtree[7];
extern int AES_handle, w_num, Geneva_ver;

#ifdef DEBUG
  long gui_load( NEO_ACC *nac, int i );
#endif

int init_gui( NEO_ACC *nac, int aes_ver )
{
  EXEGUI *exe = &exe_gui;
  
  nac->mas->most->gui = gui = &_gui;
#ifdef DEBUG
  {
    exe = (EXEGUI *)gui_load( nac, aes_ver );
#else
  if( !has_Geneva || Geneva_ver < 0x102 )
  {
    exe = (EXEGUI *)((*nac->mas->exe_load)( 2, "GUI.EXE", nac, aes_ver, &external_gui, 0 ));
#endif
    Geneva_ver = GENEVA_EXE;
  }
#ifndef DEBUG
  (*nac->mas->close_exe)();
#endif
  if( exe )
  {
    memcpy( &_gui.xtern, exe, sizeof(EXEGUI) );
    return 1;
  }
  return 0;
}

#pragma warn -par
#ifndef DEBUG
void gui_exit( int hand, int apexit )
{
  if( apexit ) appl_exit();
}

int gui_init( GEMPARBLK *blk )
{
  int ret;
  
  ret = appl_init();
  if(blk) memcpy( blk, &_GemParBlk, sizeof(GEMPARBLK) );
  return ret;
}
#endif
#pragma warn +par

/*%void shorten_bb( BITBLK *bb, RSHDR *r )
{
  int j, *ptr;

  /* clear-out every second line in the image data */
  for( j=bb->bi_hl>>1, ptr=(int *)((long)(bb->bi_pdata)+(long)r);
      --j>=0; ptr+=bb->bi_wb )
    memclr( ptr, bb->bi_wb );
  /* now, expand the image description so that every second (used)
     line becomes tacked onto the end of the previous line */
  bb->bi_x += bb->bi_wb<<3;
  bb->bi_hl >>= 1;
  bb->bi_wb <<= 1;
} */
void rsrc_fix8( RSHDR *r )
{  /* bit 13 set: no change
      !X_PREFER && xtype==1: first small text line
      !X_PREFER && xtype==2: next small text line  */
  OBJECT *ptr;
/*%  BITBLK *bb;*/
  unsigned int i;
  int y;

  for( ptr=(OBJECT *)((long)r->rsh_object+(long)r), i=r->rsh_nobs+1;
      --i; ptr++ )
    if( !((ptr->ob_flags) & (1<<13)) )
    {
      *(char *)(&ptr->ob_y) >>= 1;
      if( ((ptr->ob_state) & (X_MAGIC|X_PREFER)) != (X_MAGIC|X_PREFER) )
      {
        *(char *)(&ptr->ob_height) >>= 1;
        if( *(char *)&ptr->ob_type == '\1' )
        {
          y = *(char *)&ptr->ob_y + (*((char *)&ptr->ob_y+1)<<3);
          ptr->ob_height = 0x600;
        }
        else if( *(char *)&ptr->ob_type == '\2' )
        {
          y += 6;
          *(char *)&ptr->ob_y = y&7;
          *((char *)&ptr->ob_y+1) = y>>3;
          ptr->ob_height = 0x600;
        }
      }
    }
/*%  for( bb=(BITBLK *)((long)r->rsh_bitblk+(long)r), i=r->rsh_nbb+1;
      --i; ptr++ ) shorten_bb( bb, r );  be careful: addr's not fixed yet */
}

int rsc_func( int type, int index, void *addr, RSHDR *rsc, int fix )
{
  struct Rsc_addr { long l[2]; } old;
  int ret;
  
  old = *(struct Rsc_addr *)&_GemParBlk.global[5];
  *(long *)&_GemParBlk.global[5] = rsc->rsh_trindex+(long)rsc;
  *(long *)&_GemParBlk.global[7] = (long)rsc;
  ret = fix ? (*_gui.xtern.rsrc_rcfix)(rsc) : rsrc_gaddr( type, index, addr );
  *(struct Rsc_addr *)&_GemParBlk.global[5] = old;
  return ret;
}
int Nrsc_gaddr( int type, int index, void *addr, RSHDR *rsc )
{
  return rsc_func( type, index, addr, rsc, 0 );
}
int Nrsc_rcfix( RSHDR *rsc )
{
  return rsc_func( 0, 0, 0L, rsc, 1 );
}
void Nrsc_free( RSHDR *rsc )
{
  if( rsc ) lfree(rsc);
}
int Nrsc_load( char *name, DTA *dta, RSHDR **out )
{  /* return 0: not found   -1: no memory  -2: bad read
      must use Nrsc_rcfix and eventually Nrsc_free (or lfree)  */
  char temp[120];
  int i, err;
  RSHDR *rsc;

  strcpy( temp, name );
  if( !shel_find( temp ) ) return 0;
  Fsetdta( dta );	/* needed for TOS < 1.4 */
  if( Fsfirst(temp,0x37) || (i=Fopen(temp,0)) < 0 ) return 0;
  if( (rsc = (RSHDR *)lalloc(dta->d_length,-1)) == 0L )
  {
    Fclose(i);
    return -1;
  }
  err = Fread( i, dta->d_length, rsc ) != dta->d_length;
  Fclose(i);
  if( err ) return -2;
  if( graphics->cel_ht==8 ) rsrc_fix8( rsc );
  if( out ) *out = rsc;
  return 1;
}

#pragma warn -par
void set_dclick( int rate )
{
}
#pragma warn +par

void gad_redraw( int w_handle, Rect *r, int obj )
{
  int buf[8];
  OBJECT *o;

  buf[0] = WM_REDRAW;
  buf[2] = 0;
  buf[3] = w_handle;
  objc_offset( o=wtree[w_num], obj, &buf[4], &buf[5] );
  buf[6] = o[obj].ob_width;
  buf[7] = o[obj].ob_height;
  if( rc_intersect( r, (Rect *)&buf[4] ) )
      appl_write( buf[1] = AES_handle, 16, buf );
}

char *help_exe;
void signal_exit(void)
{
  free_help = help_exe;	/* main now frees free_help */
}

static char hlpname[120];

#pragma warn -par
void _x_help( int modal, char *topic, int caps )
{
#ifndef DEMO
  HELPCMD h;
  extern NEO_ACC nac;
  extern MOST *z;
#ifdef DEBUG
  void help_load( HELPCMD *h );
#endif
  
  if( !topic || !topic[0] ) topic = "NeoDesk";
  strcpy( hlpname, z->dflt_path );
  strcat( hlpname, "NEODESK.HLP" );
#ifndef DEBUG
  if( has_Geneva && !modal ) x_help( topic, hlpname, caps );
  else
#endif
  {
    h.nac = &nac;
    h.modal = modal || !z->dial_in_wind || (z->macr_play|z->macr_rec)!=0;
    h.topic = topic;
    h.file = hlpname;
    h.has_Geneva = has_Geneva;
    h.caps = caps;
    h.aes_ver = _GemParBlk.global[0];
    h.AES_handle = AES_handle;
    h.signal_exit = signal_exit;
    if( z->help.wind>0 ) (*z->help.new_topic)( &h );
    else
    {
      bee();
#ifdef DEBUG
      help_load( &h );
#else
      (*nac.mas->exe_load)( 2, "HELP.EXE", &h, 0, &help_exe, 0 );
#endif
      arrow();
    }
  }
#else
  void demo_version(void);
  demo_version();
#endif
}
#pragma warn +par

void arrow_dial( int *msg )
{
  shel_write( SHW_SENDTOAES, 0, 0, (char *)msg, 0L );
}

void load_set( SET_STRUCT *s )
{
#ifdef DEBUG
  int set_start( SET_STRUCT *s );

  set_start(s);
#else
  extern char *set_exe;

  if( (*nac.mas->exe_load)( 2, "SETTINGS.EXE", s, 0, &set_exe, 0 ) <= 0L )
      (*s->signal_xset)();
#endif
}

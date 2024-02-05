#include "string.h"
#include "stdlib.h"
#include "stddef.h"
#include "new_aes.h"
#include "multevnt.h"
#include "xwind.h"
#include "tos.h"
#include "ierrno.h"
#include "lerrno.h"
#include "neodesk.h"
#include "mwclinea.h"
#include "neocommn.h"
#include "settings.h"
#include "neod2_id.h"
#include "ctype.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */
#include "cattgios.h"
#include "kobold.h"
extern GUI *gui;

#pragma warn -par

int start_set( int num );

#define PHYSTOP     (*((long *) 0x42E))
#define _bootdev    (*(int *)0x446)
#define _ramtop		(*(long *)0x5A4)
#define _ramvalid	(*(long *)0x5A8)

/* describe a modeless dialog
typedef struct
{
  unsigned char menu_num;		/* menu string index */
  int treenum;				/* rsc file index */
  FORMFLAGS flags;
  long memory;				/* amount of memory */
  int (*init)( OBJECT *o, struct Form *f );		/* function to initialize it */
  int (*touch)( OBJECT *o, int num, struct Form *f );	/* called when TOUCHEXIT is clicked */
  int (*exit)( OBJECT *o, int num, struct Form *f );	/* called when EXIT is clicked */
  int (*update)( OBJECT *o, struct Form *f );		/* called during timer event */
  OBJECT *tree; 			/* dial's object tree */
  int handle;				/* handle of window containing dial */
  void *mem_ptr;			/* pointer to memory */
  char win_path[50];			/* path of calling window */
  char *old_title;			/* old title */
  int wind;				/* number of associated window */
  void *copy;				/* points to memory of dialog copy */
  SELICON_DESC icons;			/* list of selected icons */
  int apid;				/* apid of owner */
} FORM;  */

FORM *forms;
int num_forms, forms_rem;

FORM_TYPE
    formst[] = { { INSTICON,  INSTALL,	{ 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_insticon, x_insticon, x_insticon, 0L, 0L },	     /* Install icon */
		 { SORTFILT,  FILTER,	{ 1, 0, 0, 0, 0, 0, 1 }, sizeof(FILT_DESC)+sizeof(FILT_TYPE), 
		       i_filter,   t_filter,   x_filter, 0L, 0L },	     /* Filter */
		 { -1,	      FILTER,	{ 1, 0, 0, 0, 0, 0, 0 }, sizeof(FILT_DESC)+sizeof(FILT_TYPE), 
		       i_search,   t_filter,   x_search, 0L, 0L },	     /* Search wind */
		 { -1,	      FILTER,	{ 0, 1, 0, 0, 0, 0, 0 }, sizeof(FILT_DESC)+sizeof(FILT_TYPE), 
		       i_search,   t_filter,   x_search, 0L, 0L },	     /* Search drive */
		 { WIMGRP,    GRPNAME,	{ 1, 0, 0, 0, 0, 0, 1 }, sizeof(GROUP_DESC),
		       i_group,    0L,	       x_group, 0L, 0L },	     /* New group */
		 { ABOUT,     -1,	{ 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_about,    x_null,     x_null, 0L, 0L },	     /* About Neo */
		 { GWIMCHNG,  GRPITEM,	{ 0, 0, 1, 0, 1, 0, 1 }, sizeof(GROUP_ITEM),
		       i_gitem,    0L,	       x_gitem, 0L, 0L },	     /* Group item */
		 { SNAPGRID,  GRID,	{ 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_snap,	   t_snap,     x_snap, 0L, 0L },	     /* Desk Snap */
		 { GWIMSNAP,  GRID,	{ 0, 0, 1, 0, 0, 0, 0 }, 0L,		    
		       i_snap,	   t_snap,     x_snap, 0L, 0L },	     /* Group Snap */
		 { SAVEDESK,  SAVEINF,	{ 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_savecfg,  0L,	       x_savecfg, 0L, 0L },	     /* Save Config */
		 { -1,	      TOSTAKES, { 0, 0, 0, 0, 0, 1, 0 }, 120,		    
		       i_ttp,	   t_ttp,      x_ttp, 0L, 0L }, 	     /* TTP */
		 { CREATE,    NEWFOLD,	{ 1, 0, 0, 0, 0, 0, 1 }, 0L,		    
		       i_folder,   0L,	       x_folder, 0L, 0L },	     /* New folder */
		 { -1,	      DISKINF,	{ 0, 0, 0, 0, 1, 0, 1 }, 0L,		    
		       i_drvinf,   0L,	       x_drvinf, 0L, 0L },	     /* Disk info */
		 { -1,	      FILEINF,	{ 0, 0, 0, 0, 1, 0, 1 }, 0L,		    
		       i_fileinf,  0L,	       x_fileinf, 0L, 0L },	     /* File info */
		 { -1,	      FOLDINF,	{ 0, 0, 0, 0, 1, 0, 1 }, 0L,		    
		       i_foldinf,  0L,	       x_foldinf, 0L, 0L },	     /* Folder info */
		 { -1,	      GRPNAME,	{ 0, 0, 0, 0, 1, 0, 1 }, sizeof(GROUP_HDR), 
		       i_grpinf,   0L,	       x_grpinf, 0L, 0L },	     /* Group info */
		 { -1,	      GRPNAME,	{ 0, 0, 0, 0, 0, 1, 1 }, sizeof(GROUP_HDR), 
		       i_grpinf,   0L,	       x_grpinf, 0L, 0L },	     /* Modal Group info */
		 { INSTAPP,   APPLIC,	{ 0, 1, 0, 0, 0, 0, 0 }, 120+120,	    
		       i_applic,   t_applic,   x_applic, 0L, 0L },	     /* Install desk app */
		 { WIMAPP,    APPLIC,	{ 1, 0, 0, 0, 0, 0, 0 }, 120+120,	    
		       i_applic,   t_applic,   x_applic, 0L, 0L },	     /* Install wind app */
		 { PROGINFO,  EDITNPI,	{ 0, 1, 0, 0, 0, 0, 1 }, 120+120+sizeof(NPI_TYPE),
		       i_npi,	   t_npi,      x_npi, 0L, 0L }, 	     /* Edit desk NPI */
		 { WIMNPI,    EDITNPI,	{ 1, 0, 0, 0, 0, 0, 1 }, 120+120+sizeof(NPI_TYPE),
		       i_npi,	   t_npi,      x_npi, 0L, 0L }, 	     /* Edit wind NPI */
		 { FORMAT,    FLFORMAT, { 0, 1, 0, 0, 0, 0, 0 }, sizeof(DISKOP),    
		       i_format,   t_format,   x_format, 0L, 0L },	     /* Format floppy */
		 { -1,	      DOPSTAT,	{ 0, 1, 0, 0, 0, 0, 1, 0, 1 }, sizeof(DISKOP),	  
		       i_diskop,   t_diskop,   x_diskop, u_diskop, 0L },     /* Disk op stat */
		 { EDITENV,   ENVIRON,	{ 0, 1, 0, 0, 0, 0, 1 }, sizeof(ENV_TYPE),  
		       i_editenv,  t_editenv,  x_editenv, 0L, 0L },	     /* Edit env */
		 { -1,	      ENVIRON,	{ 0, 0, 0, 0, 0, 1, 1 }, sizeof(ENV_TYPE),  
		       i_editenv,  t_editenv,  x_editenv, 0L, 0L },	     /* Edit NPI env */
		 { -1,	      ENVEDIT,	{ 0, 0, 0, 0, 0, 1, 0 }, 0L,		    
		       i_ededit,   0L,	       x_ededit, 0L, 0L },	     /* Env sub-edit */
		 { -1,	      FILENAME, { 0, 0, 0, 0, 1, 0, 1 }, sizeof(RENAME),    
		       i_rename,   0L,	       x_rename, 0L, 0L },	     /* Rename file/fold */
		 { PRINTDIR,  DIRPRN,	{ 1, 0, 0, 0, 1, 0, 0 }, 0L,		    
		       i_prndir,   0L,	       x_prndir, 0L, 0L },	     /* Print dir */
		 { -1,	      COPYMODE, { 0, 1, 0, 0, 0, 0, 0 }, sizeof(DISKOP),    
		       i_copyall,  t_copyall,  x_copyall, 0L, 0L },	     /* Diskcopy */
		 { -1,	      FILECONF, { 0, 0, 0, 0, 0, 0, 0 }, 120+120+2*sizeof(DTA)+1, 
		       i_nameconfl, 0L,        x_nameconfl, 0L, 0L },	     /* Name conflict */
		 { -1,	      FILEOP,	{ 0, 0, 0, 0, 0, 0, 0 }, sizeof(TREE)+30+120+3*29+120+2,
		       i_fileop,   t_fileop,   x_fileop, u_fileop, 0L },     /* File copy/del */
		 { -1,	      FILTER,	{ 0, 0, 0, 0, 0, 1, 1 }, sizeof(FILT_DESC)+sizeof(FILT_TYPE), 
		       i_filefilt, t_filter,   x_filefilt, 0L, 0L },	     /* Filter */
		 { CHANGREZ,  TTREZ,	{ 0, 1, 0, 0, 0, 0, 1 }, 0L,
		       i_newrez,   t_newrez,   x_newrez, 0L, 0L },	     /* Change rez */
		 { -1,	      FOLDCONF, { 0, 0, 0, 0, 0, 1, 0 }, 0L, 
		       i_foldconfl, 0L,        x_foldconfl, 0L, 0L },	     /* Folder conflict */
		 { -1,	      MACRODEF, { 0, 0, 0, 0, 0, 1, 0 }, 0L, 
		       i_macro,    t_macro,    x_macro, 0L, 0L },	     /* Macro */
		 { -1,	      NEWNAME,  { 0, 0, 0, 0, 0, 1, 0 }, 120L, 
		       i_newname,  0L,         x_newname, 0L, 0L },	     /* New name on copy */
		 { -1,	      FYI,      { 0, 1, 0, 0, 0, 0, 1, 1 }, 4+36*3+20,
		       i_opfyi,    0L,         x_fyi, u_fyi, 0L },	     /* FYI for queued items */
/*		 { -1,	      FYI,      { 0, 1, 0, 0, 0, 0, 1, 1, 0, 1 }, 4+36*3+20,
		       i_helpfyi,  0L,         x_fyi, 0L, 0L },	             /* FYI for Help */ */
		 { MPOPMISC,  MOREPREF, { 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_more,	   t_more,     x_more, 0L, 0L },	     /* Misc. Pref */
		 { MPOPFILE,  PREFER,	{ 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_filepref, 0L,	       x_filepref, 0L, 0L },	     /* File Pref */
		 { MPOPACC,   ACCLIST,	{ 0, 1, 0, 0, 0, 0, 0 }, MAX_NEO_ACC*9,     
		       i_accpref,  0L,	       x_accpref, 0L, 0L },	     /* ACC Pref */
		 { MPOPINF,   INFLIST,	{ 0, 1, 0, 0, 0, 0, 0 }, 11*9,		    
		       i_infpref,  0L,	       x_infpref, 0L, 0L },	     /* INF Pref */
		 { MPOPEXT,   EXTENS,	{ 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_extpref,  t_extpref,  x_extpref, 0L, 0L },	     /* Extens Pref */
		 { MPOPPATH,  PREFPATH, { 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_pthpref,  0L,	       x_pthpref, 0L, 0L },	     /* Paths Pref */
		 { MPOPMEM,   MEMORY,	{ 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_memory,   0L,	       x_memory, u_memory, 0L },     /* Memory */
		 { MPOPDIAL,  DIALPREF, { 0, 1, 0, 0, 0, 0, 0 }, 0L,		    
		       i_dialpref, 0L,	       x_dialpref, 0L, 0L },	     /* Dial Pref */
		 { MPOPDESK,  DESKPREF, { 0, 1, 0, 0, 0, 0, 0 }, 121,		    
		       i_deskpr,   t_deskpr,   x_deskpr, 0L, 0L },	     /* Desk Pref */
		 { MPOPWIND,  WINPREF,	{ 0, 1, 0, 0, 0, 0, 0 }, 2+2+sizeof(WIND_PRF)+4*sizeof(WIND_FONT), 
		       i_windpr,   t_windpr,   x_windpr, 0L, 0L },	     /* Window Pref */
		 { -1,	      NOTEPREF, { 0, 0, 0, 0, 0, 1, 0 }, 2+2+sizeof(WIND_PRF)+4*sizeof(WIND_FONT), 
		       i_notepr,   t_notepr,   x_notepr, 0L, 0L },	     /* Note Pref */
		 { -1,	      PICOPTS,  { 0, 0, 0, 0, 0, 1, 0 }, 0L,
		       i_picopt,   0L,         x_picopt, 0L, 0L },	     /* Picture options */
		 { 0, 0, {0,0,0,0,0,0,0,0}, 0L, 0L, 0L } };		       /* end of list */

int flev, ignore_parent=-1;
Rect form_rect[4], old_frect;
char blit_ok[4];
char pos_err;
int delta[4];
char filt_path[120], copy_slice=-1, *free_help, *free_set;
char *env_parent, *env_edit, *env_ptr, *kobold_buf;
DTA *nameconf_sdta, *nameconf_ddta;
extern MOST *z;
extern MASTER *mas;
extern TREE maintree;
extern int w_num, w_handle, d_active, dum, wxref[], w_active, num_w_active, AES_handle,
    moving;
extern OBJECT *icons, *form, *menu, *popups;
extern char *msg_ptr[], re_name, glob[], slash[], diskbuff[], tmpf[],
    ext[3][5], tmpf[], volname[21], ifmt[], lfmt[], nfmt[], Nfmt[],
    ver_gt_12, crlf[], colon_slash[];
extern int i_per_row[7], in_wind[7][2], max_itm[7][2];
extern Rect ww[7][2];
extern unsigned int witems, items[7];
extern FSTRUCT *wfile;
extern GROUP_ITEM *group_start[7];
extern GROUP_DESC *group_desc[7];
extern NPI_DESC *last_npi;
extern GRAPHICS *graphics;
extern int char_w, char_h, bar_h;
extern char showinf_path[120], in_showinf, showinf_ok, showinf_update, showinf_all,
    *show_path, first_no_close, *c_buf, *c_curbuf, *last_buf, iconedit, is_clip,
    *nameconf_old, *nameconf_new, filt_templ, falc_vid, aes_ge_40, TT_vid, aes_ge_20,
    jog_background, has_Geneva, cnover_mode;
extern int show_ret, show_parm1, *show_date, show_wind, nameconf_ret, form_handle, Geneva_ver,
    inall_vol;
extern long show_size;
extern char show_name[35];
extern GROUP_HDR *show_grp;
extern int (*show_func)( GROUP_HDR *gh );
extern SELICON_DESC showinf_icon;
extern unsigned char fdc_level;
extern PRN_PARAM prn_param;
extern long c_buflen, c_bufmax;
extern unsigned char *copy_q, copy_ok;  /* remember: copy_q can move! */
extern ICONBUF *nic_icons;
extern struct Max_icon
{
  int text_w, data_w, h;
} max_icon;
extern unsigned char *mac;
extern int ignore_events;
extern LoadCookie *lc;

/********************* Dialog manager routines *********************/
SEL_ICON *get_sel_icon( SELICON_DESC *f, int reselect )
{	/* remember to cmfree( &f->icons ) when done with list */
	/* reselect=-1 for no unselect when done */
  int i, j, count;
  FSTRUCT *fs;
  SEL_ICON *s;

  if( !f->icons )
  {
    mac_play_icons();
    count = 0;
    if( w_active>=0 )
    {
      w_handle = wxref[w_num = num_w_active];
      set_wfile();
      for( fs=wfile, i=witems; --i>=0; fs++ )
	if( fs->state ) count++;
    }
    else if( d_active>=0 )
    {
      for( i=z->num_icons; --i>=0; )
	if( z->idat[i].state&1 ) count++;
    }
    if( (f->nicons=count) == 0 ) return 0L;
    if( (s = f->icons = lalloc(count*sizeof(SEL_ICON),w_num)) == 0 ) return 0L;
    if( w_active>=0 )
    {
      for( fs=wfile, i=witems; --i>=0; fs++ )
	if( fs->state )
	{
	  s->wnum = w_num;
	  s->u.fs = fs;
	  s++;
	}
    }
    else
    {
      w_num = -1;
      for( i=0; i<z->num_icons; i++ )
	if( z->idat[i].state&1 )
	{
	  s->wnum = -1;
	  s->u.desk_item = i+1;
	  s++;
	}
    }
    f->cicon = 0;
    if( count && !reselect )
    {
      de_act( -1, -1 );
      info();
    }
    if( count ) mac_rec_icons(f);
  }
  if( (Getshift()&3)==3 || f->cicon >= f->nicons )
  {
    if( reselect >= 0 ) de_act( -1, -1 );
    info();
    return 0L;
  }
  s = &f->icons[f->cicon++];
  if( reselect )
  {
    if( s->wnum >= 0 )
    {
      de_act_d(-1);
      de_act_w( i = s->u.fs - z->file[s->wnum], j = s->wnum );
      select_w( i, SELECTED, wxref[j], 1 );
    }
    else
    {
      de_act_w( -1, -1 );
      de_act_d( i = s->u.desk_item );
      select_d( i, SELECTED );
    }
    info();
  }
  return s;
}
int set_form_path( FORM *f )
{
  OBJECT *o = f->tree;

  if( *(char *)&o[1].ob_type == X_MOVER && !f->flags.parent_closed )
  {
    short_path( 0L, o[2].ob_spec.free_string=f->win_path, o[2].ob_width/6,
        sizeof(forms->win_path) );
    return 1;
  }
  return 0;
}
void dial_pos( FORM *f, int sign )
{
  long *tr, *m;

  if( f->apid==AES_handle && f->treenum>NO_POS )
  {  
    tr = (long *)&f->tree[0].ob_x;
    m = (long *)&z->dialogs[f->treenum>0 ? f->treenum-FILTER : MAX_DIAL-FILTER+1][0];
    if( sign<0 ) *tr = *m;
    else *m = *tr;
  }
}
void form_y( FORM *f, int sign )
{
  int i, h;
  OBJECT *o = f->tree;

  dial_pos( f, sign );
  if( (o[1].ob_type>>8) != X_MOVER ) return;
  if( f->wind<0 )	/* on desktop */
  {
    h = o[2].ob_height*sign;  /* get title height */
    for( i=o[0].ob_head; i; i = o[i].ob_next )
      o[i].ob_y += h;
    o[0].ob_height += h;
    if( sign<0 ) f->old_title = o[2].ob_spec.free_string;
    else o[2].ob_spec.free_string = f->old_title;
    if( hide_if( o, 2, sign>0 ) ) o[0].ob_state |= X_PREFER;
    else o[0].ob_state &= ~X_PREFER;
    o[0].ob_spec.obspec.framesize = sign<0 ? 0 : -2;
    hide_if( o, 1, sign>0 );
  }
  else if( sign<0 )
  {
    o[2].ob_state = X_MAGIC|X_SMALLTEXT;
    o[2].ob_type = G_STRING;
    o[2].ob_flags &= ~(FL3DMASK|HIDETREE); /* show the "title" */
    f->old_title = o[2].ob_spec.free_string;
    set_form_path(f);
    o[0].ob_spec.obspec.framesize = 0;
    o[0].ob_state &= ~X_PREFER;
    o[1].ob_flags |= HIDETREE;
  }
  else
  {
    o[0].ob_spec.obspec.framesize = -2;
    o[0].ob_state |= X_PREFER;
    o[1].ob_flags &= ~HIDETREE;
    o[2].ob_flags &= ~HIDETREE;
    o[2].ob_state = 0;
    o[2].ob_type = (X_UNDERLINE<<8)|G_BUTTON;
    o[2].ob_spec.free_string = f->old_title;
    o[2].ob_flags |= FL3DIND;
  }
}

void free_dial( FORM *f )
{
  long *l, *m;
  
  for( m=f->copy; (l=m)!=0; )
  {
    m = *(long **)l;
    lfree(l);
  }
  f->copy = 0L;
}

void free_form( FORM *f )
{
  int i;
  
  i = f-forms;
  if( !--num_forms ) cmfree( (char **)&forms );
  else
  {
    memcpy( f, f+1, (num_forms-i)*sizeof(FORM) );
    forms_rem++;
  }
}

void modal_finish( FORM **f )
{
  flev--;
  blit_form( *f, 1 );
  wind_lock(0);
}
/* use a form taking into account dynamic movement of an existing form */
int form_proc( int func( OBJECT *o, int num, FORM *f ), int num, FORM **f )
{
  int ret, form;
  
  form = *f-forms;
  ret = (*func)( (*f)->tree, num, *f );
  *f = forms+form;
  return ret;
}
int _use_form( FORM **f, int num )
{
  int but, ret=0, i, is_help;
  Rect r;

  if( num==-1 )
  {
    if( (*f)->exit ) (*(*f)->exit)( (*f)->tree, -1, *f );
/*%    close_fwind( *f, 0 ); */
    return 1;
  }
  but = num&0x7FFF;	    /* treat double-clicks as singles */
  if( (*f)->tree[but].ob_flags & TOUCHEXIT )
  {
    if( (*f)->touch ) ret = form_proc( (*f)->touch, (*f)->flags.double_click ? num : but, f );
  }
  else if( (*f)->tree[but].ob_flags&EXIT )
  {
    is_help = *(char *)&((*f)->tree[but].ob_type) == X_HELP && (*f)->apid==AES_handle;
    if( !is_help && (*f)->exit )
    {
      ret = form_proc( (*f)->exit, num, f );
      if( (*f)->handle>0 && !((*f)->tree[but].ob_flags & (1<<11)) &&
          Getshift()&3 && (*f)->treenum!=COPYMODE/*003*/ ) ret = 0;
    }
    /* reset the object */
    i = (*f)->tree[but].ob_state & ~SELECTED;
    if( !ret )
      if( (*f)->handle>0 ) x_wdial_change( (*f)->handle, but, i );
      else objc_change( (*f)->tree, but, 0, 0, 0, 0, 0, i, 1 );
    else (*f)->tree[but].ob_state = i;
    /* process Help after button reset */
    if( is_help )
    {
/*      i = 0;
      if( (*f)->handle>0 ) i = 1;
      else if( !(*f)->flags.modal )
      {
        modal_finish( f );
        old_frect = form_rect[flev];
        if( start_form( HFYI_FORM )<0 )
        {
          ignore_parent = *f-forms;
          i = 1;
        }
      } */
      if( (*f)->handle<=0 )
      {
        flev--;
        r = form_rect[flev];
        blit_form( *f, 1 );
      }
      i = *f - forms;
      if( (*f)->handle<=0 ) _x_help( 1, (*f)->tree[2].ob_spec.free_string, 1 );
      else _x_help( 0, (*f)->old_title, 1 );
      *f = i + forms;	/* maybe location of list has changed */
      if( (*f)->handle<=0 )
      {
        form_rect[flev] = r;
        blit_form( *f, 0 );
        objc_draw( (*f)->tree, 0, 8, Xrect(form_rect[flev]) );
        flev++;
      }
    }
  }
  return ret;
}
int modal_formdo( FORM **f )
{
  int ret;
  
  arrow();
  wind_lock(1);
  blit_form( *f, 0 );
  hide_if( (*f)->tree, 1, blit_ok[flev] && !pos_err );	/* mover */
  objc_draw( (*f)->tree, 0, 8, Xrect(form_rect[flev]) );
  flev++;
  for(;;)
  {
    if( (*f)->update && !form_proc( (int (*)( OBJECT *o, int num, FORM *f ))((*f)->update),
        0, f ) ) break;
    if( _use_form( f, ret=form_do( (*f)->tree, 0 ) ) ) break;
    if( ignore_parent >= 0 ) return 0;
  }
  modal_finish(f);
  dial_pos( (*f), 0 );
  close_fwind( (*f), 0, 1 );
  return ret;
}
void modeless_active( FORM *f, int active )
{
  FORM *f2;
  int i;
  
  for( f2=forms, i=num_forms; --i>=0; f2++ )
    if( f2->handle>0 && f2!=f )
    {
      wind_get( f2->handle, X_WF_DIALFLGS, &dum );
      wind_set( f2->handle, X_WF_DIALFLGS, active ? dum|X_WD_ACTIVE : dum&~X_WD_ACTIVE );
    }
}
void close_fwind( FORM *f, int ac_close, int recover_ok )
{
  int num = f-forms;
  
  if( num==ignore_parent ) ignore_parent = -1;
  if( f->handle>0 )
  {
    if( f->handle==ignore_events )
    {
      modeless_active( f, 1 );
      ignore_events = -1;
    }
    else recover_ok = 0;
    lock_drive( 0, f->handle, 0 );
    if( f->flags.locked )
    {
      f->flags.locked = 0;
      wind_lock(0);
    }
    if( !ac_close )
    {
      if( f->flags.opened ) wind_close(f->handle);
      wind_delete(f->handle);
    }
    if( f->tree ) form_y( f, 1 );
    f->handle = 0;
  }
  else recover_ok = 0;
  if( f->mem_ptr ) cmfree( (char **)&f->mem_ptr );
  if( f->icons.icons ) cmfree( (char **)&f->icons.icons );
  free_dial(f);
  free_form(f);
  if( recover_ok && ignore_parent>=0 )
  {
    f = forms+ignore_parent;
    form_rect[flev] = old_frect;
    modal_formdo( &f );
  }
}
void free_show(void)
{
  if( (long)showinf_icon.icons == -1L ) showinf_icon.icons = 0L;
  else cmfree( (char **)&showinf_icon.icons );
  in_showinf = 0;
}
void close_all_fwind( int apid, int ac_close )
{
  FORM *f;
  int i;

  /* must go in reverse order because num_forms changes */
  for( i=num_forms, f=forms+i-1; --i>=0; f-- )
    if( apid<0 || f->apid==apid )
    {
      if( f->exit ) (*f->exit)( f->tree, -1, f );
      close_fwind( f, ac_close, 0 );
    }
  if( apid==AES_handle )
  {
    free_show();
    ignore_events = -1;
  }
}
void close_wforms( int num, int update )
{
  FORM *f;
  int i;

  /* must go in reverse order because num_forms changes */
  for( i=num_forms, f=forms+i-1; --i>=0; f-- )
    if( f->wind==num )
      if( !update || f->flags.close_on_update )
      {
        if( f->exit ) (*f->exit)( f->tree, -1, f );
        close_fwind( f, 0, 0 );
      }
      else f->flags.parent_closed = 1;
  if( num==show_wind ) free_show();
}
void close_form_reord( int desk, int menu )  /* only when entering reorder mode */
{
  FORM *f;
  int i;

  /* must go in reverse order because num_forms changes */
  for( i=num_forms, f=forms+i-1; --i>=0; f-- )
    if( f->menu_num==menu && (desk && f->flags.dmenu ||
	!desk && f->flags.wmenu) )
    {
      if( f->exit ) (*f->exit)( f->tree, -1, f );
      close_fwind( f, 0, 0 );
    }
  if( in_showinf ) free_show();
}
void _form_path( FORM *f, int num )
{
  int buf[8];

  buf[0] = WM_REDRAW;
  buf[2] = 0;
  buf[3] = f->handle;
  *(long *)&buf[4] = *(long *)&f->tree[num].ob_x + *(long *)&f->tree[0].ob_x;
  *(long *)&buf[6] = *(long *)&f->tree[num].ob_width;
  appl_write( buf[1] = AES_handle, 16, buf );
}
void form_path( int num )
{
  FORM *f;
  int i;

  for( f=forms, i=num_forms; --i>=0; f++ )
    if( f->handle>0 && f->wind==num && set_form_path(f) )
        _form_path( f, 2 );
}

/* process input from the user to a modeless dialog */
void use_form( int hand, int num )
{
  FORM *f;
  int i;

  if( !hand ) return;
  for( f=forms, i=num_forms; --i>=0; f++ )
    if( f->handle == hand )
    {
      if( _use_form( &f, num ) ) close_fwind( f, 0, 1 );
      return;
    }
}

int update_forms( int use )
{
  int count=0, i;
  FORM *f;

  for( i=num_forms; --i>=0; )
  {
    f = forms+i;	/* forms can change during update! */
    if( f->handle > 0 && f->update && (ignore_events<=0 || f->handle==ignore_events) )
    {
      if( use ) (*f->update)( f->tree, f );
      count++;
    }
  }
  if( free_help )
  {
    Mfree(free_help);
    free_help = 0L;
  }
  if( free_set )
  {
    Mfree(free_set);
    free_set = 0L;
  }
  return count;
}

/* calculate a window's border based on the size of an object tree */
void calc_bord( long type, OBJECT *tree, Rect *g )
{
/*%  int i; */

/*%  if( tree[0].ob_y < (i=bar_h<<1) ) tree[0].ob_y = i; */
  x_wind_calc( WC_BORDER, type, type>>16L, tree[0].ob_x, tree[0].ob_y,
      tree[0].ob_width, tree[0].ob_height, &g->x, &g->y, &g->w, &g->h );
  if( g->y < bar_h )
  {
    tree[0].ob_y += bar_h-g->y;
    g->y = bar_h;
  }
}
void form_pos( FORM *f, int chop )
{
  int i, x, y, delta, mx, my, dum;
  OBJECT *form = f->tree;

  x = form[0].ob_x;
  y = form[0].ob_y;
  x_form_center( form, &form_rect[flev].x, &form_rect[flev].y,
      &form_rect[flev].w, &form_rect[flev].h );
  delta = form[0].ob_x - form_rect[flev].x;
  if( !f->flags.no_center ) switch( z->dial_mode )
  {
    case 1:	/* follow mouse */
      graf_mkstate( &mx, &my, &dum, &dum );
      x = mx - (form[0].ob_width>>1);
      y = my - (form[0].ob_height>>1);
      break;
    case 2:	/* center */
      x = form[0].ob_x;
      y = form[0].ob_y;
  }
  form_rect[flev].x = (form[0].ob_x = x) - delta;
  form_rect[flev].y = (form[0].ob_y = y) - delta;
/*%  form_width = form_rect[flev].w;
  form_height = form_rect[flev].h; */
  x = y = 0;
  if( form_rect[flev].x+form_rect[flev].w > (i=z->maximum.x+z->maximum.w) )
  {
    form_rect[flev].x = i - form_rect[flev].w;
    x++;
  }
  if( form_rect[flev].y+form_rect[flev].h > (i=z->maximum.y+z->maximum.h) )
  {
    form_rect[flev].y = i - form_rect[flev].h;
    y++;
  }
  if( form_rect[flev].x < z->maximum.x )
  {
    form_rect[flev].x = z->maximum.x;
    x++;
  }
  if( form_rect[flev].y < z->maximum.y )
  {
    form_rect[flev].y = z->maximum.y;
    y++;
  }
  form[0].ob_x = form_rect[flev].x + delta;
  form[0].ob_y = form_rect[flev].y + delta;
  if( chop )
  {
    rc_intersect( &z->maximum, &form_rect[flev] );
    pos_err = x==2 || y==2;  /* changed twice, so dial is too big for scrn */
  }
}

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

int find_max( OBJECT *o, int num )
{
  if( num > dum ) dum = num;
  return 1;
}

static long *copy_ptr;
static FORM *dial_form;

int dial_alloc( void **v, int size )
{
  void *out;
  
  if( (out = lalloc( size+4, dial_form->wind )) == 0 )
  {
    free_dial( dial_form );
    return 0;
  }
  memcpy( (char *)out+4, *v, size );
  *(long *)out = 0L;
  *v = (char *)out+4;
  *copy_ptr = (long)out;
  copy_ptr = (long *)out;
  return 1;
}

int dial_copy( OBJECT *o, int num )
{
  TEDINFO *ted;
  
  if( !dum ) return 0;
  o += num;
  switch( (char)o->ob_type )
  {
    case G_FBOXTEXT:
    case G_FTEXT:
      if( !dial_alloc( (void **)&o->ob_spec.tedinfo, sizeof(TEDINFO) ) ||
	  !dial_alloc( (void **)&o->ob_spec.tedinfo->te_ptext, o->ob_spec.tedinfo->te_txtlen ) )
	  return dum=0;
  }
  return 1;
}

FORM *add_form( FORM *old )
{
  FORM *new, *f;
  int i;
  
  f = forms;
  if( !add_thing( (void **)&forms, &num_forms, &forms_rem, old, 1, sizeof(FORM), -1 ) ) return 0L;
  if( f != forms )	/* forms moved, fix win_paths */
    for( f=forms, i=num_forms-1; --i>=0; f++ )
      if( (f->tree[1].ob_type>>8) == X_MOVER && f->handle>0 )	/* was f->wind>=0 */
          f->tree[2].ob_spec.free_string = f->win_path;
  new = forms+num_forms-1;
  memclr( (FORM_TYPE *)new + 1, sizeof(FORM)-sizeof(FORM_TYPE) );
  return new;
}

FORM *duplic_form( FORM *old, FORM_TYPE *ft, int wnum )
{
  int w;
  FORM *new;

  w = old-forms;	/* get old form index */  
  if( (new=add_form(old)) == 0L ) return 0L;
  old = forms+w;	/* recalculate old form because it may have moved */
  memcpy( new, ft, sizeof(FORM_TYPE) );
  new->ft = ft;
  new->wind = wnum;
  dum = 0;	/* used in find_max */
  map_tree( old->tree, 0, -1, find_max );
  if( ((void *)copy_ptr = new->copy = lalloc(++dum*sizeof(OBJECT)+4L,new->wind)) == 0 ) return 0L;
  *copy_ptr = 0L;
  w = w_num;
  w_num = old->wind;	/* so that short_path() will work right */
  form_y( old, 1 );
  memcpy( new->tree=(OBJECT *)(copy_ptr+1), old->tree, dum*sizeof(OBJECT) );
  form_y( old, -1 );
  w_num = w;
  dum = 1;	/* used in dial_copy */
  dial_form = new;
  map_tree( new->tree, 0, -1, dial_copy );
  return !dum ? 0L : new;
}

FORM *already( FORM_TYPE *ft, int wnum, int apid )
{
  FORM *f, *f2, *old;
  int i, j;

  for( old=0L, f=forms, i=num_forms; --i>=0; f++ )
    if( f->ft==ft )
    {
      old = f;
      if( old->handle>0 && old->wind==wnum && !old->flags.multiple )
      {
        wind_set( old->handle, WF_TOP );
        return 0L;
      }
      break;
    }
  for( f=forms, i=num_forms; --i>=0; f++ )
      if( f->handle>0 && f->treenum==ft->treenum && f->apid==apid )
      {
        for( f2=f, j=i; j-- >= 0; f2++ )
  	  if( f->handle>0 && f2->treenum==ft->treenum && f->apid==apid )
	    if( f2==old && !f2->flags.copyable )
	    {
	      f_alert1( msg_ptr[92] );
	      wind_set( old->handle, WF_TOP );
	      return 0L;
	    }
	    else if( f2->wind==wnum && f2->menu_num==ft->menu_num &&
	        !f2->flags.multiple )
	    {
	      wind_set( f2->handle, WF_TOP );
	      return 0L;
	    }
        return duplic_form( f, ft, wnum );
      }
  if( (f = add_form((FORM *)ft)) != 0 )
  {
    f->wind = wnum;
    f->ft = ft;
    return f;
  }
  return 0L;
}

void blit_form( FORM *f, int unblit )
{
  OBJECT *o = f->tree;
  
  if( !unblit )
  {
    delta[flev] = o[0].ob_x - form_rect[flev].x;
    if( f->flags.no_blit || form_rect[flev].x<0 || form_rect[flev].w>z->maximum.w )  /* 002 */
        blit_ok[flev] = 0;
    else blit_ok[flev] = form_dial( X_FMD_START,  0, 0, 0, 0,  Xrect(form_rect[flev]) );
  }
  else
  {
    form_rect[flev].x = o[0].ob_x - delta[flev];
    form_rect[flev].y = o[0].ob_y - delta[flev];
    form_dial( blit_ok[flev] ? X_FMD_FINISH : FMD_FINISH,  0, 0, 0, 0,
	Xrect(form_rect[flev]) );
  }
}

int start_ext_form( int apid, long type, FORM_TYPE *ft )	/* start external (NPG) form */
{
  return start_fform( ft, apid, type );
}

#define FORM_WIND NAME|MOVER|CLOSER

int start_form( int num )
{
  return start_fform( &formst[num], AES_handle, FORM_WIND );
}

/* start a form taking into account dynamic movement of an existing form */
int start_sform( int num, FORM **f )
{
  int ret, fnum;
  
  fnum = *f - forms;
  ret = start_fform( &formst[num], AES_handle, FORM_WIND );
  *f = forms+fnum;
  return ret;
}

int start_fform( FORM_TYPE *ft, int apid, long type )
{ /* ret: -1=opened window  0=failed  other=modal return */
  int dum, hand, delta, ret=0;
  FORM *f;

  if( (f = already(ft,ft->flags.dmenu ? -1 : w_num, apid)) != 0L )	   /* window is not already open */
  {
    f->apid = apid;
    f->flags.opened = 0;
    if( f->memory )
    {
      if( (f->mem_ptr=lalloc(f->memory,-1)) == 0 )
      {
        free_form(f);
        return 0;
      }
      memclr( f->mem_ptr, f->memory );
    }
    if( flev || ((f->flags.modal || !z->dial_in_wind || apid==AES_handle &&
        (z->macr_play|z->macr_rec)!=0) && !f->flags.force_modeless) )
        f->handle = hand = 0;
    else f->handle = hand = x_wind_create( type, type>>16L, Xrect(z->maximum) );
    if( !f->tree )			/* dialog not used before */
    {
      if( f->treenum<0 )		/* special case */
      {
	if( !(*f->init)( f->tree, f ) )
	{
	  close_fwind( f, 0, 1 );
	  return 0;
	}
      }
      else lrsrc_gaddr( 0, f->treenum, &f->tree );
      ft->tree = f->tree;
      if( f->handle>0 ) form_y( f, -1 );
      else dial_pos( f, -1 );
      form_pos( f, f->handle<=0 );
      if( f->handle>0 )
      {
	calc_bord( type, f->tree, &form_rect[flev] ); /* fit a window around it */
	/* and reposition the dialog at this location */
	x_wind_calc( WC_WORK, type, type>>16L, Xrect(form_rect[flev]),
	    &f->tree[0].ob_x, &f->tree[0].ob_y, &dum, &dum );
      }
    }
    else if( f->handle>0 )
    {
      form_y( f, -1 );
      form_pos( f, 0 );
      calc_bord( type, f->tree, &form_rect[flev] );
    }
    else
    {
      dial_pos( f, -1 );
      form_pos( f, 1 );
    }
    if( (*f->init)( f->tree, f ) )	 /* initialize the dialog */
      if( hand>0 )
      {
        if( f->flags.ignore_events && ignore_events<=0 )
        {
          modeless_active( f, 0 );
          ignore_events = f->handle;
        }
	wind_set( hand, WF_CURRXYWH, Xrect(form_rect[flev]) );
	/* tell Geneva it's a dialog in a window */
	wind_set( hand, X_WF_DIALOG, f->tree );
	/* set the name according to the text in the hidden title object */
	wind_set( hand, WF_NAME, f->old_title );
	wind_open( hand, Xrect(form_rect[flev]) );
	f->flags.opened = 1;
	ret = -1;
      }
      else ret = modal_formdo( &f );
    else close_fwind( f, 0, 1 );
  }
  return ret;
}

int find_parent( OBJECT *tree, int parent )
{
  int lastobj;

  while( tree[parent=tree[lastobj=parent].ob_next].ob_tail != lastobj &&
      parent>=0 && lastobj );
  return(parent);
}
void form_draw( FORM *f, int num, int edit )
{  /* edit is fallback if current cursor line is now hidden */
  int ind, dum, par;

  if( f->handle>0 )
  {
    if( edit )
    {
      wind_get( f->handle, X_WF_DIALEDIT, &ind, &dum );
      wind_set( f->handle, X_WF_DIALEDIT, 0, 0 );
    }
    x_wdial_draw( f->handle, num, 8 );
    if( ind && edit )
    {
      if( f->tree[ind].ob_flags&(HIDETREE|DISABLED) ||
	  f->tree[find_parent(f->tree,ind)].ob_flags&(HIDETREE|DISABLED) ) ind = edit;
      wind_set( f->handle, X_WF_DIALEDIT, ind, -1 );
    }
  }
  else objc_draw( f->tree, num, 8, 0, 0, 0, 0 );
}
void set_longedit( OBJECT *o, int ind, int count )
{
  if( Geneva_ver >= 0x104 )
  {
    o += ind;
    while( --count > 0 )
      (o++)->ob_spec.tedinfo->te_tmplen = X_LONGEDIT;
  }
}
/********************************************************************/
SEL_ICON *inst_icon;
int cdecl draw_inst( PARMBLK *pb )
{
  register NICONBLK *i_ptr;
  register int i, j;

  j = pb->pb_obj-INFLOPPY;
  set_clp_rect( (Rect *)&pb->pb_xc, 1 );
  draw_icon( &nic_icons[j].nicb, pb->pb_x, pb->pb_y,
      icons[j+1].ob_spec.iconblk->ib_ptext, 0, 0, 72 );
  set_clp_rect( &z->maximum, 1 );
  return(0);
}
int i_insticon( OBJECT *form, FORM *f )
{
  int i, j, k;
  char *str;
  OBJECT *o;
  ICONSAVE *is;
  static USERBLK pb = { &draw_inst };

  for( i=INTRASH-INFLOPPY+1, o=&form[INFLOPPY]; --i>=0; o++ )
  {
    o->ob_type = G_USERDEF;
    o->ob_spec.userblk = &pb;
    o->ob_width = 74;
    o->ob_height = ICON_H+8;
  }
  if( (inst_icon=get_sel_icon( &f->icons, 1 )) != 0L && inst_icon->wnum>=0 )
  {
    cmfree( (char **)&f->icons );
    inst_icon = 0L;
  }
  if( inst_icon )
  {
    str = (is=&z->idat[inst_icon->u.desk_item-1])->label;
    j = is->c;
    k = 1;
    obj_true1( form, is->state&4, INVOL );	/* 004 */
    hide_if( form, INVOL, is->type<CLIPBRD );	/* 004 */
  }
  else
  {
    j = k = 0;
    str = "";
    hide_if( form, INVOL, 1 );	/* 004 */
  }
  obj_enab( form, k, 2, INCHANGE, INREMOVE );
  form[INLETTER].ob_spec.tedinfo->te_ptext[0] = j;
  strcpy( form[INSTRING].ob_spec.tedinfo->te_ptext, str );
  return 1;
}

void prev_blit( FORM *f, int unblit )
{
  flev--;
  blit_form( f, unblit );
  flev++;
}

void modal_unblit( FORM *f, int unblit, int redraw )	/* 003 */
{
  if( f->handle<=0 )
  {
    prev_blit( f, unblit );
    if( !unblit && redraw ) form_draw( f, 0, 0 );
  }
}

int get_icvol( OBJECT *form, ICONSAVE *is )	/* 004 */
{
  if( form[INVOL].ob_state&SELECTED ) return is->state|=4;
  is->state &= ~4;
  return 0;
}

int x_insticon( OBJECT *form, int foo, FORM *f )
{
  int i, k, l, m, ret=1;
  ICONSAVE *is;

  if( foo<0 ) return 1;
  is = &z->idat[i = inst_icon ? inst_icon->u.desk_item-1 : 0];
  k = form[INLETTER].ob_spec.tedinfo->te_ptext[0];
  if( foo!=INCANC ) modal_unblit( f, 1, 0 );
  switch(foo)
  {
    case INALL:
      inall_vol = form[INVOL].ob_state&SELECTED ? 4 : 0;	/* 004 */
      install_devices(0);
      if( inall_vol )					/* 004 */
      {
        inall_vol = 0;
        icon_volnames();
      }
      break;
    case INREMOVE:
      remove_icon(i);
      break;
    case INCHANGE:
    case INFLOPPY:
    case INHARD:
    case INRAM:
      if( foo!=INCHANGE || is->type<CLIPBRD )
      {
	if( k>='a' && k<='z' ) k &= 0x5F;
	else if( !k )
	{
	  f_alert1( msg_ptr[30] );
	  ret = 0;
	  break;
	}
	else if( k<'A' || k>'Z' )
	{
	  f_alert1( msg_ptr[31] );
	  ret = 0;
	  break;
	}
	if( foo == INCHANGE ) get_icvol( form, is );
      }
      if( foo == INCHANGE )
      {
	strcpy( is->label, form[INSTRING].ob_spec.tedinfo->te_ptext );
	is->c = k;
        if( is->state & 4 )	/* 004 */
        {
          get_volname( &is->c, 0 );
          arrow();
        }
	break;
      }
    case INCLIP:
    case INPRINT:
    case INTRASH:
      if( (i=add_desk()) < 0 )
      {
	f_alert1( msg_ptr[32] );
	foo = INCANC;
	break;
      }
      get_max_icon(-1);
      graf_mkstate( &l, &m, &dum, &dum );
      graf_dragbox( max_icon.text_w, max_icon.h, l-(max_icon.text_w>>1),
	  m-(max_icon.h>>1), Xrect(*(Rect *)&z->desk[0].ob_x),
	  &z->desk[i].ob_x, &z->desk[i].ob_y );
      z->desk[i].ob_x -= z->desk[0].ob_x;
      z->desk[i].ob_y -= z->desk[0].ob_y;
      (is=&z->idat[--i])->c = k;
      strcpy( is->label, form[INSTRING].ob_spec.tedinfo->te_ptext );
      is->nicb = &nic_icons[is->type = foo-INFLOPPY].nicb;	/* 003: set nicb */
      if( foo <= INRAM && get_icvol( form, is ) )		/* 004 */
      {
        get_volname( &is->c, 0 );
        arrow();
      }
      break;
  }
  if( ret )
  {
    if( foo != INCANC )
    {
      if( foo == INALL ) do_desk();
      else rmv_icon_redraw(i);
      modal_unblit( f, 0, 0 );
    }
    if( inst_icon )
      if( i_insticon( form, f ) && inst_icon )
      {
	form_draw( f, 0, INLETTER );
	return 0;
      }
  }
  else if( foo!=INCANC ) modal_unblit( f, 0, 1 );
  return ret;
}

/********************************************************************/
int new_fsub( OBJECT *o, int *out, FILT_DESC *fd )
{
  static int filt_subs[] = { FILTM2, FILSIZ, FILDAT, FILTIM, FILATT },
	     filt_edit[] = { FILTM2E0, FILSIZE0, FILDATE0, FILTIME0, 0 };
  int i;

  for( i=0; i<5 && fd->filt_sub<0; i++ )
    if( o[FILTEMP+i].ob_state&SELECTED ) fd->filt_sub = i;
  for( i=0; i<5; i++ )
  {
    if( !i )
      if( !fd->filt_sub ) hide_if( o, FILTM2,
	  !hide_if( o, FILTM1, !fd->filter->flags.s.use_long ) );
      else hide_if( o, FILTM2, hide_if( o, FILTM1, 0 ) );
    else hide_if( o, filt_subs[i], i==fd->filt_sub );
    *(long *)&o[filt_subs[i]].ob_x = *(long *)&o[FILTM1].ob_x;
  }
  if( fd->filt_sub<0 )
  {
    if( out ) *out = 0;
    return 0;
  }
  if( out ) *out = !fd->filt_sub && !fd->filter->flags.s.use_long ? FILTM1E0 :
      filt_edit[fd->filt_sub];
  return !fd->filt_sub && !fd->filter->flags.s.use_long ? FILTM1 :
      filt_subs[fd->filt_sub];
}

int conv_idt( int idt, int **arr, int *m, int *d, int *y )
{
  switch( idt&0xf00 )
  {
    case 0x000:
      arr[0] = m;
      arr[1] = d;
      arr[2] = y;
      return 0;
    case 0x100:
      arr[0] = d;
      arr[1] = m;
      arr[2] = y;
      return 1;
    default:
      arr[0] = y;
      arr[1] = m;
      arr[2] = d;
      return 2;
    case 0x300:
      arr[0] = y;
      arr[1] = d;
      arr[2] = m;
      return 3;
  }
}
void filt_date( TEDINFO *ted, OBJECT *o, int idt, int m, int d, int y )
{
  int i, *arr[3];
  char *ptr = ted->te_ptext, *list=msg_ptr[1], c;
  
  list += 3*conv_idt( idt, arr, &m, &d, &y );
  spf( ptr, "%02d%02d%02d", *arr[0], *arr[1], *arr[2] );
  ptr = o[FILDATFM].ob_spec.free_string;
  if( (c = (char)idt) == 0 ) c = '/';
  for( i=0; i<3; i++ )
  {
    *ptr++ = *ptr++ = *list++;
    *ptr++ = c;
  }
  strcpy( ptr-1, msg_ptr[2] );
}
void init_filter( OBJECT *o, FILT_DESC *fd, int all_grp )
{
  int i, j;

  fd->filt_sub = -1;
  for( i=13; --i>=8; )
  {
    obj_true1( o, /*%j=*/ fd->filter->flags.i&(1<<i), 12-i+FILTEMP );
/*%    if( j && !fd->filt_sub ) fd->filt_sub = 12-i; */
  }
  for( i=0; i<6; i++ )
  {
    to_filename( z->template[i], o[FILTM1E0+i].ob_spec.tedinfo->te_ptext );
    obj_true1( o, fd->filter->flags.s.use_temp&(1<<i), FILTM1B0+i );
  }
  if( all_grp>=0 )
  {
    hide_if( o, FILALL, all_grp==1 );
    hide_if( o, FILGRP, all_grp==2 );
    *(long *)&o[FILGRP].ob_x = *(long *)&o[FILALL].ob_x;
  }
  obj_true1( o, fd->filter->flags.s.allfold, FILALL );
  obj_true1( o, fd->filter->flags.s.make_grp, FILGRP );
  obj_ltrue( o, fd->filter->use_size, 3, FILSIZB0 );
  obj_ltrue( o, fd->filter->use_date, 3, FILDATB0 );
  obj_ltrue( o, fd->filter->use_time, 3, FILTIMB0 );
  for( i=0; i<3; i++ )
  {
    spf( get_str(o,FILSIZE0+i), lfmt, fd->filter->sizes[i] );
    filt_date( o[FILDATE0+i].ob_spec.tedinfo, o, fd->filt_idt, fd->filter->dates[i]>>5 & 0xf,
	fd->filter->dates[i] & 0x1f, (j=(fd->filter->dates[i]>>9 & 0x7f)+80)<100 ? j : j-100 );
    spf( get_str(o,FILTIME0+i), "%02d%02d%02d", fd->filter->times[i]>>11 & 0x1f,
	fd->filter->times[i]>>5 & 0x3f, (fd->filter->times[i] & 0x1f)<<1 );
    o[FILTM2E0+i].ob_spec.tedinfo->te_ptext = fd->filter->long_tmpl[i];
  }
  obj_true1( o, fd->filter->mask&FA_READONLY, FILREAD );
  obj_true1( o, fd->filter->mask&FA_ARCHIVE, FILARC );
  new_fsub( o, 0L, fd );
  set_longedit( o, FILTM2E0, 3 );
}
#define FILT ((FILT_DESC *)f->mem_ptr)
void _init_filter( OBJECT *o, FORM *f, FILT_TYPE *out, char *title, int all_grp )
{
  FILT_DESC *fd = FILT;
  
  if( !filt_path[0] ) strcpy( filt_path, z->dflt_path );
  memcpy( FILT->filter=(FILT_TYPE *)((char *)f->mem_ptr+sizeof(FILT_DESC)),
      fd->cur_filt=out, sizeof(FILT_TYPE) );
  FILT->filt_hand = f->handle;
  FILT->filt_idt = z->idt_fmt;
  if( f->handle>0 ) f->old_title = title;
  else o[2].ob_spec.free_string = title;
  init_filter( o, FILT, all_grp );
}
int i_filter( OBJECT *o, FORM *f )
{
  _init_filter( o, f, &z->filter[w_num], msg_ptr[132], 1 );
  return 1;
}
void fix_str( OBJECT *o, int num, char *str )
{
  char *ptr, *ptr2;

  for( ptr=get_str(o,num), ptr2=str; ptr2<str+9; )
  {
    if( *ptr ) *ptr2++ = *ptr++;
    if( !*ptr || ptr2-str==2 || ptr2-str==5 || ptr2-ptr==8 ) *ptr2++ = '\0';
  }
}
unsigned int get_date( OBJECT *o, int num, int idt )
{
  char str[3*3];
  int *arr[3], m, d, y;

  fix_str( o, num, str );
  conv_idt( idt, arr, &m, &d, &y );
  *arr[0] = atoi(str);
  *arr[1] = atoi(str+3);
  *arr[2] = atoi(str+6);
  return( (m<<5) | d | ((y>=80 ? y-80 : y)<<9) );
}
unsigned int get_time( OBJECT *o, int num )
{
  char str[3*3];

  fix_str( o, num, str );
  return( (atoi(str)<<11) | (atoi(str+3)<<5) | (atoi(str+6)>>1) );
}
void get_filt( OBJECT *o, FILT_DESC *fd )
{
  int i, j;

  form = o;
  fd->filter->flags.s.templates = is_sel(FILTEMP);
  fd->filter->flags.s.size = is_sel(FILSIZE);
  fd->filter->flags.s.date = is_sel(FILDATE);
  fd->filter->flags.s.time = is_sel(FILTIME);
  fd->filter->flags.s.attrib = is_sel(FILATTR);
  fd->filter->flags.s.allfold = is_sel(FILALL);
  fd->filter->flags.s.make_grp = is_sel(FILGRP);
  fd->filter->mask = 0;
  if( is_sel(FILREAD) ) fd->filter->mask = FA_READONLY;
  if( is_sel(FILARC) ) fd->filter->mask |= FA_ARCHIVE;
  for( i=6, j=0; --i>=0; )
  {
    j <<= 1;
    j |= is_sel(FILTM1B0+i);
  }
  fd->filter->flags.s.use_temp = j;
  fd->filter->use_size = scan_sel( FILSIZB0, FILSIZB0+3 ) - FILSIZB0;
  fd->filter->use_date = scan_sel( FILDATB0, FILDATB0+3 ) - FILDATB0;
  fd->filter->use_time = scan_sel( FILTIMB0, FILTIMB0+3 ) - FILTIMB0;
  for( i=0; i<3; i++ )
  {
    fd->filter->sizes[i] = atol( get_str(o,FILSIZE0+i) );
    fd->filter->dates[i] = get_date( o, FILDATE0+i, fd->filt_idt );
    fd->filter->times[i] = get_time( o, FILTIME0+i );
  }
}
void draw_fsub( OBJECT *o, FILT_DESC *fd )
{
  int edit;

  if( fd->filt_hand>0 )
  {
    wind_set( fd->filt_hand, X_WF_DIALEDIT, 0, 0 );
    x_wdial_draw( fd->filt_hand, new_fsub( o, &edit, fd ), 8 );
    wind_set( fd->filt_hand, X_WF_DIALEDIT, edit, -1 );
  }
  else objc_draw( o, new_fsub( o, &edit, fd ), 8, 0, 0, 0, 0 );
}
int rfselect( FORM *f, char *path, char *templ, char *ltempl, char *title )
{
  int ret;
  
  ret = fselect( path, templ, ltempl, title );
  if( f->handle<=0 ) form_draw( f, 0, 0 );
  return ret;
}
int _x_filter( OBJECT *o, int num, FORM *f )
{
  char temp[150], mask;
  int i, oh, edit;
  struct Hdr
  {
    long magic;
    int ver;
  } str;
  FILT_TYPE f2;
  FILT_DESC *fd = FILT;

  switch( num )
  {
    case FILTM2B0:
    case FILTM1X0:
      o[num].ob_state &= ~SELECTED;  /* so use_form will not de-sel it later */
      fd->filter->flags.s.use_long ^= 1;
      draw_fsub(o,fd);
      return 0;
    case FILSAVE:
      get_filt( o, fd );
      if( rfselect( f, filt_path, "*.NFL", 0L, msg_ptr[131] ) )
      {
	bee();
#ifndef DEMO
	if( TOS_error( i=prep_save(filt_path), 0 ) )
	{
	  str.magic = FILT_MAGIC;
	  str.ver = FILT_VER;
	  if( !TOS_error( oh=cFwrite(i,sizeof(struct Hdr),&str), 0 ) ||
	      oh!=sizeof(struct Hdr) ||
	      !TOS_error( oh=cFwrite(i,sizeof(FILT_TYPE),fd->filter), 0 ) ||
	      oh!=sizeof(FILT_TYPE) )
	  {
	    cFclose(i);
	    cFdelete(filt_path);
	    arrow();
	    f_alert1( msg_ptr[15] );
	    return 0;
	  }
	  cFclose(i);
	  arrow();
	}
	else
	{
	  arrow();
	  spf( temp, msg_ptr[36], pathend(filt_path), "" );
	  f_alert1( temp );
	}
#else DEMO
	demo_version();
#endif
      }
      return 0;
    case FILLOAD:
      if( rfselect( f, filt_path, "*.NFL", 0L, msg_ptr[130] ) )
      {
#ifndef DEMO
	bee();
	if( TOS_error( i=cFopen(filt_path,0), 0 ) )
	{
	  if( TOS_error( oh=cFread(i,sizeof(struct Hdr),&str), 0 ) &&
	      oh==sizeof(struct Hdr) && str.magic==FILT_MAGIC && str.ver==FILT_VER &&
	      TOS_error( oh=cFread(i,sizeof(FILT_TYPE),&f2), 0 ) &&
	      oh==sizeof(FILT_TYPE) )
	  {
	    memcpy( fd->filter, &f2, sizeof(FILT_TYPE) );
	    wind_set( fd->filt_hand, X_WF_DIALEDIT, 0, 0 );
	    init_filter( o, fd, -1 );
	    new_fsub( o, &edit, fd );
	    wind_set( fd->filt_hand, X_WF_DIALEDIT, edit, -1 );
	    close_form( *(Rect *)&o[0].ob_x, 0 );
	  }
	  else
	  {
	    spf( temp, msg_ptr[56], ".NFL" );	/* 004: was wrong errmsg */
	    f_alert1( temp );
	  }
	  Fclose(i);
	}
	arrow();
#else DEMO
	demo_version();
#endif
      }
      return 0;
    case FILOK:
      get_filt( o, fd );
      memcpy( fd->cur_filt, fd->filter, sizeof(FILT_TYPE) );
      mask = 0;
      for( i=5; i>=0; i-- )
      {
	mask <<= 1;
	from_filename( form[FILTM1E0+i].ob_spec.tedinfo->te_ptext, temp, 1 );
	if( !temp[0] ) strcpy( temp, glob );
	if( strcmp( temp, z->template[i] ) )
	{
	  strcpy( z->template[i], temp );
	  mask |= 1;
	}
      }
      oh = w_num;
      for( w_num=0; w_num<7; w_num++ )
	if( (w_handle=wxref[w_num]) != oh && w_handle >= 0 &&
	    !z->filter[w_num].flags.s.use_long &&
	    z->filter[w_num].flags.s.use_temp&mask )
	{
	  set_wfile();
	  first(0);
	}
      w_handle = wxref[w_num=oh];
      set_wfile();
    default:
      return 1;
  }
}
int x_filter( OBJECT *o, int num, FORM *f )
{
  int ret;

  ret = _x_filter( o, num, f );
  if( num==FILOK )
  {
    re_name = 1;
    first(0);
  }
  return ret;
}
int t_filter( OBJECT *o, int num, FORM *f )
{
  if( o[num].ob_state&SELECTED )
  {
    FILT->filt_sub = num-FILTEMP;
    draw_fsub(o,FILT);
  }
  return 0;
}
/********************************************************************/
char search_templ[sizeof(z->search_filt.long_tmpl)];
int search_wnum;
int search_sel( int i )
{
  int k;

  k = i - (i%i_per_row[w_num]);
  if( k > max_itm[w_num][1] ) k = max_itm[w_num][1];
  if( z->w[w_num].f_off.i[1] != k )
  {
    z->w[w_num].f_off.i[1] = k;
    rdrw_all();
  }
  select_w( i, 1, w_handle, 1 );
  info();
  return(1);
}
int search_open( char *path, char *name )
{
  int i;
  FSTRUCT *fs;

  de_act( -1, -1 );
  open_to_path( path );
  for( fs=wfile, i=0; i<witems; i++, fs++ )
    if( !strcmp( fs->name, name ) ) return search_sel(i);
  return 0;
}
int into_group( DTA *d, char *path )
{
  PROG_TYPE t;
  int num, nx, x, y, dx, dy;
  NICONBLK *nib;
  char temp[120];
  FSTRUCT fs;
  
  if( search_wnum<0 ) return 0;
  strcpy( temp, path );
  strcpy( spathend(temp), d->d_fname );
  t = iprog_type( search_wnum, temp );
  fs.groupname[0] = 0;
  if( !(d->d_attrib & S_IJDIR) && strstr( d->d_fname, ".GRP" ) )	/* 003 */
    if( is_group( &fs, temp, 1 ) ) t.p.pexec_mode = GROUP;
  if( t.p.pexec_mode == NPI )		/* 003 */
    strcpy( fs.groupname, last_npi->npi.name );
  get_icon_match( path, d->d_fname, &nib, t.p.pexec_mode );
  group_unit( search_wnum, &dx, &dy );
  dx += group_desc[search_wnum]->hdr.snapx;
  dy += group_desc[search_wnum]->hdr.snapy;
  if( (nx = (ww[search_wnum][0].w+(dx>>1))/dx) == 0 ) nx = 1;
  x = (group_desc[search_wnum]->hdr.entries % nx) * dx;
  y = (group_desc[search_wnum]->hdr.entries / nx) * dy;
  if( !add_group( search_wnum, -1, temp, fs.groupname[0] ? fs.groupname/*003*/ : d->d_fname,
      &t, nib, x, y ) )
  {
    search_wnum = -1;
    return 0;
  }
  first(0);
  bee();
  return 1;
}
int search( char *path, int ignore, int srcwind )
{
  int i, k, l;
  char temp[120], temp2[22];
  FSTRUCT *f;
  DTA d;

  if( !tree_init(path,0L) ) return(1);
  if( path[0] == CLIP_LET )
  {
    strcpy( temp2, "|" );
    strcat( temp2, msg_ptr[95] );
  }
  else
  {
    temp2[0] = path[0];
    temp2[1] = ':';
    temp2[2] = '\0';
  }
  if( ignore )
    for( i=0, f=z->file[srcwind]; i<items[srcwind]; i++, f++ )
      if( f->type.p.pexec_mode != FOLDER )
      {
	strcpy( d.d_fname, f->name );
	d.d_attrib = f->read_only;
	d.d_time = (int)f->date;
	d.d_date = f->date>>16L;
	d.d_length = f->size;
	if( filter_it( &z->search_filt, &d, search_templ ) )
	  if( !into_group( &d, path ) )
	  {
	    strcpy( temp, path );
	    if( (l = pathend(temp)) != 3 ) temp[l-1] = '\0';
	    spf( diskbuff, msg_ptr[84], f->name, msg_ptr[85+(l==3)],
		temp2 );
	    spf( tmpf, diskbuff, spathend(temp) );
	    switch( f_alert1(tmpf) )
	    {
	      case 1:
		return search_sel(i);
	      case 3:
		return(1);
	    }
	  }
      }
  while( (i=tree_next(0L)) != 1 )
  {
    if( _abort() ) return(1);
    if( !i && (!ignore || maintree.tree_lev) && filter_it( &z->search_filt,
	&maintree.tree_curr, search_templ ) )
      if( !into_group( &maintree.tree_curr, maintree.tree_path ) )
      {
	*(maintree.tree_path+(i=pathend(maintree.tree_path))-1) = '\0';
	spf( diskbuff, msg_ptr[84], maintree.tree_fname, msg_ptr[85+(i==3)], temp2 );
	spf( tmpf, diskbuff, spathend(maintree.tree_path) );
	strcat( maintree.tree_path, slash );
	switch( f_alert1( tmpf ) )
	{
	  case 1:
	    return search_open( maintree.tree_path, maintree.tree_fname );
	  case 3:
	    return(1);
	}
      }
  }
  return(0);
}
SEL_ICON *srch_icon;
int i_search( OBJECT *o, FORM *f )
{
  if( f->wind<0 && (srch_icon = get_sel_icon( &f->icons, 1 )) == 0 ) return 0;
  _init_filter( o, f, &z->search_filt, msg_ptr[133], 2 );
  return 1;
}
void start_sgroup(void)
{
  if( z->search_filt.flags.s.make_grp && open_to_path(msg_ptr[33]) > 0 )
  {
    first(0);
    bee();
    search_wnum = w_num;
  }
  else search_wnum = -1;
}
int x_search( OBJECT *form, int foo, FORM *f )
{
  int ret, err=0, i;
  char temp[120];
  static char pth[]="x:\\";
  SEL_ICON *s;

  ret = _x_filter( form, foo, f );
  if( foo==FILOK )
  {
    z->search_filt.flags.s.allfold = 1;
    get_filt_templ( &z->search_filt, search_templ );
    if( !(z->search_filt.flags.i&0x1F00) ) return ret;	/* 003 */
    modal_unblit( f, 1, 0 );
    bee();
    if( !search_templ[0] ) strcpy( search_templ, glob );
    if( f->wind<0 )
    {
      start_sgroup();
      for( s=srch_icon; s && !err; )
      {
	if( (i=z->idat[s->u.desk_item-1].type) <= CLIPBRD )
	{
	  if( (*pth = get_drive(s->u.desk_item)) != 0 ) err = search( pth, 0, 0 );
	}
	else if( i>=D_PROG )
	{
	  i -= D_PROG;
	  if( z->programs[i].p.type.p.pexec_mode==FOLDER )
	      err = search( z->programs[i].p.path, 0, 0 );
	}
	s = get_sel_icon( &f->icons, -1 );
      }
    }
    else
    {
      de_act_w(-1,-1);
      info();
      strcpy( temp, z->w[f->wind].path );
      iso(temp);
      start_sgroup();
      err = search( temp, 1, f->wind );
    }
    if( !err && search_wnum<0 ) f_alert1( msg_ptr[87] );
    if( f->wind<0 ) de_act_d(-1);
    arrow();
    modal_unblit( f, 0, 0 );
    /***  form_draw( f, 0, 0 );  ***/
  }
  return ret;
}
/********************************************************************/
void init_groupinf( OBJECT *o, GROUP_HDR *gh, int nohide )
{
  o[NGRPWIND].ob_spec.tedinfo->te_ptext = gh->name;
  o[NGRPDESC].ob_spec.tedinfo->te_ptext = gh->desc[0];
  o[NGRPDESC+1].ob_spec.tedinfo->te_ptext = gh->desc[1];
  hide_if( o, GRPHIDE, nohide );
}

#define NEWGROUP	((GROUP_DESC *)f->mem_ptr)
int gr_wnum;	/* 003: save beforehand */

int i_group( OBJECT *o, FORM *f )
{
  memclr( NEWGROUP, sizeof(GROUP_DESC) );
  o[NGRPNAME].ob_spec.tedinfo->te_ptext[0] = 0;
  init_groupinf( o, &NEWGROUP->hdr, 0 );
  strcpy( NEWGROUP->path, z->w[gr_wnum=w_num].path );
  return 1;
}

unsigned long time_now(void)
{
  return ((unsigned long)Tgetdate()<<16)|Tgettime();
}

int save_group( char *path, GROUP_DESC *gd, int wnum )
{
  int i, j, ret=0;
  char temp[100];
  GROUP_HDR *gh=&gd->hdr;
  GROUP_ITEM *gi;
  NICONBLK *n;

  bee();
  if( TOS_error( i=prep_save(path), 0 ) )
  {
    gh->ver = GROUP_VER;
    gh->hdr_size = sizeof(GROUP_HDR);
    gh->ent_size = sizeof(GROUP_ITEM)-4;
    gh->modify = time_now();
    if( TOS_error( j=cFwrite(i,sizeof(GROUP_HDR),gh), 0 ) )
      if( j!=sizeof(GROUP_HDR) ) f_alert1( msg_ptr[15] );
      else
      {
        ret = 1;
        if( wnum>=0 )	/* 003 */
          for( gi=group_start[wnum]; gi && ret; gi=gi->next )
          {	/* 003: zero-out nicon before saving. Used to do 2 Fwrites() */
            n = gi->nicon;
            gi->nicon = 0L;
	    ret = TOS_error(
	        cFwrite( i, sizeof(GROUP_ITEM)-4, (long *)gi+1 ), 0 );
            gi->nicon = n;
          }
      }
    cFclose(i);
    if( !ret ) cFdelete(path);
    arrow();
    return ret;
  }
  else
  {
    arrow();
    spf( temp, msg_ptr[36], spathend(path), "" );
    f_alert1( temp );
    return ret;
  }
}

void new_group( GROUP_HDR *gh, int wnum )
{
  int x, y, w, h;

  x_wind_calc( WC_WORK, WIND_TYPE, XWIND_TYPE, Xrect(*(Rect *)&z->w[wnum].x),
      &x, &y, &w, &h );
  get_max_icon(-1);
  fix_coord( 0, &gh->x, x, char_w );
  fix_coord( 0, &gh->y, y, char_h );
  fix_coord( 0, &gh->w, w, icon_width(20)+2 );
  fix_coord( 0, &gh->h, h, max_icon.h );
  gh->split = 0;
  gh->offset[0] = gh->offset[1] = 0;
  gh->opts.i = 0xFFFF;
  gh->opts.s.showicon = z->mshowicon;
  gh->opts.s.largetext = z->mstlgsml;
  gh->sort = z->msort_type;
  gh->snapx = z->snapx;
  gh->snapy = z->snapy;
  gh->create = gh->modify = time_now();
}

void modal_update( FORM *f, char *path, int draw, int noclose )
{
  static char temp[]="x:\\";
  
  if( *(path+1)!=':' )
  {
    temp[0] = *path;
    path = temp;
  }
  modal_unblit( f, 1, 0 );
  first_no_close = noclose;
  update_drive( path, 0 );
  first_no_close = 0;
  modal_unblit( f, 0, draw );
}

int x_group( OBJECT *o, int num, FORM *f )
{
  char *ptr;

  if( num==NGRPOK )
  {
    if( *(ptr=o[NGRPNAME].ob_spec.tedinfo->te_ptext) == 0 ) return 0;
    if( !NEWGROUP->hdr.name[0] ) strcpy( NEWGROUP->hdr.name, ptr );
    iso( NEWGROUP->path );
    strcat( NEWGROUP->path, ptr );
    strcat( NEWGROUP->path, ".GRP" );
    new_group( &NEWGROUP->hdr, gr_wnum );
    if( save_group( NEWGROUP->path, NEWGROUP, -1 ) )
    {
      modal_update( f, NEWGROUP->path, 0, 0 );
      open_to_path( NEWGROUP->path );
    }
  }
  return 1;
}

/********************************************************************/
int i_about( OBJECT *o, FORM *f )
{
  f->tree = *mas->about;
  return 1;
}
int x_null( OBJECT *o, int num, FORM *f )
{
  return 1;
}
/********************************************************************/
GROUP_ITEM *gitem;
char *group_fname;
int gitem_wind;
#define NEWITEM 	((GROUP_ITEM *)f->mem_ptr)
int mac_check( unsigned char sh, unsigned char key );
void mac_key( unsigned char k1, unsigned char k2, unsigned char *k );
int mac_get( OBJECT *o, int off );
int mac_set( FORM *f, int off, int flag );
void hide_mbox( OBJECT *o, int off, int box2 );
void read_mackey( OBJECT *o, FORM *f, int off );
int i_gitem( OBJECT *o, FORM *f )
{
  int l;
  SEL_ICON *s;

  if( (s=get_sel_icon( &f->icons, 1 )) == 0L ) return 0;
  l = o[GRPILONG].ob_width/6;
  if( (group_fname = lalloc(l,gitem_wind=s->wnum)) == 0 ) return 0;
  memcpy( NEWITEM, gitem=s->u.fs->grp_item, sizeof(GROUP_ITEM) );
  short_path( gitem->p.path, o[GRPILONG].ob_spec.free_string=group_fname, l, l );
  o[GRPIWIND].ob_spec.tedinfo->te_ptext = NEWITEM->name;
  o[GRPIDESC].ob_spec.tedinfo->te_ptext = NEWITEM->desc[0];
  o[GRPIDESC+1].ob_spec.tedinfo->te_ptext = NEWITEM->desc[1];
  set_longedit( o, GRPIDESC, 2 );
  mac = &NEWITEM->key.shift - 2;
  mac_set( f, GRPIBOX, 0 );
  hide_mbox( o, GRPIBOX, 1 );
  return 1;
}
int x_gitem( OBJECT *o, int num, FORM *f )
{
  if( num==GRPIRKEY )
  {
    read_mackey( o, f, GRPIBOX );
    return 0;
  }
  else if( num==GRPICKEY )
  {
    *(mac+2) = *(mac+3) = 0;
    mac_set( f, GRPIBOX, 0 );
    form_draw( f, GRPIBOX, 0 );
    return 0;
  }
  else if( num==GRPIOK )
    if( mac_get( o, GRPIBOX ) )
    {
      memcpy( gitem, NEWITEM, sizeof(GROUP_ITEM) );
      modal_unblit( f, 1, 0 );
      update_othwind( gitem_wind, 1 );
      modal_unblit( f, 0, 0 );
    }
  cmfree(&group_fname);
  if( num>0 )		/* not cancelling due to parent window close */
    if( i_gitem(o,f) )	/* get next item */
    {
      form_draw( f, 0, GRPIWIND );
      return 0;
    }
  return 1;
}
/********************************************************************/
int snap_num, tsnapx, tsnapy;
void set_boxes( OBJECT *o )
{
  o[SNAPI0+1].ob_x = o[SNAPI0+3].ob_x = o[SNAPI0].ob_x + tsnapx + 12*6+2;
  o[SNAPI0+2].ob_y = o[SNAPI0+3].ob_y = o[SNAPI0].ob_y + tsnapy + 32+8;
}
#define SORTTEMP	((ICONSAVE *)f->mem_ptr)
int i_snap( OBJECT *o, FORM *f )
{
  if( (snap_num = w_num) >= 0 && !group_desc[snap_num] ) snap_num = -1;
  if( (SORTTEMP = lalloc(sizeof(ICONSAVE)*z->num_icons,snap_num)) == 0 )
      return 0;
  obj_true1( o, z->snap_resort, SNAPSORT );
  obj_true1( o, z->snap_over, SNAPOVER );
  if( hide_if( o, SNAPSORT, snap_num >= 0 ) )
  {
    tsnapx = group_desc[snap_num]->hdr.snapx;
    tsnapy = group_desc[snap_num]->hdr.snapy;
  }
  else
  {
    if( !z->num_icons ) return 0;
    tsnapx = z->snapx;
    tsnapy = z->snapy;
  }
  o[SNAPI0].ob_width = o[SNAPI0+1].ob_width = o[SNAPI0+2].ob_width =
      o[SNAPI0+3].ob_width = 12*6+2;
  o[SNAPI0].ob_height = o[SNAPI0+1].ob_height = o[SNAPI0+2].ob_height =
      o[SNAPI0+3].ob_height = 32+8;
  set_boxes(o);
  return 1;
}
int snap_int( int i )
{
  if( i>0 ) return 1;
  else if( i<0 ) return -1;
  return 0;
}
int snap_cmpg( FSTRUCT *a, FSTRUCT *b )
{
  int i;

  if( (i = snap_int(a->grp_item->y - b->grp_item->y)) == 0 )
      i = snap_int(a->grp_item->x - b->grp_item->x);
  return i;
}
int snap_cmpd( OBJECT *a, OBJECT *b )
{
  int i;

  if( a->ob_flags&HIDETREE )
    if( b->ob_flags&HIDETREE ) return 0;
    else return -1;
  else if( b->ob_flags&HIDETREE ) return 1;
  if( (i = snap_int(a->ob_y - b->ob_y)) == 0 ) i =
      snap_int(a->ob_x - b->ob_x);
  return i;
}
void get_snaps( OBJECT *o )
{
  form = o;
  z->snap_resort = is_sel(SNAPSORT);
  z->snap_over = is_sel(SNAPOVER);
}
int snap_ok( int i )
{
  return !(z->desk[i].ob_flags&HIDETREE) && (d_active<0 ||
      z->idat[i-1].state&1);
}
int x_snap( OBJECT *o, int num, FORM *f )
{
  int x0, y0, bordx, bordy, i, j, *x, over=0, tx, ty;
  FSTRUCT *fs;
  GROUP_ITEM *gi, *gi2;

  get_max_icon(snap_num);
  tx = tsnapx + max_icon.text_w;
  ty = tsnapy + max_icon.h;
  switch( num )
  {
    case SNAPOK:
      get_snaps(o);
      over = z->snap_over;
      if( snap_num >= 0 )
      {
	group_desc[snap_num]->hdr.snapx = tsnapx;
	group_desc[snap_num]->hdr.snapy = tsnapy;
	x0 = (ww[snap_num][0].w + (tx>>1)) / tx;
	if( z->snap_resort )
	{
	  over = 0;
	  qsort( fs=z->file[snap_num], items[snap_num],
	      sizeof(FSTRUCT), cmp_name );
	  for( i=0; i<items[snap_num]; i++, fs++ )
	  {
	    fs->grp_item->x = (i%x0) * tx;
	    fs->grp_item->y = (i/x0) * ty;
	  }
	}
	else for( gi=group_start[snap_num]; gi; gi=gi->next )
	{
	  if( (gi->x = (gi->x + (tx>>1)) / tx * tx) > (x0-1)*tx ) gi->x = (x0-1)*tx;	/* 003: added if */
	  gi->y = (gi->y + (ty>>1)) / ty * ty;
	}
      }
      else
      {
	z->snapx = tsnapx;
	z->snapy = tsnapy;
	if( (bordx = tx-max_icon.text_w-4) < 0 ) bordx = 0;
	if( (bordy = ty-max_icon.h) < 0 ) bordy = 0;
	x0 = (z->maximum.w-max_icon.text_w-4) / tx * tx + bordx;
	y0 = (z->maximum.h-max_icon.h) / ty * ty + bordy;
	for( i=1; i<=z->num_icons; i++ )
	  if( snap_ok(i) )
	  {
	    if( (z->desk[i].ob_x = bordx + (z->desk[i].ob_x + (tx>>1)) /
		tx * tx) > x0 ) z->desk[i].ob_x = x0;
	    if( (z->desk[i].ob_y = bordy + (z->desk[i].ob_y + (ty>>1)) /
		ty * ty) > y0 ) z->desk[i].ob_y = y0;
	  }
      }
    case SNAPCLEA:
      get_snaps(o);
      if( snap_num >= 0 )
      {
	qsort( fs=z->file[snap_num], items[snap_num],
	    sizeof(FSTRUCT), snap_cmpg );
	group_start[snap_num] = gi = fs->grp_item;
	x0 = (x0-1) * tx;	/* 003: was *= tx */
	for( i=1, fs++; i<items[snap_num]; i++, fs++ )
	{
	  if( over )
	    for( gi2=group_start[snap_num]; gi2!=gi; gi2=gi2->next )
	      if( *(long *)&gi2->x == *(long *)(x=&gi->x) )
		if( (*x += tx) > x0 )
		{
		  *(x+1) += ty;
		  *x = 0;
		}
	  gi = (gi->next = fs->grp_item);
	}
	gi->next = 0L;
	modal_unblit( f, 1, 0 );
	update_othwind( snap_num, 0 );
	modal_unblit( f, 0, 0 );
      }
      else
      {
	memcpy( SORTTEMP, z->idat, sizeof(ICONSAVE)*z->num_icons );
	/* temporarily set last object's ob_next to "next" icon */
	z->desk[z->num_icons].ob_next = z->num_icons+1;
	qsort( &z->desk[1], z->num_icons, sizeof(OBJECT), snap_cmpd );
	for( i=1; i<=z->num_icons; i++ )
	{
	  memcpy( &z->idat[i-1], &SORTTEMP[z->desk[i].ob_next-2], sizeof(ICONSAVE) );
	  z->desk[i].ob_next = i+1;
	}
	z->desk[i-1].ob_next = 0;
	if( over )
	  for( i=1; i<=z->num_icons; i++ )
	    if( snap_ok(i) )
	      for( j=1; j<i; j++ )
		if( snap_ok(j) && *(long *)&z->desk[j].ob_x ==
		    *(long *)(x=&z->desk[i].ob_x) )
		  if( (*x += tx) > x0 )
		    if( (*(x+1) += ty) > y0 )
		    {
		      *x = x0;
		      *(x+1) = y0;
		    }
		    else *x = bordx;
	modal_unblit( f, 1, 0 );
	do_desk();
	modal_unblit( f, 0, 0 );
      }
    default:
      return 1;
  }
}
int t_snap( OBJECT *o, int num, FORM *f )
{
  int x0, y0, bordx, bordy;

  x0 = y0 = 0;
  switch( num )
  {
    case SNAPUP:
      if( tsnapy>-12 ) y0--;
      break;
    case SNAPDOWN:
      if( tsnapy<9 ) y0++;
      break;
    case SNAPLEFT:
      if( tsnapx>-20 ) x0--;
      break;
    case SNAPRT:
      if( tsnapx<13 ) x0++;
      break;
    case SNAPDFLT:
      x0 = 6-tsnapx;
      y0 = 1-tsnapy;
      break;
  }
  if( x0 || y0 )
  {
    tsnapx += x0;
    tsnapy += y0;
    set_boxes(o);
    form_draw( f, SNAPBOX, 0 );
  }
  return 0;
}
/********************************************************************/
unsigned char memupdt[] = { MEMST, MEMTT, MEMFREE, MEMLARGE, MEMNEO };
void ver2asc( unsigned int ver, char *str )
{
  spf( str, ver>=0x160 ? "%x.%02x" : "%x.%x", ver>>8, (unsigned char)ver );
}
int umem( OBJECT *o, int reset )
{
  static int msg[] = { 140, 141, 119, 120, 142 };
  static long STmem[3], TTmem[3], large[2], total[2], blocks[2], neomem1[2], neomem2[2],
      *ptrs[5][3] = { &STmem[0], 0L, &STmem[2],
                      &TTmem[0], 0L, &TTmem[2],
                      &total[0], &blocks[0], &blocks[0],
                      &large[0], 0L, 0L,
                      &neomem1[0], &neomem2[0], &neomem2[0] };
  int ret, i;
  long stack, l, *p1, *p2, *p3;
  char *prev, *new;

  if( reset )
  {
    STmem[1] = TTmem[1] = large[1] = total[1] = neomem1[1] = neomem2[1] = -1L;
    blocks[1] = -1;
  }
  stack = Super((void *)0L);
  blocks[0]=total[0]=large[0]=STmem[0]=TTmem[0]=0L;
  prev=0L;
  memstat( &neomem1[0], &neomem2[0] );
  while( (l=(long)Malloc(-1L)) >= 4L )
  {
    if( (new=Malloc(l)) == 0 ) break;
    if( !total[0] ) large[0] = l;
    total[0] += l;
    if( (long)new > *(long *)0x436 ) TTmem[0] += l;	/* 003: was (long)new >> 24L */
    else STmem[0] += l;
    blocks[0]++;
    *(char **)new = prev;
    prev = new;
  }
  while( prev )
  {
    new = *(char **)prev;
    Mfree(prev);
    prev = new;
  }
  STmem[2] = PHYSTOP;
  TTmem[2] = _ramvalid==0x1357bd13L ?
      (_ramtop<0x1000000L ? _ramtop&0xFFFFFFL : _ramtop-0x1000000L) :
      0L;
  Super( (void *)stack );
  for( i=ret=0; i<5; i++ )
  {
    p1 = ptrs[i][0];
    p2 = ptrs[i][1];
    p3 = ptrs[i][2];	/* 003 */
    if( *p1!=*(p1+1) || p2 && *p2!=*(p2+1) )
    {
      ret |= 1<<i;
      *(p1+1) = *p1;
      if( p2 ) *(p2+1) = *p2;
      spf( o[memupdt[i]].ob_spec.tedinfo->te_ptext, msg_ptr[msg[i]],
	  *p1, p3 ? *p3 : 0L );
    }
  }
  return ret;
}
int i_memory( OBJECT *o, FORM *f )
{
  char buf[2][10];
  unsigned int v = Sversion();

  ver2asc( (int)Supexec(get_OS), buf[0] );
  ver2asc( _GemParBlk.global[0], buf[1] );
  spf( o[MEMROM].ob_spec.tedinfo->te_ptext, msg_ptr[81], buf[0],
      (unsigned char)v, v>>8, buf[1] );
  spf( o[MEMTKB].ob_spec.tedinfo->te_ptext, lfmt, z->take_always/1024L );
  spf( o[MEMLKB].ob_spec.tedinfo->te_ptext, lfmt, z->limit_to/1024L );
  obj_ltrue( o, z->mem_mode, 3, MEMLIM );
  umem( o, 1 );
  return 1;
}
void pref_saveinf(void)
{
#ifndef DEMO
  int hand;
  char c, filename[120];

  if( f_alert1( msg_ptr[93] ) == 1 )
  {
    bee();
    strcpy( filename, z->dflt_path );
    strcat( filename, "NEO_INF.DAT" );
    if( (hand=cFopen(filename,2)) == IEFILNF ) hand = cFcreate(filename,0);
    if( TOS_error( (long)hand, 0 ) )
    {
      c = INFDAT_VER;
      cFwrite( hand, 1L, &c );
      cFwrite( hand, 40L, z->rezes[0] );
      cFwrite( hand, 90L, z->rezname[0] );
      cFwrite( hand, (long)(MAX_NEO_ACC*9+1), z->neo_acc );
      cFwrite( hand, 2L, &z->rezvdi );
      cFwrite( hand, 9L, z->drezname );
      cFwrite( hand, sizeof(z->mem_limit), z->mem_limit );
      cFclose(hand);
    }
    arrow();
  }
#endif
}
int x_memory( OBJECT *o, int num, FORM *f )
{
  int i;
  long take, limit;
  M_LIMIT *l;

  if( num==MEMOK )
  {
    form = o;
    i = scan_sel( MEMLIM, MEMLIM+3 ) - MEMLIM;
    take = atoi(o[MEMTKB].ob_spec.tedinfo->te_ptext)*1024L;
    limit = atoi(o[MEMLKB].ob_spec.tedinfo->te_ptext)*1024L;
    if( z->take_always != take || z->limit_to != limit || z->new_mem_mode != i )
    {
      if( z->new_mem_mode!=MEM_ALWAYS && i==MEM_ALWAYS ) f_alert1( msg_ptr[32] );
      else z->mem_mode = i;
      l = &z->mem_limit[z->rez_num];
      z->take_always = l->take_always = take;
      z->limit_to = l->limit_to = limit;
      z->new_mem_mode = l->mem_mode = i;
      pref_saveinf();
    }
  }
  return 1;
}
int u_memory( OBJECT *o, FORM *f )
{
  int i, j;

  i = umem( o, 0 );
  for( j=0; j<5; j++ )
    if( i&(1<<j) ) form_draw( f, memupdt[j], 0 );
  return 1;	/* no exit if modal */
}

/********************************************************************/
int i_savecfg( OBJECT *o, FORM *f )
{
  int i;
  
  for( i=0; i<3; i++ )
  {
    strcpy( o[i*3+SIDNAM].ob_spec.tedinfo->te_ptext, z->new_inf_name[i] );
    obj_selec( o, z->saveconf&(1<<i), 1, i*3+SIDON );
  }
  return 1;
}
int x_savecfg( OBJECT *o, int num, FORM *f )
{
  int e, file, k, l;
  char *ptr, temp[120];

  if( num != SIOK ) return 1;
#ifndef DEMO
  bee();
  form = o;
  for( z->saveconf=0, e=2; e>=0; e-- )	     /* must be reversed so saveconf is right */
    if( is_sel(SIDON+3*e) && *(ptr=o[SIDNAM+3*e].ob_spec.tedinfo->te_ptext) != 0 )
    {
      z->saveconf |= (1<<e);
      strcpy( z->new_inf_name[e], ptr );
      strcpy( temp, z->dflt_path );
      strcat( temp, ptr );
      strcat( temp, ext[e] );
      if( (file = prep_save( temp )) < 0 )
      {
	spf( temp, msg_ptr[36], z->new_inf_name[e], ext[e] );
	f_alert1( temp );
	e = -1;
      }
      else switch(e)
      {
	case 0:
	  if( f->handle<=0 ) wind_lock(0);
	  saveinf(file);
	  if( f->handle<=0 ) wind_lock(1);
	  break;
	case 1:
	  ptr = z->macro;
	  l = z->macro_len;
	  k = MACRO_VER;
	  goto save_it;
	case 2:
	  ptr = z->notes;
	  l = z->notes_len;
	  k = NOTE_VER;
save_it:  cFwrite( file, 4L, ext[e] );
	  cFwrite( file, 2L, &k );
	  cFwrite( file, 2L, &l );
	  if( e==2 )
	  {
	    cFwrite( file, 4L, &z->wind_font[4].id );
	    cFwrite( file, 2L, &z->note_col );
	  }
	  if( l ) cFwrite( file, l, ptr );
	  break;
      }
      if( e>=0 ) cFclose( file );
    }
  modal_update( f, z->dflt_path, 0, 0 );
  arrow();
#else DEMO
  demo_version();
#endif DEMO
  return 1;
}
/********************************************************************/
char *ttp_ptr, *ttp_path;
#define TTP_SHORT 124-38*3
#define TTP_ADD ((char *)f->mem_ptr)
void ttp_mode( OBJECT *o, int u, int off )
{
  TEDINFO *ted;

  obj_true1( o, u, TOSARGV-TOSEDIT+off );
  ted = o[TOSEDIT+3-TOSEDIT+off].ob_spec.tedinfo;
  o[TOSEDIT+3-TOSEDIT+off].ob_width = (!u ? TTP_SHORT : 38)*char_w;
  ted->te_txtlen = ted->te_tmplen = !u ? TTP_SHORT+1 : 39;
  ted->te_pvalid[TTP_SHORT] = !u ? 0 : 'X';
  ted->te_ptmplt[TTP_SHORT] = !u ? 0 : '_';
  set_longedit( o, off, u ? 5 : 4 );
  if( !u ) ted->te_tmplen = TTP_SHORT+1;
  hide_if( o, TOSEDIT+4-TOSEDIT+off, u );
}
int i_ttp( OBJECT *o, FORM *f )
{
  register int i, j;
  register char *ptr;

  for( j=0, i=TOSEDIT; i<TOSEDIT+5; i++ )
  {
    ptr = o[i].ob_spec.tedinfo->te_ptext;
    if( ttp_ptr )
      if( j<strlen(ttp_ptr) )
      {
	memcpy( ptr, ttp_ptr+j, 38 );
	*(ptr+38) = '\0';
	j += 38;
      }
      else *ptr='\0';
    else strcpy( ptr, z->ttp_params[i-TOSEDIT] );
  }
  strcpy( TTP_ADD, ttp_path );
  ttp_mode( o, z->use_argv>0, TOSEDIT );
  return 1;
}
int x_ttp( OBJECT *o, int num, FORM *f )
{
  int i;
  
  if( num >= 0 )
  {
    for( i=0; i<5; i++ )
      strcpy( z->ttp_params[i], o[i+TOSEDIT].ob_spec.tedinfo->te_ptext );
    z->use_argv = o[TOSARGV].ob_state&SELECTED;
  }
  return 1;
}
void append_ttp( OBJECT *o, char *str, int first )
{
  int i, l, m;
  char *ptr;

  for( i=5; --i>=0; )
    if( o[i+TOSEDIT].ob_spec.tedinfo->te_ptext[0] ) break;
  if( i<0 )
  {
    if( first ) return;
    i=0;
  }
  o += i+TOSEDIT;
  while( *str )
  {
    while( (l=strlen( ptr=o->ob_spec.tedinfo->te_ptext )) >= 38 )
      if( ++i>=5 ) return;
      else o++;
    strncpy( ptr+l, str, m=38-l );
    if( strlen(str) <= m ) return;
    str += m;
  }
}
int t_ttp( OBJECT *o, int num, FORM *f )
{
  if( num==TOSARGV )
  {
    ttp_mode( o, o[TOSARGV].ob_state&SELECTED, TOSEDIT );
    form_draw( f, TOSBOX, TOSEDIT );
  }
  else if( num==TOSADD )
    if( rfselect( f, TTP_ADD, "*.*", 0L, msg_ptr[172] ) )
    {
      append_ttp( o, " ", 1 );
      append_ttp( o, TTP_ADD, 0 );
      form_draw( f, TOSBOX, TOSEDIT );
    }
  return 0;
}
/********************************************************************/
PICOPT *picopts;
int i_picopt( OBJECT *o, FORM *f )
{
  o[2].ob_spec.free_string = picopts==&z->desk_picopts ? msg_ptr[180] :
      msg_ptr[181];
  obj_true1( o, picopts->fit, PICOFIT );
  obj_ltrue( o, picopts->mode, 3, PICOUNCH );
  return 1;
}
int x_picopt( OBJECT *o, int num, FORM *f )
{
  if( num==PICOOK )
  {
    form = o;
    picopts->fit = is_sel(PICOFIT);
    picopts->mode = scan_sel( PICOUNCH, PICOUNCH+3 ) - PICOUNCH;
  }
  return 1;
}
/********************************************************************/
int i_more( OBJECT *o, FORM *f )
{
  unsigned int i;
  
  obj_true1( o, z->other_pref.b.consumption, PREFCONS );
  obj_ltrue( o, z->other_pref.b.dflt_twst, 4, MSPRNOR );
  obj_true1( o, z->other_pref.b.virus_check, MSPRVIR );
  obj_true1( o, z->other_pref.b.check_fnames, MSPRNAM );
  obj_true1( o, z->other_pref.b.av_server, MSPRAV );
  obj_true1( o, z->other_pref.b.use_kobold, MSPRKO );
  obj_true1( o, z->other_pref.b.prevent_mult, MSPMULT );
  obj_enab( o, z->other_pref.b.av_server, 1, MSPMULT );
  hide_if( o, MSPRKO, z->cliparray[2]>=639 );	/* Kobold doesn't work in low rez */
  obj_ltrue( o, z->status_report, 3, PREFSALL );
  if( (i=(unsigned char)(z->idt_fmt>>8)) > 3 ) i = 0;
  obj_ltrue( o, i, 4, MSPRDATE );
  o[MSPRDSEP].ob_spec.tedinfo->te_ptext[0] = (char)z->idt_fmt ? (char)z->idt_fmt : '/';
  o[MSPRNSEP].ob_spec.tedinfo->te_ptext[0] = z->num_sep;	/* 003 */
  obj_true( o, !z->other_pref.b.long_numbers, MSPRNSHR );	/* 005 */
  return 1;
}
int t_more( OBJECT *o, int num, FORM *f )
{
  form = o;
  obj_enab( o, is_sel(MSPRAV), 1, MSPMULT );
  form_draw( f, MSPRBOX, 1 );
  return 0;
}
int x_more( OBJECT *o, int num, FORM *f )
{
  int oh, i, j;
  char c, c2;

  if( num == MSPROK )
  {
    form = o;
    z->other_pref.b.dflt_twst = scan_sel( MSPRNOR, MSPRNOR+4 )-MSPRNOR;
    z->other_pref.b.virus_check = is_sel(MSPRVIR);
    z->other_pref.b.check_fnames = is_sel(MSPRNAM);
    z->other_pref.b.av_server = is_sel(MSPRAV);
    z->other_pref.b.prevent_mult = is_sel(MSPMULT);
    set_avserv();
    z->other_pref.b.use_kobold = is_sel(MSPRKO);
    z->status_report = scan_sel(PREFSALL,PREFSALL+3)-PREFSALL;
    z->idt_fmt = ((scan_sel( MSPRDATE, MSPRDATE+4 )-MSPRDATE) << 8) |
	o[MSPRDSEP].ob_spec.tedinfo->te_ptext[0];
    c = *lc->num_sep;
    *lc->num_sep = z->num_sep = o[MSPRNSEP].ob_spec.tedinfo->te_ptext[0];	/* 003 */
    c2 = *lc->long_numbers;
    *lc->long_numbers = z->other_pref.b.long_numbers = is_sel(MSPRNLNG);	/* 005 */
    i = z->other_pref.b.consumption;
    z->other_pref.b.consumption = j = is_sel(PREFCONS);
    if( j != i || z->num_sep!=c || *lc->long_numbers!=c2/*005*/ )
    {
      modal_unblit( f, 1, 0 );
      oh = w_num;
      for( w_num=0; w_num<7; w_num++ )
	if( (w_handle=wxref[w_num]) >= 0 && ed_wind_type(w_num)==EDW_DISK )
	{
	  set_wfile();
	  if(j) first(0);
	  else
	  {
	    redraw_wind( z->maximum, 1 );
	    info();
	  }
	}
      modal_unblit( f, 0, 0 );
      w_handle = wxref[w_num=oh];
      set_wfile();
    }
  }
  else if( num==MSPRVIEW )
  {
    picopts = &z->view_picopts;
    start_sform( PIC_FORM, &f );
    return 0;
  }
  return 1;
}
/********************************************************************/
int i_filepref( OBJECT *o, FORM *f )
{
  obj_ltrue( o, z->move_mode, PREFMASK-PREFMCPY+1, PREFMCPY );
  obj_ltrue( o, z->back_speed, 5, PREFBAKS );
  obj_true1( o, z->conf_copy, PREFCYES );
  obj_true1( o, z->conf_del, PREFDYES );
  obj_true1( o, z->conf_over, PREFOYES );
  obj_true1( o, z->tos_pause, PREFPYES );
  obj_true1( o, z->use_master, PREFMYES );
  obj_true1( o, z->quit_alert, PREFQYES );
  obj_ltrue( o, z->other_pref.b.clip_mode, 3, MSPRALW );
  return 1;
}
int x_filepref( OBJECT *o, int num, FORM *f )
{
  if( num==PREFOK )
  {
    form = o;
    z->move_mode = scan_sel(PREFMCPY,PREFMASK+1)-PREFMCPY;
    z->conf_copy = is_sel(PREFCYES);
    z->conf_del = is_sel(PREFDYES);
    z->conf_over = is_sel(PREFOYES);
    z->back_speed = scan_sel(PREFBAKS,PREFBAKS+5)-PREFBAKS;
    z->tos_pause = is_sel(PREFPYES);
    z->use_master = is_sel(PREFMYES);
    z->other_pref.b.clip_mode = scan_sel( MSPRALW, MSPRALW+3 )-MSPRALW;
    z->quit_alert = is_sel(PREFQYES);
  }
  return 1;
}
/********************************************************************/
static int inf_max;
void pref_list( OBJECT *o, int typ, char *temp )
{	/* typ is true if editing INF file list */
  int i, *r;

  if( typ )
  {
    obj_true1( o, z->rezvdi&(1<<10), ACCCLI0+10 );
    o[LISTNAM0+10].ob_spec.tedinfo->te_ptext = temp+9*10;
  }
  else obj_ltrue( o, z->cli, 10, ACCCLI0 );
  for( i=0, inf_max=-1; i<10; i++ )
  {
    o[LISTNAM0+i].ob_spec.tedinfo->te_ptext = temp+9*i;
    o[LISTNAM0+i].ob_spec.tedinfo->te_txtlen = 9;
    if( typ )
    {
      obj_true1( o, z->rezvdi&(1<<i), ACCCLI0+i );
      r = z->rezes[i];
      if( r[0] || inf_max<0 )
      {
	spf( o[LISTSIZ0+i].ob_spec.free_string, "%d x %d",
	    r[0] ? r[0] : graphics->v_x_max, r[0] ? r[1] : graphics->v_y_max );
	if( !r[0] ) strcpy( temp+9*i, z->inf_name );
	o[LISTSIZ0+i].ob_flags &= ~HIDETREE;
	o[LISTNAM0+i].ob_flags &= ~HIDETREE;
	o[LISTNAM0+i].ob_flags |= EDITABLE;
      }
      else
      {
	o[LISTSIZ0+i].ob_flags |= HIDETREE;
	o[LISTNAM0+i].ob_flags |= HIDETREE;
	o[LISTNAM0+i].ob_flags &= ~EDITABLE;
      }
      if( inf_max<0 && (!r[0] || r[0]==graphics->v_x_max && r[1]==graphics->v_y_max) ) inf_max=i;
    }
  }
}
#define ACCTEMP 	((char *)f->mem_ptr)
int i_accpref( OBJECT *o, FORM *f )
{
  int i;
  char *ptr;
  
  memcpy( ACCTEMP, z->neo_acc[0], MAX_NEO_ACC*9 );
  for( i=0; i<MAX_NEO_ACC; i++ )
    if( (ptr=strchr(ACCTEMP+9*i,' ')) != 0 ) *ptr='\0';
  pref_list( o, 0, ACCTEMP );
  return 1;
}
int x_accpref( OBJECT *o, int num, FORM *f )
{
  int i, j, cli;
  char *ptr;

  if( num == LISTOK )
  {
    form = o;
    for( i=0; i<MAX_NEO_ACC; i++ )
    {
      j = strlen( ptr = ACCTEMP+9*i );
      for( ; j<8; j++ )
	*(ptr+j) = ' ';
      *(ptr+j) = '\0';
      if( is_sel(ACCCLI0+i) ) cli = i;
    }
    if( z->cli!=cli || memcmp( z->neo_acc[0], ACCTEMP, MAX_NEO_ACC*9 ) )
    {
#ifndef DEMO
      write_acc_bad();
#endif
      memcpy( z->neo_acc[0], ACCTEMP, MAX_NEO_ACC*9 );
      z->cli = cli;
      pref_saveinf();
      get_d_icon(-1);
#ifndef DEMO
      neo_acc_init();
#endif
    }
  }
  return 1;
}
/********************************************************************/
void new_inf_name(void)
{
  char temp[20], **ptr;
  static char str[40], *key;
  extern OBJECT *lowmenu;

  strcpy( temp, z->inf_name );
  strcat( temp, ".INF" );
  if( *(ptr = &menu[MPOPRELD].ob_spec.free_string) != str && !lowmenu )
  {	  /* first time, find key equiv */
    key = *ptr + strlen(*ptr);
    while( *--key==' ' );
    while( *--key!=' ' );
    key++;
  }
  *ptr = str;
  if( lowmenu ) spf( str, "  %s", temp );
  else spf( str, msg_ptr[46], temp, key );
}
#define INFTEMP 	((char *)f->mem_ptr)
int i_infpref( OBJECT *o, FORM *f )
{
  memcpy( INFTEMP, z->rezname[0], 10*9 );
  strcpy( INFTEMP+9*10, z->drezname );
  pref_list( o, 1, INFTEMP );
  return 1;
}
int x_infpref( OBJECT *o, int num, FORM *f )
{
  int i, rvdi[10], nrezvdi, notdflt[3];
  char *path, *ptr;
  
  if( num==INFLOK )
  {
    form = o;
    for( i=nrezvdi=0; i<10; i++ )
      rvdi[i] = is_sel(ACCCLI0+i);
    for( i=0; i<3; i++ )	/* 003 */
      notdflt[i] = strcmp( z->inf_name, z->new_inf_name[i] );
    if( is_sel(ACCCLI0+10) ) nrezvdi |= (1<<10);
    for( i=0; i<10 && z->rezes[i][0]; )
      if( !INFTEMP[i*9] )
      {
	memcpy( z->rezes[i], z->rezes[i+1], (9-i)<<2 );
	memcpy( INFTEMP+i*9, INFTEMP+i*9+9, (9-i)*9 );
	memcpy( &rvdi[i], &rvdi[i+1], (9-i)*2 );
	INFTEMP[9*9] = '\0';
	z->rezes[9][0] = z->rezes[9][1] = 0;
	if( inf_max==i ) inf_max = -1;
	else if( inf_max>i ) inf_max--;
	if( i*9 == z->inf_name-z->rezname[0] )
	{
	  z->inf_name = z->drezname;		/* 003: was z->rezname[9] */
/*	  new_inf_name();  003 */
	}
      }
      else
      {
	if( rvdi[i] ) nrezvdi |= 1<<i;
	i++;
      }
    if( inf_max>=0 && !z->rezes[inf_max][0] )	/* max already -1 if blank */
    {
      z->rezes[inf_max][0] = graphics->v_x_max;
      z->rezes[inf_max][1] = graphics->v_y_max;
      z->inf_name = z->rezname[inf_max];
/*      new_inf_name();	003: moved */
    }
    else if( !memcmp( z->rezname[0], INFTEMP, 9*10 ) &&
	nrezvdi==z->rezvdi && !strcmp( z->drezname, INFTEMP+9*10 ) )
    {
/*      z->rezvdi = nrezvdi;  003: not needed, already == */
      return 1;
    }
    z->rezvdi = nrezvdi;
    memcpy( z->rezname[0], INFTEMP, 10*9 );
    strcpy( z->drezname, INFTEMP+10*9 );
    for( i=0; i<3; i++ )	/* 003: changed */
      if( notdflt[i] ) strcpy( z->new_inf_name[i], z->inf_name );
    new_inf_name();	/* 003 */
    pref_saveinf();
  }
  return 1;
}
/********************************************************************/
static int exten, ext_chng;
int del_ext(void)
{
  if( z->num_ext==1 )
  {
    Bconout(2,7);
    return 0;
  }
  z->num_ext--;
  z->ext_rem++;
  memcpy( &z->extension[exten], &z->extension[exten+1],
      (z->num_ext-exten)*sizeof(EXTENSION) );
  ext_chng = 1;
  if( exten > z->num_ext-1 ) exten = z->num_ext-1;
  return 1;
}
int get_extn( OBJECT *o, int i )
{
  EXTN_TYPE e;
  EXTENSION *ex;
  char *ptr1, *ptr2;

  if( !*(ptr2=o[EXTNNAME].ob_spec.tedinfo->te_ptext) )
  {
    del_ext();
    return 0;
  }
  else
  {
    e.c = 0;
    ex = &z->extension[i];
    ext_chng |= strcmp( ptr1=ex->extns+1, ptr2 );
    strcpy( ptr1, ptr2 );
    if( ex->extns[1] )
    {
      ex->extns[0] = '.';
      form = o;
      e.s.tos = is_sel(EXTNTOS);
      e.s.bat = is_sel(EXTNBAT);
      e.s.parm = is_sel(EXTNPARM);
      e.s.npg = is_sel(EXTNNPG);
      e.s.unkn = 1;
    }
    else ex->extns[0] = '\0';
    if( ex->type.c != e.c )
    {
      ex->type = e;
      ext_chng = 1;
    }
    return 1;
  }
}
void redo_extn( OBJECT *o, int i )
{
  EXTN_TYPE e;

  strcpy( o[EXTNNAME].ob_spec.tedinfo->te_ptext, z->extension[i].extns+1 );
  e = z->extension[i].type;
  obj_selec( o, e.s.tos, 1, EXTNTOS );
  obj_selec( o, !e.s.tos && !e.s.bat, 1, EXTNGEM );
  obj_selec( o, e.s.bat, 1, EXTNBAT );
  obj_selec( o, e.s.parm, 1, EXTNPARM );
  obj_selec( o, e.s.npg, 1, EXTNNPG );
}
int i_extpref( OBJECT *o, FORM *f )
{
  redo_extn( o, exten=0 );
  ext_chng = 0;
  return 1;
}
void draw_ext( FORM *f )
{
  int j;
  form_draw( f, EXTNNAME, EXTNNAME );
  for( j=EXTNTOS; j<=EXTNNPG; j++ )
    form_draw( f, j, 0 );
}
int t_extpref( OBJECT *o, int num, FORM *f )
{
  switch( num )
  {
    case EXTNUP:
      if( get_extn( o, exten ) )
	if( --exten < 0 ) exten = z->num_ext-1;
      redo_extn( o, exten );
      break;
    case EXTNDOWN:
      if( get_extn( o, exten ) )
	if( ++exten >= z->num_ext ) exten = 0;
      redo_extn( o, exten );
      break;
  }
  draw_ext(f);
  return 0;
}
int x_extpref( OBJECT *o, int num, FORM *f )
{
  int i;
  
  switch( num )
  {
    case EXTNNEW:
      if( (i=add_extn()) >= 0 )
      {
	ext_chng = 1;
	redo_extn( o, exten=i );
	draw_ext(f);
      }
      return 0;
    case EXTNDEL:
      del_ext();
      redo_extn( o, exten );
      draw_ext(f);
      return 0;
    case EXTNOK:
      if( !get_extn( o, exten ) ) return 0;
    default:
      if( ext_chng )
      {
	modal_unblit( f, 1, 0 );      
	i = w_num;
	for( w_num=0; w_num<7; w_num++ )
	  if( (w_handle = wxref[w_num]) >= 0 )
	  {
	    set_wfile();
	    first(0);
	  }
	w_handle = wxref[w_num=i];
	set_wfile();
	get_d_icon(-1);
	do_desk();
	modal_unblit( f, 0, 0 );
      }
      return 1;
  }
}
/********************************************************************/
int i_pthpref( OBJECT *o, FORM *f )
{
  strcpy( o[PREFBTCH].ob_spec.tedinfo->te_ptext, z->batch_name );
  strcpy( o[PREFAUTO].ob_spec.tedinfo->te_ptext, z->autoexec );
  strcpy( o[PREFTEXT].ob_spec.tedinfo->te_ptext, z->text_reader );
  return 1;
}
int x_pthpref( OBJECT *o, int num, FORM *f )
{
  if( num == PREFPOK )
  {
    strcpy( z->batch_name, o[PREFBTCH].ob_spec.tedinfo->te_ptext );
    strcpy( z->autoexec, o[PREFAUTO].ob_spec.tedinfo->te_ptext );
    strcpy( z->text_reader, o[PREFTEXT].ob_spec.tedinfo->te_ptext );
  }
  return 1;
}
/********************************************************************/
int i_folder( OBJECT *o, FORM *f )
{
  if( w_num<0 ) return 0;
  obj_selec( o, z->open_fold, 1, FOLDOPEN );	/* 003 */
  o[FOLDEDIT].ob_spec.tedinfo->te_ptext[0] = 0;
  return 1;
}
int x_folder( OBJECT *o, int num, FORM *f )
{
  char temp[13], filename[120];

  if( num == FOLDOK )
  {
    form = o;
    z->open_fold = is_sel( FOLDOPEN );		/* 003 */
    if( !test_filename( o[FOLDEDIT].ob_spec.tedinfo->te_ptext,
	temp, 1 ) ) return 0;
    if( temp[0] )
    {
      bee();
      strcpy( filename, z->w[f->wind].path );
      iso(filename);
      strcat( filename, temp );
      if( check_dir(filename,0)>0 && dcreate(filename) )
      {
        if( z->open_fold )			/* 003 */
        {
          strcat( strcpy( tmpf, filename ), slash );
          strcpy( z->w[f->wind].path, tmpf );
          z->w[f->wind].f_off.l = 0L;
          re_name = 1;
        }
	modal_update( f, filename, 0, 0 );
      }
      arrow();
    }
  }
  return 1;
}
/********************************************************************/
#define SHOWGRP 	((GROUP_HDR *)f->mem_ptr)
int i_grpinf( OBJECT *o, FORM *f )
{
  char *ptr;	/* 003 , str2[35]; */
  int buf[8], i;
  GROUP_HDR *gh;
  static char enab[] = { NGRPNAME, NGRPWIND, NGRPDESC, NGRPDESC+1 };

  show_ret = 0;
  memcpy( gh=SHOWGRP, show_grp, sizeof(GROUP_HDR) );
  init_groupinf( o, gh, 1 );
  ptr = o[NGRPITEM].ob_spec.tedinfo->te_ptext;
  if( show_size >= 0 )
  {
    /* txt_cons( show_size, str2 ); 003 */
    spf( ptr, msg_ptr[22], gh->entries, gh->entries!=1 ? msg_ptr[23] : "",
	show_size, /*str2*/ "", show_size!=1L ? msg_ptr[23] : "" );
  }
  else *ptr = 0;
  to_tandd( gh->create, buf );
  tandd_to_str( buf, o[NGRPCRE].ob_spec.free_string );
  to_tandd( gh->modify, buf );
  tandd_to_str( buf, o[NGRPMOD].ob_spec.free_string );
  ptr = spathend(showinf_path);
  if( strlen(ptr)>12 )
  {
    ptr="";
    for( i=0; i<4; i++ )
      o[enab[i]].ob_flags &= ~EDITABLE;
  }
  else
    for( i=0; i<4; i++ )
      o[enab[i]].ob_flags |= EDITABLE;
  strcpy( o[NGRPNAME].ob_spec.tedinfo->te_ptext, ptr );
  set_longedit( o, NGRPDESC, 2 );
  return 1;
}
int x_grpinf( OBJECT *o, int num, FORM *f )
{
  char *ptr, *end;
  GROUP_HDR *gh;
  
  show_ret = 1;
  if( num == NGRPOK )
  {
    ptr = o[NGRPNAME].ob_spec.tedinfo->te_ptext;
    end = spathend(showinf_path);
    gh = SHOWGRP;
    if( !*ptr ) ptr = end;
    else strcpy( end, ptr );
    if( !gh->name[0] ) strcpy( gh->name, ptr );
    strcat( showinf_path, ".GRP" );
    modal_unblit( f, 1, 0 );
    if( (show_ret = show_func ? (*show_func)(gh) : 1) != 0 &&
	show_size>=0 && strcmp( showinf_path, show_path ) )
    {
      bee();
      if( TOS_error( cFrename( 0, show_path, showinf_path ), 0 ) )
	  strcpy( show_path, showinf_path );
      arrow();
    }
    modal_unblit( f, 0, 0 );
  }
  else if( !show_func ) show_ret = 0;
  showinf_ok = -1;
  return 1;
}
/********************************************************************/
int i_foldinf( OBJECT *o, FORM *f )
{
  int buf[8], ret=0;

  bee();
  show_ret = 0;
  if( tree_init( show_path, 0L ) )
  {
    if( !show_date ) to_tandd( ((unsigned long)maintree.tree[0].d_date<<16) |
	maintree.tree[0].d_time, show_date=buf );
    tandd_to_str( show_date, o[FOLDTIME].ob_spec.free_string );
    while( tree_next(0L) != 1 );
    strcpy( o[FOLDNAME].ob_spec.tedinfo->te_ptext, show_name );
    spf( o[FOLDFILE].ob_spec.free_string, nfmt, maintree.files );
    spf( o[FOLDFOLD].ob_spec.free_string, nfmt, maintree.folders );
    spf( o[FOLDHFIL].ob_spec.free_string, nfmt, maintree.f_hidden );
    consump( show_path[0], o[FOLDSIZE].ob_spec.free_string, maintree.bytes_total, -1 );
    consump( show_path[0], o[FOLDHBYT].ob_spec.free_string, maintree.bytes_hidden, -1 );
    if( !ver_gt_12 )
    {
      o[FOLDCANC].ob_flags |= HIDETREE;
      o[FOLDNAME].ob_flags &= ~EDITABLE;
      o[FOLDOK2].ob_x = (o[0].ob_width-o[FOLDOK2].ob_width)>>1;
    }
    ret = 1;
  }
  arrow();
  return ret;
}
int x_foldinf( OBJECT *o, int num, FORM *f )
{
  int i;
  
  show_ret = 1;
  if( num == FOLDOK2 )
  {
    bee();
    i = set_filename( &f, 0, -1, FOLDNAME, show_path, tmpf );
    arrow();
    switch(i)
    {
      case 0:
	show_ret = 2;
	break;
      case -2:
	return 0;
    }
  }
  showinf_ok = -1;
  return 1;
}
/********************************************************************/
static char file_flags, is_prg;
int i_fileinf( OBJECT *o, FORM *f )
{
  int hand, ret=0, buf[8];
  
  bee();
  show_ret = 0;
  if( TOS_error( (long)(hand=cFopen(show_path, 0)), 2 ) )
  {
    strcpy( o[FILEEDIT].ob_spec.tedinfo->te_ptext, show_name );
    hide_if( o, FILEBOX1, (is_prg = cFread( hand, 0x1AL, tmpf )==0x1AL &&
	tmpf[0]=='\x60' && tmpf[1]=='\x1A') != 0 );
    file_flags = tmpf[0x19];
    obj_true1( o, file_flags&1, FILEFLON );
    obj_true1( o, file_flags&2, FILETTLD );
    obj_true1( o, file_flags&4, FILETTAL );
    obj_ltrue( o, (file_flags>>4)&3, 4, FILEPRVT );
    if( !show_date )
    {
      show_parm1 = cFattrib( show_path, 0, 0 )&S_IJRON;
      cFdatime( (DOSTIME *)buf, hand, 0 );
      to_tandd( (unsigned)buf[0] | ((long)buf[1]<<16), show_date=buf );
      show_size = cFseek( 0L, hand, 2 );
    }
    cFclose(hand);
    consump( show_path[0], o[FILELEN].ob_spec.free_string, show_size, -1 );
    tandd_to_str( show_date, o[FILETIME].ob_spec.free_string );
    obj_true1( o, show_parm1 & S_IJRON, FILERO );
    obj_selec( o, 0, 1, FILESET );
    ret = 1;
  }
  arrow();
  return ret;
}
int x_fileinf( OBJECT *o, int num, FORM *f )
{
  int settime, newrw, i, j, buf[2], hand;
  char nflags;
  
  show_ret = 1;
  if( num == FILEOK )
  {
    form = o;
    settime = is_sel(FILESET);
    newrw = (is_sel(FILERO)) ? S_IJRON : 0;
    for( j=0; j<4; j++ )
      if( is_sel(FILEPRVT+j) ) nflags = j<<4;
    if( (is_sel(FILEFLON)) ) nflags |= 1;
    if( (is_sel(FILETTLD)) ) nflags |= 2;
    if( (is_sel(FILETTAL)) ) nflags |= 4;
    if( (i=set_filename( &f, 0, -1, FILEEDIT, show_path, tmpf )) <= 0 )
    {
      if( i==-2 ) return 0;
      if( !i )
      {
	if( settime || is_prg && nflags!=file_flags )
	  if( TOS_error( (long)(hand=cFopen(tmpf,2)), 2 ) )	/* 006: open mode was 0 */
	  {
	    if( is_prg && nflags!=file_flags )
	    {
	      if( cFseek( 0x19L, hand, 0 ) == 0x19L ) TOS_error(
		  cFwrite( hand, 1L, &nflags ), 0 );
	    }
	    if( settime )
	    {
	      buf[0] = Tgettime();
	      buf[1] = Tgetdate();
	      TOS_error( cFdatime( (DOSTIME *)buf, hand, 1 ), 0 );
	    }
	    cFclose(hand);
	  }
	if( show_parm1 != newrw )
	  if( TOS_error( (long)(j=cFattrib( tmpf, 0, 0 )),
	      0 ) ) cFattrib( tmpf, 1, (j&~S_IJRON)|newrw );
      }
      show_ret = 2;
    }
  }
  showinf_ok = -1;
  return 1;
}
/********************************************************************/
void dinf( char *str, int num, int all )
{
  if( all ) spf( str, nfmt, num );
  else str[0]=0;
}
void dinf2( char *str, long num, int all )
{
  if( all ) spf( str, Nfmt, num );
  else str[0]=0;
}
int i_drvinf( OBJECT *o, FORM *f )
{
  Bsec boot;
  unsigned long d_inf[4];
  char name[3];
  int ret=0;

  show_ret = 0;
  bee();
  name[0] = show_parm1;
  name[1] = ':';
  name[2] = '\0';
  if( tree_init(name,0L) )
  {
    if( showinf_all )
      do
        if( _abort() )
        {
          arrow();
          return 0;
        }
      while( tree_next(0L) != 1 );
    if( TOS_error( (long)bootsec( show_parm1, &boot ), 0 ) )
    {
      strcpy( show_name, volname );
      strcpy( o[DISKVOL].ob_spec.tedinfo->te_ptext, show_name );
      cDfree( (DISKINFO *)d_inf, show_parm1-'A'+1 );
      spf( o[DISKSIDE].ob_spec.free_string, ifmt, boot.sides );
      spf( o[DISKTPS].ob_spec.free_string, nfmt, boot.tps );
      spf( o[DISKSPT].ob_spec.free_string, nfmt, boot.spt );
      spf( o[DISKBPS].ob_spec.free_string, nfmt, boot.bps );
      spf( o[DISKCLSZ].ob_spec.free_string, nfmt, boot.clsiz );
      dinf( o[DISKHFIL].ob_spec.free_string, maintree.f_hidden, showinf_all );
      dinf( o[DISKFILE].ob_spec.free_string, maintree.files, showinf_all );
      dinf( o[DISKFOLD].ob_spec.free_string, maintree.folders, showinf_all );
      spf( o[DISKSECS].ob_spec.free_string, Nfmt, boot.secs );
      spf( o[DISKTOTL].ob_spec.free_string, Nfmt, (d_inf[1]-2)*d_inf[2]*d_inf[3] );	/* 003: -2 */
      dinf2( o[DISKHBYT].ob_spec.free_string, maintree.bytes_hidden, showinf_all );
      dinf2( o[DISKUSED].ob_spec.free_string, maintree.bytes_total, showinf_all );
      spf( o[DISKFREE].ob_spec.free_string, Nfmt, d_inf[0]*d_inf[2]*d_inf[3] );
      ret = 1;
    }
  }
  arrow();
  return ret;
}
int x_drvinf( OBJECT *o, int num, FORM *f )
{
  char *ptr;
  
  if( num == DISKOK )
    if( strcmp( ptr=o[DISKVOL].ob_spec.tedinfo->te_ptext, show_name ) )
	new_volname( show_wind, ptr, show_parm1 );
  showinf_ok = -1;
  show_ret = 1;
  return 1;
}
/********************************************************************/
/*%  if( macr )
  {
    strcpy( temp_path, filename );
    strcpy( filename, spathend(filename) );
    iso(temp_path);
    all=0;
  }
  else */
#define APP_TEMP	((char *)f->mem_ptr)
#define APP_FILE	((char *)f->mem_ptr+120)
static int appnum, app_all, app_wnum;
static char atmpf[21];
static PROG_TYPE pt;
static APP_FLAGS af;
void appl_type( OBJECT *o, PROG_TYPE pt, APP_FLAGS af, int off )
{
  obj_selec( o, pt.p.tos, 1, APPLTOS-APPLPRGT+off );
  obj_selec( o, !pt.p.tos && !pt.p.batch, 1, APPLGEM-APPLPRGT+off );
  obj_selec( o, pt.p.batch, 1, APPLBAT-APPLPRGT+off );
  obj_selec( o, pt.p.takes_params, 1, APPLPARM-APPLPRGT+off );
  obj_selec( o, pt.p.npg, 1, APPLNPG-APPLPRGT+off );
  obj_true( o, !af.p.docpath, APPLPPRG-APPLPRGT+off );
  obj_ltrue( o, 2-af.p.reload, 3, APPLRYES-APPLPRGT+off );
}
void init_app( OBJECT *o, FORM *f )
{
  char temp[120];
  
  strcpy( o[APPLNAME].ob_spec.tedinfo->te_ptext, !app_all ? APP_FILE :
      z->apps[appnum].name );
  o[APPLEXTN].ob_spec.tedinfo->te_ptext = atmpf;
  if( z->apps[appnum].type.i )
  {
    pt = z->apps[appnum].type;
    af = z->apps[appnum].flags;
    strcpy( atmpf, z->apps[appnum].extn );
  }
  else
  {
    strcpy( temp, APP_TEMP );
    strcat( temp, APP_FILE );
    pt = prog_type( app_wnum, temp );
    af.i = (*mas->set_caches)(-1) | (1<<4);
    atmpf[0] = '\0';
  }
  appl_type( o, pt, af, APPLPRGT );
}
char app_mac;
int i_applic( OBJECT *o, FORM *f )
{
  int i;
  char *ptr;
  extern char filename[];
  
  app_all = 1;
  w_num = app_wnum = num_w_active;
  set_wfile();
  find_handle();
  if( app_mac )
  {
    app_mac = 0;
    strcpy( APP_TEMP, filename );
    strcpy( APP_FILE, ptr=spathend(APP_TEMP) );
    *ptr = 0;
    app_all=0;
  }
  else if( w_active>=0 )
  {
    get_full_name( APP_TEMP, w_active, num_w_active );
    strcpy( APP_FILE, ptr=spathend(APP_TEMP) );
    *ptr = 0;
    app_all=0;
  }
  else if( d_active>=0 )
  {
    strcpy( APP_TEMP, z->programs[z->idat[d_active].type-D_PROG].p.path );
    strcpy( APP_FILE, spathend(APP_TEMP) );
    app_all=0;
  }
  if( !app_all ) iso(APP_TEMP);
  obj_enab( o, app_all, 1, APPLREMV );
  o[APPLUP].ob_flags = o[APPLDWN].ob_flags = app_all ? TOUCHEXIT :
      HIDETREE;
  for( i=0, appnum=-1; i<z->num_apps && appnum<0; i++ )
    if( z->apps[i].type.i )
      if( app_all ) appnum=i;
      else if( !strcmp( z->apps[i].name, APP_FILE ) &&
	  !strcmp( z->apps[i].path, APP_TEMP ) )
      {
	obj_enab( o, 1, 1, APPLREMV );
	appnum=i;
      }
  de_act_other(1);
  de_act_d(-1);
  if( appnum<0 )
    if( (appnum=add_app())<0 ) return 0;
  init_app( o, f );
  return 1;
}
void draw_app( FORM *f )
{
  int i;
  static char rlist[] = { APPLNAME, APPLEXTN, APPLTOS,
      APPLGEM, APPLBAT, APPLPARM, APPLNPG, APPLPPRG, APPLPDOC, APPLRYES,
      APPLRNO, APPLRDFT };

  for( i=0; i<sizeof(rlist); i++ )
    form_draw( f, rlist[i], i<2 ? APPLEXTN : 0 );
}
void get_app( PROG_TYPE *pt, APP_FLAGS *af, int off )
{
  pt->p.tos = is_sel(APPLTOS-APPLPRGT+off);
  pt->p.batch = is_sel(APPLBAT-APPLPRGT+off);
  pt->p.takes_params = is_sel(APPLPARM-APPLPRGT+off);
  pt->p.npg = is_sel(APPLNPG-APPLPRGT+off);
  af->p.docpath = is_sel(APPLPDOC-APPLPRGT+off);
  af->p.reload = APPLRYES-APPLPRGT+off+2-
      scan_sel(APPLRYES-APPLPRGT+off,APPLRYES-APPLPRGT+off+3);
}
void _get_app( FORM *f )
{
  form = f->tree;
  get_app( &pt, &af, APPLPRGT );
  z->apps[appnum].type = pt;
  z->apps[appnum].flags = af;
  strcpy( z->apps[appnum].extn, atmpf );
  if( !app_all )
  {
    strcpy( z->apps[appnum].name, APP_FILE );
    strcpy( z->apps[appnum].path, APP_TEMP );
  }
}
int t_applic( OBJECT *o, int num, FORM *f )
{
  int i = appnum;
  
  switch( num )
  {
    case APPLUP:
      while( i<z->num_apps && !z->apps[++i].type.i );
      if( i>=z->num_apps )
      {
	i = -1;
	while( i<appnum && !z->apps[++i].type.i );
      }
      break;
    case APPLDWN:
      while( i>=0 && !z->apps[--i].type.i );
      if( i<0 )
      {
	i = z->num_apps;
	while( i>appnum && !z->apps[--i].type.i );
      }
  }
  if( i!=appnum )
  {
    _get_app(f);
    appnum = i;
    init_app( o, f );
    draw_app(f);
  }
  return 0;
}
int x_applic( OBJECT *o, int num, FORM *f )
{
  switch( num )
  {
    case APPLINST:
#ifdef DEMO
      demo_version();
#else DEMO
      form = o;
      _get_app(f);
#endif DEMO
      break;
    case APPLREMV:
      z->apps[appnum].type.i = 0;
      z->apps[appnum].name[0] = z->apps[appnum].path[0] = '^';
      break;
  }
  return 1;
}
/********************************************************************/
#define NPI_PATH	((char *)f->mem_ptr)
#define NPI_TEMP	((char *)f->mem_ptr+120)
#define NPI_NPI 	((NPI_TYPE *)((char *)f->mem_ptr+120*2))
#define NPI_NAME	(NPI_NPI->name)
char catt[][3] = { "16", "32", "48", "56", "64" },
  cattnum[] = { _CaTT_s16_mhz, _CaTT_s32_mhz, _CaTT_s48_mhz, _CaTT_s56_mhz, _CaTT_s64_mhz },
  cattmodes[] = { 16, 32, 48, 56, 64 };
int npi_catt;
void npi_prog( OBJECT *o, FORM *f )
{
  int i=o[NPIPATH].ob_width/6;

  short_path( NPI_NPI->path[0] ? NPI_NPI->path : msg_ptr[127], NPI_TEMP, i, i );
  o[NPIPATH].ob_spec.tedinfo->te_ptext = NPI_TEMP;	/* 003: now BOXTEXT */
  o[NPINAME].ob_spec.tedinfo->te_ptext = NPI_NPI->name;
}
void disp_catt( OBJECT *o, FORM *f, int draw )
{
  NPI_NPI->af.p.CaTT = npi_catt+1;
  obj_enab( o, cattmodes[npi_catt], 1, NPICATT+1 );
  o[NPICATT+1].ob_spec.free_string = catt[npi_catt];
  if( draw ) form_draw( f, NPICATT+1, NPINAME );
}
void npi_set( OBJECT *o, FORM *f )
{
  int i;
  APP_FLAGS ap;
  
  npi_prog( o, f );
  appl_type( o, NPI_NPI->pt, af=NPI_NPI->af, NPIPRGT );
  obj_selec( o, af.p.clock, 1, NPICLK );
  if( (i=af.p.MSTe_16M*2|af.p.MSTe_cache) != 0 ) i--;
  obj_ltrue( o, i, 3, NPIM8HZ );
  obj_selec( o, af.p.TT_cache, 1, NPITCHC );
  obj_selec( o, af.p.Blitter, 1, NPIBLIT );
  obj_selec( o, af.p.singletask, 1, NPISING );
  disp_catt( o, f, 0 );
  for( i=5; --i>=0; )
    o[NPIPARM0+i].ob_spec.tedinfo->te_ptext = NPI_NPI->params[i];
  obj_true1( o, NPI_NPI->use_argv, NPIARGV );
  ttp_mode( o, NPI_NPI->use_argv, NPIPARM0 );
  o[NPINAME].ob_spec.tedinfo->te_ptext = NPI_NPI->name;
}
int read_npi( NPI_TYPE *npi, char *path )
{
  int h, l, ret=0;
  
  h = cFopen(path,0);
  if( h==AEFILNF || h==AEPTHNF ) missing_file(path);	/* 004 */
  else if( TOS_error( h, 0 ) )
  {
    if( TOS_error( l=cFread( h, sizeof(NPI_TYPE), npi ), 0 ) &&
	l == sizeof(NPI_TYPE) && npi->magic==NPI_MAGIC && npi->ver==NPI_VER ) ret = 1;
    cFclose(h);
  }
  return ret;
}
static int npi_num, npi_chk;
static MENU npi_menu;
int npi_opt( OBJECT *o, int num )
{
  int i;
  static char opts[] = { NPIMAIN, NPIPARMS, NPIMISC, -1 };

  npi_menu.mn_tree = popups;
  npi_menu.mn_menu = PNPI0-1;
  npi_menu.mn_item = num+PNPI0;
  o[NPIOPT].ob_spec.free_string = popups[PNPI0 + (npi_num = num)].ob_spec.free_string;
  if( opts[num]<0 ) return 0;
  for( i=0; i<3; i++ )
    hide_if( o, opts[i], i==num );
  *(long *)&o[opts[num]].ob_x = *(long *)&o[NPIMAIN].ob_x;
  return opts[npi_num];
}
int chk_npi( NPI_TYPE *npi )
{
  extern int _chknum;
  int sum=0, i;
  
  _chknum = 0;
  _chksmstr( npi->path, &sum );
  _chksmstr( npi->name, &sum );
  for( i=5; --i>=0; )
    _chksmstr( npi->params[i], &sum );
  _chksm( (char *)&npi->use_argv, &sum, 1 );
  _chksm( (char *)&npi->af, &sum, sizeof(APP_FLAGS)+sizeof(PROG_TYPE) );
  _chksm( npi->env, &sum, sizeof(npi->env) );
  return sum;
}
void new_npi( FORM *f )
{
  NPI_PATH[0] = 0;
  memclr( NPI_NPI, sizeof(NPI_TYPE) );
  NPI_NPI->af.i = (*mas->set_caches)(-1) | (1<<12);
  NPI_NPI->pt.p.set_me = NPI_NPI->pt.p.show_status = NPI_NPI->pt.p.clear_screen = 1;
  memcpy( NPI_NPI->env, z->env, sizeof(z->env) );
  strcpy( NPI_NPI->params[0], "$$" );
  npi_chk = chk_npi( NPI_NPI );
}
int i_npi( OBJECT *o, FORM *f )
{
  int i, j, fast=-1;
  NPI_DESC npi;
  CaTT_GIOS cg;		/* 003 */
  
  if( in_showinf ) show_ret = 0;
  w_num = num_w_active;
  set_wfile();
  find_handle();
  if( w_active>=0 ) get_full_name( NPI_PATH, w_active, num_w_active );
  else if( d_active>=0 ) strcpy( NPI_PATH, z->programs[z->idat[d_active].type-D_PROG].p.path );
  else NPI_PATH[0] = 0;
  if( !NPI_PATH[0] || !read_npi( NPI_NPI, NPI_PATH ) ) new_npi(f);
  else npi_chk = chk_npi( NPI_NPI );
  if( !Init_CaTT_GIOS() ) CaTT_inquire( &cg, sizeof(cg) );
  else cg.max_function_code = 0;
  for( i=0; i<sizeof(cattnum); i++ )
    if( cattnum[i] > cg.max_function_code ) cattmodes[i] = 0;
    else for( j=0; ; j++ )
      if( j==8 || !cg.catt_modes_table[j] )
      {
        cattmodes[i] = 0;
        break;
      }
      else if( cg.catt_modes_table[j]==cattmodes[i] )
      {
        fast = i;
        break;
      }
  if( !NPI_NPI->af.p.CaTT && fast>0 ) NPI_NPI->af.p.CaTT = fast+1;
  if( (npi_catt = NPI_NPI->af.p.CaTT-1) < 0 ) npi_catt = 2;	/* 48 MHz by default */
  npi_set( o, f );
  npi_opt( o, 0 );
  if( !in_showinf ) de_act( -1, -1 );
  info();
  return 1;
}
void new_npi_opt( OBJECT *o, FORM *f, int num )
{
  int x, old;
  
  old = npi_num;
  x = npi_opt( o, num );
  form_draw( f, NPIOPT, 0 );
  if( !x )
  {
    env_ptr = NPI_NPI->env;
    env_parent = &NPI_NPI->use_parent;
    start_sform( NENV_FORM, &f );
    env_ptr = 0L;
    x = npi_opt( o, old );
    form_draw( f, NPIOPT, 0 );
  }
  form_draw( f, x, NPINAME );
}
int t_npi( OBJECT *o, int num, FORM *f )
{
  int x, y;
  
  switch(num)
  {
    case NPIOPT:
      objc_offset( o, NPIOPT, &x, &y );
      if( menu_popup( &npi_menu, x-2, y-2, &npi_menu ) && npi_menu.mn_item!=npi_num+PNPI0 )
  	  new_npi_opt( o, f, npi_menu.mn_item-PNPI0 );
      break;
    case NPIARGV:
      ttp_mode( o, NPI_NPI->use_argv=o[NPIARGV].ob_state&SELECTED, NPIPARM0 );
      form_draw( f, NPIPARMS, NPINAME );
      break;
    case NPICATT:
      if( npi_catt>0 )
      {
        npi_catt--;
        disp_catt( o, f, 1 );
      }
      break;
    case NPICATT+2:
      if( npi_catt<sizeof(cattnum)-1 )
      {
        npi_catt++;
        disp_catt( o, f, 1 );
      }
      break;
  }
  return 0;
}
void dflt_npiname( char *name, char *path )
{
  strcpy( name, spathend(path) );
  strcpy( name+find_extn(name), ".NPI" );
}
int save_npi( OBJECT *o, NPI_TYPE *npi, int chk, char *path )
{
  int newchk, h, i;
  char temp[120];
  APP_FLAGS *af;

  if( !npi->name[0] )
    if( chk<0 ) return chk;
    else dflt_npiname( npi->name, path );
  form = o;
  get_app( &npi->pt, af=&npi->af, NPIPRGT );
  af->p.clock = is_sel(NPICLK);
  af->p.MSTe_16M = is_sel(NPIM8HZ+1) || is_sel(NPIM8HZ+2);
  af->p.MSTe_cache = is_sel(NPIM8HZ+2);
  af->p.TT_cache = is_sel(NPITCHC);
  af->p.Blitter = is_sel(NPIBLIT);
  af->p.singletask = is_sel(NPISING);
  newchk = chk_npi(npi);
  if( chk )
    if( newchk == npi_chk ) return chk;
    else switch( f_alert1( msg_ptr[145] ) )
    {
      case 2:
	return chk;
      case 3:
	return 0;
    }
  if( !path[0] )
  {
    strcpy( temp, z->dflt_path );
    if( !fselect( temp, "*.NPI", 0L, msg_ptr[144] ) ) return 0;
    strcpy( path, temp );
  }
  bee();
  if( TOS_error( h=prep_save(path), 0 ) )
  {
    npi->magic = NPI_MAGIC;
    npi->ver = NPI_VER;
    if( !TOS_error( i=cFwrite(h,sizeof(NPI_TYPE),npi), 0 ) ||
	i!=sizeof(NPI_TYPE) )
    {
      cFclose(h);
      cFdelete(path);
      arrow();
      f_alert1( msg_ptr[15] );
      return 0;
    }
    npi_chk = newchk;
    cFclose(h);
    arrow();
    return 1;
  }
  else
  {
    arrow();
    spf( temp, msg_ptr[36], pathend(path), "" );
    f_alert1( temp );
    return 0;
  }
}
int x_npi( OBJECT *o, int num, FORM *f )
{
  char temp[120], exts[50];
  int i;
  EXTENSION *e;
  
  if( in_showinf ) show_ret = 1;
  switch( num )
  {
    case NPIPROG:
      strcpy( temp, NPI_NPI->path );
      if( (i=z->num_ext) > 10 ) strcpy( exts, "*.*" );
      else
      {
	strcpy( exts, "*.{" );
	for( e=z->extension; --i>=0; e++ )
	{
	  strcat( exts, e->extns+1 );
	  if(i) strcat( exts, "," );
	}
	strcat( exts, "}" );
      }
      if( rfselect( f, temp, "*.*", exts, msg_ptr[143] ) )
      {
	strcpy( NPI_NPI->path, temp );
	dflt_npiname( NPI_NAME, NPI_NPI->path );
	NPI_NPI->pt = iprog_type( -1, NPI_NPI->path );
	npi_set( o, f );
	if( f->handle>0 ) _form_path( f, NPIPATH );
	else form_draw( f, NPIPATH, NPINAME );
	form_draw( f, NPINAME, NPINAME );
	new_npi_opt( o, f, 0 );
      }
      return 0;
    case NPINEW:
      if( !save_npi( o, NPI_NPI, 1, NPI_PATH ) ) return 0;
      new_npi(f);
      npi_set( o, f );
      npi_opt( o, 0 );
      form_draw( f, 0, NPINAME );
      return 0;
    case -1:
      if( in_showinf ) showinf_ok = -1;
      return 1;
    default:
      switch( save_npi( o, NPI_NPI, num==NPISAVE ? 0 : -1, NPI_PATH ) )
      {
	case 0:
	  return 0;
	case -1:
	  if( in_showinf ) showinf_ok = -1;
	  return 1;
      }
      update_npi( NPI_NPI, NPI_PATH );
      modal_update( f, NPI_PATH, 0, 0 );
  }
  if( in_showinf ) showinf_ok = -1;
  return 1;
}
/********************************************************************/
int i_dialpref( OBJECT *o, FORM *f )
{
  obj_true1( o, z->dial_in_wind, DPRWIND );
  obj_ltrue( o, z->dial_mode, 3, DPRLAST );
  hide_if( o, DPRBUT, gui->xtern.x_settings!=0L /* 004: was !has_Geneva */ );
  return 1;
}
int x_dialpref( OBJECT *o, int num, FORM *f )
{
  if( num == DPROK )
  {
    form = o;
    z->dial_in_wind = is_sel(DPRWIND);
    z->dial_mode = scan_sel( DPRLAST, DPRLAST+3 ) - DPRLAST;
  }
  else if( num==DPRBUT )
  {
    start_set(1);
    return 0;
  }
  return 1;
}
/********************************************************************/
#define fcopy	(((DISKOP *)f->mem_ptr)->copy)
#define fsides	(((DISKOP *)f->mem_ptr)->sides)
#define fback	(((DISKOP *)f->mem_ptr)->back)
#define cside	(((DISKOP *)f->mem_ptr)->cside)
#define dside	(((DISKOP *)f->mem_ptr)->dside)
#define ferr	(((DISKOP *)f->mem_ptr)->err)
#define fpause	(((DISKOP *)f->mem_ptr)->paused)
#define src_ok	(((DISKOP *)f->mem_ptr)->src_ok)
#define rwf	(((DISKOP *)f->mem_ptr)->rwf)
#define fform	(((DISKOP *)f->mem_ptr)->form_num)
#define fscnum	(((DISKOP *)f->mem_ptr)->scnum)
#define fspt	(((DISKOP *)f->mem_ptr)->spt)
#define fbps	(((DISKOP *)f->mem_ptr)->bps)
#define ftracks (((DISKOP *)f->mem_ptr)->tracks)
#define fslice	(((DISKOP *)f->mem_ptr)->slice)
#define faltslice (((DISKOP *)f->mem_ptr)->altslice)
#define faltcnt (((DISKOP *)f->mem_ptr)->altcnt)
#define ftimes	(((DISKOP *)f->mem_ptr)->times)
#define ftimes0 (((DISKOP *)f->mem_ptr)->times0)
#define fblksiz (((DISKOP *)f->mem_ptr)->blksiz)
#define ftwst	(((DISKOP *)f->mem_ptr)->twst)
#define fspc	(((DISKOP *)f->mem_ptr)->spc)
#define fdens	(((DISKOP *)f->mem_ptr)->dens)
#define sdrv	(((DISKOP *)f->mem_ptr)->sdrv)
#define ddrv	(((DISKOP *)f->mem_ptr)->ddrv)
#define ctrack	(((DISKOP *)f->mem_ptr)->ctrack)
#define dtrack	(((DISKOP *)f->mem_ptr)->dtrack)
#define fptr	(((DISKOP *)f->mem_ptr)->ptr)
#define fptr0	(((DISKOP *)f->mem_ptr)->ptr0)
#define ffmt	(((DISKOP *)f->mem_ptr)->fmt)
#define ffunc	(((DISKOP *)f->mem_ptr)->func)
#define fmemory (((DISKOP *)f->mem_ptr)->memory)
#define fmem_ptr (((DISKOP *)f->mem_ptr)->mem_ptr)
#define ffirst  (((DISKOP *)f->mem_ptr)->first)
#define flocked (f->flags.locked)
DISKOP *op_stat;
int opbar( OBJECT *o, int num, unsigned int total, unsigned int curr )
{
  int *w, new, ret;
  
  if( (new = (long)(total-curr)*o[DOPBAR1].ob_width/total) !=
      *(w = &o[num?DOPBAR2+2:DOPBAR1+2].ob_width) )
  {
    *w = new;
    return 1;
  }
  return 0;
}
int calc_fat( FORM *f, unsigned int *total )
{
  *total = fspt*ftracks*fsides;
  /* (total-boot-rootdir-2x)/spc-reservcl */
  if( fspc[fdens]==2 )
  {
    /* (total-1-7-2x)/2-2 = 512x*8/12 */
    return (3 * *total - 36 + 2053) / 2054;
  }
  else		/* 004 */
  {
    /* (total-1-7-2x)/2-2 = 512x*8/12 */
    return (3 * *total - 30 + 1029) / 1030;
  }
}
int stat_format( FORM *f, OBJECT *o, int draw )
{
  int i, j, err=0, barch;
  unsigned int k, total;
  register char *ptr;
  extern char boot_inf[];
  extern int scnum;
  
  ptr = fmem_ptr;
  barch = opbar( o, 0, ftracks, ctrack );
  form = o;
  translate( 77, (long)(ftwst>1 ? 2 : 2+ftwst), 0, 0, 0 );
  o[DOPDRIV].ob_spec.free_string[0] = sdrv;
  o[DOPSIDE].ob_spec.free_string[0] = cside+'1';
  spf( o[DOPNUM].ob_spec.free_string, "%2d", ctrack );
  if( draw )
  {
    form_draw( f, DOPTYPE-1, 0 );
    if( barch ) form_draw( f, DOPBAR1+2, 0 );
    scnum = fscnum;
    err = format_sec( ptr, sdrv, fspt, ctrack, cside, ftwst );
    fscnum = scnum;
    if( --cside<0 )
    {
      cside = fsides-1;
      if( --ctrack<0 && !err )
      {
	memclr( ptr, fspt<<9 );
	if( TOS_error( (long)Flopwr( ptr, "", sdrv-'A',
	    1, 2-fsides, fsides-1, fspt ), 0 ) )
	{
	  memcpy( ptr, boot_inf, 24 );
	  *(ptr+26) = fsides;
	  *(ptr+24) = fspt;
	  i = *(ptr+22) = calc_fat( f, &total );
	  *(ptr+20) = total>>8;
	  *(ptr+19) = total;
	  *(ptr+13) = fspc[fdens];
	  if( fdens ) *(ptr+21) = 0xF0;	/* HD media desc */
	  *(long *)(ptr+512) = *(long *)(ptr+((i+1L)<<9)) =	/* 003 */
	     ((long)*(ptr+21)<<24L) | 0xFFFF00L;
	  for( i=8; i<11; i++ )
	    *(ptr+i) = Random();
	  for( i=k=0; i<14; i++ )
	    k += *((int *)ptr+i);
	  k = 0x1235-k;
	  *(ptr+510) = k>>8;
	  *(ptr+511) = k;
	  err = TOS_error( (long)Flopwr( ptr, "", sdrv-'A', 1, 0,
	      0, fspt ), 0 ) ? -1 : 1;
	}
      }
    }
  }
  return err;
}
void disp_fmt( OBJECT *o, FORM *f, int draw )	/* 004 */
{
  static char strs[3][2][4] = { { "[9", "1[0" }, { "18", "2[0" }, { "2[7", "[30" } };
  int i;
  unsigned int total;

  form = o;
  i = scan_sel( FLFMTSD, FLFMTSD+3 )-FLFMTSD;
  if( !draw || i!=fdens )
  {
    o[FLFMTS9].ob_spec.free_string = strs[i][0];
    o[FLFMTS9+1].ob_spec.free_string = strs[i][1];
    if( draw ) form_draw( f, FLFMTS9-1, 0 );
    if( !draw || fspc[i]!=fspc[fdens] )
    {
      obj_true( o, 2-fspc[i], FLFMTSPC );
      if( draw ) form_draw( f, FLFMTSPC-1, 0 );
    }
    fdens = i;
  }
  fspc[fdens] = is_sel(FLFMTSPC) ? 1 : 2;
  fspt = (fdens+1) * (is_sel(FLFMTS9) ? 9 : 10);
  fsides = is_sel(FLFMTS1) ? 1 : 2;
  ftracks = scan_sel( FLFMT79, FLFMT79+4 )-FLFMT79+79;
  if( ftracks==79 ) ftracks=40;
  i = calc_fat( f, &total );
  /* (total-boot-rootdir-2x)/spc-reservcl */
  spf( o[FLFMTUS1].ob_spec.tedinfo->te_ptext, Nfmt,
      (long)(((total-1-7-(i<<1))/fspc[fdens]-2)*fspc[fdens])<<9L );
  spf( o[FLFMTUS2].ob_spec.tedinfo->te_ptext, Nfmt, (long)total<<9L );
  if( draw ) form_draw( f, FLFMTUS1-1, 0 );
}
int i_format( OBJECT *o, FORM *f )
{
  int i;
  extern char ver_gt_10;

  sdrv = z->format_drive+'A';
  if( d_active>=0 && d_active<MANY_ACTIVE )
    if( (i=get_drive( d_active+1 )) == 0 ) return 0;
    else if( i<'C' ) sdrv = i;
  if( !ver_gt_10 )
  {
    if( z->twst==1 ) z->twst=0;
    obj_enab( o, 0, 1, FLFMTTW );
  }
  if( fdc_level<2 )
  {
    obj_enab( o, 0, 1, FLFMTXD );
    if( z->spt > 20 ) z->spt >>= 1;
    if( !fdc_level )
    {
      obj_enab( o, 0, 1, FLFMTHD );
      if( z->spt > 10 ) z->spt >>= 1;
    }
  }
  obj_true( o, 1-(sdrv-'A'), FLFMTA );
  obj_ltrue( o, z->tracks==40?0:z->tracks-79, 4, FLFMT79 );
  obj_true( o, z->sides==1, FLFMTS1 );
  i = 0;
  if( (fspt = z->spt) > 10 )
  {
    i++;
    if( fspt > 20 ) i++;
  }
  memcpy( fspc, z->spc, sizeof(z->spc) );
  obj_true( o, !(fspt%9), FLFMTS9 );
  obj_ltrue( o, z->twst, 4, FLFMTTNO );
  obj_ltrue( o, fdens=i, 3, FLFMTSD );
  disp_fmt( o, f, 0 );	/* must be after previous */
  if( !hide_if( o, FLFMTBAK, f->handle>0 ) ) z->format_bak = 0;
  obj_true1( o, z->format_bak, FLFMTBAK );
  return 1;
}
int t_format( OBJECT *o, int num, FORM *f )	/* 004 */
{
  disp_fmt( o, f, 1 );
  return 0;
}
int x_format( OBJECT *o, int num, FORM *f )
{
  int i;
  char temp[120];

  form = o;
  z->sides = fsides;
  z->tracks = ftracks;
  z->spt = fspt;
  sdrv = (z->format_drive = is_sel(FLFMTA+1)) + 'A';
  z->twst = ftwst = scan_sel(FLFMTTNO,FLFMT11+2)-FLFMTTNO;
  z->format_bak = fback = is_sel(FLFMTBAK);
  memcpy( z->spc, fspc, sizeof(z->spc) );
  if( num==FLFMTOK )
  {
    if( is_locked(sdrv) )
    {
      TOS_error( LOCK_ERR, 0 );
      return 0;
    }
    spf( temp, msg_ptr[28], sdrv );
    if( f_alert1(temp) == 1 )
    {
      fform = FMT_FORM;
      ftwst = auto_twst( ftwst, fspt );
      ffunc = stat_format;
      fcopy = 0;
      ctrack = ftracks-1;
      cside = fsides-1;
      fscnum = 0;
      fmemory = fspt*1024L;
      fslice = z->back_speed<<1;
      op_stat = (DISKOP *)f->mem_ptr;
      w_num = w_handle = -1;
      start_sform( OPST_FORM, &f );
      if( f->handle<=0 ) modal_update( f, &sdrv, 1, 0 );
      return 0;
    }
  }
  return 1;
}
/********************************************************************/
void lock_diskop( FORM *f )
{
  int lock;
  
  lock = !fback && !fpause;
  if( flocked != lock && f->handle>0 ) wind_lock( flocked=lock );
  lock = fback && !fpause;
  lock_drive( sdrv-'A', f->handle, lock );
  if( fcopy ) lock_drive( ddrv-'A', f->handle, lock );
}
void sec_track( FORM *f, int track, int draw )
{
  int i;
  OBJECT *o = f->tree;
  
  o[DOPSECT].ob_y = o[DOPTRACK].ob_y;
  i = (o[DOPSECT].ob_flags & HIDETREE) != 0;
  if( hide_if( o, DOPSECT, !hide_if( o, DOPTRACK, track ) ) != i && draw )
      form_draw( f, i?DOPSECT:DOPTRACK, 0 );
}
int i_diskop( OBJECT *o, FORM *f )
{
  memcpy( f->mem_ptr, op_stat, sizeof(DISKOP) );
  if( fmemory && (fmem_ptr = lalloc( fmemory, -1 )) == 0 ) return 0;
  if( !fcopy ) sec_track( f, 1, 0 );
  hide_if( o, DOPBAR1+1, hide_if( o, DOPBAR2, fcopy ) );
  if( !hide_if( o, DOPBAK, f->handle>0 ) ) fback = 0;
  obj_true1( o, fback, DOPBAK );
  obj_true1( o, 0, DOPPAUSE );
  ferr = fpause = flocked = 0;
  if( has_Geneva ) lock_diskop(f);
  else ffirst = 1;
  (*ffunc)( f, o, 0 );
  return 1;
}
int t_diskop( OBJECT *o, int num, FORM *f )
{
  switch( num )
  {
    case DOPBAK:
      fback = o[DOPBAK].ob_state&SELECTED;
      lock_diskop(f);
      break;
    case DOPPAUSE:
      if( (fpause = o[DOPPAUSE].ob_state&SELECTED) == 0 &&
	  f_alert1( msg_ptr[147] ) == 2 ) ferr=1;
      lock_diskop(f);
  }
  return 0;
}
void cancel_diskop( int err, FORM *f, int close )
{
  static char temp[] = "x:\\";
  int i;

  if( fcopy )
  {
    copy_free();
    copy_ok = 1;
  }
  if( fmemory ) cmfree( (char **)&fmem_ptr );	/* 004 */
  if( err>0 ) reformat_msg();
  temp[0] = fcopy ? ddrv : sdrv;
  if( f->handle>0 ) 
  {
    i = fform;	/* set now, because close_fwind frees f */
    if( close ) close_fwind( f, 0, 1 );
    else lock_drive( 0, f->handle, 0 );
    update_drive( temp, 0 );
    if(i) start_sform( i, &f );
  }
  if( err==-1 && !in_showinf ) m_showinf( temp[0], 0 );
}
int u_diskop( OBJECT *o, FORM *f )
{
  int i, err, msg[8], x, y, b, dum, slice=0;
  EMULTI e;

  if( ffirst )
  {
    /*if( !has_Geneva ) 003: not needed*/ form_draw( f, 0, 0 );
    lock_diskop(f);
    ffirst = 0;
  }
  e.type = X_MU_DIALOG;
  if( !fpause ) do
  {
    if( (err=ferr) != 0 )
    {
      cancel_diskop( err, f, 1 );
      return 0;
    }
    if( !fback )
    {
      graf_mkstate( &x, &y, &b, &dum );
      if( b&1 && (i=objc_find( o, 0, 8, x, y )) > 0 &&
	  o[i].ob_flags&(EXIT|TOUCHEXIT) &&	/* 003 */
	  (f->handle>0 || !form_button( o, i, 1, &dum )) )
      {
	if( f->handle>0 ) multi_evnt( &e, msg );
	if( (char)(o[i].ob_type>>8)!=X_HELP )
	  if( i==DOPCANC )
	  {
	    o[DOPCANC].ob_state &= ~SELECTED;
            cancel_diskop( 1, f, 1 );
            return 0;
	  }
	  else
	  {
	    t_diskop( o, i, f );
	    if( fpause ) break;
	  }
      }
    }
    if( (err = (*ffunc)( f, o, 1 )) != 0 )
    {
      cancel_diskop( err, f, 1 );
      return 0;
    }
  }
  while( !fpause && (!fback || slice++<fslice) );
  return 1;
}
int x_diskop( OBJECT *o, int num, FORM *f )
{ /* let close_fwind() clear locks */
  cancel_diskop( 1, f, 0 );
  return 1;
}
/********************************************************************/
#define ENV_PARENT (((ENV_TYPE *)f->mem_ptr)->parent)
#define ENV_TPAR   (((ENV_TYPE *)f->mem_ptr)->temp_parent)
#define ENV_ENV    (((ENV_TYPE *)f->mem_ptr)->env)
void restore_environ( FORM *f )
{
  register int i;
  register char *ptr, *ptr2;

  i=ENVSTR0;
  ptr2 = ENV_ENV;
  while( *ptr2 || *(ptr2+1) )
  {
    ptr = f->tree[i].ob_spec.tedinfo->te_ptext;
    if(i>ENVSTR0) ptr2++;
    while( (*ptr++ = *ptr2++) != 0 );
    ptr2--;
    i++;
  }
  for( ; i<ENVSTR0+10; i++ )
    f->tree[i].ob_spec.tedinfo->te_ptext[0] = '\0';
}
char small_env;
void set_envparent( FORM *f )
{
  int i;
  unsigned int *fl;
  
  for( i=ENVSTR0; i<ENVSTR0+10; i++ )
  {
    obj_enab( f->tree, !ENV_TPAR, 1, i );
    fl = &f->tree[i].ob_flags;
    if( !small_env )
    {
      if( ENV_TPAR ) *fl &= ~EDITABLE;
      else *fl |= EDITABLE;
      *fl &= ~(SELECTABLE|EXIT);
    }
    else
    {
      *fl &= ~EDITABLE;
      *fl |= SELECTABLE|EXIT;
    }
  }
  obj_true1( f->tree, ENV_TPAR, ENVPARNT );
}
int i_editenv( OBJECT *o, FORM *f )
{
  ENV_ENV = !env_ptr ? z->env : env_ptr;
  ENV_TPAR = *(ENV_PARENT = !env_ptr ? &z->env_parent : env_parent);
  /* must be before set_envparent() 002 */
  hide_if( o, ENVCLICK, small_env = o[0].ob_width > z->maximum.w );
  restore_environ(f);
  set_envparent(f);
  return 1;
}
int t_editenv( OBJECT *o, int num, FORM *f )
{
  ENV_TPAR = o[ENVPARNT].ob_state&SELECTED;
  set_envparent(f);
  form_draw( f, ENVBOX, ENVSTR0 );
  return 0;
}
int x_editenv( OBJECT *o, int num, FORM *f )
{
  register int i;
  register char *ptr, *ptr2;

  if( num>=ENVSTR0 && num<ENVSTR0+10 )
  {
    env_edit = o[num].ob_spec.tedinfo->te_ptext;
    if( start_sform( ENVE_FORM, &f ) == ENVEOK ) form_draw( f, num, 0 );
    return 0;
  }
  else if( num == ENVOK )
  {
    *ENV_PARENT = ENV_TPAR;
    memclr( ptr2=ENV_ENV, 620 );
    for( i=ENVSTR0; i<ENVSTR0+10; i++ )
      if( *(ptr = o[i].ob_spec.tedinfo->te_ptext) )
	do
	  *ptr2++ = *ptr;
	while( *ptr++ );
  }
  restore_environ(f);	       /* so blanks get eaten */
  return 1;
}
int i_ededit( OBJECT *o, FORM *f )
{
  char *ptr;
  
  strncpy( o[ENVESTR].ob_spec.tedinfo->te_ptext, env_edit, 33 );
  ptr = o[ENVESTR+1].ob_spec.tedinfo->te_ptext;
  if( strlen(env_edit)>33 ) strcpy( ptr, env_edit+33 );
  else *ptr = 0;
  set_longedit( o, ENVESTR, 2 );
  return 1;
}
int x_ededit( OBJECT *o, int num, FORM *f )
{
  if( num==ENVEOK )
  {
    strcpy( env_edit, o[ENVESTR].ob_spec.tedinfo->te_ptext );
    strcat( env_edit, o[ENVESTR+1].ob_spec.tedinfo->te_ptext );
  }
  return 1;
}
/********************************************************************/
#define renf_count  (((RENAME *)f->mem_ptr)->count)
#define renf_icon   (((RENAME *)f->mem_ptr)->icon)
#define renf_update (((RENAME *)f->mem_ptr)->update)
int i_rename( OBJECT *o, FORM *f )
{
  SEL_ICON *s;
  char is_disk;
  int type;
  
  is_disk = ed_wind_type(f->wind)==EDW_DISK;
  if( !f->icons.icons ) renf_update = 0;	/* first time only */
  while( (s=get_sel_icon( &f->icons, 1 )) != 0 )
    if( ((type=s->u.fs->type.p.pexec_mode)>=D_PROG && !is_disk || is_disk) )
      if( !ver_gt_12 && type == FOLDER ) f_alert1( msg_ptr[66] );
      else break;
  if( !s ) return 0;
  to_filename( (renf_icon=s)->u.fs->name, o[NAMEEDIT].ob_spec.tedinfo->te_ptext );
  return 1;
}
int x_rename( OBJECT *o, int num, FORM *f )
{
  int i, on, ret=1, cont=1;
  char temp1[120], temp2[120];
  
  if( num==NAMEOK )
  {
    on = w_num;
    w_num = f->wind;
    set_wfile();
    i = set_filename( &f, 0, renf_icon->u.fs-wfile, NAMEEDIT, temp1, temp2 );
    w_num = on;
    set_wfile();
    if( i<0 )
      if( i==-2 ) ret = 0;
      else
      {
	cont = 0;
	renf_update=1;
      }
    else if( !i ) renf_update = 1;
  }
  else if( num==NAMEQUIT || num<0 )
  {
    if( num<0 ) renf_update = 0;
    cont = 0;
  }
  if( cont )
  {	/* 003: reworked to unblit first */
    modal_unblit( f, 1, 0 );
    i = i_rename( o, f );
    modal_unblit( f, 0, i );
    if( f->handle>0 ) form_draw( f, 0, NAMEEDIT );	/* 003 */
    if(i) return 0;
  }
  if( renf_update )
    if( ed_wind_type(f->wind)!=EDW_DISK ) get_all_icons();
    else modal_update( f, z->w[f->wind].path, 0, 1 );
  return ret;
}
/********************************************************************/
int i_prndir( OBJECT *o, FORM *f )
{
  obj_selec( o, z->dir_prn.u.size, 1, PRNSIZ );
  obj_selec( o, z->dir_prn.u.date, 1, PRNDATE );
  obj_selec( o, z->dir_prn.u.time, 1, PRNTIME );
  obj_selec( o, z->dir_prn.u.fold, 1, PRNFOLD );
  return 1;
}
int x_prndir( OBJECT *o, int num, FORM *f )
{
  int err=0, oi;
  DPRN_TYPE dp;
  char temp[125];
  extern int prn_dev;
  extern char filename[];

  prn_dev = 0;
  switch( num )
  {
    case PRNFILE:
      if( !rfselect(f,0L,glob,0L,msg_ptr[129]) || !TOS_error( prn_dev=prep_save(filename), 0 ) ) return 1;
      strcpy( temp, filename );
    case PRNPRN:
#ifndef DEMO
      read_q_set();
      dp.c = 0;
      form = o;
      dp.u.size = is_sel(PRNSIZ);
      dp.u.date = is_sel(PRNDATE);
      dp.u.time = is_sel(PRNTIME);
      dp.u.fold = is_sel(PRNFOLD);
      z->dir_prn.c = dp.c;
      if( prn_dev || !check_prn() )
      {
	w_num = f->wind;
	set_wfile();
	find_handle();
	strcpy( filename, z->w[w_num].path );
	if( filename[0] == CLIP_LET )
	{
	  prn_str( msg_ptr[95] );
	  prn_str( filename+2 );
	}
	else prn_str( filename );
	isolate();
	prn_str( crlf );
	prn_str( crlf );
	bee();
	oi = z->showicon[w_num];
	z->showicon[w_num] = 0;
	maintree.bytes_total = maintree.files = 0L;
	err = !print_tree(0);
	arrow();
	if( prn_dev==-2 ) f_alert1( msg_ptr[15] );
	else if( !err )
	{
	  spf( tmpf, msg_ptr[22], (int)maintree.files, maintree.files!=1 ? msg_ptr[23] : "",
	      maintree.bytes_total, "", maintree.bytes_total!=1L ? msg_ptr[23] : "" );
	  prn_str( crlf );
	  prn_str( tmpf );
	  prn_str( crlf );
	  if( prn_param.ffd && prn_dev<=0 ) m_formfeed();
	}
	if( prn_dev>0 )
	{
	  cFclose(prn_dev);
	  modal_update( f, temp, 0, 1 );
	}
	if( (z->showicon[w_num] = oi) != 0 ) get_icn_matches(w_num);
	modal_unblit( f, 1, 0 );
	rdrw_all();
	modal_unblit( f, 0, 0 );
      }
#else DEMO
      demo_version();
#endif DEMO
  }
  return 1;
}
/********************************************************************/
void force( int drv )
{
  char garbage[]="x:\\..";

  if( drv != CLIP_LET )
  {
    *(mas->bad_media) = drv-'A';
    garbage[0] = drv;
    Fopen( garbage, 0 );
  }
}
Bsec boot[2];
int allfile, allfold, blksiz, blksizf;
char acopypath[2][4] = { "x:\\", "x:\\" }, copy_clip;
void set_copies( FORM *f )
{
  obj_enab( f->tree, z->diskcopy>0, 1, CMCOPIES );
}
int rw_disk( int rw, unsigned int sec, int disk, unsigned int size, char *ptr )
{
  register int j;
  char err;
  long ret;
  extern LoadCookie *lc;
  char *l;

  disk -= 'A';
  l = (*lc->lock_drive);
  for( j=4; --j>=0; l++ )
    if( *l==disk ) *l = -2;
  l = (*lc->lock_drive);
  j = 0;
  do
    ret = Rwabs( rw, ptr, size, sec, disk );
  while( ret==AE_CHNG && ++j<2 );
  for( j=4; --j>=0; l++ )
    if( *l==-2 ) *l = disk;
  err = !TOS_error( ret, 0 );
  if( !rw && !sec && !err )
    for( j=8; j<11; j++ )
      *(ptr+j) = (char)Random();
  return( err );
}
int stat_copyall( FORM *f, OBJECT *o, int draw )
{
  static char was_first;
  char barch, err=0;
  long ll;
  int i, k;
  unsigned int rws;
  union
  {
    unsigned char c[2];
    unsigned int i;
  } u;
  static unsigned int rwsiz;	/* keep around for first read (after initialize) */

  if( dtrack<=0 && dside<=0 && !rwf )
  {
    if( !ftimes-- )
    {
      force( ddrv );
      if( ferr==2 && dtrack ) return 1;
      return -2;
    }
    if( dtrack >= -1 )	/* not first time */
    {
      spf( filename, msg_ptr[123], ftimes+1, ftimes0 );
      if( f_alert1(filename) == 2 ) return -2;
      err=0;
    }
    o[DOPBAR2+2].ob_width = 0;		/* 003: moved here */
    if( draw ) form_draw( f, 0, 0 );	/* 003 */
    fptr = fptr0 = c_buf+(long)blksizf*fsides*fspt*fbps;
    ctrack = dtrack = ftracks;
    cside = dside = 0;
    fscnum = 0;
    rwf = 0;
    was_first = !draw;
  }
  if( was_first && draw ) was_first=0;
  else
  {
    if( rwf==0 )
    {
      if( sdrv>'B' )
      {
	rwsiz = ctrack;
	if( (ctrack-=faltslice) < dtrack-blksiz ) ctrack = dtrack-blksiz;
	if( ctrack<0 ) ctrack = 0;
	rwsiz -= ctrack;
      }
      else if( --cside<0 )
      {
	cside=fsides-1;
	ctrack--;
      }
    }
    else if( rwf==(ffmt?2:1) )
      if( ddrv>'B' )
      {
	rwsiz = dtrack;
	if( (dtrack-=faltslice) < ctrack ) dtrack = ctrack;
	if( dtrack<0 ) dtrack = 0;
	rwsiz -= dtrack;
      }
      else if( --dside<0 )
      {
	dside=fsides-1;
	dtrack--;
      }
  }
  o[DOPDRIV].ob_spec.free_string[0] = i = rwf ? ddrv : sdrv;
  i = i<='B';
  k = rwf ? dtrack : ctrack;
/*  if( !i ) k += rwsiz;*/
  barch = opbar( o, rwf, ftracks, k );
  form = o;
  translate( 77, (long)(rwf==2?(ftwst>1 ? 2 : 2+ftwst):rwf), 0, 0, 0 );
  o[DOPSIDE].ob_spec.free_string[0] = i ? (rwf ? dside : cside)+'1' : ' ';
  if( i ) spf( o[DOPNUM].ob_spec.free_string, "%2d", rwf ?
      dtrack : ctrack );
  else
  {
    k = fspt*fsides;
    spf( o[DOPNUM].ob_spec.free_string, msg_ptr[78], (long)(unsigned)
	((rwf?dtrack:ctrack)*k), (long)(unsigned)(((rwf?dtrack:ctrack)+rwsiz)*k - 1) );
    rws = rwsiz*k;
  }
  sec_track( f, i, draw );
  if( draw ) 	/* 003: moved reset DOPBAR1 up */
  {
    form_draw( f, DOPTYPE-1, 0 );
    if( barch ) form_draw( f, rwf ? DOPBAR2+2 : DOPBAR1+2, 0 );
    switch( rwf )
    {
      case 0:
	if( sdrv <= 'B' )	   /* source is a floppy disk */
	{
	  fptr -= (long)fspt*fbps;
	  if( !src_ok || ftimes==ftimes0-1 )	/* 003: was !ftimes */
	  {
	    if( (err = _abort()<<1) == 0 ) err = !TOS_error( (long)
		Floprd( fptr, "", sdrv-'A', 1, ctrack, cside, fspt ), 0 );
	  }
	  if( !cside && !ctrack )  /* randomize serial no. in boot sec */
	    for( i=8; i<11; i++ )
	      *(fptr+i) = (char)Random();
	  if( err ) src_ok=0;
	}
	else
	{
	  fptr -= (long)rws*fbps;
	  err = rw_disk( 0, ctrack*fspt*fsides, sdrv, rws, fptr );
	}
	if( dtrack-ctrack == blksiz || !ctrack && !cside )
	{
	  fptr = fptr0 = c_buf+(long)blksizf*fsides*fspt*fbps;
	  rwf = ffmt ? 2 : 1;
	  if( ctrack || cside ) src_ok=0;	/* multiple reads */
	}
	break;
      case 1:
	if( ddrv <= 'B' )
	{
	  fptr0 -= (long)fspt*fbps;
	  if( (err=_abort()<<1) == 0 )
	    do
	      if( (ll = Flopwr(fptr0, "", ddrv-'A', 1, dtrack, dside,
		  fspt )) != 0 ) err = ll==AEWRPRO ? f_writepro() :
		  !TOS_error( ll, 0 );
	    while( ll && !err );
	}
	else
	{
	  fptr0 -= (long)rws*fbps;
	  if( !dtrack )
	  {
	    u.i = boot[1].sides;
	    *(fptr0+27) = u.c[0];
	    *(fptr0+26) = u.c[1];
	    u.i = boot[1].spt;
	    *(fptr0+25) = u.c[0];
	    *(fptr0+24) = u.c[1];
	  }
	  if( (err=_abort()<<1) == 0 ) err = rw_disk( 1, dtrack*fspt*fsides,
	      ddrv, rws, fptr0 );
	}
	rwf = dtrack==ctrack && cside==dside ? 0 : (ffmt ? 2 : 1);
	break;
      case 2:		   /* format destination */
	scnum = fscnum;
	err = format_sec( ffmt, ddrv, fspt, dtrack, dside, ftwst );
	fscnum = scnum;
	rwf = 1;
    }
  }
  return ferr = err;
}
int i_copyall( OBJECT *o, FORM *f )
{
  int j, k;
  char err=0, last, empty, temp[]="x:\\*.*";
  long errnum, len, blklen;
  unsigned long blocks;

  blksiz = blksizf = 0;
  boot[1].sides = 999;
  acopypath[0][0] = sdrv = *(copy_q+3);
  acopypath[1][0] = ddrv = *(copy_q+4);
  copy_clip = acopypath[0][0]==CLIP_LET || acopypath[1][0]==CLIP_LET;
  bee();
  for( j=0; j<2 && !err; j++ )		/* read the boot-sectors */
  {
    if( !j )
    {
      temp[0] = acopypath[0][0];
      err = (errnum = cFsfirst(temp,0x37)) != 0 && errnum != AEFILNF;
    }
    if( !err && (errnum = bootsec( acopypath[j][0], &boot[j] )) == 0 )
    {
      if( (acopypath[0][0]==CLIP_LET || fix_bootsec( &boot[j] )) && !j )
      { 			/* find out how many swaps are needed */
	is_clip = acopypath[1][0]==CLIP_LET;
	if( !copy_clip )
	{
	  /* number of blocks (in tracks/side) that can be copied in each swap */
	  blksiz = c_buflen / boot[0].bps / boot[0].spt / boot[0].sides;
	  ffmt = c_buf + c_buflen - boot[0].spt*1024; /* format buffer */
	  blksizf = ffmt>c_buf ? ((long)ffmt-(long)c_buf) /
	      boot[0].bps / boot[0].spt / boot[0].sides : 0L;
	  blocks = 0L;
	}
	empty = 1;
	temp[2] = '\0';
	if( tree_init(temp,0L) )
	  while( (k=tree_next(0L)) != 1 ) switch(k)
	  {
	    case -2:
	      empty = 1;
	      break;
	    case -3:
	      if( empty ) maintree.tree_fsize = 0L;
	      else break;
	    case 0:
	      if( !copy_clip ) do
	      {
		len = c_buflen - sizeof(copy_struct);
		last=0;
		if( maintree.tree_fsize > len ) blklen = len;
		else
		{
		  last++;
		  blklen = maintree.tree_fsize;
		}
		maintree.tree_fsize -= blklen;
		if( blklen & 1 ) blklen++;
		c_buflen = len - blklen;
		if( last && c_buflen <= sizeof(copy_struct) || !last )
		{
		  c_buflen = c_bufmax;
		  blocks++;
		}
	      } while( !last );
	      empty = 0;
	  }
	if( c_buflen != c_bufmax )
	{
	  blocks++;
	  c_buflen = c_bufmax;
	}
	allfile = maintree.files;
	allfold = maintree.folders;
	if( !copy_clip )
	{
	  spf( o[CMFOSWAP].ob_spec.tedinfo -> te_ptext, lfmt,
	      blksizf ? (long)(boot[0].tps + blksizf - 1) / blksizf : 0L );
	  spf( o[CMNOSWAP].ob_spec.tedinfo -> te_ptext, lfmt,
	      blksiz ? (long)(boot[0].tps + blksiz - 1) / blksiz : 0L );
	  spf( o[CMFISWAP].ob_spec.tedinfo -> te_ptext, lfmt, blocks );
	}
      }
    }
    else if( !j ) err = !TOS_error( errnum, 0 );
  }
  arrow();
  if( !err )
  {
    if( copy_clip )
    {	/* force copy by files */
      z->diskcopy = 0;
      x_copyall( o, CMOK, f );
      jog_background=1;
      return 0;
    }
    *(o[CMSOURCE].ob_spec.free_string) = acopypath[0][0];
    *(o[CMDEST].ob_spec.free_string) = acopypath[1][0];
    obj_enab( o, blocks>0L && boot[1].sides<999, 1, CMFILES );	 /* by files */
    /* prevent copying flop to HD by assuming diff in # sectors < 256 */
    obj_enab( o, (boot[0].sides == boot[1].sides && boot[0].spt <=
	boot[1].spt || acopypath[1][0]>='C') && (unsigned)(boot[1].secs-boot[0].secs)<256
	&& boot[0].bps == boot[1].bps && blksiz, 1, CMNOFMT );
    obj_enab( o, acopypath[1][0]<='B' && boot[0].sides && boot[0].sides<=2
	&& boot[0].spt<12 && boot[0].tps<83 && boot[0].tps>0 &&
	boot[0].bps==512 && blksizf, 1, CMFORMAT );
    if( hide_if( o, CMCOPIES, !copy_clip && acopypath[0][0]<='B' && acopypath[1][0]<='B' ) )
	o[CMCOPIES].ob_flags |= EDITABLE;
    spf( o[CMCOPIES].ob_spec.tedinfo->te_ptext, ifmt, z->disk_copies );
    if( (unsigned char)z->diskcopy > 2 ) z->diskcopy = 0;
    if( o[CMFILES].ob_state&DISABLED && !z->diskcopy ) z->diskcopy = 1;
    if( o[CMFORMAT].ob_state&DISABLED && z->diskcopy==1 ) z->diskcopy = 2;
    if( o[CMNOFMT].ob_state&DISABLED && z->diskcopy==2 ) z->diskcopy = 0;
    obj_ltrue( o, z->diskcopy, 3, CMFILES );
    set_copies(f);
    hide_if( o, CMBACK, f->handle>0 );
    obj_true1( o, z->dskcpy_bak, CMBACK );
    copy_ok = 0;
    return 1;
  }
  return 0;
}
int t_copyall( OBJECT *o, int num, FORM *f )
{
  z->diskcopy = num-CMFILES;
  set_copies(f);
  form_draw( f, CMCOPIES, CMCOPIES );
  return 0;
}
int x_copyall( OBJECT *o, int num, FORM *f )
{
  int i, c;
  
  form = o;
  z->dskcpy_bak = fback = is_sel(CMBACK);
  if( num==CMOK )
  {
    if( is_locked(sdrv) || is_locked(ddrv) )
    {
      TOS_error( LOCK_ERR, 0 );
      return 0;
    }
    switch(z->diskcopy)
    {
      case 0:
	copy_free();
 	i = z->macr_rec;
	z->macr_rec = 0;
	c = z->in_copy;
	z->in_copy = 0;	/* fake out the queueing alert */
	cpy_from_d( acopypath[0], acopypath[1], 1 );
	z->in_copy |= c;
	z->macr_rec = i;
	copy_ok = 1;
	break;
      case 2:
	if( acopypath[1][0] > 'B' )
	  if( f_alert1(msg_ptr[83]) == 2 ) return 1;
	blksizf = blksiz;
	ffmt = 0L;
      case 1:
	if( blksizf > boot[0].tps ) blksizf = boot[0].tps;
	c_buflen = (long)blksizf*boot[0].sides*boot[0].spt*boot[0].bps;
	if( ffmt )
	{
	  ffmt = c_buf + c_buflen;
	  c_buflen += boot[0].spt*1024;
	}
	if( c_buf != diskbuff ) lshrink( c_buf, c_buflen );
	clip_ptrs(c_bufmax = c_buflen);
	if( copy_clip || ddrv > 'B' || (ftimes = ftimes0 = z->disk_copies/*003*/ =
	    atoi(o[CMCOPIES].ob_spec.tedinfo->te_ptext)) <= 0 )
	    ftimes = ftimes0 = 1;
	fform = 0;
	ftwst = auto_twst( z->other_pref.b.dflt_twst, boot[0].spt );
	ffunc = stat_copyall;
	fblksiz = blksizf;
	fcopy = sdrv > 'B' ? 2 : 1;
	ftracks = boot[0].tps;
	dtrack = -2;
	fspt = boot[0].spt;
	fbps = boot[0].bps;
	src_ok = 1;
	rwf = 0;
	cside = dside = 0;
	fsides = boot[0].sides;
	fmemory = 0L;
	if( fcopy==2 )
	{
	  fslice = 0;
	  i = 5-z->back_speed;
	  faltslice = (blksizf+i-1)/i;
	}
	else fslice = z->back_speed<<1;
	op_stat = (DISKOP *)f->mem_ptr;
	w_num = w_handle = -1;
	start_sform( OPST_FORM, &f );
	if( f->handle<=0 ) modal_update( f, &sdrv, 1, 0 );
    }
  }
  else
  {
    copy_free();
    copy_ok = 1;
    jog_background = 1;		/* 004 */
  }
  return 1;
}
/********************************************************************/
#define DESKPIC ((char *)f->mem_ptr)
#define DESKCHG *((char *)f->mem_ptr+120)
void desk_picname( FORM *f )
{
  f->tree[KPRFILE].ob_spec.free_string = spathend(DESKPIC);
}
int hide_dbox( OBJECT *o )
{
  int i;

  form = o;
  hide_if( o, KPRIBOX, i=is_sel(KPRSHOW) );
  return i;
}
int i_deskpr( OBJECT *o, FORM *f )
{
  obj_true1( o, z->desk_in_wind, KPRWIND );
  obj_true1( o, z->show_pic, KPRSHOW );
  obj_true( o, !z->wall_pic, KPRCENT );
  obj_ltrue( o, z->pic_colormode, 3, KPRCALL );
  strcpy( DESKPIC, z->desk_pic );
  hide_dbox(o);
  DESKCHG = 0;
  desk_picname(f);
  return 1;
}
void load_init( int init, FORM *f )
{
  if( f->handle<=0 )
  {
    prev_blit( f, 1 );
    wind_lock(0);
  }
  if( !init ) load_desk_pic();
  else init_desktop( 1, 1 );
  do_desk();
  if( f->handle<=0 )
  {
    prev_blit( f, 0 );
    wind_lock(1);
  }
}
int t_deskpr( OBJECT *o, int num, FORM *f )
{
  if( num==KPRSHOW ) form_draw( f, hide_dbox(o) ? KPRIBOX : KPRBOX, 0 );
  return 0;
}
int x_deskpr( OBJECT *o, int num, FORM *f )
{
  char temp[120], c1, c2, c4;
  int c3;
  
  switch( num )
  {
    case KPRSEL:
      strcpy( temp, DESKPIC );
      if( rfselect( f, temp, "*.*", "*.{IMG,NEO,P[IC][123],TN?,BMP}",
	  msg_ptr[148] ) )
      {
	strcpy( DESKPIC, temp );
	desk_picname(f);
	form_draw( f, KPRFILE-1, 0 );
	DESKCHG = 1;
      }
      return 0;
    case KPROPT:
      picopts = &z->desk_picopts;
      if( start_sform( PIC_FORM, &f ) == PICOOK ) DESKCHG = 1;
      return 0;
    case KPRNOTE:
      if( start_sform( NOTE_SUB, &f ) == NOTEOK )
      {		/* 003 */
        modal_unblit( f, 1, 0 );
        do_desk();
        modal_unblit( f, 0, 1 );
      }
      return 0;
    case KPROK:
      form = o;
      strcpy( z->desk_pic, DESKPIC );
      c1 = z->show_pic;
      c2 = z->wall_pic;
      c3 = z->pic_colormode;
      c4 = z->desk_in_wind;
      z->show_pic = is_sel(KPRSHOW);
      z->wall_pic = is_sel(KPRWALL);
      z->pic_colormode = scan_sel( KPRCALL, KPRCALL+3 ) - KPRCALL;
      z->desk_in_wind = is_sel(KPRWIND);
      if( DESKCHG || z->show_pic != c1 || z->wall_pic != c2 ||
	  z->pic_colormode != c3 ) load_init( 0, f );
      if( z->desk_in_wind != c4 ) load_init( 1, f );
      break;
  }
  return 1;
}
/********************************************************************/
#define CONFLOLD  ((char *)f->mem_ptr)
#define CONFLNEW  ((char *)f->mem_ptr+120)
#define CONFODTA  (DTA *)((long)f->mem_ptr+120+120)
#define CONFNDTA  (DTA *)((long)f->mem_ptr+120+120+sizeof(DTA))
#define CONFLATT  *((char *)f->mem_ptr+120+120+2*sizeof(DTA))
/***int u_nameconfl( OBJECT *o, FORM *f )  /* actually used by parent dialog */
{
  if( CONFLRET!=CONFLWAIT )
  return 1;
}***/
void confl_data( char *str, DTA *dta )
{
  int time[6];
  char buf[100], *mp;
  
  if( !dta ) str[0] = 0;
  else
  {
    to_tandd( ((long)dta->d_date<<16)|dta->d_time, time );
    mp = msg_ptr[82];
    msg_ptr[82] = "";	/* skip "on" after time */
    tandd_to_str( time, buf );
    msg_ptr[82] = mp;
    spf( str, msg_ptr[173], dta->d_length, dta->d_length!=1L ?
        msg_ptr[23] : "", buf );
  }
}
void confl_names( FORM *f )
{
  OBJECT *o = f->tree;
  
  to_filename( spathend(CONFLOLD), o[FCONNAME].ob_spec.tedinfo->te_ptext );
  to_filename( spathend(CONFLNEW), o[FCONNEW].ob_spec.tedinfo->te_ptext );
}
void confl_copy( FORM *f )
{	/* fix for modeless */
  strcpy( nameconf_old, CONFLOLD );
  strcpy( nameconf_new, CONFLNEW );
}
int test_confl( FORM *f )
{
  DTA *old;
  int ret;
  
  if( !nameconf_sdta )
  {
    if( !strcmp( CONFLOLD, CONFLNEW ) ) return 0;
    old = Fgetdta();
    Fsetdta(CONFODTA);
    cFsfirst(CONFLOLD,0x37);
    confl_data( f->tree[FCONNAME+1].ob_spec.tedinfo->te_ptext, CONFODTA );
    Fsetdta(CONFNDTA);
    ret = cFsfirst(CONFLNEW,0x37);
    confl_data( f->tree[FCONNEW+1].ob_spec.tedinfo->te_ptext, CONFNDTA );
    Fsetdta(old);
    if( !ret ) return 1;
    ret = cFrename( 0, CONFLOLD, CONFLNEW );
    if( ret == AEACCDN ) return 1;
    if( !TOS_error( ret, 0 ) ) nameconf_ret = -1;
    else confl_copy(f);
    return 0;
  }
  else
  {
    confl_data( f->tree[FCONNAME+1].ob_spec.tedinfo->te_ptext, nameconf_sdta );
    confl_data( f->tree[FCONNEW+1].ob_spec.tedinfo->te_ptext, nameconf_ddta );
    return 1;
  }
}
int i_nameconfl( OBJECT *o, FORM *f )
{
  int ret=1;
  
  form = o;	/* nameconf_sdta only set when copy/move */
  translate( 75, nameconf_sdta ? (long)(moving+1) : 0L, 0L, 0, 0 );
  strcpy( CONFLOLD, nameconf_old );
  strcpy( CONFLNEW, nameconf_new );
  nameconf_ret = 0;
  if( !nameconf_sdta && !strcmp( CONFLOLD, CONFLNEW ) ) ret = 0;
  else
  {
    if( !nameconf_sdta )
    {
      o[FCONNAME].ob_flags &= ~EDITABLE;
      if( (CONFLATT=cFattrib( CONFLOLD, 0, 0 )) & 1 )
          cFattrib( CONFLOLD, 1, CONFLATT&0xFE );
    }
    else
    {
      o[FCONNAME].ob_flags |= EDITABLE;
      CONFLATT = -1;
    }
    confl_names(f);
    if( !test_confl(f) ) ret = 0;
  }
  nameconf_sdta = nameconf_ddta = 0L;
/***  if( src )
  {
    wind_get( src->handle, X_WF_DIALFLGS, &flags );
    wind_set( src->handle, X_WF_DIALFLGS, flags & ~X_WD_ACTIVE );
  } ***/
  return ret;
}
void clean_confl( FORM *f )
{
  if( CONFLATT!=-1 && CONFLATT & 1 ) cFattrib( CONFLNEW, 1, CONFLATT );
}
int x_nameconfl( OBJECT *o, int num, FORM *f )
{
  char temp[20];
  
  if( num==FCONOK )
  {
    if( o[FCONNAME].ob_flags&EDITABLE )
    {
      if( !test_filename( o[FCONNAME].ob_spec.tedinfo->te_ptext, temp, 1 ) )
      {
        confl_names(f);
        return 0;
      }
      if( temp[0] ) strcpy( spathend(CONFLOLD), temp );
    }
    if( !test_filename( o[FCONNEW].ob_spec.tedinfo->te_ptext, temp, 1 ) )
    {
      confl_names(f);
      return 0;
    }
    if( temp[0] ) strcpy( spathend(CONFLNEW), temp );
    else
    {
      nameconf_ret = 1;
      clean_confl(f);
      return 1;
    }
    if( CONFLATT==-1 ) confl_copy(f);
    else if( test_confl(f) ) return 0;
    clean_confl(f);
    return 1;
  }
  else if( num==FCONSKIP )
  {
    nameconf_ret = 1;
    clean_confl(f);
    return 1;
  }
  nameconf_ret = -1;
  clean_confl(f);
  return 1;
}
/********************************************************************/
#define OPTREE	((TREE *)f->mem_ptr)
#define OPFILES (*(int *)((long)f->mem_ptr+sizeof(TREE)))
#define OPFOLDS (*(int *)((long)f->mem_ptr+sizeof(TREE)+2))
#define OPLEN	(*(int *)((long)f->mem_ptr+sizeof(TREE)+4))
#define OPEND	(*(int *)((long)f->mem_ptr+sizeof(TREE)+6))
#define OPQ	(*(long *)((long)f->mem_ptr+sizeof(TREE)+8))
#define OPFUNC	*(((int (**)( OBJECT *o, FORM *f, int init ))((long)f->mem_ptr+sizeof(TREE)+12)))
#define OPDEL	*((char *)f->mem_ptr+sizeof(TREE)+12+4)
#define OPCONF	*((char *)f->mem_ptr+sizeof(TREE)+12+5)
#define OPREN	*((char *)f->mem_ptr+sizeof(TREE)+12+6)
#define OPCOUNT *((char *)f->mem_ptr+sizeof(TREE)+12+7)
#define OPBACK	*((char *)f->mem_ptr+sizeof(TREE)+12+8)
#define OPMOVE	*((char *)f->mem_ptr+sizeof(TREE)+12+9)
#define OPFILT	*((char *)f->mem_ptr+sizeof(TREE)+12+10)
#define OPERATE *((char *)f->mem_ptr+sizeof(TREE)+12+11)
#define OPPAUSE *((char *)f->mem_ptr+sizeof(TREE)+12+12)
#define OPFLAGS *((char *)f->mem_ptr+sizeof(TREE)+12+13)
#define OPSSLICE *((char *)f->mem_ptr+sizeof(TREE)+12+14)
#define OPINFOLD *((char *)f->mem_ptr+sizeof(TREE)+12+15)
#define OPERR	*((char *)f->mem_ptr+sizeof(TREE)+12+16)
#define OPNAME	((char *)f->mem_ptr+sizeof(TREE)+12+17)
#define OPTEMPL ((char *)f->mem_ptr+sizeof(TREE)+12+17+120)
#define OPDEST  ((char *)f->mem_ptr+sizeof(TREE)+12+17+120+3*29)
#define OPNEWDIR *((char *)f->mem_ptr+sizeof(TREE)+12+17+120+3*29+120)
#define OPFIRST  *((char *)f->mem_ptr+sizeof(TREE)+12+17+120+3*29+120+1)
#define READING 1
#define WRITING 2
#define MOVING	3
#define DELING	4
#define OTYPE	1
#define OFOLD	2
#define OFILE	4
#define ONAME	8
int file_form=-1;

void lock_fileop( FORM *f )
{
  int lock;
  
  if( OPERATE > 0 )
  {
    lock = !OPBACK && !OPPAUSE;
    if( flocked != lock && f->handle>0 ) wind_lock( flocked=lock );
  }
}
void stat_fop( FORM *f, int mode )
{
  OBJECT *o;
  int i;
  
  form = o = f->tree;
  if( !mode ) o[FOSTAT].ob_spec.tedinfo->te_ptext[0] =
      o[FOFILES].ob_spec.tedinfo->te_ptext[0] =
      o[FOFOLD].ob_spec.tedinfo->te_ptext[0] =
      o[FONAME].ob_spec.tedinfo->te_ptext[0] = 0;
  else
  {
    if( mode&OTYPE )
    {
      if( OPERATE<0 ) o[FOSTAT].ob_spec.tedinfo->te_ptext[0] = 0;
      else translate( 80, OPERATE, 0, 0, 0 );
      form_draw( f, FOSTAT, 0 );
    }
    if( mode&OFOLD && OPCOUNT )
    {
      spf( o[FOFOLD].ob_spec.tedinfo->te_ptext, nfmt, OPFOLDS );
      form_draw( f, FOFOLD, 0 );
    }
    if( mode&OFILE && OPCOUNT )
    {
      spf( o[FOFILES].ob_spec.tedinfo->te_ptext, nfmt, OPFILES );
      form_draw( f, FOFILES, 0 );
    }
    if( mode&ONAME )
    {
      i = o[FONAME].ob_width / char_w;
      short_path( OPNAME, o[FONAME].ob_spec.tedinfo->te_ptext, i, i );
      form_draw( f, FONAME, 0 );
    }
  }
}
FORM *get_fileform(void)
{
  return forms+file_form;
}
void fop_nums( int mode, char *path )
{
  FORM *f;
  char temp[120];
  
  if( file_form<0 ) return;
  f = get_fileform();
  strcpy( temp, OPNAME );
  if( path ) strcpy( OPNAME, path );
  if( mode>=0 ) OPERATE = mode;
  stat_fop( f, mode==5 ? OTYPE|OFOLD|ONAME : OTYPE|OFILE|ONAME );
  strcpy( OPNAME, temp );
}
long next_q( long q )
{
  return q + strlen(copy_q+q) + 1;
}
void op_flags( FORM *f, int set )
{
  if( !OPDEL )
  {
    form = f->tree;
    OPCONF = is_sel(FOOVER);
    OPREN = is_sel(FOREN);
    OPMOVE = is_sel(FOMOVE);
  }
  if( set && OPDEL>=0 )
    if( OPDEL )
    {
      z->count_del = OPCOUNT;
      if( f->handle>0 ) z->del_bak = OPBACK;
      z->filt_del = OPFILT;
    }
    else
    {
      z->count_copy = OPCOUNT;
      if( f->handle>0 ) z->copy_bak = OPBACK;
      copy_slice = f->handle>0 ? OPSSLICE : -1;
      z->conf_over = OPCONF;
      z->rename_dest = OPREN;
      z->filt_copy = OPFILT;
    }
}
void op_select( FORM *f, int sel )
{
  static char obs[] = { FOCOUNT, FOFILT, FOEDIT };
  int i;
  
  for( i=sizeof(obs); --i>=0; )
    if( sel ) f->tree[obs[i]].ob_flags |= SELECTABLE;
    else f->tree[obs[i]].ob_flags &= ~SELECTABLE;
  if( !sel ) op_flags( f, 1 );	/* about to begin operation, get flags */
}
static int op_prev;
void op_init(FORM *f)
{
  int i;
  
  if( OPFILT ) get_filt_templ( OPTREE->filt=&z->copydel_filt,
       OPTREE->filt_templ=OPTEMPL );
  else OPTREE->filt = 0L;
  OPLEN = (*copy_q<<8) + *(copy_q+1);
  OPNEWDIR = 0;
  if( !OPDEL )
  {
    strcpy( OPDEST, copy_q+(OPQ=4L) );
    OPQ = next_q(OPQ);
  }
  else OPQ=3L;
  if( !(OPFLAGS&1) )	/* not abs path (copy grp item or drive) */
  {
    strcpy( OPNAME, copy_q+OPQ );
    OPEND = pathend(OPNAME);
    OPQ = next_q(OPQ);
  }
  OPINFOLD = 0;
  OPERR = 0;
  op_prev = 0;
}
int op_next(FORM *f)
{
  int i, is_fold;
  DTA *old, new;
  char *end, *ptr, *ptr2;
  int ret;
  
  end = OPNAME+OPEND;
again:
  if( OPINFOLD )
  {
    if( op_prev==-3 ) strcpy( OPNAME, OPTREE->tree_path );
    ret = tree_next(OPTREE);
    if( ret==0 || ret==-2 /*%|| op_prev==-3*/ )
    {
      strcpy( ptr=OPNAME, OPTREE->tree_path );
      ptr += pathend(ptr);
      if( ret<0 ) *ptr = 0;
      else strcpy( ptr, OPTREE->tree_fname );
    }
    if( ret==-1 ) goto again;
    if( ret!=1 ) return op_prev=ret;
    OPINFOLD = 0;
    if( (i=OPTREE->tree_stat) != AENMFIL )
    {
      OPERR = i;
      return op_prev=1;	/* error */
    }
    return op_prev=-3;
  }
  if( OPQ > OPLEN ) return op_prev=1;	/* all done */
  if( OPFLAGS&1 ) strcpy( OPNAME, copy_q+OPQ );
  else strcpy( end, copy_q+OPQ );
  is_fold = *spathend(copy_q+OPQ)==0;
  OPQ = next_q(OPQ);
  OPTREE->tree_valid = 0;
  if( OPFILT && !is_fold )
  {
    old = Fgetdta();
    Fsetdta( &new );
    i = cFsfirst( OPNAME, 0x37 );
    Fsetdta( old );
    if( i==AENMFIL || i==AEFILNF ) goto again;
    if( i<0 )
    {
      OPERR = i;
      return op_prev=1; /* error */
    }
    if( !filter_it( &z->copydel_filt, &new, OPTEMPL ) ) goto again;  /* skip */
  }
  if( is_fold )
  {
    if( tree_init(OPNAME,OPTREE) )
    {
      OPTREE->tree_valid = 1;
      OPINFOLD = 1;
      if( OPFLAGS&2 ) goto again;
      return op_prev=-2;   /* found folder */
    }
    OPERR = OPTREE->tree_stat;
    return op_prev=1;	/* error */
  }
  return op_prev=0;	/* found file */
}
int op_copy( OBJECT *o, FORM *f, int init )
{
  int err=0, i, j;
  char tmp[120];

  if( file_form<0 ) file_form = f-forms;
  moving = OPMOVE;
  if( init ) op_init(f);
  else
  {
    i = write_back();
    f = get_fileform();	/* it might have changed! */
    if( i != -1 )
    {
      if( i>0 ) err = 1;
    }
    else if( (i = read_fblock()) == -2 )
    {
      f = get_fileform();	/* it might have changed! */
      switch( op_next(f) )
      {
        case 1:
          if( OPNEWDIR>0 && !OPERR ) err = c_new_dir( OPDEST, 0L, &OPNEWDIR,
   	      0, 1, &OPFOLDS ) > 0;
          if( !err )
            if( (i = copy_a_buffer( 0L, &OPFILES, &OPFOLDS )) == -2 ) break;
          file_form = -1;
          return -1;
        case 0:
          if( OPNEWDIR < 0 ) OPNEWDIR = 0;
          strcpy( tmp, OPDEST );
          strcat( tmp, spathend(OPNAME) );
          memcpy( &maintree, OPTREE, sizeof(TREE) );
          err = copy_a_file( OPNAME, tmp, OPDEST, 0, OPNEWDIR, &OPFILES, &OPFOLDS );
          f = get_fileform();	/* it might have changed! */
          OPNEWDIR = 0;
          break;
        case -2:
          if( pathend(OPNAME) > 3 )	/* 003: copy drive to folder on dtop */
          {
            ++OPNEWDIR;
            strcat( OPDEST, OPNAME+uppath(OPNAME)+1 );
          }
          break;
        case -3:
          if( !OPFILT ) iso(OPNAME);
          err = 0;
          if( OPNEWDIR > 0 )
          {
            memcpy( &maintree, OPTREE, sizeof(TREE) );
            err = copy_a_file( OPNAME, OPDEST, OPDEST, 1, OPNEWDIR, &OPFILES, &OPFOLDS );
            f = get_fileform();	/* it might have changed! */
          }
          OPNEWDIR = 0;
          if( OPMOVE && !err ) err = c_remdir( OPNAME );
          *(spathend(OPDEST)-1) = '\0';
          iso(OPDEST);
      }
    }
    else if( i==0 ) err = 1;
  }
  file_form = -1;
  if( err ) return -1;
  return 0;
}
int op_del( OBJECT *o, FORM *f, int init )
{
  int i;
  
  if( init ) op_init(f);
  else if( (i=op_next(f))==1 ) return -1;
  else if( !i )
  {
    stat_fop( f, ONAME );
    OPFILES--;
    if( !fdelete(OPNAME,0,OPTREE->tree_att) ) return 1;   /* 006 */
/*    if( (OPERR = cFdelete(OPNAME)) < 0 ) return 1;  */
    stat_fop( f, OFILE );
  }
  else if( i==-3 )
  {
    if( !OPFILT )
    {
      iso(OPNAME);
      stat_fop( f, ONAME );
      if( (OPERR = cDdelete(OPNAME)) < 0 ) return 1;
    }
    OPFOLDS--;
    stat_fop( f, OFOLD );
  }
  return 0;
}
int cancel_fileop( int err, FORM *f, int close )
{
  char temp[120], *dest, del, modal=0;

  if( err==99 && OPERATE && f_alert1( msg_ptr[175] ) != 1 ) return 0;
  if( (del=OPDEL)==0 )
  {
    if( err==99 || err==98 ) err_del();
    clean_fwrite( !err );
    copy_free();
  }
  copy_ok = 1;
  strcpy( temp, copy_q+(del?3:4) );
  if( err>0 ) TOS_error( OPERR, 0 );
  if( err!=97 && (err != 99 || OPERATE) )
  {
    if( f->handle>0 )
    {
      if( close ) close_fwind( f, 0, 1 );	 /* careful: sets f->handle==0 */
      update_drive( temp, 0 );
    }
    else
    {
      modal_update( f, temp, 0, 0 );
      modal=1;
    }
    if( !del )	/* can't use OPDEL now */
    {
      dest = copy_q+next_q(4L);
      if( err>0 && temp[0]!=*dest || moving && !intersect( temp, dest ) )
        if( !modal ) update_drive( dest, 0 );
        else modal_update( f, dest, 0, 0 );
    }
  }
  if( err>0 && err<97 ) free_copyq();
  jog_background = 1;
  return 1;
}
FORM *num_form( int num )
{
  return forms+num;
}
int form_num( FORM *f )
{
  return f-forms;
}
int u_fileop( OBJECT *o, FORM *f )
{
  int err=0, msg[8], x, y, b, dum, slice=0, i, frm;
  EMULTI e;

  if( OPFIRST )
  {
    lock_fileop(f);
    OPFIRST = 0;
  }
  if( OPERATE < 0 )
  {
    OPERATE = 0;
    OPFILES = OPFOLDS = 0;
    stat_fop( f, OTYPE|OFILE|OFOLD );
    op_init(f);
    for(;;)
      if( (i=op_next(f)) == 1 ) break;
      else if( i==-2 )
      {
	OPFOLDS++;
	stat_fop( f, OFOLD|ONAME );	/* 004: ONAME */
      }
      else if( !i )
      {
	OPFILES++;
	stat_fop( f, OFILE|ONAME );	/* 004: ONAME */
      }
    OPERATE = -1;
    stat_fop( f, OTYPE );
    if( OPFOLDS+OPFILES > 1 )	/* 004 */
    {
      strcat( OPNAME, " +" );
      stat_fop( f, ONAME );
    }
    OPERATE = 0;
    if( !OPPAUSE )
    {
      OPERATE = OPDEL ? DELING : READING;
      lock_fileop(f);
      stat_fop( f, OTYPE );
      frm = form_num(f);
      (*OPFUNC)( o, f, 1 );
      f = num_form(frm);
      op_select( f, 0 );
    }
  }
  e.type = X_MU_DIALOG;
  if( !OPPAUSE ) do
  {
    if( (err=OPERR) != 0 )
    {
      cancel_fileop( err, f, 1 );
      return 0;
    }
    if( !OPBACK )
    {
      graf_mkstate( &x, &y, &b, &dum );
      if( b&1 && (i=objc_find( o, 0, 8, x, y )) > 0 &&
	  o[i].ob_flags&(EXIT|TOUCHEXIT) &&	/* 003 */
	  (f->handle>0 || !form_button( o, i, 1, &dum )) )
      {
	if( f->handle>0 ) multi_evnt( &e, msg );
	if( (char)(o[i].ob_type>>8)!=X_HELP )
	  if( i==FOCANC )
	  {
	    o[FOCANC].ob_state &= ~SELECTED;
            if( cancel_fileop( 99, f, 1 ) ) return 0;
            form_draw( f, FOCANC, 0 );
	  }
	  else
	  {
	    t_fileop( o, i, f );
	    if( OPPAUSE ) break;
	  }
      }
    }
    frm = form_num(f);
    err = (*OPFUNC)( o, f, 0 );
    f = num_form(frm);
    if( err )
    {
      cancel_fileop( err, f, 1 );
      return 0;
    }
  }
  while( !OPPAUSE && (!OPBACK || slice++<OPSSLICE) );
  return !err;
}
void oppause( OBJECT *o, FORM *f )
{
  hide_if( o, FOPAUSE, hide_if( o, FOOK, OPPAUSE )^1 );
}
int kblen, kbrem;
long add_kobold( int n, long opq, int add )
{
  char buf[200], *p;
  int l=0;

  if( !opq )
  {
    spf( buf, "#%d", n );
    l=1;
    if( n==_COPY ) strcat( buf, " OPEN_FOLDERS" );
    else if( n==_CHOOSE )
    {
      strcat( buf, " *+ " );
      l=0;
    }
  }
  else if( !strncmp(copy_q+opq,"0:",2) )		/* is it to/from clip? */
  {
    cmfree( &kobold_buf );
    return 0;
  }
  else
  {
    spf( buf, "#%d %s%s ", n, add ? "+ " : "", copy_q+opq );
    if( add && (p=strchr(buf,'\\')) != 0 ) strcpy( p, " " );	/* remove \ from folder name */
  }
  l += strlen(buf);
  if( !add_string( (void **)&kobold_buf, &kblen, &kbrem, buf, 20, l, -1 ) )
  {
    cmfree( &kobold_buf );
    return 0;
  }
  return next_q(opq);
}
int send_kobold( FORM *f )
{
  int id, msg[8], len;
  long opq;

  cmfree( &kobold_buf );
  if( z->other_pref.b.use_kobold && /*!(OPFLAGS&1) && 004 */ !is_clip &&
     z->cliparray[2]>=639 && ((id = appl_pfind("KOBOLD_2")) >= 0 ||
     (id = appl_pfind("KOBOLD_3")) >= 0) )
  {
    copy_free();
    len = (*copy_q<<8) + *(copy_q+1);
    if( OPDEL ) opq = 3L;
    else if( (opq=add_kobold( _DST_SELECT, 4L, 0 )) == 0 ) goto err;
    if( (opq=add_kobold( _SRC_SELECT, opq, 0 )) == 0 ) goto err;
    while( opq<=len )
      if( (opq=add_kobold( _SRC_SELECT, opq, 1 )) == 0 ) goto err;
    if( OPFLAGS&1 )	/* 004 */
      if( add_kobold( _CHOOSE, 0, 0 ) == 0 ) goto err;
    if( add_kobold( OPDEL ? _DELETE : (OPMOVE ? _MOVE : _COPY), 0, 0 ) == 0 ) goto err;
    msg[0] = KOBOLD_JOB_NO_WINDOW;
    msg[1] = AES_handle;
    msg[2] = 0;
    *(char **)&msg[3] = kobold_buf;
    appl_pwrite( id, 16, msg );
    return 1;
err:if( !OPDEL ) copy_init();
    return 0;
  }
  return 0;
}
int i_fileop( OBJECT *o, FORM *f )
{
  int i, frm;
  
  OPDEL = (i=*(copy_q+2)) == MDELF ? 1 : (i==MDELFNC?-1:0);
  if( OPDEL )
  {
    copy_free();
    OPFUNC = op_del;
    OPFLAGS = 0;
  }
  else
  {
    OPFUNC = op_copy;
    obj_true( o, (OPMOVE=((OPFLAGS=*(copy_q+3))&4)!=0)==0, FOCOPY );
  }
  if( send_kobold(f) )
  {
    cancel_fileop( 97, f, 0 );
    return 0;
  }
  if( OPDEL>=0 )
  {
    translate( 76, OPDEL, (long)(f->handle>0 ? f->old_title :
        o[FILESEG].ob_spec.free_string), 0, 0 );
    hide_if( o, FOBOX, !OPDEL );
    o[FOPAUSE].ob_x = o[FOOK].ob_x;
    o[FOPAUSE].ob_state &= ~SELECTED;
    obj_true1( o, OPCONF = (cnover_mode=(OPFLAGS&0x18)>>3)==3 ?
        z->conf_over : cnover_mode, FOOVER );
    obj_true1( o, OPREN=z->rename_dest, FOREN );
    OPPAUSE = OPDEL ? z->conf_del : z->conf_copy || z->move_mode==PREFMASK-PREFMCPY;
    if( ((i=Getshift())&0xC)==8 && (i&3) ) OPPAUSE ^= 1;
    if( (OPCOUNT = OPDEL ? z->count_del : z->count_copy) == 0 ) OPCOUNT = OPPAUSE;
    obj_true1( o, OPCOUNT, FOCOUNT );
  }
  else OPCONF = OPPAUSE = OPCOUNT = 0;
  oppause( o, f );
  hide_if( o, FOBACK, f->handle>0 );
  obj_true1( o, OPBACK = f->handle>0 && (OPDEL ? z->del_bak : z->copy_bak), FOBACK );
  obj_true1( o, OPFILT = OPDEL ? z->filt_del : z->filt_copy, FOFILT );
  short_path( copy_q+(OPDEL ? 3L : 4L), f->win_path, o[2].ob_width/6, sizeof(forms->win_path) );	/* 004 */
  if( OPCOUNT ) OPERATE = -1;
  else
  {
    frm = form_num(f);
    (*OPFUNC)( o, f, 1 );
    f = num_form(frm);
  }
  jog_background = 1;		/* 003 for all cases now */
  stat_fop( f, 0 );
  OPSSLICE = z->back_speed;	/* has to be before op_flags */
  op_select( f, OPPAUSE );
  OPERR = 0;
  copy_ok = 0;
  flocked = 0;
  if( has_Geneva ) lock_fileop(f);
  else OPFIRST = 1;
  return 1;
}
int x_fileop( OBJECT *o, int num, FORM *f )
{
  int frm;

  switch(num)
  {
    case FOEDIT:
      if( start_sform( FOP_FILT, &f ) == FILOK && OPFILT && OPCOUNT ) OPERATE = -1;
      return 0;
    case FOOK:
      form = o;
      OPMOVE = is_sel(FOCOPY+1);
      OPREN = z->rename_dest = is_sel(FOREN);
      if( !OPCOUNT || OPFILES || OPFOLDS )
      {		/* nothing to operate on, a quick way out */
        OPERATE = OPDEL ? DELING : READING;
        OPPAUSE = 0;
        lock_fileop(f);
        frm = form_num(f);
        (*OPFUNC)( o, f, 1 );
        f = num_form(frm);
        op_select( f, 0 );
        stat_fop( f, OTYPE );
        oppause( o, f );
        form_draw( f, FOBBOX, 0 );
        return 0;
      }
    case FOCANC:
      return cancel_fileop( 99, f, 0 );
    default:
      cancel_fileop( 98, f, 0 );
      return 1;
  }
}
int i_filefilt( OBJECT *o, FORM *f )
{
  _init_filter( o, f, &z->copydel_filt, msg_ptr[134], 0 );
  return 1;
}
int x_filefilt( OBJECT *o, int num, FORM *f )
{
  int ret;
  
  ret = _x_filter( o, num, f );
  z->copydel_filt.flags.s.allfold = 1;
  return ret;
}
int t_fileop( OBJECT *o, int num, FORM *f )
{
  form = o;
  if( o[num].ob_flags & SELECTABLE ) switch(num)
  {
    case FOBACK:
      OPBACK = is_sel(FOBACK);
      lock_fileop(f);
      break;
    case FOCOUNT:
      if( (OPCOUNT = is_sel(FOCOUNT)) != 0 ) OPERATE = -1;
      else
      {
	stat_fop( f, 0 );
	form_draw( f, FOSBOX, 0 );
      }
      break;
    case FOFILT:
      OPFILT = is_sel(FOFILT);
      if( OPCOUNT ) OPERATE = -1;
      break;
    case FOPAUSE:
      OPPAUSE = is_sel(FOPAUSE);
      lock_fileop(f);
      break;
  }
  op_flags( f, OPERATE>=READING );
  return 0;
}
/********************************************************************/
/* added dynamic font list for 002 */
typedef struct
{
  int id;
  char name[34], scale;		/* 003: was 33 */
} FONT_DESC;
OBJECT *wfontpop;
FONT_DESC *font_desc;
#define wfont_id(x)    font_desc[x].id
#define wfont_scale(x) font_desc[x].scale
#define wfont_name(x)  font_desc[x].name

#define NWFONT	    *(int *)f->mem_ptr
#define wfont_num   *((int *)f->mem_ptr+1)
#define WPR	    ((WIND_PRF *)((long)f->mem_ptr+2+2))
#define WFONT	    ((WIND_FONT *)((long)f->mem_ptr+2+2+sizeof(WIND_PRF)))
int total_fonts;
char dial_fonts;	/* # of dialogs having fonts list */
int add_font( int id, char *name, char scale )
{
  static int fonts_rem, obs, obs_rem;
  int i, w;
  char *ptr;
  
  if( !wfontpop )
  {
    if( !add_thing( (void **)&wfontpop, &obs, &obs_rem, &popups[PNPI0-1],
        3, sizeof(OBJECT), -1 ) ) return 0;
    wfontpop[0].ob_width = char_w*24+4;
    wfontpop[0].ob_next = wfontpop[0].ob_head = wfontpop[0].ob_tail = -1;
  }
  if( !add_thing( (void **)&wfontpop, &obs, &obs_rem, &popups[PNPI0],
      3, sizeof(OBJECT), -1 ) ) return 0;
  if( !add_thing( (void **)&font_desc, &total_fonts, &fonts_rem,
      0L, 3, sizeof(FONT_DESC), -1 ) ) return 0;
  i = total_fonts-1;
  (*graphics->vst_font)( wfont_id(i) = id, scale );
  wfont_scale(i) = graphics->is_scalable = (*graphics->check_mono)(scale);
  wfontpop[i+1].ob_y = i*wfontpop[1].ob_height;
  wfontpop[i+1].ob_width = wfontpop[0].ob_width;
  ptr = wfont_name(i);
  *ptr = ' ';
  /* if the font id is 1, then it's the default system font, otherwise
     use its real name */
  strncpy( ptr+1, id==1 ? msg_ptr[151] : name, 32 );
  *(ptr+33) = 0;	/* 003 */
  /* add this object to the fonts popup */
  objc_add( wfontpop, 0, i+1 );
  if( (w = strlen(ptr)*char_w) > wfontpop[0].ob_width )
      wfontpop[0].ob_width = w;
  for( i=1; i<=total_fonts; i++ )
  {	/* always do this whole loop so that names get reset after add_thing */
    wfontpop[i].ob_spec.free_string = wfont_name(i-1);
    wfontpop[i].ob_width = wfontpop[0].ob_width;
  }
  return 1;
}
void free_fonts(void)
{
  if( !--dial_fonts )
  {
    cmfree( (char **)&wfontpop );
    cmfree( (char **)&font_desc );
  }
}
/* show a color/fill pattern sample */
void wpr_sample( OBJECT *o, int num, int val )
{
  o[num].ob_spec.index = (o[num].ob_spec.index&0xFFFF0000L) |
      (unsigned int)(num==WPRPSAMP ? popups[val+PFIL0].ob_spec.index :
      popups[val+PCOL0].ob_spec.index);
}
void font_samp_ted( TEDINFO *ted, FORM *f, int mono )
{
  ted->te_font = wfont_scale(wfont_num)&1 ?
      (mono ? GDOS_MONO : GDOS_PROP) : GDOS_BITM;
  ted->te_junk1 = wfont_id(wfont_num);
  ted->te_junk2 = WFONT[NWFONT].size;
}
void wpr_stat( OBJECT *o, FORM *f, int draw )
{
  int *i;
  TEDINFO *ted;
  
  wpr_sample( o, WPRPSAMP,   WPR->s.fillpattern );
  wpr_sample( o, WPRPSAMP+1, WPR->s.interiorcol );
  wpr_sample( o, WPRPSAMP+2, WPR->s.textcol );
  obj_true1( o, WPR->s.textmode, WPROPAQ );
  i = &(ted = o[WPRSAMP].ob_spec.tedinfo)->te_color;
  *i = (*i & 0xF000) | WPR->i;
  font_samp_ted( ted, f, NWFONT==1 || NWFONT==2 );
  if( draw )
  {
    form_draw( f, WINBOX, 0 );
    form_draw( f, WPRSAMP, 0 );
  }
}
int get_nwfont( OBJECT *o, FORM *f )
{
  int fn, i;
  
  form = o;
  fn = NWFONT = scan_sel( WPRICON, WPRICON+4 ) - WPRICON;
  for( i=0; i<total_fonts; i++ )
    obj_enab( wfontpop, !fn || fn==3 || wfont_scale(i)&2, 1, i+1 );
  return fn;
}

void wpr_draw( FORM *f, int inter )
{
  int oh;

  modal_unblit( f, 1, 0 );
  if( inter ) do_desk();
  else new_colors();
  oh = w_num;
  for( w_num=0; w_num<7; w_num++ )
    if( (w_handle = wxref[w_num]) >= 0 )
    {
      set_wfile();
      set_window();
      if(inter) rdrw_all();
      else _redraw_obj( z->maximum, 0 );
    }
  w_handle = wxref[w_num=oh];
  modal_unblit( f, 0, !inter );
}

int x_windpr( OBJECT *o, int num, FORM *f )
{
  if( num==WPROK )
  {
    z->wind_prf = *WPR;
    memcpy( z->wind_font, WFONT, 4*sizeof(WIND_FONT) );
    load_fonts();
    form = o;
    z->real_time = is_sel(WOPREAL);
    wpr_draw(f,1);
    set_wfile();
  }
  else if( num==WOPGAD )
  {
    if( start_set(0) == KOOK ) wpr_draw(f,0);
    return 0;
  }
  if( num!=WPROK || !(Getshift()&3) ) free_fonts();	/* 004: conditional */
  else return 0;
  return 1;
}
/* display a popup menu */
unsigned int _popup( OBJECT *o, int parent, int obj, unsigned int val, OBJECT *pop )
{
  MENU m, out;
  int x, y;

  m.mn_tree = pop;
  m.mn_menu = parent;
  m.mn_item = parent+val+1;
  m.mn_scroll = 1;
  /* If this is the patterns or colors popup, then display a checkmark
     at the right value by setting it to a BOXCHAR */
  if( (char)pop[parent+1].ob_type==G_BOX || (char)pop[parent+1].ob_type==G_BOXCHAR )
    for( x=parent+1; x<=pop[parent].ob_tail; x++ )
      pop[x].ob_type = x==m.mn_item ? G_BOXCHAR : G_BOX;
  objc_offset( o, obj, &x, &y );
  if( menu_popup( &m, x-2, y-2, &out ) ) return out.mn_item-parent-1;
  return val;
}
unsigned int do_popup( OBJECT *o, int parent, int obj, unsigned int val )
{
  return _popup( o, parent, obj, val, popups );
}
unsigned int do_fontpop( OBJECT *o, int obj, unsigned int val )
{
  return _popup( o, 0, obj, val, wfontpop );
}
void redo_font( FORM *f )
{
  (*graphics->vst_font)( wfont_id(wfont_num), wfont_scale(wfont_num) );
}
void npr_stat( OBJECT *o, FORM *f, int draw );
void new_point( FORM *f, OBJECT *o, int draw, int ind )
{
  int dum;
  
  redo_font(f);
  WFONT[NWFONT].size = (*graphics->vst_point)( WFONT[NWFONT].size, &dum, &dum, &dum, &dum );
  /* set the new number */
  spf( o[ind].ob_spec.tedinfo->te_ptext, ifmt, WFONT[NWFONT].size );
  /* reset and draw the sample */
  if( ind==WOPT ) wpr_stat( o, f, draw );
  else npr_stat( o, f, draw );
  /* redraw the point number, if necessary */
  if( draw ) form_draw( f, ind, 0 );
}
void set_fnum( FORM *f, OBJECT *o, int i, int draw, int name, int ind )
{
  int dum;

  wfont_num = i;
  /* set the name of the current font in the dialog */
  o[name].ob_spec.free_string = wfont_name(i)+1;
  if( draw ) form_draw( f, name, 0 );
  WFONT[NWFONT].id = wfont_id(i);
  redo_font(f);
/*%  vst_alignment( vdi_hand, 0, 5, &dum, &dum );*/
  /* reset point size because the previous point size might not be possible */
  new_point( f, o, draw, ind );
}
void new_fnum( FORM *f, OBJECT *o, int draw, int name, int ind )
{
  int i;
  
  for( i=0; i<total_fonts; i++ )
    if( wfont_id(i)==WFONT[NWFONT].id )
    {
      set_fnum( f, o, i, draw, name, ind );
      return;
    }
  set_fnum( f, o, 0, draw, name, ind );
}
int get_all_fonts( FORM *f, int o )
{
  int i, id;
  char namebuf[33];
  FORM *oth;
  
  dial_fonts++;
  for( oth=forms, i=num_forms; --i>=0; oth++ )
    if( oth->treenum==o ) return 1;
  total_fonts = 0;
  bee();
  (*graphics->load_fonts)( 0L, 0 );
  if( graphics->has_gdos )
    for( i=1; i<=graphics->total_fonts; i++ )
    {
      namebuf[32] = 0;
      id = vqt_name( graphics->handle, i, namebuf );	/* font name */
      if( id != -1 )				/* not a font to skip */
        if( !add_font( id, namebuf, graphics->scalable && namebuf[32] && id!=1 ) )
        {
          if( !wfontpop )
          {
            arrow();
            return 0;
          }
          break;
        }
    }
  arrow();
  if( !wfontpop )
    if( !add_font( 1, namebuf, 0 ) ) return 0;
  wfontpop[0].ob_height = total_fonts*popups[PNPI0].ob_height;
  return 1;
}
int i_windpr( OBJECT *o, FORM *f )
{
  if( !get_all_fonts( f, NOTEPREF ) ) return 0;
  get_nwfont( o, f );
  *WPR = z->wind_prf;
  memcpy( WFONT, z->wind_font, 4*sizeof(WIND_FONT) );
  obj_true1( o, z->real_time, WOPREAL );
  new_fnum( f, o, 0, WPRFONT, WOPT );
  wpr_stat( o, f, 0 );
  hide_if( o, WPRICON+3, Geneva_ver>0x102 );
  hide_if( o, WOPGAD, gui->xtern.x_settings!=0L /* 004: was has_Geneva */ );
  return 1;
}
void point_up( FORM *f, int ind, int test )
{
  int pt, h;

  /* start at point+1 and continue until the size changes, since all sizes
     may not be available */
  redo_font(f);
  for( pt=WFONT[NWFONT].size+1; pt<=25; pt++ )
    if( (*graphics->vst_point)( pt, &dum, &dum, &dum, &h ) == pt )
    {
      if( test && h > bar_h-2 ) return;
      WFONT[NWFONT].size = pt;
      new_point( f, f->tree, 1, ind );
      return;
    }
}
void point_down( FORM *f, int ind )
{
  int pt;
  
  if( WFONT[NWFONT].size < 3 ) return;
  /* find the next lower point size */
  redo_font(f);
  if( (*graphics->vst_point)( pt=WFONT[NWFONT].size-1, &dum,
      &dum, &dum, &dum ) <= pt )
  {
    WFONT[NWFONT].size = pt;
    new_point( f, f->tree, 1, ind );
  }
}
int popup_fill( OBJECT *o, int parent, int val )
{
  return do_popup( o, PFIL0-1, parent, val );
}
int popup_col( OBJECT *o, int parent, int val )
{
  return do_popup( o, PCOL0-1, parent, val );
}
int t_windpr( OBJECT *o, int num, FORM *f )
{
  int i;
  
  if( num>0 )
  {
    switch( num )
    {
      case WPROPAQ:			/* toggle text transparent/opaque */
	WPR->s.textmode ^= 1;
	break;
      case WPRLEFT:			/* previous fill pattern */
	WPR->s.fillpattern--;
	break;
      case WPRRT:			/* next fill pattern */
	WPR->s.fillpattern++;
	break;
      case WPRPAT:			/* fill pattern popup */
	WPR->s.fillpattern = popup_fill( o, WPRPAT-1, WPR->s.fillpattern );
	break;
      case WPRLEFT+3:			/* previous interior color */
	WPR->s.interiorcol--;
	break;
      case WPRRT+3:			/* next interior color */
	WPR->s.interiorcol++;
	break;
      case WPRPAT+3:			/* interior color popup */
	WPR->s.interiorcol = popup_col( o, WPRPAT+2, WPR->s.interiorcol );
	break;
      case WPRLEFT+6:			/* previous text color */
	WPR->s.textcol--;
	break;
      case WPRRT+6:			/* next text color */
	WPR->s.textcol++;
	break;
      case WPRPAT+6:			/* text color popup */
	WPR->s.textcol = popup_col( o, WPRPAT+5, WPR->s.textcol );
	break;
      case WPRICON:
      case WPRICON+1:
      case WPRICON+2:
      case WPRICON+3:
	i = NWFONT;	/* get old value */
	if( get_nwfont( o, f ) != i ) new_fnum( f, o, 1, WPRFONT, WOPT );
	break;
      case WPRFONT:		   /* do the fonts popup list */
	if( total_fonts>1 ) set_fnum( f, o, do_fontpop( o, WPRFONT, wfont_num ), 1, WPRFONT, WOPT );
	break;
      case WOPUP:		  /* go up one point size */
	point_up( f, WOPT, NWFONT==3 );
	break;
      case WOPDWN:
	point_down( f, WOPT );
	break;
    }
    wpr_stat( o, f, 1 );
  }
  return 0;
}
/********************************************************************/
#define NNFONT	    *(int *)f->mem_ptr
#define nfont_num   *((int *)f->mem_ptr+1)
#define NOPAQ	    (*((char *)f->mem_ptr+4))
#define NOCOL	    (*((char *)f->mem_ptr+5))
#define NFONT	    ((WIND_FONT *)((long)f->mem_ptr+2+2+1+1))

void npr_stat( OBJECT *o, FORM *f, int draw )
{
  int *i;
  TEDINFO *ted;

  o[NOTECSAM].ob_spec.obspec.interiorcol =
      popups[NOCOL+PCOL0].ob_spec.obspec.interiorcol;
  i = &((ted=o[NOTESAMP].ob_spec.tedinfo)->te_color);
  *i = (*i & 0xF07F) | (NOCOL<<8) | (NOPAQ<<7);
  font_samp_ted( ted, f, 0 );
  if( draw )
  {
    form_draw( f, NOTECSAM, 0 );
    form_draw( f, NOTESAMP, 0 );
  }
}
int i_notepr( OBJECT *o, FORM *f )
{
  if( !get_all_fonts( f, WINPREF ) ) return 0;
  NNFONT = 0;
  memcpy( NFONT, &z->wind_font[4], sizeof(WIND_FONT) );
  NOCOL = z->note_col;
  obj_true1( o, NOPAQ=z->note_opaq, NOTEOPAQ );
  new_fnum( f, o, 0, NOTEFONT, NOTEPT );
  npr_stat( o, f, 0 );
  return 1;
}
int x_notepr( OBJECT *o, int num, FORM *f )
{
  if( num==NOTEOK )
  {
    memcpy( &z->wind_font[4], NFONT, sizeof(WIND_FONT) );
    form = o;
    z->note_opaq = is_sel(NOTEOPAQ);
    z->note_col = NOCOL;
    load_fonts();
/*    prev_blit( f, 1 );	003: draw in parent dial instead
    do_desk();
    prev_blit( f, 0 ); */
  }
  if( num!=NOTEOK || !(Getshift()&3) ) free_fonts();	/* 004: conditional */
  else return 0;
  return 1;
}
int t_notepr( OBJECT *o, int num, FORM *f )
{
  if( num>0 )
  {
    switch( num )
    {
      case NOTEOPAQ:			/* toggle text transparent/opaque */
	NOPAQ ^= 1;
	break;
      case NOTLEFT:			/* previous text color */
	if( --NOCOL < 0 ) NOCOL = 15;
	break;
      case NOTERT:			/* next text color */
	if( ++NOCOL > 15 ) NOCOL = 0;
	break;
      case NOTECOL:			/* text color popup */
	NOCOL = popup_col( o, NOTECOL-1, NOCOL );
	break;
      case NOTEFONT:		  /* do the fonts popup list */
	if( total_fonts>1 ) set_fnum( f, o, do_fontpop( o, NOTEFONT, nfont_num ), 1, NOTEFONT, NOTEPT );
	break;
      case NOTEPL:		   /* go up one point size */
	point_up( f, NOTEPT, 0 );
	break;
      case NOTEMI:
	point_down( f, NOTEPT );
	break;
    }
    npr_stat( o, f, 1 );
  }
  return 0;
}
/********************************************************************/
int ST_rez, old_ST, fv_mode, fv_mode0;
int get_bootdev(void)
{
  return(_bootdev);
}
int f_al_inf( char *msg, char *name )
{
  spf( filename, msg, name );
  return(f_alert1(filename));
}
void change_rez(void)
{
  int drv, hand, err;
  char dinf[15], *ptr, *name, *ptr2;
  static char reztbl[] = { 1, 2, 3, 0, 4, 0, 5, 6 };	/* 002 */

  if( aes_ge_40 )
  {
    if( falc_vid ) shel_write( SHW_NEWREZ, fv_mode, 1, 0L, 0L );
    else shel_write( SHW_NEWREZ, ST_rez+2, 0, 0L, 0L );
    return;
  }
  name = "NEWDESK.INF";
  if( !TT_vid && !falc_vid )
    if( !aes_ge_20 ) name="DESKTOP.INF";
  err=0;
  if( (drv=Supexec((long (*)())get_bootdev)) != 0 ||
      f_al_inf( msg_ptr[100], name ) == 1 )
  {
    dinf[0] = drv+'A';
    dinf[1] = ':';
    dinf[2] = '\\';
    strcpy( dinf+3, name );
    if( (hand=cFopen(dinf,2)) > 0 )
    {
      cFread( hand, 1024L, diskbuff );
      ptr = diskbuff;
      while( ptr < diskbuff+1024 )
	if( !strncmp( ptr++, "\r\n#E ", 5 ) ) break;
      if( ptr < diskbuff+1024 )
      {
	cFseek( (long)(ptr+8-diskbuff), hand, 0 );
	fpf( hand, "%x", reztbl[ST_rez] );	/* 002 */
	if( (ptr2 = strchr( ptr, '\r' )) != 0 && ptr2-ptr>15 )
	{
	  cFseek( (long)(ptr+16-diskbuff), hand, 0 );
	  fpf( hand, "%02x %02x", (unsigned char)(fv_mode>>8),
	      (unsigned char)fv_mode );
	}
      }
      else err++;
      cFclose(hand);
    }
    else err++;
    if( err ) f_al_inf( msg_ptr[101], name );
    else if( f_al_inf( msg_ptr[106], name ) == 1 ) warmboot();
  }
}
void get_ST_rez(void)
{
  int j;
  
  j = fv_mode & (VERTFLAG|STMODES|COL80|NUMCOLS);
  ST_rez = -1;
  if( j==(COL40|BPS4|STMODES|VERTFLAG) ) ST_rez = 0;		 /* ST low */
  else if( j==(COL80|BPS2|STMODES|VERTFLAG) ) ST_rez = 1;	 /* ST med */
  else if( j==(COL80|BPS1|STMODES) ) ST_rez = 2;	/* ST high */
}
void fv_stat( OBJECT *o, FORM *f, int draw )
{
  int i, j;

  /* set 40/80 columns buttons */
  obj_true1( o, (j=fv_mode&COL80)==0, FALTC40 );
  obj_true1( o, j, FALTC80 );
  /* set ST-compatible rez */
  get_ST_rez();
  obj_ltrue( o, ST_rez, 3, FALSTL );
  /* set # of bitplanes/colors */
  obj_ltrue( o, fv_mode&NUMCOLS, 5, FALC2 );
  /* can't have 2-color, 40-columns */
  obj_enab( o, (fv_mode&NUMCOLS) != BPS1, 1, FALTC40 );
  obj_enab( o, (fv_mode&COL80) != COL40, 1, FALC2 );
  /* set interlace/double line */
  obj_true1( o, fv_mode&VERTFLAG, FALDBL );
  if( fv_mode & VGA_FALCON )	       /* it's a VGA monitor */
  {
    /* can't have 80-columns, True Color */
    obj_enab( o, (fv_mode&NUMCOLS) != BPS16, 1, FALTC80 );
    obj_enab( o, (fv_mode&COL80) != COL80, 1, FALC2+4 );
  }
  if( draw )
  {
    form_draw( f, FALTC40-1, 0 );
    form_draw( f, FALSTL-1, 0 );
    form_draw( f, FALC2-1, 0 );
    form_draw( f, FALDBL, 0 );
  }
}
static char ttreztbl[] = { 0, 1, 2, 7, 4, 6 };
int i_newrez( OBJECT *o, FORM *f )
{
  int rez, i;

  old_ST = rez = Getrez();
  if( !falc_vid )
  {
    for( i=VOSTLOW; i<=VOSTLOW+5; i++ )
      if( rez==8 && i!=VOSTLOW+5 || TT_vid && i==VOSTLOW+5 ||
	  !TT_vid && rez==2 && i!=VOSTLOW+2 ||
	  !TT_vid && rez!=2 && i==VOSTLOW+2 ||
	  !TT_vid && i>VOSTLOW+2 )
      {
	o[i].ob_state |= DISABLED;
	o[i].ob_state &= ~SELECTED;
      }
      else
      {
	o[i].ob_state &= ~DISABLED;
	if( ttreztbl[i-VOSTLOW]==rez ) o[i].ob_state |= SELECTED;
	else o[i].ob_state &= ~SELECTED;
      }
    hide_if( o, TTRFBOX, 0 );
    hide_if( o, TTRTTBOX, 1 );
  }
  else
  {
    fv_mode = fv_mode0 = Vsetmode(-1);
    o[FALDBL].ob_spec.free_string = mon_type()!=2 ? msg_ptr[170] : msg_ptr[171];
    fv_stat( o, f, 0 );
    hide_if( o, TTRFBOX, 1 );
    hide_if( o, TTRTTBOX, 0 );
  }
  hide_if( o, TTRINF, !aes_ge_40 );
  return 1;
}
int t_newrez( OBJECT *o, int num, FORM *f )
{
  int old_vid, i;

  if( num<=0 ) return 1;  
  if( !(o[num].ob_state&DISABLED) )
  {
    /* save current values */
    old_ST = ST_rez;
    old_vid = fv_mode;
    /* only modify some of the bits; set them to 0 by default */
    fv_mode &= ~(VERTFLAG|STMODES|COL80|NUMCOLS);
    if( num==FALSTL ) fv_mode |= (STMODES|COL40|BPS4|VERTFLAG); 	/* ST low */
    else if( num==FALSTM ) fv_mode |= (STMODES|COL80|BPS2|VERTFLAG);	/* ST med */
    else if( num==FALSTH ) fv_mode |= (STMODES|COL80|BPS1);    /* ST high */
    else
    {
      form = o;
      if( is_sel(FALTC80) ) fv_mode |= COL80;	  /* 80 cols */
      if( is_sel(FALDBL) ) fv_mode |= VERTFLAG;   /* interlace */
      fv_mode |= scan_sel( FALC2, FALC2+5 ) - FALC2;
    }
    get_ST_rez();
    /* only redraw the dialog if something important has changed */
    if( ((old_vid&NUMCOLS)==BPS1) != ((fv_mode&NUMCOLS)==BPS1) ||
	(old_vid&COL80) != (fv_mode&COL80) || fv_mode&VGA_FALCON &&
	((old_vid&NUMCOLS)==BPS16) != ((fv_mode&NUMCOLS)==BPS16) ||
	ST_rez != old_ST ) fv_stat( o, f, 1 );
  }
  return 0;
}
int x_newrez( OBJECT *o, int num, FORM *f )
{
  static int modes[] = { COL40|BPS4|STMODES|VERTFLAG, COL80|BPS2|STMODES|VERTFLAG,
      COL80|BPS1|STMODES, 0, COL80|BPS4, 0, COL80|BPS1|STMODES, COL40|BPS16 };

  if( num==TTROK )
    if( falc_vid )
    {
      if( fv_mode != fv_mode0 ) change_rez();
    }
    else
    {
      form = o;
      ST_rez = ttreztbl[scan_sel(VOSTLOW,VOSTLOW+6)-VOSTLOW];
      fv_mode = modes[ST_rez];
      if( ST_rez != old_ST ) change_rez();
    }
  return 1;
}
/********************************************************************/
char *mac_buf;
int mac_check( unsigned char sh, unsigned char key )
{
  unsigned char *ptr;

  if( (ptr = mac_buf) != 0 )	/* 003: because of GRPITEM usage */
    while( *ptr || *(ptr+1) )
    {
      if( ptr != mac && *(ptr+2)==sh && *(ptr+3)==key )
      {
        f_alert1( msg_ptr[118] );
        return(0);
      }
      ptr += chtoi(ptr);
    }
  *(mac+2)=sh;
  *(mac+3)=key;
  return(1);
}

void mac_key( unsigned char k1, unsigned char k2, unsigned char *k )
{
  if( *k == k1 ) *k = k2;
}
int mac_get( OBJECT *o, int off )
{
  unsigned char sh=0, key;

  form = o;
  if( is_sel(off+MACROCNT-MACROBOX) ) sh|=4;
  if( is_sel(off+MACROLSH-MACROBOX) ) sh|=2;
  if( is_sel(off+MACRORSH-MACROBOX) ) sh|=1;
  if( is_sel(off+MACROALT-MACROBOX) ) sh|=8;
  key = *(mac+3);
  if( !(sh&4) )
  {
    mac_key( 0x74, 0x4d, &key );
    mac_key( 0x73, 0x4b, &key );
    mac_key( 0x77, 0x47, &key );
  }
  else if( sh&4 )
  {
    mac_key( 0x4d, 0x74, &key );
    mac_key( 0x4b, 0x73, &key );
    mac_key( 0x47, 0x77, &key );
  }
  if( !(sh&8) )
  {
    if( key>=0x78 && key<=0x83 ) key -= 0x76;
  }
  else if( key>=0x2 && key<=0xd ) key += 0x76;
  if( !(sh&3) )
  {
    if( key>=0x54 && key<=0x5d ) key -= 0x19;
  }
  else if( key>=0x3b && key<=0x44 ) key += 0x19;
  return( mac_check( sh, key ) );
}

int mac_set( FORM *f, int off, int flag )
{
  static unsigned char obj[] = { MACRONAM, MACROSIZ, MACROCNT,
      MACROLSH, MACRORSH, MACROALT, MACROKEY },
      keycode[] = { 1, 0xf, 0xe, 0x53, 0x52, 0x62, 0x61, 0x47, 0x48, 0x50,
	  0x4d, 0x4b, 0x72, 0x60 },
      *keynam[] = { "Esc", "Tab", "Bksp", "Del", "Ins", "Help", "Undo",
	  "Home", "", "", "", "", "Entr", "ISO" }, fmt[]="F%d",
	  kpfmt[]="kp %c";
  int i;
  unsigned char *ptr, c, c2;
  OBJECT *o = f->tree;

  ptr = mac;
  while( *ptr || *(ptr+1) ) ptr += chtoi(ptr);
  o += off-MACROBOX;
  if( off==MACROBOX )
  {
    o[MACRONAM].ob_spec.tedinfo->te_ptext = mac+4;
    spf( o[MACROSIZ].ob_spec.tedinfo->te_ptext, nfmt, chtoi(mac) );
    spf( o[MACROSPC].ob_spec.tedinfo->te_ptext, nfmt,
        (int)(MACRO_SIZ - z->macro_len - 2) );
  }
  obj_selec( o, *(mac+2)&4, 1, MACROCNT );
  obj_selec( o, *(mac+2)&2, 1, MACROLSH );
  obj_selec( o, *(mac+2)&1, 1, MACRORSH );
  obj_selec( o, *(mac+2)&8, 1, MACROALT );
  *(ptr = o[MACROKEY].ob_spec.tedinfo->te_ptext) = '\0';
  c = *(mac+3);
  if( c==0x74 ) c=0x4d;
  else if( c==0x73 ) c=0x4b;
  else if( c==0x77 ) c=0x47;
  else if( c>=0x78 && c<=0x83 ) c -= 0x76;
  for( i=0; i<sizeof(keycode); i++ )
    if( keycode[i] == c ) strcpy( ptr, keynam[i] );
  if( c >= 0x3b && c <= 0x44 ) spf( ptr, fmt, c-0x3a );
  else if( c >= 0x54 && c <= 0x5d ) spf( ptr, fmt, c-0x53 );
  if( !*ptr )
  {
    c2 = *(Keytbl( (void *)-1L, (void *)-1L, (void *)-1L )->unshift + c);
    if( c >= 0x63 && c <= 0x72 || c == 0x4a || c == 0x4e )
	spf( ptr, kpfmt, c2 );
    else
    {
      *ptr++ = c2;
      *ptr = '\0';
    }
  }
  if( flag )
    for( i=0; i<sizeof(obj); i++ )
      form_draw( f, obj[i], 0 );
  return(flag);
}
char macr_all, macr_ret;
void hide_mbox( OBJECT *o, int off, int box2 )
{
  hide_if( o, off+MACRBOX1-MACROBOX, box2 );
  hide_if( o, off+MACRBOX2-MACROBOX, box2^1 );
}
int i_macro( OBJECT *o, FORM *f )
{
  if( (mac_buf = lalloc( z->macro_len, -1 )) == 0 ) return 0;
  memcpy( mac_buf, z->macro, z->macro_len );
  mac = mac_buf + z->macstrt;
  mac_set( f, MACROBOX, 0 );
  hide_if( o, MACROUP, hide_if( o, MACRODWN, macr_all ) );
  obj_enab( o, macr_all, 1, MACRORUN );
  hide_mbox( o, MACROBOX, 1 );
  macr_ret = 0;
  return 1;
}
int t_macro( OBJECT *o, int num, FORM *f )
{
  unsigned char *ptr, *ptr2, *ptr3;
  unsigned int i;

    switch( num )
    {
      case MACROUP:
	if( !mac_get( o, MACROBOX ) ) return 0;
	ptr = ptr3 = mac;
	if( ptr == mac_buf )
	  while( *ptr || *(ptr+1) )
	  {
	    mac = ptr;
	    ptr += chtoi(ptr);
	  }
	else
	{
	  ptr2 = mac_buf;
	  while( ptr2 < ptr )
	  {
	    mac = ptr2;
	    ptr2 += chtoi(ptr2);
	  }
	}
	if( mac != ptr3 ) mac_set( f, MACROBOX, 1 );
	break;
      case MACRODWN:
	if( mac_get( o, MACROBOX ) )
	{
	  ptr3 = mac;
	  mac += chtoi(mac);
	  if( !*mac && !*(mac+1) ) mac = mac_buf;
	  if( mac != ptr3 ) mac_set( f, MACROBOX, 1 );
	}
    }
  return 0;
}
void read_mackey( OBJECT *o, FORM *f, int off )
{
  int i;

  hide_mbox( o, off, 0 );
  form_draw( f, off, 0 );
  i = (Bconin(2)>>16) | (Getshift()<<8);
  if( mac_check( i>>8, i ) ) mac_set( f, off, 0 );
  hide_mbox( o, off, 1 );
  form_draw( f, off, 0 );
}
int x_macro( OBJECT *o, int num, FORM *f )
{
  char *ptr;
  int i;
  long l;
  
  switch(num)
  {
      case MACRORKY:
        read_mackey( o, f, MACROBOX );
	return 0;
      case MACROCH:
	if( mac_get( o, MACROBOX ) )
	{
end:	  memcpy( z->macro, mac_buf, z->macro_len );
	  macr_ret = 1;
	}
	break;
      case MACRORMV:
	ptr = mac + (i=chtoi(mac));
	memcpy( z->macro, mac_buf, l=mac-mac_buf );	/* 003: copy first part */
	memcpy( z->macro+l, ptr, z->macro_len - (ptr-mac_buf) );	/* 003: copy rest */
	z->macro_rem += i;
	if( (z->macro_len -= i) <= 2 )	/* 003: added */
	    free_macros();
	macr_ret = 1;
	break;
      case MACRORUN:
	if( mac_get( o, MACROBOX ) )
	{
	  z->macr_play = 1;
	  z->macptr = (z->macstrt = mac-mac_buf) + sizeof(int) +
	      sizeof(int) + 21;
	  goto end;
	}
	break;
    }
  return 1;
}
/********************************************************************/
char *foldc_old, *foldc_new;
int i_foldconfl( OBJECT *o, FORM *f )
{
  to_filename( foldc_old, o[FLDCNAME].ob_spec.tedinfo->te_ptext );
  to_filename( foldc_old, o[FLDCNEW].ob_spec.tedinfo->te_ptext );
  return 1;
}
int x_foldconfl( OBJECT *o, int num, FORM *f )
{
  if( num==FLDCMAKE && !test_filename( o[FLDCNEW].ob_spec.tedinfo->te_ptext, foldc_new, 1 ) )
  {
    i_foldconfl( o, f );
    form_draw( f, FLDCNEW, 0 );
    return 0;
  }
  return 1;
}
/********************************************************************/
int i_newname( OBJECT *o, FORM *f )
{
  char c, *p;
  int l;
  
  c = *(p=spathend(nameconf_new));
  *p = 0;
  l = o[NNPATH].ob_width/6;
  short_path( nameconf_new, (char *)f->mem_ptr, l, l );
  o[NNPATH].ob_spec.free_string = (char *)f->mem_ptr;
  *p = c;
  to_filename( p, o[NNOLD].ob_spec.tedinfo->te_ptext );
  strcpy( o[NNNEW].ob_spec.tedinfo->te_ptext, o[NNOLD].ob_spec.tedinfo->te_ptext );
  nameconf_ret = -1;
  return 1;
}
int x_newname( OBJECT *o, int num, FORM *f )
{
  char temp[13];
  
  if( num==NNOK )
    if( !test_filename( o[NNNEW].ob_spec.tedinfo->te_ptext, temp, 1 ) )
    {
      i_newname( o, f );
      form_draw( f, NNNEW, 0 );
      return 0;
    }
    else
    {
      strcpy( spathend(nameconf_new), temp );
      nameconf_ret = 0;
    }
  else if( num==NNSKIP ) nameconf_ret = 1;
  return 1;
}
/********************************************************************/
#define FYI_COUNT 	(*(long *)f->mem_ptr)
#define FYI_BUF		((char *)((long)f->mem_ptr+4))
int init_fyi( OBJECT *o, FORM *f, int msg )
{
  int line;
  char *ptr;
  
  strcpy( ptr=FYI_BUF, msg_ptr[msg] );
  line = 0;
  o += FYISTR1;
  do
  {
    if( line<3 ) o->ob_spec.tedinfo->te_ptext = ptr;
    else o->ob_spec.free_string = ptr;
    if( (ptr = strchr( ptr, '|' )) == 0L ) break;
    *ptr++ = 0;
    o++;
  }
  while( ++line<4 );
  /* pause 2 seconds */
  FYI_COUNT = get_timer() + 2*200;
  return 1;
}
int i_opfyi( OBJECT *o, FORM *f )
{
  return init_fyi( o, f, 176 );
}
/*int i_helpfyi( OBJECT *o, FORM *f )
{
  return init_fyi( o, f, 177 );
} */
int x_fyi( OBJECT *o, int num, FORM *f )
{
  return 1;
}
int u_fyi( OBJECT *o, FORM *f )
{
  if( get_timer() > FYI_COUNT )
  {
    close_fwind( f, 0, 1 );
    return 0;
  }
  return 1;
}
#pragma warn +par

char *set_exe;
void signal_xset(void)
{
  free_set = set_exe;	/* main now frees free_set */
  set_exe = 0L;		/* 003 */
}

int start_set( int num )
{
  static SET_STRUCT s;
  extern NEO_ACC nac;
  
  if( set_exe ) return (*s.start_dial)( num );
  else
  {
    s.nac = &nac;
    s.AES_handle = AES_handle;
    s.first_form = num;
    s.fills = &popups[PFIL0];
    s.colors = &popups[PCOL0];
    s.signal_xset = signal_xset;
    s.popup_fill = popup_fill;
    s.popup_col = popup_col;
    s.get_wcolors = get_wcolors;
    s.new_colors = new_colors;
    bee();
    load_set( &s );
    arrow();
    return s.modal_ret;
  }
}

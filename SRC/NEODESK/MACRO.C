#include "neodesk.h"   /* moving folder same device tries to delete if Use Ex*/
#include "aes.h"
#include "string.h"
#include "ctype.h"
#include "tos.h"	/* Change to Data, then change mask color. Bad checks?*/
#include "stdlib.h"
#include "stdio.h"
#include "neocommn.h"
#include "neod2_id.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */

extern unsigned int items[7], witems;
extern int d_active, w_active, w_num, use_8x16, snum, w_handle, moving, AES_handle, copyqlen;
extern char filename[], *msg_ptr[], m_on, colon_slash[], slash[], nil[], is_clip, *exec_env;
extern unsigned char *mac, *copy_q;
extern MOST *z;
extern EMULTI emulti;
extern FSTRUCT *wfile;
extern TREE maintree;
extern int wxref[7], w_open;
extern OBJECT *menu, *wmenu[7];
extern NPI_DESC *last_npi;

static unsigned char main_m[] = {
  OPEN, SHOWINF, QUICKINF, SEARCHDR, DELITEM, FORMAT, PRINTDSK,
  FORMFEED, QUIT, MPOPMEM, MPOPFILE, MPOPDESK, MPOPWIND, MPOPDIAL, MPOPPATH,
  MPOPEXT, MPOPINF, MPOPACC, EDITENV, MPOPMISC, MPOPRELD, INSTICON,
  INSTAPP, PROGINFO, BEMACRO, EXCHMAC, SAVEDESK, LOADINF, EDITICON,
  SNAPGRID, CHANGREZ, 0
},
main_xref[] = {		/* 003: mABOUT was first, but it == 0! */
  mOPEN, mSHOWINF, mQUICKINF, mSEARCHDR, mDELITEM, mFORMAT, mPRINTDSK,
  mFORMFEED, mQUIT, mMPOPMEM, mMPOPFILE, mMPOPDESK, mMPOPWIND, mMPOPDIAL, mMPOPPATH,
  mMPOPEXT, mMPOPINF, mMPOPACC, mEDITENV, mMPOPMISC, mMPOPRELD, mINSTICON,
  mINSTAPP, mPROGINFO, mBEMACRO, mEXCHMAC, mSAVEDESK, mLOADINF, mEDITICON,
  mSNAPGRID, mCHANGREZ, 0
},
dir_m[] = {
  WIMOPEN, WIMSHOW, WIMQUICK, WIMSRCH, WIMDEL, CREATE, CLOSEFLD, CLOSEWIN,
  SELALL, WIMGRP, SHOWICON, SHOWTEXT, STLGSML, STCOLUMN, STSIZE, STDATE,
  STTIME, SORTFILT, PRINTDIR, UPDATE, FUPDATE, SORTNAME, SORTDATE, SORTSIZE,
  SORTTYPE, SORTNONE, REORDER, WIMAPP, WIMNPI, WIMLOAD, 0
},
dir_xref[] = {
  mWIMOPEN, mWIMSHOW, mWIMQUICK, mWIMSRCH, mWIMDEL, mCREATE, mCLOSEFLD, mCLOSEWIN,
  mSELALL, mWIMGRP, mSHOWICON, mSHOWTEXT, mSTLGSML, mSTCOLUMN, mSTSIZE, mSTDATE,
  mSTTIME, mSORTFILT, mPRINTDIR, mUPDATE, mFUPDATE, mSORTNAME, mSORTDATE, mSORTSIZE,
  mSORTTYPE, mSORTNONE, mREORDER, mWIMAPP, mWIMNPI, mWIMLOAD, 0
},
grp_m[] = {
  GWIMOPEN, GWIMSHOW, GWIMCLOS, GWIMSEL, GWIMDEL, GWIMSAVE, GWIMICON, GWIMTEXT,
  GWIMLGSM, GWIMNAME, GWIMTYPE, GWIMSTYP, GWIMSPTH, GWIMUPDT, GWIMAPP,
  GWIMSNAP, GWIMCHNG, GWIMLOAD, 0
},
grp_xref[] = {
  mGWIMOPEN, mGWIMSHOW, mGWIMCLOS, mGWIMSEL, mGWIMDEL, mGWIMSAVE, mGWIMICON, mGWIMTEXT,
  mGWIMLGSM, mGWIMNAME, mGWIMTYPE, mGWIMSTYP, mGWIMSPTH, mGWIMUPDT, mGWIMAPP,
  mGWIMSNAP, mGWIMCHNG, mGWIMLOAD, 0
},
grp_dir_xref[] = {	/* 003 */
  mWIMOPEN, mWIMSHOW, mCLOSEWIN, mSELALL, mWIMDEL, -1, mSHOWICON, mSHOWTEXT,
  mSTLGSML, mSORTNAME, mSORTTYPE, -1, -1, mUPDATE, mWIMAPP,
  -1, -1, mWIMLOAD, 0
};

int find_wpl( int wh, int num )
{
  if( !strncmp( filename, z->w[num].path, pathend(z->w[num].path)-1 ) )
  {
    w_num = num;
    set_wfile();
    w_handle = wh;
    return(1);
  }
  return(0);
}
int find_wn( char *path )
{
  strcpy( filename, path );
  return( find_place( find_wpl ) );
}
int find_wi( char *str, int type, int *num )
{
  register int i;
  FSTRUCT *fs;

  for( fs=wfile, i=0; i<witems; i++, fs++ )
    if( fs->type.p.pexec_mode == type && !strcmp( fs->name, str ) )
    {
      *num = i;
      return(1);
    }
  return(0);
}
int f_dl( int i, long ch )
{
  return( z->idat[i].c == ch );
}
int f_di( int i, long type )
{
  return( z->idat[i].type == type );
}
int fdldi( long n, int *num, int func( int i, long j ) )
{
  register int i;

  for( i=0; i<z->num_icons; i++ )
    if( (*func)( i, n ) )
    {
      *num = i;
      return(1);
    }
  mac_err();
  return(0);
}
int find_dl( int ch, int *num )
{
  return( fdldi( ch, num, f_dl ) );
}
int find_di( int type, int *num )
{
  return( fdldi( type, num, f_di ) );
}
/********************************************************************/
int _mac_add( char *ptr, int len, int off )
{
  if( !add_string( (void **)&z->macro, &z->macro_len, &z->macro_rem, 0L, 20, len, ALLOC_MAS ) )
  {
    f_alert1( msg_ptr[108] );
    return 0;
  }
  memcpy( z->macro+z->macptr+off, ptr, len );
  return 1;
}
int _add_macro( int type, void *parms );
int add_macro( int type, ... )
{
  if( !z->macro_func || (*z->macro_func)( type, &... ) )
      return _add_macro( type, &... );
  return 0;
}
int _add_macro( int type, void *parms )
{
  int len, i, j, k;
  char *ptr, temp[120];

  if( z->macr_rec || type==MACLOAD )
  {
    len = 0;
    switch( type )
    {
      case MACLOAD:
        ptr = *(char **)parms;
        len = *(((int *)parms) + 2);
        break;
      case MACINT:
	ptr = (char *)parms;
	len = sizeof(int);
	break;
      case MACSTR:
	ptr = *(char **)parms;
	len = strlen(ptr)+1;
	break;
      case MACCHR:
	ptr = (char *)parms + 1;
	len = sizeof(char);
	break;
      case MACWIND:
	if( !add_macro( MACCHR, *(int *)parms ) ) return(0);
      case MACPTH:
	strcpy( ptr = temp, z->w[w_num].path );
	temp[len = pathend(ptr)] = '\0';
	len++;
	break;
      case MACSELD:
	if( !add_macro( MACCHR, MSELD ) ) return 0;
	len = z->macptr;
	if( !add_macro( MACCHR, 0 ) ) return 0;
	  for( i=j=0; i<z->num_icons; i++ )
	    if( z->idat[i].state&1 )
	    {
	      j++;
	      if( !add_macro( MACICON, i ) ) return 0;
	    }
	*(z->macro+len) = j;
	return 1;
      case MACICON:
        i = *(int *)parms;
	switch( z->idat[i].type )
	{
	  case FLOPPY:
	  case HARDDSK:
	  case RAMDISK:
	    k = z->idat[i].c;
	    break;
	  default:
	    k = z->idat[i].type;
	}
	return add_macro( MACCHR, k );
      case MACCPYQ:
        if( !add_macro( MACCHR, *(copy_q+2) ) ||
            !add_macro( MACINT, len = (*copy_q<<8) + *(copy_q+1) - 1 ) ) return 0;
        ptr = (char *)copy_q + 3;
    }
    if( !z->macro_len && type!=MACLOAD )
    {
      if( !_mac_add( "\0\0\0", 4, 0 ) ) return 0;
      if( !_mac_add( ptr, len, 2 ) )
      {
        z->macr_rec = 0;
        return 0;
      }
      len += 2;
    }
    else
    {
      i = 0;
      if( z->macstrt==z->macptr && type!=MACLOAD )
      {
        if( !_mac_add( "\0", 2, 0 ) )
        {
          z->macr_rec = 0;
          return 0;
        }
        i = 2;
      }
      if( !_mac_add( ptr, len, i ) )
      {
        *(z->macro+z->macstrt) = *(z->macro+z->macstrt+1) = 0;
        z->macr_rec = 0;
        return 0;
      }
      len += i;
    }
    z->macptr += len;
    if( type==MACLOAD ) return 1;
    i = chtoi(z->macro+z->macstrt) + len;
    *(z->macro+z->macstrt) = i>>8;
    *(z->macro+z->macstrt+1) = i;
    *(z->macro+z->macptr) = *(z->macro+z->macptr+1) = '\0';
    return(1);
  }
  return(0);
}

int mseld_icons( int count )
{
  int k, m;

  while( --count>=0 )
  {
    k = *(z->macro+z->macptr++);
    if( k>='A' ? find_dl( k, &m ) : find_di( k, &m ) ) select_d( m+1, SELECTED );
    else return(0);
  }
  return 1;
}

int mac_seld( int *ind )
{
  if( !mseld_icons( *(z->macro+z->macptr++) ) ) return 0;
  get_top_wind();
  *ind = *(z->macro+z->macptr++);
  return(1);
}
void mac_resp( int ret, int id )
{
  int buf[8];
  
  buf[0] = NEO_MAC_RESP;
  buf[1] = AES_handle;
  buf[2] = 0;
  buf[3] = NEO_ACC_MAGIC;
  buf[4] = ret;
  appl_pwrite( id, 16, buf );
}

void rcv_mac_msg( int buffer[] )
{
  int i;
  
  if( buffer[3]==NEO_ACC_MAGIC ) switch( buffer[0] )
  {
    case NEO_MAC_PLAY:
      if( z->macr_rec || z->macr_play ) mac_resp( 0, buffer[4] );
      else
      {
        z->old_macro = z->macro;
        z->macro = *(char **)&buffer[5];
        z->mac_resp = buffer[4];
        z->macstrt = 0;
	z->macptr = sizeof(int) + sizeof(int) + 21;
        z->macr_play = 1;
      }
      break;
    case NEO_MAC_ADD:
      if( z->macr_rec ) 
      z->macr_rec = 1;
      i = _add_macro( buffer[3], *(void **)&buffer[4] );
      z->macr_rec = 0;
      mac_resp( i, buffer[4] );
  }
}

void end_mac_play(void)
{
  if( z->old_macro )
  {
    z->macro = z->old_macro;
    z->old_macro = 0L;
    mac_resp( 1, z->mac_resp );
  }
}

void macro_ind(void)
{
  Rect box;
  int i;

  if( (i = z->macr_rec || z->macr_play) != m_on )
  {
    m_on = i;
    set_clp_rect( &box, 0 );
    gtext( 0, 1, m_on ? "M" : " ", 1+use_8x16, 0 );
    if( !z->macr_play ) end_mac_play();
  }
}
void free_macros(void)
{
  cmfree( &z->macro );
  z->macro_len = z->macptr = 0;
}
unsigned int chtoii(void)
{
  unsigned int i;

  i = chtoi(z->macro+z->macptr);
  z->macptr += sizeof(int);
  return(i);
}
void mac_err(void)
{
  f_alert1( msg_ptr[59] );
  z->macr_play = 0;
}
void next_macs(void)
{
  z->macptr += strlen(z->macro+z->macptr)+1;
}

int place_ok( int place )
{
  int num;
  
  place = w_open-place;
  for( num=7; --num>=0; )
    if( wxref[num] >= 0 && z->w[num].place == place ) return num;
  return -1;
}

static int xref( int to_mac, unsigned char *x, unsigned char *m, int num )
{
  char *p;
  
  if( !to_mac )
  {
    p = m;
    m = x;
    x = p;
  }
  if( (p=strchr(m,num)) == 0 ) return -1;
  return *(x+(p-m));
}

int macxref( int notgrp, int num )
{
  return xref( notgrp, grp_dir_xref, grp_xref, num );
}

void use_menu_mac( int num, int type, int place )
{
  int i, w, h;
  OBJECT *o;
  
  if( place>=0 )
  {
    if( (w=place_ok(place)) < 0 )
    {
/*      mac_err();   002 removed */
      return;
    }
    if( ed_wind_type(w) == EDW_GROUP )
    {
      if( type != MGMENU )
        if( (num = macxref( 0, num )) < 0 )	/* 003 */
        {
          mac_err();
          return;
        }
        else type = MGMENU;
    }
    else if( type != MWMENU )
      if( (num = macxref( 1, num )) < 0 )	/* 003 */
      {
        mac_err();
        return;
      }
      else type = MWMENU;
  }
  else if( !w_open )
  {
/*    mac_err();   002 removed */
    return;
  }
  else w = 0;
  i = mac_mxref( type, num, 0 );
  if( type==MCMENU )
  {
    o = menu;
    h = -1;
  }
  else
  {
    o = wmenu[w];
    h = wxref[w];
  }
  if( !(o[i].ob_state&DISABLED) ) use_menu( h, i );
}
int main_macro( int buffer[] )
{
  int i, m, del, j, k, l, err, foo;
  PROG_TYPE pt;
  char *ptr, temp[120], tmpf[120], (*npi_parm)[5][39];
  struct Wstruct *ws;
  extern char app_mac;
  
      if( z->macptr >= z->macstrt + chtoi(z->macro+z->macstrt) || emulti.mouse_k==3 )
	  z->macr_play = 0;		/* end of macro or both Shift keys */
      else
      {
	if( d_active>=0 || w_active>=0 )
	{
	  de_act( -1, -1 );
	  info();
	}
	switch( i = *(z->macro+z->macptr++) )
	{
	  case MCMENU:
	  case MGMENU:
	  case MWMENU:
	    j = i;
	    switch( i = *((unsigned char * /*002*/)z->macro+z->macptr++) )
	    {
	      case MSELW:
		strcpy( filename, z->macro+z->macptr );
		next_macs();
		if( (i=*(z->macro+z->macptr++)) == mLOADINF ||
		    i==mWIMLOAD || i==mGWIMLOAD ) reload_inf(1);
		else if( i == mINSTAPP || i==mWIMAPP || i==mGWIMAPP )
		{
		  app_mac = 1;
                  start_form( WAPP_FORM );
                }
                else use_menu_mac( i, j, -1 );
		break;
	      case MSELD:
		if( !mac_seld( &m ) ) break;
		i = m;
	      default:
	        if( j != MCMENU ) use_menu_mac( *(z->macro+z->macptr++), j, i );
		else use_menu_mac( i, j, -1 );
	    }
	    break;
	  case MOPDL:
	    filename[0] = *(z->macro+z->macptr++);
	    strcpy( filename+1, colon_slash );
	    open_to_path(filename);
	    break;
	  case MOPDP:
	    open_prn();
	    break;
	  case MOPDF:
	    open_to_path(z->macro+z->macptr);
	    next_macs();
	    break;
	  case MOPWF:
	    ptr = z->macro+z->macptr;
	    next_macs();
	    if( find_wn( ptr ) && find_wi( z->macro+z->macptr, FOLDER, &m ) )
	    {
	      next_macs();
	      open_folder(m);
	    }
	    else
	    {
	      strcat( filename, z->macro+z->macptr );
	      strcat( filename, slash );
	      next_macs();
	      open_to_path(filename);
	    }
	    break;
	  case MOPNPI:
	    ptr = z->macro+z->macptr;
	    next_macs();
	    pt = iprog_type( -1, ptr );
	    if( !last_npi ) break;
	    if( npi_type( ptr, 0L ) != PROG ) break;
	    pt.p.pexec_mode = 0;
	    z->new_cache = last_npi->npi.af.i;
	    npi_argv(0);
	    npi_parm = &last_npi->npi.params;
	    strcpy( filename, last_npi->npi.path );
	    filename[l = pathend(filename)] = 0;
	    open_program( -1, last_npi->npi.path+l, filename,
	        pt, nil, npi_parm, 1, 1 );
	    z->new_cache = -1;
	    exec_env = 0L;
	    npi_argv(1);
	    break;
	  case MOPRG:
	    pt.i = chtoii();
	    strcpy( ptr=filename, z->macro+z->macptr );
	    next_macs();
	    strcpy( temp, z->macro+z->macptr );
	    next_macs();
	    strcpy( tmpf, z->macro+z->macptr );
	    next_macs();
	    if( *(z->macro+z->macptr++) )
	      if( check_batch(0) ) ptr = NULL;
	      else break;
	    open_program( -1, temp, ptr, pt, tmpf, 0L, 1, 1 );
	    break;
	  case MOPT:
	    strcpy( filename, z->macro+z->macptr );
	    next_macs();
	    open_text( nil, nil, 0 );
	    break;
	  case MCOPF:
	  case MDELF:
	  case MCAF:
	    err = 0;
	    k = -1;
	    l = (j = chtoii()) + 1;
	    if( add_copy_str( (char *)&l, 2 ) )
	    {
	      k = copyqlen - 2;
	      temp[0] = i;
	      if( !add_copy_str( temp, 1 ) ||
	          !add_copy_str( z->macro+z->macptr, j ) ) err++;
	      else z->macptr += j;
	    }
	    else err++;
	    if( err )
	    {
	      if( k>=0 ) *(copy_q+k) = *(copy_q+k+1) = 0;
	      z->macr_play = 0;
	    }
	    break;
	  case MRENF:
	    start_form( RENF_FORM );
	    break;
	  case MPRNF:
	    print_file( z->macro+z->macptr );
	    next_macs();
	    break;
	  case MLIST:
	    list_setup( 0, -1 );
	    break;
	  case MDELALL:
	    filename[0] = *(z->macro+z->macptr++);
	    strcpy( filename+1, colon_slash );
	    trash_all(filename);
	    update_drive( filename, 0 );
	    break;
	  case MNEWVOL:
	    ptr = z->macro+z->macptr;
	    next_macs();
	    new_volname( w_num, ptr, *(z->macro+z->macptr++) );
	    break;
	  case MDUPLIC:
	    if( w_num>=0 ) return i;
	    break;
	  case MACOPEN:
	    neo_ac_open( z->macro+z->macptr );
	    next_macs();
	    break;
	  default:
	    if( find_wn( z->macro+z->macptr ) )
	    {
	      next_macs();
	      set_window();
              ws = &z->w[w_num];
	      switch(i)
	      {
		case MNEWPTH:
		  i = *(z->macro+z->macptr++);
		  new_path( i, 1 );
		  break;
		case MWSALL:
		  select_all();
		  break;
		case MWIOT:
		  use_gmenu( -1, z->showicon[w_num] ? SHOWTEXT : SHOWICON );
		  break;
		case MWTOP:
		  buffer[3] = w_handle;
		  get_top_wind();
		  return i;
		case MWSPLIT:
		  buffer[4] = chtoii();
		  return i;
		case MWMOVE:
		  buffer[4] = chtoii();
		  buffer[5] = chtoii();
		  *(long *)&buffer[6] = *(long *)&ws->w;
		  return i;
		case MWSIZE:
		  buffer[4] = chtoii();
		  buffer[5] = chtoii();
		  buffer[6] = chtoii();
		  buffer[7] = chtoii();
		  return i;
		case MWARROW:
		  foo = chtoii();
		  j = *(z->macro+z->macptr++);
		  if( (snum = *(z->macro+z->macptr++)) == 0 && !ws->split ) snum++;
	          set_wfile();
		  set_window();
		  arrowed( foo, j, 1 );
		  break;
		case MWCLOSE:
		  close_wind();
		  break;
		case MWFULL:
		  return i;
	      }
	    }
	    else if( i == MWTOP ) next_macs();	/* 003 */
	    else mac_err();
	}
      }
  return -1;
}

int mac_mxref( int type, int num, int to_mac )
{
  if( type==MCMENU )
  {
    if( !to_mac && num==mABOUT ) return ABOUT;	/* 003 */
    if( to_mac && num==ABOUT ) return mABOUT;	/* 003 */
    return xref( to_mac, main_xref, main_m, num );
  }
  if( type==MGMENU ) return xref( to_mac, grp_xref, grp_m, num );
  return xref( to_mac, dir_xref, dir_m, num );
}

void mac_rec_icons( SELICON_DESC *f )
{
  int n, w;
  SEL_ICON *s;
  
  if( !z->macr_rec ) return;
  s = f->icons;
  w = s->wnum;
  if( !add_macro( MACCHR, MICONS ) || !add_macro( MACCHR,
      w>=0 ? w_open-z->w[w].place : -1 ) || !add_macro( MACINT, n=f->nicons ) )
      return;
  for( ; --n>=0; s++ )
    if( w>=0 )
    {
      if( !add_macro( MACSTR, s->u.fs->name ) ) return;
    }
    else if( !add_macro( MACICON, s->u.desk_item-1 ) ) return;
}

void mac_play_icons(void)
{
  int pl, w, n;
  FSTRUCT *fs;
  unsigned int i;
  
  if( !z->macr_play ) return;
  if( *(z->macro+z->macptr)==MICONS )
  {
    z->macptr++;
    pl = *(z->macro + z->macptr++);
    if( pl>=0 && (w=place_ok(pl)) < 0 ) return;
    n = chtoii();
    if( pl<0 ) mseld_icons(n);
    else
      while( --n>=0 )
      {
        for( fs=z->file[w], i=0; i<items[w]; fs++, i++ )
          if( !strcmp( z->macro+z->macptr, fs->name ) )
          {
            next_macs();
            select_w( i, 1, wxref[w], 1 );
            break;
          }
        if( i==items[w] )
        {
          mac_err();
          return;
        }
      }
  }
}

SEL_ICON *get_msel_icon( SELICON_DESC *f, int reselect, int record )
{
  SEL_ICON *s;
  char r, p;
  
  r = z->macr_rec;
  p = z->macr_play;
  if( !record ) z->macr_rec = 0;
  z->macr_play = 0;
  s = get_sel_icon( f, reselect );
  if( !record ) z->macr_rec = r;
  z->macr_play = p;
  return s;
}

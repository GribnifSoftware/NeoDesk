#define linea_init { int x; }
#include "aes.h"
#include "string.h"
#include "tos.h"
#include "lerrno.h"
#include "stdlib.h"
#include "stdio.h"
#include "neodesk.h"
#include "neocommn.h"
#include "neod2_id.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */
extern GUI *gui;

extern MOST *z;
extern OBJECT *menu, *popups;
extern int w_active, num_w_active, d_active, w_num, w_handle, w_open;
extern char filename[125], iconedit, *msg_ptr[], *last_buf, ext[3][5];
extern char know_icons[7], reorder_on, macr_all, macr_ret;
extern ICIC *icic;
extern MASTER *mas;
extern unsigned int items[7];
extern FSTRUCT *wfile;
extern GROUP_DESC *group_desc[7];
extern FORM_TYPE formst[];
extern int num_forms;
extern unsigned int witems;

/********************************************************************/
void use_menu( int wind, int item )
{	/* wind is **handle** */
  register int i, j, k, l, mac_num;
  unsigned char *ptr;
  static char old_rec, old_play, noerr;
  extern ICNEO icneo;
  FORM_TYPE *f;
#ifndef DEBUG
  static char *start=0L;
#endif

    j = wind>=0 && w_num>=0 ? ed_wind_type(w_num)==EDW_GROUP : 0;
    if( (wind>=0 || item != BEMACRO && item != OPEN) &&
        (wind<0 || j && item != GWIMOPEN || !j && item != WIMOPEN) )
    {
      if( wind>=0 && w_num>=0 ) k = j ? MGMENU : MWMENU;
      else k = MCMENU;
      mac_num = mac_mxref( k, item, 1 );
      if( w_active>=0 && (mac_num==mLOADINF || mac_num==mWIMLOAD || mac_num==mGWIMLOAD ||
          mac_num==mINSTAPP || mac_num==mWIMAPP || mac_num==mGWIMAPP) )  /* 002: added desktop */
      {
        l = w_num;		/* 003: used to be j */
        w_num = num_w_active;
        set_wfile();
        noerr = 1;
        for( i=0; i<witems && noerr; i++ )
          if( wfile[i].state )
            if( (noerr = add_macro( MACCHR, k )) != 0 &&
                (noerr = add_macro( MACCHR, MSELW )) != 0 )
            {
              get_full_name( filename, i, w_num );
              if( (noerr = add_macro( MACSTR, filename )) != 0 )
                  noerr = add_macro( MACCHR, mac_num );
            }
        w_num = l;		/* 003: used to be j */
        set_wfile();
      }
      else if( add_macro( MACCHR, k ) != 0 && 
          (d_active<0 || add_macro( MACSELD )) &&
          (k==MCMENU || add_macro( MACCHR, w_open-z->w[w_num].place )) )
          add_macro( MACCHR, mac_num );
    }
    i = item;
    if( iconedit && wind<0 && (i = (*icic->trans_mmenu)( item, 0 )) != 0 ) item = i;
    else if( iconedit && wind>0 && w_num<0 )
    {
      (*icic->use_menu)( w_num, -item );
      return;
    }
    else if( wind>=0 && (i = trans_gmenu( w_num, item, 0 )) != 0 ) item = i;
    if( iconedit && !i )
    {
      if( (i=(*icic->use_menu)( wind<0 ? -1 : w_num, item )) != 0 )
      {
        if( i==1 )
        {
#ifndef DEBUG
          Mfree( start );
#endif
          z->macr_rec = old_rec;
          z->macr_play = old_play;
          iconedit = reorder_on = 0;
          set_newdesk( z->desk, 1 );
          reset_desktop();
#ifdef USE_NEO_ACC
          neo_acc_init();
#endif
          all_inactive();
        }
        form_dial( FMD_FINISH, 0, 0, 0, 0, Xrect(*(Rect *)&z->desk[0].ob_x) );
        open_all( 1, 0 );
      }
      return;
    }
    for( k=0, f=formst; k<LAST_FORM; f++, k++ )
      if( item==f->menu_num && (wind>=0 && f->flags.wmenu ||
          wind<0 && f->flags.dmenu || j && f->flags.group) )
      {
        start_form(k);
        return;
      }
    if( j && !i )
      switch( item )	/* group menu */
      {
        case GWIMSTYP:
          group_desc[w_num]->hdr.opts.s.showtype ^= 1;
          set_attrib(1);
          break;
        case GWIMSPTH:
          group_desc[w_num]->hdr.opts.s.showpath ^= 1;
          set_attrib(1);
          break;
        case GWIMSAVE:
          m_savegrp(0);
          break;
      }
    else if( wind>=0 )
    {
      switch( item=i )
      {
        case WIMSRCH:
          m_search();
          break;
        case WIMOPEN:
          m_open();
          break;
        case WIMSHOW:
          m_showinf( 0, 1 );
          break;
        case WIMQUICK:
          m_showinf( 0, 0 );
          break;
        case CLOSEFLD:
          i = z->macr_rec;
          z->macr_rec = 0;
          backup(1);
          z->macr_rec = i;
          break;
        case CLOSEWIN:
          close_wind();
          break;
        case SELALL:
          select_all();
          info();
          break;
        case SHOWICON:
        case SHOWTEXT:
          i = SHOWTEXT-item;
          if( z->showicon[w_num] != i )
            if( (z->showicon[w_num]=i) != 0 )
            {
              if( !know_icons[w_num] ) get_icn_matches(w_num);
              set_attrib(2);
            }
            else if(j)		/* 003: sort group if text */
            {
              i = z->sort_type[w_num];
              z->sort_type[w_num] = -1;
              new_sort(i);
              set_attrib(3);
            }
            else set_attrib(2);
          break;
        case STLGSML:
          z->stlgsml[w_num] ^= 1;
          set_attrib(1);
          break;
        case STSIZE:
        case STDATE:
        case STTIME:
          i = item-STSIZE;
/*%          if( w_num >= 0 ) */
          z->sizdattim[w_num][i] ^= 1;
/*%          else sizdattim[i] ^= 1; */
          set_attrib(1);
          break;
        case STCOLUMN:
/*%          if( w_num >= 0 )*/
        z->stcolumn[w_num] ^= 1;
/*%          else stcolumn ^= 1; */
          set_attrib(1);
          break;
        case FUPDATE:
          m_update(1);
          break;
        case UPDATE:
          m_update(0);
          break;
        case SORTNAME:
        case SORTDATE:
        case SORTSIZE:
        case SORTTYPE:
        case SORTNONE:
          new_sort( item-SORTNAME );
          break;
        case REORDER:
          m_reorder();
          break;
        case WIMLOAD:
          reload_inf(0);
          break;
      }
    }
    else switch( item=i )
    {
      case QUIT:
        m_quit(0);
        break;
      case OPEN:
        m_open();
        break;
      case QUICKINF:
        m_showinf( 0, 0 );
        break;
      case SHOWINF:
        m_showinf( 0, 1 );
        break;
      case SEARCHDR:
        m_search();
        break;
      case PRINTDSK:
        m_printdsk();
        break;
      case LOADINF:
        reload_inf(0);
        break;
      case BEMACRO:
        if( (z->macr_rec ^= 1) != 0 )
        {
          z->macptr = 0;
          if( z->macro_len )
          {
            while( *(z->macro+z->macptr) || *(z->macro+z->macptr+1) )
                z->macptr += chtoi(z->macro+z->macptr);
            z->macstrt = z->macptr;
          }
          else z->macstrt = 0;
          if( /*%!add_macro( MACINT, 0 ) ||*/ !add_macro( MACINT, 0 ) ||
              !add_macro( MACSTR, msg_ptr[110] ) ) z->macr_rec = 0;
        }
        else
        {
          macr_all = 0;
          if( start_form(MACR_FORM) && !macr_ret ) z->macr_rec = 1;
        }
        break;
      case EXCHMAC:
        z->macstrt = 0;
        macr_all = 1;
        start_form(MACR_FORM);
        break;
      case FORMFEED:
        m_formfeed();
        break;
      case MPOPRELD:
#ifndef DEMO
        strcpy( filename, z->dflt_path );
        strcat( filename, z->inf_name );
        strcat( filename, ext[0] );
        if( (j = cFopen( filename, 0 )) == AEFILNF )
        {
          spf( filename, msg_ptr[34], z->inf_name );
          f_alert1( filename );
        }
        else if( TOS_error((long)j, 0) )
        {
          cFclose(j);
          really_reload_inf( NULL, 0, 0 );
        }
#else DEMO
        demo_version();
#endif DEMO
	break;
      case EDITICON:
        if( last_buf )
          if( f_alert1( msg_ptr[97] ) == 1 ) free_clip();
          else break;
        bee();
#ifdef DEBUG
        if( (icic = (ICIC *)ic_main( &icneo, 1 )) != 0 )
#else
        if( (long)(icic = (ICIC *)(*mas->exe_load)( 1, "ICONEDIT.EXE", &icneo,
            1, &start, 0 )) > 0 )
#endif
        {
          iconedit++;
          old_rec = z->macr_rec;
          old_play = z->macr_play;
          z->macr_rec = z->macr_play = 0;
#ifdef USE_NEO_ACC
          write_acc_bad();
#endif
        }
        arrow();
        break;
    }
/*%  } */
}


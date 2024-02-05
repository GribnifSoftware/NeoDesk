#include "neodesk.h"
#include "new_aes.h"
#include "xwind.h"
#include "string.h"
/*%#include "ctype.h"*/
#include "tos.h"
#include "stdlib.h"
#include "stdio.h"
#include "lerrno.h"
#include "ierrno.h"
#include "neocommn.h"
#include "neod2_id.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */

#define COPY_SIZE(c)	((c+1)*10240L)

#define TRANFILE    75

int r_hand, w_hand, *rw_file, *rw_fold, cancel, conflict;
char *rw_auxpath, r_first, w_first, clip_ndir, clip_src[120], cnover_mode;
copy_struct *r_cs, *w_cs;
long r_len, w_len;

extern int moving, w_num, copyqlen, nameconf_ret;
extern char *c_buf, *c_curbuf, *last_buf, update_clip, is_clip, TOS_abort,
     *ocur, *obuf, jog_background, *foldc_old, *foldc_new, *nameconf_old,
     *nameconf_new;
extern long c_buflen, c_bufmax, clip_seek;
extern char diskbuff[512*2], filename[125], tmpf[156], slash[], *msg_ptr[], fvalid[], copy_slice;
extern unsigned int witems;
extern unsigned char *copy_q;
extern unsigned char dest_off;
extern MOST *z;
extern TREE maintree;
extern struct
{
  int num;
  ERRSTRUC *errstruc;
} fold_err;
extern OBJECT *form;
extern FSTRUCT *wfile;
extern DTA *nameconf_sdta, *nameconf_ddta;
extern FORM_TYPE formst[];
extern int file_form;

/**********************************************************************/
void copybee(void)
{
  if( copy_slice<0 ) bee();
}
int test_filename( char *from, char *to, int flag )
{
  from_filename( from, to, flag );
  if( z->other_pref.b.check_fnames ) do
    if( (*to<'A' || *to>'Z') && (*to<'0' || *to>'9') && !strchr(fvalid,
	*to ) )
    {
      f_alert1( msg_ptr[117] );
      return(0);
    }
  while( *to++ );
  return(1);
}
char get_over(void)
{
  if( cnover_mode == 3 ) return z->conf_over == (Getshift()==4);
  return cnover_mode;
}
int start_cform( int num )
{
/*  int i, ret;
  extern FORM *forms;
  
  i = file_form - forms;
  ret = start_form( num );
  file_form = forms+i;
  return ret; 003 */
  return start_form( num );
}
int cdecl check_dir( char *path, int draw )
				/* make sure this directory does not already */
				/* exist.   ret: 0=Abort, 1=not there, */
				/*     -1=use existing */
{
  int i, ret=1;
  char new[15];
  DTA dma, *odta;

  if( too_many_dirs( path ) ) return(0);    /* too far down the tree? */
  cDsetdrv( *path - 'A' );
  foldc_old = path + (i = pathend(path));
  odta = (DTA *)Fgetdta();
  Fsetdta(&dma);
  while( ret>0 && (!dsetpath( path ) || !cFsfirst( path, 0x37 )) )
    if( get_over() ) ret=-1;
    else
    {
      to_filename( path+i, foldc_new=new );
      switch( start_cform( OCON_FORM ) )
      {
	case FLDCMAKE:		  /* copy the new directory name and re-check */
	  if( new[0] ) strcpy( path+i, new );
	  break;
	case FLDCUSE:		  /* use existing directory */
	  ret = -1;
	  break;
	default:		  /* Abort button */
	  ret = 0;
      }
      if( draw && ret ) fop_nums( -1, path );
    }
  Fsetdta(odta);
  return(ret);
}
/********************************************************************/
void clip_ptrs( long len )
{
  if( last_buf ) ((BUF_HDR *)last_buf)->nextbuf = (BUF_HDR *)c_buf;
  ((BUF_HDR *)c_buf)->bufsiz = len;
  ((BUF_HDR *)c_buf)->nextbuf = (BUF_HDR *)0L;
  ((BUF_HDR *)c_buf)->lastbuf = (BUF_HDR *)last_buf;
}
static char did_clip;
static long copy_size;
long get_copy_size(void)
{
/*  long l;
  
  if( (l=copy_size) <= 0 )
  {
    copy_size = COPY_SIZE(copy_slice);
    if( l==-1L ) return copy_size;	/* first time */
    return 0L;
  } */
  if( copy_size<=0 ) copy_size = COPY_SIZE(copy_slice);
  return copy_size;
}
void cdecl copy_init(void)
{
  long len;
  
  update_clip=0;
  if( (len = (long)lalloc(-1L,0)) > 20000L ) len -= 20000L;
  else len = 0L;
  c_buflen = len & 0xFFFFFFFEL;
  if( c_buflen<=sizeof(diskbuff) || (c_buf=(char *)lalloc(c_buflen,-1)) == 0 )
  {
    c_buf = diskbuff;
    c_buflen = is_clip ? -1L : sizeof(diskbuff);
  }
  if( (long)c_buf&1 )
  {
    (long)c_buf &= 0xFFFFFFFEL;
    c_buflen--;
  }
  if( is_clip && c_buflen > 0 ) clip_ptrs(c_buflen);
  c_curbuf = c_buf + sizeof(BUF_HDR);
  c_bufmax = c_buflen -= sizeof(BUF_HDR);
  did_clip = 0;
  copy_size = -1L;
  copy_slice = -1;
  cnover_mode = 3;
  file_form = -1;	/* 003: was 0L when file_form was type FORM */
}
/********************************************************************/
void cdecl copy_free(void)
{
  if( is_clip )
  {
    if( c_buf )
    {
      if( c_buflen != c_bufmax )
      {
	last_buf = c_buf;
	lshrink( c_buf, ((BUF_HDR *)c_buf)->bufsiz =
	    c_bufmax-c_buflen+sizeof(BUF_HDR) );
      }
      else if( c_bufmax > 0 )
      {
	lfree(c_buf);
	if( last_buf ) ((BUF_HDR *)last_buf)->nextbuf = 0L;
      }
      c_buf = 0L;
    }
    is_clip = 0;
  }
  else if( c_buf != diskbuff ) cmfree(&c_buf);
  if( update_clip ) update_drive( "0:\\", 0 );
  update_clip = 0;
  dest_off = 0;
}
/********************************************************************/
int cdecl copy_files( char *src, char *dest, int *file, int *fold,
		      int ignore_1st )
			       /* *src should not end with slash unless a dir */
			       /* *dest should since it is a directory */
{
  char temp[120], temp2[120], err=0, *ptr;
  register int ret, i;
  char new_dir=0;

  bee();
  i = pathend(src);
  strcpy( temp, dest );
  if( ignore_1st ) dest_off = strlen(temp);
  else
  {
    ret = pathend(temp);
    if( !*(src+i) )
      for( ptr=temp+(--ret); *--ptr != '\\'; ret-- );
    dest_off = ret;
  }
  TOS_abort++;
  if( *(src+i) )
  {
    strcat( temp, src+i );
    maintree.tree_valid = 0;	/* don't use DTA values for file attribs */
    err = copy_a_file( src, temp, dest, 0, 0, file, fold );  /* only one file */
  }
  else
  {
    strcpy( filename, src );
    *(filename+i-1) = '\0';		/* search for more files to copy */
    if( moving && *src==CLIP_LET && temp[0]==CLIP_LET )
    {
      *(spathend(temp)-1) = '\0';
      switch( check_dir(temp,1) )
      {
	case 0:
	  err++;
	  goto out;
	case 1:
	  strcat( temp, slash );
	  err = !TOS_error( cFrename( 0, filename, temp ), 0 );
	  goto out;
	case -1:
	  strcat( temp, slash );
	  new_dir--;
      }
    }
    if( (i=pathend(temp)) > 3 && !ignore_1st )
    {
      temp[i] = '\0';
      new_dir++;			/* this directory needs to be created */
    }
    if( tree_init( filename, 0L ) ) while( !err && (ret=tree_next(0L)) != 1 )
    {
      if( ret != -3 )
      {
	strcpy( filename, maintree.tree_path );
	isolate();
      }
      switch( ret )
      {
	case -2:			/* actually moved to new directory */
	  new_dir++;
	  strcat( temp, maintree.tree_fname );
	  strcat( temp, slash );
	  break;
	case -3:			/* backed-up to previous directory */
	  if( new_dir > 0 )
	  {
	    maintree.tree_valid = 1;
	    err = copy_a_file( filename, temp, temp, 1, new_dir, file, fold );
	  }
	  new_dir = 0;
	  if( moving && !err ) err = c_remdir( filename );
	  strcpy( filename, maintree.tree_path );
	  isolate();
	  *(spathend(temp)-1) = '\0';
	  iso(temp);
	  break;
	case 0: 			/* found a file */
	  strcat( filename, maintree.tree_fname );
	  strcpy( temp2, temp );
	  strcat( temp2, maintree.tree_fname );
	  if( new_dir < 0 ) new_dir = 0;
	  maintree.tree_valid = 1;
	  err = copy_a_file( filename, temp2, temp, 0, new_dir, file, fold );
	  new_dir = 0;
      }
    }
    if( new_dir>0 && !err ) err = c_new_dir( temp, 0L, &new_dir,
	0, 1, fold ) > 0;
  }
out:
  TOS_abort = 0;
  arrow();
  return(err);
}
/*************/
int c_remdir( char *path )
{
  char temp[120];
  register copy_struct *last, *cs;

  cs = (copy_struct *)(c_buf+sizeof(BUF_HDR));
  last = (copy_struct *)0L;
  strcpy( temp, path );
  iso(temp);
  while( (char *)cs < c_curbuf )
  {
    if( !strncmp( cs->from, temp, strlen(temp) ) ) last = cs;
    cs = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
  }
  if( last && *last->to != CLIP_LET ) last->del_dir++;
  else if( strlen(temp)>3 )	/* 003: added if */
  {
    strcpy( temp, path );
    iso(temp);
#ifdef DEMO
    if( *temp==CLIP_LET )
#endif DEMO
    return( !TOS_error( cDdelete(temp), fold_err.num,
	fold_err.errstruc ) );
  }
  return(0);
}
/***********************/
int c_new_dir( char *path, char *auxpath, char *n_d, int in_buf,
	       int dcreat, int *fold )
				/* path should have trailing slash on entry */
				/* in_buf signifies if path is in buffer */
{	/* return:  0: Ok   -1: Skip   2: Abort */
  char err=0, temp[120], temp2[120];
  register int i;
  register char new_dir;
  register copy_struct *cs;

  for( new_dir=*n_d; new_dir && err<=0; new_dir-- )
  {
    strcpy( temp, path );
    for( i=0; i<new_dir; i++ )
      *(spathend(temp)-1) = '\0';
    strcpy( temp2, temp );
    if( dcreat ) fop_nums( 5, temp );
    switch( check_dir( temp, 1 ) )
    {
      case -1:
	err = -1;
/*	  (*n_d)--; */
	break;
      case 0:
	err = 2;
	break;
      case 1:
	if( dcreat ) fop_nums( 5, temp );
	if( !dcreat || (err = !dcreate(temp)) == 0 )
	{
	  i = strlen(temp2);
	  if( auxpath && intersect( path, auxpath ) )
	      change_path( auxpath, temp, i );
	  if( !in_buf ) change_path( path, temp, i );
	  cs = (copy_struct *)(c_buf+sizeof(BUF_HDR));
	  while( (char *)cs < c_curbuf )
	  {
	    if( (in_buf || cs->to != path) && !strncmp( cs->to, temp2, i ) )
	       change_path( cs->to, temp, i );
	    cs = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
	  }
	}
	break;
    }
    (*fold)--;
  }
  return(err);
}
/****************/
void change_path( char *old, char *new, int len )
{
  char temp2[120];

  strcpy( temp2, new );
  strcat( temp2, old + len );
  strcpy( old, temp2 );
}
/*******************************************************************/
int copy_confl( copy_struct *cs, int *hand, int *flg, int *cancel,
    int *conflict )
{
  /* hand:	result of Fsfirst
     flg:	name changed, tried to Fdelete the file
     cancel:	skip the file
     conflict:	a conflict occurred */

  char *to, old[120], new[120];
  int i, ret=0;
  DTA dma, ddta, *odta;
  FORM *f;

  odta = (DTA *)Fgetdta();
  Fsetdta(&dma);
  *conflict = *cancel = *flg = 0;
  to = cs->to;
  if( z->rename_dest )
  {
    nameconf_new = to;
    if( !start_cform( NEWN_FORM ) || nameconf_ret==-1 )
    {
      cs->noerr = 0;
      ret = 1;
    }
    else if( nameconf_ret==1 )
    {
      (*cancel)++;
      ret = 1;
    }
    else fop_nums( -1, to );
  }
  while( !*flg && !ret && cs->noerr && (*hand=cFsfirst( to, 0x37 )) == 0 )
  {
    *conflict = 1;
    i = FCONOK;
    if( get_over() ) strcpy( new, to );
    else
    {
      strcpy( nameconf_new = new, to );
bad:  strcpy( nameconf_old = old, to );
      nameconf_sdta = &dma;
      ddta.d_length = cs->len;
      if( cs==r_cs || /*005*/moving && cs->from[0] == to[0] ) ddta.d_length += r_len;	/* add amount yet unread */
      if( cs->state & CS_ODD ) ddta.d_length--;
      *(long *)&ddta.d_time = *(long *)&cs->time;
      nameconf_ddta = /*moving ? 0L 004 :*/ &ddta;
      formst[FCON_FORM].flags.modal = 1;
      if( !start_cform( FCON_FORM ) )
      {
        cs->noerr = 0;
        ret = 1;
        continue;
      }
      else switch( nameconf_ret )
      {
        case 0:
  	  if( strcmp( to, old ) )
  	  {
  	    f = get_fileform();
	    switch( set_filename( &f, 1, -1, -1/*003: was FCONNAME*/, to, old ) )
	    {
	      case -2:
	        goto bad;
	      case -1:
	        i = FCONABRT;
	        break;
	      case 0:
	        i = 0;
	        break;
	      case 1:
	        i = FCONSKIP;
	        break;
	    }
	  }
          fop_nums( -1, new );
	  break;
	case -2:
	  goto bad;
	case -1:
	  i = FCONABRT;
          break;
	case 1:
	  i = FCONSKIP;
	  break;
      }
    }
    if( i == FCONOK )
      if( !strcmp( new, to ) )
        if( !strcmp( cs->from, to ) )
        {
	  (*cancel)++;
	  ret = 1;
	}
	else if( dma.d_attrib & S_IJDIR ) f_alert1( msg_ptr[12] );
	else
	{
#ifdef DEMO
	  if( *to != CLIP_LET ) (*cancel)++;
	  else
#endif DEMO
	  cs->noerr = TOS_error( cFdelete(to), 0 );
	  *flg = 1;
	}
      else strcpy( to, new );
    else if(i)
    {
      if( i==FCONSKIP ) (*cancel)++;
      else cs->noerr = 0;
      ret = 1;
    }
  }
  Fsetdta(odta);
  return(ret);
}
/********************************************************************/
void rw_setup( int read, int hand, copy_struct *cs, long len, char *auxpath, int *file, int *fold )
{
  if( read )
  {
    r_first = 1;
    r_hand = hand;
    r_cs = cs;
    r_len = len;
    rw_auxpath = auxpath;
  }
  else
  {
    w_first = 1;
    w_hand = hand;
    w_cs = cs;
    w_len = len;
  }
  rw_file = file;
  rw_fold = fold;
}
void errdel( copy_struct *cs )
{
  if( cs && !(cs->state&CS_FIRST) )
  {
    cs->noerr = 0;
    cFdelete( cs->to );
  }
}
void err_del(void)
{
  errdel( w_cs );
  errdel( r_cs );
}
int write_fblock(void)
{
  char noerr;
  long l, len;
  static char *w_ptr;
  
  if( w_first )
  {
    w_first = 0;
    w_ptr = (char *)w_cs+sizeof(copy_struct);
  }
  len = w_len;
  if( copy_slice >= 0 )
    if( (l = get_copy_size()) == 0 ) return 1;		/* end of slice */
    else if( l < len ) len = l;
  if( (noerr = !_abort()) != 0 )
  {
    copy_size -= len;
    if( !TOS_error( l=cFwrite( w_hand, len, w_ptr ), 0 ) ) noerr = 0;
    else if( l != len )
    {
      f_alert1( msg_ptr[15] );
      noerr = 0;
    }
    else
    {
      w_ptr += len;
      w_len -= len;
    }
  }
  if( noerr && !w_len ) noerr = -1;
  if( !w_len || !noerr )
  {
    cFclose(w_hand);
    if( !noerr )
    {
      if( !conflict && w_hand>0 ) cFdelete( w_cs->to );
    }
    else if( w_cs->state & CS_LAST )
    {
#ifndef DEMO
      if( (w_hand = cFopen( w_cs->to, 2 )) > 0 )
      {
	cFdatime( (DOSTIME *)w_cs->time, w_hand, 1 );
	cFclose(w_hand);
	cFattrib( w_cs->to, 1, w_cs->att|S_IJWAC );
      }
#else DEMO
      cFdelete( w_cs->to );
      cancel++;
#endif DEMO
    }
    w_cs = 0L;
  }
  return noerr;
}
int move_del( copy_struct *cs )
{
  int i;
  char temp[120];

#ifdef DEMO
  if( *cs->from!=CLIP_LET ) return( cs->noerr );
#endif DEMO
  if( moving && cs->noerr )
  {
    if( !(cs->state & CS_EMPTY) && cs->noerr != 99 && *cs->from != *cs->to
	&& cs->state & CS_LAST )
    {
      fop_nums( 4, cs->from );
      cs->noerr = fdelete( cs->from, 0, cs->att );
    }
    strcpy( temp, cs->from );
    for( ;cs->del_dir && cs->noerr; cs->del_dir-- )
    {
      temp[i=pathend(temp)] = '\0';
      fop_nums( 4, temp );
      cs->noerr = TOS_error( cDdelete(temp), fold_err.num, fold_err.errstruc );
      temp[i-1] = '\0';
    }
  }
  cs->del_dir = 0;
  return( cs->noerr );
}
void finish_write( copy_struct *cs, int *file )
{
  copy_struct cs2;
  char temp[120];
  
  if( cancel )
  {
    cs->noerr = 99;
    r_cs = 0L;
  }
  if( *cs->from != CLIP_LET && cs->noerr ) cs->state &= ~CS_FIRST;
  if( (cs->state & CS_LAST || cancel) && !(cs->state & CS_EMPTY) )
      (*file)--;
  if( obuf && (!moving || !is_clip)/*003*/ )	/* from clip to another drive */
  {
    cs->new_dir = clip_ndir;
    strcpy( temp, cs->to );
    strcpy( cs->to, cs->from );
    if( moving && cs->noerr && !cancel )
    {
      memcpy( &cs2, cs, sizeof(copy_struct) );
      strcpy( cs2.from, clip_src );
      strcpy( cs2.to, temp );
      move_del( &cs2 );
    }
    c_curbuf = ocur;
    c_buf = obuf;
    obuf = NULL;
  }
}
void copy_buf( copy_struct *cs, char *auxpath, int *file, int *fold )
{
  int end_to;
  long len2;
  char temp[120], move, *to, *from;
  int hand, flg;

  cancel = conflict = 0;
  to = cs->to;
  from = cs->from;
  if( cs->new_dir )	/* create new directories */
  {
    if( c_new_dir( to, auxpath, &cs->new_dir, 1, 1, fold ) > 0 )
    {
      cs->noerr=0;
      goto next;
    }
    if( *to != CLIP_LET ) cs->new_dir = 0; /* was removed in 3.04 (for recover to clip?) */
    if( cs->state & CS_EMPTY ) goto next;  /* there is no file there */
  }
  fop_nums( 2+(move = moving && *to == *from), to );
  if( cs->state & CS_FIRST )
  {
    if( copy_confl( cs, &hand, &flg, &cancel, &conflict ) ) goto next;
    end_to = pathend(to);
    if( hand<0 && hand!=IEFILNF ) cs->noerr = TOS_error( (long)hand, 0 );
    if( cs->noerr && (flg || hand < 0) )
    {
      copybee();
      if( move )
      { 			   /* always a return from this block */
#ifdef DEMO
	if( *cs->to!=CLIP_LET ) cancel++;
	else
	{
#endif DEMO
	if( cs->att & 1 ) cs->noerr = TOS_error( cFattrib(
	    from, 1, 0 ), 0 );	      /* unset read-only */
	if( cs->noerr )
	  if( _abort() ) cs->noerr = 0;
	  else if( cFrename( 0, from, to ) )
	  {
	    spf( temp, msg_ptr[13], spathend(from) );
	    f_alert1( temp );
	    cs->noerr = 0;
	  }
	  else if( cs->att & 1 ) cs->noerr = TOS_error( cFattrib(
	      to, 1, cs->att ), 0 );	/* restore read-only */
#ifdef DEMO
	}
#endif DEMO
	goto next;
      }
      hand = cFcreate( to, 0 );
      if( hand==IEACCDN && end_to==3 )
      {
	f_alert1( msg_ptr[14] );
	cs->noerr = 0;
      }
      else cs->noerr = TOS_error( (long)hand, 0 );
      conflict = flg;
      if( *from != CLIP_LET && cs->noerr ) cs->state &= ~CS_FIRST;
/* used to interfere with recovering to clip */
    }
  }
  else
  {
    copybee();
    if( (cs->noerr = TOS_error( (long)(hand =
	cFopen( to, 2 )), 0 )) != 0 ) cFseek( 0L, hand, 2 );
  }
  if( cs->noerr )
  {
    if( cs->len > 0L )
    {
      rw_setup( 0, hand, cs, cs->len - (cs->state & CS_ODD), 0L, file, fold );
      while( cs->noerr>0 )
      {
        cs->noerr = write_fblock();
        if( copy_slice>=0 ) break;
      }
      if( cs->noerr==-1 ) cs->noerr = 1;
    }
next:
    if( !w_cs ) finish_write( cs, file );
  }
}
int clip_already( char *name, copy_struct *last )
{
  copy_struct *cs;

  if( !cFsfirst( name, 0x37 ) ) return 1;
  cs = (copy_struct *)((long)c_buf + sizeof(BUF_HDR));
  while( cs < last )
  {
    if( !strcmp( name, cs->to ) ) return 1;
    cs = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
  }
  return 0;
}
void into_clip(void)
{
  int i, j, noerr; /*, new; */
  copy_struct *cs, *cs2, *next;
  char *lim = c_curbuf, temp[150], temp2[20];
  long size=0L, l;

  if( did_clip || !c_buf /* 003: for kobold */) return;
  did_clip = 1;
  i = z->other_pref.b.clip_mode;
  cs = cs2 = (copy_struct *)((long)c_buf + sizeof(BUF_HDR));
  for( noerr=1; (char *)cs < lim; cs=next )
  { /* get next now because memcpy may change things */
    next = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
    if( noerr ) noerr = cs->noerr;	/* only delete until first error */
    if( noerr )
    {
/*	new = cs->new_dir; */
      move_del( cs );
    }
    else if( cs->state&CS_CLIPOK && c_buf != diskbuff &&
        (!moving || *cs->to!=*cs->from)/*&& cs->dest_off not used */ )
    {
      if( i == 2 ) i = f_alert1( msg_ptr[124] ) - 1;
      if( !i )
      {
	l = cs->len+sizeof(copy_struct);
	temp2[0] = CLIP_LET;
	temp2[1] = ':';
	strcpy( temp2+2, spathend(cs->to)-1 );
	if( clip_already( temp2, cs2 ) )
	{
	  spf( temp, msg_ptr[125], temp2+3 );
	  f_alert1( temp );
	}
	else
	{
  	  if( cs2 != cs ) memcpy( cs2, cs, l );
/*	  strcpy( cs2->to+3, cs2->to+cs2->dest_off );
	  *(cs2->to) = CLIP_LET;
	  if( (cs2->new_dir += new) > (j=slashes(cs2->to+3)) ) cs2->new_dir = j;
	  new=0; */
	  strcpy( cs2->to, temp2 );
	  cs2->new_dir = 0;
	  size += l;
	  cs2 = (copy_struct *)((long)cs2+l);
	  is_clip=update_clip=1;
	}
      }
    }
  }
  if( size ) clip_ptrs(c_buflen = c_bufmax - size);
}
int write_back(void)
{
  if( !w_cs ) return -1;
  return copy_a_buffer( rw_auxpath, rw_file, rw_fold );
}
char clean_fwrite( char noerr )
{
  register copy_struct *cs;

  if( !noerr )
  {
    into_clip();
    r_cs = w_cs = 0L;
  }
  else
  {
    cs = (copy_struct *)((long)c_buf + sizeof(BUF_HDR));
    noerr = cs->noerr;
    while( (char *)cs < c_curbuf && noerr )
    {
      noerr = move_del( cs );
      cs = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
    }
    c_curbuf = c_buf + sizeof(BUF_HDR);
    c_buflen = c_bufmax;
  }
  return noerr;
}
int cdecl copy_a_buffer( char *auxpath, int *file, int *fold )
{
  char noerr, *from_clip;
  register copy_struct *cs;

  noerr = 1;
  if( (cs=w_cs) != 0 )
    if( (cs->noerr = write_fblock()) == -1 )
    {
      from_clip = obuf;
      cs->noerr = 1;
      finish_write( cs, file );
      cs = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
      if( (char *)cs >= c_curbuf || from_clip )
      {
        noerr = clean_fwrite( noerr );
        arrow();
        return !noerr;
      }
    }
    else return !cs->noerr;
  else cs = (copy_struct *)((long)c_buf + sizeof(BUF_HDR));
  if( c_buflen < 0L ) return(0);
  if( *cs->to != CLIP_LET )
  {
    while( (char *)cs < c_curbuf && noerr )
    {
      copy_buf( cs, auxpath, file, fold );
      noerr = cs->noerr;
      if( w_cs && noerr ) return -2;
      cs = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
    }
    noerr = clean_fwrite( noerr );
  }
  arrow();
  return( !noerr );
}
/********************************************************************/
int clip_del( copy_struct *cs )
{
  if( *cs->to==CLIP_LET ) return move_del( cs );
  return 1;
}
int read_fblock(void)
{
  int i;
  char move, noerr=1, last=0;
  long len, l;
  static long mem_left;
  static char *r_ptr;
  
  if( !r_cs ) return -2;
  move = moving && *r_cs->from == *r_cs->to;
  if( r_first || !mem_left /* back for next block */ )
  {
    r_first = 0;
    if( c_curbuf != (char *)r_cs )
    {
      memcpy( c_curbuf, r_cs, sizeof(copy_struct) );
      r_cs = (copy_struct *)c_curbuf;
    }
    r_cs->len = 0L;
    if( !move ) fop_nums( 1, r_cs->from );
    mem_left = c_buflen - sizeof(copy_struct);
    r_ptr = c_curbuf += sizeof(copy_struct);
  }
  len = mem_left;
  if( copy_slice >= 0 )
    if( (l = get_copy_size()) == 0 ) return -1;		/* end of slice */
    else if( l < len ) len = l;
  if( (noerr = !_abort()) != 0 )
    {
      if( move ) r_cs->len = 0L;   /* 005: had r_len = 0L; */
      else
      {
        copy_size -= len;
        if( (noerr=TOS_error( l=cFread( r_hand, len, r_ptr ), 0 )) != 0 )
        {
          r_cs->len += l;
          if( (r_len -= l) < 0 ) r_len = 0L;
          r_ptr += l;
          if( (mem_left -= l) < 0 ) mem_left = 0L;
        }
      }
      if( noerr )
      {
        if( move/*005*/ || r_len<=0L )
        {
          r_cs->state |= CS_LAST;
          if( r_cs->state&CS_FIRST ) r_cs->state |= CS_CLIPOK;
          last++;
          if( r_cs->len & 1 )
          {
   	    r_cs->state |= CS_ODD;
	    r_cs->len++;
	    if( mem_left ) mem_left--;		/* 003 */
          }
        }
        else if( is_clip && !mem_left )
        {
          f_alert1( msg_ptr[94] );
          noerr = 0;
        }
        if( noerr && (move/*005*/ || !r_len || !mem_left) )
        {
          c_buflen = mem_left;
          c_curbuf += r_cs->len;
          if( c_buflen < sizeof(copy_struct) || !last )
          {
   	    if( (i = copy_a_buffer( rw_auxpath, rw_file, rw_fold )) == 0 )
  	        copybee();
  	    noerr = i<=0;
  	  }
        }
      }
    }
  if( last ) noerr = -1;
  if( last || !noerr )
  {
    cFclose(r_hand);
    if( noerr && r_cs ) clip_del(r_cs);
    else err_del();
    r_cs = 0L;
  }
  return noerr;
}
char confl_stat1( copy_struct *cs, char *src, char tvalid, long *len )
{
  if( tvalid )
  {
    cs->att = maintree.tree_att;
    *(long *)cs->time = maintree.tree_date;
    *len = maintree.tree_fsize;
  }
  else if( !TOS_error( (long)(cs->att = cFattrib( src, 0, 0 )), 0 ) ) return 0;
  return 1;
}
char confl_stat2( copy_struct *cs, int hand, long *len )
{
  cFdatime( (DOSTIME *)cs->time, hand, 0 );
  if( TOS_error( *len = cFseek( 0L, hand, 2 ), 0 ) &&
      TOS_error( cFseek( 0L, hand, 0 ), 0 ) ) return 1;
  return 0;
}
int cdecl copy_a_file( char *src, char *dest, char *auxpath, int empty,
		       int new_dir, int *file, int *fold )
{
  int hand=0, i, can, dum;
  long len, l, whole_len;
  register char noerr=1, move;
  register copy_struct *cs, cs2;
  BUF_HDR *bh;
  char tvalid;

  tvalid = maintree.tree_valid;
  maintree.tree_valid = 0;
  cs = (copy_struct *)c_curbuf;
  strcpy( cs->to, dest );
  strcpy( cs->from, src );
  if( *src==CLIP_LET || *dest==CLIP_LET ) did_clip = 1;
  cs->new_dir = new_dir;
  cs->noerr = 1;
  cs->del_dir = 0;
/*  cs->dest_off = dest_off;*/
  if( is_clip && (*src != CLIP_LET || !moving ) )
  {		/* copy/move from drive into clip or copy within clip */
    if( c_buflen < (signed long)sizeof(copy_struct) )
    {
      f_alert1( msg_ptr[94] );
      return(1);
    }
    if( cs->new_dir )
      if( c_new_dir( cs->to, auxpath, &cs->new_dir, 0, 0, fold ) > 0 ) return(1);
    if( cs->noerr )
    {
      if( *src && (cs->noerr = confl_stat1( cs, src, tvalid, &cs->len )) != 0 &&	/* 003: get stats for confl in file->clip */
          !tvalid && (hand=cFopen(src,0)) > 0 )
      {
        cs->noerr = confl_stat2( cs, hand, &cs->len );
        cFclose(hand);
      }
      if( !copy_confl( cs, &hand, &i, &can, &dum ) )
      {
	if( hand<0 && hand!=IEFILNF && hand!=IEPTHNF ) cs->noerr =
	    TOS_error( (long)hand, 0 );
      }
      else if( can ) cs->noerr = 99;
      else return(1);
    }
    if( !cs->noerr || !i && !hand ) return(0);
    if( !empty ) (*file)--;
  }
  if( *src == CLIP_LET && (!is_clip || moving) )
  {		/* copy/move from clip to drive or move within clip */
    bh = (BUF_HDR *)last_buf;
    if( (cs = find_item( src, &bh, (copy_struct *)((long)bh + sizeof(BUF_HDR)),
	0 )) != 0 )
    {
      cs->del_dir = 0;
      ocur = c_curbuf;
      obuf = c_buf;
      if( moving && is_clip )	      /* move within clip */
      {
	memcpy( &cs2, cs, sizeof(cs2) );
	c_buf = (c_curbuf = (char *)&cs2) - sizeof(BUF_HDR);
	c_curbuf += cs2.len;
	strcpy( cs2.to, dest );
	strcpy( cs2.from, src );
	cs2.new_dir = new_dir;
	cs2.noerr = 1;
	copy_buf( &cs2, auxpath, file, fold );
	i = move_del( &cs2 );
        c_curbuf = ocur;
        c_buf = obuf;
        obuf = NULL;
      }
      else
      {
	strcpy( cs->from, cs->to );
	c_curbuf = (char *)((long)(c_buf = (char *)bh) + bh->bufsiz);
	strcpy( cs->to, dest );
	strcpy( clip_src, src );
	clip_ndir = cs->new_dir;
	cs->new_dir = new_dir;
	cs->noerr = 1;
	cs->state |= CS_FIRST|CS_LAST;
	copy_buf( cs, auxpath, file, fold );
	i = cs->noerr;
      }
      return( !i );
    }
    else
    {
      TOS_error( AEFILNF, 0 );
      return(1);
    }
  }
  if( empty )
  {				/* empty directory to be created */
    cs->state = CS_FIRST|CS_EMPTY;
    cs->len = 0L;
    c_buflen -= sizeof(copy_struct);
    c_curbuf += sizeof(copy_struct);
    if( c_buflen <= sizeof(copy_struct) )
       if( (noerr = !copy_a_buffer( auxpath, file, fold )) != 0 )
	   copybee();
    if( noerr ) clip_del(cs);
  }
  else
  {
    noerr = confl_stat1( cs, src, tvalid, &whole_len );
    if( noerr && (hand=cFopen(src,0)) > 0 )
    {
      if( !tvalid ) noerr = confl_stat2( cs, hand, &whole_len );
      cs->state = CS_FIRST;
      rw_setup( 1, hand, cs, whole_len, auxpath, file, fold );
      while( noerr>0 )
      {
        noerr = read_fblock();
        if( copy_slice>=0 ) break;
      }
      if( noerr==-1 ) noerr = 1;
    }
    else noerr=0;
  }
  return( !noerr );
}
/********************************************************************/
void cpy_from_d( char *src, char *dest, int ignore )
  /* set ignore if dest is folder. is set for copying to desktop folder
     or from window to drive */
{
  char msg[4+120+120];
  int l;
  
  msg[2] = MCOPF;
  msg[3] = (3<<3)|(moving<<2)|(ignore<<1)|1;
  strcpy( msg+4, dest );
  strcpy( msg+(l=4+strlen(msg+4)+1), src );
  if( !add_copy( msg, l+strlen(src)+1 ) ) return;
  if( z->in_copy ) jog_background = 1;
  arrow();	/* in case was grabbing hand */
}
/********************************************************************/
int ccomplete( char *src, /*%char *dest,*/ int wind, FSTRUCT *fs )
{
  if( ed_wind_type(wind)==EDW_GROUP )
  {
    strcpy( src, fs->grp_item->p.path );
    return 1;
  }
  else
  {
    strcpy( src, z->w[wind].path );
    strcpy( spathend(src), fs->name );
    return 0;
  }
}
void inc_cpq( int qpos, int add )
{
  unsigned char *ptr;
  
  ptr = copy_q+qpos;
  add += (*ptr<<8) + *(ptr+1);
  *ptr++ = add>>8;
  *ptr = add;
}
void copy_from_w( int item, int wind, char *path )
{
  char msg[4+120], src[120], *end, c;
  SEL_ICON *s;
  SELICON_DESC i;
  int grp, k, qpos, first=1;
  FSTRUCT *f;
  
  msg[2] = MCOPF;
  i.icons = 0L;
  f = item>=0 ? &z->file[wind][item] : 0L;
  while( (s=get_msel_icon( &i, 0, 0 )) != 0 )
    if( s->wnum!=wind || s->u.fs!=f )
    {
      strcpy( msg+4, path );
      grp = ccomplete( src, s->wnum, s->u.fs );
      end = spathend(src);
      if( s->u.fs->type.p.pexec_mode==FOLDER ) strcat( src, slash );
      if( !check_copy( src, msg+4 ) ) break;
      if( first )
      {
        msg[3] = (3<<3)|(moving<<2)|(0<<1)|grp;
        if( !add_copy( msg, k=4+strlen(msg+4)+1 ) ) break;
        qpos = copyqlen - k;
        first = 0;
        if( !grp )
        {
          c = *end;
          *end = 0;
          if( !add_copy_str( src, k=end-src+1 ) ) break;
          inc_cpq( qpos, k );
          *end = c;
        }
      }
      if( grp )
      {
        if( !add_copy_str( src, k=strlen(src)+1 ) ) break;
      }
      else if( !add_copy_str( end, k=strlen(end)+1 ) ) break;
      inc_cpq( qpos, k );
    }
  cmfree( (char **)&i.icons );
  if( z->in_copy ) jog_background = 1;
  *path = 0;	/* prevent update_drive() in main() */
  arrow();	/* in case was grabbing hand */
}

void msg_rest( int *buf, char *msg, int size )
{
  char **p;
  int qpos, i, k;

  if( !add_copy( msg, size ) ) return;
  qpos = copyqlen - size;
  p = *(char ***)&buf[6];
  for( i=buf[5]; --i>=0; p++ )
  {
    if( !add_copy_str( *p, k=strlen(*p)+1 ) ) break;
    inc_cpq( qpos, k );
  }
}

void msg_copy( int *buf )
{
  char msg[4];
  
  msg[2] = MCOPF;
  msg[3] = buf[4];  /* flags: 0:   abs path
                              1:   ignore 1st
                              2:   move
                              3-4: 3=normal  0=no conf over  1=conf */
  msg_rest( buf, msg, sizeof(msg) );
}

void msg_del( int *buf )
{
  char msg[3];
  
  msg[2] = MDELF;
  msg_rest( buf, msg, sizeof(msg) );
}

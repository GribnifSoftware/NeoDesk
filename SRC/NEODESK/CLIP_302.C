#include "string.h"
#include "tos.h"
#include "stdlib.h"
#include "lerrno.h"
#include "ierrno.h"
#include "aes.h"
#include "neodesk.h"
#include "mwclinea.h"
#include "neocommn.h"
#include "neod2_id.h"

extern long clip_seek;
extern int dum;
extern char clip_drive, *c_buf, *c_curbuf, *last_buf, *ocur, *obuf,
    is_clip, nil[], neodesk_dat[];
copy_struct *clip_pos;
char *clip_buf;

copy_struct *find_item( char *item, BUF_HDR **buf, copy_struct *cs,
    int flag )
{
  int l;

  l = strlen(item);
  while( *buf )
  {
    while( (char *)cs < (char *)*buf + (*buf)->bufsiz )
    {
      if( !strncmp( cs->to, item, l ) && (!*(cs->to+l)||
          *(cs->to+l)=='\\'||*(cs->to+l-1)=='\\') && 
          (flag || slashes(cs->to+l) <= cs->new_dir) ) return(cs);
      cs = (copy_struct *)((long)cs + cs->len + sizeof(copy_struct));
    }
    cs = (copy_struct *)((long)(*buf = (*buf)->lastbuf)+sizeof(BUF_HDR));
  }
  return((copy_struct *)0L);
}
BUF_HDR *spbh;
copy_struct *spcs;
unsigned char get_attrib( char *item, copy_struct *cs )
{
  return( slashes(cs->to+pathend(item)) || cs->state&CS_EMPTY ? S_IJDIR : 
      cs->att );
}
int find_file( char *item )
{
  spbh = (BUF_HDR *)last_buf;
  return( (spcs=find_item( item, &spbh, 
      (copy_struct *)((long)spbh+sizeof(BUF_HDR)), 0 ))
      != 0 ? get_attrib(item,spcs) : AEFILNF );
}
int setpath( char *path, int flag )
{
  char temp[120];
  int i;

  spbh = (BUF_HDR *)last_buf;
  strcpy( temp, path );
  iso(temp);
  if( (i=pathend(temp)) > 3 && flag ) temp[i-1] = '\0';
  return( 
      (spcs=find_item( temp, &spbh, (copy_struct *)((long)spbh+sizeof(BUF_HDR)),
      0 )) != 0 || i==3 ); /*slashes(temp)==1 ); */  /* keep this order */
}
void shrink_buf( BUF_HDR *bh, copy_struct *cs )
{
  lshrink( bh, bh->bufsiz -= sizeof(copy_struct)+cs->len );
}
int did_del;
void remove_buf( BUF_HDR *bh )
{
  BUF_HDR *obh;
  
  obh = bh;
  bh = bh->lastbuf;
  lfree(obh);              /* just in case we ever get real multitasking */
  last_buf = (char *)bh;
  if( bh ) bh->nextbuf = 0L;
}
void remove_clip( void )
{
  long l;
  copy_struct *cs, *tcs, *lcs;
  BUF_HDR *bh;

  if( did_del )
  {
    bh = (BUF_HDR *)last_buf;
    while( bh )
    {
      cs = (copy_struct *)((long)bh+sizeof(BUF_HDR));
      while( (long)cs < (long)bh + bh->bufsiz )
      {
        tcs = cs;
        l = 0;
        while( !*tcs->to )
        {
          l += tcs->len + sizeof(copy_struct);
          tcs = (copy_struct *)((long)tcs+tcs->len+sizeof(copy_struct));
          if( (long)tcs >= (long)bh + bh->bufsiz )
          {
            if( bh == (BUF_HDR *)last_buf )
            {
              if( (long)cs == (long)bh + sizeof(BUF_HDR) )
              {
                remove_buf( bh );
                bh = (BUF_HDR *)last_buf;
              }
              else
              {
                shrink_buf( bh, cs );
                bh = bh->lastbuf;
              }
              goto next_buf;
            }
          }
          else if( !*tcs->to ) cs->len += tcs->len + sizeof(copy_struct);
          else
          {
            memcpy( (char *)cs, (char *)tcs, (long)bh+bh->bufsiz - 
                (long)tcs );
            tcs = (copy_struct *)((long)bh + bh->bufsiz - l);
            *tcs->to = '\0';
            tcs->len = l - sizeof(copy_struct);
            if( bh == (BUF_HDR *)last_buf ) shrink_buf( bh, tcs );
            break;
          }
        }
        cs = (copy_struct *)((long)cs + sizeof(copy_struct) + cs->len);
      }
      bh = bh->lastbuf;
next_buf: ;
    }
    did_del = 0;
  }
}
int remove_item( char *path, int is_dir )
{
  copy_struct *cs;
  BUF_HDR *bh;
  char att;
  
  bh = (BUF_HDR *)last_buf;
  if( (cs=find_item( path, &bh, (copy_struct *)((long)bh+sizeof(BUF_HDR)), 0 ))
      != 0 )
  {
    att = get_attrib( path, cs );
    if( !is_dir )
    {
      if( att&S_IJDIR ) return(AEACCDN);
      else if( cs->new_dir )
      {
        iso(cs->to);
        if( !find_item( cs->to, &bh, (copy_struct *)(cs + cs->len + 
            sizeof(copy_struct)), 0 ) ) cs->state |= CS_EMPTY;
      }
      else /*  remove_clip( bh, cs ); */
      {
        *cs->to = '\0';
        did_del = 1;
      }
      return(0);
    }
    else
    {
      if( !(att&S_IJDIR) || slashes( cs->to+strlen(path) ) > 1 || 
          !(cs->state&CS_EMPTY) ) return(AEACCDN);
      else if( !(--cs->new_dir) ) /*  remove_clip( bh, cs );  */
      {
        *cs->to = '\0';
        did_del = 1;
      }
      else *(cs->to+strlen(path)) = '\0';
      return(0);
    }
  }
  return( is_dir ? AEPTHNF : AEFILNF );
}
int     cDsetdrv( int drv )
{
        if( (clip_drive = drv == CLIP_LET-'A') != 0 ) return(drvmap());
        return( Dsetdrv( drv ) );
}
/*     int     cDgetdrv( void )         not used */
int     cDfree( DISKINFO *buf, int driveno )
{
        BUF_HDR *b;
        
        if( driveno == CLIP_LET-'A'+1 )
        {
          buf->b_free = buf->b_total = (long)lalloc(-1L,0);
          buf->b_secsiz = 1L;
          buf->b_clsiz = 1L;
          b = (BUF_HDR *)last_buf;
          while( b )
          {
            buf->b_total += b->bufsiz;
            b = b->lastbuf;
          }
          return(0);
        }
        if( is_locked(driveno) ) return LOCK_ERR;
        return( Dfree( buf, driveno ) );
}
void fix_path( char *path, char *temp )
{
  int i;
  extern char slash[];
  
  strcpy( temp, path );
  if( temp[i=strlen(temp)-1] == '.' ) temp[i--] = '\0';
  if( temp[i] != '\\' ) strcat( temp, slash );
}
int     cDcreate( char *path )
{
        int ret, i;
        char temp[120], *obuf2=0L, *ocur2;
        
        if( *path==CLIP_LET )
        {
          i = 0;
          if( !is_clip )
          {
            if( c_buf ) return AEACCDN;	/* can't create folder in clip while in_copy */
            is_clip = i = 1;
            copy_init();
          }
          else if( obuf )
          {
            obuf2 = c_buf;
            c_buf = obuf;
            ocur2 = c_curbuf;
            c_curbuf = ocur;
          }
          fix_path( path, temp );
          ret = copy_a_file( nil, temp, 0L, 1, 1, &dum, &dum );
          if( i ) copy_free();
          else if( obuf2 )
          {
            obuf = c_buf;
            c_buf = obuf2;
            ocur = c_curbuf;
            c_curbuf = ocur2;
          }
          return( ret ? CLIP_IGN : 0 );
        }
        if( is_locked(*path) ) return LOCK_ERR;
        return( Dcreate( path ) );
}
int     cDdelete( char *path )
{
        char temp[120], *ptr;
        
        if( *path==CLIP_LET )
        {
          fix_path( path, temp );
          return( remove_item( temp, 1 ) );
        }
        if( is_locked(*path) ) return LOCK_ERR;
        strcpy( temp, path );
        /* chop trailing \ to avoid bug in Falcon TOS */
        if( (ptr = strrchr(temp,'\\')) != 0 && !*(ptr+1) ) *ptr=0;
        return( Ddelete( temp ) );
}
int     cDsetpath( char *path )
{
        char temp[120];
        
        if( clip_drive )
        {
          temp[0] = CLIP_LET;
          temp[1] = ':';
          strcpy( temp+2, path );
          fix_path( temp, temp );
          return( setpath(temp,1) ? 0 : AEPTHNF );
        }
        return( Dsetpath( path ) );
}
int     cFcreate( char *filename, int attr )
{
        if( *filename==CLIP_LET ) return(CLIP_ERR);
        if( is_locked(*filename) ) return LOCK_ERR;
        return( Fcreate( filename, attr ) );
}
int     cFopen( char *filename, int mode )
{
        int ret;
        
        if( *filename==CLIP_LET )
          if( (ret=find_file(filename)) >= 0 )
            if( spcs->att&S_IJRON && mode>=1 || ret&S_IJDIR ) 
                return(AEACCDN);
            else
            {
              clip_seek = 0L;
              clip_pos = spcs;
              return(CLIP_HAND);
            }
          else return(AEFILNF);
        if( is_locked(*filename) ) return LOCK_ERR;
        if( !strncmp(filename,"U:\\DEV\\",7) ) return AEACCDN;	/* 003 */
        return( Fopen( filename, mode ) );
}
int     cFclose( int handle )
{
        if( handle == CLIP_HAND )
        {
          clip_seek = -1L;
          return(0);
        }
        return( Fclose( handle ) );
}
int is_odd( copy_struct *cs )
{
  return( cs->state & CS_ODD ? 1 : 0 );
}
long    cFread( int handle, long count, void *buf )
{
        long l;
        
        if( handle == CLIP_HAND )
          if( clip_seek >= 0 )
          {
            l = clip_pos->len - clip_seek - is_odd(clip_pos);
            l = count < l ? count : l;
            memcpy( buf, (char *)clip_pos+sizeof(copy_struct)+clip_seek, l );
            clip_seek += l;
            return(l);
          }
          else return(AEIHNDL);
        return( Fread( handle, count, buf ) );
}
long    cFwrite( int handle, long count, void *buf )
{
        if( handle == CLIP_HAND ) return(CLIP_ERR);
        return( Fwrite( handle, count, buf ) );
}
int     cFdelete( char *filename )
{
        if( *filename==CLIP_LET )
        {
          if( filename==neodesk_dat ) return(CLIP_ERR);
          return( remove_item( filename, 0 ) );
        }
        if( is_locked(*filename) ) return LOCK_ERR;
        return( Fdelete( filename ) );
}
long    cFseek( long offset, int handle, int seekmode )
{
        if( handle == CLIP_HAND )
        {
          if( clip_seek < 0L ) return(AEIHNDL);
          switch(seekmode)
          {
            case 0:
              clip_seek = offset;
              break;
            case 1:
              clip_seek += offset;
              break;
            case 2:
              clip_seek = clip_pos->len - 1 - offset - is_odd(clip_pos);
          }
          if( clip_seek < 0 ) clip_seek = 0;
          if( clip_seek >= clip_pos->len - is_odd(clip_pos) ) 
              clip_seek = clip_pos->len-1-is_odd(clip_pos);
          return(clip_seek);
        }
        return( Fseek( offset, handle, seekmode ) );
}
int     cFattrib( char *filename, int wflag, int attrib )
{
        char att;
        
        if( *filename==CLIP_LET )
        {
          if( (att = find_file(filename)) >= 0 )
          {
            if( wflag ) spcs->att = attrib;
            return(att);
          }
          return(AEFILNF);
        }
        if( is_locked(*filename) ) return LOCK_ERR;
        return( Fattrib( filename, wflag, attrib ) );
}
/*    int     cDgetpath( char *path, int driveno )   not used */
int next_fs( DTA *dta, char *file, BUF_HDR *bh, copy_struct *cs )
{
  char *ptr, path[120], temp[13], temp2[14];
  unsigned char att;
  int i, l, flg;
  copy_struct *cs0;

  l = -(dta->d_reserved[0]);
  strcpy( path, file );
  *(path + l) = '\0';
  dta->d_attrib = 0;
  from_filename( dta->d_reserved+10, temp, 1 );
  if( cs ) do
  {
    flg = 0;
    ptr = cs->to+l;
    if( (att = get_attrib( path, cs )) & S_IJDIR )
    {
      for( i=0; *ptr && *ptr!='\\'; i++ )
        temp2[i] = *ptr++;
      temp2[i] = '\0';
      ptr = temp2;
    }
    if( (dta->d_reserved[1]&att || !att) && *ptr && match( ptr, temp ) ) flg++;
    cs0 = cs;
    cs = find_item( path, &bh, (copy_struct *)((long)cs + cs->len +
        sizeof(copy_struct)), 0 );
    if( flg )
    {
      dta->d_attrib = att;
      *(long *)&dta->d_time = att&S_IJDIR ? ((long)Tgettime()<<16L)|Tgetdate() : /* 003: was 0L for folder */
          *(long *)&cs0->time;
      dta->d_length = att&S_IJDIR ? 0L : cs0->len-is_odd(cs0);
      for( i=0; i<12 && *ptr && *ptr!='\\'; i++ )
        dta->d_fname[i] = *ptr++;
      dta->d_fname[i] = '\0';
      *(BUF_HDR **)&dta->d_reserved[2] = bh;
      *(copy_struct **)&dta->d_reserved[6] = cs;  /* cs3 */
      return(0);
    }
  }
  while( cs );
  return(AENMFIL);
}
int     cFsnext( void )
{
        DTA *dta;
        BUF_HDR *bh;
        copy_struct *cs;
        int ret;
        
        dta = Fgetdta();
        if( dta->d_reserved[0] < 0 && dta->d_fname[13] == -CLIP_LET /* 003 */ )
        {
          cs = *(copy_struct **)&dta->d_reserved[6];
          if( (bh = *(BUF_HDR **)&dta->d_reserved[2]) != 0 )
              return( next_fs( dta, cs->to, bh, cs ) );
          return(AENMFIL);
        }
        if( (ret = Fsnext()) == AEFILNF ) return AENMFIL;	/* 003 */
        return ret;
}
int     cFsfirst( char *filename, int attr )
{
        DTA *dta;
        int i;
        
        dta = Fgetdta();	/* 003: moved here */
        if( *filename==CLIP_LET )
        {
          if( setpath(filename,0) )
          {
            if( !spcs ) return(AEFILNF);
            dta->d_reserved[0] = -(i=pathend(filename));
            dta->d_reserved[1] = attr;
            *(BUF_HDR **)&dta->d_reserved[2] = spbh;
            *(long *)&dta->d_reserved[6] = (long)spcs; /* spbh + spbh->bufsiz;*/
            to_filename( filename+i, dta->d_reserved+10 );
            dta->d_fname[13] = -CLIP_LET;	/* 003 */
            if( i>3 && match( ".", filename+i ) )
            {
              strcpy( dta->d_fname, "." );
              dta->d_attrib = S_IJDIR;
              dta->d_time = 0;
              dta->d_date = 0;
              dta->d_length = 0L;
              return(0);
            }
            return( next_fs( dta, filename, spbh,
                *(copy_struct **)&dta->d_reserved[6] ) == AENMFIL ? 
                AEFILNF : 0 );
          }
          return(AEPTHNF);
        }
        else dta->d_fname[13] = 0;	/* 003 */
        if( is_locked(*filename) ) return LOCK_ERR;
        return( Fsfirst( filename, attr ) );
}
int     cFrename( int zero, char *oldname, char *newname )
{
        char temp[120], temp2[120], temp3[120], *ptr, att;
        int i;
        
        if( *oldname==CLIP_LET )
        {
          if( find_file(newname) >= 0 ) return(AEACCDN);
          if( (att=find_file(oldname)) >= 0 )
            if( att&S_IJRON ) return(AEACCDN);
            else
            {
              if( att & S_IJDIR )               /* folder */
              {
                i = pathend(oldname);   /* oldname must not end in \ */
                fix_path( newname, temp2 );
                spbh = (BUF_HDR *)last_buf;
                spcs = (copy_struct *)((long)spbh+sizeof(BUF_HDR));
                fix_path( oldname, temp3 );
                while( (spcs=find_item( temp3, &spbh, spcs, 1 )) != 0 )
                {
                  strcpy( temp, spcs->to );
                  strcpy( spcs->to, temp2 );
                  if( (ptr = strchr( temp+i, '\\' )) != 0 )
                      strcat( spcs->to, ptr+1 );
                  spcs = (copy_struct *)((long)spcs + spcs->len + 
                      sizeof(copy_struct));
                }
              }
              else if( spcs->new_dir )      /* not a folder, immediate child */
              {
                strcpy( temp, spcs->to );
                strcpy( spcs->to, newname );
                if( (i=pathend(temp)) != pathend(newname) ||
                    strncmp( temp, newname, i ) )    /* moved */
                {
                  iso(temp);
                  i = spcs->new_dir;
                  spcs->new_dir = 0;
                  if( (spcs = find_item( temp, &spbh, (copy_struct *)
                      ((long)spcs + sizeof(copy_struct) + spcs->len), 0 ))
                      != 0 ) spcs->new_dir += i;
                  else return( cDcreate(temp) );
                }
              }
              else strcpy( spcs->to, newname );  /* not a folder */
              return(0);
            }
          return(AEFILNF);
        }
        if( is_locked(*oldname) ) return LOCK_ERR;
        return( Frename( zero, oldname, newname ) );
}
int     cFdatime( DOSTIME *timeptr, int handle, int wflag )
{
        if( handle == CLIP_HAND )
        {
          if( clip_seek < 0L ) return(AEIHNDL);
          if( wflag )
          {
            if( clip_pos->att & S_IJRON ) return(AEACCDN);
            *(long *)clip_pos->time = *(long *)timeptr;
          }
          else *(long *)timeptr = *(long *)clip_pos->time;
          return(0);
        }
        return( Fdatime( timeptr, handle, wflag ) );
}

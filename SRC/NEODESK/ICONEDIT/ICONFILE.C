#include "tos.h"

#define KEY_START 0x37

  #define MAX_PLANES4 4    /* 4 data planes */
  #define MAX_W       2    /* width in words */
  #define MAX_H       32   /* height in pixels */
  #define MAX_DATA4   ((MAX_PLANES4+1)*MAX_W*MAX_H*2)
         /* (4 data planes + 1 mask plane) * (normal + seleced) */
  #define OLD_PLANES 2	   /* planes in a Neo 3 file */
  #define MAX_SIZE4   ((MAX_PLANES4<<4)|MAX_PLANES4)
  typedef struct
  {
    unsigned char size_x, size_y;
    unsigned char readicon;	/* set to 0, 1, or 2 (1, 2, 4 planes) */
    unsigned char norm_read, sel_read;
    unsigned char unused;
    unsigned char xchar, ychar;
    unsigned char planes;      /* set to MAX_SIZE before calling */
    unsigned char type;              /* use ICON_TYPE to convert */
    char text[12];            /* see old icon format for details */
    unsigned char colors[OLD_PLANES];
    int data[MAX_DATA4];
  } NEO_ICON;

typedef struct
{
  unsigned char size_x, size_y;
  unsigned char x_char, y_char;
  unsigned char colors;
  unsigned char type;
  char string[12];
  long data[3][4];	/* col_data, col_mask, sel_data, sel_mask */
} ICONFILE;

/*typedef struct    not valid
{
  unsigned char type;
  char string[12];
} ICONCOPY; */

struct
{
  char magic[4];
  int ver;
  unsigned char copylen;
  char copyright[67];
  unsigned int entries;
  long created, modified;
  char author[26], comment[106];
  unsigned int code_len;
} header;

void unencrypt( void *ptr, long size );
void null_func(void);
int *read_im( int hand, int *out, long off );
int *clear_data( int *out, int count );

int extract_icon( int hand, unsigned int num, NEO_ICON *icon )
{
  unsigned char planes, pl_ind, new;
  ICONFILE icf;
  int i, *out;
  long l;
  
  icon->sel_read = 0;
  icon->norm_read = 0;
  l = sizeof(header) + (long)null_func - (long)extract_icon + 
      num*sizeof(ICONFILE);
  if( Fseek( l, hand, 0 ) != l || Fread( hand, sizeof(icf), &icf ) != 
      sizeof(icf) ) return(-2);
  unencrypt( &icf, sizeof(icf) );
  if( icf.size_x > icon->size_x ) return(-3);
  if( icf.size_y > icon->size_y ) return(-4);
  if( (pl_ind = icon->planes) > 15 )
  {
    if( (pl_ind=icon->readicon) > 2 ) return -5;
    if( !icf.data[pl_ind][0] && !icf.data[pl_ind][2] ) return -6;
    planes = "\1\2\4"[pl_ind];
    new = 1;
  }
  else if( !icf.data[0][0] ) return -5;
  else
  {
    planes = 2;
    pl_ind = 0;
    new = 0;
  }
  (int)l = icf.size_x*icf.size_y<<1;
  out = icon->data;
  if( icon->planes&0xf )
  {
    if( icf.data[pl_ind][1] )
    {
      if( (out=read_im( hand, out, icf.data[pl_ind][1] )) == 0 ) return -2;
      icon->norm_read |= 1<<4;
    }
    else if( !new ) out = clear_data( out, l );
    if( icf.data[pl_ind][0] )
    {
      if( (out=read_im( hand, out, icf.data[pl_ind][0] )) == 0 ) return -2;
      icon->norm_read |= planes;
    }
    else if( !new ) out = clear_data( out, l*planes );
  }
  if( icon->planes&0xf0 )
  {
    if( icf.data[pl_ind][3] )
    {
      if( (out=read_im( hand, out, icf.data[pl_ind][3] )) == 0 ) return -2;
      icon->sel_read |= 1<<4;
    }
    else if( !new ) out = clear_data( out, l );
    if( icf.data[pl_ind][2] )
    {
      if( (out=read_im( hand, out, icf.data[pl_ind][2] )) == 0 ) return -2;
      icon->sel_read |= planes;
    }
    else if( !new ) out = clear_data( out, l*planes );
  }
  *(int *)&icon->size_x = *(int *)&icf.size_x;
  icon->unused = 0;
  *(int *)&icon->xchar = *(int *)&icf.x_char;
  *(ICONCOPY *)&icon->type = *(ICONCOPY *)&icf.type;  /* copies one extra byte */
  *(int *)&icon->colors = 1;
  return(0);
}

int *read_im( int hand, int *out, long off )
{
  unsigned char temp[4], t, *b, *b2;
  char k, t2;
  int buf[MAX_PLANES4*MAX_W*MAX_H+1], l;
  
  if( Fseek( off, hand, 0 ) != off || Fread( hand, 3L, temp ) != 3L ) return 0L;
  l = *(int *)temp;
  if( Fread( hand, l, buf ) != l ) return 0L;
  unencrypt( b=(char *)buf, l );
  if( !(temp[2]&3) )	/* uncompressed, do a long copy */
  {
    l >>= 2;
    while( --l>=0 ) *((long *)out)++ = *((long *)b)++;
  }
  else while( --l>=0 )
    if( (t = (k=*b++) & 0xC0) == 0 )	/* uncompressed */
    {
      k &= 0x3F;
      l -= k+1;
      do
        *((char *)out)++ = *b++;	/* copy k+1 bytes */
      while( --k>=0 );
    }
    else if( t == 0x40 )
    {
      k &= 0x3F;
      t = *b++;
      l--;
      do
        *((char *)out)++ = t;		/* repeat byte k+1 times */
      while( --k>=0 );
    }
    else if( t == 0x80 )
    {
      l -= (t = ((k>>4)&3)+1) + 1;
      k = (k&0xf) + 1;
      do
      {
        t2 = t;
        b2 = b;
        do
          *((char *)out)++ = *b2++;
        while( --t2>=0 );
      }
      while( --k>=0 );
      b = b2;
    }
    else break;
  return out;
}

void unencrypt( void *buf, long size )
{
  char key=KEY_START;

  while( --size >= 0 )
  {
    *((char *)buf)++ ^= key;
    key += 0x21;
  }
}

int *clear_data( int *out, int count )
{
  count>>=2;
  while( --count >= 0 ) *((long *)out)++ = 0L;
  return out;
}

void null_func(void)
{
}


#ifdef GNVAHELP
  #include "\source\neodesk\dither.h"
  #include "lerrno.h"
  void xmfree( void *addr );
  void *xmalloc( long size );
  int xrealloc( void **start, long size );
  #define cFread(a,b,c) _Fread(b,c)
  #define cFseek(a,b,c) _Fseek(a,c)
  #define lalloc(x,y) xmalloc(x)
  #define lshrink(a,b) xrealloc(a,b)
  #define lfree xmfree
  #define memclr(a,b) memset(a,0,b)
  #define lvplanes vplanes
  #define lxmax xmax
  #define lymax ymax
  #define lxasp xasp
  #define lyasp yasp
  #define xfix_cicon(data,len,old,new,s,devspef) \
          x_graf_rast2rez( data, len, old, s, devspef )
  PICTURE *picture;
  int pcol( int c );
  void cmfree( char **x )
  {
    if( *x ) { lfree(*x); *x=0L; }
  }

#else GNVAHELP

#include "stdlib.h"
#include "tos.h"
#include "string.h"
#include "stdio.h"
#include "aes.h"
#include "vdi.h"
#include "dither.h"
#include "lerrno.h"
#ifndef TEST
  #include "neocommn.h"
  #include "neod2_id.h"
  #include "guidefs.h"	/* must come after aes.h and neocommn.h */
  extern MASTER *mas;
#endif

#ifdef TEST

  #define CJar_cookie     0x434A6172L     /* "CJar" */
  #define CJar_xbios      0x434A          /* "CJ" */
  #define CJar_OK         0x6172          /* "ar" */
  #define CJar(mode,cookie,value)         xbios(CJar_xbios,mode,cookie,value)
  #define _VDO_COOKIE 0x5F56444FL
  #define SBL_COOKIE  0x4F53424CL
  #define cFread Fread
  #define cFseek Fseek
  #define cFclose Fclose
  #define cFopen Fopen
  #define lalloc(x,y) Malloc(x)
  #define lshrink(a,b) Mshrink(0,a,b)
  #define memclr(a,b) memset(a,0,b)
  #define lfree Mfree
  #define EXTERN
  void open_vdi(void);
  void unpak_rgb( unsigned char *c, int *rgb )
  {
    *rgb = (*c * 1000L + 127) / 255;
  }
  long dispkey;

#else TEST

  extern GRAPHICS *graphics;
  #define EXTERN extern
  #define vsl_type(a,b) (*graphics->vsl_type)(b)

#endif TEST

int pcol( int c );
int pathend( char *ptr );
int find_extn( char *ptr );
#ifndef PRINT_ALLOCS
void cmfree( char **ptr );
#endif
void cdecl bconws( char *ptr );
int save_img( char *name, PICTURE *pic );

EXTERN char falc_vid, TT_vid, has_sblast, has_clut;
EXTERN int vplanes, xmax, ymax, colvals, v_bpp, colmax, colmax1, vwrap;
EXTERN int vdi_hand, xasp, yasp;
static char rgb15, intel;

int old_rez, lvplanes, lxmax, lymax, lvwrap, lxasp, lyasp;
int reztbl[][6] = {  /* w, h, planes, bpl, xasp, yasp */
    320, 200, 4, 160, 1, 1,
    640, 200, 2, 160, 1, 2,
    640, 400, 1, 80, 1, 1,
    0,0,0,0,0,0,
    640, 480, 4, 320, 1, 1,
    0,0,0,0,0,0,
    1280, 960, 1, 160, 1, 1,
    320, 480, 8, 320, 2, 1 };

PICTURE *picture;

unsigned long (*farbtbl2)[256];
int (*farbtbl)[256][32];

#endif GNVAHELP

void vq_scrninfo( int handle, int *work_out );

void scrninf(void)
{
  int out[272];
  
  if( lvplanes != 16 ) return;
  if( CJar( 0, 0x45644449L, 0L ) == CJar_OK )
  {
    vq_scrninfo( vdi_hand, out );
    intel = out[14]&0x80;
    if( out[14] && !(out[14]&1) && out[8]+out[9]+out[10]==15 ) rgb15=1;
  }
  else if( !falc_vid )
  {
    intel = 1;
    rgb15 = 1;
  }
}

#ifndef GNVAHELP

int test_rez (void)
{	int     i, np, color, pxy[8], rgb[3], bpp = 0;
	unsigned int    backup[32], test[32];
	static int black[3] = {0, 0, 0}, white[3] = {1000, 1000, 1000};
	int     (*rgb_palette)[256][4];
	MFDB    screen;
	MFDB pixel = {0L, 16, 1, 1, 0, 1, 0, 0, 0};
	MFDB stdfm = {0L, 16, 1, 1, 1, 1, 0, 0, 0};
	static char pixtbl[16] = {0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 16};

        if( v_bpp >= 0 ) return v_bpp;
	if (lvplanes >= 8)
	{
	  if( !farbtbl2 )
                if( (farbtbl2 = (unsigned long (*)[256])lalloc(256*4L,-1)) == 0 ||
                    (farbtbl = (int (*)[256][32])lalloc(256*32*2,-1)) == 0 ) return AENSMEM;
		for (color = 0; color < 256; color++)
			*(unsigned char *)&(*farbtbl2)[color] = color;

		if (lvplanes == 8)
		{
			color = 0xff;
			memclr (test, lvplanes * sizeof (int));
			for (np = 0; np < lvplanes; np++)
				test[np] = (color & (1 << np)) << (15 - np);

			pixel.fd_addr = stdfm.fd_addr = test;
			vr_trnfm (vdi_hand, &stdfm, &pixel);

			for (i = 1; i < lvplanes; i++)
				if (test[i])	break;

			if (i >= lvplanes && !(test[0] & 0x00ff))
				bpp = 1;
		}
		else
		{
			scrninf();
			vs_clip ( vdi_hand, 0, black);
			graf_mouse (M_OFF, 0L);
			screen.fd_addr = 0L;
			vswr_mode (vdi_hand,MD_REPLACE);
			vsl_type (vdi_hand,1);
			memclr (pxy, sizeof (pxy));
			pixel.fd_addr = backup;	/* Punkt retten */

			memclr (backup, sizeof (backup));

			vro_cpyfm (vdi_hand, S_ONLY, pxy, &screen, &pixel);

			/* Alte Farbe retten */
			vq_color (vdi_hand, 15, 1, rgb);

			/* Ger„teabh„ngiges Format testen */
			pixel.fd_addr = test;
			vsl_color (vdi_hand,15);
			vs_color (vdi_hand, 15, white);
			v_pline (vdi_hand, 2, pxy);

			memclr (test, lvplanes * sizeof (int));
			vro_cpyfm (vdi_hand, S_ONLY, pxy, &screen, &pixel);

			for (i = (lvplanes + 15) / 16 * 2; i < lvplanes; i++)
				if (test[i])	break;

			if (i >= lvplanes)
			{
				vs_color (vdi_hand, 15, black);
				v_pline (vdi_hand, 2, pxy);

				memclr (test, lvplanes * sizeof (int));
				vro_cpyfm (vdi_hand, S_ONLY, pxy, &screen, &pixel);

				for (i = (lvplanes + 15) / 16 * 2; i < lvplanes; i++)
					if (test[i])	break;

				if (i >= lvplanes)
					bpp = (lvplanes + 7) / 8;
			}

			/* Alte Farbe restaurieren */
			vs_color (vdi_hand, 15, rgb);

			pixel.fd_addr = backup;	/* Punkt restaurieren */
			vro_cpyfm (vdi_hand, S_ONLY, pxy, &pixel, &screen);

			/* Read the color palette */
		 	if( (rgb_palette = (int (*)[256][4])lalloc(256*4*2,-1)) != 0 )
			{
			  for (color = 0; color < 255; color++)
			  {	if (color < 16)
				{	vq_color (vdi_hand, pixtbl[color], 1, (*rgb_palette)[color]);
					(*rgb_palette)[color][3] = pixtbl[color];
				}
				else
				{	vq_color (vdi_hand, color + 1, 1, (*rgb_palette)[color]);
					(*rgb_palette)[color][3] = color + 1;
				}
			  }
			  vq_color (vdi_hand, 1, 1, (*rgb_palette)[255]);
			  (*rgb_palette)[255][3] = 1;

			  memclr (backup, sizeof (backup));
	 		  memclr (farbtbl, 32 * 256 * sizeof (int));
			  stdfm.fd_nplanes = pixel.fd_nplanes = lvplanes;

			  vro_cpyfm (vdi_hand, S_ONLY, pxy, &screen, &pixel);

			  /* Alte Farbe retten */
			  vq_color (vdi_hand, 15, 1, rgb);

			  for (color = 0; color < 256; color++)
			  {
				vs_color (vdi_hand, 15, (*rgb_palette)[color]);
				vsl_color (vdi_hand, 15);
				v_pline (vdi_hand, 2, pxy);

				stdfm.fd_addr = pixel.fd_addr = &(*farbtbl)[color][0];

				/* vro_cpyfm, weil v_get_pixel nicht mit TrueColor (>=24 Planes) funktioniert */
				vro_cpyfm (vdi_hand, S_ONLY, pxy, &screen, &pixel);

				if (farbtbl2 != NULL && bpp)
				{	(*farbtbl2)[color] = 0L;
					memcpy (&(*farbtbl2)[color], pixel.fd_addr, bpp);
				}

				vr_trnfm (vdi_hand, &pixel, &stdfm);
				for (np = 0; np < lvplanes; np++)
					if ((*farbtbl)[color][np])
						(*farbtbl)[color][np] = 0xffff;
			  }

			  /* Alte Farbe restaurieren */
			  vs_color (vdi_hand, 15, rgb);

			  pixel.fd_addr = backup;	/* Punkt restaurieren */
			  vro_cpyfm (vdi_hand, S_ONLY, pxy, &pixel, &screen);
  			  lfree( rgb_palette );
			}

			graf_mouse (M_ON, 0L);
		}
	}

	return (v_bpp=bpp);
}

void std_to_byte (unsigned int *col_data, long len, int old_planes, unsigned long (*farbtbl2)[256], MFDB *s)
{	long  x, i, mul[32], pos;
	unsigned int np, *new_data, pixel, color, back[32];
	int  memflag = 0;
	unsigned char *p1, *p2;
	unsigned long  colback;

	if (s->fd_addr == col_data)
	{
		if ((col_data = (unsigned int *)lalloc (len * 2 * s->fd_nplanes,-1)) == 0L)
			return;
		memcpy (col_data, s->fd_addr, len * 2 * s->fd_nplanes);
		memflag = 1;
	}
	new_data = (unsigned int *)s->fd_addr;
	p1 = (unsigned char *)new_data;

	if (old_planes < 8)
	{
		colback = (*farbtbl2)[(1 << old_planes) - 1];
		(*farbtbl2)[(1 << old_planes) - 1] = (*farbtbl2)[255];
	}

	for (i = 0; i < old_planes; i++)
		mul[i] = i * len;

	pos = 0;

	for (x = 0; x < len; x++)
	{
		for (np = 0; np < old_planes; np++)
			back[np] = col_data[mul[np] + x];

		for (pixel = 0; pixel < 16; pixel++)
		{
			color = 0;
			for (np = 0; np < old_planes; np++)
			{
				color |= ((back[np] & 0x8000) >> (15 - np));
				back[np] <<= 1;
			}

			switch (v_bpp)
			{
				case 2:
					new_data[pos++] = *(unsigned int *)&(*farbtbl2)[color];
					break;

				case 3:
					p2 = (unsigned char *)&(*farbtbl2)[color];
					*(p1++) = *(p2++);
					*(p1++) = *(p2++);
					*(p1++) = *(p2++);
					break;

				case 4:
					((unsigned long *)new_data)[pos++] = (*farbtbl2)[color];
					break;
			}
		}
	}

	if (old_planes < 8)
		(*farbtbl2)[(1 << old_planes) - 1] = colback;

	if (memflag)
		lfree (col_data);
}

void xfix_cicon (unsigned int *col_data, long len, int old_planes, int new_planes, MFDB *s, int devspef)
/* len: length in bytes of one bitplane of data */
{	long  x, i, old_len, rest_len, mul[32], pos;
	unsigned int np, *new_data, mask, pixel, bit, color, back[32], old_col[32], maxcol;
	char  got_mem = 0;
	MFDB  d;

	len >>= 1;

	if (old_planes == new_planes)
	{	if (s)
		{	if (new_planes == lvplanes)
			{
				d = *s;
				d.fd_stand = 0;
				s->fd_addr = col_data;
				if (d.fd_addr == s->fd_addr)
				{	if ((d.fd_addr = lalloc (len * 2 * new_planes,-1)) == 0L)
						d.fd_addr = s->fd_addr;
					else
						got_mem = 1;
				}

				vr_trnfm (vdi_hand, s, &d);
				if (got_mem)
				{
					memcpy (s->fd_addr, d.fd_addr, len * 2 * new_planes);
					lfree (d.fd_addr);
				}
			}
			else
				memcpy (s->fd_addr, col_data, len * 2 * new_planes);
		}
		return;
	}

	if (new_planes <= 8)
	{
		old_len  = old_planes * len;
		rest_len = new_planes * len - old_len;

		if (s)
		{
			new_data = &((unsigned int *)s->fd_addr)[old_len];
			memclr (new_data, rest_len * 2);
			memcpy (s->fd_addr, col_data, old_len * 2);
			col_data = s->fd_addr;
		}
		else
			new_data = (unsigned int *)&col_data[old_len];

		for (x = 0; x < len; x++)
		{
			mask = 0xffff;

			for (i = 0; i < old_len; i += len)
				mask &= (unsigned int)col_data[x+i];

			if (mask)
				for (i = 0; i < rest_len; i += len)
					new_data[x+i] |= mask;
		}

		if (s && devspef )	/* ins ger„teabh„ngige Format konvertieren */
		{
			d = *s;
			d.fd_stand = 0;
			if ((d.fd_addr = lalloc (len * 2 * new_planes,-1)) == 0L)
				d.fd_addr = s->fd_addr;

			vr_trnfm (vdi_hand, s, &d);
			if (d.fd_addr != s->fd_addr)
			{
				memcpy (s->fd_addr, d.fd_addr, len * 2 * new_planes);
				lfree (d.fd_addr);
			}
		}
	}
	else	/* TrueColor, bzw RGB-orientierte Pixelwerte */
	{
		if (!v_bpp || !s)
		{
			for (i = 0; i < new_planes; i++)
				mul[i] = i * len;

			if (old_planes < 8)
			{
				maxcol = (1 << old_planes) - 1;
				memcpy (old_col, &(*farbtbl)[maxcol], new_planes * sizeof (int));
				memclr (&(*farbtbl)[maxcol], new_planes * sizeof (int));
			}

			if (s)
			{
				new_data = &((unsigned int *)s->fd_addr)[old_len];
				memclr (new_data, rest_len * 2);
				memcpy (s->fd_addr, col_data, old_len * 2);
				col_data = s->fd_addr;
			}

			for (x = 0; x < len; x++)
			{
				bit = 1;
				for (np = 0; np < old_planes; np++)
					back[np] = col_data[mul[np] + x];

				for (pixel = 0; pixel < 16; pixel++)
				{
					color = 0;
					for (np = 0; np < old_planes; np++)
					{
						color += ((back[np] & 1) << np);
						back[np] >>= 1;
					}

					for (np = 0; np < new_planes; np++)
					{	pos = mul[np] + x;
						col_data[pos] = (col_data[pos] & ~bit) | ((*farbtbl)[color][np] & bit);
					}

					bit <<= 1;
				}
			}
			if (old_planes < 8)
				memcpy ((*farbtbl)[maxcol], old_col, new_planes * sizeof (int));

			if (s && devspef)	/* ins ger„teabh„ngige Format konvertieren */
			{
				d = *s;
				d.fd_stand = 0;
				if ((d.fd_addr = lalloc (len * 2 * new_planes,-1)) == 0L)
					d.fd_addr = s->fd_addr;

				vr_trnfm (vdi_hand, s, &d);
				if (d.fd_addr != s->fd_addr)
				{
					memcpy (s->fd_addr, d.fd_addr, len * 2 * new_planes);
					lfree (d.fd_addr);
				}
			}
		}
		else
		{
			std_to_byte (col_data, len, old_planes, farbtbl2, s);
			s->fd_stand = 0;
		}
	}
}
#endif GNVAHELP

void fix_color( int *c )
{
  int i;
  
  for( i=3; --i>=0; c++ )
    if( *c>999 ) *c = 1000;
    else if( *c<1 ) *c = 1;
}
int from_intens( int c )
{
  return (c-1)*(colmax1+1)/1000;
}
void intens_pall( int intens[][3], int pall[], int colors )
{
  int c;
  
  while( --colors>=0 )
  {
    fix_color( &intens[colors][0] );
    c = colmax & ( (from_intens(intens[colors][0])<<8) |
        (from_intens(intens[colors][1])<<4) | from_intens(intens[colors][2]) );
    if( colmax1==0xf && !TT_vid ) c = ((c&0xEEE)>>1)|((c&0x111)<<3);
    pall[colors] = c;
  }
}

int fix_pall( int *i, int ste )	/* 003: now actually modify pall[] */
{
  int c = *i;

  if( colmax1==0xf )
  {
    if( ste==2 ) return c;
    else if( ste ) return *i = ((c&0x777)<<1)|((c&0x888)>>3);
    return *i = ((c&0x777)<<1)|0x111;
  }
  else if( ste ) return *i = c&0x777;
  return c;
}
void pall_intens( int pall[], int intens[][3], int colors, int ste )
{
  int p;
  
  while( --colors>=0 )
  {
    p = fix_pall(&pall[colors],ste);
    intens[colors][0] = ((p>>8)&colmax1)*1000/colmax1;
    intens[colors][1] = ((p>>4)&colmax1)*1000/colmax1;
    intens[colors][2] = ( p    &colmax1)*1000/colmax1;
  }
}

void decode_img( unsigned char *data, unsigned char *comp, int plen, long len, int width, int planes )
{
  int w, i, vrc=1, pl, extra;
  long plinc = len/planes;
  unsigned char b, *c, *d;
  
  d = data;
  if( planes>8 )
  {
    width = ((picture->mfdb.fd_w+15)&0xfff0) * (planes>16 ? 3 : 2);
    extra = 0;
/*    width *= planes;
    if( (i = picture->mfdb.fd_w&0xf) == 0 ) extra = 0;
    else extra = (16-i) * (planes>16 ? 3 : 2); */
    planes = 1;
  }
  else extra = width&1;
  pl = planes;
  w = width;
  while( len>0 )
  {
    c = comp;
    while( vrc && len>0 )
    {
      if( (b=*c++)==0 )
        if( (b=*c++)!=0 )
        {
          while( b-- )
            for( i=-1; ++i<plen; )
            {
              *d++ = *(c+i);
              w--;
            }
          c += plen;
        }
        else if( *c!=0xff ) return;	/* end of file */
        else
        {
          vrc = *++c;
          comp = ++c;
        }
      else if( b==(unsigned char)'\x80' )
        for( i=*c++; --i>=0; )
        {
          *d++ = *c++;
          w--;
        }
      else
      {
        i = b&0x7f;
        b = (signed char)b<0 ? -1 : 0;
        while( --i>=0 )
        {
          *d++ = b;
          w--;
        }
      }
      if( w<=0 )
      {
        for( i=extra; --i>=0; )
        {
          *d++ = 0;	/* pad odd-sized scan line */
          len--;
        }
        if( !--pl )
        {
          pl = planes;
          data = d = data+width+extra;
          if( --vrc ) c = comp;
        }
        else d += plinc-width-extra;
        len -= width;
        w = width;
      }
    }
    comp = c;
    vrc = 1;
  }
}

#ifndef GNVAHELP
int pcol2( int c, int le4 )
{
  if( lvplanes>4 && c==15 && le4 ) return 1;
  return pcol(c);
}

void set_cols( int start, int *old, int *new )
{
  int c, i;
  static int le4;

  if( !new ) new = picture->use_vdi ? &picture->intens[0][0] :
      &picture->pall[0];
  if( TT_vid && !picture->use_vdi )
  {
    if( start>=picture->colors ) return;
    if( old ) EgetPalette( start, picture->colors-start, old+start );
    EsetPalette( start, picture->colors-start, new+start );
  }
  else
  {
    if( old ) le4 = picture->mfdb.fd_nplanes<=4;
    for( i=0; i<picture->colors; i++ )
    if( picture->use_vdi )
    {
      c = pcol2( i, le4 );
      if( c>=start )
      {
        if( old )
        {
          vq_color( vdi_hand, c, 1, old );
          fix_color(old);
        }
        vs_color( vdi_hand, c, new );
      }
      if( old ) old+=3;
      new+=3;
    }
    else if( i>=start )
    {
      c = Setcolor( i, *new++ );
      if( old ) *old++ = c;
    }
  }
}

int video_mode( char favor_colors, int strez )
{
  int old, rez, new, i, mon;
  char *c, pl_ok, rez_ok;
  static char ttst[]={ 7, 4, 2, 1, 0, -1 };
  static int falc[19][6]={ 320, 200, 4, STMODES|COL40|BPS4, 1, 1,
  			 640, 200, 2, STMODES|COL80|BPS2, 1, 2,
  			 640, 400, 1, STMODES|COL80|BPS1, 1, 1,
  			 640, 480, 16, COL80|BPS16, 1, 1,
  			 640, 240, 16, COL80|BPS16|VERTFLAG, 1, 2,
  			 320, 480, 16, COL40|BPS16, 2, 1,
  			 320, 240, 16, COL40|BPS16|VERTFLAG, 1, 1,
  			 640, 480, 8, COL80|BPS8, 1, 1,
  			 320, 480, 8, COL40|BPS8, 2, 1,
  			 640, 240, 8, COL80|BPS8|VERTFLAG, 1, 2,
  			 320, 240, 8, COL40|BPS8|VERTFLAG, 1, 1,
  			 640, 480, 4, COL80|BPS4, 1, 1,
  			 320, 480, 4, COL40|BPS4, 2, 1,
  			 640, 240, 4, COL80|BPS4|VERTFLAG, 1, 2,
  			 320, 240, 4, COL40|BPS4|VERTFLAG, 1, 1,
  			 640, 480, 2, COL80|BPS2, 1, 1,
  			 320, 480, 2, COL40|BPS2, 2, 1,
  			 640, 240, 2, COL40|BPS2|VERTFLAG, 1, 2,
  			 320, 240, 2, COL40|BPS2|VERTFLAG, 1, 1 };

  old_rez = -1;
  if( picture->mfdb.fd_nplanes <= lvplanes && picture->mfdb.fd_w <= lxmax &&
      picture->mfdb.fd_h <= lymax && strez<0 ) return -1;
  if( has_sblast || (unsigned long)Logbase() >= 0xC00000L ) return -1;
  if( !falc_vid )
  {
    rez = Getrez();
    if( lxmax != reztbl[rez][0] || lymax != reztbl[rez][1] ||
        lvplanes != reztbl[rez][2] ) return -1;
    if( rez == 6 ) return -1;
    if( !TT_vid )	/* reworked for 002 */
    {
      if( rez==2 ) return -1;
      if( strez>=2 )
      {
        old_rez = rez;
        return 1;	/* ST High or TT: goto ST med */
      }
    }
    if( (new=strez)<0 || !TT_vid && strez==2 )
    {
      c = TT_vid ? ttst : ttst+3;
      while( *c>=0 )
      {
        pl_ok = reztbl[*c][2]>=picture->mfdb.fd_nplanes;
        rez_ok = reztbl[*c][0]>=picture->mfdb.fd_w &&
            reztbl[*c][1]>=picture->mfdb.fd_h;
        if( favor_colors && pl_ok && (new<0 || rez_ok) ||
            !favor_colors && rez_ok && (new<0 || pl_ok) ) new = *c;
        c++;
      }
    }
    if( new<0 || new==rez ) return -1;
    old_rez = rez;
    lxmax = reztbl[new][0];
    lymax = reztbl[new][1];
    lvplanes = reztbl[new][2];
    lvwrap = reztbl[new][3];
    lxasp = reztbl[new][4];
    lyasp = reztbl[new][5];
    picture->use_vdi = 0;
    return new;
  }
  else
  {
    if( (mon=mon_type()) == 0 ) return -1;	/* ST mono monitor */
    rez = (old = Vsetmode(-1)) & ~(VERTFLAG|STMODES|COL80|NUMCOLS);
    if( strez>=0 && strez<=2 ) new = strez;
    else
    {
      i = 3;
      if( mon==2 ) i=5;
      new = -1;
      for( ; i<19; i++ )
      {
        pl_ok = falc[i][2]>=picture->mfdb.fd_nplanes;
        rez_ok = falc[i][0]>=picture->mfdb.fd_w &&
            falc[i][1]>=picture->mfdb.fd_h;
        if( favor_colors && pl_ok && (new<0 || rez_ok) ||
            !favor_colors && rez_ok && (new<0 || pl_ok) ) new = i;
      }
      if( new<0 ) return -1;
    }
    rez |= falc[new][3];
    if( mon!=2 && !(rez&STMODES) ) rez ^= VERTFLAG;
    if( rez != old )
    {
      old_rez = old;
      lxmax = falc[new][0];
      lymax = falc[new][1];
      lvplanes = falc[new][2];
      lxasp = falc[new][4];
      lyasp = falc[new][5];
      return rez;
    }
    return -1;
  }
}

#ifdef TEST
void set_video( int mode )
{
  if( falc_vid )
  {
    v_clsvwk(vdi_hand);
    xbios( 5, 0L, 0L, 3, mode );
    open_vdi();
  }
  else Setscreen( (void *)-1L, (void *)-1L, mode );
}
#else
void set_video( int mode )
{
  extern GRAPHICS *graphics;

  if( falc_vid && graphics->handle>0 )
  {
    (*graphics->graph_exit)();
    xbios( 5, 0L, 0L, 3, mode );
    (*graphics->reinit)();
  }
  else Setscreen( (void *)-1L, (void *)-1L, mode );
}
#endif
#endif GNVAHELP

int trnfm( MFDB *s, MFDB *d )
{
  void *temp=0L;
  long len;
  
  if( d->fd_nplanes==24 ) return 0;	/* 003 */
  if( s->fd_addr==d->fd_addr )
    if( (temp = lalloc(len=(long)(d->fd_wdwidth<<1)*d->fd_nplanes*d->fd_h, -1)) == 0 )
        return AENSMEM;
    else d->fd_addr = temp;
  vr_trnfm( vdi_hand, s, d );
  if( temp )
  {
    memcpy( d->fd_addr = s->fd_addr, temp, len );
    lfree(temp);
  }
  return 0;
}

void set_bytes(void)
{
  picture->mfdb.fd_wdwidth = (picture->mfdb.fd_w+15)>>4;
  picture->len = (long)(picture->mfdb.fd_wdwidth<<1)*picture->mfdb.fd_h*picture->mfdb.fd_nplanes;
}

#ifndef GNVAHELP
/* convert interleaved (ST device-specific) to standard format */
int to_stand( PICTURE *pic )
{
  unsigned int *d, *d0, *s;
  int p, pl, plen, i;
  
  if( (pl = pic->mfdb.fd_nplanes) == 1 )
  {
    pic->mfdb.fd_stand = 1;
    return 0;
  }
  picture = pic;
  set_bytes();
  if( (d=lalloc(pic->len,-1)) != 0 )
  {
    d0 = d;
    plen = pic->mfdb.fd_h*pic->mfdb.fd_wdwidth;
    for( p=0; p<pl; p++ )
      for( s=(unsigned int *)pic->mfdb.fd_addr+p, i=plen; --i>=0; )
      {
        *d++ = *s;
        s += pl;
      }
    memcpy( pic->mfdb.fd_addr, d0, pic->len );
    lfree(d0);
    pic->mfdb.fd_stand = 1;
    return 0;
  }
  return AENSMEM;
}
#endif GNVAHELP

/* 15-bit:   0RRRRRGG|GGGBBBBB
   16-bit:   RRRRRGGG|GGGBBBBB */

void tc24_to_16(void)
{
  long l;
  int w;
  unsigned char *o;
  unsigned char *i;
  
  i = picture->mfdb.fd_addr;
  o = picture->mfdb.fd_addr;
  for( l=picture->len; (l-=3)>=0; )
  {
    w = !rgb15 ? ((*i++ & 0xF8)<<(11-3)) | ((*i++ & 0xFC)<<(5-2)) | (*i++ >> 3) :
       ((*i++ & 0xF8)<<(10-3)) | ((*i++ & 0xF8)<<(5-3)) | (*i++ >> 3);
    if( intel )
    {
      *o++ = w;
      *o++ = w>>8;
    }
    else
    {
      *o++ = w>>8;
      *o++ = w;
    }
  }
}

int transform_pic( PICTURE *pic, int devspef )
{
  MFDB m;
  int i;
  void *old, *new;
  long len;

  picture = pic;  
  if( pic->mfdb.fd_nplanes>1 || devspef<0 )
  {
    if( (i=pic->mfdb.fd_nplanes-lvplanes) != 0 )
    {
#ifndef GNVAHELP
      if( !pic->mfdb.fd_stand )
        if( to_stand(pic) ) return AENSMEM;  /* Degas or Neo */
#endif
      if( i<0 )	/* current display has more planes */
      {
#ifndef GNVAHELP
        if( test_rez() < 0 ) return AENSMEM;
#endif
        i = pic->mfdb.fd_nplanes;
        pic->mfdb.fd_nplanes = lvplanes;
        set_bytes();
        if( (new = lalloc( pic->len, -1 )) == 0L ) return AENSMEM;
        old = pic->mfdb.fd_addr;
        pic->mfdb.fd_addr = new;
        xfix_cicon( old, (long)(pic->mfdb.fd_wdwidth<<1)*
            pic->mfdb.fd_h, i, lvplanes, &pic->mfdb, devspef>0 );
        lfree(old);
      }
#ifdef GNVAHELP
      else return AEPLFMT;
#else
      else if( lvplanes>8 )
      {
        scrninf();
        tc24_to_16();
        pic->mfdb.fd_nplanes = lvplanes;
        pic->colors = 0;
        set_bytes();
        lshrink( pic->mfdb.fd_addr, pic->len );
        pic->mfdb.fd_stand = 0;
      }
      else
      {
        if( !pic->colors ) pic->palette = 0L;
        else if( (pic->palette = lalloc( pic->colors*4L, -1 )) == 0L ) return AENSMEM;
        ditherfile( pic );
        cmfree((char **)&pic->palette);
        pic->mfdb.fd_nplanes = 1;
        pic->colors = 0;
        set_bytes();
        lshrink( pic->mfdb.fd_addr, pic->len );
        pic->mfdb.fd_stand = !devspef;	/* doesn't matter since it's 1 plane */
      }
#endif GNVAHELP
    }
    if( pic->mfdb.fd_stand && devspef>=-1 )	/* 003: added >=-1, nplanes test */
    {
      if( pic->mfdb.fd_nplanes>1 )	
      {
        memcpy( &m, &pic->mfdb, sizeof(MFDB) );
        m.fd_stand = 0;
        if( trnfm( &pic->mfdb, &m ) ) return AENSMEM;
      }
      pic->mfdb.fd_stand = 0;
    }
  }
  return 0;
}

#ifndef GNVAHELP
int picarr[8];

void pic_arr( int is_scr, void *logbase, MFDB *s )
{
  int i;
  
  picarr[0] = picarr[1] = 0;
  picarr[4] = (i=lxmax-picture->mfdb.fd_w)<0 ? 0 : i>>1;
  picarr[6] = picarr[4] + (picarr[2] = (i<0 ? lxmax : picture->mfdb.fd_w) - 1);
  picarr[5] = (i=lymax-picture->mfdb.fd_h)<0 ? 0 : i>>1;
  picarr[7] = picarr[5] + (picarr[3] = (i<0 ? lymax : picture->mfdb.fd_h) - 1);
  vs_clip( vdi_hand, 1, picarr+4 );
  if( is_scr ) s->fd_addr = 0L;
  else
  {
    s->fd_addr = logbase;
    s->fd_wdwidth = ((s->fd_w = lxmax)+15)>>4;
    s->fd_h = lymax;
    s->fd_stand = 0;
    s->fd_nplanes = lvplanes;
  }
}

void draw_pic( PICTURE *pic, MFDB *s /*, char cd16*/ )
{
  int i, cols[2], wid, wrap, j;
  int *v, *d, *v2, *d2;
  long l;

  if( pic->use_vdi )
    if( pic->mfdb.fd_nplanes>1 )
/*      if( cd16 )
      {
        /* hack 16-color CD II blit to memory by blitting 4 planes separately */
        cols[0] = 1;
        cols[1] = 0;
        l = pic->len>>2;
        (long)pic->mfdb.fd_addr += pic->len;
        pic->mfdb.fd_nplanes = 1;
        wrap = 10;
        picarr[5] += 4*wrap;
        picarr[7] += 4*wrap;
        for( i=4; --i>=0; )
        {
          *(long *)&pic->mfdb.fd_r1 = 0;
          pic->mfdb.fd_r3 = 0;
          *(long *)&s->fd_r1 = 0;
          s->fd_r3 = 0;
          (long)pic->mfdb.fd_addr -= l;
          picarr[5] -= wrap;
          picarr[7] -= wrap;
          vrt_cpyfm( vdi_hand, MD_TRANS, picarr, &pic->mfdb, s, cols );
          cols[0] <<= 1;
        }
        pic->mfdb.fd_nplanes = 4;
      }
      else*/ vro_cpyfm( vdi_hand, S_ONLY, picarr, &pic->mfdb, s );
    else
    {
      cols[0] = 1;
      cols[1] = 0;
      vrt_cpyfm( vdi_hand, MD_REPLACE, picarr, &pic->mfdb, s, cols );
    }
  else
  {
    v=(int *)((long)(s->fd_addr?s->fd_addr:Logbase())+
        (long)lvwrap*picarr[5]+(picarr[4]+8>>4<<1)*lvplanes);
    d=(int *)pic->mfdb.fd_addr;
    wid=picarr[2]+16>>4;
    wrap=pic->mfdb.fd_wdwidth*pic->mfdb.fd_nplanes;
    if( pic->mfdb.fd_nplanes==lvplanes )
      for( wid*=(lvplanes<<1), i=0; i++<=picarr[3]; v+=lvwrap>>1, d+=wrap )
        memcpy( v, d, wid );
    else	/* must be mono onto a color screen */
      for( i=0; i++<=picarr[3]; d+=wrap, v+=lvwrap>>1 )
        for( d2=d, v2=v, j=-1; ++j<wid; v2+=lvplanes )
          *v2 = *d2++;
  }
} 
void center_pic( PICTURE *pic, int is_scr, void *logbase/*, char cd16*/ )
{
  MFDB s;

  pic_arr( is_scr, logbase, &s );
  draw_pic( pic, &s/*, cd16*/ );
}

int save_cols( PICTURE *pic, int **oc, int mode )
{
  int i;

  if( !mode )
  {  
    if( !has_clut || pic->mfdb.fd_nplanes <= lvplanes )
    {
      i = pic->use_vdi ? pic->colors*6 : pic->colors*2;
      if( (*oc = lalloc(i,-1)) == 0 ) return AENSMEM;
      set_cols( 0, *oc, 0L );
    }
    else *oc = 0L;
  }
  else
  {
    if( *oc ) set_cols( 0, 0L, *oc );
    cmfree( (char **)oc );
  }
  return 0;
}

#ifndef TEST
int fit_screen( PICTURE *p, PICOPT *po )
{
  int wh;
  static char asps[] = { 0, -1, 1 };

  wh = po->fit ? 0 : -1;
  return fit_pic( p, 1, asps[po->mode], wh, wh );
}
#endif

int save_img( char *name, PICTURE *pic );
int disp_pic( PICTURE *pic )
{
  int *oc, i, ret=0, cols;
#ifndef TEST
  extern MASTER *mas;
  extern MOST *z;
#endif

  picture = pic;
  i = video_mode(1,pic->strez);
  cols = pic->colors;
  if( pic->mfdb.fd_nplanes>1 && pic->mfdb.fd_nplanes > lvplanes && lvplanes<=8 )
      cols = 0;		/* it's going to get dithered, so don't change colors */
  oc = 0L;
  if( pic->use_vdi && cols && save_cols( pic, &oc, 0 ) ) ret = AENSMEM;
  else if(
#ifndef TEST
     !fit_screen( pic, &z->view_picopts ) &&
#endif
     !transform_pic( pic, -1 ) )
  {
    if( i >= 0 ) set_video(i);
    if( !pic->use_vdi && save_cols( pic, &oc, 0 ) ) ret = AENSMEM;
    else
    {
#ifndef TEST
      bconws( "\033E" );
#endif
      center_pic( pic, 1, Logbase()/*, 0*/ );
#ifndef TEST
      if( (*mas->wait_key)() == 0x1f ) save_img( "$NEODSK$.IMG", pic );
#else
      if( (char)((dispkey=Bconin(2)&0xFFFFFFL)>>16) == 0x1f ) save_img( "$NEODSK$.IMG", pic );
#endif
      save_cols( pic, &oc, 1 );
    }
    if( old_rez>=0 ) set_video(old_rez);
  }
  else
  {
    save_cols( pic, &oc, 1 );
    ret = AENSMEM;
  }
  return ret;
}

int img_hand;

void iFwrite( int l, void *buf )
{
  long w;
  extern char *msg_ptr[];
  
  if( img_hand>0 && l>0 )
    if( (w=Fwrite( img_hand, l, buf )) != l )
    {
      Fclose(img_hand);
      img_hand = -1;
#ifndef TEST
      if( w >= 0 ) form_alert( 1, msg_ptr[15] );
      else TOS_error( w, 0 );
#else
      w++;
#endif
    }
}

void out_ibyte( char c )
{
  iFwrite( 1L, &c );
}

int out_iunenc( char *u, char *data )
{
  int i, j, ret;
  
  if( u )
  {
    for( i=ret=data-u; i>0; )
    {
      j = i>255 ? 255 : i;
      out_ibyte( 0x80 );
      out_ibyte( j );
      iFwrite( j, u );
      u += j;
      i -= j;
    }
    return ret;
  }
  return 0;
}

#pragma warn -par
int save_img( char *name, PICTURE *pic )
{
#if !defined(DEMO) && !defined(TEST)
  int hand, wb, w, t, p, cols, dcols[16][3], (*intens)[3];
  extern long dflt_pall[16];
  long plen, l;
  char *d, *d2, *u;
  struct
  {
    int ver;
    int hdlen;
    int planes;
    int patrun;
    int micwid, micht;
    int pixwid, pixht;
    long sign;
    int colormode;
  } header;

  picture = pic;
  set_bytes();
  header.ver = 1;
  cols = (intens=pic->intens)!=0 ? pic->colors : 0;
  if( !cols && pic->mfdb.fd_nplanes<16 )
  {
    for( w=cols=1<<pic->mfdb.fd_nplanes; --w>=0; )
      for( t=1; t<4; t++ )
        unpak_rgb( (unsigned char *)&dflt_pall[pcol(w)]+t, &dcols[w][t-1] );
    intens = dcols;
  }
  header.hdlen = (sizeof(header) + cols*3*sizeof(int))>>1;
  header.planes = pic->mfdb.fd_nplanes;
  header.patrun = 2;
  header.micwid = header.micht = 0x174;
  header.pixwid = pic->mfdb.fd_w;
  header.pixht = pic->mfdb.fd_h;
  header.sign = 0x58494d47L;	/* XIMG */
  header.colormode = 0;
  if( (img_hand=hand = Fcreate(name,0)) < 0 ) return hand;
  iFwrite( sizeof(header), &header );
  if( cols && intens ) iFwrite( cols*3*sizeof(int), intens );
  wb = (pic->mfdb.fd_w+7)>>3;
  u = d = d2 = pic->mfdb.fd_addr;
  l = pic->len;
  plen = (long)pic->mfdb.fd_wdwidth*2*pic->mfdb.fd_h;
  w = wb;
  p = 0;
  while( l>0 )
  {
    if( !d[0] || d[0]==-1 )
    {
      for( t=1; d[t]==d[0] && ++t<0x7f && t<w; );
      if( t>2 )
      {
        out_iunenc( u, d );
        u = 0L;
        out_ibyte( d[0] ? 0x80|t : t );
        w -= t;
        d += t;
        continue;
      }
    }
    for( t=2; t+2<=w && !memcmp( d+t-2, d+t, 2 ); )
      if( (t+=2) > (0x7e<<1) ) break;
    if( t>2 )
    {
      out_iunenc( u, d );
      u = 0L;
      out_ibyte( 0 );
      out_ibyte( t>>1 );
      out_ibyte( *d );
      out_ibyte( *(d+1) );
      w -= t;
      d += t;
      continue;
    }
    if( w<=0 )
    {
      out_iunenc( u, d );
      u = 0L;
      if( ++p>=header.planes )
      {
        d = d2 += wb + (wb&1);
        p = 0;
      }
      else d = d2 + plen*p;
      l -= wb + (wb&1);
      w = wb;
      continue;
    }
    if( !u ) u = d;
    d++;
    w--;
  }
  out_iunenc( u, d );
  if( img_hand<0 )
  {
    Fdelete(name);
    return img_hand;
  }
  Fclose(hand);
  return 0;
#else
  #ifndef TEST
  demo_version();
  #endif
  return AEACCDN;
#endif
}
#pragma warn +par
#endif GNVAHELP

#ifdef GNVAHELP
  int load_img( PICTURE *pic )
#else
  int load_img( int h, PICTURE *pic )
#endif
{
  struct
  {
    int ver;
    int hdlen;
    int planes;
    int patrun;
    int micwid, micht;
    int pixwid, pixht;
    long sign;
    int colormode;
  } header;
  int (*intens)[3]=0L, *pall=0L, colors, i;
  unsigned char *comp, *data;
  long len, dlen;
  
  picture = pic;
  if( cFread(h,sizeof(header),&header)==sizeof(header) &&
      (header.ver==1 || header.ver==0) )
  {
    colors = header.planes <= 8 ? 1<<header.planes : 0;
    if( *(int *)&header.sign == 0x80 ) colors = header.hdlen - 9;
    else if( header.sign==0x53545454 ) /* STTT */
        colors = (header.colormode&0x3FF);
    if( colors )
    {
      if( (pall = lalloc(colors<<1,-1)) == 0 ) return AENSMEM;
      if( (intens = lalloc(colors*6,-1)) == 0 )
      {
        lfree(pall);
        return AENSMEM;
      }
      if( header.sign==0x58494d47L ) /* XIMG */
      {
        cFread( h, colors*6, intens );
        intens_pall( intens, pall, colors );
      }
      else if( colors )
        if( header.sign==0x53545454 /* STTT */ )
        {
          cFread( h, colors<<1, pall );
          pall_intens( pall, intens, colors, 0 );
        }
        else if( *(int *)&header.sign == 0x80 )
        {
          cFread( h, (colors-2)<<1, pall+2 );
          *pall = header.sign;
          *(pall+1) = header.colormode;
          pall_intens( pall, intens, colors, 0 );
        }
        else
        {
          for( i=0; i<colors; i++ )
          {
            vq_color( vdi_hand, pcol(i), 1, intens[i] );
            fix_color( intens[i] );
          }
          if( colvals==2 ) intens[1][0] = intens[1][1] = intens[1][2] =
              intens[0][0] ? 0 : 1000;
          intens_pall( intens, pall, colors );
        }
    }
    len = cFseek( 0L, h, 2 );
    len -= cFseek( header.hdlen<<1, h, 0 );
    pic->pall = pall;
    pic->intens = intens;	/* so they will get freed if err */
    if( (data = lalloc( dlen=(long)((header.pixwid+15)>>4<<1)*header.pixht*
        header.planes, -1 )) == 0 ) return AENSMEM;
    if( (comp = lalloc(len+3,-1)) == 0L )
    {
      lfree(data);
      return AENSMEM;
    }
    cFread( h, len, comp );
    comp[len] = comp[len+1] = 0;
    comp[len+2] = 0xfe;
    pic->mfdb.fd_addr = (int *)data;
    pic->mfdb.fd_nplanes = header.planes;
    pic->mfdb.fd_w = header.pixwid;
    pic->mfdb.fd_h = header.pixht;
    pic->mfdb.fd_stand = 1;
    pic->colors = colors;
    pic->strez = -1;
    pic->xasp = header.micwid;
    pic->yasp = header.micht;
    decode_img( data, comp, header.patrun, dlen, (header.pixwid+7)>>3, header.planes );
    lfree(comp);
    set_bytes();
    return 0;
  }
  return AEPLFMT;
}

int pcol_ibm( int c, int pl )
{
  static char pall[16] = { 15, 9, 10, 11, 12, 13, 14, 8, 7, 1, 2, 3, 4, 5, 6, 0 };
  
  if( pl==1 ) return c ? 1 : 0;
  return c<16 ? pall[c] : c;
}

#ifndef GNVAHELP
void bmp_data( char *data, char *comp, int swidth, int height, int planes, int is_ico, int pos, int invert )
{
  int swb, dwb, dxw, i, pl, j, bit, k, ht=height;
  char *d, *d2, *d3;
  unsigned char b;
  long plsiz;
  
  dwb = (swidth+15)>>4<<1;		/* width of dest */
  swb = ((long)swidth*planes+31)>>5<<2;	/* width of src */
  memclr( data, (plsiz = (long)dwb * ht) * planes );
  d = data;
  if( !pos )
  {
    d += plsiz - dwb;
    dwb = -dwb;
  }
  if( planes==1 )
    for( d3=d; --ht>=0; )
    {
      if( !invert )
        for( i=swb; --i>=0; )
          *d++ = *comp++;
      else
        for( i=swb; --i>=0; )
          *d++ = ~*comp++;
      d = d3+=dwb;
    }
  else if( planes==4 )
    for( d3=d; --ht>=0; )
    {
      for( i=0; i<swb; )
        {
          bit = 7-((i&3)<<1);
          b = *comp++;
          b = (pcol_ibm(b&0xf,4) << 4) | pcol_ibm(b>>4,4);
          for( j=2; --j>=0; bit-- )
            for( d2=d, pl=4; --pl>=0; d2+=plsiz )
            {
              *d2 |= (b&1)<<bit;
              b >>= 1;
            }
          if( !(++i&3) ) d++;
        }
      d = d3+=dwb;
    }
  else if( planes==8 )
    for( d3=d; --ht>=0; )
    {
      for( i=0; i<swb; )
        {
          bit = 7-(i&7);
          b = pcol_ibm(*(unsigned char *)comp++,8);
          for( d2=d, pl=8; --pl>=0; d2+=plsiz )
          {
            *d2 |= (b&1)<<bit;
            b >>= 1;
          }
          if( !(++i&7) ) d++;
        }
      d = d3+=dwb;
    }
  if( is_ico ) bmp_data( data + plsiz*planes, comp, swidth, height, 1, 0, 0, 0 );
}

long long86( void *p )
{
  return *(long *)p = *(unsigned char *)p |
      ((unsigned int)*((unsigned char *)p+1)<<8) | 
      ((unsigned long)*((unsigned char *)p+2)<<16) | 
      ((unsigned long)*((unsigned char *)p+3)<<24);
}

int int86( void *p )
{
  return *(int *)p = *(unsigned char *)p |
      ((unsigned int)*((unsigned char *)p+1)<<8);
}

int load_bmp( int h, int is_ico, PICTURE *pic )
{
  int colors, (*intens)[3]=0L, *pall=0L, i, j;
  char *comp, *data;
  long l;
  struct
  {
    int type;
    long size;
    int reserved[2];
    long hdrsize;
  } header1;
  struct
  {
    long hdrsize;
    long width;
    long height;
    int useless;
    int planes;
    long compression;
    long size;
    long xpels, ypels;
    long used, important;
  } header2;
  
  if( (is_ico || cFread(h,sizeof(header1),&header1)==sizeof(header1) &&
      header1.type==0x424D) && cFread(h,sizeof(header2),&header2)==sizeof(header2) )
  {
    if( header2.compression ) return AEPLFMT;
    cFseek( long86(&header2.hdrsize)-sizeof(header2), h, 1 );
    colors = int86(&header2.planes) > 8 ? 0 : 1<<header2.planes;
    if( header2.planes >= 24 ) return AEPLFMT;
    if( colors )
    {
      if( (pall = lalloc(colors<<1,-1)) == 0 ) return AENSMEM;
      if( (intens = lalloc(colors*6,-1)) == 0 )
      {
        lfree(pall);
        return AENSMEM;
      }
      for( i=0; i<colors; i++ )
      {
        cFread( h, 4L, &l );
        j = pcol_ibm( i, header2.planes );
        unpak_rgb( (unsigned char *)&l+2, &intens[j][0] );
        unpak_rgb( (unsigned char *)&l+1, &intens[j][1] );
        unpak_rgb( (unsigned char *)&l, &intens[j][2] );
      }
      intens_pall( intens, pall, colors );
    }
    pic->pall = pall;
    pic->intens = intens;	/* so they will get freed if err */
    long86( &header2.width );
    long86( &header2.height );
    if( (data = lalloc( (long)((header2.width+15)>>4<<1)*header2.height*
        header2.planes, -1 )) == 0 ) return AENSMEM;
    if( (comp = lalloc( l=(long)((header2.width+31)>>5<<2)*header2.height*
        header2.planes, -1 )) == 0 )
    {
      lfree(data);
      return AENSMEM;
    }
    cFread( h, l, comp );
    pic->mfdb.fd_addr = (int *)data;
    pic->mfdb.fd_nplanes = header2.planes;
    pic->mfdb.fd_w = header2.width;
    if( is_ico ) header2.height >>= 1;
    pic->mfdb.fd_h = header2.height;
    pic->mfdb.fd_stand = 1;
    pic->colors = colors;
    pic->strez = -1;
    pic->xasp = pic->yasp = 1;
    bmp_data( data, comp, header2.width, header2.height, header2.planes, is_ico, 0, colors==2 && !pall[0] );
    lfree(comp);
    picture=pic;
    set_bytes();
    return 0;
  }
  return AEPLFMT;
}

/******
static int curplane, curln, curcol;

void putword( char *pic, unsigned char *data )
{
  pic += (curplane<<1)+curln*160+(curcol<<3);
  *pic++ = *data++;
  *pic   = *data;
  if( ++curln>=200 )
  {
    curln=0;
    if( ++curcol>=20 )
    {
      curcol=0;
      curplane++;
    }
  }
}

void decode_tny( char *pic, unsigned char *ctrlptr, int ctrlcnt )
{
  int i, j;
  unsigned char *dataptr;

  dataptr=ctrlptr+ctrlcnt;
  curplane=curln=curcol=0;
  while( curplane<4 )
  {
    if( *ctrlptr>=128 )
      for( j=256-*ctrlptr++; --j>=0; )
      {
        putword( pic, dataptr );
        dataptr += 2;
      }
    else if( *ctrlptr==0 )
    {
      for( j=(*(ctrlptr+1)<<8)+*(ctrlptr+2); --j>=0; )
        putword( pic, dataptr );
      ctrlptr += 3;
      dataptr += 2;
    }
    else if( *ctrlptr==1 )
    {
      for( j=(*(ctrlptr+1)<<8)+*(ctrlptr+2); --j>=0; )
      {
        putword( pic, dataptr );
        dataptr+=2;
      }
      ctrlptr += 3;
    }
    else for( j=*ctrlptr++; --j>=0; )
    {
      putword( pic, dataptr );
      dataptr+=2;
    }
  }
}
*******/

int load_other( int hand, int type )
{
  char *optr, *sptr0=0L, *pic, *sptr, c;
  int i, j, l, r, *pall, (*intens)[3], num, ttm, newrez, k, cntrls;
  static int widths[] = { 40, 80, 80, 0, 320, 0, 160, 320 },
      nextpl[] = { 6, 2, 0, 0, 6, 0, 0, 14 };
  void cdecl tiny_decompress( char *controls, char *data, char *out );

  if( (pall = lalloc(16<<1,-1)) == 0 ) return AENSMEM;
  if( (intens = lalloc(16*6,-1)) == 0 )
  {
    lfree(pall);
    return AENSMEM;
  }
  picture->pall = pall;	/* so they will get freed if err */
  picture->intens = intens;
  if( (pic = lalloc(32000L,-1)) == 0 ) return AENSMEM;
      switch(type)
      {
        case 1:		/* TNY */
          cFread( hand, 1L, &c );
          if( (r=c) > 2 )
          {
            r -= 3;
            cFseek( 4L, hand, 1 );
          }
	  cFread( hand, 32L, pall );
	  cFread( hand, 2L, &cntrls );
	  cFread( hand, 2L, &num );
	  if( (sptr0 = lalloc(32000L,-1)) == 0 )
	  {
	    lfree(pic);
	    return AENSMEM;
	  }
          break;
	case 2:		/* NEO */
	  cFread( hand, 2L, &r );
	  cFread( hand, 2L, &r );
	  cFread( hand, 32L, pall );
	  cFseek( 92L, hand, 1 );
	  break;
	case 3:
	case 4:		/* Degas */
	  cFread( hand, 2L, &r );
	  cFread( hand, 32L, pall );
	  if( r&0x8000 && (sptr0 = lalloc(32000L,-1)) == 0 )
	  {
	    lfree(pic);
	    return AENSMEM;
	  }
	  r &= 3;
	  break;
      }
      if( r>7 || r==3 || r==5 )		/* 003 */
      {
        lfree(pic);
        return AEPLFMT;
      }
      cFread( hand, 32000L, sptr0 ? sptr0 : pic );
      picture->mfdb.fd_addr = (int *)pic;
      picture->mfdb.fd_w = reztbl[r][0];
      picture->mfdb.fd_h = reztbl[r][1];
      picture->mfdb.fd_stand = 0;
      picture->strez = r;
      picture->xasp = reztbl[r][4];
      picture->yasp = reztbl[r][5];
      if( r==2 && pall[0]==pall[1] )	/* 003 */
      {
        pall[0] = 0x777;
        pall[1] = 0;
      }
      pall_intens( pall, intens, picture->colors =
          1<<(picture->mfdb.fd_nplanes = reztbl[r][2]), r>2/* 003: was 0 */ );
      set_bytes();
      if( type==1 )
      {
/*        decode_tny( pic, sptr0, cntrls );*/
        tiny_decompress( sptr0, sptr0+cntrls, pic );
        lfree( sptr0 );
      }
      else if( sptr0 )
      {
	sptr = sptr0;
	for( l=0; l<reztbl[r][1]; l++, pic+=reztbl[r][3] )
	  for( k=0; k<reztbl[r][2]; k++ )
	  {
	    optr = pic + (k<<1);
	    for( j=widths[r]; j>0; )
	      if( *sptr>='\0' && (int)*sptr<='\x7f' )
	      {
		i = *sptr++;
		while( i-- >= 0 )
		{
		  *optr++ = *sptr++;
		  if( j--&1 ) optr += nextpl[r];
		}
	      }
	      else if( *sptr != -128 )
	      {
		i = -(*sptr++);
		while( i-- >= 0 )
		{
		  *optr++ = *sptr;
		  if( j--&1 ) optr += nextpl[r];
		}
		sptr++;
	      }
	      else sptr++;
	  }
	lfree(sptr0);
      }
  return 0;
}

void free_pic( PICTURE **pic, int do_pic )
{
  if( *pic )
  {
    cmfree( (char **)&(*pic)->mfdb.fd_addr );
    cmfree( (char **)&(*pic)->pall );
    cmfree( (char **)&(*pic)->intens );
    cmfree( (char **)&(*pic)->palette );
    if( do_pic ) cmfree( (char **)pic );
  }
  cmfree( (char **)&farbtbl );
  cmfree( (char **)&farbtbl2 );
  v_bpp = -1;	/* so that farbtbls will be redone */
}

int load_pic( int hand0, char *filename, PICTURE **pic, char *title )
{
  int type, picrez, j, ret=0, hand;
  extern char *msg_ptr[];

  if( hand0>0 ) hand = hand0;
  else if( (hand = cFopen(filename,0)) < 0 ) return hand;
  j = find_extn( filename );
  picrez = filename[j+3] <= '6' && filename[j+3] >= '1';
  if( !strncmpi( filename+j, ".IMG", 3 ) ) type = 0;
  else if( !strncmpi( filename+j, ".TN", 3 ) && (picrez ||
      (filename[j+3]&0x5f)=='Y') ) type = 1;
  else if( !strcmpi( filename+j, ".NEO" ) ) type = 2;
  else if( !strcmpi( filename+j, ".BMP" ) ) type = 5;
  else if( !strncmpi( filename+j, ".PI", 3 ) && picrez ) type = 3;
  else if( !strncmpi( filename+j, ".PC", 3 ) && picrez ) type = 4;
  else ret = AEPLFMT;
  if( !ret )
    if( (picture = lalloc( sizeof(PICTURE), -1 )) == 0L )
    {
/*      if( hand0<=0 ) cFclose(hand);  003: redundant */
      ret = AENSMEM;
    }
    else
    {
#ifndef TEST
      if( title ) menu_msg(title);
#else
      title++;
#endif
      memclr(picture,sizeof(PICTURE));
      picture->use_vdi = 1;
      if( type==0 ) ret = load_img( hand, picture );
      else if( type==5 ) ret = load_bmp( hand, 0, picture );
      else ret = load_other( hand, type );
      *pic = picture;
      if( ret )
      {
        free_pic( pic, 1 );
        if( hand>0 ) cFseek( 0L, hand, 0 );	/* 003 */
      }
    }
  if( hand0<=0 ) cFclose(hand);
  return ret;
}
#endif GNVAHELP

unsigned int *copy2( unsigned int *to, unsigned int *from )
{
  *((char *)to)++ = *((char *)from)++;
  *((char *)to)++ = *((char *)from);
  return to;
}

unsigned int *copy3( unsigned int *to, unsigned int *from )
{
  *((char *)to)++ = *((char *)from)++;
  *((char *)to)++ = *((char *)from)++;
  *((char *)to)++ = *((char *)from);
  return to;
}

unsigned int rol( int n ) 0xE358;

#ifndef GNVAHELP
int rsz_to_stand( PICTURE *p )
{
  if( !p->mfdb.fd_stand )
    if( to_stand(p) ) return AENSMEM;
  return 0;
}
#endif

int resize_x( PICTURE *p, int new )
{
  int old, d, d0, i1, i2, x, y, l, pl;
  unsigned int *ip, *ip0, *op, i, o, *ip2, *op2;
  unsigned int *(*copyfunc)( unsigned int *to, unsigned int *from );

  old = p->mfdb.fd_w;
  if( new==old ) return 0;
#ifndef GNVAHELP
  if( rsz_to_stand(p) ) return AENSMEM;
#endif
  p->mfdb.fd_w = new;
  picture = p;
  set_bytes();
  pl=p->mfdb.fd_nplanes;
  if( (op=lalloc(p->len+(p->mfdb.fd_wdwidth<<1)*pl,-1)) == 0 ) return AENSMEM;
  memclr( op, p->len );
  ip = ip0 = p->mfdb.fd_addr;
  p->mfdb.fd_addr = op;
  if( pl>8 )
  {
    if( pl>16 )
    {
      copyfunc = copy3;
      i = 3;
    }
    else
    {
      copyfunc = copy2;
      i = 2;
    }
    o = ((old+15)&0xfff0) * i;
    y = ((new+15)&0xfff0) * i;
    ip2 = ip;
    op2 = op;
  }
  if( --new > --old )
  {
    d0 = (i1 = old+old) - new;
    i2 = (old-new) << 1;
    if( pl<=8 )
      for( ; --pl>=0; )
        for( l=p->mfdb.fd_h; --l>=0; )
        {
          i = rol(*ip++);
          o = i&1;
          for( x=y=0, d=d0; x<new; )
          {
            if( !(++x & 15) )
            {
              *op++ = o;
              o = 0;
            }
            else o += o;
            o |= i&1;
            if( d<0 ) d += i1;
            else
            {
              if( !(++y & 15) ) i = *ip++;
              i = rol(i);
              d += i2;
            }
          }
          *op++ = o<<(-x&15);
        }
    else
      for( l=p->mfdb.fd_h; --l>=0; )
      {
        (*copyfunc)( op, ip );
        for( x=new+1, d=d0; --x>=0; )
        {
          (char *)op += i;
          if( d<0 ) d += i1;
          else
          {
            (char *)ip += i;
            d += i2;
          }
          (*copyfunc)( op, ip );
        }
        (char *)ip = (char *)ip2 += o;
        (char *)op = (char *)op2 += y;
      }
  }
  else
  {
    d0 = (i1 = new+new) - old;
    i2 = (new-old) << 1;
    if( pl<=8 )
      for( ; --pl>=0; )
        for( l=p->mfdb.fd_h; --l>=0; )
        {
          i = rol(*ip++);
          o = i&1;
          for( x=1, y=0, d=d0; y<old; )
          {
            if( !(++y & 15) ) i = *ip++;
            i = rol(i);
            if( d<0 ) d += i1;
            else
            {
              if( !(++x & 15) )
              {
                *op++ = o;
                o = 0;
              }
              else o += o;
              o |= i&1;
              d += i2;
            }
          }
          if( (x&=15)!=0 ) *op++ = o<<(16-x);
        }
    else
      for( l=p->mfdb.fd_h; --l>=0; )
      {
        (*copyfunc)( op, ip );
        for( x=old+1, d=d0; --x>=0; )
        {
          (char *)ip += i;
          if( d<0 ) d += i1;
          else
          {
            (char *)op += i;
            d += i2;
            (*copyfunc)( op, ip );
          }
        }
        (char *)ip = (char *)ip2 += o;
        (char *)op = (char *)op2 += y;
      }
  }
  lfree(ip0);
  return 0;
}

int resize_y( PICTURE *p, int new )
{
  int d, old, d0, i1, i2, x, pl, w;
  unsigned int *ip, *ip2, *ip0, *op;
  long pls;

  old = p->mfdb.fd_h;
  if( new==old ) return 0;
#ifndef GNVAHELP
  if( rsz_to_stand(p) ) return AENSMEM;
#endif
  p->mfdb.fd_h = new;
  picture = p;
  set_bytes();
  w = p->mfdb.fd_wdwidth<<1;
  pl = p->mfdb.fd_nplanes;
  if( (op=lalloc(p->len+w*pl,-1)) == 0 ) return AENSMEM;
  if( pl>8 )
  {
    if( pl>16 ) w *= 24;
    else w <<= 4;
    pl = 1;
  }
  ip = ip0 = p->mfdb.fd_addr;
  p->mfdb.fd_addr = op;
  if( --new > --old )
  {
    pls = (long)(old+1) * w;		/* 004: long */
    d0 = (i1 = old+old) - new;
    i2 = (old-new) << 1;
    for( ip2=ip; --pl>=0; )
    {
      memcpy( op, ip, w );
      for( x=new+1, d=d0; --x>=0; )
      {
        (char *)op += w;
        if( d<0 ) d += i1;
        else
        {
          (char *)ip += w;
          d += i2;
        }
        memcpy( op, ip, w );
      }
      (char *)ip = (char *)ip2 += pls;
    }
  }
  else
  {
    pls = (long)(new+1) * w;		/* 004: long */
    d0 = (i1 = new+new) - old;
    i2 = (new-old) << 1;
    for( ip2=op; --pl>=0; )
    {
      memcpy( op, ip, w );
      for( x=old+1, d=d0; --x>=0; )
      {
        (char *)ip += w;
        if( d<0 ) d += i1;
        else
        {
          (char *)op += w;
          d += i2;
          memcpy( op, ip, w );
        }
      }
      (char *)op = (char *)ip2 += pls;
    }
  }
  lfree(ip0);
  return 0;
}

int fit_pic( PICTURE *p, int inside, int aspect, int w, int h )
{ /* aspect: 0:fit w,h  -1:screen aspect  1:picture aspect */
  int r, cw, ch, nw, nh;
  unsigned long xa, ya;
  
  cw = p->mfdb.fd_w;
  ch = p->mfdb.fd_h;
  if( !w ) w = lxmax;
  else if( w<0 ) w = cw;
  if( !h ) h = lymax;
  else if( h<0 ) h = ch;
  if( aspect )
  {
    if( aspect>0 )
    {
      xa = cw;
      ya = ch;
    }
    else
    {
      xa = (unsigned long)p->xasp*lyasp;
      ya = (unsigned long)p->yasp*lxasp;
      if( xa==ya ) xa = ya = 1;
      else for( r=2; r<5; r++ )
        while( !(xa%r) && !(ya%r) )
        {
          xa /= r;
          ya /= r;
        }
      xa *= cw;
      ya *= ch;
    }
    if( !xa || !ya ) xa = ya = 1;
    nw = (unsigned long)w*xa/ya;
    nh = (unsigned long)nw*ya/xa;
    if( inside ? nw > w : nw < w ) nh = (unsigned long)(nw=w)*ya/xa;
    if( inside ? nh > h : nh < h ) nw = (unsigned long)(nh=h)*xa/ya;
    w = nw;
    h = nh;
  }
  if( (r = resize_x( p, w )) != 0 ) return r;
  return resize_y( p, h );
}

#if defined(GNVAHELP) || defined(TEST)
int pcol( int c )
{
  static char pall[16] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };
  
  if( lvplanes>4 )
    if( c==255 ) return 1;
    else if( c==15 ) return lvplanes==8 ? 255 : 16;
  return c<16 ? pall[c] : (lvplanes==8 ? c : c+1);
}
#endif

#ifndef GNVAHELP
#ifdef TEST
void cdecl bconws( char *ptr )
{
  while( *ptr ) Crawio( *ptr++ );
}
void cmfree( char **ptr )
{
  if( *ptr )
  {
    lfree(*ptr);
    *ptr = NULL;
  }
}
int pathend( char *ptr )
{
  register char *ch;

  if( (ch=strrchr(ptr,'\\')) == NULL ) return(0);
  return( ch - ptr + 1 );
}
int find_extn( char *ptr )
{
  register int i;

  i = pathend(ptr);
  while( *(ptr+i) != '.' && *(ptr+i) ) i++;
  return(i);
}
void open_vdi(void)
{
  int dum, ex[57];
  static int work_in[] = { 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 2 }, work_out[57];

  vdi_hand = graf_handle( &dum, &dum, &dum, &dum );
  v_opnvwk( work_in, &vdi_hand, work_out );
  lxmax = work_out[0]+1;
  lymax = work_out[1]+1;
  lxasp = work_out[3];
  lyasp = work_out[4];
  colvals = work_out[39];
  if( colvals<=512 )
  {
    colmax = 0x777;
    colmax1 = 7;
  }
  else
  {
    colmax = 0xFFF;
    colmax1 = 0xF;
  }
  vq_extnd( vdi_hand, 1, ex );
  lvplanes = ex[4];
}
int main( int argc, char *argv[] )
{
  char *filename;
  void *new, *p;
  long vid, l;
  int x, y, x0, y0, fac, pl, c;
  PICTURE *pic;
  
  filename = argc>1 ? argv[1] : "h:\\arabesq\\grib5.img";
  appl_init();
  v_bpp = -1;
  open_vdi();
  has_sblast = CJar( 0, SBL_COOKIE, &vid ) == CJar_OK;
  if( CJar( 0, _VDO_COOKIE, &vid ) != CJar_OK ) vid = 0L;
  else vid &= 0x00FF0000L;
  falc_vid = vid==0x30000L;
  TT_vid = vid==0x20000L;
  load_pic( 0, filename, &pic, 0L );
  if( !pic->mfdb.fd_stand ) to_stand(pic);
  x = x0 = pic->mfdb.fd_w;
  y = y0 = pic->mfdb.fd_h;
  pl = pic->mfdb.fd_nplanes;
  c = pic->colors;
  if( (new = lalloc( pic->len, -1 )) == 0L ) goto end;
  memcpy( new, pic->mfdb.fd_addr, l=pic->len );
  for(;;)
  {
    printf( "\033Ew0=%d h0=%d  w=%d h=%d  E: Expand  A: Aspect  B: Both",
        x0, y0, pic->mfdb.fd_w, pic->mfdb.fd_h );
    disp_pic( pic );
    if( (p = lalloc( l, -1 )) == 0L ) goto end;
    memcpy( p, new, l );
    lfree(pic->mfdb.fd_addr);
    pic->colors = c;
    pic->mfdb.fd_addr = p;
    pic->mfdb.fd_w = x0;
    pic->mfdb.fd_h = y0;
    pic->mfdb.fd_stand = 1;
    pic->mfdb.fd_nplanes = pl;
    set_bytes();
    fac = Kbshift(-1)&3 ? 8 : 1;
    switch( (int)(dispkey>>16L) )
    {
      default:
end:    v_clsvwk( vdi_hand );
        appl_exit();
        if(new) lfree(new);
        return 0;
      case 0x12:
        if( fit_pic( pic, 1, 0, 0, 0 ) ) goto end;
        break;
      case 0x1E:
        if( fit_pic( pic, 1, -1, -1, -1 ) ) goto end;
        break;
      case 0x30:
        if( fit_pic( pic, 1, -1, 0, 0 ) ) goto end;
        break;
      case 0x48:
        y-=fac;
        if( resize_x( pic, x ) || resize_y( pic, y ) ) goto end;
        break;
      case 0x4D:
        x+=fac;
        if( resize_x( pic, x ) || resize_y( pic, y ) ) goto end;
        break;
      case 0x50:
        y+=fac;
        if( resize_x( pic, x ) || resize_y( pic, y ) ) goto end;
        break;
      case 0x4B:
        x-=fac;
        if( resize_x( pic, x ) || resize_y( pic, y ) ) goto end;
        break;
    }
  }
}

#else TEST

extern PICTURE *desk_pic;

void free_desk_pic(void)
{
  extern MOST *z;

  free_pic( &desk_pic, 1 );
  cmfree( (char **)&z->pic_ptr );
}

void set_temps( int dplanes )
{
  lvplanes = dplanes;
  lvwrap = vwrap;
  lxmax = xmax;
  lymax = ymax;
  lxasp = xasp;
  lyasp = yasp;
}

int load_desk_pic(void)
{
  int ret=1, i, x, y;
  extern MOST *z;
  extern char *msg_ptr[];
  char *mem/*, cd16*/;
  int *oc;
  long l;
  MFDB s;
  extern OBJECT *menu;

  free_desk_pic();  
  if( z->desk_pic[0] && z->show_pic )
  {
    set_temps(vplanes);
    if( (mem = lalloc(l=(long)lxmax*lymax*lvplanes>>3L,-1)) == 0 ) return AENSMEM;
    wind_lock(1);
    bee();
    if( (ret = load_pic( 0, z->desk_pic, &desk_pic, msg_ptr[149] )) == 0 )
    {
      if( !z->wall_pic ) fit_screen( desk_pic, &z->desk_picopts );
      z->pic_ptr = (long)mem;
      if( !has_clut )
      {
        if( save_cols( desk_pic, &oc, 0 ) ) return AENSMEM;
      }
      else if( z->pic_colormode!=2 ) set_cols( z->pic_colormode?16:0, 0L, 0L );
      if( (ret = transform_pic( desk_pic, -1 )) == 0 )
        if( z->wall_pic )
        {
          pic_arr( 0, mem, &s );
          x = picarr[2];
          y = picarr[3];
          desk_pic->use_vdi = 1;
          for( picarr[5]=z->maximum.y/*003*/; picarr[5]<lymax; picarr[5]+=y+1 )
            for( picarr[4]=0; picarr[4]<lxmax; picarr[4]+=x+1 )
            {
              if( (i = picarr[4] + x) >= lxmax )
              {
                i = lxmax-1;
                picarr[2] = i - picarr[4];
              }
              else picarr[2] = x;
              picarr[6] = i;
              if( (i = picarr[5] + y) >= lymax )
              {
                i = lymax-1;
                picarr[3] = i - picarr[5];
              }
              else picarr[3] = y;
              picarr[7] = i;
              draw_pic( desk_pic, &s/*, cd16*/ );
            }
        }
        else
        {
          memclr( mem, l );
          center_pic( desk_pic, 0, mem/*, cd16*/ );
        }
      if( !has_clut ) save_cols( desk_pic, &oc, 1 );
      free_pic( &desk_pic, 1 );
    }
    else lfree(mem);
    menu_msg(0L);
    arrow();
    wind_lock(0);
  }
  if( ret==AEPLFMT ) f_alert1( msg_ptr[11] );
  return ret;
}

int view_pic( int hand, char *name )
{
  PICTURE *pic;
  int ret;
  extern MOST *z;
  
  set_temps(vplanes);
  wind_lock(1);
  if( (ret=load_pic( hand, name, &pic, msg_ptr[149] )) == 0 )
  {
    disp_pic( pic );
    free_pic( &pic, 1 );
  }
  menu_msg(0L);
  wind_lock(0);
  return ret;
}
#endif
#endif GNVAHELP

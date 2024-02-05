#include "stdlib.h"
#include "tos.h"
#include "string.h"
#include "stdio.h"

#define BUFLEN 32766L

long fpos, ipos0, ipos, longer, start;
int ohand;
unsigned long ilen;
char *obufptr, *iptr, copybuf[10240], *optr;

#define N		 4096	/* size of ring buffer */
#define F		   18	/* upper limit for match_length */
#define THRESHOLD	2   /* encode string into position and length
						   if match_length is greater than this */
#define NIL			N	/* index for root of binary search trees */

unsigned char
  text_buf[N + F - 1];	/* ring buffer of size N,
  with extra F-1 bytes to facilitate string comparison */
int match_position, match_length,  /* of longest match.  These are
		set by the InsertNode() procedure. */
  lson[N + 1], rson[N + 257], dad[N + 1];  /* left & right children &
		parents -- These constitute binary search trees. */

int _getc(void)
{
  if( !ilen ) return EOF;
  ilen--;
  return *((unsigned char *)iptr)++;
}

void writ( int hand, int len, char *ptr )
{
  ptr -= (len <<= 1);
  if( obufptr ) memcpy( obufptr+fpos, ptr, len );
  else if( Fwrite( hand, len, ptr ) != len )
  {
    printf( "Write error" );
    exit(-1);
  }
  fpos += len;
  longer += len;
}

void writb( int hand, int num )
{
  if( obufptr ) obufptr[fpos] = num;
  else if( Fwrite( hand, 1L, (char *)&num+1 ) != 1L )
  {
    printf( "Write error" );
    exit(-1);
  }
  fpos++;
  longer++;
}

void compare( char cmp )
{
  if( cmp != *optr++ )
  {
    printf( "\nMismatch at %lX actual (%lX arc)", fpos+0x1c, ipos0-ilen+start );
    exit(-1);
  }
  fpos++;
}

#define _putc(c)	writb(ohand,c)

void Decode(void)	/* Just the reverse of Encode(). */
{
	int  i, j, k, r, c;
	unsigned int  flags;
	
	memset( text_buf, 0, N-F );
/*	for (i = 0; i < N - F; i++) text_buf[i] = ' '; */
	r = N - F;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = _getc()) == EOF) break;
			flags = c | 0xff00;		/* uses higher byte cleverly */
		}							/* to count eight */
		if (flags & 1) {
			if ((c = _getc()) == EOF) break;
			compare(c);  text_buf[r++] = c;  r &= (N - 1);
		} else {
			if ((i = _getc()) == EOF) break;
			if ((j = _getc()) == EOF) break;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				compare(c);  text_buf[r++] = c;  r &= (N - 1);
			}
		}
	}
}

void InitTree(void)  /* initialize trees */
{
	int  i;

	/* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
	   left children of node i.  These nodes need not be initialized.
	   Also, dad[i] is the parent of node i.  These are initialized to
	   NIL (= N), which stands for 'not used.'
	   For i = 0 to 255, rson[N + i + 1] is the root of the tree
	   for strings that begin with character i.  These are initialized
	   to NIL.  Note there are 256 trees. */

	for (i = N + 1; i <= N + 256; i++) rson[i] = NIL;
	for (i = 0; i < N; i++) dad[i] = NIL;
}

void InsertNode(int r)
	/* Inserts string of length F, text_buf[r..r+F-1], into one of the
	   trees (text_buf[r]'th tree) and returns the longest-match position
	   and length via the global variables match_position and match_length.
	   If match_length = F, then removes the old node in favor of the new
	   one, because the old one will be deleted sooner.
	   Note r plays double role, as tree node and position in buffer. */
{
	int  i, p, cmp;
	unsigned char  *key;

	cmp = 1;  key = &text_buf[r];  p = N + 1 + key[0];
	rson[r] = lson[r] = NIL;  match_length = 0;
	for ( ; ; ) {
		if (cmp >= 0) {
			if (rson[p] != NIL) p = rson[p];
			else {  rson[p] = r;  dad[r] = p;  return;  }
		} else {
			if (lson[p] != NIL) p = lson[p];
			else {  lson[p] = r;  dad[r] = p;  return;  }
		}
		for (i = 1; i < F; i++)
			if ((cmp = key[i] - text_buf[p + i]) != 0)  break;
		if (i > match_length) {
			match_position = p;
			if ((match_length = i) >= F)  break;
		}
	}
	dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
	dad[lson[p]] = r;  dad[rson[p]] = r;
	if (rson[dad[p]] == p) rson[dad[p]] = r;
	else                   lson[dad[p]] = r;
	dad[p] = NIL;  /* remove p */
}

void DeleteNode(int p)  /* deletes node p from tree */
{
	int  q;
	
	if (dad[p] == NIL) return;  /* not in tree */
	if (rson[p] == NIL) q = lson[p];
	else if (lson[p] == NIL) q = rson[p];
	else {
		q = lson[p];
		if (rson[q] != NIL) {
			do {  q = rson[q];  } while (rson[q] != NIL);
			rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
			lson[q] = lson[p];  dad[lson[p]] = q;
		}
		rson[q] = rson[p];  dad[rson[p]] = q;
	}
	dad[q] = dad[p];
	if (rson[dad[p]] == p) rson[dad[p]] = q;  else lson[dad[p]] = q;
	dad[p] = NIL;
}

void Encode(void)
{
	int  i, c, len, r, s, last_match_length;
	unsigned char  code_buf[17], *code_buf_ptr, mask;
	
	InitTree();  /* initialize trees */
	code_buf[0] = 0;  /* code_buf[1..16] saves eight units of code, and
		code_buf[0] works as eight flags, "1" representing that the unit
		is an unencoded letter (1 byte), "0" a position-and-length pair
		(2 bytes).  Thus, eight units require at most 16 bytes of code. */
	code_buf_ptr = code_buf+1;
	mask = 1;
	s = 0;  r = N - F;
	memset( &text_buf[s], 0, r );
/*	for (i = s; i < r; i++) text_buf[i] = ' ';*/  /* Clear the buffer with
		any character that will appear often. */
	for (len = 0; len < F && (c = _getc()) != EOF; len++)
		text_buf[r + len] = c;  /* Read F bytes into the last F bytes of
			the buffer */
	for (i = 1; i <= F; i++) InsertNode(r - i);  /* Insert the F strings,
		each of which begins with one or more 'space' characters.  Note
		the order in which these strings are inserted.  This way,
		degenerate trees will be less likely to occur. */
	InsertNode(r);  /* Finally, insert the whole string just read.  The
		global variables match_length and match_position are set. */
	do {
		if (match_length > len) match_length = len;  /* match_length
			may be spuriously long near the end of text. */
		if (match_length <= THRESHOLD) {
			match_length = 1;  /* Not long enough match.  Send one byte. */
			code_buf[0] |= mask;  /* 'send one byte' flag */
			*code_buf_ptr++ = text_buf[r];  /* Send uncoded. */
		} else {
			*code_buf_ptr++ = (unsigned char) match_position;
			*code_buf_ptr++ = (unsigned char)
				(((match_position >> 4) & 0xf0)
			  | (match_length - (THRESHOLD + 1)));  /* Send position and
					length pair. Note match_length > THRESHOLD. */
		}
		if ((mask <<= 1) == 0) {  /* Shift mask left one bit. */
			for (i = code_buf_ptr-code_buf, code_buf_ptr=code_buf; --i>=0; )  /* Send at most 8 units of */
				_putc(*code_buf_ptr++);     /* code together */
			code_buf[0] = 0;
		        code_buf_ptr = code_buf+1;
		        mask = 1;
		}
		last_match_length = match_length;
		for (i = 0; i < last_match_length &&
				(c = _getc()) != EOF; i++) {
			DeleteNode(s);		/* Delete old strings and */
			text_buf[s] = c;	/* read new bytes */
			if (s < F - 1) text_buf[s + N] = c;  /* If the position is
				near the end of buffer, extend the buffer to make
				string comparison easier. */
			s = (s + 1) & (N - 1);  r = (r + 1) & (N - 1);
				/* Since this is a ring buffer, increment the position
				   modulo N. */
			InsertNode(r);	/* Register the string in text_buf[r..r+F-1] */
		}
		while (i++ < last_match_length) {	/* After the end of text, */
			DeleteNode(s);					/* no need to read, but */
			s = (s + 1) & (N - 1);  r = (r + 1) & (N - 1);
			if (--len) InsertNode(r);		/* buffer may not be empty. */
		}
	} while (len > 0);	/* until length of string to be processed is zero */
	if (code_buf_ptr-code_buf > 1) {		/* Send remaining code. */
		for (i = code_buf_ptr-code_buf, code_buf_ptr=code_buf; --i>=0; )
		    _putc(*code_buf_ptr++);
	}
}

void copy_file( int ahand, int ohand, long ind )
{
  long ilen;
  
  while( ind && (ilen=Fread(ahand,ind>sizeof(copybuf)?sizeof(copybuf):ind,copybuf))
      > 0 )
  {
    Fwrite( ohand, ilen, copybuf );
    ind -= ilen;
  }
}

long get_timer(void)
{
  return *(long *)0x4ba;
}
/* format of archive:

main program:
  magic                 601a
  text size
  data size
  BSS size
  symbol table size (length of archived entries)
  10 unused bytes
  text segment...
  data segment...
  symbol table (archive is here)...
  fixup offset
  fixups

archived programs:
  name                  "XXXXXXXX.XXX"
  pointer to next
  text size+data size
  BSS size
  compressed data...
*/

main( int argc, char *argv[] )
{
  int ihand, ahand, i, j, name_len;
  char *ptr, *ibuf, *bbuf, is_exe, long_name, *name;
  unsigned char c;
  unsigned long ind, totl, len0, timer;
  typedef struct
  {
    int magic;
    long text, data, bss, sym;
    char unused[10];
  } PRGHDR;
  PRGHDR prghdr, iprghdr;
  struct
  {
    char name[12];
    long next, text, data, bss, fixup, total;
  } archdr;
  
  if( argc<4 || *argv[1]!='a' && *argv[1]!='x' && *argv[1]!='A' )
  {
    Cconws( "\033EFormat:  compress a|x|A archive file1 [-name_in_arc1] [file2] [-name2] ..." );
    return(-1);
  }
  long_name = *argv[1]=='A';
  while( argc >= 4 )
  {
    longer = 0L;
  if( (ihand = Fopen(argv[3],0)) > 0 )
  {
    if( (ahand = Fopen(argv[2],0)) < 0 )
    {
      Cconws( "\033EArchive file must exist before running compress" );
      return(-1);
    }
    if( Fread(ahand,sizeof(prghdr),&prghdr) != sizeof(prghdr) || 
        prghdr.magic!=0x601a )
    {
      Cconws( "\033EArchive is not an executable" );
      return(-1);
    }
    is_exe=1;
    if( Fread( ihand, sizeof(iprghdr), &iprghdr ) != sizeof(iprghdr) ||
        iprghdr.magic!=0x601a || long_name ) is_exe=0;
    if( (ohand = Fcreate("COMPRESS.CMP",0)) < 0 ) return(ohand);
    Fwrite( ohand, sizeof(prghdr), &prghdr );
    copy_file( ahand, ohand, prghdr.text+prghdr.data );
    if( is_exe )
    {
      if( (ibuf = Malloc(ilen=Fseek(0L,ihand,2)-sizeof(iprghdr))) == 0 )
          return(-8);
    }
    else if( (ibuf = Malloc(ilen=Fseek(0L,ihand,2))) == 0 ) return(-8);
    len0 = ilen;  /*% &= 0xFFFFFFFEL; */
    printf( "\033EReading %s", argv[3] );
    Fseek( is_exe ? sizeof(iprghdr) : 0L, ihand, 0 );
    Fread( ihand, ilen, ibuf );
    if( prghdr.sym )
    {
      for( i=0; !i; )
      {
        Fread( ahand, sizeof(archdr), &archdr );
        totl = archdr.total;
        if( !archdr.name[0] ) totl += *((int *)archdr.name+1);
        if( !archdr.next )
        {
          archdr.next = Fseek(0L,ahand,1)+totl;
          i++;
        }
        Fwrite( ohand, sizeof(archdr), &archdr );
        copy_file( ahand, ohand, totl );
      }
      printf( "\nAdding to existing archive...scanning..." );
    }
    else printf( "\nNew archive...scanning..." );
/*%    iptr = ibuf;
    for( ind=0; ind<65536; ind++ )
      count[ind] = 0;
    while( ilen )
    {
      count[((unsigned int)*iptr<<8)|(unsigned char)*(iptr+1)]++;
      iptr+=2;
      ilen-=2;
    } */
  }
  else return(-33);
/*%  printf( "\nCalculating" );
  for( i=totl=0; i<128; )
  {
    max = 0;
    for( ind=1; ind<65536; ind++ )
      if( count[ind] > count[max] ) max = ind;
    if( !count[max] ) i=128;
    j = count[max];
    for( ind=max; ind<65536 && i<128; ind++ )
      if( count[ind] == j )
      {
        printf( "." );
        totl += count[ind];
        count[ind] = 0;
        table[i++] = ind;
        tabl_max = i;
      }
  }
  printf( "\nPotential savings: %ld", totl ); */
  name = argc>=5 && *argv[4]=='-' ? argv[4]+1 : argv[3];
  if( long_name )
  {
    *(int *)archdr.name = 0;
    *((int *)archdr.name+1) = name_len = strlen(name) + 1;
  }
  else
  {
    if( (ptr = strrchr(name,'\\')) == 0 ) ptr=name;
    else ptr++;
    for( i=0; i<12; i++, ptr++ )
      archdr.name[i] = *ptr>='a' ? *ptr&0x5F : *ptr;
    name_len = 0;
  }
  archdr.next = 0L;
  if( is_exe )
  {
    archdr.fixup = len0 - (archdr.text = iprghdr.text) -
        (archdr.data = iprghdr.data);
    archdr.bss = iprghdr.bss;
  }
  else
  {
    archdr.text = archdr.data = archdr.fixup = 0;
    archdr.bss = len0;
  }
  Fwrite( ohand, sizeof(archdr), &archdr );
  if( name_len ) Fwrite( ohand, name_len, name );
  longer += sizeof(archdr) + name_len;
  start = Fseek( 0L, ohand, 1 );
  printf( "\nPacking as %s...", is_exe ? "an executable" : "a text file" );
  ilen = len0;
  iptr = ibuf;
  obufptr = Malloc(len0);
  fpos=0;
/*%  while( ilen != -2 )
  {
    i = ((unsigned int)*iptr<<8)|(unsigned char)*(iptr+1);
    for( j=0; j<tabl_max; j++ )
      if( table[j]==i ) break;
    if( j<tabl_max )
    {
      if( llen )
      {
        if( lasttbl >= 0 && llen < 9 )
        {
          if( !obufptr ) Fseek( -1, ohand, 1 );
          fpos--;
          longer--;
          writb( ohand, -((lasttbl<<3)|(llen-1))-64 );
          writ( ohand, llen, iptr );
        }
        else
        {
          writb( ohand, -llen );
          writ( ohand, llen, iptr );
        }
        llen = 0;
      }
      lasttbl = j<8 ? j : -1;
      writb( ohand, j );
    }
    else if( llen == 63 || !ilen )
    {
      lasttbl = -1;
      writb( ohand, -llen );
      writ( ohand, llen, iptr );
      llen = 1;
    }
    else llen++;
    iptr+=2;
    ilen-=2;
  } **/
  Encode();
  if( longer&1 ) _putc(0);
  if( obufptr ) Fwrite( ohand, fpos, obufptr );
  copy_file( ahand, ohand, 0x8FFFFFFFL );
  prghdr.sym += longer;
  Fseek( 0L, ohand, 0 );
  Fwrite( ohand, sizeof(prghdr), &prghdr );
  Fseek( start-sizeof(archdr)-name_len, ohand, 0 );
  archdr.total = fpos;
  Fwrite( ohand, sizeof(archdr), &archdr );
  if( name_len ) Fwrite( ohand, name_len, name );
  printf( "\nActual savings: %ld out of %ld (%ld%%)\nComparing\n", 
      len0-fpos, len0, (len0-fpos)*100L/len0 );
  if( fpos > len0 )
  {
    printf( "It got bigger!!!" );
    exit(-1);
  }
  if( (bbuf = (char *)Malloc(fpos)) == 0 ) return(-8);
  optr = ibuf;
  ipos = fpos;
  Fread( ohand, ilen=ipos0=fpos, iptr=bbuf );
  fpos = 0L;
  timer = Supexec(get_timer);
  Decode();
  printf( "Comparison OK:  %ld tics used", Supexec(get_timer)-timer );
  Fclose(ohand);
  Fclose(ahand);
  if( (ipos=Fdelete(argv[2])) == 0 ) ipos = Frename( 0, "COMPRESS.CMP",
      argv[2] );
    j = argc>=5 && *argv[4]=='-' ? 2 : 1;
    argc -= j;
    for( i=3; i<argc; i++ )
      argv[i] = argv[i+j];
  }	/* end of outer while */
  return 0;
}

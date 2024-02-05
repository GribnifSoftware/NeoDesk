#include <tos.h>
#include <string.h>
#include "aes.h"
#include "neodesk.h"
#include "neocommn.h"
#include "neod1_id.h"

#ifdef DEBUG_ON
/*%  #define MEM_DEBUG */
#endif

#define TOS_ver ((*(SYSHDR **)0x4f2)->os_version)

#define VOID_STAR void *

#define NULL ((char *)0)

/*
 * block header: every memory block has one.
 * A block is either allocated or on the free
 * list of the arena.  There is no allocated list: it's not necessary.
 * The next pointer is only valid for free blocks: for allocated blocks
 * maybe it should hold a verification value..?
 *
 * Zero-length blocks are possilbe free; they hold space which might
 * get coalesced usefully later on.
 */

struct block {
    struct block *b_next;   /* NULL for last guy; next alloc or next free */
    long b_size;
};

/*
 * arena header: every arena has one.  Each arena is always completely
 * filled with blocks; the first starts right after this header.
 */

struct arena {
    struct arena *a_next;
    struct block *a_ffirst;
    long a_size;
};

struct used_bl {
    struct used_bl *next;
    long size;
    int id;
} *last_used;

/*
 * Arena linked-list pointer, and block size.  The block size is initialized
 * to Malloc(-1L)/20 when you start up, because somebody said that 20
 * Malloc blocks per process was a reasonable maximum.  This is hopelessly
 * unbalanced: 25K on a 520ST and 200K on a Mega 4, so it's tempered by
 * the constant MAXARENA as the largest minimum you'll get (I chose 100K).
 */

struct arena *a_first = (struct arena *)NULL;
static long minarena = -1L, allocated=0L, in_use=0L;
extern MOST z;

#define MAXARENA (100L*1024L)
#define MINARENA (1024L*8)

long get_ver(void)
{
  return TOS_ver;
}

#ifdef MEM_DEBUG

static int chksum;
static int mchksm( void *ptr, int size )
{
  int sum=0;

  size>>=1;
  while( --size >= 0 )
    sum = ((sum+1)<<1) + *((int *)ptr)++;
  return sum;
}

static int mem_chk(void)
{
  struct arena *a;
  register struct block *b;
  int sum=0;

  for (a = a_first; a; a = a->a_next)
  {
    sum += mchksm( a, sizeof(struct arena) );
    for (b = a->a_ffirst; b; b = b->b_next)
      sum += mchksm( b, sizeof(struct block) );
  }
  return sum;
}

void breakpoint(void) 0x4AFC;
void f_alert( char *str )
{
  if( form_alert( 1, str )==2 ) breakpoint();
}

void check_size( struct block *b )
{
  if( b->b_size<=0 )
  {
    f_alert( "[1][Bad b_size!][OK]" );
    breakpoint();
  }
}
#endif

long temp_take_always, temp_limit_to;
struct arena *alloc_mas;

void init_always(void)		/* 004: grab always block before exe loads */
{
  temp_take_always = z.take_always;
  temp_limit_to = z.limit_to;
  /* Allocate the first block and keep it allocated for future ALLOC_MAS use */
  lalloc( 1L, -1, 0 );
}

void *malloc_small( long size )	/* 004 */
{
  void *p=0L, *n;
  long l;

  for(;;)
  {
    l = (long)Malloc(-1L);
    if( l<size || (n = Malloc(l)) == 0 ) break;
    *(void **)n = p;
    p = n;
  }
  if( !p ) return 0L;
  n = *(void **)p;
  Mshrink( 0, p, size );
  if( !n ) return p;
  do
  {
    l = *(long *)n;
    Mfree(n);
  } while( (n = (void *)l) != 0 );
  return p;
}

void *lalloc( long size, int id, int alert )
{
    struct arena *a, *a2;
    register struct block *b, *mb, **q;
    register long temp, temp2;

    if( size==-1L )
    {
      /* find largest */
      for( size=0L, a = a_first; a; a = a->a_next )
        if( a!=alloc_mas || id==ALLOC_MAS )	/* 004 */
          for( b = a->a_ffirst; b; b = b->b_next )
            if( b->b_size > size ) size = b->b_size;
      if( (size = (size - sizeof(struct used_bl) - 3) & ~3) < 0 ) size = 0;
      switch( z.mem_mode )
      {
        case MEM_LIMIT:
          /* maybe unallocated area is larger */
          if( (temp = (z.limit_to - allocated -sizeof(struct arena)-sizeof(struct block) - 3L) & ~3L)
              > size ) return (void *)temp;
          return (void *)size;
        case MEM_ALWAYS:
          return (void *)size;
        case MEM_NONE:
          if( (temp = ((long)Malloc(-1L) - sizeof(struct arena)
              - sizeof(struct block) - sizeof(struct used_bl) - 3L)&~3) > size )
              return (void *)temp;
          return (void *)size;
      }
    }
    /* make sure size is a multiple of four; fail if it's negative */
    if (size <= 0)
    {
#ifdef MEM_DEBUG
      f_alert( "[1][lalloc: bad size][Ok|Break]" );
#endif
      return 0;
    }
    size = (size+3+sizeof(struct used_bl)) & ~3;

#ifdef MEM_DEBUG
    if( chksum != mem_chk() )
    {
      f_alert( "[1][lalloc: bad checksum][Ok|Break]" );
      chksum = mem_chk();
      return 0;
    }
#endif
    for (a = a_first; a; a = a->a_next)
      if( a!=alloc_mas || id==ALLOC_MAS ) {	/* 004 */
        for (b = *(q = &a->a_ffirst); b; b = *(q = &b->b_next)) {
            /* if big enough, use it */
            if (b->b_size >= size) {

                /* got one */
                mb = b;

                /* cut the free block into an allocated part & a free part */
                temp = mb->b_size - size - sizeof(struct block);
                if (temp >= (signed long)sizeof(struct used_bl)) {
                    /* large enough to cut */
                    b = (struct block *)(((char *)(b+1)) + size);
                    b->b_size = temp;
                    b->b_next = mb->b_next;
                    *q = b;
                    mb->b_size = size;
                }
                else {
                    /* not big enough to cut: unlink this from free list */
                    *q = mb->b_next;
                    /* take the whole thing */
                    size = mb->b_size + sizeof(struct block);
                }
                mb++;
                ((struct used_bl *)mb)->next = last_used;
                ((struct used_bl *)mb)->size = size;
                ((struct used_bl *)mb)->id = id;
                last_used = (struct used_bl *)mb;
                in_use += size;
#ifdef MEM_DEBUG
		chksum = mem_chk();
#endif
                return (void *)((char *)mb+sizeof(struct used_bl));
            }
        }
    }

    /* no block available: get a new arena */

    if (minarena<0)
    {
      if( (int)Supexec(get_ver) > 0x102 ) minarena = MINARENA;
      else minarena = (long)Malloc(-1L) / 20;
      if (minarena > MAXARENA) minarena = MAXARENA;
      switch( z.mem_mode )
      {
        case MEM_ALWAYS:
          if( (temp=size) + sizeof(struct arena) + sizeof(struct block)
              < z.take_always ) temp = z.take_always - sizeof(struct arena)
              - sizeof(struct block);
          goto skip;
        case MEM_LIMIT:
          temp = size;
          goto skip;
      }
    }

    if (size < minarena) {
        temp = minarena;
    }
    else {
        temp = size;
    }

skip:
    temp2 = temp + sizeof(struct arena) + sizeof(struct block);
    switch( z.mem_mode )
    {
      case MEM_ALWAYS:
        if( temp2+allocated > temp_take_always )
        {
	  if( f_alert1( msg_ptr[35] ) != 1 ) return 0L;
	  temp_take_always = temp2+allocated;	/* 004 */
	}
        break;
      case MEM_LIMIT:
        if( temp2+allocated > temp_limit_to )
        {
	  if( f_alert1( msg_ptr[35] ) != 1 ) return 0L;
	  temp_limit_to = temp2+allocated;	/* 004 */
        }
    }

    a = (struct arena *)malloc_small(temp2);

    /* if Malloc failed return failure */
    if (a == 0) {
        if( alert ) f_alert1( msg_ptr[36] );
        return 0;
    }

    allocated += temp2;
    a->a_size = temp + sizeof(struct block);
    /* 002: new blocks are now added to the end of the list */
    /* was: a->a_next = a_first; a_first = a; */
    a->a_next = 0L;
    if( (a2=a_first) != 0 )
    {
      for( ; a2->a_next; a2 = a2->a_next);
      a2->a_next = a;
    }
    else
    {
      a_first = a;
      if( z.mem_mode!=MEM_ALWAYS ) alloc_mas = a;	/* 004 */
    }
    mb = (struct block *)(a+1);
    mb->b_next = 0;
    mb->b_size = size;
    in_use += temp;

    if (temp > (size + sizeof(struct block))) {
        b = a->a_ffirst = (struct block *)(((char *)(mb+1)) + size);
        b->b_next = 0;
        b->b_size = temp - size - sizeof(struct block);
        in_use -= temp - size;
#ifdef MEM_DEBUG
        check_size(b);
#endif
    }
    else {
        a->a_ffirst = 0;
    }

    mb++;
    ((struct used_bl *)mb)->next = last_used;
    ((struct used_bl *)mb)->size = size;
    ((struct used_bl *)mb)->id = id;
    last_used = (struct used_bl *)mb;
#ifdef MEM_DEBUG
    chksum = mem_chk();
#endif
    return (void *)((char *)mb+sizeof(struct used_bl));
}

void shrink( struct arena *a, struct block *fb, struct block *pb )
{
  long new;
  
  /* if size was above minarena, shrink it if possible */
  if( z.mem_mode != MEM_ALWAYS && a->a_size-sizeof(struct block) >
      minarena && !fb->b_next )
  {
    new = a->a_size-fb->b_size+sizeof(struct block);
    if( new < minarena )
    {
      new = minarena;
      fb->b_size = (long)(a+1)+new - (long)(fb+1);
#ifdef MEM_DEBUG
      check_size(fb);
#endif
    }
    else if( !pb ) a->a_ffirst = 0L;
    else pb->b_next = 0L;
    allocated -= a->a_size-new;
    Mshrink( 0, a, (a->a_size=new)+sizeof(struct arena) );
  }
}

int lfree( VOID_STAR xfb )
{
    struct arena *a, **qa;
    struct used_bl *u, *u2;
    register struct block *b;
    register struct block *pb;
    register struct block *fb = (struct block *)((char *)xfb-sizeof(struct used_bl));

    /* set fb (and b) to header start */
    b = --fb;

#ifdef MEM_DEBUG
    if( chksum != mem_chk() )
    {
      f_alert( "[1][lfree: bad checksum][Ok|Break]" );
      chksum = mem_chk();
      return 0;
    }
#endif

    /* the arena this block lives in */
    for (a = *(qa = &a_first); a; a = *(qa = &a->a_next)) {
        if ((long)b > (long)a && (long)b < ((long)a + a->a_size)) goto found;
    }
#ifdef MEM_DEBUG
    f_alert( "[1][lfree: no arena][Ok|Break]" );
#endif
    return -1;

found:
    /* Found it! */
    /* a is this block's arena */

    /* remove from used list */
    for( u=last_used, u2=0L; u; u2=u, u=u->next )
      if( (char *)(u+1) == (char *)xfb )
      {
        if( u2 ) u2->next = u->next;
        else last_used = u->next;
        in_use -= u->size;
        break;
      }
    if( !u )
    {
#ifdef MEM_DEBUG
      f_alert( "[1][lfree: can\'t find link][Ok|Break]" );
#endif
      return -1;
    }

    /* set pb to the previous free block in this arena, b to next */
    for (pb = 0, b = a->a_ffirst;
         b && (b < fb);
         pb = b, b = b->b_next)
      if( b==pb )
      {
#ifdef MEM_DEBUG
        f_alert( "[1][lfree: bad link][Ok|Break]" );
#endif
        return -1;
      }

    fb->b_next = b;

    /* Coalesce backwards: if any prev ... */
    if (pb) {
        /* if it's adjacent ... */
        if ((((long)(pb+1)) + pb->b_size) == (long)fb) {
            pb->b_size += sizeof(struct block) + fb->b_size;
#ifdef MEM_DEBUG
            check_size(pb);
#endif
            fb = pb;
        }
        else {
            /* ... else not adjacent, but there is a prev free block */
            /* so set its next ptr to fb */
            pb->b_next = fb;
        }
    }
    else {
        /* ... else no prev free block: set arena's free list ptr to fb */
        a->a_ffirst = fb;
    }

    /* Coalesce forwards: b holds start of free block AFTER fb, if any */
    if (b && (((long)(fb+1)) + fb->b_size) == (long)b) {
        fb->b_size += sizeof(struct block) + b->b_size;
        fb->b_next = b->b_next;
#ifdef MEM_DEBUG
        check_size(fb);
#endif
    }

    /* if, after coalescing, this arena is entirely free, Mfree it! */
    if ( (z.mem_mode!=MEM_ALWAYS || allocated > z.take_always) &&
        (long)(a->a_ffirst) == (long)(a+1) &&
        (a->a_ffirst->b_size + sizeof(struct block)) == a->a_size) {
            allocated -= a->a_size+sizeof(struct arena);
            *qa = a->a_next;
            (void)Mfree(a);
    }
    else shrink( a, fb, pb );
    
#ifdef MEM_DEBUG
    chksum = mem_chk();
#endif
    return 0;   /* success! */
}

int lshrink( VOID_STAR xfb, long new )
{
    struct arena *a;
    struct used_bl *u;
    register struct block *b, *nb, *pb;
    register struct block *fb = (struct block *)((char *)xfb-sizeof(struct used_bl));
    long delta;

    /* set fb (and b) to header start */
    b = --fb;

#ifdef MEM_DEBUG
    if( chksum != mem_chk() )
    {
      f_alert( "[1][lshrink: bad checksum][Ok|Break]" );
      chksum = mem_chk();
      return -1;
    }
#endif

    if( new<=0L ) return -1;	/* bad new size */

    /* the arena this block lives in */
    for (a = a_first; a; a = a->a_next) {
        if ((long)b > (long)a && (long)b < ((long)a + a->a_size)) goto found;
    }
#ifdef MEM_DEBUG
    f_alert( "[1][lshrink: no arena][Ok|Break]" );
#endif
    return -1;

found:
    /* Found it! */
    /* a is this block's arena */

    new = (new + sizeof(struct used_bl) + 3) & ~3;
    /* shrink in used list */
    for( u=last_used; u; u=u->next )
      if( (char *)(u+1) == (char *)xfb )
        if( (delta = u->size - new) < 0 )
        {
#ifdef MEM_DEBUG
          f_alert( "[1][lshrink: new > old][Ok|Break]" );
#endif
          return -1;
        }
        else
        {
          in_use -= delta;
          u->size = new;
          if( delta <= sizeof(struct used_bl) )
              goto ret0;		/* not enough savings to be worth it */
          break;
        }
    if( !u )
    {
#ifdef MEM_DEBUG
      f_alert( "[1][lshrink: can\'t find link][Ok|Break]" );
#endif
      return -1;
    }

    /* set pb to the previous free block in this arena, b to next */
    for (pb = 0, b = a->a_ffirst;
         b && (b < fb);
         pb = b, b = b->b_next)
      if( b==pb )
      {
#ifdef MEM_DEBUG
        f_alert( "[1][lshrink: bad link][Ok|Break]" );
#endif
        return -1;
      }
      
    fb->b_size = new;

    /* create a new free block */
    fb = (struct block *)((long)(fb+1) + new);
    fb->b_next = b;
    fb->b_size = delta - sizeof(struct block);
#ifdef MEM_DEBUG
    check_size(fb);
#endif
    
    if( !pb ) {
        /* no prev free block: set arena's free list ptr to fb */
        a->a_ffirst = fb;
    }

    /* Coalesce forwards: b holds start of free block AFTER fb, if any */
    if (b && (((long)(fb+1)) + fb->b_size) == (long)b) {
        fb->b_size += sizeof(struct block) + b->b_size;
        fb->b_next = b->b_next;
#ifdef MEM_DEBUG
        check_size(fb);
#endif
    }

    shrink( a, fb, pb );

ret0:
#ifdef MEM_DEBUG
    chksum = mem_chk();
#endif
    return 0;   /* success! */
}

int lfreeall( int id )
{
  struct used_bl *u, *bl;

  for( u=last_used; u; )
    if( u->id == id )
    {
      bl = u;
      u = u->next;
      if( lfree((char *)bl+sizeof(struct used_bl)) ) return 0;
    }
    else u = u->next;
  return 1;
}

int lrealloc( void **xfb, long size, int alert )
{
    struct arena *a;
    struct used_bl *u;
    register struct block *b;
    register struct block *pb, *mb;
    register struct block *fb = (struct block *)((char *)*xfb-sizeof(struct used_bl));
    long new, temp;
    void *newb;

    /* set fb (and b) to header start */
    b = --fb;

#ifdef MEM_DEBUG
    if( chksum != mem_chk() )
    {
      f_alert( "[1][lrealloc: bad checksum][Ok|Break]" );
      chksum = mem_chk();
      return -1;
    }
#endif

    /* the arena this block lives in */
    for (a = a_first; a; a = a->a_next) {
        if ((long)b > (long)a && (long)b < ((long)a + a->a_size)) goto found;
    }
#ifdef MEM_DEBUG
    f_alert( "[1][lrealloc: no arena][Ok|Break]" );
#endif
    return -1;

found:
    /* Found it! */
    /* a is this block's arena */

    new = (size + sizeof(struct used_bl) + 3) & ~3;
    /* find in used list */
    for( u=last_used; u; u=u->next )
      if( (char *)(u+1) == (char *)*xfb )
        if( u->size==new ) return 0;	/* no change */
        else if( u->size > new )	/* shrink it */
        {
          lshrink( *xfb, size );
          return 0;
        }
        else break;
    if( !u )
    {
#ifdef MEM_DEBUG
      f_alert( "[1][lrealloc: can\'t find link][Ok|Break]" );
#endif
      return -1;
    }

    /* set pb to the previous free block in this arena, b to next */
    for (pb = 0, b = a->a_ffirst;
         b && (b < fb);
         pb = b, b = b->b_next)
      if( b==pb )
      {
#ifdef MEM_DEBUG
        f_alert( "[1][lrealloc: bad link][Ok|Break]" );
#endif
        return -1;
      }

    /* expand forward, if contiguous and large enough */
    if( b && (long)u+u->size == (long)b &&
        (temp=b->b_size+sizeof(struct block)+u->size) >= new )
    {
      if( temp-new > sizeof(struct used_bl) )
      {
        mb = (struct block *)((long)u + new);
        mb->b_next = b->b_next;	/* destroying b, so skip it */
        mb->b_size = temp-new-sizeof(struct block);
#ifdef MEM_DEBUG
        check_size(mb);
#endif
        in_use += new-u->size;
        u->size = fb->b_size = new;
      }
      else
      {
        in_use += temp-u->size;
        u->size = fb->b_size = temp;
        mb = b->b_next;
      }
      if( pb ) pb->b_next = mb;
      else a->a_ffirst = mb;	/* first free in arena */
#ifdef MEM_DEBUG
      chksum = mem_chk();
#endif
      return 0;
    }
    if( (newb = lalloc( size, u->id, alert )) == 0 ) return -1;
    memcpy( newb, *xfb, u->size-sizeof(struct used_bl) );
    lfree(*xfb);
    *xfb = newb;

#ifdef MEM_DEBUG
    chksum = mem_chk();
#endif
    return 0;   /* success! */
}

void memstat( long *_in_use, long *_allocated )
{
  *_in_use = in_use;
  *_allocated = allocated;
}

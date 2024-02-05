void init_always(void);
void *malloc_small( long size );
void *lalloc( long size, int id, int alert );
int lfree( void *xfb );
int lfreeall( int id );
void memstat( long *in_use, long *allocated );
int lshrink( void *block, long newsiz );
int lrealloc( void **xfb, long size, int alert );
long get_ver(void);
int f_alert1( char *msg );

extern char *msg_ptr[];

void magx_single( int * );
void magx_unsingle( int *, BASPAG * );

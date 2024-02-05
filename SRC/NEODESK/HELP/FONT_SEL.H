static int max_ht, char_w, char_h, total_fonts;
static char has_gdos;
static int work_out[57];
static OBJECT *form;

static void rsrc_adr( int type, int obj, void **tree );
void cur_point(void);
void init_font( int *id, int *point );
int make_form( int tree, int num, char *s, void func( OBJECT *o, int obj ) );
void no_fonts(void);
void load_fonts(void);
int font_sel( OBJECT *o, int sys, int *out_id, int *out_pt );
void set_if( int num, int true );
int hide_if( OBJECT *tree, int truth, int idx );
static char *_lalloc( long len );
static void _lfree( void *p );
static int _lrealloc( void *p, long size );
static void _spf(char *buf, char *fmt, ...);

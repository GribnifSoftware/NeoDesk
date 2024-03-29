int redirect( char *str, int *hand, 
    long (*dosf)(const char *p, int m), char *msg, int md );
void echo_nl(void);
int mouse_vex( int flag );
int first( char *ptr, char *msg );
int next(void);
void get_path( char *ptr );
int is_dir( char *p );
void getPATH( char **ptr );
int getstr( char *string, int len );
void get_env( char **ptr, char *var );
int hasch(void);
void ack(void);
void intswap( void *ptr1, void *ptr2, int num );
void prompt(void);
void backup(void);
void backsp(void);
void cursor(void);
int gtext( char *string );
int write_redir( char *string );
void newline(void);
void resize( Rect *rect, int prmpt );
void redraw(int prmpt);
int is_sep( int ch );
void parse(void);
int insert_var( char *dollar, char **ptr, char **var_end, int func );
int is_digit( int ch, int hex );
void to_upper( char *ptr );
void set_path(void);
char *htext( int mode, int i );
void help_text( int mode, int lng );
void append( char *p );
void copy( int mode );
int c_or_l( char *ptr, long *l );
int c_to_l( char *ptr, long *l );
void for_jump(void);
int find_next( char *ptr, int loop );
int is_found( char *pm, int l, int *err );
int strxcmp( char *a, char *b, int l );
int iftest(void);
void errs( int num, char *ptr );
int countif(void);
void to_date( unsigned int date );
void to_time( unsigned int time );
void add_menu(void);
int bconin2(void);
void rmatch( char *str, char *pat, char *out );
void uppercase( char *ptr );
void get_mvar_val( VAR_STRUCT *vs, char *buf );
void dispvar( char *name, char *val, int flag );
void make_var( char *name, char *val );
void bad_var( char *name );
void var_not_found(void);
int find_var( char *name, char **val );       /* USES path AS A BUFFER!! */
void jump_to_line( int line, long pos );
void dmabuf(int flag);
int logical( char *ptr );
func_test( int bit );
int is_num( int i, long *a, long *b, int flg );
int is_num2( char *ptr, long *num );
int is_num3( char *ptr, long *num );
void truth(int n);
void open_it(void);
void top_it(void);
void write_text(void);
void close_text(void);
int open_batch( char *ptr );
void batch(void);
void close_batch(void);
void reset_mouse(void);
int top_wind(void);
void get_bp(void);
int read_key( int key );
int is_ctrlc(void);
long bconinr( void );
long bconin( int hand );
void end_in(void);
char *add_env( char *ptr, char *add );
void remove_env( char *ptr2 );
int find_env( char **ptr2 );
char *variables( char *ptr, int *done, char **p3, int flag );
int ck_batch(void);
void zero_cmd(void);
void echo( char *s );
int varredir(int in);


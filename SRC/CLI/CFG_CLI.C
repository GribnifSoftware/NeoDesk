#include "tos.h"
#include "linea.h"
#include "string.h"
#include "stdio.h"
#include "aes.h"

#define MAGIC   0x4510
#define VERSION 0x202
#define VAR_LEN	80
#define VARNAMLEN 8

#define NUM_VARS  6

char path[120];
long power[7] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };

struct
{
  int version;
  int num_vars;
  unsigned int backup_size;
  int rs232_buf;
  unsigned int batch_buf;
  long screen_size;
  unsigned int alias_size;
  char *buf;
  long bufsiz, real_bufsiz;
} Config;

struct
{
  unsigned int *var;
  int is_long;
  int scale;
  long min, max;
  char *name;
} var[NUM_VARS] = {
{ (unsigned int *)&Config.screen_size, 1, 1, 0, 4000000L,
    "Bytes for scroll-back (Help key):" },
{ (unsigned int *)&Config.backup_size, 0, 1, 512, 65535,
    "Bytes for command history (Shift-):" },
{ (unsigned int *)&Config.alias_size, 0, 1, 0, 65535,
    "Bytes for all aliases:" },
{ (unsigned int *)&Config.num_vars, 0, VAR_LEN+VARNAMLEN+4, 0, 2000,
    "Number of user-defined variables:" },
{ (unsigned int *)&Config.batch_buf, 0, 1, 256, 65535,
    "Bytes for batch file buffer:" },
{ (unsigned int *)&Config.rs232_buf, 0, 1, 0, 32766,
    "Bytes for RS-232 buffer:" } };

int nvar, digit;

void bconws( char *str )
{
  while( *str ) Bconout( 2, *str++ );
}

void bconwsr( char *str )
{
  while( *str ) Bconout( 5, *str++ );
}

void total(void)
{
  char buf[15];

  sprintf(buf, "\033f\033Y\051\104%7ld", Config.bufsiz );
  bconws(buf);
}

long get_val( int i )
{
  return( var[i].is_long ? *(long *)var[i].var : (long)*var[i].var );
}

void var_val( int i )
{
  char buf[15];

  sprintf(buf, "\033f\033Y%c\104%7ld", '\040'+2+i, get_val(i) );
  bconws(buf);
}

void set_val( long l )
{
  if( l >= var[nvar].min && l <= var[nvar].max )
  {
    Config.bufsiz -= get_val(nvar)*var[nvar].scale;
    if( var[nvar].is_long ) *(long *)var[nvar].var = l;
    else *var[nvar].var = l;
    Config.bufsiz += get_val(nvar)*var[nvar].scale;
    var_val(nvar);
    total();
  }
  else Bconout( 2, 7 );
}

void cursor(void)
{
  char *buf="\033Yxx\033e";
  
  buf[2] = '\040'+2+nvar;
  buf[3] = '\104'+(6-digit);
  bconws(buf);
}

char *pathend( char *str )
{
  char *ptr;
  
  return( (ptr=strrchr(str,'\\')) != 0 ? ptr+1 : str );
}

int select(void)
{
  int buttn, i;
  char temp[120], name[13];
  
  strcpy( name, pathend(path) );
  strcpy( temp, path );
  strcpy( pathend(temp), "NEO_CLI.*" );
  bconws( "\033f\033EWhere is NEO_CLI.ACC or NEO_CLI.NPG?" );
  show_mouse(1);
  i = fsel_input( temp, name, &buttn );
  hide_mouse();
  if(i)
    if( buttn==1 )
    {
      strcpy( pathend(temp), name );
      strcpy( path, temp );
      return(1);
    }
  return(0);
}

int main( int argc, char *argv[] )
{
  int hand, dum, i;
  char done=0, buf[50];
  long l;
  
  linea_init();
  hide_mouse();
  appl_init();
  graf_mouse( ARROW, 0L );
  if( argc > 1 ) strcpy( path, argv[1] );
  else
  {
    path[0] = Dgetdrv()+'A';
    path[1] = ':';
    Dgetpath( path+2, 0 );
    strcat( path, "\\" );
    if( !select() ) done++;
  }
  while( !done )
  {
    if( (hand=Fopen(path,0)) < 0 ) form_alert( 1,
        "[1][Could not open the|file in write mode!][Ok]" );
    else if( Fseek( 0x2c, hand, 0 ) != 0x2c || Fread( hand, 2L, &dum ) !=
        2L || Fread( hand, sizeof(Config), &Config ) != sizeof(Config) )
        form_alert( 1, "[1][Error reading file!][Ok]" );
    else if( dum != MAGIC || Config.version != VERSION ) form_alert( 1,
        "[1][This version of CFG_CLI|is not compatible with|\
the NEO_CLI you chose][Ok]" );
    else
    {
      bconws( "\033f\033E\033p\
               NeoDesk CLI Configuration Options:               \033q\r\n\r\n" );
      for( i=0; i<NUM_VARS; i++ )
      {
        bconwsr( var[i].name );
        var_val(i);
        sprintf( buf, "\033Y%c\115(from %ld to %ld)",
            '\040'+2+i, var[i].min, var[i].max );
        bconws(buf);
        bconws( "\r\n" );
      }
      bconws( "----------------------------------  -------\r\n\
Total Bytes\r\n\r\n\033p\
                Arrow keys select digit to change               \r\n\
        + and - increase/decrease    0-9 change directly        \r\n\
                  'S' Save and quit    'Q' Quit                 \033q" );
      total();
      nvar=digit=0;
      while(!done)
      {
        cursor();
        i = ((l = Bconin(2)) >> 16) & 0xFF;
        switch( (char)l )
        {
          case 0:
            switch(i)
            {
              case '\x4d':	/* right */
                if( digit ) digit--;
                break;
              case '\x4b':	/* left */
                if( digit<6 ) digit++;
                break;
              case '\x48':	/* up */
                if( nvar ) nvar--;
                break;
              case '\x50':	/* down */
                if( nvar < NUM_VARS-1 ) nvar++;
                break;
            }
            break;
          case 'S':
          case 's':
            show_mouse(1);
            if( Fseek( 0x2e, hand, 0 ) != 0x2e || Fwrite( hand,
                sizeof(Config), &Config ) != sizeof(Config) )
            {
              form_alert( 1, "[1][Error writing|new configuration!][Ok]" );
              hide_mouse();
              break;
            }
            hide_mouse();
          case 'q':
          case 'Q':
            done++;
            break;
          case '+':
            if( ((l=get_val(nvar))/power[digit]%10) < '9' )
                set_val( l+power[digit] );
            break;
          case '-':
            if( (l=get_val(nvar))/power[digit] ) set_val( l-power[digit] );
            break;
          default:
            i = (char)l;
            if( i >= '0' && i <= '9' )
            {
              l = get_val(nvar);
              set_val( l+(i-'0'-(l/power[digit]%10))*power[digit] );
              if( digit ) digit--;
            }
        }
      }
    }
    if( hand>0 ) Fclose(hand);
    if( !done )
      if( !select() ) done++;
  }
  bconws( "\033E\033f" );
  show_mouse(1);
  return(0);
}

#define NPI_VER         0x0400
#define NPI_MAGIC       0x2E4E5049L     /* ".NPI" */

typedef union
{
  unsigned int i;
  struct
  {
    unsigned pexec_mode:8;      /* also holds icon type in "file" list */
    unsigned set_me:1;
    unsigned batch:1;           /* reserved in NEO_ACC.H */
    unsigned return_status:1;
    unsigned show_status:1;
    unsigned clear_screen:1;
    unsigned npg:1;
    unsigned tos:1;
    unsigned takes_params:1;
  }p;
} PROG_TYPE;

typedef union
{
  unsigned int i;
  struct
  {
    unsigned docpath:1;
    unsigned reload:2;
    unsigned clock:1;
    unsigned MSTe_16M:1;
    unsigned MSTe_cache:1;
    unsigned TT_cache:1;
    unsigned Blitter:1;
    unsigned reserved:1;        /* must be 0 for set_caches to work */
    unsigned CaTT:3;
    unsigned singletask:1;
    unsigned unused:3;
  } p;
} APP_FLAGS;

typedef struct
{
  long magic;
  int ver;
  char path[120],
       name[21],
       params[5][39],
       use_argv;
  APP_FLAGS af;
  PROG_TYPE pt;
  char env[621], use_parent;
} NPI_TYPE;

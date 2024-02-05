#include "tos.h"
#include "aes.h"
#include "..\neocommn.h"

#define COUNTRIES 1

typedef struct
{
  char lang;
  char *msg;
} SMSG;

/* English must be first */
SMSG msgs[][COUNTRIES] = {
{ 0, "\r\n      SPOOLxxx by Dan Wilga\r\n\
Copyright 1994, Gribnif Software\r\n" },
{ 0, "\aSPOOLxxx was not found. Please make\r\n\
sure the file's name is correct.\r\n" },
{ 0, "\aBad number of spool Kb requested\r\n" },
{ 0, "\aSPOOLxxx must run \033pAFTER\033q NEOLOAD.PRG\r\n" },
{ 0, "\aSPOOLxxx needs NEOLOAD.PRG\
version 4.0 or newer\r\n" }
};

extern char country;
extern LoadCookie *cookie;
char *bufptr;

void message( int num )
{
  int i = COUNTRIES;
  
  while( --i>0 )
    if( msgs[num][i].lang == country ) break;
  Cconws( msgs[num][i].msg );
}

long install( char *buf, unsigned int kb )
{
  long size;
  LoadCookie *lc = cookie;
  
  if( cookie->pr_valid != PR_VALID ) return 0L;
  size = kb*1024L;
  *lc->pr_count = 0L;
  *lc->pr_bufmax = (*lc->pr_bufstart=*lc->pr_buftail=buf) +
      (*lc->pr_bufsiz = size);
  bufptr = buf;
  *lc->bufptr = &bufptr;
  return size;
}

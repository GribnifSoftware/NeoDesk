#include "tos.h"
#include "aes.h"
#include "vdi.h"
#include "..\neocommn.h"
#include "win_var.h"
#include "win_inc.h"
#include "string.h"
#include "xwind.h"

static void fixx( int *i )
{
  *i = ((*i&0xFF)*char_w) + (*i>>8);
}

static void fixy( int *i )
{
  *i = ((*i&0xFF)*char_h) + (*i>>8);
}

static int obfix( OBJECT *tree, int ind )
{
  int *i = &tree[ind].ob_x;

  fixx( i++ );
  fixy( i++ );
  fixx( i++ );
  fixy( i );
  return(1);
}

/*
void map_tree( OBJECT *tree, int this, int last, int func( OBJECT *tree, int tmp ) )
{
  int tmp1;

  tmp1 = 0;
  while (this != last && this != -1)
    if (tree[this].ob_tail != tmp1)
    {
      tmp1 = this;
      this = -1;
      if( (*func)( tree, tmp1 ) ) this = tree[tmp1].ob_head;
      if (this == -1) this = tree[tmp1].ob_next;
    }
    else
    {
      tmp1 = this;
      this = tree[tmp1].ob_next;
    }
}
*/

static long add;

void rsc_incr( long *loc )
{
  *loc += add;
}

int long_rsc( RSHDR *rsc )
{
  return ((RSHDR2 *)rsc)->rsh_extvrsn == X_LONGRSC;
}

void fix_rsc( RSHDR *rshdr, char *rsc )
{
  unsigned i, j, siz;
  char *ptr, is_new;
  RSHDR2 *rshdr2;

  if( (is_new = long_rsc(rshdr)) != 0 ) siz = sizeof(RSHDR2);
  else siz = sizeof(RSHDR);
  rshdr2 = ((RSHDR2 *)rshdr);
  for( ptr=(is_new?rshdr2->rsh_object:rshdr->rsh_object)+rsc-siz,
      i=(is_new?rshdr2->rsh_nobs:rshdr->rsh_nobs)+1; --i>0; ptr+=sizeof(OBJECT) )
  {
    j=((OBJECT *)ptr)->ob_type&0xff;
/**    if( (j=((OBJECT *)ptr)->ob_type&0xff) == G_CICON ) ((OBJECT *)ptr)->ob_spec.index =
        num_cic && ((OBJECT *)ptr)->ob_spec.index<num_cic ?
        *(cicon_ptr+((OBJECT *)ptr)->ob_spec.index) : 0L;
    else**/ if( j != G_CICON && j != G_BOXCHAR && j != G_IBOX && j != G_BOX ) rsc_incr(
        (long *)&((OBJECT *)ptr)->ob_spec.free_string );
    obfix( (OBJECT *)ptr, 0 );
  }
  for( ptr=(is_new?rshdr2->rsh_tedinfo:rshdr->rsh_tedinfo)+rsc-siz,
      i=(is_new?rshdr2->rsh_nted:rshdr->rsh_nted)+1; --i>0; ptr+=sizeof(TEDINFO) )
  {
    rsc_incr( (long *)&((TEDINFO *)ptr)->te_ptext );
    rsc_incr( (long *)&((TEDINFO *)ptr)->te_ptmplt );
    rsc_incr( (long *)&((TEDINFO *)ptr)->te_pvalid );
  }
  for( ptr=(is_new?rshdr2->rsh_iconblk:rshdr->rsh_iconblk)+rsc-siz,
      i=(is_new?rshdr2->rsh_nib:rshdr->rsh_nib)+1; --i>0; ptr+=sizeof(ICONBLK) )
  {
    rsc_incr( (long *)&((ICONBLK *)ptr)->ib_pmask );
    rsc_incr( (long *)&((ICONBLK *)ptr)->ib_pdata );
    rsc_incr( (long *)&((ICONBLK *)ptr)->ib_ptext );
  }
  for( ptr=(is_new?rshdr2->rsh_bitblk:rshdr->rsh_bitblk)+rsc-siz,
      i=(is_new?rshdr2->rsh_nbb:rshdr->rsh_nbb)+1; --i>0; ptr+=sizeof(BITBLK) )
    rsc_incr( (long *)&((BITBLK *)ptr)->bi_pdata );
  for( ptr=(is_new?rshdr2->rsh_frstr:rshdr->rsh_frstr)+rsc-siz,
      i=(is_new?rshdr2->rsh_nstring:rshdr->rsh_nstring)+1; --i>0; ptr+=sizeof(long) )
    rsc_incr( (long *)ptr );
  for( ptr=(is_new?rshdr2->rsh_frimg:rshdr->rsh_frimg)+rsc-siz,
      i=(is_new?rshdr2->rsh_nimages:rshdr->rsh_nimages)+1; --i>0; ptr+=sizeof(long) )
    rsc_incr( (long *)ptr );
  for( ptr=(is_new?rshdr2->rsh_trindex:rshdr->rsh_trindex)+rsc-siz,
      i=(is_new?rshdr2->rsh_ntree:rshdr->rsh_ntree)+1; --i>0; ptr+=sizeof(long) )
    rsc_incr( (long *)ptr );
}

int rsrc_rcfix( RSHDR *rsc )
{
  char is_new;
#if 0
  char *txt, **ptxt;
  OBJECT **ptr;
  int i;
  long *l, *cicons, ii, ic;
  CICONBLK *c;
  CICON *ci, *next, *best;
#endif

  is_new = long_rsc(rsc);
  add = (long)rsc;
#if 0
  num_cic = 0L;
  if( (rsc->rsh_vrsn & (1<<2)) || is_new )
  {
    l = get_cicons(rsc);
    *l += (long)rsc;
    cicons = *(long **)l;
    for( l=cicons, ii=0L; !*l; l++, ii++ );
    if( *l++ == -1L )
    {
      cicon_ptr = cicons;
      num_cic = ii;
      for( c=(CICONBLK *)l; *cicons!=-1L; )
      {
        *cicons++ = (long)c;
        ii = (long)((c->monoblk.ib_wicon+15)>>4) * c->monoblk.ib_hicon;	/* words in mono data */
        ic = *(long *)((long)c + sizeof(ICONBLK));	/* number of color icons */
        txt = (char *)((c->monoblk.ib_pmask =
            (c->monoblk.ib_pdata = (int *)((long)c + sizeof(ICONBLK) +
            sizeof(long))) + ii) + ii);
        ptxt = &c->monoblk.ib_ptext;
        *ptxt = *ptxt && (unsigned long)*ptxt < (unsigned long)txt-
            (unsigned long)rsc ? (char *)((long)rsc + (long)*ptxt) : txt;
        ci = (CICON *)((long)txt + 12);
        best = 0L;
        while( --ic>=0 )
        {
          ci->col_data = (int *)(ci+1);
          next = (CICON *)(ci->col_mask = ci->col_data + ii*ci->num_planes);
          if( ci->sel_data )
          {
            ci->sel_data = ci->col_mask + ii;
            ci->sel_mask = ci->sel_data + ii*ci->num_planes;
            next = (CICON *)ci->sel_mask;
          }
          if( ci->num_planes <= vplanes && (!best || ci->num_planes > best->num_planes) )
              best = ci;
          ci->next_res = 0L;
          ci = (CICON *)((int *)next + ii);
        }
        if( best ) trans_cicon( c, best, 0 );
        c = (CICONBLK *)ci;
      }
    }
    else rsc->rsh_vrsn &= ~(1<<2);
  }
#endif
  fix_rsc( rsc, (char *)rsc+(is_new ? sizeof(RSHDR2) : sizeof(RSHDR)) );
  return 1;
}

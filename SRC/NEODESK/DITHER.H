#ifndef __DITHER_H
  #define __DITHER_H
  #include "vdi.h"

  typedef struct
  {
    MFDB mfdb;
    int colors, strez;
    int *pall, (*intens)[3];
    long len;
    unsigned char **palette, use_vdi;
    int xasp, yasp;
  } PICTURE;

  void ditherfile( PICTURE *fi );
#endif

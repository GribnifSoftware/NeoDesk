/************************************************************************

     CATTGIOS.H

     CyReL CaTTamaran GIOS Includes
     
     Copyright (c) 1994 by Cybercube Research Limited
     Toronto, Canada
     All rights reserved

*/

#if !defined( __CATTGIOS )
#define __CATTGIOS

#define	_CaTT_inquire 0
#define	_CaTT_s16_mhz 1
#define	_CaTT_s32_mhz 2
#define	_CaTT_s40_mhz 3
#define	_CaTT_s48_mhz 4
#define	_CaTT_s64_mhz 5
#define	_CaTT_smhz_up 6
#define	_CaTT_smhz_dn 7
#define	_CaTT_g_cmode 8
#define	_CaTT_sense_t 9
#define	_CaTT_sfpumhz 10
#define	_CaTT_gfpumhz 11
#define _CaTT_s56_mhz 12

typedef struct GIOS_STRCT
        {
                char    header[16];
                char    revision[6];
                char    version[17];
                char    date[11];
                char    terminator;
                char    reserved;
                
                int     max_function_code;
                int     boot_mode;
                int     desktop_mode;
                int     current_mode;
                int     old_mode;
                int     fpu_mode;
                int     catt_modes_table[8];

        } CaTT_GIOS;

extern int Init_CaTT_GIOS(void);
extern int CaTT_inquire(CaTT_GIOS *pntr,long length);
extern int CaTT_s16_mhz(void);
extern int CaTT_s32_mhz(void);
extern int CaTT_s40_mhz(void);
extern int CaTT_s48_mhz(void);
extern int CaTT_s64_mhz(void);
extern int CaTT_smhz_up(void);
extern int CaTT_smhz_dn(void);
extern int CaTT_g_cmode(void);
extern int CaTT_sense_t(void);
extern int CaTT_sfpumhz(int mhz);
extern int CaTT_gfpumhz(void);

#endif

/************************************************************************/

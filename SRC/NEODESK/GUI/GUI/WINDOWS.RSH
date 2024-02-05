/* GEM Resource C Source */

#include <portab.h>
#include <aes.h>
#include "WINDOWS.H"

#if !defined(WHITEBAK)
#define WHITEBAK    0x0040
#endif
#if !defined(DRAW3D)
#define DRAW3D      0x0080
#endif

#define FLAGS9  0x0200
#define FLAGS10 0x0400
#define FLAGS11 0x0800
#define FLAGS12 0x1000
#define FLAGS13 0x2000
#define FLAGS14 0x4000
#define FLAGS15 0x8000
#define STATE8  0x0100
#define STATE9  0x0200
#define STATE10 0x0400
#define STATE11 0x0800
#define STATE12 0x1000
#define STATE13 0x2000
#define STATE14 0x4000
#define STATE15 0x8000

TEDINFO rs_tedinfo[] =
{ "\0",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x11A1, 0, 1, 1, 1,
  "\0",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x1180, 0, 1, 1, 1,
};

OBJECT rs_object[] =
{ 
  /******** Tree 0 DESK ****************************************************/
        -1,       -1,       -1, G_BOX     ,   /* Object 0  */
  LASTOB, NORMAL, (LONG)0x00001143L,
  0x0000, 0x0000, 0x0034, 0x0009,
  
  /******** Tree 1 WIND ****************************************************/
        -1, WCLOSE  , WSIZE   , G_IBOX    ,   /* Object 0  */
  NONE, SHADOWED, (LONG)0x00011100L,
  0x0000, 0x0000, 0x0234, 0x0C12,
  WMOVE   ,       -1,       -1, G_BOXCHAR ,   /* Object 1 WCLOSE */
  SELECTABLE, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\005' << 24)|0x00011100L,
  0x0000, 0x0000, 0x0302, 0x0301,
  WICONIZ ,       -1,       -1, G_BOXTEXT ,   /* Object 2 WMOVE */
  NONE, NORMAL, (LONG)&rs_tedinfo[0],
  0x0202, 0x0000, 0x042B, 0x0301,
  WBACK   ,       -1,       -1, G_BOXCHAR ,   /* Object 3 WICONIZ */
  SELECTABLE, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\177' << 24)|0x00011100L,
  0x042D, 0x0000, 0x0302, 0x0301,
  WFULL   ,       -1,       -1, G_BOXCHAR ,   /* Object 4 WBACK */
  SELECTABLE, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\256' << 24)|0x00011100L,
  0x052F, 0x0000, 0x0302, 0x0301,
  WILEFT  ,       -1,       -1, G_BOXCHAR ,   /* Object 5 WFULL */
  SELECTABLE, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\007' << 24)|0x00011100L,
  0x0731, 0x0000, 0x0302, 0x0301,
  WINFO   ,       -1,       -1, G_BOXCHAR ,   /* Object 6 WILEFT */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\004' << 24)|0x00011100L,
  0x0000, 0x0201, 0x0302, 0x0301,
  WIRT    ,       -1,       -1, G_BOXTEXT ,   /* Object 7 WINFO */
  NONE, NORMAL, (LONG)&rs_tedinfo[1],
  0x0202, 0x0201, 0x062F, 0x0301,
  WTOOLBOX,       -1,       -1, G_BOXCHAR ,   /* Object 8 WIRT */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\003' << 24)|0x00011100L,
  0x0731, 0x0201, 0x0302, 0x0301,
  WMNLEFT ,       -1,       -1, G_IBOX    ,   /* Object 9 WTOOLBOX */
  NONE, NORMAL, (LONG)0x00011100L,
  0x0000, 0x0402, 0x0234, 0x0301,
  WMENU   ,       -1,       -1, G_BOXCHAR ,   /* Object 10 WMNLEFT */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\004' << 24)|0x00011100L,
  0x0000, 0x0603, 0x0302, 0x0301,
  WMNRT   ,       -1,       -1, G_BOX     ,   /* Object 11 WMENU */
  NONE, NORMAL, (LONG)0x00011170L,
  0x0202, 0x0603, 0x062F, 0x0301,
  WUP     ,       -1,       -1, G_BOXCHAR ,   /* Object 12 WMNRT */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\003' << 24)|0x00011100L,
  0x0631, 0x0603, 0x0302, 0x0301,
  WVBIGSL ,       -1,       -1, G_BOXCHAR ,   /* Object 13 WUP */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\001' << 24)|0x00011100L,
  0x0631, 0x0804, 0x0302, 0x0301,
  WDOWN   , WVSMLSL , WVSMLSL , G_BOX     ,   /* Object 14 WVBIGSL */
  NONE, NORMAL, (LONG)0x00011111L,
  0x0631, 0x0A05, 0x0302, 0x0204,
  WVBIGSL ,       -1,       -1, G_BOX     ,   /* Object 15 WVSMLSL */
  NONE, DRAW3D|STATE12|STATE15, (LONG)0x00011101L,
  0x0000, 0x0401, 0x0302, 0x0602,
  WVSPLIT ,       -1,       -1, G_BOXCHAR ,   /* Object 16 WDOWN */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\002' << 24)|0x00011100L,
  0x0631, 0x0B09, 0x0302, 0x0301,
  WUP2    ,       -1,       -1, G_BOX     ,   /* Object 17 WVSPLIT */
  NONE, NORMAL, (LONG)0x00011100L,
  0x0000, 0x0F0A, 0x0234, 0x0600,
  WVBIGSL2,       -1,       -1, G_BOXCHAR ,   /* Object 18 WUP2 */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\001' << 24)|0x00011100L,
  0x0631, 0x040B, 0x0302, 0x0301,
  WDOWN2  , WVSMLSL2, WVSMLSL2, G_BOX     ,   /* Object 19 WVBIGSL2 */
  NONE, NORMAL, (LONG)0x00011111L,
  0x0631, 0x060C, 0x0302, 0x0204,
  WVBIGSL2,       -1,       -1, G_BOX     ,   /* Object 20 WVSMLSL2 */
  NONE, DRAW3D|STATE12|STATE15, (LONG)0x00011101L,
  0x0000, 0x0401, 0x0302, 0x0602,
  WLEFT   ,       -1,       -1, G_BOXCHAR ,   /* Object 21 WDOWN2 */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\002' << 24)|0x00011100L,
  0x0631, 0x0710, 0x0302, 0x0301,
  WHBIGSL ,       -1,       -1, G_BOXCHAR ,   /* Object 22 WLEFT */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\004' << 24)|0x00011100L,
  0x0000, 0x0911, 0x0302, 0x0301,
  WRT     , WHSMLSL , WHSMLSL , G_BOX     ,   /* Object 23 WHBIGSL */
  NONE, NORMAL, (LONG)0x00011111L,
  0x0202, 0x0911, 0x0715, 0x0301,
  WHBIGSL ,       -1,       -1, G_BOX     ,   /* Object 24 WHSMLSL */
  NONE, DRAW3D|STATE12|STATE15, (LONG)0x00011101L,
  0x0603, 0x0000, 0x0306, 0x0301,
  WHSPLIT ,       -1,       -1, G_BOXCHAR ,   /* Object 25 WRT */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\003' << 24)|0x00011100L,
  0x0717, 0x0911, 0x0302, 0x0301,
  WLEFT2  ,       -1,       -1, G_BOX     ,   /* Object 26 WHSPLIT */
  NONE, NORMAL, (LONG)0x00011100L,
  0x011A, 0x0804, 0x0600, 0x040E,
  WHBIGSL2,       -1,       -1, G_BOXCHAR ,   /* Object 27 WLEFT2 */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\004' << 24)|0x00011100L,
  0x051A, 0x0911, 0x0302, 0x0301,
  WRT2    , WHSMLSL2, WHSMLSL2, G_BOX     ,   /* Object 28 WHBIGSL2 */
  NONE, NORMAL, (LONG)0x00011111L,
  0x071C, 0x0911, 0x0712, 0x0301,
  WHBIGSL2,       -1,       -1, G_BOX     ,   /* Object 29 WHSMLSL2 */
  NONE, DRAW3D|STATE12|STATE15, (LONG)0x00011101L,
  0x0603, 0x0000, 0x0306, 0x0301,
  WSIZE   ,       -1,       -1, G_BOXCHAR ,   /* Object 30 WRT2 */
  SELECTABLE|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\003' << 24)|0x00011100L,
  0x052F, 0x0911, 0x0302, 0x0301,
         0,       -1,       -1, G_BOXCHAR ,   /* Object 31 WSIZE */
  SELECTABLE|LASTOB|TOUCHEXIT, DRAW3D|STATE11|STATE12|STATE15, (LONG) ((LONG)'\006' << 24)|0x00011100L,
  0x0631, 0x0911, 0x0302, 0x0301,
};

OBJECT *rs_trindex[] =
{ &rs_object[0],   /* Tree  0 DESK     */
  &rs_object[1],   /* Tree  1 WIND     */
};

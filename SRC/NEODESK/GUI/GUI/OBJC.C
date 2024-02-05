#include "new_aes.h"
#include "xwind.h"
#include "stdlib.h"
#include "string.h"
#include "tos.h"
#include "neocommn.h"
#include "vdi.h"
#include "win_var.h"
#include "win_inc.h"

#define DEBUGGER(x,y,z)

void v_ftext16( int handle, int x, int y, int *wstr, int strlen );
void v_ftext16_mono( int handle, int x, int y, int *wstr, int strlen, int offset );
void vqt_f_extent16( int handle, int *wstr, int strlen, int *extent );

static int RSBB0DATA[] =
{ 0x03C0, 0x0C30, 0x1008, 0x2004,
  0x2004, 0x4002, 0x4002, 0x4002,
  0x4002, 0x2004, 0x2004, 0x1008,
  0x0C30, 0x03C0, 0x0000
};

static int RSBB1DATA[] =
{ 0x03C0, 0x0C30, 0x1008, 0x23C4,
  0x27E4, 0x4FF2, 0x4FF2, 0x4FF2,
  0x4FF2, 0x27E4, 0x23C4, 0x1008,
  0x0C30, 0x03C0, 0x0000
};

static int RSBB2DATA[] =
{ 0xFFFF, 0x8001, 0x8001, 0x8001,
  0x8001, 0x8001, 0x8001, 0x8001,
  0x8001, 0x8001, 0x8001, 0x8001,
  0x8001, 0x8001, 0xFFFF
};

static int RSBB3DATA[] =
{ 0xFFFF, 0x8001, 0xB00D, 0x9819,
  0x8C31, 0x8661, 0x83C1, 0x8181,
  0x83C1, 0x8661, 0x8C31, 0x9819,
  0xB00D, 0x8001, 0xFFFF
};

static int RSBB4DATA[] =
{ 0x1556, 0x0002, 0x1000, 0xFFF2,
  0x8010, 0x9012, 0x8010, 0x9012,
  0x8010, 0x9012, 0x8010, 0x955A,
  0x8010, 0x8010, 0xFFF0
};

static int RSBB5DATA[] =
{ 0x1B6D, 0x0000, 0xFFF9, 0x9008,
  0x8009, 0x9B6D, 0x8008, 0xFFF8
};

static int RS0_MMASK[] =
{ 0x03C0, 0x0FF0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFF, 0x7FFF,
  0x7FFF, 0x3FFF, 0x3FFF, 0x1FFE,
  0x0FFE, 0x07FC, 0x01F0
};

static int RS0_MDATA[] =
{ 0x03C0, 0x0C30, 0x1008, 0x2404,
  0x2806, 0x4802, 0x5003, 0x4003,
  0x4003, 0x2007, 0x2006, 0x100E,
  0x0C3C, 0x07F8, 0x01E0
};

static int RS0_4CMASK[] =
{ 0x03C0, 0x0FF0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFF, 0x7FFF,
  0x7FFF, 0x3FFF, 0x3FFE, 0x1FFE,
  0x0FFC, 0x07F8, 0x01E0
};

static int RS0_4CDATA[] =
{ 0x03C0, 0x0C30, 0x11F8, 0x27FC,
  0x2FFE, 0x4FFA, 0x5FFB, 0x5FFB,
  0x5FFB, 0x3FF7, 0x3FF6, 0x1FCE,
  0x0C3C, 0x07F8, 0x01E0, 0x03C0,
  0x0C30, 0x11F8, 0x27FC, 0x2FFE,
  0x4FFA, 0x5FFB, 0x5FFB, 0x5FFB,
  0x3FF7, 0x3FF6, 0x1FCE, 0x0C3C,
  0x07F8, 0x01E0, 0x03C0, 0x0C30,
  0x11F8, 0x27FC, 0x2FFE, 0x4FFA,
  0x5FFB, 0x5FFB, 0x5FFB, 0x3FF7,
  0x3FF6, 0x1FCE, 0x0C3C, 0x07F8,
  0x01E0, 0x03C0, 0x0C30, 0x1008,
  0x2004, 0x2006, 0x4006, 0x4007,
  0x4007, 0x4007, 0x200F, 0x200E,
  0x103E, 0x0FFC, 0x07F8, 0x01E0
};

static int RS0_4SMASK[] =
{ 0x03C0, 0x0FE0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFF, 0x7FFF,
  0x7FFF, 0x3FFF, 0x1FFE, 0x1FFE,
  0x0FFC, 0x07F8, 0x01E0
};

static int RS0_4SDATA[] =
{ 0x03C0, 0x0FE0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFD, 0x7FFD,
  0x7FFD, 0x3FFD, 0x1FFA, 0x1FFA,
  0x0FE4, 0x0618, 0x01E0, 0x03C0,
  0x0FE0, 0x1FF8, 0x3FFC, 0x3FFE,
  0x7FFE, 0x7FFD, 0x7FFD, 0x7FFD,
  0x3FFD, 0x1FFA, 0x1FFA, 0x0FE4,
  0x0618, 0x01E0, 0x03C0, 0x0FE0,
  0x1FF8, 0x3FFC, 0x3FFE, 0x7FFE,
  0x7FFD, 0x7FFD, 0x7FFD, 0x3FFD,
  0x1FFA, 0x1FFA, 0x0FE4, 0x0618,
  0x01E0, 0x0000, 0x01E0, 0x07F8,
  0x0F04, 0x1C02, 0x18E2, 0x39F1,
  0x33F9, 0x33F9, 0x33F9, 0x11F2,
  0x10E2, 0x0804, 0x0618, 0x01E0
};

static int RS0_1CMASK[] =
{ 0x03C0, 0x0FF0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFF, 0x7FFF,
  0x7FFF, 0x3FFF, 0x3FFF, 0x1FFE,
  0x0FFE, 0x07FC, 0x01F0
};

static int RS0_1CDATA[] =
{ 0x03C0, 0x0C30, 0x1008, 0x2404,
  0x2806, 0x4802, 0x5003, 0x4003,
  0x4003, 0x2007, 0x2006, 0x100E,
  0x0C3C, 0x07F8, 0x01E0
};

static int RS0_1SMASK[] =
{ 0x03C0, 0x0FE0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFF, 0x7FFF,
  0x7FFF, 0x3FFF, 0x1FFF, 0x1FFE,
  0x0FFE, 0x07FC, 0x01F0
};

static int RS0_1SDATA[] =
{ 0x0000, 0x01E0, 0x07F8, 0x0F04,
  0x1C02, 0x18E2, 0x39F1, 0x33F9,
  0x33F9, 0x33F9, 0x11F3, 0x10E2,
  0x0806, 0x061C, 0x01F0
};

/***** static int RS0_2CMASK[] =
{ 0x03C0, 0x0FF0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFF, 0x7FFF,
  0x7FFF, 0x3FFF, 0x3FFE, 0x1FFE,
  0x0FFC, 0x07F8, 0x01E0
};

static int RS0_2CDATA[] =
{ 0x03C0, 0x0C30, 0x1008, 0x2004,
  0x2006, 0x4006, 0x4007, 0x4007,
  0x4007, 0x200F, 0x200E, 0x103E,
  0x0FFC, 0x07F8, 0x01E0, 0x03C0,
  0x0C30, 0x11F8, 0x27FC, 0x2FFE,
  0x4FFA, 0x5FFB, 0x5FFB, 0x5FFB,
  0x3FF7, 0x3FF6, 0x1FCE, 0x0C3C,
  0x07F8, 0x01E0
};

static int RS0_2SMASK[] =
{ 0x03C0, 0x0FE0, 0x1FF8, 0x3FFC,
  0x3FFE, 0x7FFE, 0x7FFF, 0x7FFF,
  0x7FFF, 0x3FFF, 0x1FFE, 0x1FFE,
  0x0FFC, 0x07F8, 0x01E0
};

static int RS0_2SDATA[] =
{ 0x0000, 0x01E0, 0x07F8, 0x0F04,
  0x1C02, 0x18E2, 0x39F1, 0x33F9,
  0x33F9, 0x33F9, 0x11F2, 0x10E2,
  0x0804, 0x0618, 0x01E0, 0x03C0,
  0x0FE0, 0x1FF8, 0x3FFC, 0x3FFE,
  0x7FFE, 0x7FFD, 0x7FFD, 0x7FFD,
  0x3FFD, 0x1FFA, 0x1FFA, 0x0FE4,
  0x0618, 0x01E0
}; *****/

static int RS1_MMASK[] =
{ 0x0FF0, 0x7FFE, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x7FFE, 0x0FF0
};

static int RS1_MDATA[] =
{ 0x0FF0, 0x781E, 0xE007, 0xC003,
  0xC003, 0xE007, 0x781E, 0x0FF0
};

static int RS1_4CMASK[] =
{ 0x1E00, 0x3F00, 0x7F80, 0x7F80,
  0x7F80, 0x3F00, 0x1E00, 0x0000
};

static int RS1_4CDATA[] =
{ 0x1E00, 0x2F00, 0x5F80, 0x7F80,
  0x7E80, 0x3D00, 0x1E00, 0x0000,
  0x1E00, 0x2F00, 0x5F80, 0x7F80,
  0x7E80, 0x3D00, 0x1E00, 0x0000,
  0x1E00, 0x2F00, 0x5F80, 0x7F80,
  0x7E80, 0x3D00, 0x1E00, 0x0000,
  0x1E00, 0x2100, 0x4080, 0x4080,
  0x4180, 0x2300, 0x1E00, 0x0000
};

static int RS1_4SMASK[] =
{ 0x1E00, 0x3F00, 0x7F80, 0x7F80,
  0x7F80, 0x3F00, 0x1E00, 0x0000
};

static int RS1_4SDATA[] =
{ 0x1E00, 0x3100, 0x6080, 0x4080,
  0x4080, 0x2100, 0x1E00, 0x0000,
  0x1E00, 0x3100, 0x6080, 0x4080,
  0x4080, 0x2100, 0x1E00, 0x0000,
  0x1E00, 0x3100, 0x6080, 0x4080,
  0x4080, 0x2100, 0x1E00, 0x0000,
  0x1E00, 0x3F00, 0x7F80, 0x7F80,
  0x7F80, 0x3F00, 0x1E00, 0x0000
};

static int RS1_1CMASK[] =
{ 0x0FF0, 0x7FFE, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x7FFE, 0x0FF0
};

static int RS1_1CDATA[] =
{ 0x0FF0, 0x781E, 0xE007, 0xC003,
  0xC003, 0xE007, 0x781E, 0x0FF0
};

static int RS1_1SMASK[] =
{ 0x0FF0, 0x7FFE, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x7FFE, 0x0FF0
};

static int RS1_1SDATA[] =
{ 0x0FF0, 0x781E, 0xE3C7, 0xCFF3,
  0xCFF3, 0xE3C7, 0x781E, 0x0FF0
};

static int RS2_MMASK[] =
{ 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0x0000
};

static int RS2_MDATA[] =
{ 0xFFFC, 0x8006, 0x8006, 0x8006,
  0x8006, 0x8006, 0x8006, 0x8006,
  0x8006, 0x8006, 0x8006, 0x8006,
  0xFFFE, 0x7FFC, 0x0000
};

static int RS2_4CMASK[] =
{ 0xFFFC, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0x7FFE, 0x0000
};

static int RS2_4CDATA[] =
{ 0xFFFC, 0x8006, 0xBFF6, 0xBFF6,
  0xBFF6, 0xBFF6, 0xBFF6, 0xBFF6,
  0xBFF6, 0xBFF6, 0xBFF6, 0x8006,
  0xFFFE, 0x7FFE, 0x0000, 0xFFFC,
  0x8006, 0xBFF6, 0xBFF6, 0xBFF6,
  0xBFF6, 0xBFF6, 0xBFF6, 0xBFF6,
  0xBFF6, 0xBFF6, 0x8006, 0xFFFE,
  0x7FFE, 0x0000, 0xFFFC, 0x8006,
  0xBFF6, 0xBFF6, 0xBFF6, 0xBFF6,
  0xBFF6, 0xBFF6, 0xBFF6, 0xBFF6,
  0xBFF6, 0x8006, 0xFFFE, 0x7FFE,
  0x0000, 0xFFFC, 0x800E, 0x800E,
  0x800E, 0x800E, 0x800E, 0x800E,
  0x800E, 0x800E, 0x800E, 0x800E,
  0xFFFE, 0xFFFE, 0x7FFC, 0x0000
};

static int RS2_4SMASK[] =
{ 0xFFFC, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0x7FFE, 0x0000
};

static int RS2_4SDATA[] =
{ 0xFFFC, 0xFFFE, 0xC002, 0xDFFE,
  0xDDDE, 0xDEBE, 0xDF7E, 0xDFFE,
  0xDDDE, 0xDBEE, 0xD7F6, 0xDFFE,
  0xDFFE, 0x7FFE, 0x0000, 0xFFFC,
  0xFFFE, 0xC002, 0xDFFE, 0xDDDE,
  0xDEBE, 0xDF7E, 0xDFFE, 0xDDDE,
  0xDBEE, 0xD7F6, 0xDFFE, 0xDFFE,
  0x7FFE, 0x0000, 0xFFFC, 0xFFFE,
  0xC002, 0xDFFE, 0xDDDE, 0xDEBE,
  0xDF7E, 0xDFFE, 0xDDDE, 0xDBEE,
  0xD7F6, 0xDFFE, 0xDFFE, 0x7FFE,
  0x0000, 0x0000, 0x7FFE, 0x7FFE,
  0x6002, 0x6C1A, 0x6632, 0x6362,
  0x61C2, 0x61C2, 0x6362, 0x6632,
  0x6C1A, 0x6002, 0x7FFE, 0x0000
};

/*****static int RS2_2CMASK[] =
{ 0xFFFC, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0x7FFE, 0x0000
};

static int RS2_2CDATA[] =
{ 0xFFFC, 0x800E, 0x800E, 0x800E,
  0x800E, 0x800E, 0x800E, 0x800E,
  0x800E, 0x800E, 0x800E, 0xFFFE,
  0xFFFE, 0x7FFC, 0x0000, 0xFFFC,
  0x8006, 0xBFF6, 0xBFF6, 0xBFF6,
  0xBFF6, 0xBFF6, 0xBFF6, 0xBFF6,
  0xBFF6, 0xBFF6, 0x8006, 0xFFFE,
  0x7FFE, 0x0000
};

static int RS2_2SMASK[] =
{ 0xFFFC, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0x7FFE, 0x0000
};

static int RS2_2SDATA[] =
{ 0x0000, 0x7FFE, 0x7FFE, 0x6002,
  0x6C1A, 0x6632, 0x6362, 0x61C2,
  0x61C2, 0x6362, 0x6632, 0x6C1A,
  0x6002, 0x7FFE, 0x0000, 0xFFFC,
  0xFFFE, 0xC002, 0xDFFE, 0xDDDE,
  0xDEBE, 0xDF7E, 0xDFFE, 0xDDDE,
  0xDBEE, 0xD7F6, 0xDFFE, 0xDFFE,
  0x7FFE, 0x0000
}; ***/

static int RS2_1CMASK[] =
{ 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0x0000
};

static int RS2_1CDATA[] =
{ 0xFFFC, 0x8006, 0x8006, 0x8006,
  0x8006, 0x8006, 0x8006, 0x8006,
  0x8006, 0x8006, 0x8006, 0x8006,
  0xFFFE, 0x7FFC, 0x0000
};

static int RS2_1SMASK[] =
{ 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
  0xFFFE, 0xFFFE, 0x0000
};

static int RS2_1SDATA[] =
{ 0x0000, 0x7FFE, 0x7FFE, 0x6002,
  0x6C1A, 0x6632, 0x6362, 0x61C2,
  0x61C2, 0x6362, 0x6632, 0x6C1A,
  0x6002, 0x7FFE, 0x0000
};

static int RS3_MMASK[] =
{ 0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC,
  0x3FFC, 0x3FFC, 0x3FFC, 0x0000
};

static int RS3_MDATA[] =
{ 0x3FFC, 0x2004, 0x2004, 0x2004,
  0x2004, 0x2004, 0x3FFC, 0x0000
};

static int RS3_4CMASK[] =
{ 0x7F80, 0x7FC0, 0x7FC0, 0x7FC0,
  0x7FC0, 0x7FC0, 0x7FC0, 0x3FC0
};

static int RS3_4CDATA[] =
{ 0x7F80, 0x40C0, 0x5EC0, 0x5EC0,
  0x5EC0, 0x40C0, 0x7FC0, 0x3FC0,
  0x7F80, 0x40C0, 0x5EC0, 0x5EC0,
  0x5EC0, 0x40C0, 0x7FC0, 0x3FC0,
  0x7F80, 0x40C0, 0x5EC0, 0x5EC0,
  0x5EC0, 0x40C0, 0x7FC0, 0x3FC0,
  0x7F80, 0x40C0, 0x41C0, 0x41C0,
  0x41C0, 0x5FC0, 0x7FC0, 0x3FC0
};

static int RS3_4SMASK[] =
{ 0x7F80, 0x7FC0, 0x7FC0, 0x7FC0,
  0x7FC0, 0x7FC0, 0x7FC0, 0x3FC0
};

static int RS3_4SDATA[] =
{ 0x7F80, 0x7FC0, 0x7FC0, 0x7FC0,
  0x7FC0, 0x7FC0, 0x7FC0, 0x3FC0,
  0x7F80, 0x7FC0, 0x7FC0, 0x7FC0,
  0x7FC0, 0x7FC0, 0x7FC0, 0x3FC0,
  0x7F80, 0x7FC0, 0x7FC0, 0x7FC0,
  0x7FC0, 0x7FC0, 0x7FC0, 0x3FC0,
  0x0000, 0x3FC0, 0x2040, 0x2940,
  0x2640, 0x2940, 0x2040, 0x3FC0
};

static int RS3_1CMASK[] =
{ 0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC,
  0x3FFC, 0x3FFC, 0x3FFC, 0x0000
};

static int RS3_1CDATA[] =
{ 0x3FFC, 0x2004, 0x2004, 0x2004,
  0x2004, 0x2004, 0x3FFC, 0x0000
};

static int RS3_1SMASK[] =
{ 0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC,
  0x3FFC, 0x3FFC, 0x3FFC, 0x0000
};

static int RS3_1SDATA[] =
{ 0x3FFC, 0x381C, 0x2664, 0x2184,
  0x2664, 0x381C, 0x3FFC, 0x0000
};

/* 003: changed indices in next two structs */
static CICON rs_cicon[] =
{
  1, RS0_1CDATA, RS0_1CMASK, RS0_1SDATA, RS0_1SMASK, &rs_cicon[1],
/*  2, RS0_2CDATA, RS0_2CMASK, RS0_2SDATA, RS0_2SMASK, &rs_cicon[2], 003 */
  4, RS0_4CDATA, RS0_4CMASK, RS0_4SDATA, RS0_4SMASK, 0L,
  1, RS1_1CDATA, RS1_1CMASK, RS1_1SDATA, RS1_1SMASK, &rs_cicon[3],
  4, RS1_4CDATA, RS1_4CMASK, RS1_4SDATA, RS1_4SMASK, 0L,
  1, RS2_1CDATA, RS2_1CMASK, RS2_1SDATA, RS2_1SMASK, &rs_cicon[5],
/*  2, RS2_2CDATA, RS2_2CMASK, RS2_2SDATA, RS2_2SMASK, &rs_cicon[7], 003 */
  4, RS2_4CDATA, RS2_4CMASK, RS2_4SDATA, RS2_4SMASK, 0L,
  1, RS3_1CDATA, RS3_1CMASK, RS3_1SDATA, RS3_1SMASK, &rs_cicon[7],
  4, RS3_4CDATA, RS3_4CMASK, RS3_4SDATA, RS3_4SMASK, 0L,
};

static CICONBLK rs_ciconblk[] =
{ RS0_MMASK, RS0_MDATA, "\0", 0x1000|'\0',
    0,	 0,   0,   0,  16,  15,   0,   0,   0,	 8, &rs_cicon[0],
  RS1_MMASK, RS1_MDATA, "\0", 0x1000|'\0',
    0,	 0,   0,   0,  16,   8,   0,   0,   0,	 8, &rs_cicon[2],
  RS2_MMASK, RS2_MDATA, "\0", 0x1000|'\0',
    0,	 0,   0,   0,  16,  15,   0,   0,   0,	 8, &rs_cicon[4],
  RS3_MMASK, RS3_MDATA, "\0", 0x1000|'\0',
    0,	 0,   0,   0,  16,   8,   0,   0,   0,	 8, &rs_cicon[6]
};

#define RADIO_HI  0
#define RADIO_LOW 1
#define CHECK_HI  2
#define CHECK_LOW 3

CICONBLK *radio, *check;

/*        [radchk][selected] (hi only) */
int *rbdat[2][2] = { RSBB0DATA, RSBB1DATA, RSBB2DATA, RSBB3DATA };

char dcl_ret, ted_buf[81], use_oldstate;
int old_state;

void to_arr( int x, int y, int w, int h )
{
  oarray[2] = (oarray[0] = x) + w - 1;
  oarray[3] = (oarray[1] = y) + h - 1;
}

void to_larr( int x, int y, int w, int h )
{
  oarray[4] = oarray[2] = (oarray[8] = oarray[6] = oarray[0] = x) + w - 1;
  oarray[7] = oarray[5] = (oarray[9] = oarray[3] = oarray[1] = y) + h - 1;
}

void box_inout( int i )
{
  oarray[0] += i;
  oarray[1] += i;
  oarray[2] -= i;
  oarray[3] -= i;
}

void pline_inout( int i )
{
  oarray[8] = oarray[6] = oarray[0] += i;
  oarray[4] = oarray[2] -= i;
  oarray[9] = oarray[3] = oarray[1] += i;
  oarray[7] = oarray[5] -= i;
}

void pline_2( int *arr )
{
  v_pline( vdi_hand, 2, arr );
}

void pline_5(void)
{
  v_pline( vdi_hand, 5, oarray );
}

void vdi_box( int intcol, int inter, int style, int writ, Rect *r )
{
  _vsf_color( intcol );
  _vsf_interior( inter );
  _vsf_style( style );
  _vswr_mode( writ );
  if( r )
  {
    to_arr( r->x+1, r->y+1, r->w-2, r->h-2 );
    vr_recfl( vdi_hand, oarray );
  }
}

static void set_line( int mode, int type, int color )
{
  _vswr_mode( mode );
  _vsl_type( type );
  _vsl_color( color );
}

int p_extent( int num )
{
  int pts[8];

  _vst_charmap( 0 );
  vqt_f_extent16( vdi_hand, text_arr, num, pts );
  _vst_charmap( 1 );
  return pts[2]-pts[0];
}

int prop_extent( int font, char *str, int *num )
{
  int pts[8];
  
  *num = strlen(str);
  if( font==GDOS_PROP || font==GDOS_MONO )
  {
    text_2_arr( str, num );
    return p_extent( *num );
  }
  else if( font==GDOS_BITM )
  {
    vqt_extent( vdi_hand, str, pts );
    return pts[2]-pts[0];
  }
  return 0;
}
int txt_extent( char *str, int clip, int cw, int id, int font, int just, Rect *r, int *x, int *y )
{
  int i, len, num, w=r->w-2;
  char c;

  if( id != 1 && (font==GDOS_PROP || font==GDOS_BITM) )
  {
    len = prop_extent( font, str, &num );
    if( clip && len > w )
    {
      while( len > w && --num>0 )	/* order important */
	if( font==GDOS_PROP ) len = p_extent(num);
	else
	{
	  c = str[num];
	  str[num] = 0;
	  len = prop_extent( font, str, &num );
	  str[num] = c;
	}
      if( num<=0 ) return -1;
    }
  }
  else
  {
    num = strlen(str);
    if( clip && (len = w/cw) <= num ) num = len;
    if( !num ) return -1;
    len = num * cw;
  }
  switch(just)
  {
    default:
      i = 1;
      break;
    case 1:
      i = r->w-len-1;
      break;
    case 2:
      i = (r->w-len+1)>>1;
  }
  *x = r->x+i;
  *y = r->y+((r->h-txt_clh+1)>>1);
  return(num);
}

int ted_font( int font, int id, int point, int *cw, int *ch )
{
  if( font==SMALL )
  {
    point = 8;
    _vst_font( 1, 0 );
  }
  else if( speedo && (font==GDOS_MONO || font==GDOS_PROP) )
  {
    if( _vst_font( id, 1 ) != id )
    {
      point=0;
      font=IBM;
      _vst_font( font_id, font_scalable );
    }
  }
  else if( font==GDOS_BITM ) _vst_font( id, 0 );
  else
  {
    point=0;
    font = IBM;
    _vst_font( font_id, font_scalable );
  }
  if( point<=0 ) point = ptsiz;
  _vst_point( point, oarray, oarray, cw, ch );
  return font;
}

int text_ini( Rect *r, int just, int font, int id, int point, char **str, int color, int effects,
    int *cw, int *x, int *y, TINI *specl, int clip )
{
  int ch, i, num;

  if( !*str || !**str ) return(-1);
  if( specl && ((specl->o->ob_statex&((X_MAGMASK|X_KBD_EQUIV)>>8)) ==
      ((X_MAGIC|X_KBD_EQUIV)>>8) || get_typex(txt_app,specl->o) == X_RADCHKUND) )
  {
    specl->ind = -1;
    for( i=num=0; i<sizeof(ted_buf)-1; i++, ++*str )
      if( **str == '[' ) specl->ind = i;
      else if( (ted_buf[num++] = **str) == 0 ) break;
    *str = ted_buf;
  }
  font = ted_font( font, id, point, cw, &ch );
  if( clip && r->h <= ch ) return(-1);
  _vst_effects( effects );
  _vst_alignment( 0, 5 );
  _vst_color( color );
  return txt_extent( *str, clip, *cw, id, font, just, r, x, y );
}

void vdi_text( Rect *r, int just, int font, int id, int point, char *str, int mode, int color, int effects, TINI *specl, int clip )
{
  char c;
  int num, w, x, y, l[4];

  if( (num = text_ini( r, just, font, id, point, &str, color, effects, &w, &x, &y, specl, clip )) >= 0 )
  {
    _vswr_mode( 2-mode );
    if( gr_font==id && (font==GDOS_MONO || font==GDOS_PROP) ||
        *is_scalable )
    {
      _vst_charmap( 0 );
      text_2_arr( str, &num );
      if( gr_font==id && font==GDOS_PROP ) v_ftext16( vdi_hand, x, y, text_arr, num );
      else ftext16_mono( x, y, num );
      _vst_charmap( 1 );
    }
    else
    {
      c = *(str+num);
      *(str+num) = '\0';
      v_gtext( vdi_hand, x, y, str );
      *(str+num) = c;
    }
    if( specl && specl->ind>=0 && !(specl->o->ob_state&DISABLED) &&
        ((specl->o->ob_statex&((X_MAGMASK|X_KBD_EQUIV)>>8)) == ((X_MAGIC|X_KBD_EQUIV)>>8) ||
        get_typex(txt_app,specl->o) == X_RADCHKUND) )
    {
      set_line( 2, 1, color );
      l[2] = (l[0] = x+specl->ind*w) + w-1;
      l[3] = l[1] = y + (font!=IBM ? 6 : char_h-1  /* Neo 002 (char_h>10?char_h-2:char_h-1)*/);
      pline_2(l);
    }
  }
}

void format_ted( TEDINFO *ted )
{
  char *str, *ptr, *dat, *out;
  int i;

  if( *(dat=ted->te_ptext) == '@' ) dat="";
  out=ted_buf;
  if( (ptr=ted->te_ptmplt) > (char *)0x2000L )	/* help WP_START look right */
    for( i=0; i<sizeof(ted_buf)-1 && *ptr; i++, ptr++ )
      *out++ = *ptr=='_' ? (*dat ? *dat++ : '_') : *ptr;
  *out++ = '\0';
}

void inner( int i, /*int state,*/ Rect *r, Rect *r2 )
{
  *r2 = *r;
  if( i > 0 )
  {
    r2->x += i;
    r2->y += i;
    r2->w -= i<<1;
    r2->h -= i<<1;
  }
}

void round_seg( char *x, char *o )
{
  int temp[4], *t, l;

  for( t=temp, l=4; --l >= 0; )
    *t++ = oarray[*x++] + *o++;
  pline_2( temp );
}

void rounded( int thick, int col, int one )
{
  int k;
  static char xr[] = { 0, 1, 2, 1,  4, 3, 4, 5,  2, 7, 0, 7,  8, 5, 8, 3 },
             off[] = { 2, 0,-2, 0,  0, 2, 0,-2, -2, 0, 2, 0,  0,-2, 0, 2 };
  static int corn[4][3][3] = { 0, 0, 0x2000,  0, 0x6000, 0x6000,
      0x6000, 0xe000, 0xe000,  0, 0, 0x8000,  0, 0xc000, 0xc000,
      0xc000, 0xe000, 0xe000,  0x8000, 0, 0,  0xc000, 0xc000, 0,
      0xe000, 0xe000, 0xc000,  0x2000, 0, 0,  0x6000, 0x6000, 0,
      0xe000, 0xe000, 0x6000 };
  char *x, *o;
  int temp[2], *t;

  thick = abs(thick);
  if( --thick>2 )
  {
    while( --thick>=0 )
    {
      rounded( 2, col, one );
      oarray[1]+=2;
      oarray[8]+=2;
      oarray[4]-=2;
      oarray[7]-=2;
      if( one ) for( t=oarray, k=8; --k>=0; (*t++)++ );
      else pline_inout(-1);
    }
    return;
  }
  _vsl_color( temp[0] = col );
  temp[1] = 0;
  if( one )
      (*drw_image)( corn[2][thick], oarray[4]-1, oarray[5]-1, 2, 3, 1, 2, temp, 0, 0 );
  else
    for( t=oarray+6, k=4; --k>=0; t-=2 )
      (*drw_image)( corn[k][thick], *t-1, *(t+1)-1, 2, 3, 1, 2, temp, 0, 0 );
  do
  {
    if( !one )
      for( x=xr, o=off, k=4; --k>=0; x+=4, o+=4 )
        round_seg( x, o );
    else
    {
      round_seg( xr+4, off+4 );
      round_seg( xr+8, off+8 );
    }
    oarray[1]--;
    oarray[8]--;
    oarray[4]++;
    oarray[7]++;
  }
  while( --thick>=0 );
}

int _obj_draw( OBJECT *tree, int this, int depth, int xclip,
    int yclip, int wclip, int hclip )
{
  return _objc_draw( (OBJECT2 *)tree, curapp, this, depth, xclip, yclip, wclip,
      hclip );
}

int get_state( OBJECT2 *o, int *state, int *magic, int *effects,
    unsigned long *l )
{
  OB_PREFER op;

  *effects = 0;
  *state = o->ob_state|(o->ob_statex<<8);
  *magic = (*state&X_MAGMASK) == X_MAGIC;
  if( *magic )
  {
    *effects = ((o->ob_flags&X_ITALICS)>>13) | ((o->ob_flags&X_BOLD)>>14);
    if( *state&X_PREFER )
    {
      if( o->ob_next<0 ) op = settings->color_root;
      else if( *state&X_DRAW3D ) op = settings->color_3D;
      else if( o->ob_flags&EXIT ) op = settings->color_exit;
      else op = settings->color_other;
      *l = op.l & 0xFFFFL;
      *state = *state&~(SHADOWED|OUTLINED|X_ROUNDED);
      if( op.s.shadowed ) *state |= SHADOWED;
      if( op.s.outlined ) *state |= OUTLINED;
      if( op.s.draw_3D ) *state |= X_DRAW3D;
      if( op.s.rounded ) *state |= X_ROUNDED;
      return 1;
    }
  }
  return 0;
}

void dflt_clip_ini( Rect *r )
{
  dcl_ret = 1;
  _vs_clip( r && r->w && r->h, r );  /* turn clipping on/off */
  _v_mouse(0);
}

int dflt_clip(void)
{
  if( dcl_ret )
  {
    dcl_ret=0;
    return 1;
  }
  return 0;
}

void reset_clip(void)
{
  clip_ini = dflt_clip_ini;
  clip_it = dflt_clip;
}

char get_typex( APP *ap, OBJECT2 *o )
{
  if( !ap || ap->flags.flags.s.special_types ) return o->ob_typex;
  return 0;
}

static void draw_icon( ICONBLK *iconblk, CICON *cicn, int ob_x, int ob_y, int state )
{
	int 	m_mode, i_mode, mskcol, icncol, planes;
	int     *mask, *data, *dark = 0L;
	int     mindex[2], iindex[2], buf;
	char    invert = 0, selected;
	Rect r2;

	selected = state & SELECTED;

        r2.x = ob_x+iconblk->ib_xicon+1;
        r2.y = ob_y+iconblk->ib_yicon+1;
        r2.w = (iconblk->ib_wicon+7)>>3;
        r2.h = iconblk->ib_hicon;

        m_mode  = MD_TRANS;

        if( !cicn )
        {
          if( selected ) invert = 1;
          mask = iconblk->ib_pmask;
          data = iconblk->ib_pdata;
          planes = 1;
 	  i_mode = MD_TRANS;
        }
	else if (selected) /* it was an objc_change */
	{
		planes = cicn->num_planes;
		if ( cicn->sel_data )
		{
			mask = cicn->sel_mask;
			data = cicn->sel_data;
			if (planes > 1)
			{	if (planes > 8)	/* TrueColor, bzw RGB-orientierte Grafikkarte? */
					i_mode = S_AND_D;
				else
					i_mode = S_OR_D;
			}
			else
				i_mode = MD_TRANS;
		}
		else
		{
			mask = cicn->col_mask;
			data = cicn->col_data;

			if (planes > 1)
			{	if (planes > 8)
					i_mode = S_AND_D;
				else
					i_mode = S_OR_D;
				dark = cicn->sel_mask;
			}
			else
				invert = 1;
		}
	}
	else
	{
		planes = cicn->num_planes;
		mask = cicn->col_mask;
		data = cicn->col_data;

		if (planes > 1)
		{	if (planes > 8)
				i_mode = S_AND_D;
			else
				i_mode = S_OR_D;
		}
		else
			i_mode = MD_TRANS;
	}

	mindex[0] = !cicn || ((iconblk->ib_char & 0x0f00) != 0x0100) ?
	    (iconblk->ib_char & 0x0f00) >> 8 : WHITE;
	mindex[1] = WHITE;

	icncol = iindex[0] = (int)(((unsigned int)iconblk->ib_char & 0xf000U) >> 12U);
	iindex[1] = WHITE;

	mskcol = (iconblk->ib_char & 0x0f00) >> 8;

	if (invert)
	{
		buf       = iindex[0];
		iindex[0] = mindex[0];
		mindex[0] = buf;
		i_mode    = MD_TRANS;
	}
	if (selected)
	{
		buf    = icncol;
		icncol = mskcol;
		mskcol = buf;
	}

	(*drw_image) (mask, Xrect(r2), 1, m_mode, mindex, 0, 0);
	(*drw_image) (data, Xrect(r2), planes, i_mode, iindex, 0, 0);

	if (dark)
	{
		mindex [0] = BLACK;
		mindex [1] = WHITE;
		(*drw_image) (dark, Xrect(r2), 1, MD_TRANS, mindex, 0, 0);
	}
}

void xor_col( unsigned int *col )
{
  static char colx[] = { 1, 0, 13, 15, 14, 10, 12, 11, 9, 8, 5, 7, 6, 2, 4, 3 };

  *col = (*col&0xF0F0) | colx[*col&15] | (colx[(*col>>8)&15]<<8);
}

void at_adj( OBSPEC spec, Rect *r )
{
  int i;
  
  if( (i=spec.obspec.framesize) < 0 ) i = 0;
  if( i > 1 ) i = 1; /* 003 */
  if( r )
  {
    r->x -= add3d_h-i;
    r->y -= add3d_v-i;
    r->w += (add3d_h-i)<<1;
    r->h += (add3d_v-i)<<1;
  }
}

void col3d( unsigned int *col, unsigned int new )
{
  if( col )
  {
    if( !(*col&0x7f) ) *col |= new&0x7f;
    if( !(*col&0xf00) ) *col |= new&0xf00;
  }
}

long get_frame( OBJECT2 *tree )
{
  TEDINFO *ted;
  
  switch( tree->ob_type )
  {
    case G_BUTTON:
    case G_USERDEF:
      return but_spec((OBJECT *)tree);
    case G_FTEXT:
    case G_FBOXTEXT:
    case G_TEXT:
    case G_BOXTEXT:
      ted = (TEDINFO *)get_spec((OBJECT *)tree);
      return ((long)ted->te_thickness<<16L) | (unsigned int)ted->te_color;
    case G_BOX:
    case G_IBOX:
    case G_BOXCHAR:
      return get_spec((OBJECT *)tree);
    default:
      return 0L;
  }
}

int _adj_atari3d( int type, OBJECT2 *o, Rect *r, OBDESC *od )
{
  int j;
  char move_ok;
  unsigned int *col = &od->atari_col;
  OBSPEC spec;

  spec.index = get_frame(o);
  switch( o->ob_type )
  {
    case G_BOX:
    case G_BOXCHAR:
    case G_BOXTEXT:
    case G_FTEXT:
    case G_TEXT:
    case G_FBOXTEXT:
      *col = (int)spec.index;
      break;
    case G_TITLE:
    case G_STRING:
    case G_BUTTON:
    case G_IBOX:
      *col = 0x1000;
      break;
    default:
      return 0;
  }
  move_ok = (j=get_typex(txt_app,o)) != X_RADCHKUND &&
      j != X_UNDERLINE && j != X_GROUP;
  switch( type )
  {
    case FL3DIND:
      od->atari_move = move_ok && ind_move;
      col3d( col, ind_col );
      if( ind_change && o->ob_state&SELECTED ) xor_col(col);
      if( move_ok ) at_adj( spec, r );
      return 1;
    case FL3DACT:
      od->atari_move = move_ok && act_move;
      col3d( col, act_col );
      if( act_change && o->ob_state&SELECTED ) xor_col(col);
      if( move_ok ) at_adj( spec, r );
      return 1;
    case FL3DBAK:
      if( o->ob_type!=G_BOX && o->ob_type!=G_BOXCHAR &&
	  o->ob_type!=G_BOXTEXT && o->ob_type!=G_FTEXT &&
	  o->ob_type!=G_TEXT && o->ob_type!=G_FBOXTEXT ) return 0;
      col3d( col, bkgrnd_col );
      *col &= 0xff7f;	/* make text transparent */
      return 2;
  }
  return 0;
}

int get_obdesc( OBJECT2 *o, Rect *r, OBDESC *od )
{
  OB_PREFER op;
  int i;

  od->atari_move=0;
  od->atari3d=0;
  if( (i=o->ob_flags&FL3DMASK) != 0 &&
      (!txt_app || txt_app->flags.flags.s.special_types) )
      od->atari3d = _adj_atari3d( i, o, r, od );
  od->effects = 0;
  od->state = o->ob_state|(o->ob_statex<<8);
  od->magic = (od->state&X_MAGMASK) == X_MAGIC;
  if( od->magic )
  {
    od->effects = ((o->ob_flags&X_ITALICS)>>13) | ((o->ob_flags&X_BOLD)>>14);
    if( od->state&X_PREFER )
    {
      if( o->ob_next<0 ) op = settings->color_root;
      else if( od->state&X_DRAW3D ) op = settings->color_3D;
      else if( o->ob_flags&EXIT ) op = settings->color_exit;
      else op = settings->color_other;
      od->color.l = op.l & 0xFFFFL;
      od->state = od->state&~(SHADOWED|OUTLINED|X_ROUNDED);
      if( op.s.shadowed ) od->state |= SHADOWED;
      if( op.s.outlined ) od->state |= OUTLINED;
      if( op.s.atari_3D )	/* 003 */
      {
	if( !od->atari3d ) od->atari3d =
	    _adj_atari3d( o->ob_next<0 ? FL3DBAK : FL3DACT, o, 0L, od );
	od->state |= X_DRAW3D;
      }
      else if( op.s.draw_3D ) od->state |= X_DRAW3D;
      if( op.s.rounded ) od->state |= X_ROUNDED;
      if( op.s.shadow_text )
      { 	/* 003 */
	od->state |= X_SHADOWTEXT;
	if( op.s.bold_shadow ) od->effects |= 1;	/* bold */
      }
      return 1;
    }
  }
  return 0;
}

void rec3d( bfobspec color )
{
  unsigned int i=color.interiorcol;

  pline_inout(1);
  _vsl_color(0);
  oarray[4] = oarray[0];
  oarray[5] = oarray[1];
  v_pline( vdi_hand, 3, oarray+2 );
  if( vplanes>2 )
  {
    xor_col( &i );
    _vsl_color(i&0xF);
    oarray[4] = oarray[2];
    oarray[5] = oarray[7];
    v_pline( vdi_hand, 3, oarray+2 );
  }
  pline_inout(-1);
}

void fill_col( bfobspec c, Rect *r )
{
  vdi_box( c.interiorcol, c.fillpattern ? 2 : 0, c.fillpattern < 7 ?
      c.fillpattern : 8, 1, r );
}

int do_userdef( OBJECT2 *tree, int item, int state, Rect *r, OBSPEC spec )
{
  int ret;
  PARMBLK p;

  p.pb_tree = (OBJECT *)tree;
  p.pb_obj = item;
  p.pb_prevstate = use_oldstate ? old_state : state;
  p.pb_currstate = state;
  inner( 1, r, (Rect *)&p.pb_x );
  p.pb_xc = clip_rect.x;
  p.pb_yc = clip_rect.y;
  p.pb_wc = clip_rect.w;
  p.pb_hc = clip_rect.h;
  p.pb_parm = spec.userblk->ub_parm;
  ret = userdef( spec.userblk->ub_code, &p );
  _vs_clip( 1, &clip_rect );
  return ret;
}

void gray_menu( OBJECT2 *o, OBDESC *od, Rect *r, char *s, char col )	/* 003 */
{
  int w, x, y, xp, arr[4];
  char *out;

  if( vplanes>=4 && col>1 )
      set_line( 1, 1, od->color.b.textcol = col );
  else set_line( 1, 1, 1 );
  if( od->magic && o->ob_statex&(X_SMALLTEXT>>8) ) w = 6;
  else w = char_w;
  y = r->y + (r->h>>1);
  for( out=ted_buf, x=xp=0; ; x++, s++, out++ )
    if( *s != '-' )
    {
      if( xp != x )
      {
        arr[0] = xp*w + r->x + 2;
        arr[1] = arr[3] = y;
        arr[2] = x*w + r->x - 1;
        pline_2(arr);
        if( r->h>=16+2 )
        {
          arr[1] = ++arr[3];
          pline_2(arr);
        }
      }
      xp = x+1;
      if( (*out = *s) == 0 ) break;
    }
    else *out = ' ';
}

int _objc_draw( OBJECT2 *tree, APP *ap, int this, int depth0, int xclip,
    int yclip, int wclip, int hclip )
{
  int parent, odx, ody, font, just, id, point, i, j, k, start,
      **dat, depth, cols[2] = { 0, 0 };
  long l, numic, icsiz;
  char *str, round, round_pr, prefer, typex, round_buttons,
      graymenu, gray;
  OBJECT2 *o;
  CICON *ci;
  OBSPEC spec;
  APP *of;
  TINI ti;
  Rect r, r2, rold, clip;
  OBDESC od;
  static char tmp[2]="\0";

  bkgrnd_col = act_col = ind_col = 0x1180 | (settings->color_root.l&0x7f);	/* 004 */
  round_buttons = ap ? ap->flags.flags.s.round_buttons : 1;
  graymenu = tree[0].ob_typex == X_GRAYMENU ? settings->graymenu : 0;	/* 003 */
  of = form_app;
  txt_app = form_app = ap;
  if( !wclip ) wclip = 32767/2;
  if( !hclip ) hclip = 32767/2;
  clip.x=xclip;
  clip.y=yclip;
  clip.w=wclip;
  clip.h=hclip;
  if( !gintersect( desktop->outer, clip, &clip ) ) return(0);
  (*clip_ini)( &clip );
  start = this;
  while( (*clip_it)() )
  {
    parent = this = start;
    depth = depth0;
    if( !this )
    {
      odx = 0;
      ody = 0;
    }
    else
    {
      objc_offset( (OBJECT *)tree, this, &odx, &ody );
      odx -= u_object((OBJECT *)tree,parent)->ob_x;
      ody -= u_object((OBJECT *)tree,parent)->ob_y;
    }
    while (this >= 0)
      if (tree[this].ob_tail != parent)
      {
	parent = this;	  /* This is a new node       */
	this = -1;
	odx += tree[parent].ob_x;
	ody += tree[parent].ob_y;
	if( depth-->=0 && !(tree[parent].ob_flags&HIDETREE) )
	{
	  o = &tree[parent];
	  r.x = odx-1;
	  r.y = ody-1;
	  r.w = o->ob_width+2;
	  r.h = o->ob_height+2;
	  prefer = get_obdesc( o, &r, &od );
	  if( o->ob_type==G_STRING || gintersect( clip, r, &r2 ) )
	  {
	    font = IBM;
	    id = 1;
	    point = 0;
	    round = 0;
	    spec.index = get_spec( (OBJECT *)o );
	    ti.o = o;
	    gray = 0;
	    round_pr = round_buttons && od.magic && od.state&X_ROUNDED;
	    typex = get_typex(ap,o);
	    if( od.magic && typex==X_USRDEFPRE ) od.state =
		do_userdef( tree, parent, od.state, &r, o->ob_spec );
	    switch( o->ob_type )
	    {
	      case G_BOX:
		tmp[0] = '\0';
		goto box;
	      case G_BOXCHAR:
		tmp[0] = spec.obspec.character;
		just = 2;
box:		if( !prefer ) od.color.b = spec.obspec;
		else
		{
		  od.color.b.character = spec.obspec.character;
		  od.color.b.framesize = spec.obspec.framesize;
		}
		str = tmp;
		goto drawbox;
	      case G_BOXTEXT:
		str = spec.tedinfo->te_ptext;
		goto drawboxt;
	      case G_BUTTON:
		if( !prefer ) od.color.l = but_spec((OBJECT *)o);
		else od.color.l |= but_spec((OBJECT *)o)&0xFF0000L;
		just = 2;
		str = spec.free_string;
		if( typex==X_MOVER )
		{
		  i = char_h<16 ? 8 : 15;
		  j = r.y+((r.h-i)>>1)+(char_h>=16);
		  cols[0] = 1;
                  (*drw_image)( char_h<16 ? RSBB5DATA : RSBB4DATA, r.x+1, j, 2,
                      i, 1, 2, cols, 0, 0 );
		  break;
		}
		if( typex==X_UNDERLINE || typex==X_RADCHKUND &&
		    !(o->ob_flags&EXIT) ) goto string;
		if( typex==X_GROUP )
		{
		  inner( 1, &r, &r2 );
		  r2.y--;
		  r2.h++;
		  if( od.atari3d ) od.color.l = (od.color.l&0xFFFF0000L) | bkgrnd_col;
		  fill_col( od.color.b, &r2 );
		  rold = r;
		  set_line( 1, 1, od.color.b.framecol );
		  i = char_h>>1;
		  to_larr( r.x, r.y+i, r.w, r.h-i );
		  pline_inout(1);
		  pline_5();
		  if( od.atari3d )
		  {
		    rec3d( od.color.b );
		    _vsl_color( od.color.b.framecol );
		    oarray[5] = oarray[7] = (oarray[1] = oarray[9] = oarray[3] -= i) + i;
		    oarray[0]++;
		    oarray[9]++;
		    pline_5();
		    oarray[0]--;
		    oarray[9]--;
		    rec3d( od.color.b );
		    to_larr( Xrect(r) );
		    rounded( 1, od.color.b.framecol, 1 );
		    od.color.b.textmode=0;
		  }
		  else od.color.b.textmode=1;
		  r.x += char_w;
		  r.y++;
		  if( (r.w -= char_w<<1) < 2 ) r.w = 2;
		  r.h = char_h;
		  if( od.atari3d )
		  {
		    r.y += 2;
		    i = (od.magic && o->ob_statex&(X_SMALLTEXT>>8)) ? 6 : char_w;
		    r.w = strlen(spec.free_string)*i + 3;
		    r.x--;
		    fill_col( od.color.b, &r );
		    r.x++;
		    od.atari3d = 0;
		  }
		  goto string;
		}
		if( !od.magic && round_buttons && (o->ob_flags&(SELECTABLE|TOUCHEXIT|EXIT))==
		    (SELECTABLE|EXIT) && !(od.state&(SHADOWED|OUTLINED|X_DRAW3D)) && !od.atari3d ) round++;
		goto drawbox;
	      case G_FTEXT:
		format_ted( spec.tedinfo );
		str = ted_buf;
		goto ftext;
	      case G_FBOXTEXT:
		format_ted( spec.tedinfo );
		str = ted_buf;
drawboxt:	od.color.l = ((long)spec.tedinfo->te_thickness<<16);
		if( !prefer ) od.color.l |= (unsigned)spec.tedinfo->te_color;
		just = spec.tedinfo->te_just;
		font = spec.tedinfo->te_font;
		id = spec.tedinfo->te_junk1;
		point = spec.tedinfo->te_junk2;
drawbox:	if( od.atari3d ) od.color.l = od.color.l&0xFFFF0000L | od.atari_col;
/*		    (od.color.l&(od.atari3d==2||vplanes<2||o->ob_type==G_BOXCHAR||
		    o->ob_type==G_BUTTON?0xFFFF0000L:0xFF000000L)) | od.atari_col;  003 */
		inner( od.color.b.framesize, &r, &r2 );
		fill_col( od.color.b, &r2 );
drawframe:	if( (j=od.color.b.framesize) != 0 )
		{
		  set_line( 1, 1, od.color.b.framecol );
		  to_larr( Xrect(r) );
		  if( j<0 && o->ob_type==G_IBOX )
		  {
		    j--;
		    pline_inout(1);
		  }
		  if( round_pr || round )
		  {
		    if( round_pr && od.state&OUTLINED )
		    {
		      rounded( 3, 0, 0 );	/* white box */
		      oarray[1]+=3;
		      oarray[8]+=3;
		      oarray[4]-=3;
		      oarray[7]-=3;
		      pline_inout(-2);
		    }
		    else if( j>0 ) pline_inout(j);
		    rounded( j, od.color.b.framecol, 0 );
		  }
		  else if( od.atari3d!=2 || !(od.state&OUTLINED) || j>0 )  /* 003 */
		  {
		    if( j>0 && od.atari3d!=1 ) pline_inout(1);
		    i = j<0 ? -1 : 1;
		    for( j=abs(j); --j>=0; )
		    {
		      pline_5();
		      pline_inout(i);
		    }
		  }
		}
		goto drawtext;
	      case G_IBOX:
		if( !prefer ) od.color.b = spec.obspec;
		else od.color.b.framesize = spec.obspec.framesize;
		od.color.b.fillpattern = 0;
		tmp[0] = '\0';
		str = tmp;
		goto drawframe;
	      case G_CICON:
		if( !spec.iconblk ) break;
		icsiz = spec.iconblk->ib_hicon * (long)((spec.iconblk->ib_wicon+15)>>4<<1);
		numic = *(long *)(l = spec.index + sizeof(ICONBLK));
		l += icsiz + icsiz + 12 + sizeof(long);
		ci = 0L;
		while( numic-->0 )
		{
		  if( ((CICON *)l)->next_res )
		  {
		    ci = (CICON *)l;
		    break;
		  }
		  i = ((CICON *)l)->num_planes + 1;
		  if( ((CICON *)l)->sel_data ) l+=icsiz*i;
		  l += 5*sizeof(long)+sizeof(int)+icsiz*i;
		}
		if( ci )
		{
		  draw_icon( spec.iconblk, ci, r.x, r.y, od.state );
		  goto icon_text;
		}	/* fall through if none fits */
	      case G_ICON:
		draw_icon( spec.iconblk, 0L, r.x, r.y, od.state );
icon_text:	i = ((unsigned)spec.iconblk->ib_char>>12)&0xF;
		j = ((unsigned)spec.iconblk->ib_char>>8)&0xF;
		if( spec.iconblk->ib_ptext[0] )
		{
		  r2.x = r.x+spec.iconblk->ib_xtext;
		  r2.y = r.y+spec.iconblk->ib_ytext;
		  r2.w = spec.iconblk->ib_wtext+2;
		  r2.h = spec.iconblk->ib_htext+2;
		  vdi_box( (od.state&SELECTED) ? i : j, 2, 8, 1, &r2 );
		}
		vdi_text( &r2, 2, SMALL, 1, 0, spec.iconblk->ib_ptext, 0,
		    (od.state&SELECTED)?j:i, od.effects, &ti, 0 );
		if( (tmp[0]=spec.iconblk->ib_char) != 0 )
		{
		  r2.x = r.x+spec.iconblk->ib_xicon+spec.iconblk->ib_xchar;
		  r2.y = r.y+spec.iconblk->ib_yicon+spec.iconblk->ib_ychar;
		  vdi_text( &r2, 0, SMALL, 1, 0, tmp, 0, (od.state&SELECTED) ? j : i,
		      od.effects, &ti, 0 );
		}
		od.state &= ~SELECTED;
		od.color.l = 0L;
		break;
	      case G_IMAGE:
		cols[0] = spec.bitblk->bi_color;
		cols[1] = 0;
                (*drw_image)( spec.bitblk->bi_pdata, r.x+1, r.y+1,
                    spec.bitblk->bi_wb, spec.bitblk->bi_hl, 1, 2, cols,
                    spec.bitblk->bi_x, spec.bitblk->bi_y );
		od.color.l = 0L;
		break;
	      case G_USERDEF:
		od.state = do_userdef( tree, parent, od.state, &r, spec );
		od.color.l = 0x00001170L;
		break;
	      case G_TEXT:
		str = spec.tedinfo->te_ptext;
ftext:		od.color.l = spec.tedinfo->te_color;
		just = spec.tedinfo->te_just;
		font = spec.tedinfo->te_font;
		id = spec.tedinfo->te_junk1;
		point = spec.tedinfo->te_junk2;
		if( od.atari3d == 2 )
		{
		  od.color.l = 0L;
		  goto drawbox;
		}
		goto drawtext;
	      case G_STRING:
		od.color.l = 0x00001100L;
		goto graystring;
	      case G_TITLE:
		od.color.l = 0x00011100L;
graystring:	if( graymenu && od.state&DISABLED )	/* 003 */
		{
		  gray_menu( o, &od, &r, spec.free_string, graymenu );
		  if( vplanes>=4 && graymenu>1 ) gray = 1;
		  str = ted_buf;
		  just = 0;
		  goto drawtext;
		}
string: 	just = 0;
		str = spec.free_string;
		if( typex==X_UNDERLINE )
		{
		  if( od.atari3d )
		  {
		    od.color.l = (od.color.l&0xFFFF0000L) | bkgrnd_col;
		    fill_col( od.color.b, &r );
		    od.atari3d = 0;
		    od.color.b.textmode = 0;
		  }
		  else od.color.b.textmode = 1;
		  set_line( 1, 1, od.color.b.framecol );
		  to_arr( Xrect(r) );
		  oarray[1] = oarray[3];
		  if( !settings->flags.s.long_titles ) oarray[2] =
		      oarray[0]+strlen(str)*char_w;
		  pline_2(oarray);
		  od.state &= ~SELECTED;
		}
drawtext:	if( od.magic && o->ob_statex&(X_SMALLTEXT>>8) ) font=SMALL;
		if( (o->ob_type!=G_STRING || o->ob_flags&RBUTTON) && typex==X_RADCHKUND &&
		    !(o->ob_flags&EXIT) && !(o->ob_state&SHADOWED) )
		{
		  if( char_h<16 || od.atari3d )
		    if( !(od.state&DISABLED) )
		    {
		      radio->monoblk.ib_char = check->monoblk.ib_char = ((ind_col&0xF00)<<4);
		      i = o->ob_flags&RBUTTON;
		      j = r.y + ((r.h - (char_h<16 ? 10 : 18))>>1);	/* 003 */
		      draw_icon( i?&radio->monoblk:&check->monoblk,
			  i?radio->mainlist:check->mainlist, r.x, j, od.state );
		    }
		    else
		    {
		      od.state &= ~DISABLED;
		      od.effects |= 2;
		    }
		  else
		  {
		    dat = rbdat[o->ob_flags&RBUTTON ? 0 : 1];
		    j = r.y+((r.h-13)>>1);
		    if( !(od.state&SELECTED) )
		    {
		      cols[0] = 0;
                      (*drw_image)( dat[1], r.x+1, j, 2, 15, 1, 2, cols, 0, 0 );
		    }
		    cols[0] = 1;
                    (*drw_image)( dat[od.state&SELECTED], r.x+1, j, 2, 15, 1, 2, cols, 0, 0 );
		  }
		  od.atari3d = 0;
		  r.x += 3*8;
		  od.state &= ~SELECTED;
		  i=1;
		}
		else i=0;
		if( (j=od.color.b.framesize) > 0 && od.atari3d==1 ) j-=2;
		inner( j, &r, &r2 );
		if( od.magic && (od.state&(SELECTED|X_DRAW3D)) == (SELECTED|X_DRAW3D) ||
		    od.atari_move && od.state&SELECTED )
		{
		  r2.x++;
		  r2.y++;
		}
	        if( o->ob_type==G_TITLE )	/* 003 */
	        {
	          r2.h--;
	          k = 0;
	        }
		else k = o->ob_type!=G_STRING && o->ob_type!=G_BUTTON;
		if( od.atari3d==2 && od.state&DISABLED ) od.effects |= 2;
		if( od.magic && od.state&X_SHADOWTEXT && (!od.color.b.textcol ||
		    od.color.b.interiorcol && od.color.b.fillpattern) )
		{	/* 003 */
		  j = r2.x;
		  if( od.effects&1 ) r2.x+=2;
		  else r2.x++;
		  r2.y++;
		  vdi_text( &r2, just, font, id, point, str, od.color.b.textmode, od.color.b.textcol,
		      od.effects, &ti, k );
		  od.color.b.textcol = od.color.b.textcol ? 0 : 1;
		  r2.x = j;
		  r2.y--;
		}
		vdi_text( &r2, just, font, id, point, str, od.color.b.textmode, od.color.b.textcol,
		    od.effects, &ti, k );
		if(i) r.x -= 3*8;
		if( typex==X_GROUP ) r = rold;
		break;
	      default:
		DEBUGGER(OBDRAW,UNKTYPE,o->ob_type);
		break;
	    }
	    if( od.magic && typex==X_USRDEFPOST ) od.state =
		do_userdef( tree, parent, od.state, &r, o->ob_spec );
	    if( od.state&CROSSED )
	    {
	      set_line( 1, 1, 0 );
	      inner( od.color.b.framesize+1, &r, &r2 );
	      to_arr( Xrect(r2) );
	      pline_2(oarray);
	      i = oarray[1];
	      oarray[1] = oarray[3];
	      oarray[3] = i;
	      pline_2(oarray);
	    }
	    if( od.state&CHECKED )
	    {
	      r2 = r;
	      r2.x += 2;	/* 003: was ++ */
	      r2.y++;		/* 003 */
	      r2.h = char_h;
	      vdi_text( &r2, 0, IBM, 1, 0, "", 0, gray?graymenu:1/*003*/, 0, 0L, 0 );
	    }
	    if( od.state&OUTLINED )
	    {
	      to_larr( Xrect(r) );
	      if( round_pr )
	      {
		set_line( 1, 1, BLACK );
		rounded( 1, BLACK, 0 );
	      }
	      else
	      {
		set_line( 1, 1, WHITE );
		pline_5();
		pline_inout(-1);
		pline_5();
		pline_inout(-1);
		if( od.atari3d == 2 )
		{
		  xor_col( &od.atari_col );
		  _vsl_color( od.atari_col&0xf );
		  to_larr( Xrect(r) );
		  v_pline( vdi_hand, 3, oarray+2 );
		  pline_inout(-1);
		  v_pline( vdi_hand, 3, oarray+2 );
		  pline_inout(-1);
		}
		_vsl_color( 1 );
		if( round ) rounded( 1, BLACK, 0 );
		else pline_5();
	      }
	    }
	    if( od.state&SHADOWED )
	    {
	      set_line( 1, 1, od.color.b.framecol );
	      j = od.color.b.framesize+1;
	      i = od.color.b.framesize<0 ? 1 : j;
	      to_larr( r.x+i, r.y+i, r.w-j, r.h-j );
	      j = abs(od.color.b.framesize)<<1;
	      if( round_pr ) rounded( j, od.color.b.framecol, 1 );
	      else
		while( --j>=0 )
		{
		  v_pline( vdi_hand, 3, oarray+2 );
		  oarray[2]++;
		  oarray[4]++;
		  oarray[5]++;
		  oarray[7]++;
		}
	    }
	    if( od.magic && od.state&X_DRAW3D || od.atari3d==1 )
	    {
	      inner( od.color.b.framesize, &r, &r2 );
	      to_larr( Xrect(r2) );
	      if( od.atari3d )
	      {
		if( od.color.b.framesize<=0 ) pline_inout(1);
		set_line( 1, 1, 0 );
		if( !(od.state&SELECTED) )
		{
		  oarray[4] = oarray[0];
		  oarray[5] = oarray[1];
		}
		v_pline( vdi_hand, 3, oarray+2 );
		if( od.state&SELECTED )
		{
		  oarray[4] = oarray[0];
		  oarray[5] = oarray[1];
		  i = 1;
		}
		else
		{
		  oarray[4] = oarray[2];
		  oarray[5] = oarray[7];
		  if( vplanes>1 )
		  {
		    xor_col( &od.atari_col );
		    i = od.atari_col&0xf;
		  }
		  else i = 1;
		}
		_vsl_color(i);
	      }
	      else
	      {
		pline_inout(1);
		set_line( 1, 1, od.color.b.framecol );
		if( !(od.state&SELECTED) )
		{
		  v_pline( vdi_hand, 3, oarray+2 );
		  _vsl_color( 0 );
		}
		oarray[4] = oarray[0];
		oarray[5] = oarray[1];
	      }
	      v_pline( vdi_hand, 3, oarray+2 );
	      od.state &= ~SELECTED;
	    }
	    if( od.state&DISABLED && od.atari3d!=2 && !gray/*003*/ )
	    {
	      inner( od.color.b.framesize, &r, &r2 );
	      vdi_box( 0, 2, 4, 2, &r2 );
	    }
	    if( od.state&SELECTED )
	    {
	      inner( od.color.b.framesize, &r, &r2 );
	      vdi_box( 1, 1, 0, 3, &r2 );
	    }
	    if( depth>=0/*003*/ ) this = tree[parent].ob_head;
	  }
	  /* else not visible, go to sibling */
	}
	if (this == -1)
	{
	  if( parent == start ) break;
	  this = tree[parent].ob_next;
	  odx -= tree[parent].ob_x;
	  ody -= tree[parent].ob_y;
	  depth++;
	}
      }
      else	  /* revisit parent */
      {
	parent = this;
	if( parent == start ) break;
	this = tree[parent].ob_next;
	odx -= tree[parent].ob_x;
	ody -= tree[parent].ob_y;
	depth++;
      }
  }	/* end clip loop */
  _v_mouse(1);
  txt_app = 0L;
  form_app = of;
  return(1);
}

int objc_offset( OBJECT *tree, int obj, int *x, int *y )
{
  int xy[2];

  goffset_objc( tree, obj, xy );
  *x = xy[0];
  *y = xy[1];
  return 1;
}

int goffset_objc( OBJECT *tree, int obj, int *xy )
{
  register int parent=1, lastobj;

  xy[1] = xy[0] = 0;
  do
  {
    if( parent )
    {
      parent=0;
      xy[0] += tree[obj].ob_x;
      xy[1] += tree[obj].ob_y;
    }
    if( tree[obj=tree[lastobj=obj].ob_next].ob_tail == lastobj ) parent++;
  }
  while( obj >= 0 && lastobj );
  return(1);
}

void set_rect( OBJECT *tree, int this, Rect *r )
{
  tree += this;
  *r = *(Rect *)&tree->ob_x;
}

int _objc_find( OBJECT *tree, OFT *o )
{
  int ret;

  txt_app = curapp;
  ret = objc_find( tree, o->start, o->depth, o->mx, o->my );
  txt_app = 0L;
  return ret;
}

int objc_find( OBJECT *tree, int this, int depth, int mx, int my )
{
  int start, tmp1, x, y, ret=-1;
  Rect r;

  if( (start = tmp1 = this) != 0 )
  {
    objc_offset( tree, this, &x, &y );
    mx -= x-tree[this].ob_x;
    my -= y-tree[this].ob_y;
  }
  set_rect( tree, this, &r );
  if( in_rect( mx, my, &r ) )
  {
    ret = this;
    while (this >= 0)
    {
      set_rect( tree, this, &r );
      if (tree[this].ob_tail != tmp1)
      {
        tmp1 = this;    /* This is a new node       */
        this = -1;
        if( depth-->=0 && !(tree[tmp1].ob_flags&HIDETREE) &&
            in_rect( mx, my, &r ) )
	{
	  ret=tmp1;
	  if( depth>=0/*003*/ ) this = tree[ret].ob_head;
	}
        mx -= r.x;
        my -= r.y;
        if (this == -1)
        {
          if( tmp1 == start ) break;
          this = tree[tmp1].ob_next;
          mx += r.x;
          my += r.y;
          depth++;
        }
      }
      else        /* revisit parent */
      {
        tmp1 = this;
        if( tmp1 == start ) break;
        this = tree[tmp1].ob_next;
        mx += r.x;
        my += r.y;
        depth++;
      }
    }
  }
  return(ret);
}

int gfind_parent( OBJECT *tree, int parent )
{
  int lastobj;

  while( tree[parent=tree[lastobj=parent].ob_next].ob_tail != lastobj &&
      parent>=0 && lastobj );
  return(parent);
}

void obj_rect( OBJECT *tree, int obj, Rect *r )
{
  OBJECT *tree2 = tree+obj;

  goffset_objc( tree, obj, (int *)r );
  r->x--;
  r->y--;
  r->w = tree2->ob_width+2;
  r->h = tree2->ob_height+2;
}

void obj_extent( OBJECT *tree, int obj, Rect *r )
{
  obj_rect( tree, obj, r );
  adjust_rect( tree+obj, r, 1 );
}

void get_outer( OBJECT2 *tree, OBJECT2 *tree2, int obj, Rect *r, Rect *r2 )
{
  OBSPEC spec;

  obj_rect( (OBJECT *)tree, obj, r );
  spec.index = get_frame(tree2);
  inner( spec.obspec.framesize, r, r2 );
}

typedef struct { int obj, resvd; Rect clip; int new, redraw; } OCT;

#pragma warn -par
int objc_change( OBJECT *tree, int obj, int resvd, int x, int y, int w, int h,
    int new, int redraw )
{
  Rect r;
  
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return change_objc( tree, curapp, obj, &r, new, redraw );
}
#pragma warn +par

int change_objc( OBJECT *tree, APP *ap, int obj, Rect *clip, int new, int redraw )
{
  Rect r, r2;
  OBJECT2 *tree2 = (OBJECT2 *)u_object(tree,obj);
  int old, t, othstate;
  char dif;
  APP *of;
  OBDESC od;

  if( (old = tree2->ob_state) == new ) return 1;
  tree2->ob_state = new;
  if( !redraw ) return 1;
  of = form_app;
  txt_app = form_app = ap;
  get_obdesc( tree2, 0L, &od );
  txt_app = 0L;
  if( ((dif=(unsigned char)old^(unsigned char)new)&SELECTED) &&
      (t=tree2->ob_type)!=G_USERDEF && t!=G_ICON && t!=G_CICON &&
      (!od.magic || !(od.state&X_DRAW3D)) && !od.atari3d &&
      (get_typex(ap,tree2)!=X_RADCHKUND || tree2->ob_flags&EXIT) )
  {
    (*clip_ini)( clip );
    while( (*clip_it)() )
    {
      get_outer( (OBJECT2 *)tree, tree2, obj, &r, &r2 );
      vdi_box( 1, 1, 0, 3, &r2 );
    }
    _v_mouse(1);
    dif &= ~SELECTED;
  }
  if( dif&(SELECTED|CROSSED|CHECKED|DISABLED|OUTLINED|SHADOWED) ) /* 004: added & */
  {
    old_state = old | (tree2->ob_statex<<8);
    use_oldstate++;
    _objc_draw( (OBJECT2 *)tree, ap, obj, 0, Xrect(*clip) );
    use_oldstate--;
    if( form_app==ap && (od.magic && od.state&X_DRAW3D || od.atari3d) )
	drw_alt( tree, obj, 0 );
  }
  form_app = of;
  return 1;
}

int ted_off( TEDINFO *ted, int i )
{
  char *s=ted->te_ptmplt, *e;
  int j=0;

  if( (e = strrchr(s,'_')) != 0 )
  {
    for( ; i && s<=e; j++ )
      if( *s++ == '_' ) i--;
    while( s<=e && *s++ != '_' ) j++;
  }
  return(j);
}

int ted_off_w( TEDINFO *ted, int i, int w )
{
  int o, pts[8];
  char c;
  
  o = ted_off(ted,i);
  if(w) return o*w;
  if( speedo && *is_scalable )
  {
    text_2_arr( ted_buf, &o );
    _vst_charmap( 0 );
    vqt_f_extent16( vdi_hand, text_arr, o, pts );
    _vst_charmap( 1 );
  }
  else
  {
    c = ted_buf[o];
    ted_buf[o] = 0;
    vqt_extent( vdi_hand, ted_buf, pts );
    ted_buf[o] = c;
  }
  return pts[2]-pts[0];
}

void draw_curs( int x, int w, TEDINFO *ted, int idx, Rect *r, char mode )
{
  int line[4];

  line[2] = line[0] = x + ted_off_w( ted, idx, w );
  line[3] = (line[1] = r->y+((r->h-char_h)>>1)) + char_h;
  if( !mode )
  {
    line[2] += w ? w-1 : 7;
    vr_recfl( vdi_hand, line );
  }
  else
  {
    line[1] -= 2;
    line[3] += 2;
    pline_2( line );
  }
}

int ted_ok( OBJECT2 *tree2 )
{
  if( tree2->ob_type != G_FTEXT && tree2->ob_type != G_FBOXTEXT &&
      tree2->ob_type != G_TEXT && tree2->ob_type != G_BOXTEXT ||
      !(tree2->ob_flags&EDITABLE) || (tree2->ob_state&DISABLED) ) return 0;
  return 1;
}

TEDINFO *long_left( OBJECT2 *tree2, int *idx )
{
  TEDINFO *ted;
  
  if( ted_ok(tree2-1) && (ted=tree2[-1].ob_spec.tedinfo)->
      te_tmplen==X_LONGEDIT )
  {
    *idx = strlen(ted->te_ptext);
    return ted;
  }
  return 0L;
}

char long_insert( char *ptr, OBJECT2 *tree2, int off )
{
  char temp[sizeof(ted_buf)], temp2[sizeof(ted_buf)], cont;
  TEDINFO *ted;
  int j, k;
  
  for( cont=0; ; )
  {
    cont++;
    ted = (tree2++)->ob_spec.tedinfo;
    strcpy( temp2, ted->te_ptext+off );
    strcpy( ted->te_ptext+off, ptr );
    k = strlen(ptr)+off;
    if( (j = strlen(temp2))+k > ted->te_txtlen-1 ) j = ted->te_txtlen-1-k;
    if( j>=0 )
    {
      strncpy( ptr=ted->te_ptext+k, temp2, j );
      *(ptr+j) = 0;
      if( temp2[j] == 0 ) break;
      strcpy( temp, temp2 );
      ptr = temp+j;
    }
    if( ted->te_tmplen!=X_LONGEDIT ) break;
    off = 0;
  }
  return cont;
}

int __xobjc_edit( OBJECT *tree, int obj, long key, int *idx, int kind )
{
  return( _xobjc_edit( tree, curapp, obj, key, idx, kind ) );
}

int objc_edit( OBJECT *tree, int obj, int key, int *idx, int kind )
{
  return( _xobjc_edit( tree, curapp, obj, key, idx, kind ) );
}

void fake_edit( OBJECT *tree, int obj, int key, int *idx )
{
  _xobjc_edit( tree, txt_app, obj, 0, idx, 3 );
  _xobjc_edit( tree, txt_app, obj, key, idx, 2 );
  _xobjc_edit( tree, txt_app, obj, 0, idx, 3 );
}

int _xobjc_edit( OBJECT *tree, APP *ap, int obj, long key, int *idx, int kind )
{
  Rect r, r2, temp;
  OBJECT2 *tree2 = (OBJECT2 *)u_object(tree,obj);
  int w, x, y, i, j, ok, ks=key>>16, font, id, point,
      draw_s, draw_l, ret=1, obj0=obj;
  char *ptr, *ptr2, *ptr3, low, cont=0, str[2];
  TEDINFO *ted, *ted2;
  APP *of;
  void (*ini)( Rect *r );
  int (*it)(void);
  OBDESC od;

  of = form_app;
  txt_app = form_app = ap;
  for(;;)
  {
    if( !ted_ok(tree2) ) return 0;
    get_outer( (OBJECT2 *)tree, tree2, obj, &r, &r2 );
    ted = (TEDINFO *)get_spec((OBJECT *)tree2);
    font = (tree2->ob_statex&((X_MAGIC|X_SMALLTEXT)>>8)) == ((X_MAGIC|X_SMALLTEXT)>>8) ?
	SMALL : ted->te_font;
    format_ted(ted);
    ptr3 = ted_buf;
    get_obdesc( tree2, &r, &od );
    od.color.l = ted->te_color;
    if( od.atari3d != 0 ) od.color.l = (od.color.l&0xFFFF0000L) | od.atari_col;
    if( font==GDOS_PROP || font==GDOS_MONO || font==GDOS_BITM )
    {
      id = ted->te_junk1;
      point = ted->te_junk2;
    }
    else
    {
      id = 1;
      point = 0;
    }
    text_ini( &r, ted->te_just, font, id, point, &ptr3,
	od.color.b.textcol, od.effects, &w, &x, &y, 0L, 1 );
    if( gr_font!=id ) font = IBM;
    else if( font==GDOS_BITM || font==GDOS_PROP ) w = 0;
    if( od.atari3d && od.atari_move && tree2->ob_state&SELECTED )
    {
      x++;
      y++;
    }
    set_line( 3, 1, 1 );
    vdi_box( 1, 1, 0, 3, 0L );
    (*clip_ini)( 0L );
    while( (*clip_it)() ) switch( kind )
    {
      case 1:
init:	if( ted->te_ptext[0] == '@' ) ted->te_ptext[0]=0;
	*idx = strlen(ted->te_ptext);
      case 3:
	draw_curs( x, w, ted, *idx, &r, settings->flags.s.insert_mode );
	break;
      case 2:
	if( !cont && key != -1 )
	    draw_curs( x, w, ted, *idx, &r, settings->flags.s.insert_mode );
	low = key>>8;
	draw_l = 0;
	draw_s = *idx;
	switch( (char)key )
	{
	  case -1:
	    draw_l = 1000;
	    draw_s = 0;
	    break;
	  case '\033':		  /* Esc */
	    if( ks&4 ) goto dflt;
	    *idx = 0;
	    ted->te_ptext[0] = '\0';
	    draw_l = 1000;
	    draw_s = 0;
	    break;
	  case '\x7f':		  /* Del */
l_del:	    draw_l = strlen(ted->te_ptext+*idx);
	    if( ks&3 )		  /* Shift-Del */
		ted->te_ptext[*idx] = '\0';
	    else
		if( ted->te_tmplen == X_LONGEDIT && (!*(ted->te_ptext+*idx) ||
		strlen(ted->te_ptext)==ted->te_txtlen-1) )
	    {
	      draw_l = 1000;
	      ptr = ted->te_ptext;
	      if( *(ptr+*idx) ) strcpy( ptr+*idx, ptr+*idx+1 );
	      for( cont=0, i=1, ted2=ted; ; i++ )
	      {
		cont++;
		ok = ted2->te_txtlen;
		ted2 = u_tedinfo((OBJECT *)tree2,i);
		ptr += (j = strlen(ptr));
		strncpy( ptr, ted2->te_ptext, j=ok-j-1 );
		if(!j) j = 1;
		ok = strlen(ptr=ted2->te_ptext) == ted2->te_txtlen-1;
		if( j >= strlen(ptr) ) *ptr = 0;
		else strcpy( ptr, ptr+j );
		if( !ok || ted2->te_tmplen!=X_LONGEDIT ) break;
	      }
	    }
	    else if( ted->te_ptext[*idx] )
		strcpy( ted->te_ptext+*idx, ted->te_ptext+*idx+1 );
	    break;
	  case '\b':		  /* Bksp */
	    if( ks&3 )		  /* Shift-Bksp */
	    {
	      draw_l = strlen(ted->te_ptext);
	      strcpy( ted->te_ptext, ted->te_ptext+*idx );
	      draw_s = *idx = 0;
	    }
	    else if( *idx )
	    {
	      --*idx;
	      draw_s--;
	      if( ted->te_tmplen==X_LONGEDIT ) goto l_del;
	      draw_l = strlen(ted->te_ptext+*idx);
	      strcpy( ted->te_ptext+*idx, ted->te_ptext+*idx+1 );
	    }
	    else if( (ted2=long_left( tree2, idx )) != 0 )
	    {
	      obj0--;
	      ret = 2;
	      if( *idx == ted2->te_txtlen-1 ) --*idx;
	      fake_edit( tree, obj-1, '\x7f', idx );
	    }
	    break;
	  case '\x34':		  /* Shift-left */
	    if( low == 0x4b )
	    {
	      *idx=0;
	      break;
	    }
	    else goto dflt;
	  case '\x36':		  /* Shift-right */
	    if( low == 0x4d ) goto init;
	    else goto dflt;
	  case 0:
	    switch( low )
	    {
	      case '\x4b':		  /* left */
		if( *idx ) --*idx;
		else if( (ted2=long_left( tree2, idx )) != 0 )
		{
		  ted = ted2;
		  obj--;
		  tree2--;
		  ret = 2;
		}
		break;
	      case '\x4d':		  /* right */
		if( ted->te_ptext[*idx] ) ++*idx;
		else if( ted->te_tmplen==X_LONGEDIT )
		{
		  *idx = 0;
		  ret = 3;
		}
		break;
	      case '\x52':		  /* Insert */
		settings->flags.s.insert_mode ^= 1;
		break;
	      case '\x73':		  /* Control-left */
		ptr = (ptr2 = ted->te_ptext) + *idx;
		while( ptr!=ptr2 && *(ptr-1)==' ' ) ptr--;
		while( ptr!=ptr2 && *(ptr-1)!=' ' ) ptr--;
		*idx = ptr - ted->te_ptext;
		break;
	      case '\x74':		  /* Control-right */
		ptr = ted->te_ptext + *idx;
		while( *ptr && *ptr!=' ' ) ptr++;
		while( *ptr==' ' ) ptr++;
		*idx = ptr - ted->te_ptext;
		break;
	    }
	    break;
	  case '\n':	  /* Control-Return: only if LONGEDIT */
	    if( ted->te_tmplen == X_LONGEDIT )
	    {
	      draw_l = strlen( ptr=ted->te_ptext+*idx );
	      cont = long_insert( ptr, tree2+1, 0 );
	      *ptr = 0;
	      break;
	    }	  /* otherwise process as normal char */
	  default:
dflt:	    ptr = ted->te_ptext+ted->te_txtlen-1;
	    ptr2 = ted->te_pvalid;
	    if( (ok = strlen(ptr2)) == 0 ) break;
	    if( (i=*idx) > ted->te_txtlen-2 )
	    {
	      j = ted->te_txtlen-2;
	      i = 0;
	    }
	    else
	    {
	      j = i;
	      i = ted->te_txtlen - i - 2;
	    }
	    draw_s = j;
	    ptr2 += j>=ok ? ok-1 : j;
	    key = (char)key;
	    if( (ok = key>='a' && key<='z') != 0 && strchr( "ANFfPpHx", *ptr2 ) ) key &= 0x5F;
	    ok = ok || key >= 'A' && key <= 'Z';
	    switch( *ptr2 )
	    {
	      case 'x':
	      case 'X':
		ok++;
		break;
	      case 'H':
	      case 'h':
		ok = 0; 	  /* ignore previous (a-z) assignment */
		if( strchr("abcdefABCDEF",key) || key>='0' && key<='9' ) ok++;
		break;
	      case 'A':
	      case 'a':
		if( key==' ' ) ok++;
		break;
	      case 'N':
	      case 'n':
		if( key==' ' || key>='0' && key<='9' ) ok++;
		break;
	      case 'F':
		if( strchr("?*:",key) ) ok++;
	      case 'f':
		if( strchr(fvalid,key) ) ok++;
		else if( key>='0' && key<='9' ) ok++;
		break;
	      case 'P':
		if( key=='?' || key=='*' ) ok++;
	      case 'p':
		if( strchr("\\:._!@#$%^&()+-=~`;\'\",<>|[]{}",key) ) ok++;
		else if( key>='0' && key<='9' ) ok++;
		break;
	      default:
		if( *ptr2>'0' && *ptr2<='9' )
		{
		  ok = 0;	  /* ignore previous (a-z) assignment */
		  if( key>='0' && key<=*ptr2 ) ok++;
		}
		else ok++;
	    }
	    if( ok )
	    {
	      *ptr-- = '\0';
	      if( settings->flags.s.insert_mode )
	      {
		if( ted->te_tmplen==X_LONGEDIT )
		{
		  if( *idx==ted->te_txtlen-1 )
		  {
		    draw_l = *idx = 0;
		    fake_edit( tree, obj+1, key, idx );
		    ret = 3;
		    break;
		  }
		  str[0] = key;
		  str[1] = 0;
		  cont = long_insert( str, tree2, *idx );
		  ptr = ted->te_ptext+*idx;
		}
		else
		{
		  while(--i>=0)
		    *ptr-- = *(ptr-1);
		  *ptr = key;
		}
		draw_l = strlen(ptr);
	      }
	      else
	      {
		draw_l = 1;
		i = *(ptr=ted->te_ptext+j);
		*ptr = key;
		if( !i ) *++ptr = '\0';
	      }
	      if( ted->te_tmplen==X_LONGEDIT && *idx>=ted->te_txtlen-2 )
	      {
		*idx = 0;
		ret = 3;
	      }
	      else if( *idx<ted->te_txtlen-1 ) ++*idx;
	    }
	    else
	    {
	      ptr = ted->te_ptext+*idx;
	      ptr2 = ted->te_ptmplt+ted_off(ted,draw_s=*idx);
	      i = 0;
	      while( *ptr2 )
	      {
		if( *ptr2=='_' ) i++;
		if( *ptr2++==key && strchr( ptr2, '_' )/*003*/ )
		{
		  *idx += (draw_l=i);
		  while(--i>=0) *ptr++ = ' ';
		  *ptr++ = '\0';
		  draw_l += strlen(ptr2)+1;	/* 003 */
		  break;
		}
	      }
	    }
	}
	format_ted(ted);
	if( draw_l )
	{
	  if( !w ) r2 = r;
	  else
	  {
	    r2.x = x + ted_off_w( ted, draw_s, w );
	    r2.y = y;
	    r2.w = (ted_off( ted, draw_s+draw_l ) - ted_off( ted, draw_s )) * w;
	    r2.h = txt_clh;
	  }
	  if( gintersect( temp=clip_rect, r2, &r2 ) )
	  {
	    if( !w ) fill_col( od.color.b, &r2 );
	    ini = clip_ini;
	    it = clip_it;
	    clip_ini = dflt_clip_ini;
	    clip_it = dflt_clip;
	    _objc_draw( (OBJECT2 *)tree, ap, obj, 8, r2.x, r2.y, r2.w, r2.h );
	    clip_ini = ini;
	    clip_it = it;
	    _vs_clip( 1, &temp );
	  }
	}
	vdi_box( 1, 1, 0, 3, 0L );
	if( !w ) txt_extent( ted_buf, 0, 0, id, font, ted->te_just, &r, &temp.x, &y );	/* 003: don't alter x */
	/* !long edit */
	if( !cont && key!=-1 && ret<=1 )
	    draw_curs( x, w, ted, *idx, &r, settings->flags.s.insert_mode );
	break;
      default:
	DEBUGGER(OBJCED,UNKTYPE,kind);
    }
    _v_mouse(1);
    if( !cont-- || ted->te_tmplen != X_LONGEDIT ) break;
    tree2++;
    obj++;
    key = -1;
  }
  /* had long edit, so turn cursor back on now */
  if( key==-1 && ret==1 ) _xobjc_edit( tree, txt_app, obj0, 0L, idx, 3 );
  txt_app = 0L;
  form_app = of;
  return ret;
}

void _xcol( unsigned int old, unsigned int new, unsigned int *dat, int h )
{
  unsigned int c, m;
  int i, j, x;

  if( old==new ) return;
  for( j=h; --j>=0; dat++ )
    for( i=16; --i>=0; )
    {
      for( c=0, x=4; --x>=0; )
      {
        c <<= 1;
        c |= (*(dat+x*h)>>i) & 1;
      }
      if( c==old )
        for( c=new, x=0, m=~(1<<i); x<4; x++ )
        {
          *(dat+x*h) = (*(dat+x*h) & m) | ((c&1)<<i);
          c >>= 1;
        }
    }
}

void _xcolor( unsigned int col, unsigned int new )
{
  unsigned int i, j;
  static char ctran[] = { 0, 15, 1, 2, 4, 6, 3, 5, 7, 8, 9, 10,
      12, 14, 11, 13 };

  _xcol( i=ctran[col&0xf], j=ctran[new&0xf],
      (unsigned int *)radio->mainlist->col_data, radio->monoblk.ib_hicon );
  _xcol( i, j, (unsigned int *)radio->mainlist->sel_data, radio->monoblk.ib_hicon );
  _xcol( i, j, (unsigned int *)check->mainlist->col_data, check->monoblk.ib_hicon );
  _xcol( i, j, (unsigned int *)check->mainlist->sel_data, check->monoblk.ib_hicon );
}

void reinit_ic( int **ptr, int *dat )
{
  if( *ptr != dat ) lfree(*ptr);
  *ptr = dat;
}

void find_best( CICONBLK *ci )
{
  CICON *c = ci->mainlist;

  ci->mainlist = 0L;
  while(c)
  {
    if( c->num_planes <= vplanes &&
        (!ci->mainlist || c->num_planes > ci->mainlist->num_planes) )
        ci->mainlist = c;
    c = c->next_res;
  }
}

void trans_cicon( CICONBLK *c, CICON *best, int alloc )
{
  MFDB d;
  long len, vlen;
  char eq;

  d.fd_stand = 1;
  d.fd_nplanes = vplanes;
  d.fd_w       = c->monoblk.ib_wicon;
  d.fd_h       = c->monoblk.ib_hicon;
  d.fd_wdwidth = d.fd_w >> 4;

  vlen = (len = (long)(d.fd_wdwidth<<1)*d.fd_h) * vplanes;
  if( (eq = best->num_planes == vplanes && !alloc) != 0 ) d.fd_addr = best->col_data;
  else if( (d.fd_addr = lalloc( vlen, -1 )) == 0L ) return;
  if( alloc ) memcpy( best->col_data=d.fd_addr, best->col_data, vlen );
  else best->next_res = (CICON *)1L;
  fix_cicon((unsigned int *)best->col_data, len, best->num_planes, vplanes, &d);
  if( !eq ) best->col_data = d.fd_addr;
  if (best->sel_data)
  {
    if( eq ) d.fd_addr = best->sel_data;
    else if( (d.fd_addr = lalloc( vlen, -1 )) == 0L )
    {
      best->sel_data = 0L;
      return;
    }
    if( alloc ) memcpy( best->sel_data=d.fd_addr, best->sel_data, vlen );
    fix_cicon((unsigned int *)best->sel_data, len, best->num_planes, vplanes, &d);
    if( !eq ) best->sel_data = d.fd_addr;
  }
  best->num_planes = vplanes;
}

void new_cic( unsigned int new )
{
  static int *chk, *chks, *rad, *rads, pl;
  static unsigned int color=8;
  static CICON *radold, *chkold;
  unsigned int temp;

  if( radio )
  {
    reinit_ic( &radio->mainlist->col_data, rad );
    reinit_ic( &radio->mainlist->sel_data, rads );
    reinit_ic( &check->mainlist->col_data, chk );
    reinit_ic( &check->mainlist->sel_data, chks );
    radio->mainlist->num_planes = check->mainlist->num_planes = pl;
    radio->mainlist = radold;
    check->mainlist = chkold;
  }
  radold = (radio = &rs_ciconblk[char_h>=16?RADIO_HI:RADIO_LOW])->mainlist;
  chkold = (check = &rs_ciconblk[char_h>=16?CHECK_HI:CHECK_LOW])->mainlist;
  find_best( radio );
  find_best( check );
  pl = radio->mainlist->num_planes;
  rad = radio->mainlist->col_data;
  rads = radio->mainlist->sel_data;
  chk = check->mainlist->col_data;
  chks = check->mainlist->sel_data;
  if( pl>=4 )
  {
    _xcolor( temp=color, new );
    color = new;
    xor_col( &new );
    xor_col( &temp );
    _xcolor( temp, new );
    trans_cicon( radio, radio->mainlist, 1 );
    trans_cicon( check, check->mainlist, 1 );
  }
}

void ob_fixspec(void)
{
  new_cic( ind_col );
}

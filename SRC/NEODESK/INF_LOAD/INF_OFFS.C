#include "neodesk.h"
#include "aes.h"
#include "xwind.h"
#include "tos.h"
#include "neocommn.h"
#include "neod2_id.h"
#include "string.h"
#include "guidefs.h"	/* must come after aes.h and neocommn.h */
extern GUI *gui;

#define IDT_cookie      0x5F494454L     /* "_IDT" */

extern MOST dum, *z;	/* this is really not correct! */
extern MASTER *mas;
extern PRN_PARAM prn_param;
extern GROUP_DESC *group_desc[7];
extern char neocntrl[], ver_gt_10;
extern int AES_handle, diskbuff[512], sort_type, stcolumn, vplanes,
    stlgsml, sizdattim[3], showicon, q_handle, c_handle, w_num,
    w_active, num_w_active, d_active, bar_h, dtop_handle, use_8x16,
    num_icons, bar_w;
extern char *msg_ptr[], tmpf[], filename[], ext[3][5], TT_mono;
extern GRAPHICS *graphics;
extern OBJECT *form, *icons;
extern ICONBUF *nic_icons;
extern unsigned char fdc_level;
extern long dflt_pall[16];
extern INF_CONV inf_conv;
extern LoadCookie *lc;
extern struct Max_icon
{
  int text_w, data_w, h;
} max_icon;
int inall_vol;

#define offset(a) (void *)&a
#define diskbuff  ((char *)diskbuff)

#ifndef DEMO

void *offs[] = {  /* first 1 unused */
    offset(dum.new_mem_mode),
    offset(dum.autoexec[0]),
    offset(dum.autoexec[0]),	/* gets changed later */
    offset(dum.cntrl_set),
    offset(dum.pallette),
    offset(dum.snapx), offset(dum.snapy), offset(dum.snap_resort), offset(dum.snap_over),
    offset(dum.dir_prn.c),
    offset(dum.move_mode), offset(dum.conf_copy), offset(dum.conf_del), offset(dum.conf_over),
      offset(dum.diskcopy), offset(dum.dskcpy_bak), offset(dum.disk_copies),
      offset(dum.copy_bak), offset(dum.del_bak), offset(dum.rename_dest), offset(dum.filt_copy),
      offset(dum.filt_del), offset(dum.count_copy), offset(dum.count_del),
    offset(dum.back_speed), offset(dum.tos_pause), offset(dum.use_master), offset(dum.ctrlZ),
      offset(dum.status_report), offset(dum.quit_alert), offset(dum.saveconf), offset(dum.other_pref.i),
      offset(dum.idt_fmt), offset(dum.speed), offset(dum.num_sep), offset(dum.open_fold),
      offset(dum.view_picopts.fit), offset(dum.view_picopts.mode),
    offset(dum.tracks), offset(dum.sides), offset(dum.spt), offset(dum.twst),
      offset(dum.format_drive), offset(dum.format_bak), offset(dum.spc),
    offset(dum.batch_name[0]),
    offset(dum.text_reader[0]),
    offset(dum.w[0].place), offset(dum.w[0].f_off.l), offset(dum.showicon[0]), offset(dum.stlgsml[0]),
        offset(dum.stcolumn[0]), offset(dum.sizdattim[0]), offset(dum.sort_type[0]), offset(dum.w[0].path),
    offset(dum.w[1].place), offset(dum.w[1].f_off.l), offset(dum.showicon[1]), offset(dum.stlgsml[1]),
        offset(dum.stcolumn[1]), offset(dum.sizdattim[1]), offset(dum.sort_type[1]), offset(dum.w[1].path),
    offset(dum.w[2].place), offset(dum.w[2].f_off.l), offset(dum.showicon[2]), offset(dum.stlgsml[2]),
        offset(dum.stcolumn[2]), offset(dum.sizdattim[2]), offset(dum.sort_type[2]), offset(dum.w[2].path),
    offset(dum.w[3].place), offset(dum.w[3].f_off.l), offset(dum.showicon[3]), offset(dum.stlgsml[3]),
        offset(dum.stcolumn[3]), offset(dum.sizdattim[3]), offset(dum.sort_type[3]), offset(dum.w[3].path),
    offset(dum.w[4].place), offset(dum.w[4].f_off.l), offset(dum.showicon[4]), offset(dum.stlgsml[4]),
        offset(dum.stcolumn[4]), offset(dum.sizdattim[4]), offset(dum.sort_type[4]), offset(dum.w[4].path),
    offset(dum.w[5].place), offset(dum.w[5].f_off.l), offset(dum.showicon[5]), offset(dum.stlgsml[5]),
        offset(dum.stcolumn[5]), offset(dum.sizdattim[5]), offset(dum.sort_type[5]), offset(dum.w[5].path),
    offset(dum.w[6].place), offset(dum.w[6].f_off.l), offset(dum.showicon[6]), offset(dum.stlgsml[6]),
        offset(dum.stcolumn[6]), offset(dum.sizdattim[6]), offset(dum.sort_type[6]), offset(dum.w[6].path),
    offset(dum.mshowicon), offset(dum.mstlgsml), offset(dum.mstcolumn), offset(dum.msizdattim[0]),
        offset(dum.msort_type),		/* 003 */
    offset(dum.filter[0].flags.i), offset(dum.filter[0].use_size),
        offset(dum.filter[0].mask), offset(dum.filter[0].times[0]),
        offset(dum.filter[0].sizes[0]), offset(dum.filter[0].long_tmpl[0]),
        offset(dum.filter[0].long_tmpl[1]), offset(dum.filter[0].long_tmpl[2]),
    offset(dum.filter[1].flags.i), offset(dum.filter[1].use_size),
        offset(dum.filter[1].mask), offset(dum.filter[1].times[0]),
        offset(dum.filter[1].sizes[0]), offset(dum.filter[1].long_tmpl[0]),
        offset(dum.filter[1].long_tmpl[1]), offset(dum.filter[1].long_tmpl[2]),
    offset(dum.filter[2].flags.i), offset(dum.filter[2].use_size),
        offset(dum.filter[2].mask), offset(dum.filter[2].times[0]),
        offset(dum.filter[2].sizes[0]), offset(dum.filter[2].long_tmpl[0]),
        offset(dum.filter[2].long_tmpl[1]), offset(dum.filter[2].long_tmpl[2]),
    offset(dum.filter[3].flags.i), offset(dum.filter[3].use_size),
        offset(dum.filter[3].mask), offset(dum.filter[3].times[0]),
        offset(dum.filter[3].sizes[0]), offset(dum.filter[3].long_tmpl[0]),
        offset(dum.filter[3].long_tmpl[1]), offset(dum.filter[3].long_tmpl[2]),
    offset(dum.filter[4].flags.i), offset(dum.filter[4].use_size),
        offset(dum.filter[4].mask), offset(dum.filter[4].times[0]),
        offset(dum.filter[4].sizes[0]), offset(dum.filter[4].long_tmpl[0]),
        offset(dum.filter[4].long_tmpl[1]), offset(dum.filter[4].long_tmpl[2]),
    offset(dum.filter[5].flags.i), offset(dum.filter[5].use_size),
        offset(dum.filter[5].mask), offset(dum.filter[5].times[0]),
        offset(dum.filter[5].sizes[0]), offset(dum.filter[5].long_tmpl[0]),
        offset(dum.filter[5].long_tmpl[1]), offset(dum.filter[5].long_tmpl[2]),
    offset(dum.filter[6].flags.i), offset(dum.filter[6].use_size),
        offset(dum.filter[6].mask), offset(dum.filter[6].times[0]),
        offset(dum.filter[6].sizes[0]), offset(dum.filter[6].long_tmpl[0]),
        offset(dum.filter[6].long_tmpl[1]), offset(dum.filter[6].long_tmpl[2]),
    offset(dum.copydel_filt.flags.i), offset(dum.copydel_filt.use_size),
        offset(dum.copydel_filt.mask), offset(dum.copydel_filt.times[0]),
        offset(dum.copydel_filt.sizes[0]), offset(dum.copydel_filt.long_tmpl[0]),
        offset(dum.copydel_filt.long_tmpl[1]), offset(dum.copydel_filt.long_tmpl[2]),
    offset(dum.search_filt.flags.i), offset(dum.search_filt.use_size),
        offset(dum.search_filt.mask), offset(dum.search_filt.times[0]),
        offset(dum.search_filt.sizes[0]), offset(dum.search_filt.long_tmpl[0]),
        offset(dum.search_filt.long_tmpl[1]), offset(dum.search_filt.long_tmpl[2]),
    offset(dum.template[0]), offset(dum.template[1]), offset(dum.template[2]),
        offset(dum.template[3]), offset(dum.template[4]), offset(dum.template[5]),
    offset(dum.ttp_params[0]), offset(dum.ttp_params[1]), offset(dum.ttp_params[2]),
        offset(dum.ttp_params[3]), offset(dum.ttp_params[4]),
    offset(dum.use_argv), offset(dum.env_parent),
    offset(dum.dial_in_wind), offset(dum.dial_mode),
    offset(dum.desk_in_wind), offset(dum.wind_pos), offset(dum.show_pic),
        offset(dum.wall_pic), offset(dum.pic_colormode), offset(dum.desk_pic),
        offset(dum.desk_picopts.fit), offset(dum.desk_picopts.mode),
    offset(dum.wind_prf), offset(dum.real_time), offset(dum.wind_font[0].id),
        offset(dum.wind_font[1].id), offset(dum.wind_font[2].id),
        offset(dum.wind_font[3].id),
    offset(dum.help.font), offset(dum.help.topic), offset(dum.help.match),
        offset(dum.help.hlp_path),
    offset(dum.gui_settings.flags.i), offset(dum.gui_settings.menu_start),
        offset(dum.gui_settings.redraw_all),
    offset(dum.gui_settings.wind_keys[0]),
    offset(dum.gui_settings.color_3D.l),
    offset(dum.autoexec[0]),	/* gets changed later to *wstates */
    offset(dum.autoexec[0]),	/* gets changed later to *wstates[16] */
    offset(dum.autoexec[0]),	/* gets changed later to *dwcolors[0][0] */
    offset(dum.autoexec[0]),	/* gets changed later to *dwcolors[0][16] */
    offset(dum.autoexec[0]),	/* gets changed later to *dwcolors[1][0] */
    offset(dum.autoexec[0]),	/* gets changed later to *dwcolors[1][16] */
    0L };

INF_OFF inf_off[] = {
	     /* \octal! */
    "AUTOEXC", "S",    	&offs[1], 38,
    "QUEUE",   "\2d",  	&offs[2], 39,	/* init_inf_offs depends on this */
    "CONTROL", "\16d", 	&offs[3], 40,
    "PALETTE", "\20X", 	&offs[4], 41,
    "SNAP",    "ddbb", 	&offs[5], 42,
    "PRNTDIR", "h", 	&offs[9], 116,
    "COPYMOV", "ddddhbdbbbbbbb",&offs[10], 152,
    "MISCPRF", "ddddddhdddhbbh",&offs[17+7], 153,
    "FORMAT",  "dddddb\3d",	&offs[26+12], 154,
    "BATCH",   "S",	&offs[28+17], 155,
    "TXTREAD", "S",	&offs[29+17], 156,
    "WINDOW1", "\6dDddd\3dds",	&offs[30+17], 157,	/* init_inf_offs depends on this */
    "WINDOW2", "\6dDddd\3dds",	&offs[38+17], 0,
    "WINDOW3", "\6dDddd\3dds",	&offs[46+17], 0,
    "WINDOW4", "\6dDddd\3dds",	&offs[54+17], 0,
    "WINDOW5", "\6dDddd\3dds",	&offs[62+17], 0,
    "WINDOW6", "\6dDddd\3dds",	&offs[70+17], 0,
    "WINDOW7", "\6dDddd\3dds",	&offs[78+17], 0,
    "DFLTWIN", "ddd\3dd",	&offs[86+17], 0,
    "FILTER1", "x\3hx\6x\3XSSS", &offs[86+22], 158,
    "FILTER2", "x\3hx\6x\3XSSS", &offs[94+22], 0,
    "FILTER3", "x\3hx\6x\3XSSS", &offs[102+22], 0,
    "FILTER4", "x\3hx\6x\3XSSS", &offs[110+22], 0,
    "FILTER5", "x\3hx\6x\3XSSS", &offs[118+22], 0,
    "FILTER6", "x\3hx\6x\3XSSS", &offs[126+22], 0,
    "FILTER7", "x\3hx\6x\3XSSS", &offs[134+22], 0,
    "CPDLFLT", "x\3hx\6x\3XSSS", &offs[142+22], 159,
    "SRCHFLT", "x\3hx\6x\3XSSS", &offs[142+30], 160,
    "SRCHTMP", "SSSSSS", &offs[150+30], 161,
    "TTP1",    "s", &offs[156+30], 162,
    "TTP2",    "s", &offs[157+30], 0,
    "TTP3",    "s", &offs[158+30], 0,
    "TTP4",    "s", &offs[159+30], 0,
    "TTP5",    "s", &offs[160+30], 0,
    "ENVOPTS", "bb", &offs[161+30], 163,
    "DIALPRF", "bd", &offs[163+30], 164,
    "DESKPRF", "b\4dbbdsbh", &offs[165+30], 165,
    "WINDPRF", "xb\2d\2d\2d\2d", &offs[171+32], 166,
    "HELPPRF", "\6dS\2bs", &offs[171+38], 109,
    "GUI1", "\2dkk", &offs[175+38], 0,
    "GUI2", "\15k", &offs[178+38], 0,
    "GUI8", "\4X", &offs[179+38], 0,
    "GUI9", "\20x", &offs[180+38], 0,	/* init_inf_offs depends on this */
    "GUIA", "\20x", &offs[181+38], 0,
    "GUIB", "\20x", &offs[182+38], 0,
    "GUIC", "\20x", &offs[183+38], 0,
    "GUID", "\20x", &offs[184+38], 0,
    "GUIE", "\20x", &offs[185+38], 0,
    0 };

FUNC_INF_OFF func_inf_off[] = {
    "DESKICN", "dddSSdS", offset(dum.idat), winf_deskicn, rinf_deskicn, 167,
    "APPLIC2", "dxSSS", offset(dum.apps), winf_applic, rinf_applic, 168,
    "EXTENSN", "hS", offset(dum.extension), winf_extens, rinf_extens, 89,
    "ENVIRON", "s", 0L, winf_env, rinf_env, 169,
    "DIALOG",  "d\2d", 0L, winf_dialog, rinf_dialog, 90,
    "AV", "SS", offset(dum.av_status), winf_av, rinf_av, 0,
    0 };

NSETTINGS gui_settings = {
    NSET_VER, sizeof(NSETTINGS),
    0, 0,
    { 1, /* pulldown */           1, /* insert_mode */
      1, /* long titles */        1, /* alerts under mouse */
      0, /*  */                   0, /* */
      0, /* tear always topped */ 0, /* */
      0,			  1, /* use wcolors cpx */
      0  /* reserved */ },
    15, /* gadget pause */
    { 8, 0, ' ' },      /* menu start */
    { 8, 0xf, 0 },      /* app switch */
    { 0xc, 0x2c, 0 },   /* app sleep */
    { 0xc, 0x39, 0 },   /* ascii table */
    { 0xb, 0x61, 0 },   /* redraw all */
    { { 7, 0x68, 0 },   /* up page */
      { 7, 0x6e, 0 },   /* down page */
      { 4, 0x68, 0 },   /* up line */
      { 4, 0x6e, 0 },   /* down line */
      { 7, 0x6a, 0 },   /* left page */
      { 7, 0x6c, 0 },   /* rt page */
      { 4, 0x6a, 0 },   /* left line */
      { 4, 0x6c, 0 },   /* rt line */
      { 8, 1, 0 },      /* close */
      { 8, 0xe, 0 },    /* cycle */
      { 8, 0x53, 0 },   /* full */
      { 4, 0x63, 0 },   /* info left */
      { 4, 0x65, 0 } }, /* info rt */
    { 0L }, { 0L }, { 0L }, { 0L },
    { 0xb, 0xe, 0 },	/* cycle in app */
    { 0x8, 0x29, 0 },	/* iconify */
    { 0xc, 0x29, 0 },	/* iconify all */
    LWHITE,		/* graymenu */
    0,			/* wcolor_mode */
  { {
    0x20, 0x9880, 0x0, 0x9880, 0x9880, 0x9880,
    0x9880, 0x0, 0x9880,		/* info */
    0x0,				/* tool */
    0x9880, 0x0, 0x9880,		/* menu */
    0x9880, 0x0, 0x9880, 0x9880,	/* vslider */
    0x0,				/* split */
    0x9880, 0x0, 0x9880, 0x9880,	/* vslider */
    0x9880, 0x0, 0x9880, 0x9880,	/* hslider */
    0x0,				/* split */
    0x9880, 0x0, 0x9880, 0x9880,	/* hslider */
    0x9880 },
  {
    0x20, 0x9080, 0x0, 0x9080, 0x9080, 0x9080,
    0x9080, 0x0, 0x9080,		/* info */
    0x0,				/* tool */
    0x9080, 0x0, 0x9080,		/* menu */
    0x9080, 0x0, 0x9080, 0x9080,	/* vslider */
    0x0,				/* split */
    0x9080, 0x0, 0x9080, 0x9080,	/* vslider */
    0x9080, 0x0, 0x9080, 0x9080,	/* hslider */
    0x0,				/* split */
    0x9080, 0x0, 0x9080, 0x9080,	/* hslider */
    0x9080 },
  {
    0x20, 0x9080, 0x0, 0x9080, 0x9080, 0x9080,
    0x9080, 0x0, 0x9080,		/* info */
    0x0,				/* tool */
    0x9080, 0x0, 0x9080,		/* menu */
    0x9080, 0x0, 0x9080, 0x9080,	/* vslider */
    0x0,				/* split */
    0x9080, 0x0, 0x9080, 0x9080,	/* vslider */
    0x9080, 0x0, 0x9080, 0x9080,	/* hslider */
    0x0,				/* split */
    0x9080, 0x0, 0x9080, 0x9080,	/* hslider */
    0x9080 } },
  { { /* 1 plane */
    0x1100, 0x1100, 0x1100, 0x1100, 0x1100, 0x1100,
    0x1100, 0x1100, 0x1100,		/* info */
    0,					/* tool */
    0x1100, 0x1100, 0x1100,		/* menu */
    0x1100, 0x1111, 0x1100, 0x1100,	/* vslider */
    0x1100,				/* split */
    0x1100, 0x1111, 0x1100, 0x1100,	/* vslider */
    0x1100, 0x1111, 0x1100, 0x1100,	/* hslider */
    0x1100,				/* split */
    0x1100, 0x1111, 0x1100, 0x1100,	/* hslider */
    0x1100,
    0x1100, 0x1141, 0x11E1, 0x1141, 0x1141, 0x1141,
    0x1100, 0x1100, 0x1100,		/* info */
    0,					/* tool */
    0x1100, 0x1100, 0x1100,		/* menu */
    0x1141, 0x1151, 0x1141, 0x1141,	/* vslider */
    0x1100,				/* split */
    0x1141, 0x1151, 0x1141, 0x1141,	/* vslider */
    0x1141, 0x1151, 0x1141, 0x1141,	/* hslider */
    0x1100,				/* split */
    0x1141, 0x1151, 0x1141, 0x1141,	/* hslider */
    0x1141 },
  {  /* 2 planes */
    0x1100, 0x1100, 0x1100, 0x1100, 0x1100, 0x1100,
    0x1100, 0x1100, 0x1100,		/* info */
    0,					/* tool */
    0x1100, 0x1100, 0x1100,		/* menu */
    0x1100, 0x1111, 0x1100, 0x1100,	/* vslider */
    0x1100,				/* split */
    0x1100, 0x1111, 0x1100, 0x1100,	/* vslider */
    0x1100, 0x1111, 0x1100, 0x1100,	/* hslider */
    0x1100,				/* split */
    0x1100, 0x1111, 0x1100, 0x1100,	/* hslider */
    0x1100,
    0x1100, 0x1100, 0x11E1, 0x1100, 0x1100, 0x1100,
    0x1100, 0x1100, 0x1100,		/* info */
    0,					/* tool */
    0x1100, 0x1100, 0x1100,		/* menu */
    0x1100, 0x1151, 0x1100, 0x1100,	/* vslider */
    0x1100,				/* split */
    0x1100, 0x1151, 0x1100, 0x1100,	/* vslider */
    0x1100, 0x1151, 0x1100, 0x1100,	/* hslider */
    0x1100,				/* split */
    0x1100, 0x1151, 0x1100, 0x1100,	/* hslider */
    0x1100 },
  {  /* 4+ planes */
    0x1100, 0x1970, 0x1970, 0x1970, 0x1970, 0x1970,
    0x1100, 0x1100, 0x1100,		/* info */
    0,					/* tool */
    0x1100, 0x1100, 0x1100,		/* menu */
    0x1970, 0x1878, 0x1970, 0x1970,	/* vslider */
    0x1100,				/* split */
    0x1970, 0x1878, 0x1970, 0x1970,	/* vslider */
    0x1970, 0x1878, 0x1970, 0x1970,	/* hslider */
    0x1100,				/* split */
    0x1970, 0x1878, 0x1970, 0x1970,	/* hslider */
    0x1100,
    0x1100, 0x1178, 0x1148, 0x1178, 0x1178, 0x1178,
    0x1100, 0x1100, 0x1100,		/* info */
    0,					/* tool */
    0x1100, 0x1100, 0x1100,		/* menu */
    0x1178, 0x1979, 0x1178, 0x1178,	/* vslider */
    0x1100,				/* split */
    0x1178, 0x1979, 0x1178, 0x1178,	/* vslider */
    0x1178, 0x1979, 0x1178, 0x1178,	/* hslider */
    0x1100,				/* split */
    0x1178, 0x1979, 0x1178, 0x1178,	/* hslider */
    0x1178 } }
  };


void init_inf_offs(void)
{
  void **v;
  static char fixed, *environ;
  static int *dial;
  int j;
  FUNC_INF_OFF *i;
  NSETTINGS *n;
  
  if( !fixed )
  {
    for( v=offs; *v; v++ )
      *v = (void *)(*(long *)v - (long)&dum + (long)z);
    for( i=func_inf_off; i->type[0]; i++ )
      i->start = (void **)((long)i->start - (long)&dum + (long)z);
    fixed = 1;
    j = (n=&z->gui_settings)->wcolor_mode;
    offs[180+38] = &n->wstates[j][0];
    offs[181+38] = &n->wstates[j][16];
    offs[182+38] = &n->dwcolors[j][0][0];
    offs[183+38] = &n->dwcolors[j][0][16];
    offs[184+38] = &n->dwcolors[j][1][0];
    offs[185+38] = &n->dwcolors[j][1][16];
  }
  offs[2] = &prn_param.prn_set;
  for( v=&offs[37+17], j=0; j<7; j++, v+=8 )
    *v = group_desc[j] ? (void *)&group_desc[j]->path : (void *)&z->w[j].path;
  environ = z->env;
  func_inf_off[INF_ENVIRON-100].start = (void **)&environ;
  dial = &z->dialogs[0][0];
  func_inf_off[INF_DIALOG-100].start = (void **)&dial;
}

void put_pointers( char *result, char *label, char *fmt, void **list );
void get_pointers( char *buf, char *fmt, void **list );

int scan_inf_line( char *buf, int i )
{
  FUNC_INF_OFF *inf2;

  if( i<100 )
  {
    get_pointers( buf, inf_off[i].fmt, inf_off[i].list );
    return 0;
  }
  else
  {
    inf2 = &func_inf_off[i-100];
    return (*inf2->read)( buf, inf2->start, inf2->fmt );
  }
}
#endif DEMO

int saveinf( int file )
{
#ifndef DEMO
  char out[200];
  INF_OFF *inf;
  FUNC_INF_OFF *inf2;
  int i, j;
  unsigned long cmsg[4];
  char *ptr;
  long l;
  void *list[10], *parm;

  init_inf_offs();
  read_q_set();
  if( (c_handle = appl_pfind( neocntrl )) >= 0 )
  {
/*%    wind_update( END_UPDATE ); */
    cmsg[0] = ((long)CNTRL_REQ4 << 16) | AES_handle;
    cmsg[1] = (long) out;
    cmsg[2] = AES_handle;
    appl_pwrite( c_handle, 16, cmsg );
    get_ack( (int *)cmsg, c_handle );
/*%    wind_update( BEG_UPDATE ); */
    scan_inf_line( out, 2 );
  }
  save_desktop();
/*%  if( !z->w[0].place )  003
  {
    z->showicon[0] = showicon;
    z->stlgsml[0] = stlgsml;
    z->stcolumn[0] = stcolumn;
    z->sort_type[0] = sort_type;
    memcpy( z->sizdattim[0], sizdattim, 3*sizeof(int) );
  } */
  for( i=0; i<16; i++ )
    z->pallette[i] = (l=setcolor( i, -1 )) < 0 ? dflt_pall[i] : l;
  fpf( file, msg_ptr[37], INF_VER, graphics->v_x_max, graphics->v_y_max,
      graphics->cel_ht );
  for( inf=inf_off; inf->type[0]; inf++ )
  {
    if( inf->msg )
    {
      fps( msg_ptr[inf->msg], file );
      fps( "\r\n", file );
    }
    put_pointers( out, inf->type, inf->fmt, inf->list );
    fps( out, file );
  }
  for( inf2=func_inf_off; inf2->type[0]; inf2++ )
  {
    i = 0;
    parm = *(inf2->start);
    while( (j = (*inf2->write)( i++, &parm, list )) != 0 )
      if( j>0 )
      {
        if( i==1 && inf2->msg )
        {
          fps( msg_ptr[inf2->msg], file );
          fps( "\r\n", file );
        }
        put_pointers( out, inf2->type, inf2->fmt, list );
        fps( out, file );
      }
  }
  return 1;
#else DEMO
  demo_version();
  return 0;
#endif
}

void missing_file( char *name )		/* 004 */
{
  char temp[31], tmpf[120];

  short_path( name, temp, 30, 30 );
  spf( tmpf, msg_ptr[55], temp );
  f_alert1( tmpf );
}

int get_line( int file )
{
  return( (*mas->get_line)(file,diskbuff,1,0) );
}
void load_file( int err, int hand, int num, int flag, char *name )
{
  char str[5], *ptr;
  int i, v;
  struct Not_hdr
  {
    int len, id, pt;
    char col, opaq;
  } not_hdr = { OLD_NOTE_SIZ, 1, 8, 1, 1 };

  if( num==1 ) free_macros();
  else if( num==2 )
  {
    z->note_opaq = z->note_col = 1;
    z->wind_font[4].id = 1;
    z->wind_font[4].size = 8;
    free_notes();
  }
  if( err )
  {
    if( flag ) missing_file(name);
  }
  else
  {
    if( z->macr_rec && num==1 )
    {
      f_alert1( msg_ptr[122] );
      use_gmenu(-1,BEMACRO);
    }
    else
    {
      str[4] = '\0';
      if( cFread( hand, 4L, str ) != 4L || strcmp( str, ext[num] ) ||
	  cFread( hand, 2L, &v ) != 2L || num==1 && v!=MACRO_VER ||
	  num!=1 && v!=0x300 && v!=NOTE_VER )
      {
err:	spf( tmpf, msg_ptr[56], ext[num] );
	f_alert1(tmpf);
      }
      else if( num!=1 )	/* notes */
      {
        if( v==NOTE_VER )
        {
          if( cFread( hand, sizeof(not_hdr), &not_hdr ) != sizeof(not_hdr) ) goto err;
        }
        /* else default not_hdr */
        while( not_hdr.len > 0 )
        {
          i = not_hdr.len>sizeof(diskbuff) ? sizeof(diskbuff):not_hdr.len;
          if( cFread( hand, i, diskbuff ) != i )
          {
            free_notes();
            TOS_error( i, 0 );
            goto err;
          }
          if( !add_note( i, diskbuff ) ) break;
          not_hdr.len -= i;
        }
        z->note_opaq = not_hdr.opaq;
        z->note_col = not_hdr.col;
        z->wind_font[4].size = not_hdr.pt;
        z->wind_font[4].id = not_hdr.id;
        if( v!=NOTE_VER && (ptr = z->notes) != 0 )
        {	/* shrink old notes */
          while( *(ptr+2) ) ptr += strlen(ptr+2) + 3;
          if( (i = ptr - z->notes) == 0 ) free_notes();
          else if( (i = z->notes_len-i) > 0 )
          {
            z->notes_rem += i;
            z->notes_len -= i;
          }
        }
      }
      else if( cFread( hand, 2L, &v ) != 2L ) goto err;
      else
      {
        z->macstrt = 0;
        while( v>0 )
          if( (i = cFread( hand, sizeof(diskbuff), diskbuff)) == 0 ) break;
          else if( !TOS_error( i, 0 ) || !add_macro( MACLOAD, diskbuff, i ) )
          {
            free_macros();
            break;
          }
          else if( !i ) break;
          else v -= i;
        z->macptr -= 2;
      }
    }
    cFclose(hand);
    if( num==1 ) z->macr_play = 0;
  }
}
int dflt_icon( char c, int type )
{
  int j, w, i, x0, y0;

  for( i=0; i<z->num_icons; i++ )
  {
    j = z->idat[i].type;
    if( j>=0 && (j<=RAMDISK && type<=RAMDISK && z->idat[i].c==c ||
        type>RAMDISK && type==j) ) return 0;
  }
  if( (j = add_desk()) >= 0 )
  {
    get_max_icon(-1);
    x0 = z->snapx + max_icon.text_w;
    y0 = z->snapy + max_icon.h;
    w = z->maximum.w/x0;
    i = j-1;
    z->desk[j].ob_x = x0 * (i%w) + 7;
    if( (z->desk[j].ob_y = y0 * (i/w) + 3) > z->maximum.h-y0 )
	z->desk[j].ob_y = z->maximum.h-y0;
    z->idat[i].c = c;
    z->idat[i].state = inall_vol;	/* 004: sometimes set in x_insticon */
    if( (z->idat[i].type = type) >= num_icons ) type = num_icons-1;	/* 003: added if for when no NEOICONS.NIC */
    z->idat[i].nicb = &nic_icons[type].nicb;	/* 003 */
    strcpy( z->idat[i].label, type<NPI ?
        icons[type+1].ob_spec.iconblk->ib_ptext : nic_icons[type].text );
    return 0;
  }
  return 1;
}
void install_devices( int iconedit )
{
  int i, err;
  long map;
  
  map = drvmap();
  for( i=err=0; i<26 && !err; i++ )
    if( map & (1L<<i) ) err =
        dflt_icon( i+'A', i<2 ? FLOPPY : HARDDSK );
  if( !err && !iconedit ) err = dflt_icon( 0, CLIPBRD );
  if( !err ) err = dflt_icon( 0, TRASH );
  if( !err && !iconedit ) dflt_icon( 0, PRINTER );
}
char *skip_word( char *ptr )
{
  char *p;
  
  if( (p=strchr(ptr,' ')) != 0 ) return p+1;
  return ptr+strlen(ptr);
}
long get_idt_fmt(void)
{
  static unsigned int vals[] = { (0<<12) | (0<<8) | '/', 	  /* USA */
  				 (1<<12) | (1<<8) | '.',  /* Germany */
  				 (1<<12) | (1<<8) | 0,	  /* France */
  				 (1<<12) | (1<<8) | '.',  /* UK */
  				 (1<<12) | (1<<8) | 0,	  /* Spain */
  				 (1<<12) | (1<<8) | 0 };  /* Italy */
  unsigned int mode = (*(SYSHDR **)0x4f2)->os_base->os_palmode>>1;	/* 003: added os_base */

  return mode>=sizeof(vals)/2 ? ((1<<12) | (2<<8) | '-') : vals[mode];
}
#ifndef DEMO
  #define IS_OK(x)  !is_ext || (*inf_conv.is_ok)(x)
#else
  #define IS_OK(x)  1
#endif

void get_wcolors(void)
{
  int dcolor, dum[4], i;
  
  if( !z->gui_settings.flags.s.use_wcolors_cpx ) return;
  dcolor = _GemParBlk.global[0] >= 0x340 && _GemParBlk.global[0] != 0x399/* MagX */;
  if( _GemParBlk.global[0] >= 0x410 )
  {
    dum[0] = 0;
    if( appl_getinfo( 11, dum, dum+2, dum+2, dum+2 ) ) dcolor = dum[0]&(1<<3);
  }
  for( i=0; i<19; i++ )
  {
    if( dcolor )
    {
      dum[0] = i;
      wind_get( 0, WF_DCOLOR, dum, &dum[2], &dum[1] );
    }
    else *(long *)&dum[1] = *(long *)&(*lc->w_colors)[i][0];
    wind_set( 0, WF_DCOLOR, i, dum[2], dum[1] );
  }
}

void reset_inf( int err, int is_ext )
{
  register int i, *r;
  int j, k;
  unsigned long idt;
  long *lp;
  static char dflt_temps[][6] = { "*.*", "*.PRG", "*.TOS", "*.TTP", "*.ACC",
      "*.TXT" };
  static char hp[] = { 8, 9, 10 }, hr[] = { 18, 18, 20 }, hc[] = { 45, 75, 75 };
  static OB_PREFER ob_prefs[4][4] = {
    { { 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 4, 1 },         /* color_3D[0] (2 col) */
      { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 7, 0 },         /* color_3D[1] (4 col) */
      { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 7, 8 },         /* color_3D[2] (16 col) */
      { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 7, 8 } },       /* color_3D[3] (256 col) */
    { { 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 7, 0 },         /* color_root */
      { 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 7, 0 },         /* color_root */
      { 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 7, 0 },         /* color_root */
      { 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 7, 0 } },       /* color_root */
    { { 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 4, 1 },         /* color_exit */
      { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 7, 0 },         /* color_exit */
      { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 7, 8 },         /* color_exit */
      { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 7, 8 } },       /* color_exit */
    { { 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 4, 1 },         /* color_other */
      { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 7, 0 },         /* color_other */
      { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 7, 8 },         /* color_other */
      { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 7, 8 } } };     /* color_other */

  if( IS_OK(INF_DESKICN) )
  {
    free_desk();
    add_desk(); 	/* reinit desktop */
    cmfree((char **)&z->programs);
    z->num_progs = 0;
  }
  if( IS_OK(INF_APPLIC) )
  {
    cmfree((char **)&z->apps);
    z->num_apps = 0;
  }
  if( IS_OK(INF_EXTENSN) )
  {
    cmfree((char **)&z->extension);
    z->num_ext = 0;
  }
  if( IS_OK(INF_ENVIRON) ) memclr( z->env, 620 );
  if( IS_OK(INF_GUI) )
  {
    memcpy( &z->gui_settings, &gui_settings, sizeof(NSETTINGS) );
    lp = (long *)&z->gui_settings.color_3D.l;
    if( vplanes>=8 ) j = 3;
    else if( vplanes>=4 ) j = 2;
    else if( vplanes>=2 ) j = 1;
    else j = 0;
    for( i=0; i<4; i++ )
     *lp++ = ob_prefs[i][j].l;
  }
  if( IS_OK(INF_AV) ) free_av();
  if( !is_ext )
  {
    z->ttp_params[0][0] = z->ttp_params[1][0] = z->ttp_params[2][0] =
        z->ttp_params[3][0] = z->ttp_params[4][0] = 0;
    z->desk_pic[0] = 0;
    z->desk_in_wind = z->show_pic = z->wall_pic = z->rename_dest = z->filt_copy =
        z->filt_del = z->num_sep = z->open_fold = 0;
    z->wind_pos = z->maximum;
    r = z->cntrl_set;
    z->conf_copy = z->conf_del = z->tos_pause = z->sides = z->pic_colormode =
	prn_param.ffd = z->quit_alert = z->conf_over =
	z->mstcolumn = z->mstlgsml = z->msizdattim[0] = z->msizdattim[1] = z->msizdattim[2] =
	z->mshowicon = *r = *(r+7) = *(r+8) = z->saveconf =
        z->disk_copies = z->snap_over = z->dial_in_wind = z->format_bak =
        z->dskcpy_bak = z->copy_bak = z->del_bak = z->count_copy = z->count_del =
        z->env_parent = z->real_time = 1;
    z->wind_prf.i = 0x180;
    z->wind_font[0].id = z->wind_font[1].id = z->wind_font[2].id = z->wind_font[3].id = 1;
    z->wind_font[0].size = 8;
    z->wind_font[1].size = use_8x16 ? 9 : 8;
    z->wind_font[2].size = z->wind_font[3].size = use_8x16 ? 10 : 9;
    z->view_picopts.mode = z->view_picopts.fit = 1;
    z->desk_picopts.fit = z->desk_picopts.mode = 0;
    /* reset wind_font[4] in load_file() */
    *(r+9) = 3; 				/* dclick 3 */
    *(r+10) = *(r+11) = !fdc_level ? 3 : 6;	/* seek rate */
    *(r+12) = (0x14<<8) | 0x20; /* balance | master volume */
    *(r+13) = (6<<4) | 6;	/* treble | bass */
    *(r+4) = 0xf;		/* STe cache | STe 16 MHz | TT cache | Blit */
    z->back_speed = z->diskcopy = 2;
#ifndef DEMO
    z->use_master = !z->multitask/*003:was chk global*/ && z->dflt_path[0] >= 'C';
#else DEMO
    z->use_master = 0;
#endif
    z->other_pref.i = z->dial_mode = 0;
    z->other_pref.b.virus_check = z->other_pref.b.check_fnames = 1;
    z->move_mode = PREFMASK-PREFMCPY;
    z->use_argv = z->status_report = z->dir_prn.c = z->format_drive = 0;
    z->snapx = 6;
    z->snapy = 1;
    z->tracks = 80;
    z->spt = 9;
    z->spc[0] = 2;
    z->spc[1] = z->spc[2] = 1;
    z->speed = 100;
    z->other_pref.b.clip_mode = 2;
    z->other_pref.b.dflt_twst = z->twst = 3;
    if( CJar( 0, IDT_cookie, &idt )==CJar_OK ) z->idt_fmt = idt;
    else z->idt_fmt = Supexec(get_idt_fmt);
    prn_param.prn_set = 2;
    z->batch_name[0] = z->autoexec[0] = z->text_reader[0] = z->help.topic[0] =
        z->help.hlp_path[0] = '\0';
    z->help.match = z->help.all = 0;
    i = mas->rez;
    z->help.font = 1;
    z->help.point = hp[i];
    z->help.rows = hr[i];
    z->help.cols = hc[i];
    z->help.xoff = z->help.yoff = 0;
    z->snap_resort = z->msort_type = 0;
    *(r+1) = *(r+2) = 0;
    *(r+3) = 5;
    *(r+5) = 300;
    *(r+6) = 40;
    for( i=0; i<6; i++ )
      strcpy( z->template[i], dflt_temps[i] );
    if( err != 99 || !z->pic_ptr ) memcpy( z->pallette, dflt_pall, 16*4 );
    memclr( z->filter, 7*sizeof(FILT_TYPE) );
    memclr( &z->search_filt, sizeof(FILT_TYPE) );
    memclr( &z->copydel_filt, sizeof(FILT_TYPE) );
    z->search_filt.flags.s.allfold = z->copydel_filt.flags.s.allfold =
      z->search_filt.flags.s.make_grp = z->copydel_filt.flags.s.make_grp = 1;
/*%    get_max_icon(-1);
    x_wind_calc( WC_BORDER, WIND_TYPE, XWIND_TYPE, 0, 0,
        2+(max_icon.text_w+3)*(mas->rez?7:3), 4+(max_icon.h+1)*3,
        (int *)&idt, (int *)&idt, &j, &k ); */
    x_wind_calc( WC_BORDER, WIND_TYPE, XWIND_TYPE, 0, 0,
        z->maximum.w - (7<<4) - 20, 4+41*3,
        (int *)&idt, (int *)&idt, &j, &k );
    if( j > (i=z->maximum.w-4) ) j = i;
    for( i=0; i<7; i++ )
    {
      r = &(z->w[i].x);
      *(r+1) = bar_h+4 + (*r = i<<4);
      *(r+2) = j;
      *(r+3) = k;
      z->w[i].place = 0;
      z->filter[i].flags.s.allfold = z->filter[i].flags.s.make_grp = 1;
    }
    center_dials(1);
  }
}
int inf_load( int err, int file, int is_ext )
{
#ifndef DEMO
  int i, j, index;
  INF_OFF *inf, *inf1;
  FUNC_INF_OFF *inf2, *inf3;
  
  reset_inf( err, is_ext );
  inf3 = func_inf_off;
  inf1 = inf_off;
  i = 0;
  index = 100;
  while( !err && !get_line( file ) )
  {
    for( inf=inf1, j=0; !j; )
      if( !strncmp( diskbuff, inf->type, strlen(inf->type) ) )
      {
        if( !inf_conv.is_ok || (*inf_conv.is_ok)(i) )
            scan_inf_line( skip_word(diskbuff), i );
        inf1 = inf;
        j = 1;
      }
      else
      {
        i++;
        if( !(++inf)->type[0] )
        {
          inf = inf_off;
          i = 0;
        }
        if( inf==inf1 ) break;
      }
    if( !j ) for( inf2=inf3; !err; )
      /* optimized by scanning in same order written, starting at
         last found type */
      if( !strncmp( diskbuff, inf2->type, strlen(inf2->type) ) )
      {
        if( !inf_conv.is_ok || (*inf_conv.is_ok)(index) )
            err = (*inf2->read)( skip_word(diskbuff), inf2->start, inf2->fmt );
        inf3 = inf2;
        break;
      }
      else
      {
        index++;
        if( !(++inf2)->type[0] )
        {
          inf2 = func_inf_off;
          index = 100;
        }
        if( inf2==inf3 ) break;
      }
  }
  for( i=0; i<7; i++ )
    if( z->w[i].path[1] != ':' ) z->w[i].place = 0;
  return err;
#else DEMO
  return 1;
#endif
}

void constrain( Rect *r )	/* 003: INF window x, y fixed too */
{
  int j;

  if( r->w > z->maximum.w+2 ) r->w = z->maximum.w;
  if( r->x+r->w < bar_w ) r->x = bar_w - r->w;
  else if( r->x > (j=z->maximum.x+z->maximum.w-bar_w-3) ) r->x = j;
  if( r->h > z->maximum.h+2 ) r->h = z->maximum.h;
  if( r->y < bar_h ) r->y = bar_h;
  else if( r->y > (j=z->maximum.y+z->maximum.h-4) ) r->y = j;
}

void finish_load( char is_ext )
{
  unsigned long qmsg[4];
  int i;

  if( IS_OK(INF_QUEUE) )
    if( check_q(0) )
    {
      qmsg[0] = ((long)PR_INIT << 16) | AES_handle;
      qmsg[3] = (long)&prn_param;
      appl_pwrite( q_handle, 16, qmsg );
      get_ack( (int *)qmsg, q_handle );
    }
    else Setprt( prn_param.prn_set );
  if( IS_OK(INF_PALETTE) )
    for( i=0; i<16; i++ )
      setcolor( i, z->pallette[i] );
  if( IS_OK(INF_GUI) )
  {
    get_wcolors();
    if( gui->xtern.x_settings )		/* 004 */
      for( i=0; i<=WGSIZE; i++ )
        wind_set( 0, X_WF_DCOLSTAT, i, -1, -1,
            z->gui_settings.wstates[z->gui_settings.wcolor_mode][i] );
  }
  if( IS_OK(INF_CONTROL) )
  {
    (*gui->xtern.set_dclick)( z->cntrl_set[9] );
    if( (c_handle=appl_pfind( neocntrl )) >= 0 )
    {
      qmsg[0] = ((long)CNTRL_INIT4 << 16) | AES_handle;
      qmsg[1] = (long)z->cntrl_set;
      qmsg[2] = AES_handle;
      appl_pwrite( c_handle, 16, qmsg );
      get_ack( (int *)qmsg, c_handle );
    }
    else (*mas->set_caches)( ((unsigned int)z->cntrl_set[4]>>8)|(z->cntrl_set[4]<<8) );	/* 003: set_caches has changed */
  }
/*%  wind_update( BEG_UPDATE );*/
  if( !ver_gt_10 && z->other_pref.b.dflt_twst==1 ) z->other_pref.b.dflt_twst = 0;
  w_num = -1;
/*   text_menu_check();  003: causes INF_LOAD to crash */
  w_active = num_w_active = d_active = -1;
  if( is_ext )
  {
    if( !z->num_icons )
    {
      load_fonts();
      install_devices(0);
    }
    get_d_icon(-1);	/* redundant if called from reload */
  }
  for( i=0; i<7; i++ )	/* 002: added */
    constrain( (Rect *)&z->w[i].x );
  *lc->num_sep = z->num_sep;	/* 003 */
  *lc->long_numbers = z->other_pref.b.long_numbers;	/* 005 */
  set_avserv();			/* 003 */
}
void reload( char *name )
{ /* must not alter filename, since *name may point to it */
  int file, extnum=0, old_hand;
  register char *ptr;
  register int err, i;
  int j, k;
  char *namptr, is_ext;
  static char dflt_extns[][5] = { ".PRG", ".APP", ".TOS", ".TTP", ".GTP", ".BAT",
      ".BTP", ".NPG", ".NTP", ".ACC" };
  static unsigned char dflt_type[] = { 16, 16, 24, 26, 18, 20, 22, 17, 19, 16 };

#ifndef DEMO
  is_ext = inf_conv.is_ok != 0L;
  bee();
#else
  is_ext = 0;
#endif DEMO
nextextn:
  err = 0;
  if( name == 0L )
  {
    strcpy( filename, z->dflt_path );
    strcat( filename, z->inf_name );
    strcat( filename, ext[extnum] );
    namptr = filename;
  }
  else
  {
    namptr = name;
    extnum = 99;
  }
#ifndef DEMO
  if( (file = cFopen( namptr, 0 )) < 0 ) err = 99 - (name != 0L);
#else DEMO
  err = 99;
#endif DEMO

  ptr = namptr+find_extn(namptr);
  for( i=1; i<=2; i++ )
    if( !strcmp( ptr, ext[i] ) )
    {
      load_file( err, file, i, name!=0L, namptr );
      goto incextn;
    }

#ifndef DEMO
  init_inf_offs();
  if( !err )
    if( (err = (*mas->get_line)( file, diskbuff, 1, 1 )) == 0 )
      if( strcmp( diskbuff, INF_VER ) ) err = 9 - (name != 0L);

  if( !err ) err = get_line(file);			/* skip rez */
#endif DEMO
  if( dtop_handle>0 )
  {
    set_newdesk( 0L, 1 );
    old_hand = dtop_handle;
    dtop_handle = -1;
  }
  else old_hand = 0;
#ifndef DEMO
  err = inf_load( err, file, is_ext );
  if( err<0 ) err = 0;	/* it was an out of memory error */
#endif DEMO

  if( err < 98 ) cFclose(file);
  if( err == 98 ) missing_file(namptr);
  else if( err == 8 )
  {
    spf( tmpf, msg_ptr[56], ext[0] );
    f_alert1( tmpf );
  }
  else if( err )
  {
#ifndef DEMO
    if( !strcmp( spathend(namptr), ext[0] ) ) f_alert1( msg_ptr[67] );
    else
    {
      spf( tmpf, msg_ptr[err==99 ? 57 : 58], z->inf_name );
      f_alert1( tmpf );
    }
    if( err != 99 ) reset_inf( err, 0 );	/* don't bother resetting again */
#else DEMO
    reset_inf( err, 0 );
#endif DEMO
  }
  else
  {
    ptr = spathend(namptr);
    i=0;
    while( *ptr && *ptr!='.' )
      z->new_inf_name[0][i++] = *ptr++;
    z->new_inf_name[0][i] = '\0';
  }
  if( !z->num_ext )
    for( i=0; i<(z->multitask?10:9) && (j=add_extn())>=0; i++ )
    {
      strcpy( z->extension[j].extns, dflt_extns[i] );
      z->extension[i].type.c = dflt_type[i];
    }
  if( !z->num_icons )
  {
    load_fonts();
    install_devices(0);
  }
  get_d_icon(-1);
  if( old_hand )
  {
    dtop_handle = old_hand;
    set_newdesk( z->desk, 1 );
  }
/*%  wind_update( END_UPDATE );*/
/*%  showicon = z->showicon[0];		003
  stlgsml = z->stlgsml[0];
  stcolumn = z->stcolumn[0];
  sort_type = z->sort_type[0];
  memcpy( sizdattim, z->sizdattim[0], 3*sizeof(int) ); */
  finish_load( is_ext );
incextn:
  arrow();
  if( ++extnum < 3 ) goto nextextn;
}

#define ICNPNT	(*(ICONSAVE **)point)
PROG_TYPE desk_type;
int dicon_type;
int winf_deskicn( int num, void **point, void **out )
{
  int typ;
  
  if( num >= z->num_icons ) return 0;	/* 003: moved condition here */
  if( (typ=ICNPNT->type) == -1 )
  {	/* no icon, so skip it */
    ++ICNPNT;
    return -1;
  }
  else if( typ >= D_PROG )
  {
    dicon_type = 99;
    *out++ = &dicon_type;
  }
  else *out++ = &ICNPNT->type;
  *out++ = &z->desk[num+1].ob_x;
  *out++ = &z->desk[num+1].ob_y;
  *out++ = &ICNPNT->c;
  if( typ <= RAMDISK && ICNPNT->state&4 ) *out++ = "\xe5";	/* 004 */
  else *out++ = ICNPNT->label;
  if( typ >= D_PROG )
  {
    *out++ = &z->programs[typ-D_PROG].p.type.i;
    *out   = z->programs[typ-D_PROG].p.path;
  }
  else
  {
    desk_type.i = 0;
    tmpf[0] = 0;
    *out++ = &desk_type.i;
    *out = tmpf;
  }
  ++ICNPNT;
  return 1;
}

#define APPPNT	(*(APP **)point)
int winf_applic( int num, void **point, void **out )
{
  if( num >= z->num_apps ) return 0;	/* 003: moved condition here */
  *out++ = &APPPNT->type.i;
  *out++ = &APPPNT->flags.i;
  *out++ = &APPPNT->extn;
  *out++ = APPPNT->name;
  *out   = APPPNT->path;
  ++APPPNT;
  return 1;
}

#define EXTPNT	(*(EXTENSION **)point)
int winf_extens( int num, void **point, void **out )
{
  if( num >= z->num_ext ) return 0;	/* 003: moved condition here */
  *out++ = &EXTPNT->type.c;
  *out   = EXTPNT->extns;
  ++EXTPNT;
  return 1;
}

#pragma warn -par
#define ENVPNT	(*(char **)point)
int winf_env( int num, void **point, void **out )
{
  *out = ENVPNT;
  ENVPNT += strlen(ENVPNT) + 1;
  return **(char **)out != 0;
}
#pragma warn +par

static int dialnum;
#define DIALPNT	(*(int **)point)
int winf_dialog( int num, void **point, void **out )
{
  if( num>MAX_DIAL-FILTER+1 ) return 0;	/* last dial is ABOUT */ /* 003: moved condition here */
  dialnum = num;
  *out++ = &dialnum;
  *out = DIALPNT;
  DIALPNT += 2;
  return 1;
}

#pragma warn -par
#define AVPNT	(*(AV_INF **)point)
int winf_av( int num, void **point, void **out )
{
  if( !AVPNT ) return 0;
  *out++ = AVPNT->name;
  *out = AVPNT->data;
  AVPNT = AVPNT->next;
  return 1;
}
#pragma warn +par

int rinf_deskicn( char *buf, void **point, char *fmt )
{
  int i, j;
  void *list[20];
  void *parm;
  
  if( (i=add_desk()) < 0 ) return -1;
  parm = (void *)(*(ICONSAVE **)point + --i);
  ((ICONSAVE *)parm)->type = 0;
  if( !winf_deskicn( i, &parm, list ) ) return 1;
  get_pointers( buf, fmt, list );
  if( z->idat[i].type >= D_PROG )
  {
    if( (j=add_program( tmpf, &desk_type )) < 0 ) return -1;
    z->idat[i].type = j+D_PROG;
  }
  else if( !strcmp( z->idat[i].label, "\xe5" ) )	/* 004 */
  {
    z->idat[i].label[0] = 0;
    z->idat[i].state |= 4;
  }
  return 0;
}
int rinf_applic( char *buf, void **point, char *fmt )
{
  int i;
  void *list[20];
  void *parm;
  
  if( (i=add_app()) < 0 ) return -1;
  parm = (void *)(*(APP **)point + i);
  if( !winf_applic( i, &parm, list ) ) return 1;
  get_pointers( buf, fmt, list );
  return 0;
}
int rinf_extens( char *buf, void **point, char *fmt )
{
  int i;
  void *list[20];
  void *parm;
  
  if( (i=add_extn()) < 0 ) return -1;
  parm = (void *)(*(EXTENSION **)point + i);
  if( !winf_extens( i, &parm, list ) ) return 1;
  get_pointers( buf, fmt, list );
  return 0;
}
#pragma warn -par
int rinf_env( char *buf, void **point, char *fmt )
{
  char *ptr = z->env;
  
  while( *(ptr+1) ) ptr += strlen(ptr)+1;
  strcpy( ptr, buf );
  *(ptr+strlen(ptr)+1) = 0;
  return 0;
}
#pragma warn +par
int rinf_dialog( char *buf, void **point, char *fmt )
{
  void *list[20];
  long temp, *ptr=&temp;
  
  winf_dialog( 0, (void **)&ptr, list );
  get_pointers( buf, fmt, list );
  if( dialnum > MAX_DIAL-FILTER+1 ) return 0;
  *(*(long **)point+dialnum) = temp;
  return 0;
}
#pragma warn -par
int rinf_av( char *buf, void **point, char *fmt )
{
  void *parm, *list[20];
  AV_INF *av;

  if( (av = add_av()) == 0 ) return -1;
  parm = (void *)av;
  if( !winf_av( 0, &parm, list ) )
  {
    lfree(av);
    return 1;
  }
  get_pointers( buf, fmt, list );
  return 0;
}
#pragma warn +par

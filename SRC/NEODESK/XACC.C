#include "new_aes.h"
#include "xacc.h"
#include <string.h>
#include <time.h>
#include <tos.h>
#include <multevnt.h>
#include "xwind.h"
#include "neodesk.h"
#include "mwclinea.h"
#include "neocommn.h"
/*#include "settings.h"*/
#include "neod2_id.h"
/*#include "ctype.h"*/
#include "guidefs.h"	/* must come after aes.h and neocommn.h */
#include "kobold.h"

#define TRUE 1
#define FALSE 0
#define NULL 0L

int AvServer=-1,_xacc_msgs;
static char *XAccName,*AVName,a_name[9];
static int av_msgs,va_msgs,multi_xacc=FALSE;
static XAcc Xaccs[MAX_XACCS];

static void	store_xacc_id(int*);
static void	store_av_id(int*);
static void	kill_id(int,int);

static void	XAccSendId(int);
static void	XAccSendAcc(int);
static void XAccSendAccClose(void);
static int XAccWait(int);
static int av_openacc( char *path, int id, int ignore, int type );

static void AvProtocol(int);

extern int AES_handle, menu_id, w_num, w_handle, drag_x, drag_y,
    dtop_handle, witems, snum;
extern char aes_ge_40, has_magx, *kobold_buf, tmpf[];
extern GUI *gui;
extern LoadCookie *lc;
extern MASTER *mas;
extern MOST *z;
extern char *tail, filename[];
extern FSTRUCT *wfile;

static char *getenv(const char *var)
{
	char *s = mas->parent_env;
	const char *v;

	if (s)
		while (*s)
		{
			for (v=var; (*s) && (*s++ == *v++); )
				if ((*s == '=') && (*v == '\0'))
					return(++s);
			while (*s++);
		}
	return(NULL);
}

int AvSendMsg(int sendto,int msg_id,int *msg)
{
	if (sendto>=0)
	{
		XAcc *x;
		int err, id;

		if( (x = find_id(sendto)) != 0 )
		  if( (id=appl_find(x->name)) != sendto )
		    if( id<0 )
		    {
			kill_id(AV|XACC,sendto);
			return FALSE;
		    }
		    else msg_id = x->id = id;

		msg[0] = msg_id;
		msg[1] = AES_handle;
		msg[2] = 0;

		err = appl_write(sendto,16,msg);

		if (err>0)
			return (TRUE);
		else if (err<0 && sendto!=AES_handle)
			kill_id(AV|XACC,sendto);
	}

	return (FALSE);
}

static int XAccSend(int sendto,int msg0,int msg3,char *msg4,int msg6,int msg7)
{
	if (sendto>=0 && sendto!=AES_handle)
	{
		int msg[8],err;

		msg[0] = msg0;
		msg[1] = AES_handle;
		msg[2] = 0;
		msg[3] = msg3;
		*(char **) &msg[4] = msg4;
		msg[6] = msg6;
		msg[7] = msg7;

		err = appl_write(sendto, 16, msg);

		if (err>0)
			return (TRUE);
		else if (err<0)
			kill_id(AV|XACC,sendto);
	}

	return (FALSE);
}

int XAccSendAck(int sendto, int answer)
{
	return (XAccSend(sendto,ACC_ACK,(answer) ? 1 : 0,NULL,0,0));
}

int XAccSendText(int sendto, char *text)
{
	if (XAccSend(sendto, ACC_TEXT, 0, text, 0, 0)==TRUE)
		return (XAccWait(2000));
	else
		return (FALSE);
}

int XAccSendKey(int sendto, int scan, int state)
{
	if (XAccSend(sendto, ACC_KEY, scan, (char *) (((long) state)<<16), 0, 0)==TRUE)
		return (TRUE);
	else
		return (FALSE);
}

int XAccSendMeta(int sendto, int last, char *data, long len)
{
	if (XAccSend(sendto, ACC_META, (last) ? 1: 0, data, (int) (len>>16), (int) len)==TRUE)
		return (XAccWait(5000));
	else
		return (FALSE);
}

int XAccSendImg(int sendto, int last, char *data, long len)
{
	if (XAccSend(sendto, ACC_IMG, (last) ? 1: 0, data, (int) (len>>16), (int) len)==TRUE)
		return (XAccWait(5000));
	else
		return (FALSE);
}

static void XAccSendId(int sendto)
{
	XAccSend(sendto, ACC_ID, (XACCVERSION<<8)|XACC_LEVEL, XAccName, menu_id, 0);
}

static void XAccSendAcc(int sendto)
{
	XAccSend(sendto, ACC_ACC, (XACCVERSION<<8)|XACC_LEVEL, XAccName, menu_id, AES_handle);
}

void _XAccAvExit(void)
{
	register XAcc *xacc;
	register int i;
	int msg[8];

	if (multi_xacc)
	{
		msg[3] = AES_handle;

		for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
		{
			if (xacc->flag & AV)
				AvSendMsg(xacc->id,AV_EXIT,msg);

			if (xacc->flag & XACC)
				AvSendMsg(xacc->id,ACC_EXIT,msg);
		}
	}
}

static void XAccInformAcc(int *msg)
{
	register XAcc *xacc;
	register char *name = *((char **) &msg[4]);
	register int i;

	for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
		if (xacc->flag & XACC)
			XAccSend(xacc->id,ACC_ACC,msg[3],name,msg[6],msg[1]);
}

static void XAccSendAccClose(void)
{
	if (z->is_acc && !multi_xacc)
	{
		kill_id(AV|XACC,0);
		AvServer = -1;
		XAccSendId(0);
		AvProtocol(0);
	}
}

int AppLoaded(char *app)
{
	register char buf[9], *p, *q;
	register int i,id;

	if (app==NULL)
		return (-1);

	p = q = app;
	while ((q=strpbrk(p,":\\/"))!=NULL)
	{
		q++;
		p = q;
	}

        strcpy( buf, p );
	for (i=0;i<8;i++)
		if(*p>' ' && *p!='.')
			buf[i] = *p++;
		else
			buf[i] = ' ';
	buf[8] = '\0';

	id = appl_pfind(buf);
	if (id==AES_handle)
	{
		z->other_pref.b.av_server = 1;
		set_avserv();
		id = -1;
	}

	return (id);
}

void XAccBroadCast(int *msg)
{
	register XAcc *xacc;
	register char name[10];
	register int i;
	int next,type,id;
	int dummy,search;

	if (aes_ge_40 || has_magx /*|| (appl_getinfo(4,&dummy,&dummy,&search,&dummy) && search>0)*/ )
	{
		next = 0;
		while (appl_search(next, name, &type, &id))
		{
			if (id!=AES_handle && (type & 0x06))
				AvSendMsg(id,msg[0],msg);
			next = 1;
		}
	}
	else
		for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
			if (xacc->id>=0)
				AvSendMsg(xacc->id,msg[0],msg);
}

void _XAccSendStartup(char *xacc_name, char *av_name, int Avmsgs,int Vamsgs, int xacc_msgs)
{
	register char name[10], *buf; /*, *p;*/
/*	int xacc_size,av_size;*/
	int i,next,type,id,dummy,search;

	XAccName = /*p =*/ xacc_name;

/*	while (*p++);
	if (!stricmp(p,"XDSC"))
	{
		do
		{
			while (*p++);
		} while (*p);
	}

	xacc_size = (int) (p - xacc_name); */

/*	strcpy(a_name,"        ");
	AVName = memcpy(a_name,av_name,min((int) strlen(av_name),8));*/
	AVName = av_name;
/*	av_size = 8+1;*/

	av_msgs = Avmsgs;
	va_msgs = Vamsgs|MSG_EXIT|MSG_SENDKEY|MSG_ACCWINDOPEN;
	_xacc_msgs = xacc_msgs;

	for (i=0;i<MAX_XACCS;i++)
		Xaccs[i].id = -1;

/*	if (mint)
	{
		buf = Mxalloc(xacc_size+av_size, 0x0023);
		if (buf!=NULL)
		{
			XAccName = memcpy(buf, XAccName, xacc_size);
			buf += xacc_size;
			AVName = memcpy(buf, AVName, av_size);
		}
	} */

	if (aes_ge_40 || has_magx /*|| (appl_getinfo(4,&dummy,&dummy,&search,&dummy) && search>0)*/ )
	{
		multi_xacc = TRUE;

		next = 0;
		while (appl_search(next, name, &type, &id))
		{
			if (id!=AES_handle && (type & 0x06))
				XAccSendId(id);
			next = 1;
		}
	}

	id = AppLoaded(getenv("AVSERVER"));
	if (id<0)
		id = appl_find("AVSERVER");
	if (id<0)
		id = appl_find("GEMINI  ");
	if (id<0)
		id = appl_find("VENUS   ");

	if (id==AES_handle)
		AvServer = -1;
	else
		AvServer = id;

	if (z->is_acc & !multi_xacc)
		XAccSendAccClose();
	else if (AvServer>=0)
		AvProtocol(AvServer);
}

static int XAccWait(int wait)
{
	EMULTI event;
	int msg[8];

	event.type = MU_MESAG|MU_TIMER;
	event.high = 0;
	event.low = wait;

	for(;;)
	{
		multi_evnt( &event, msg );
		if( !(event.event&MU_MESAG) ) break;
		switch (msg[0])
		{
		case ACC_ACK:
			return((msg[3]==1) ? TRUE : FALSE);
		case AC_OPEN:
		case AC_CLOSE:
			if (!z->is_acc)
				break;
		case AP_TERM:
				return (FALSE);
		}
	}

	return(FALSE);
}

static void AvProtocol(int id)
{
	int msg[8];

	if (id!=AES_handle)
	{
		msg[3] = av_msgs;
		msg[4] = msg[5] = 0;
		*(const char **) &msg[6] = AVName;
		AvSendMsg(id,AV_PROTOKOLL,msg);
	}
}

static void AvProtoStatus(int id)
{
	int msg[8];

	if (id!=AvServer && id!=AES_handle)
	{
		msg[3] = va_msgs;
		msg[4] = msg[5] = 0;
		*(const char **) &msg[6] = AVName;
		AvSendMsg(id,VA_PROTOSTATUS,msg);
	}
}

static AV_INF *find_av( XAcc *x )
{
  AV_INF *av;

  for( av=z->av_status; av; av=av->next )
    if( !strcmp( av->name, x->name ) )
      return av;
  return 0L;
}

static void store_avstat(int *msg)
{
  AV_INF *av;
  XAcc *x;

  if( (x = find_id(msg[1])) == 0L ) return;
  if( (av = find_av(x)) == 0L )
    if( (av = add_av()) == 0L ) return;
  strcpy( av->name, x->name );
  strcpy( av->data, *(char **)&msg[3] );
}

static void get_avstat(int *msg)
{
  AV_INF *av;
  XAcc *x;

  if( (x = find_id(msg[1])) == 0L ) av=0L;
  else av = find_av(x);
  *(char **)&msg[3] = av ? av->data : 0L;
  msg[5] = msg[6] = msg[7] = 0;
  AvSendMsg(msg[1],VA_SETSTATUS,msg);
}

static int *find_avwind(int *msg)
{
  XAcc *x;
  int i, *w;

  if( (x = find_id(msg[1])) == 0L ) return 0L;
  for( i=sizeof(x->winds)/sizeof(int), w=x->winds; --i>=0; w++ )
    if( *w==msg[3] ) return w;
  for( i=sizeof(x->winds)/sizeof(int), w=x->winds; --i>=0; w++ )
    if( !*w ) return w;
  return 0L;
}

static void av_windclose(int *msg)
{
  int *i;

  if( (i = find_avwind(msg)) == 0L ) return;
  *i = 0;
}

static void av_windopen(int *msg)
{
  int *i;

  if( (i = find_avwind(msg)) == 0L ) return;
  *i = msg[3];
}

static void av_what(int *msg)
{
  int wn, wh, i;
  static char temp[120];
  
  wh = wind_find( msg[3], msg[4] );
  msg[3] = AES_handle;
  msg[4] = VA_OB_UNKNOWN;
  *(char **)&msg[5] = 0L;
  if( wh==dtop_handle )
  {
    if( (wn = find_d( msg[3], msg[4] )) >= 0 )
	switch( z->idat[wn].type )
	{
	  case FLOPPY:
	  case HARDDSK:
	  case RAMDISK:
	    if( (*temp = get_drive(wn+1)) != 0 )
	    {
	      strcpy( temp+1, ":\\" );
	      msg[4] = VA_OB_DRIVE;
              *(char **)&msg[5] = temp;
	    }
	    break;
	  case PRINTER:
	    break;
	  case TRASH:
	    msg[4] = VA_OB_SHREDDER;
	    break;
	  case CLIPBRD:
	    msg[4] = VA_OB_CLIPBOARD;
	    break;
	  default:
	    msg[4] = desk_path( temp, wn ) ? VA_OB_FOLDER : VA_OB_FILE;
            *(char **)&msg[5] = temp;
	    break;
	}
  }
  else if( (wn = wind_xref(wh)) >= 0 )
    if( (i=find_w( msg[3], msg[4], wh )) >= 0 )
    {
      get_full_name( temp, i, wn );
      if( z->file[wn][i].type.p.pexec_mode == FOLDER )
      {
        strcat( temp, "\\" );
        msg[4] = VA_OB_FOLDER;
      }
      else msg[4] = VA_OB_FILE;
      *(char **)&msg[5] = temp;
    }
    else
    {
      msg[4] = VA_OB_WINDOW;
      *(char **)&msg[5] = z->w[wn].path;
    }
  AvSendMsg( msg[1], VA_THAT_IZIT, msg );
}

char *next_str( char *s )
{
  return s + strlen(s) + 1;
}

int avopen_to_path( char *p, char *xt )
{
  int ret, i;
  FSTRUCT *fs;

  if( (ret = open_to_path(p)) != 0 && xt && !strcmp( xt = next_str(xt), "SLCT" ) )
  {
    de_act( -1, -1 );
    for(;;)
    {
      if( !*(xt = next_str(xt)) ) break;
      snum = 1;
      reset_icons();
      for( fs=wfile, i=0; i<witems; i++, fs++ )
        if( !strcmp( fs->name, xt ) )
        {
          cond_arrow(i);
          select_w( i, 1, w_handle, 1 );
          break;
        }
      info();
    }
  }
  return ret;
}

void send_vaproto( int *msg )
{
  XAcc *x;

  if( (x=find_id(msg[1])) == 0 ) AvProtoStatus(msg[1]);
  else if( !(x->flag&AV) )
  {
    x->flag |= AV;
    strcpy( x->name, x->xname );
  }
}

void open_va_item( char *p )	/* 004 */
{
  char *q;
  PROG_TYPE pt;

  while(p)
  {
    if( (q = strchr(p,' ')) != 0 )
    {
      strncpy( filename, p, q-p );
      filename[q-p] = 0;
      q++;
    }
    else strcpy( filename, p );
    if( !*spathend(filename) ) open_to_path(filename);
    else
    {
      pt = iprog_type( -1, filename );
      _open_w_icon( 0, filename, 0, EDW_DISK, &pt, pt.p.pexec_mode, 0 );
    }
    p = q;
  }
}

int updt_kobold( char *s, int keep )
{
  char *p, *p2;

  if( (p = strstr(kobold_buf,s)) != 0 &&
      (p2 = strchr(p+3,' ')) != 0 )
  {
    *p2 = 0;
    update_drive( p+3, keep );
    *p2 = ' ';
    return 1;
  }
  return 0;
}

int _XAccComm(int *msg)
{
	WIND_FONT *w;
	static char sent_resp;

	if( z->other_pref.b.av_server || msg[0] < AV_PROTOKOLL ||
	    msg[0] > VA_DRAG_COMPLETE ||
	    msg[0]==VA_START || msg[0]==AV_OPENWIND || msg[0]==AV_STARTPROG )		/* 004 */
	    switch (msg[0])
	{
	case AC_CLOSE:
		XAccSendAccClose();
		return (FALSE);
	case ACC_ACC:
		if (multi_xacc)
		{
			if (msg[7]<=0 || msg[1]==msg[7])
			{
				AvProtoStatus(msg[1]);
				store_xacc_id(msg);
			}
		}
		else if (z->is_acc)
		{
			msg[1] = msg[7];
			XAccSendId(msg[1]);
			store_xacc_id(msg);
		}
		break;
	case ACC_ID:
		if (multi_xacc)
		{
			XAccSendAcc(msg[1]);
			AvProtoStatus(msg[1]);
		}
		else if (!z->is_acc)
		{
			XAccInformAcc(msg);
			XAccSendId(msg[1]);
		}
		store_xacc_id(msg);
		break;
	case AV_EXIT:
		kill_id(AV|XACC,msg[3]);
		break;
	case ACC_EXIT:
		kill_id(XACC,msg[1]);
		break;
	case AV_PROTOKOLL:
		AvProtoStatus(msg[1]);
	case VA_PROTOSTATUS:
		store_av_id(msg);
		break;
	case VA_START:
		open_va_item(*(char **)&msg[3]);
		break;
	case AV_STATUS:
		store_avstat(msg);
		send_vaproto(msg);
		break;
	case AV_GETSTATUS:
		get_avstat(msg);
		send_vaproto(msg);
		break;
	case AV_ASKFILEFONT:
		send_vaproto(msg);
		w = &z->wind_font[w_num>=0 ? z->stlgsml[w_num]+1 : 1];
		msg[3] = w->id;
		msg[4] = w->size;
		AvSendMsg(msg[1],VA_FILEFONT,msg);
		break;
	case AV_ASKOBJECT:
		send_vaproto(msg);
		av_openacc( "", msg[1], -1, VA_OBJECT );
		break;
	case AV_OPENWIND:
		if( z->other_pref.b.av_server/*004*/ ) send_vaproto(msg);
		msg[3] = avopen_to_path( *(char **)&msg[3], *(char **)&msg[5] );
		AvSendMsg( msg[1], VA_WINDOPEN, msg );
		break;
	case AV_STARTPROG:
		if( z->other_pref.b.av_server/*004*/ ) send_vaproto(msg);
		/* 004: handle length byte correctly */
		if( *(char **)&msg[5] )
		{
	      	  tmpf[0] = ' ';
	          strcpy( tmpf+1, *(char **)&msg[5] );
	        }
	        else strcpy( tmpf, "\0" );
		run_acc_prog( 0, *(char **)&msg[3], tmpf, &msg[4], 1 );
		msg[3] = 1;	/* always say it started successfully */
		msg[4] = msg[5] = msg[6] = 0;
		AvSendMsg( msg[1], VA_PROGSTART, msg );
		break;
	case AV_ACCWINDOPEN:
		av_windopen(msg);
		send_vaproto(msg);
		break;
	case AV_ACCWINDCLOSED:
		av_windclose(msg);
		send_vaproto(msg);
		break;
	case AV_COPY_DRAGGED:	/* should actually copy to another path... */
		send_vaproto(msg);
		msg[3] = 0;	/* copy failed */
		AvSendMsg( msg[1], VA_COPY_COMPLETE, msg );
		break;
	case AV_PATH_UPDATE:
		update_drive( *(char **)&msg[3], 0 );
		send_vaproto(msg);
		break;
	case AV_WHAT_IZIT:
		send_vaproto(msg);
		av_what(msg);
		break;
	case KOBOLD_ANSWER:
		if( kobold_buf )
		  if( !sent_resp )
		  {
		    AvSendMsg( msg[1], KOBOLD_FREE_DRIVES, msg );
		    sent_resp = 1;
		  }
		  else
		  {
		    if( strstr(kobold_buf,"#13") )
		    {
		      updt_kobold( "#1 ", 0 );
		      updt_kobold( "#0 ", 1 );
		    }
		    else if( !updt_kobold( "#1 ", 0 ) )
		        updt_kobold( "#0 ", 0 );
		    cmfree(&kobold_buf);
		    sent_resp = 0;
		  }
		break;
	default:
		return (FALSE);
	}

	return (TRUE);
}

static void store_av_id(int *msg)
{
	register XAcc *xacc=Xaccs;
	register int id=msg[1],i;

	if (id==AES_handle || id<0)
		return;

	if (id==0 && z->is_acc && !multi_xacc)
		AvServer = 0;

	for (i=0;i<MAX_XACCS && xacc->id>=0;xacc++,i++)
		if (xacc->id==id)
			break;

	if (i<MAX_XACCS)
	{
		xacc->flag |= AV;
		xacc->avflag = msg[3];

		xacc->id = id;
		strncpy(xacc->name,*((char **) &msg[6]),8);
	}
}

static void store_xacc_id(int *msg)
{
	register XAcc *xacc;
	register int id=msg[1],i;
	register char *xdsc;

	if (id==AES_handle || id<0)
		return;

	i=0;
	if( (xacc = find_id(id)) == 0 )
	    for (xacc=Xaccs;i<MAX_XACCS && xacc->id>=0;xacc++,i++)
		if (xacc->id==id)
			break;

	if (i<MAX_XACCS)
	{
		xacc->flag |= XACC;

		xacc->id = id;
		xacc->version = msg[3];
		xacc->menu_id = msg[6];
		xacc->xname = *((char **) &msg[4]);
		xacc->xdsc = NULL;
		if (xacc->xname!=NULL)
		{
			xdsc = xacc->xname + strlen(xacc->xname)+1;
			if (!strcmp(xdsc,"XDSC"))
				xacc->xdsc = xdsc+5;
			if( !(xacc->flag & AV) ) strcpy( xacc->name, xacc->xname );
		}

/*		_send_msg(xacc,id,XACC_AV_INIT);*/
	}
}

static void kill_id(int flag,int id)
{
	register XAcc *xacc;
	register int i;

	if (id<0)
		return;

	for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
		if (id==xacc->id)
		{
			xacc->flag &= ~flag;
			if (id==AvServer && (xacc->flag & AV)==0)
				AvServer = -1;

			if (xacc->flag==0)
			{
				memclr(xacc,sizeof(XAcc));
				xacc->id = -1;
			}
			else if (flag==AV)
			{
				memclr(xacc->winds,sizeof(xacc->winds));
				xacc->name[0] = '\0';
			}
			else
			{
				xacc->xname = xacc->xdsc = NULL;
				xacc->version = xacc->menu_id = 0;
			}

/*			_send_msg(xacc,id,XACC_AV_EXIT);*/
/*			return;   just in case there are dups, don't */
		}
}

char *get_acc_name( char *name, char *temp )
{
  register char *p;
  PROG_TYPE pt;

  strcpy( temp, spathend(name) );
  if( (p = strchr(temp,'.')) == 0 ) return 0L;
  if( strcmp(p,".ACC") )
  {
    pt = iprog_type( -1, name );
    if( pt.p.pexec_mode!=PROG ) return 0L;
  }
  return p;
}

int find_xacc_name(char *name)
{	/* 004: now returns id instead of xacc */
	register XAcc *xacc;
	register char *p;
	register int i, l;
	char temp[14];

	if( !z->other_pref.b.av_server ) return -1;
	if( (p = get_acc_name( name, temp )) == 0 ) return -1;
	*p = 0;
	l = strlen(temp);
	for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
	  if (xacc->id>=0 && !strncmp(xacc->name,temp,l) &&
	     (!xacc->name[l] || xacc->name[l]==' ') ) return xacc->id;
	if( z->other_pref.b.prevent_mult ) return appl_pfind(temp);	/* 004 */
	return -1;
}

static int av_openacc( char *path, int id, int ignore, int type )
{
  int msg[8], i;

  if( (i=create_tail( ignore, 1, 0L, path, 0L, 0 )) != 0 )
  {
    *(char **)&msg[3] = i>0 && tail[0] ? tail+1 : NULL;
    AvSendMsg( id, type, msg );
  }
  return 1;
}

int av_open( char *path, int doit )
{
  int x, msg[8];

  if( (x = find_xacc_name(path)) < 0 ) return 0;
  if( doit )
  {
    *(char **)&msg[3] = tail[0] ? tail+1 : NULL;
    AvSendMsg( x, VA_START, msg );
  }
  return 1;
/*  return av_openacc( "", x, ignore, VA_START ); */
}

XAcc *find_xacc_xdsc(int id,char *dsc)
{
	register XAcc *xacc;
	register char *xdsc;
	register int i;

	if( !z->other_pref.b.av_server ) return 0L;
	for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
		if (xacc->id>=0 && (id<0 || xacc->id==id))
		{
			if ((xdsc=xacc->xdsc)!=NULL)
			{
				while (*xdsc!='\0')
				{
					if (!strcmp(xdsc,dsc))
						return (xacc);
					xdsc += strlen(xdsc)+1;
				}
			}

			if (xacc->id==id)
				break;
		}

	return (NULL);
}

static int next=-1;

XAcc *find_app(int first)
{
	register XAcc *xacc;
	register int i;

	if (first)
		i = 0;
	else if (next<0 || next>=MAX_XACCS)
		return (NULL);
	else
		i = next;

	for (xacc=&Xaccs[i];i<MAX_XACCS;xacc++,i++)
		if (xacc->id>=0)
		{
			next = i+1;
			return (xacc);
		}

	next = -1;
	return (NULL);
}

XAcc *find_id(int id)
{
	register XAcc *xacc;
	register int i;

	if (id<0)
		return (NULL);

	for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
		if (xacc->id==id)
			return (xacc);

	return (NULL);
}

void set_avserv(void)
{
  int i, j, msg[8];
  XAcc *xacc;

  i = z->other_pref.b.av_server ? AES_handle : -1;
  if( (j=*lc->avserver) != i )
    if( (*lc->avserver = i) < 0 && j==AES_handle )
    {
      msg[3] = AES_handle;
      for (i=0,xacc=Xaccs;i<MAX_XACCS;xacc++,i++)
	if (xacc->flag & AV) AvSendMsg(xacc->id,AV_EXIT,msg);
    }
}

void free_av(void)
{
  AV_INF *av, *av2;
  
  for( av2=z->av_status; (av=av2)!=0; )
  {
    av2 = av->next;
    lfree(av);
  }
  z->av_status = 0L;
}

AV_INF *add_av(void)
{
  AV_INF **av, *n;
  
  if( (n = lalloc( sizeof(AV_INF), ALLOC_MAS )) == 0 ) return 0L;
  for( av=&z->av_status; *av; av=&(*av)->next );
  n->next = 0L;
  *av = n;
  return n;
}

int wf_owner( int wh )
{
  int i, dum;

  if( aes_ge_40 || has_magx )
  {
    appl_getinfo( 11, &i, &dum, &dum, &dum );
    if( i&(1<<4) )	/* 005: was &4 */
    {
      wind_get( wh, WF_OWNER, &i, &dum, &dum, &dum );
      return i;
    }
  }
  return -1;
}

void _av_dragto( int wh, int id )
{
  int msg[8];

  if( create_tail( -1, 1, 0L, "", 0L, 0 ) && tail[0] )
  {
    msg[3] = wh;
    msg[4] = drag_x;
    msg[5] = drag_y;
    *(char **)&msg[6] = tail+1;
    AvSendMsg( id, VA_DRAGACCWIND, msg );
  }
}

void av_dragto(void)
{
  int wh, i, j, *w;
  XAcc *x;
  
  if( !z->other_pref.b.av_server ) return;
  if( (wh = wind_find(drag_x,drag_y)) != 0 && wind_xref(wh)<0 )
  {
    for( x=Xaccs, i=MAX_XACCS; --i>=0; x++ )
      if( x->id>=0 )
        for( w=x->winds, j=sizeof(x->winds)/sizeof(int); --j>=0; w++ )
          if( *w == wh )
          {
            _av_dragto( wh, x->id );
            return;
          }
    if( (i=wf_owner(wh)) >= 0 ) av_openacc( "", i, -1, VA_START );	/* 004 */
  }
}

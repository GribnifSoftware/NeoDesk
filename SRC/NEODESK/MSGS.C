char *msg_ptr[] = {
"[3][|Do you want to abort?][Abort|Continue]",
"MDYDMYYMDYDM",
" format",
"[0][   Copy or Move items?    |][Copy|Move|Cancel]",
"[1][Either you have not chosen|a batch file processor or|the one\
 you chose does|not exist! Please check the|Set Preferences item.][Oops!]",
"[3][Sorry, this operation would|result in a folder being|%sed into\
 itself and|therefore cannot be performed.][Hmph!]",
"mov",
"copi",
"[1][|Printer not ready!][Retry|Cancel]",
"[1][The printer queue,|NEOQUEUE.ACC is not present.|Please\
 re-boot with this|file in the root directory.][Ok]",
"[1][Only disk drives and|folders can be copied|to other disk drives][Ok]",   /* 10 */
"[1][|Unrecognized desktop|picture format][Ugh]",
"[3][The other item is a folder.|Please either enter a new|name or hit Skip]\
[Ok]",
"[1][|%s could not|be moved!][Ok]",
"[1][Access denied! You may be|trying to create more than the|maximum number\
 of items|in the root directory.|(floppy=112, hard disk=256)][Ok]",
"[1][|There is not enough room on|the disk for this operation!][Ok]",
"[3][NeoDesk internal error #%d|Please re-boot NOW!][Will do...]",
"This disk cannot be|formatted because it|contains bad sectors",
"[1][The disk in this drive is|write-protected. Please move|the write-protect\
 tab if|you wish it to be modified.][Retry|Cancel]",
"[1][The drive whose|letter appears in this|icon does not exist.][Hm!]",
"[1][Invalid drive|letter in icon!][Oh?]",   /* 20 */
" There are more items than can be displayed",
" %n item%s in %N%s byte%s",	/* 003 */
"s",
" %sCreated %s",
" %s%s | Modified %s | R",
" %n selected items in %N%s byte%s",	/* 003 */
"[1][NEODESK.RSC is not in the|same directory as NEODESK!|NeoDesk must quit!][Ok]",
"[3][Formatting the disk in|drive %c will erase all|information on it\
][Continue|Cancel]",
"[1][You do not have|enough free memory for|this operation][Ok]",	/* remove eventually */
"[1][|A disk drive MUST|have a letter!][Ok]",	/* 30 */
"[1][|A disk drive letter|can only be from|'A' to 'Z'][Ok]",
"[3][The change you have made will|not take effect until you save|NEO_INF.DAT\
 and re-run NeoDesk][OK]",
"Search Results",
"[1][There is no %s.INF|file in the same folder|as this program!][Ok]",
"[2][|%s is read-only.|Try to delete it anyway?][Delete|Skip|Cancel]",
"[1][Could not create a|%s%s file!|Is there one there|in read-only mode?]\
[Um...]",
";.INF file... ';' is a comment char and must be the first on the line\r\n\
;All filenames and paths must be uppercase\r\n;\r\n\
;Oldest NeoDesk that can read this file\r\n\
%s\r\n\
;Resolution: x,y,char_h\r\n\
%d %d %d\r\n",			/* end of 37 */
";Autoexec file",
";Printer: type,FFD",
";Control: ampm,clock,save,mins,caches,repeat1,repeat2,click,bell,\r\n\
;         mclick,floppyA,floppyB,volume/balance,treble/bass",  /* 40 */
";Color pallette: RRGGBB (16)",
";Snap to grid: x,y,resort,overlap",
"[1][|You need to use a graphics|printer for a screen dump][Oh!]",
"%N byte%s",	/* 003 */
"%N (%N)",	/* 003 */
"  Reload %-12s  %s",
"[3][The trashcan cannot be opened.|You can, however,\
 drag a file,|folder, or disk icon here to|delete its contents.][Ok]",
"[1][There are no more|windows available!][Hm!]",
"Could not change to the|path of this application!",
"Executing %s...",	/* 50 */
"\r\n\r\n\033pPress any key or mouse button to return to NeoDesk\033q",
"\r\033p<More>\033q",
"\r\n\033f\033p\033b\057\033c\040\
Press any key or mouse button to return to the desktop\033q\r",
"[1][Warning:|Do not write to the|destination disk again|\
without re-formatting!][Ok...]",
"[1][|Could not find the file|%s!][Urgh]",	/* 004 */
"[1][|Incorrect %s file format!][Hm!]",
"[3][|No %s.INF file found.|Using defaults.][Ok]",
"[1][|Incorrect %s.INF format!|Restoring defaults!][Hm!]",
"[1][This macro cannot be|executed because the|desktop has changed][Ok]",
"[1][|No further subdirectories|   can be accessed.][Ok]",	/* 60 */
"[1][%s|(TOS error #%D)][Ok]",
"[1][|TOS error #%D][Ok]",
"This folder could not|be moved because it|is not empty",
"[1][Please run INSTALL.PRG|on your original NeoDesk|disk.][Bye-Bye]",
#ifdef DEMO
"[2][Quitting this demo will|cause a warm-reset|of the computer][Quit|Stay]",
#else DEMO
"[2][|Quit NeoDesk?][Quit|Stay]",
#endif DEMO
"[3][|To rename a folder, \"move\"|it within the same window][Ok]",
"[3][No default .INF file has been|specified for this resolution.|Please \
select one by using|\".INF Files...\" in|\"Set Preferences...\"][Ok]",
"[3][|Only disk drives can be copied|to other disk drives][Oh!]",
"[1][Only disk drives, files|and folders can be dragged|to the trashcan][Ok]",
"[3][WARNING!! This operation|will permanently delete|all items on Drive \
%c!|This is your only warning!][Delete|Cancel]",
"[3][This desk accessory is|not active in memory][Bleh!]",
"[3][|This icon cannot be|copied to a folder][Oh!]",
"Large|Small|    %s Text       \07H",
"ò1 Column|1 Column |    %s        \07C",
"rename|copy|move|Name conflict during %s:",
"Copy/Move|Delete|%s items",
"Read|Writ|Formatt|Twist|%sing",
"%N to %N",		/* 003 */
"[1][This dialog is already in|use and there is no free|\
memory for a copy of it][Cancel]",
"Count|Read|Writ|Mov|Delet|Creat|%sing",	/* 80 */
"ROM ver. %s  GEMDOS ver. %x.%02x  AES ver. %s",	/* 003 */
" on",
"[3][Warning!|It may be necessary to|re-boot after this diskcopy|if the\
 destination is a hard|disk][Continue|Cancel]",
"[2][|Found %s in|%s of %s][Show|Skip|Abort]",
"folder %s",
"the root",
"[1][|No match found][Ok]",
"Reload %s.INF",
";Executable Extensions: type,ext",
";Dialog Positions: #,x,y",	/* 90 */
"Save group as...",
"[3][This dialog is|already in use][Ok]",
"[2][Update NEO_INF.DAT|to contain the new|information?][Yes|No]",
"[1][There is not enough memory|available to copy this many|files to the \
clipboard][Ok]",
"Clipboard",          /* no more than 20 chars total */
"[1][|This operation cannot be|performed on the Clipboard][Ok]",
"[3][|Taking this action will cause|the Clipboard to be emptied]\
[Continue|Cancel]",
"Begin Macro|End Macro  |  %s           ^Esc",
"[1][|The resolution|change failed][Ok]",
/*****"[3][Taking this action will modify|%s and cause a warm|re-boot of \
the computer][Continue|Cancel]", *****/
"[3][Please insert your boot disk|containing %s into|drive A][Continue|Cancel]",	/* 100 */
"[1][Either there is no|%s or it is|corrupted!][Ok]",
"[1][This operation cannot|be performed when in|Reorder mode][Ok]",
"[3][Are you sure you want to|permanently reorder the|items in this directory?]\
[Reorder|Cancel]",
"[1][The disk in this drive|has changed. The items|cannot be reordered][Ok]",
" Reorder",
"[2][%s has been|modified. Re-boot now?][Reboot|Cancel]",
"[1][There is no more room|left for desktop notes][Ok]",
"[1][There is no more room|left for macros. Macro|recording must terminate][Ok]",
";Help: font_id,point,rows,cols,x,y,topic,match_case,all,path",
"<Default Macro Name>",                   /* must be exactly 20 characters */	/* 110 */
"[3][This operation cannot|be performed when in|the icon editor][Ok]",
"[1][There is not enough free|memory to display this|compressed picture][Ok]",
"Large|Small|    %s Text",
"ò1 Column|1 Column |    %s",
"Begin Macro|End Macro  |  %s",
";Print directory",
"[1][This filename contains|one or more invalid|characters][Ok]",
"[1][|This key is already|assigned to a macro][Ok]",
"%N bytes free in %D blocks",	/* 003 */
"largest block is %N bytes",	/* 003 */ 	/* 120 */
"Edit Desktop Note: Press Return to exit",	/* 40 char max */
"[3][This macro file will not|actually be loaded because a|\
macro is being recorded now.|Please end the macro.][Ok]",
"[3][|This is disk #%d of %d.|Insert the next disk.][Next|Cancel]",
"[2][An error has occurred.|Copy the file(s) into|the file clipboard?][Yes|No]",
"[1][%s already|exists in the clipboard|and can therefore not|be overwritten.][Cancel]",
"[3][Process %s|exited with status #%d|(%%s)][Ok]",
"<Unknown>",
"undefined error",
"Directory listing",
"Load filter",	/* 130 */
"Save filter",
"Directory filter",
"Search filter",
"File operation filter",
" %n item%s",	/* 003 */
"", /*%[2][Change the path of the group|item to the destination copy?][Change|Cancel]",*/
" %n selected items",	/* 003 */
"Program",
"[2][Group \"%s\" has|changed. Save it?][Save|Abandon|Cancel]",
"%N of %N bytes free",			/* 140: changed for 003 */
"%N of %N bytes free",			/* 003 */
"%N out of %N in use",			/* 003 */
"Program for NPI file:",
"Save NPI file:",
"[2][This NPI file has|changed. Save it?][Save|Abandon|Cancel]",
"[1][This disk drive is|currently in use and|cannot be accessed][Cancel]",
"[3][Make sure the original disk|this operation was being|performed on \
before pausing|is in the drive now!][Continue|Cancel]",
"Desktop background picture:",
"Loading picture...",
"[1][Programs cannot be executed|when NeoDesk is run as a desk|\
accessory with this version|of TOS][Cancel]",		/* 150 */
"System",
";Copy/move/del: copy_mode,confirm_copies,confirm_deletes,conf_over,\r\n\
;               diskcopy,diskcopy_back,copies,copy_back,del_back,\r\n\
;               rename_dest,filt_copy,filt_del,count_copy,count_del",
";Misc: back_speed,tos_pause,unload,Control-Z_text,exit_status,quit_alert,\r\n\
;      savecfg,other_prefs,date_fmt,speed_factor,numsep,open_fold,\r\n\
;      view_fit,view_mode",		/* 003 */
";Format: tracks,sides,sectors/track,Twist,drive,backgrnd,spc(3)",
";Batch file interp.",
";Text viewer",
";Windows: placement,split,x,y,width,height,sliders,show_icons,lg/small\r\n\
;         1+columns,show_size,date,time,sort,path",
";Window filters: flags,size_type,date,time,templ_mask,3 times,3 dates,\r\n\
;                3 sizes,3 long templs",
";Copy/del filter",
";Search filter",			/* 160 */
";Pre-defined templates (6)",
";Default TTP params.: 5 lines of up to 38 chars",
";Environment: use_argv,use_parent",
";Dialogs: in_wind,pos_mode",
";Desktop: in_wind,window x,y,width,height,show_pic,wallpaper,color_mode,picture\r\n\
;         fit_pic,pic_mode",		/* 003 */
";Windows: fill,real_time,icon_id,icon_size,sm_id,sm_size,lg_id,lg_size,name_id,\r\n\
;         name_size",
";Icons: type,x,y,letter,text,prog_type,prog_path",
";Applications: type,flags,extensions,name,path",	/* 003 */
";Environment strings",
"Interlace",					/* 170 */
"Double line",
"File name to append:",
"%N byte%s, %s",		/* 003 */
"[1][Error in .NIC file format!|As much of the NIC file as|possible has been read][Ok]",
"[3][Taking this action will|terminate the current|file operation][Terminate|Return]",
"The operation you just requested|will be added to the queue||[Close",
"Click below to return to the|dialog you were just using||[Return",
"CANCEL\0\0QUIT\0\0EXIT\0\0ABORT\0\0NO\0\0",
"TOS error #%d.",
"Centered desktop picture",		/* 003: added */	/* 180 */
"Picture viewer options",		/* 003: added */
/*** add new messages before here ***/
#ifdef DEMO
"[3][NeoDesk 4 can be ordered|from Gribnif Software at|\
(413) 532-2434 or in the UK|from Compo at 04873 582][Ok!]",
"[3][|The system will now|perform a warm-reset][Ok]",
"[3][This feature has been|disabled for the demo|version.][Ok]",
#endif DEMO
"{" };

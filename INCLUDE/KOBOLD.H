/***************** Warning! Only intended for programmers! *********************/


/* --------------- Interface messages -----------------------------------------*/

#define KOBOLD_JOB 0x2F10          /* start job in memory                      */
#define KOBOLD_JOB_NO_WINDOW 0x2F11/* Ditto, without main dialog               */

#define KOBOLD_ANSWER 0x2F12       /* Answer from KOBOLD with status in word 3 */
                                   /* und line in word 4                       */

#define KOBOLD_CONFIG 0x2F13       /* Request Configuration. Word 3 & 4 have to*/
                                   /* contain a pointer to a KOBOLD_CONFIGURA- */
                                   /* TION structure.                          */

#define KOBOLD_FIRST_SLCT 0x2F14   /* Requests the current selection in the    */
#define KOBOLD_NEXT_SLCT 0x2F15    /* source window. ..FIRST.. returns the     */
                                   /* first object, ..NEXT.. all others.       */
                                   /* Words 3 & 4 should contain a pointer to  */
                                   /* 128 Byte memory for the path and file    */
                                   /* name (allocated as 'global' under Multi- */
                                   /* TOS. The KOBOLD_ANSWER can be found in   */
                                   /* word 3:                                  */
                                   /*        -1: No more selected objects      */
                                   /*       0: Object is a file                */
                                   /*       1: Object is a folder              */

#define KOBOLD_CLOSE 0x2F16        /* Serves to close KOBOLD explicitly...     */
                                   /* status answer != FINISHED                */

#define KOBOLD_FREE_DRIVES 0x2F17  /*  Free all locked drives and clear read   */
                                   /*  directories.                            */

/*-----------------------------------------------------------------------------*\
 A pointer to the following structure has to be provided in word 3 & 4 of the
 message buffer during a KOBOLD_CONFIG request. The corresponding memory must
 be allocated as 'global' (Mxalloc(size,0x1mode)) under MultiTOS because KOBOLD 
 writes to your memory while filling up this structure. Kobold confirms 
 completion with a KOBOLD_ANSWER message.                                        
\*-----------------------------------------------------------------------------*/

typedef struct
{
    unsigned int 
        version,        /* e.g. 0x251 for version 2.51                */
        reserved[8],    /*                                            */
        buffer,         /* Free file buffer at request time           */
        kobold_active,  /* 1 = KOBOLD active, 0 = KOBOLD inactive     */
        kobold_dialog,  /* 0 = no main dialog, 1 = main dialog opened */
        no_of_files,    /* Number of files selected in the source     */
        no_of_folders,  /* Number of folders selected in the source   */
        total_kb;       /* Selection in kilobyte                      */

    int source_drive,   /* Selected source drive,       -1 = None     */
        dest_drive;     /* Selected destination drive, -1 = None      */

    unsigned long 
        gemdos_mode;    /* Bitvector: Bit 0 = Drive A etc.            */
                        /* Bit set = GEMDOS-Mode                      */

} KOBOLD_CONFIGURATION;


#define MiNT 0x4d694e54L        /* MiNT-Cookie */


/*--------------------- Error codes in Answer -----------------------------*/

#define FINISHED                -1   /* KOBOLD terminated                  */ 
#define OK                      0    /* Job finished, KOBOLD still active  */
#define ERROR                   1
#define NO_MEMORY               2
#define USER_BREAK              3
#define INVALID_POINTER         4
#define LOW_BUFFER              5
#define WRONG_DRIVE             6
#define WRONG_PARAMETER         7
#define UNEXPECTED_COMMAND      8
#define INVALID_MEMSIZE         9
#define NO_SUCH_OBJECT          10
#define NO_DRIVE_SELECTED       11
#define NO_FOLDER_CREATION      12
#define WRITE_PROTECTION        13
#define LOW_SPACE               14
#define LOW_ROOT                15
#define INVALID_PATH            16
#define BUFFER_IN_USE           17
#define BAD_BPB                 18
#define BAD_READ                19
#define BAD_WRITE               20
#define UNKNOWN_COMMAND         21
#define NO_WINDOW               22
#define TOO_MANY_GOSUBS         23
#define TOO_MANY_RETURNS        24
#define LABEL_NOT_FOUND         25
#define NO_SUCH_FOLDER          26
#define REORGANIZED_MEMORY      27
#define NO_SELECTION_MODE       28
#define DRIVEVAR_NOT_SPECIFIED  29
#define MULTIPLE_LABEL          30
#define EXEC_ERROR              31

/* ---------------  Job commands --------------------------------------------*/

#define _SRC_SELECT         0
#define _DST_SELECT         1
#define _DIALOG_LEVEL       2
#define _KEEP_FLAGS         3
#define _IGNORE_WP          4
#define _ALERT              5
#define _PAUSE              6
#define _NEW_FOLDER         7
#define _CHOOSE             8
#define _RESET_STATUS       9
#define _READ_INTO_BUFFER   10
#define _WRITE_BUFFER       11
#define _COPY               12
#define _MOVE               13
#define _DELETE             14
#define _QUIT               15
#define _GOTO               16
#define _GOSUB              17
#define _RETURN             18
#define _PERMANENT          19
#define _VERIFY             21
#define _DATE               22
#define _ARCHIVE_TREATMENT  23
#define _GEMDOS_MODE        24
#define _FORMAT_PARAMETER   25
#define _FORMAT             26
#define _SOFT_FORMAT        27
#define _OFF                28
#define _ON                 29
#define _EVER_OFF           30
#define _EVER_ON            31
#define _CONSIDER_PATHS     32
#define _ON_LEVEL           33
#define _EXTENSIONS         34
#define _ARCHIVE            35
#define _FILE               36
#define _KEEP_SEQUENCE      37
#define _RESET_ARCHIVES     38
#define _OPEN_FOLDERS       39
#define _CURRENT            40
#define _KEEP               41
#define _SET                42
#define _CLEAR              43
#define _CLEARED            44
#define _SI                 45
#define _SE                 46
#define _DI                 47
#define _DE                 48
#define _CLEAR_BUFFER       51
#define _SOURCE_TREATMENT   52
#define _DIALOG_WINDOWS     53
#define _RENAME             54
#define _BUFFER             55
#define _BING               56
#define _SWAP               57
#define _DATE_DIFFERENT     58
#define _DATE_EQUAL         59
#define _DATE_YOUNGER       60
#define _DATE_OLDER         61
#define _DATE_ARBITRARY     62
#define _SIZE_DIFFERENT     63
#define _SIZE_EQUAL         64
#define _SIZE_LARGER        65
#define _SIZE_SMALLER       66
#define _SIZE_ARBITRARY     67
#define _FILE_ATTRIBUTES    68
#define _SELECT_DRIVE       69
#define _BRANCH_ON_DRIVE    70
#define _EXECUTE            71
#define _SET_DRIVE          72
#define _NEXT_DRIVE         73
#define _EXIT               74
/*-----------------------------------------------------------------------------*/
#if 0

int kobold_nr,sendbuf[8],status,zeile;

char kobold_id[] = {"KOBOLD_2"},
     job[] = {"src_select c:\ ... etc."},   /* or simply {"#0 etc."} */
     *job_package;
            .
            .
            .
appl_id = appl_init();

if (appl_id != -1)
{
    if (_GemParBlk.global[0] >= 0x400 && find_cookie(MiNT))
        multiTos = TRUE;
    else
        multiTos = FALSE;
    .
    .
    .
}

if (multiTos)
    job_package = Mxalloc(strlen(job) + 1,0x42);
    /* With memory protection: ST-preferred (for example), readable */
else
    job_package = Malloc(strlen(job) + 1);


kobold_nr = appl_find(kobold_id);        /* find ACC number  */

if ((kobold_nr >= 0) && job_package)     /* ACC present & memory available */
{
    sendbuf[0] = KOBOLD_JOB_NO_WINDOW;   /* Message: Copy without window   */
    sendbuf[1] = appl_id;                /* Own ID                         */
    sendbuf[2] = 0;                      /* No overhead                    */
    strcpy(job_package,job);             /* Copy job to the globally       */
                                         /* readable (MultiTos) memory     */
                                         /* area.                          */

    *((char **) &sendbuf[3]) = job_package;   /* Pointer to Job   */
    appl_write(kobold_nr,16,sendbuf);         /* send to Kobold   */

    /*------------------------------------------------------------*\
        Warning! At this point your program should not block AES 
        with a "wind_update(BEG_UPDATE)"!               
    \*------------------------------------------------------------*/

    /**** Wait for the job to be finished ****/
    do
    {
        evnt_mesag(sendbuf);
    
    } while(sendbuf[0] != KOBOLD_ANSWER);
    
    Mfree(job_package);
    
    status = sendbuf[3];             /* Final status         */
    zeile = sendbuf[4];              /* Last executed line   */
    
    if (status != FINISHED)          /* KOBOLD still active? */
    {
        sendbuf[0] = KOBOLD_CLOSE;   /* Message: Close       */
        sendbuf[1] = appl_id;        /* Own ID               */
        sendbuf[2] = 0;              /* No Overhead          */
    
        appl_write(kobold_nr,16,sendbuf);   /* send...       */
    
        /**** Wait for drives to be unlocked and memory freed ****/
        do          
        {           
            evnt_mesag(sendbuf);
    
        } while(sendbuf[0] != KOBOLD_ANSWER);
    }
    
    /* Error handling here... */
    .
    .
    .
}

/* =========================================================================== */
#endif
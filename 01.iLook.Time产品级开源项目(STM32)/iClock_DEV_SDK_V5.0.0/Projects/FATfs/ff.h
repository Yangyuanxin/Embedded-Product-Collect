/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.07a       (C)ChaN, 2009
/----------------------------------------------------------------------------/
/ FatFs module is an open source software to implement FAT file system to
/ small embedded systems. This is a free software and is opened for education,
/ research and commercial developments under license policy of following terms.
/
/  Copyright (C) 2009, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial use UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/----------------------------------------------------------------------------*/

#include "integer.h"

/*---------------------------------------------------------------------------/
/ FatFs Configuration Options
/
/ CAUTION! Do not forget to make clean the project after any changes to
/ the configuration options.
/
/----------------------------------------------------------------------------*/
#ifndef _FATFS
#define _FATFS

#define _WORD_ACCESS	1
/* The _WORD_ACCESS option defines which access method is used to the word
/  data in the FAT structure.
/
/   0: Byte-by-byte access. Always compatible with all platforms.
/   1: Word access. Do not choose this unless following condition is met.
/
/  When the byte order on the memory is big-endian or address miss-aligned
/  word access results incorrect behavior, the _WORD_ACCESS must be set to 0.
/  If it is not the case, the value can also be set to 1 to improve the
/  performance and code efficiency. */


#define _FS_READONLY	0
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate and useless f_getfree. */


#define _FS_MINIMIZE	0
/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/
/   0: Full function.
/   1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate and f_rename
/      are removed.
/   2: f_opendir and f_readdir are removed in addition to level 1.
/   3: f_lseek is removed in addition to level 2. */


#define	_FS_TINY	1
/* When _FS_TINY is set to 1, FatFs uses the sector buffer in the file system
/  object instead of the sector buffer in the individual file object for file
/  data transfer. This reduces memory consumption 512 bytes each file object. */


#define	_USE_STRFUNC	1
/* To enable string functions, set _USE_STRFUNC to 1 or 2. */


#define	_USE_MKFS	0
/* To enable f_mkfs function, set _USE_MKFS to 1 and set _FS_READONLY to 0 */


#define	_USE_FORWARD	0
/* To enable f_forward function, set _USE_FORWARD to 1 and set _FS_TINY to 1. */


#define _DRIVES		1
/* Number of volumes (logical drives) to be used. */


//#define	_MAX_SS	512		//SD¿¨¿ÉÒÔÓÃ512
#define _MAX_SS 4096		//SPI FLASH
//#define	_MAX_SS	1024
/* Maximum sector size to be handled. (512/1024/2048/4096) */
/* 512 for memroy card and hard disk, 1024 for floppy disk, 2048 for MO disk */


#define	_MULTI_PARTITION	0
/* When _MULTI_PARTITION is set to 0, each volume is bound to the same physical
/ drive number and can mount only first primary partition. When it is set to 1,
/ each volume is tied to the partitions listed in Drives[]. */


#define _CODE_PAGE	936 //850
/* The _CODE_PAGE specifies the OEM code page to be used on the target system.
/  When it is non LFN configuration, there is no difference between SBCS code
/  pages. When LFN is enabled, the code page must always be set correctly.
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   775 - Baltic
/   850 - Multilingual Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   858 - Multilingual Latin 1 + Euro
/   862 - Hebrew
/   866 - Russian
/   874 - Thai
/   932 - Japanese Shift-JIS       (DBCS)
/   936 - Simplified Chinese GBK   (DBCS)
/   949 - Korean                   (DBCS)
/   950 - Traditional Chinese Big5 (DBCS)
/   1258 - Vietnam
*/


#define	_USE_LFN	0
#define	_MAX_LFN	255		/* Maximum LFN length to handle (max:255) */
/* The _USE_LFN option switches the LFN support.
/
/   0: Disable LFN.
/   1: Enable LFN with static working buffer on the bss. NOT REENTRANT.
/   2: Enable LFN with dynamic working buffer on the caller's STACK.
/
/  The working buffer occupies (_MAX_LFN + 1) * 2 bytes. When enable LFN,
/  a Unicode - OEM code conversion function ff_convert() must be added to
/  the project. */


#define _FS_REENTRANT	0
#define _TIMEOUT		1000	/* Timeout period in unit of time ticks */
#define	_SYNC_t			HANDLE	/* Type of sync object used on the OS. */
								/* e.g. HANDLE, OS_EVENT*, ID and etc.. */
/* To make the FatFs module re-entrant, set _FS_REENTRANT to 1 and add user
/  provided synchronization handlers, ff_req_grant, ff_rel_grant,
/  ff_del_syncobj and ff_cre_syncobj function to the project. */



/* End of configuration options. Do not change followings without care.     */
/*--------------------------------------------------------------------------*/



/* Definitions corresponds to multiple sector size */

#if _MAX_SS == 512
#define	SS(fs)	512
#else
#if _MAX_SS == 1024 || _MAX_SS == 2048 || _MAX_SS == 4096
#define	SS(fs)	((fs)->s_size)
#else
#error Sector size must be 512, 1024, 2048 or 4096.
#endif
#endif



/* File system object structure */

typedef struct _FATFS {
	BYTE	fs_type;	/* FAT sub type */
	BYTE	drive;		/* Physical drive number */
	BYTE	csize;		/* Number of sectors per cluster */
	BYTE	n_fats;		/* Number of FAT copies */
	BYTE	wflag;		/* win[] dirty flag (1:must be written back) */
	BYTE	pad1;
	WORD	id;			/* File system mount ID */
	WORD	n_rootdir;	/* Number of root directory entries (0 on FAT32) */
#if _FS_REENTRANT
	_SYNC_t	sobj;		/* Identifier of sync object */
#endif
#if _MAX_SS != 512U
	WORD	s_size;		/* Sector size */
#endif
#if !_FS_READONLY
	BYTE	fsi_flag;	/* fsinfo dirty flag (1:must be written back) */
	BYTE	pad2;
	DWORD	last_clust;	/* Last allocated cluster */
	DWORD	free_clust;	/* Number of free clusters */
	DWORD	fsi_sector;	/* fsinfo sector */
#endif
	DWORD	sects_fat;	/* Sectors per fat */
	DWORD	max_clust;	/* Maximum cluster# + 1. Number of clusters is max_clust - 2 */
	DWORD	fatbase;	/* FAT start sector */
	DWORD	dirbase;	/* Root directory start sector (Cluster# on FAT32) */
	DWORD	database;	/* Data start sector */
	DWORD	winsect;	/* Current sector appearing in the win[] */
	BYTE *win;      //BYTE	win[_MAX_SS];/* Disk access window for Directory/FAT */
} FATFS;



/* Directory object structure */

typedef struct _DIR {
	WORD	id;			/* Owner file system mount ID */
	WORD	index;		/* Current index number */
	FATFS*	fs;			/* Pointer to the owner file system object */
	DWORD	sclust;		/* Table start cluster (0:Static table) */
	DWORD	clust;		/* Current cluster */
	DWORD	sect;		/* Current sector */
	BYTE*	dir;		/* Pointer to the current SFN entry in the win[] */
	BYTE*	fn;			/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _USE_LFN
	WCHAR*	lfn;		/* Pointer to the LFN working buffer */
	WORD	lfn_idx;	/* Last matched LFN index (0xFFFF:No LFN) */
#endif
} DIR;



/* File object structure */

typedef struct _FIL {
	FATFS*	fs;			/* Pointer to the owner file system object */
	WORD	id;			/* Owner file system mount ID */
	BYTE	flag;		/* File status flags */
	BYTE	csect;		/* Sector address in the cluster */
	DWORD	fptr;		/* File R/W pointer */
	DWORD	fsize;		/* File size */
	DWORD	org_clust;	/* File start cluster */
	DWORD	curr_clust;	/* Current cluster */
	DWORD	dsect;		/* Current data sector */
#if !_FS_READONLY
	DWORD	dir_sect;	/* Sector containing the directory entry */
	BYTE*	dir_ptr;	/* Ponter to the directory entry in the window */
#endif
#if !_FS_TINY
	BYTE	buf[_MAX_SS];/* File R/W buffer */
#endif
} FIL;



/* File status structure */

typedef struct _FILINFO {
	DWORD fsize;		/* File size */
	WORD fdate;			/* Last modified date */
	WORD ftime;			/* Last modified time */
	BYTE fattrib;		/* Attribute */
	char fname[13];		/* Short file name (8.3 format) */
#if _USE_LFN
	char *lfname;		/* Pointer to the LFN buffer */
	int lfsize;			/* Size of LFN buffer [bytes] */
#endif
} FILINFO;



/* DBCS code ranges */

#if _CODE_PAGE == 932	/* CP932 (Japanese Shift-JIS) */
#define _DF1S	0x81	/* DBC 1st byte range 1 start */
#define _DF1E	0x9F	/* DBC 1st byte range 1 end */
#define _DF2S	0xE0	/* DBC 1st byte range 2 start */
#define _DF2E	0xFC	/* DBC 1st byte range 2 end */
#define _DS1S	0x40	/* DBC 2nd byte range 1 start */
#define _DS1E	0x7E	/* DBC 2nd byte range 1 end */
#define _DS2S	0x80	/* DBC 2nd byte range 2 start */
#define _DS2E	0xFC	/* DBC 2nd byte range 2 end */

#elif _CODE_PAGE == 936	/* CP936 (Simplified Chinese GBK) */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0x80
#define _DS2E	0xFE

#elif _CODE_PAGE == 949	/* CP949 (Korean) */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x41
#define _DS1E	0x5A
#define _DS2S	0x61
#define _DS2E	0x7A
#define _DS3S	0x81
#define _DS3E	0xFE

#elif _CODE_PAGE == 950	/* CP950 (Traditional Chinese Big5) */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0xA1
#define _DS2E	0xFE

#else					/* SBCS code pages */
#define _DF1S	0

#endif



/* Character code support macros */

#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))
#define IsDigit(c)	(((c)>='0')&&((c)<='9'))

#if _DF1S	/* DBCS configuration */

#if _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E) || ((BYTE)(c) >= _DF2S && (BYTE)(c) <= _DF2E))
#else		/* One 1st byte area */
#define IsDBCS1(c)	((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E)
#endif

#if _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E) || ((BYTE)(c) >= _DS3S && (BYTE)(c) <= _DS3E))
#else		/* Two 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E))
#endif

#else		/* SBCS configuration */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */



/* Definitions corresponds to multi partition */

#if _MULTI_PARTITION		/* Multiple partition configuration */

typedef struct _PARTITION {
	BYTE pd;	/* Physical drive# */
	BYTE pt;	/* Partition # (0-3) */
} PARTITION;

extern
const PARTITION Drives[];			/* Logical drive# to physical location conversion table */
#define LD2PD(drv) (Drives[drv].pd)	/* Get physical drive# */
#define LD2PT(drv) (Drives[drv].pt)	/* Get partition# */

#else						/* Single partition configuration */

#define LD2PD(drv) (drv)	/* Physical drive# is equal to the logical drive# */
#define LD2PT(drv) 0		/* Always mounts the 1st partition */

#endif



/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,			/* 0 */
	FR_DISK_ERR,		/* 1 */
	FR_INT_ERR,			/* 2 */
	FR_NOT_READY,		/* 3 */
	FR_NO_FILE,			/* 4 */
	FR_NO_PATH,			/* 5 */
	FR_INVALID_NAME,	/* 6 */
	FR_DENIED,			/* 7 */
	FR_EXIST,			/* 8 */
	FR_INVALID_OBJECT,	/* 9 */
	FR_WRITE_PROTECTED,	/* 10 */
	FR_INVALID_DRIVE,	/* 11 */
	FR_NOT_ENABLED,		/* 12 */
	FR_NO_FILESYSTEM,	/* 13 */
	FR_MKFS_ABORTED,	/* 14 */
	FR_TIMEOUT			/* 15 */
} FRESULT;


/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_mount (BYTE, FATFS*);						/* Mount/Unmount a logical drive */
FRESULT f_open (FIL*, const char*, BYTE);			/* Open or create a file */
FRESULT f_read (FIL*, void*, UINT, UINT*);			/* Read data from a file */
FRESULT f_write (FIL*, const void*, UINT, UINT*);	/* Write data to a file */
FRESULT f_lseek (FIL*, DWORD);						/* Move file pointer of a file object */
FRESULT f_close (FIL*);								/* Close an open file object */
FRESULT f_opendir (DIR*, const char*);				/* Open an existing directory */
FRESULT f_readdir (DIR*, FILINFO*);					/* Read a directory item */
FRESULT f_stat (const char*, FILINFO*);				/* Get file status */
FRESULT f_getfree (const char*, DWORD*, FATFS**);	/* Get number of free clusters on the drive */
FRESULT f_truncate (FIL*);							/* Truncate file */
FRESULT f_sync (FIL*);								/* Flush cached data of a writing file */
FRESULT f_unlink (const char*);						/* Delete an existing file or directory */
FRESULT	f_mkdir (const char*);						/* Create a new directory */
FRESULT f_chmod (const char*, BYTE, BYTE);			/* Change attribute of the file/dir */
FRESULT f_utime (const char*, const FILINFO*);		/* Change timestamp of the file/dir */
FRESULT f_rename (const char*, const char*);		/* Rename/Move a file or directory */
FRESULT f_forward (FIL*, UINT(*)(const BYTE*,UINT), UINT, UINT*);	/* Forward data to the stream */
FRESULT f_mkfs (BYTE, BYTE, WORD);					/* Create a file system on the drive */

FRESULT f_create (FIL *,const char *,DWORD btw );  //wjh add

#if _USE_STRFUNC
int f_putc (int, FIL*);								/* Put a character to the file */
int f_puts (const char*, FIL*);						/* Put a string to the file */
int f_printf (FIL*, const char*, ...);				/* Put a formatted string to the file */
char* f_gets (char*, int, FIL*);					/* Get a string from the file */
#define f_eof(fp) (((fp)->fptr == (fp)->fsize) ? 1 : 0)
#define f_error(fp) (((fp)->flag & FA__ERROR) ? 1 : 0)
#ifndef EOF
#define EOF -1
#endif
#endif



/*--------------------------------------------------------------*/
/* User defined functions                                       */

/* Real time clock */
#if !_FS_READONLY
DWORD get_fattime (void);	/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
							/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
#endif

/* Unicode - OEM code conversion */
#if _USE_LFN
WCHAR ff_convert (WCHAR, UINT);
#endif

/* Sync functions */
#if _FS_REENTRANT
BOOL ff_cre_syncobj(BYTE, _SYNC_t*);
BOOL ff_del_syncobj(_SYNC_t);
BOOL ff_req_grant(_SYNC_t);
void ff_rel_grant(_SYNC_t);
#endif

bool disc_mount(void);
bool disc_unmount(void);


/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01
#define	FA_OPEN_EXISTING	0x00
#if _FS_READONLY == 0
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#endif
#define FA__ERROR			0x80


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */


/* FatFs refers the members in the FAT structures with byte offset instead
/ of structure member because there are incompatibility of the packing option
/ between various compilers. */

#define BS_jmpBoot			0
#define BS_OEMName			3
#define BPB_BytsPerSec		11
#define BPB_SecPerClus		13
#define BPB_RsvdSecCnt		14
#define BPB_NumFATs			16
#define BPB_RootEntCnt		17
#define BPB_TotSec16		19
#define BPB_Media			21
#define BPB_FATSz16			22
#define BPB_SecPerTrk		24
#define BPB_NumHeads		26
#define BPB_HiddSec			28
#define BPB_TotSec32		32
#define BS_55AA				510

#define BS_DrvNum			36
#define BS_BootSig			38
#define BS_VolID			39
#define BS_VolLab			43
#define BS_FilSysType		54

#define BPB_FATSz32			36
#define BPB_ExtFlags		40
#define BPB_FSVer			42
#define BPB_RootClus		44
#define BPB_FSInfo			48
#define BPB_BkBootSec		50
#define BS_DrvNum32			64
#define BS_BootSig32		66
#define BS_VolID32			67
#define BS_VolLab32			71
#define BS_FilSysType32		82

#define	FSI_LeadSig			0
#define	FSI_StrucSig		484
#define	FSI_Free_Count		488
#define	FSI_Nxt_Free		492

#define MBR_Table			446

#define	DIR_Name			0
#define	DIR_Attr			11
#define	DIR_NTres			12
#define	DIR_CrtTime			14
#define	DIR_CrtDate			16
#define	DIR_FstClusHI		20
#define	DIR_WrtTime			22
#define	DIR_WrtDate			24
#define	DIR_FstClusLO		26
#define	DIR_FileSize		28
#define	LDIR_Ord			0
#define	LDIR_Attr			11
#define	LDIR_Type			12
#define	LDIR_Chksum			13
#define	LDIR_FstClusLO		26



/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1	/* Enable word access to the FAT structure */
#define	LD_WORD(ptr)		(WORD)(*(WORD*)(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(*(DWORD*)(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(WORD*)(BYTE*)(ptr)=(WORD)(val)
#define	ST_DWORD(ptr,val)	*(DWORD*)(BYTE*)(ptr)=(DWORD)(val)
#else					/* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(WORD)(((WORD)*(BYTE*)((ptr)+1)<<8)|(WORD)*(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(((DWORD)*(BYTE*)((ptr)+3)<<24)|((DWORD)*(BYTE*)((ptr)+2)<<16)|((WORD)*(BYTE*)((ptr)+1)<<8)|*(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *(BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8)
#define	ST_DWORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *(BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8); *(BYTE*)((ptr)+2)=(BYTE)((DWORD)(val)>>16); *(BYTE*)((ptr)+3)=(BYTE)((DWORD)(val)>>24)
#endif


#endif /* _FATFS */

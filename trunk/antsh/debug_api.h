//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : debug_api.h - header file debug API.
// Date    : 03/02/2010 23:09
// Website : http://www.bugtraker.pl 
//
// ANTsh - Copyright (C) 2009 BugTraker (http://www.bugtraker.pl).
//
// ANTsh is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// ANTsh is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeFree; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------------------


#ifndef DEBUG_API
#define DEBUG_API


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------

#define _MAX_SS                         512
typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef signed char		CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;

typedef struct _FATFS_ {
	BYTE	fs_type;	/* FAT sub type */
	BYTE	drive;		/* Physical drive number */
	BYTE	csize;		/* Number of sectors per cluster */
	BYTE	n_fats;		/* Number of FAT copies */
	BYTE	wflag;		/* win[] dirty flag (1:must be written back) */
	BYTE	fsi_flag;	/* fsinfo dirty flag (1:must be written back) */
	WORD	id;			/* File system mount ID */
	WORD	n_rootdir;	/* Number of root directory entries (0 on FAT32) */
#if _FS_REENTRANT
	_SYNC_t	sobj;		/* Identifier of sync object */
#endif
#if _MAX_SS != 512
	WORD	s_size;		/* Sector size */
#endif
#if !_FS_READONLY
	DWORD	last_clust;	/* Last allocated cluster */
	DWORD	free_clust;	/* Number of free clusters */
	DWORD	fsi_sector;	/* fsinfo sector */
#endif
#if _FS_RPATH
	DWORD	cdir;		/* Current directory (0:root)*/
#endif
	DWORD	sects_fat;	/* Sectors per fat */
	DWORD	max_clust;	/* Maximum cluster# + 1. Number of clusters is max_clust - 2 */
	DWORD	fatbase;	/* FAT start sector */
	DWORD	dirbase;	/* Root directory start sector (Cluster# on FAT32) */
	DWORD	database;	/* Data start sector */
	DWORD	winsect;	/* Current sector appearing in the win[] */
	BYTE	win[_MAX_SS];/* Disk access window for Directory/FAT */
} FATFS;

typedef struct _DIR_ {
    FATFS*  fs;         /* Pointer to the owner file system object */
    WORD    id;         /* Owner file system mount ID */
    WORD    index;      /* Current read/write index number */
    DWORD   sclust;     /* Table start cluster (0:Static table) */
    DWORD   clust;      /* Current cluster */
    DWORD   sect;       /* Current sector */
    BYTE*   dir;        /* Pointer to the current SFN entry in the win[] */
    BYTE*   fn;         /* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _USE_LFN
    WCHAR*  lfn;        /* Pointer to the LFN working buffer */
    WORD    lfn_idx;    /* Last matched LFN index (0xFFFF:No LFN) */
#endif
} DIR;


typedef struct _FILINFO_ {
    DWORD fsize;      /* File size */
    WORD  fdate;      /* Last modified date */
    WORD  ftime;      /* Last modified time */
    BYTE  fattrib;    /* Attribute */
    char  fname[13];  /* Short file name (8.3 format) */
#if _USE_LFN
    XCHAR* lfname;    /* Pointer to the LFN buffer */
    int   lfsize;     /* Size of LFN buffer [characters] */
#endif
} FILINFO;

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


//---------------------------------------------------------------------------------------
//
//                        G L O B A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


FRESULT f_readdir (DIR* DirObject, FILINFO* FileInfo);
FRESULT f_opendir (DIR* DirObject, const CHAR* DirName);
FRESULT f_mount (BYTE Drive, FATFS *FileSystemObject);
FRESULT f_chdir (const CHAR* Path);


#endif

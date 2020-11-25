/*****************************************************************************
* www.goome.net
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_fs.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *  Timer related APIs
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
 
#ifndef __GM_FS_H__
#define __GM_FS_H__

#include <gm_type.h>
/****************************************************************************
 * Type of file access permitted
 ***************************************************************************/
#define GM_FS_READ_WRITE            0x00000000L
#define GM_FS_READ_ONLY             0x00000100L
#define GM_FS_CREATE                0x00010000L
#define GM_FS_CREATE_ALWAYS         0x00020000L
#define GM_FS_OPEN_SHARED           0x00000200L


/****************************************************************************
 * Type of file move permitted
 ***************************************************************************/
#define GM_FS_MOVE_COPY          0x00000001     // Move file|folder by copy
#define GM_FS_MOVE_KILL          0x00000002     // Delete the moved file|folder after moving
#define GM_FS_MOVE_OVERWRITE     0x00010000      // Overwrite the existed file in destination path when move file|folder

#define GM_FS_ATTR_DIR              0x10
#define GM_FS_ATTR_ARCHIVE          0x20

/****************************************************************************
 * 
 ***************************************************************************/
#define GM_FS_FILE_TYPE              0x00000004     
#define GM_FS_DIR_TYPE               0x00000008     
#define GM_FS_RECURSIVE_TYPE         0x00000010



extern int GM_FS_Open(const U16 * FileName, UINT Flag);
extern int GM_FS_Close(int FileHandle);
extern int GM_FS_Read(int FileHandle, void * DataPtr, UINT Length, UINT * Read);
extern int GM_FS_Write(int FileHandle, void * DataPtr, UINT Length, UINT * Written);
extern int GM_FS_Seek(int FileHandle, int Offset, int Whence);
extern int GM_FS_Commit(int FileHandle);
extern int GM_FS_GetFileSize(int FileHandle, UINT * Size);
extern int GM_FS_GetFilePosition(int FileHandle, UINT * Position);
extern int GM_FS_Delete(const U16 * FileName);
extern int GM_FS_CheckFile(const U16  * FileName);
extern int GM_FS_GetCurrentDir(U16  * DirName, UINT MaxLength); 
extern int GM_FS_SetCurrentDir(const U16 * DirName);
extern int GM_FS_CreateDir(const U16 * DirName);
extern int GM_FS_RemoveDir(const U16 * DirName);
extern int GM_FS_Rename(const U16 * FileName, const U16 * NewName);
extern int GM_FS_XDelete(const U16 * FullPath, UINT Flag, U8 *RecursiveStack, const UINT StackSize);
extern S32 GM_GetDiskFreeSpace(U16 *DriveName, U32 *free_space);



#ifdef DFLSDJG
// error codes
#define RTF_NO_ERROR                      0
#define RTF_ERROR_RESERVED                -1
#define RTF_PARAM_ERROR                   -2
#define RTF_INVALID_FILENAME              -3
#define RTF_DRIVE_NOT_FOUND               -4
#define RTF_TOO_MANY_FILES                -5
#define RTF_NO_MORE_FILES                 -6
#define RTF_WRONG_MEDIA                   -7
#define RTF_INVALID_FILE_SYSTEM           -8
#define RTF_FILE_NOT_FOUND                -9
#define RTF_INVALID_FILE_HANDLE           -10
#define RTF_UNSUPPORTED_DEVICE            -11
#define RTF_UNSUPPORTED_DRIVER_FUNCTION   -12
#define RTF_CORRUPTED_PARTITION_TABLE     -13
#define RTF_TOO_MANY_DRIVES               -14
#define RTF_INVALID_FILE_POS              -15
#define RTF_ACCESS_DENIED                 -16
#define RTF_STRING_BUFFER_TOO_SMALL       -17
#define RTF_GENERAL_FAILURE               -18
#define RTF_PATH_NOT_FOUND                -19
#define RTF_FAT_ALLOC_ERROR               -20
#define RTF_ROOT_DIR_FULL                 -21
#define RTF_DISK_FULL                     -22
#define RTF_TIMEOUT                       -23
#define RTF_BAD_SECTOR                    -24
#define RTF_DATA_ERROR                    -25
#define RTF_MEDIA_CHANGED                 -26
#define RTF_SECTOR_NOT_FOUND              -27
#define RTF_ADDRESS_MARK_NOT_FOUND        -28
#define RTF_DRIVE_NOT_READY               -29
#define RTF_WRITE_PROTECTION              -30
#define RTF_DMA_OVERRUN                   -31
#define RTF_CRC_ERROR                     -32
#define RTF_DEVICE_RESOURCE_ERROR         -33
#define RTF_INVALID_SECTOR_SIZE           -34
#define RTF_OUT_OF_BUFFERS                -35
#define RTF_FILE_EXISTS                   -36
#define RTF_LONG_FILE_POS                 -37
#define RTF_FILE_TOO_LARGE                -38
#define RTF_BAD_DIR_ENTRY                 -39
#define RTF_ATTR_CONFLICT                 -40 // Recoverable support: Can't specify FS_PROTECTION_MODE and FS_NONBLOCK_MODE
#define RTF_CHECKDISK_RETRY               -41 // Recoverable support: used for CROSSLINK
#define RTF_LACK_OF_PROTECTION_SPACE      -42
#endif

#endif

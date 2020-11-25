/* rt_sys.h: prototypes for _sys_* functions and types
 *
 * Copyright 1999 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.6 $
 * Checkin $Date: 2000/11/08 15:43:29 $
 * Revising $Author: statham $
 */

#ifndef __RT_SYS_H
#define __RT_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This type is used by the _sys_ I/O functions to denote an open
 * file.
 */
typedef int FILEHANDLE;

/*
 * Open a file. May return -1 if the file failed to open.
 */
extern FILEHANDLE _sys_open(const char * /*name*/, int /*openmode*/);
/*
 * openmode is a bitmap, whose bits are given below. They
 * correspond directly to the ANSI mode specification.
 */
#define OPEN_R 0
#define OPEN_W 4
#define OPEN_A 8
#define OPEN_B 1
#define OPEN_PLUS 2

/*
 * These names should be special strings which will be recognised
 * by _sys_open and will cause it to return the standard I/O
 * handles, instead of opening a real file.
 */
extern const char __stdin_name[];
extern const char __stdout_name[];
extern const char __stderr_name[];

/*
 * Close a file. Should return 0 on success or a negative value on error.
 */
extern int _sys_close(FILEHANDLE /*fh*/);

/*
 * Write to a file. Returns 0 on success, negative on error, and
 * the number of characters _not_ written on partial success.
 * `mode' is either 0 or OPEN_B, consistent with the mode in which
 * the file was opened.
 */
extern int _sys_write(FILEHANDLE /*fh*/, const unsigned char * /*buf*/,
                      unsigned /*len*/, int /*mode*/);

/*
 * Read from a file. Can return:
 *  - zero if the read was completely successful
 *  - the number of bytes _not_ read, if the read was partially successful
 *  - the number of bytes not read, plus the top bit set (0x80000000), if
 *    the read was partially successful due to end of file
 *  - -1 if some error other than EOF occurred
 *
 * It is also legal to signal EOF by returning no data but
 * signalling no error (i.e. the top-bit-set mechanism need never
 * be used).
 *
 * So if (for example) the user is trying to read 8 bytes at a time
 * from a file in which only 5 remain, this routine can do three
 * equally valid things:
 *
 *  - it can return 0x80000003 (3 bytes not read due to EOF)
 *  - OR it can return 3 (3 bytes not read), and then return
 *    0x80000008 (8 bytes not read due to EOF) on the next attempt
 *  - OR it can return 3 (3 bytes not read), and then return
 *    8 (8 bytes not read, meaning 0 read, meaning EOF) on the next
 *    attempt
 *
 * `mode' is either 0 or OPEN_B, consistent with the mode in which
 * the file was opened.
 */
extern int _sys_read(FILEHANDLE /*fh*/, unsigned char * /*buf*/,
                     unsigned /*len*/, int /*mode*/);

/*
 * Write a character to the output channel. This function is used
 * for last-resort error message output.
 */
extern void _ttywrch(int /*ch*/);

/*
 * Return non-zero if the argument file is connected to a terminal.
 */
extern int _sys_istty(FILEHANDLE /*fh*/);

/*
 * Move the file position to a given offset from the file start.
 * Returns >=0 on success, <0 on failure.
 */
extern int _sys_seek(FILEHANDLE /*fh*/, long /*pos*/);

/*
 * Flush any OS buffers associated with fh, ensuring that the file
 * is up to date on disk. Result is >=0 if OK, negative for an
 * error.
 */
extern int _sys_ensure(FILEHANDLE /*fh*/);

/*
 * Return the current length of a file, or <0 if an error occurred.
 * _sys_flen is allowed to reposition the file pointer (so Unix can
 * implement it with a single lseek, for example), since it is only
 * called when processing SEEK_END relative fseeks, and therefore a
 * call to _sys_flen is always followed by a call to _sys_seek.
 */
extern long _sys_flen(FILEHANDLE /*fh*/);

/*
 * Return the name for temporary file number sig in the buffer
 * name. Returns 0 on failure. maxlen is the maximum name length
 * allowed.
 */
extern int _sys_tmpnam(char * /*name*/, int /*sig*/, unsigned /*maxlen*/);

/*
 * Terminate the program, passing a return code back to the user.
 * This function may not return.
 */
extern void _sys_exit(int /*returncode*/);   /* never returns */

/*
 * Return a pointer to the command line used to invoke the program.
 * The supplied buffer may be used to store the string, but need
 * not be.
 */
extern char *_sys_command_string(char * /*cmd*/, int /*len*/);

#ifdef __cplusplus
}
#endif

#endif

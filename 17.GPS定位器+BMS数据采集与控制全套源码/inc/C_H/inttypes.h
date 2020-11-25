/* Copyright (C) ARM Ltd., 1999 */
/* All rights reserved */

/*
 * RCS $Revision: 1.5.2.2 $
 * Checkin $Date: 2001/05/24 07:32:01 $
 * Revising $Author: sdouglas $
 */

/* Based on WG14/N843 (C9X) Committee Draft August 3, 1998 */

#ifndef __inttypes_h
#define __inttypes_h

#include <stdint.h>

#if !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS)

    /* 7.8.1 */

#define PRId8         "d"
#define PRId16        "d"
#define PRId32        "d"
#define PRId64      "lld"
#define PRIdLEAST8    "d"
#define PRIdLEAST16   "d"
#define PRIdLEAST32   "d"
#define PRIdLEAST64 "lld"
#define PRIdFAST8     "d"
#define PRIdFAST16    "d"
#define PRIdFAST32    "d"
#define PRIdFAST64  "lld"
#define PRIdMAX      "jd"
#define PRIdPTR       "d"

#define PRIi8         "i"
#define PRIi16        "i"
#define PRIi32        "i"
#define PRIi64      "lli"
#define PRIiLEAST8    "i"
#define PRIiLEAST16   "i"
#define PRIiLEAST32   "i"
#define PRIiLEAST64 "lli"
#define PRIiFAST8     "i"
#define PRIiFAST16    "i"
#define PRIiFAST32    "i"
#define PRIiFAST64  "lli"
#define PRIiMAX      "ji"
#define PRIiPTR       "i"

#define PRIo8         "o"
#define PRIo16        "o"
#define PRIo32        "o"
#define PRIo64      "llo"
#define PRIoLEAST8    "o"
#define PRIoLEAST16   "o"
#define PRIoLEAST32   "o"
#define PRIoLEAST64 "llo"
#define PRIoFAST8     "o"
#define PRIoFAST16    "o"
#define PRIoFAST32    "o"
#define PRIoFAST64  "llo"
#define PRIoMAX      "jo"
#define PRIoPTR       "o"

#define PRIu8         "u"
#define PRIu16        "u"
#define PRIu32        "u"
#define PRIu64      "llu"
#define PRIuLEAST8    "u"
#define PRIuLEAST16   "u"
#define PRIuLEAST32   "u"
#define PRIuLEAST64 "llu"
#define PRIuFAST8     "u"
#define PRIuFAST16    "u"
#define PRIuFAST32    "u"
#define PRIuFAST64  "llu"
#define PRIuMAX      "ju"
#define PRIuPTR       "u"

#define PRIx8         "x"
#define PRIx16        "x"
#define PRIx32        "x"
#define PRIx64      "llx"
#define PRIxLEAST8    "x"
#define PRIxLEAST16   "x"
#define PRIxLEAST32   "x"
#define PRIxLEAST64 "llx"
#define PRIxFAST8     "x"
#define PRIxFAST16    "x"
#define PRIxFAST32    "x"
#define PRIxFAST64  "llx"
#define PRIxMAX      "jx"
#define PRIxPTR       "x"

#define PRIX8         "X"
#define PRIX16        "X"
#define PRIX32        "X"
#define PRIX64      "llX"
#define PRIXLEAST8    "X"
#define PRIXLEAST16   "X"
#define PRIXLEAST32   "X"
#define PRIXLEAST64 "llX"
#define PRIXFAST8     "X"
#define PRIXFAST16    "X"
#define PRIXFAST32    "X"
#define PRIXFAST64  "llX"
#define PRIXMAX      "jX"
#define PRIXPTR       "X"

#define SCNd16       "hd"
#define SCNd32        "d"
#define SCNd64      "lld"
#define SCNdLEAST16  "hd"
#define SCNdLEAST32   "d"
#define SCNdLEAST64 "lld"
#define SCNdFAST8     "d"
#define SCNdFAST16    "d"
#define SCNdFAST32    "d"
#define SCNdFAST64  "lld"
#define SCNdMAX      "jd"
#define SCNdPTR       "d"

#define SCNi16       "hi"
#define SCNi32        "i"
#define SCNi64      "lli"
#define SCNiLEAST16  "hi"
#define SCNiLEAST32   "i"
#define SCNiLEAST64 "lli"
#define SCNiFAST8     "i"
#define SCNiFAST16    "i"
#define SCNiFAST32    "i"
#define SCNiFAST64  "lli"
#define SCNiMAX      "ji"
#define SCNiPTR       "i"

#define SCNo16       "ho"
#define SCNo32        "o"
#define SCNo64      "llo"
#define SCNoLEAST16  "ho"
#define SCNoLEAST32   "o"
#define SCNoLEAST64 "llo"
#define SCNoFAST8     "o"
#define SCNoFAST16    "o"
#define SCNoFAST32    "o"
#define SCNoFAST64  "llo"
#define SCNoMAX      "jo"
#define SCNoPTR       "o"

#define SCNu16       "hu"
#define SCNu32        "u"
#define SCNu64      "llu"
#define SCNuLEAST16  "hu"
#define SCNuLEAST32   "u"
#define SCNuLEAST64 "llu"
#define SCNuFAST8     "u"
#define SCNuFAST16    "u"
#define SCNuFAST32    "u"
#define SCNuFAST64  "llu"
#define SCNuMAX      "ju"
#define SCNuPTR       "u"

#define SCNx16       "hx"
#define SCNx32        "x"
#define SCNx64      "llx"
#define SCNxLEAST16  "hx"
#define SCNxLEAST32   "x"
#define SCNxLEAST64 "llx"
#define SCNxFAST8     "x"
#define SCNxFAST16    "x"
#define SCNxFAST32    "x"
#define SCNxFAST64  "llx"
#define SCNxMAX      "jx"
#define SCNxPTR       "x"

#endif /* __STDC_FORMAT_MACROS */

#ifndef __cplusplus  /* wchar_t is a builtin type for C++ */
  #if !defined(__STRICT_ANSI__)
   /* unconditional in non-strict C for consistency of debug info */
    typedef unsigned short wchar_t; /* see <stddef.h> */
  #elif !defined(__wchar_t)
    #define __wchar_t 1
    typedef unsigned short wchar_t; /* see <stddef.h> */
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

intmax_t strtoimax(const char * /*restrict*/ /*nptr*/,
                   char ** /*restrict*/ /*endptr*/, int /*base*/);
    /* as for strtol */
uintmax_t strtoumax(const char * /*restrict*/ /*nptr*/,
                    char ** /*restrict*/ /*endptr*/, int /*base*/);
    /* as for strtoul */

#if 0 /* not yet implemented */
intmax_t wcstoimax(const wchar_t * restrict /*nptr*/,
                   wchar_t ** restrict /*endptr*/, int /*base*/);
uintmax_t wcstoumax(const wchar_t * restrict /*nptr*/,
                    wchar_t ** restrict /*endptr*/, int /*base*/);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __inttypes_h */

/* end of inttypes.h */

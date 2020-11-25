/* stddef.h: ANSI 'C' (X3J11 Oct 88) library header, section 4.1.4 */

/* Copyright (C) ARM Ltd., 1999
 * All rights reserved
 * RCS $Revision: 1.8.2.4 $
 * Checkin $Date: 2001/07/13 15:08:14 $
 * Revising $Author: cadeniyi $
 */

/* Copyright (C) Codemist Ltd., 1988                            */
/* Copyright 1991 ARM Limited. All rights reserved.             */
/* version 0.05 */

/*
 * The following types and macros are defined in several headers referred to in
 * the descriptions of the functions declared in that header. They are also
 * defined in this header file.
 */

#ifndef __stddef_h
#define __stddef_h

  #ifndef __STDDEF_DECLS
  #define __STDDEF_DECLS
    #undef __CLIBNS
    #ifdef __cplusplus
      #ifdef __EDG_RUNTIME_USES_NAMESPACES
        namespace std {
        #define __CLIBNS std::
      #else
        #define __CLIBNS ::
      #endif /* ifdef __EDG_RUNTIME_USES_NAMESPACES */

      extern "C" {
    #else
      #define __CLIBNS
    #endif  /* __cplusplus */

typedef int ptrdiff_t;

#if defined(__cplusplus) || !defined(__STRICT_ANSI__)
 /* unconditional in C++ and non-strict C for consistency of debug info */
  typedef unsigned int size_t;
#elif !defined(__size_t)
  #define __size_t 1
  typedef unsigned int size_t;   /* others (e.g. <stdio.h>) also define */
   /* the unsigned integral type of the result of the sizeof operator. */
#endif

#ifndef __cplusplus  /* wchar_t is a builtin type for C++ */
  #if !defined(__STRICT_ANSI__)
  /* unconditional in non-strict C for consistency of debug info */
    typedef unsigned short wchar_t; /* also in <stdlib.h> and <inttypes.h> */
  #elif !defined(__wchar_t)
    #define __wchar_t 1
    typedef unsigned short wchar_t; /* also in <stdlib.h> and <inttypes.h> */
   /*
    * An integral type whose range of values can represent distinct codes for
    * all members of the largest extended character set specified among the
    * supported locales; the null character shall have the code value zero and
    * each member of the basic character set shall have a code value when used
    * as the lone character in an integer character constant.
    */
  #endif
#endif

#undef NULL  /* others (e.g. <stdio.h>) also define */
#define NULL 0
   /* null pointer constant. */

#define offsetof(type, member) \
    ((__CLIBNS size_t)((char *)&(((type *)0)->member) - (char *)0))
   /*
    * expands to an integral constant expression that has type size_t, the
    * value of which is the offset in bytes, from the beginning of a structure
    * designated by type, of the member designated by the identifier (if the
    * specified member is a bit-field, the behaviour is undefined).
    */


    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __STDDEF_DECLS */


  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __STDDEF_NO_EXPORTS
      using std::size_t;
      using std::ptrdiff_t;
    #endif 
  #endif 

#endif

/* end of stddef.h */

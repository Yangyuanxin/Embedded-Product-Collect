/* ctype.h: ANSI 'C' (X3J11 Oct 88) library header, section 4.3 */
/* Copyright (C) Codemist Ltd. 1988-1993.                       */
/* Copyright 1991-1993 ARM Limited. All rights reserved.        */
/* version 0.03 */

/*
 * RCS $Revision: 1.10.2.2 $
 * Checkin $Date: 2001/05/23 21:56:07 $
 * Revising $Author: sdouglas $
 */

/*
 * ctype.h declares several functions useful for testing and mapping
 * characters. In all cases the argument is an int, the value of which shall
 * be representable as an unsigned char or shall equal the value of the
 * macro EOF. If the argument has any other value, the behaviour is undefined.
 */

#ifndef __ctype_h
#define __ctype_h

  #ifndef __CTYPE_DECLS
  #define __CTYPE_DECLS

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

/* N.B. - keep in step with <ctype.c> */

#define __S 1            /* whitespace           */
#define __P 2            /* punctuation          */
#define __B 4            /* blank                */
#define __L 8            /* lower case letter    */
#define __U 16           /* upper case letter    */
#define __N 32           /* (decimal) digit      */
#define __C 64           /* control chars        */
#define __X 128          /* A-F and a-f          */

extern __pure unsigned char **__rt_ctype_table(void);

#define __ctype (*__CLIBNS __rt_ctype_table())

#ifdef __cplusplus
    inline int isalnum(int __c) { return (__ctype[__c] & (__U+__L+__N)); }
#else
    #define isalnum(c) (__ctype[c] & (__U+__L+__N))
    extern int (isalnum)(int /*c*/);
#endif
    /* non-0 iff c is alphabetic or numeric */

#ifdef __cplusplus
    inline int isalpha(int __c) { return (__ctype[__c] & (__U+__L)); }
#else
    #define isalpha(c) (__ctype[c] & (__U+__L))
    extern int (isalpha)(int /*c*/);
#endif
    /* non-0 iff c is alphabetic */

#ifdef __cplusplus
    inline int iscntrl(int __c) { return (__ctype[__c] & __C); }
#else
    #define iscntrl(c) (__ctype[c] & __C)
    extern int (iscntrl)(int /*c*/);
#endif
    /* non-0 iff c is a control character - in the ASCII locale */
    /*       this means (c < ' ') || (c > '~')                  */

#ifdef __cplusplus
    inline int isdigit(int __c) { return (__ctype[__c] & __N); }
#else
    #define isdigit(c) (__ctype[c] & __N)
    extern int (isdigit)(int /*c*/);
#endif
    /* non-0 iff c is a decimal digit */

#ifdef __cplusplus
    inline int isgraph(int __c) { return (__ctype[__c] & (__L+__U+__N+__P)); }
#else
    #define isgraph(c) (__ctype[c] & (__L+__U+__N+__P))
    extern int (isgraph)(int /*c*/);
#endif
    /* non-0 iff c is any printing character other than ' ' */

#ifdef __cplusplus
    inline int islower(int __c) { return (__ctype[__c] & __L); }
#else
    #define islower(c) (__ctype[c] & __L)
    extern int (islower)(int /*c*/);
#endif
    /* non-0 iff c is a lower-case letter */

#ifdef __cplusplus
    inline int isprint(int __c) { return (__ctype[__c] & (__L+__U+__N+__P+__B)); }
#else
    #define isprint(c) (__ctype[c] & (__L+__U+__N+__P+__B))
    extern int (isprint)(int /*c*/);
#endif
    /* non-0 iff c is a printing character - in the ASCII locale */
    /*       this means 0x20 (space) -> 0x7E (tilde) */

#ifdef __cplusplus
    inline int ispunct(int __c) { return (__ctype[__c] & __P); }
#else
    #define ispunct(c) (__ctype[c] & __P)
    extern int (ispunct)(int /*c*/);
#endif
    /* non-0 iff c is a non-space, non-alpha-numeric, printing character */

#ifdef __cplusplus
    inline int isspace(int __c) { return (__ctype[__c] & __S); }
#else
    #define isspace(c) (__ctype[c] & __S)
    extern int (isspace)(int /*c*/);
#endif
    /* non-0 iff c is a white-space char: ' ', '\f', '\n', '\r', '\t', '\v'. */

#ifdef __cplusplus
    inline int isupper(int __c) { return (__ctype[__c] & __U); }
#else
    #define isupper(c) (__ctype[c] & __U)
    extern int (isupper)(int /*c*/);
#endif
    /* non-0 iff c is an upper-case letter */

#ifdef __cplusplus
    inline int isxdigit(int __c) { return (__ctype[__c] & (__N+__X)); }
#else
    #define isxdigit(c) (__ctype[c] & (__N+__X))
    extern int (isxdigit)(int /*c*/);
#endif
    /* non-0 iff c is a digit, in 'a'..'f', or in 'A'..'F' */

extern int tolower(int /*c*/);
    /* if c is an upper-case letter then return the corresponding */
    /* lower-case letter, otherwise return c.                     */

extern int toupper(int /*c*/);
    /* if c is an lower-case letter then return the corresponding */
    /* upper-case letter, otherwise return c.                     */

extern void __use_iso8859_locale(void);
extern void __use_iso8859_ctype(void);
extern void __use_iso8859_collate(void);
extern void __use_iso8859_monetary(void);
extern void __use_iso8859_numeric(void);

    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __CTYPE_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __CTYPE_NO_EXPORTS
      using std::isalnum;
      using std::isalpha;
      using std::iscntrl;
      using std::isdigit;
      using std::isgraph;
      using std::islower;
      using std::isprint;
      using std::ispunct;
      using std::isspace;
      using std::isupper;
      using std::isxdigit;
      using std::tolower;
      using std::toupper;
        using std::__use_iso8859_locale;
        using std::__use_iso8859_ctype;
        using std::__use_iso8859_collate;
        using std::__use_iso8859_monetary;
        using std::__use_iso8859_numeric;
    #endif
  #endif

#endif

/* end of ctype.h */



/* limits.h: ANSI 'C' (X3J11 Oct 88) library header, section 2.2.4.2 */
/* Copyright (C) Codemist Ltd., 1988                            */
/* Copyright 1991-1997 ARM Limited. All rights reserved         */

/*
 * RCS $Revision: 1.5 $
 * Checkin $Date: 1999/05/19 14:49:55 $
 * Revising $Author: wdijkstr $
 */

#ifndef __limits_h
#define __limits_h

#define CHAR_BIT 8
    /* max number of bits for smallest object that is not a bit-field (byte) */
#define SCHAR_MIN (-128)
    /* mimimum value for an object of type signed char */
#define SCHAR_MAX 127
    /* maximum value for an object of type signed char */
#define UCHAR_MAX 255
    /* maximum value for an object of type unsigned char */
#ifdef __FEATURE_SIGNED_CHAR
  #define CHAR_MIN (-128)
      /* minimum value for an object of type char */
  #define CHAR_MAX 127
      /* maximum value for an object of type char */
#else
  #define CHAR_MIN 0
      /* minimum value for an object of type char */
  #define CHAR_MAX 255
      /* maximum value for an object of type char */
#endif
#define MB_LEN_MAX 1
    /* maximum number of bytes in a multibyte character, */
    /* for any supported locale */

#define SHRT_MIN  (-0x8000)
    /* minimum value for an object of type short int */
#define SHRT_MAX  0x7fff
    /* maximum value for an object of type short int */
#define USHRT_MAX 65535
    /* maximum value for an object of type unsigned short int */
#define INT_MIN   (~0x7fffffff)  /* -2147483648 and 0x80000000 are unsigned */
    /* minimum value for an object of type int */
#define INT_MAX   0x7fffffff
    /* maximum value for an object of type int */
#define UINT_MAX  0xffffffffU
    /* maximum value for an object of type unsigned int */
#define LONG_MIN  (~0x7fffffffL)
    /* minimum value for an object of type long int */
#define LONG_MAX  0x7fffffffL
    /* maximum value for an object of type long int */
#define ULONG_MAX 0xffffffffUL
    /* maximum value for an object of type unsigned long int */
#ifndef __STRICT_ANSI__
  #define LLONG_MIN  (~0x7fffffffffffffffLL)
      /* minimum value for an object of type long long int */
  #define LLONG_MAX    0x7fffffffffffffffLL
      /* maximum value for an object of type long long int */
  #define ULLONG_MAX   0xffffffffffffffffULL
      /* maximum value for an object of type unsigned long int */
#endif

#endif

/* end of limits.h */

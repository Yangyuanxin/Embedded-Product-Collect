/* float.h: ANSI 'C' (X3J11 Oct 88) library header, section 2.2.4.2 */
/* Copyright (C) Codemist Ltd, 1988                             */
/* Copyright 1991 ARM Limited. All rights reserved.             */
/* version 0.01 */

/*
 * RCS $Revision: 1.8.2.1 $
 * Checkin $Date: 2001/02/22 16:23:35 $
 * Revising $Author: cadeniyi $
 */

#ifndef __float_h
#define __float_h

  #ifndef __FLOAT_DECLS
  #define __FLOAT_DECLS

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

/* IEEE version: the following values are taken from the above ANSI draft.  */
/* The ACORN FPE (v17) is known not to precisely implement IEEE arithmetic. */

#define FLT_RADIX     2
    /* radix of exponent representation */
#define FLT_ROUNDS    1
    /*
     * The rounding mode for floating-point addition is characterised by the
     * value of FLT_ROUNDS:
     *  -1 : indeterminable.
     *   0 : towards zero.
     *   1 : to nearest.
     *   2 : towards positive infinity.
     *   3 : towards negative infinity.
     *   ? : any other is implementation-defined.
     */

#define FLT_MANT_DIG        24
#define DBL_MANT_DIG        53
#define LDBL_MANT_DIG       53
    /* number of base-FLT_RADIX digits in the floating point mantissa */

/* The values that follow are not achieved under Acorn's FPE version 17  */
/* but they should be correct in due course!                             */

#define FLT_DIG      6
#define DBL_DIG      15
#define LDBL_DIG     15
    /* number of decimal digits of precision */

#define FLT_MIN_EXP  (-125)
#define DBL_MIN_EXP  (-1021)
#define LDBL_MIN_EXP (-1021)
    /* minimum negative integer such that FLT_RADIX raised to that power */
    /* minus 1 is a normalised floating-point number. */

#define FLT_MIN_10_EXP  (-37)
#define DBL_MIN_10_EXP  (-307)
#define LDBL_MIN_10_EXP (-307)
    /* minimum negative integer such that 10 raised to that power is in the */
    /* range of normalised floating-point numbers. */

#define FLT_MAX_EXP  128
#define DBL_MAX_EXP  1024
#define LDBL_MAX_EXP 1024
    /* maximum integer such that FLT_RADIX raised to that power minus 1 is a */
#define FLT_MAX_10_EXP  38
#define DBL_MAX_10_EXP  308
#define LDBL_MAX_10_EXP 308
    /* maximum integer such that 10 raised to that power is in the range of */
    /* representable finite floating-point numbers. */

#define FLT_MAX  3.40282347e+38F
#define DBL_MAX  1.79769313486231571e+308
#define LDBL_MAX 1.79769313486231571e+308L
    /* maximum representable finite floating-point number. */

#define FLT_EPSILON         1.19209290e-7F
#define DBL_EPSILON         2.2204460492503131e-16
#define LDBL_EPSILON        2.2204460492503131e-16L
    /* minimum positive floating point number x such that 1.0 + x != 1.0 */

#define FLT_MIN  1.17549435e-38F
#define DBL_MIN  2.22507385850720138e-308
#define LDBL_MIN 2.22507385850720138e-308L
    /* minimum normalised positive floating-point number. */

/*
 * The Microsoft <float.h> extensions.
 */
#ifdef __cplusplus
extern "C" {
#endif
unsigned _controlfp(unsigned, unsigned);
unsigned _clearfp(void);
unsigned _statusfp(void);
#ifdef __cplusplus
}
#endif


/*
 * Because the _EM_ constants are shared between _controlfp (masks)
 * and _statusfp (sticky bits), we adopt the convention that
 * _controlfp will shift its arguments left by 8 bits before using
 * them.
 */

#define _MCW_EM         0x001F
#define _EM_INVALID     0x0001
#define _EM_ZERODIVIDE  0x0002
#define _EM_OVERFLOW    0x0004
#define _EM_UNDERFLOW   0x0008
#define _EM_INEXACT     0x0010

#define _MCW_RC         0xC000
#define _RC_CHOP        0xC000
#define _RC_UP          0x4000
#define _RC_DOWN        0x8000
#define _RC_NEAR        0x0000

/*
 * _FPE_ constants passed as the hidden second argument to SIGFPE
 * handlers.
 */
#define _FPE_INVALID     0x04000000
#define _FPE_ZERODIVIDE  0x08000000
#define _FPE_OVERFLOW    0x10000000
#define _FPE_UNDERFLOW   0x20000000
#define _FPE_INEXACT     0x40000000


    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __FLOAT_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __FLOAT_NO_EXPORTS
      using std::_controlfp;
      using std::_clearfp;
      using std::_statusfp;
    #endif 
  #endif

/* end of float.h */
#endif /* __float_h */

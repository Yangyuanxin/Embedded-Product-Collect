/* fenv.h
 *
 * Copyright 1998 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.8 $
 * Checkin $Date: 2000/11/08 15:43:30 $
 * Revising $Author: statham $
 */

#ifndef __fenv_h
#define __fenv_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Types.
 */

typedef unsigned int __ieee_edata_t;     /* exception flags passed to traps */

typedef union {
    float f;
    float s;
    double d;
    int i;
    unsigned int ui;
    long long l;
    unsigned long long ul;
    struct { int word1, word2; } str;  
} __ieee_value_t;                        /* in/out values passed to traps */

typedef __softfp __ieee_value_t (*__ieee_handler_t) (__ieee_value_t, __ieee_value_t,
                                        __ieee_edata_t);

typedef struct {
    unsigned statusword;
    __ieee_handler_t invalid_handler;
    __ieee_handler_t divbyzero_handler;
    __ieee_handler_t overflow_handler;
    __ieee_handler_t underflow_handler;
    __ieee_handler_t inexact_handler;
} fenv_t, fexcept_t;

/*
 * Exception flags.
 */

#define FE_INVALID    (0x01)
#define FE_DIVBYZERO  (0x02)
#define FE_OVERFLOW   (0x04)
#define FE_UNDERFLOW  (0x08)
#define FE_INEXACT    (0x10)
#define FE_ALL_EXCEPT (0x1F)

/*
 * Rounding modes.
 */

#define FE_TONEAREST  (0)
#define FE_UPWARD     (1)
#define FE_DOWNWARD   (2)
#define FE_TOWARDZERO (3)

/*
 * Default environment.
 */

extern const fenv_t __fpl_default_env;
#define FE_DFL_ENV    (&__fpl_default_env)

/*
 * Exception functions.
 */

void fegetexceptflag(fexcept_t *, int);
void fesetexceptflag(const fexcept_t *, int);

void feclearexcept(int);
void feraiseexcept(int);
int fetestexcept(int);

/*
 * Rounding functions.
 */

int fegetround(void);
int fesetround(int);

/*
 * Full environment access.
 */

void fegetenv(fenv_t *);
void fesetenv(const fenv_t *);

int feholdexcept(fenv_t *);
void feupdateenv(const fenv_t *);

/*
 * Implementation-specific: ARM's own optimal status word access
 * function, and the macros that go with it.
 */
extern unsigned __ieee_status(unsigned, unsigned);
#define FE_IEEE_FLUSHZERO          (0x01000000)
#define FE_IEEE_ROUND_TONEAREST    (0x00000000)
#define FE_IEEE_ROUND_UPWARD       (0x00400000)
#define FE_IEEE_ROUND_DOWNWARD     (0x00800000)
#define FE_IEEE_ROUND_TOWARDZERO   (0x00C00000)
#define FE_IEEE_ROUND_MASK         (0x00C00000)
#define FE_IEEE_MASK_INVALID       (0x00000100)
#define FE_IEEE_MASK_DIVBYZERO     (0x00000200)
#define FE_IEEE_MASK_OVERFLOW      (0x00000400)
#define FE_IEEE_MASK_UNDERFLOW     (0x00000800)
#define FE_IEEE_MASK_INEXACT       (0x00001000)
#define FE_IEEE_MASK_ALL_EXCEPT    (0x00001F00)
#define FE_IEEE_INVALID            (0x00000001)
#define FE_IEEE_DIVBYZERO          (0x00000002)
#define FE_IEEE_OVERFLOW           (0x00000004)
#define FE_IEEE_UNDERFLOW          (0x00000008)
#define FE_IEEE_INEXACT            (0x00000010)
#define FE_IEEE_ALL_EXCEPT         (0x0000001F)

/*
 * The constants that define the format of __ieee_edata_t.
 */

#define FE_EX_RDIR              (-1 << 31)
#define FE_EX_EXCEPT_SHIFT      (26)
#define FE_EX_EXCEPT_MASK       (0x1F << FE_EX_EXCEPT_SHIFT)
#define FE_EX_INEXACT           (0x10 << FE_EX_EXCEPT_SHIFT)
#define FE_EX_UNDERFLOW         (0x08 << FE_EX_EXCEPT_SHIFT)
#define FE_EX_OVERFLOW          (0x04 << FE_EX_EXCEPT_SHIFT)
#define FE_EX_DIVBYZERO         (0x02 << FE_EX_EXCEPT_SHIFT)
#define FE_EX_INVALID           (0x01 << FE_EX_EXCEPT_SHIFT)
#define FE_EX_FLUSHZERO_SHIFT   (24)
#define FE_EX_FLUSHZERO         (1 << FE_EX_FLUSHZERO_SHIFT)
#define FE_EX_ROUND_SHIFT       (22)
#define FE_EX_ROUND_MASK        (3 << FE_EX_ROUND_SHIFT)
#define FE_EX_INTYPE_SHIFT      (7)
#define FE_EX_INTYPE_MASK       (7 << FE_EX_INTYPE_SHIFT)
#define FE_EX_OUTTYPE_SHIFT     (4)
#define FE_EX_OUTTYPE_MASK      (7 << FE_EX_OUTTYPE_SHIFT)
#define FE_EX_TYPE_SHIFT        (4)
#define FE_EX_TYPE_MASK         (7 << FE_EX_TYPE_SHIFT)
#define FE_EX_FN_SHIFT          (0)
#define FE_EX_FN_MASK           (15 << FE_EX_FN_SHIFT)

#define FE_EX_ROUND_NEAREST     (0 << FE_EX_ROUND_SHIFT)
#define FE_EX_ROUND_PLUSINF     (1 << FE_EX_ROUND_SHIFT)
#define FE_EX_ROUND_MINUSINF    (2 << FE_EX_ROUND_SHIFT)
#define FE_EX_ROUND_ZERO        (3 << FE_EX_ROUND_SHIFT)

#define FE_EX_BASETYPE_FLOAT    (0)
#define FE_EX_BASETYPE_DOUBLE   (1)
#define FE_EX_BASETYPE_INT      (4)
#define FE_EX_BASETYPE_LONGLONG (5)
#define FE_EX_BASETYPE_UINT     (6)
#define FE_EX_BASETYPE_ULONGLONG (7)

#define FE_EX_TYPE_FLOAT        (FE_EX_BASETYPE_FLOAT << FE_EX_TYPE_SHIFT)
#define FE_EX_TYPE_DOUBLE       (FE_EX_BASETYPE_DOUBLE << FE_EX_TYPE_SHIFT)
#define FE_EX_TYPE_INT          (FE_EX_BASETYPE_INT << FE_EX_TYPE_SHIFT)
#define FE_EX_TYPE_LONGLONG     (FE_EX_BASETYPE_LONGLONG << FE_EX_TYPE_SHIFT)
#define FE_EX_TYPE_UINT         (FE_EX_BASETYPE_UINT << FE_EX_TYPE_SHIFT)
#define FE_EX_TYPE_ULONGLONG    (FE_EX_BASETYPE_ULONGLONG << FE_EX_TYPE_SHIFT)

#define FE_EX_INTYPE_FLOAT      (FE_EX_BASETYPE_FLOAT << FE_EX_INTYPE_SHIFT)
#define FE_EX_INTYPE_DOUBLE     (FE_EX_BASETYPE_DOUBLE << FE_EX_INTYPE_SHIFT)
#define FE_EX_INTYPE_INT        (FE_EX_BASETYPE_INT << FE_EX_INTYPE_SHIFT)
#define FE_EX_INTYPE_LONGLONG   (FE_EX_BASETYPE_LONGLONG << FE_EX_INTYPE_SHIFT)
#define FE_EX_INTYPE_UINT       (FE_EX_BASETYPE_UINT << FE_EX_INTYPE_SHIFT)
#define FE_EX_INTYPE_ULONGLONG  (FE_EX_BASETYPE_ULONGLONG<<FE_EX_INTYPE_SHIFT)

#define FE_EX_OUTTYPE_FLOAT     (FE_EX_BASETYPE_FLOAT << FE_EX_OUTTYPE_SHIFT)
#define FE_EX_OUTTYPE_DOUBLE    (FE_EX_BASETYPE_DOUBLE << FE_EX_OUTTYPE_SHIFT)
#define FE_EX_OUTTYPE_INT       (FE_EX_BASETYPE_INT << FE_EX_OUTTYPE_SHIFT)
#define FE_EX_OUTTYPE_LONGLONG  (FE_EX_BASETYPE_LONGLONG<<FE_EX_OUTTYPE_SHIFT)
#define FE_EX_OUTTYPE_UINT      (FE_EX_BASETYPE_UINT << FE_EX_OUTTYPE_SHIFT)
#define FE_EX_OUTTYPE_ULONGLONG (FE_EX_BASETYPE_ULONGLONG<<FE_EX_OUTTYPE_SHIFT)

#define FE_EX_FN_ADD            (1 << FE_EX_FN_SHIFT)
#define FE_EX_FN_SUB            (2 << FE_EX_FN_SHIFT)
#define FE_EX_FN_MUL            (3 << FE_EX_FN_SHIFT)
#define FE_EX_FN_DIV            (4 << FE_EX_FN_SHIFT)
#define FE_EX_FN_REM            (5 << FE_EX_FN_SHIFT)
#define FE_EX_FN_RND            (6 << FE_EX_FN_SHIFT)
#define FE_EX_FN_SQRT           (7 << FE_EX_FN_SHIFT)
#define FE_EX_FN_CVT            (8 << FE_EX_FN_SHIFT)
#define FE_EX_FN_CMP            (9 << FE_EX_FN_SHIFT)
#define FE_EX_FN_RAISE          (15 << FE_EX_FN_SHIFT)

#define FE_EX_CMPRET_UNORDERED  (8)
#define FE_EX_CMPRET_LESS       (4)
#define FE_EX_CMPRET_EQUAL      (2)
#define FE_EX_CMPRET_GREATER    (1)

#ifdef __cplusplus
}
#endif

#endif

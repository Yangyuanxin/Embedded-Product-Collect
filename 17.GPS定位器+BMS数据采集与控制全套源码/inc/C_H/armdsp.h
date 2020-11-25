/* armdsp.h
 *
 * Copyright 2001 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.1.2.2 $
 * Checkin $Date: 2001/07/31 16:10:40 $
 * Revising $Author: statham $
 */

/* ----------------------------------------------------------------------
 * This header file provides a set of DSP-type primitive
 * operations, such as saturating integer arithmetic and 16x16 bit
 * multiplication. The operations it provides are exactly the same
 * operations supported directly by the ARM9E.
 * 
 * This header file is intended as an example implementation. It
 * demonstrates how to use the inline assembly feature in the ARM
 * compilers, to construct intrinsic functions that provide
 * C-language access to the ARM9E's DSP capability.
 */

#ifndef ARMDSP_ARMDSP_H
#define ARMDSP_ARMDSP_H

#ifndef ARMDSP_WORD32_DEFINED
#define ARMDSP_WORD32_DEFINED
typedef signed int Word32;
typedef signed short Word16;
#define ARMDSP_WORD32_MAX ((Word32)0x7FFFFFFF)
#define ARMDSP_WORD32_MIN ((Word32)-0x80000000)
#define ARMDSP_WORD16_MAX ((Word16)0x7FFF)
#define ARMDSP_WORD16_MIN ((Word16)-0x8000)
#endif

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/*
 * Saturating 32-bit integer addition.
 */
__inline Word32 qadd(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        qadd ret, x, y;
    }
    return ret;
}

/*
 * Saturating 32-bit integer subtraction.
 */
__inline Word32 qsub(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        qsub ret, x, y;
    }
    return ret;
}

/*
 * Saturating 32-bit integer addition, with the second operand
 * multiplied by two. (i.e. return x + 2 * y)
 */
__inline Word32 qdadd(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        qdadd ret, x, y;
    }
    return ret;
}

/*
 * Saturating 32-bit integer subtraction, with the second operand
 * multiplied by two. (i.e. return x - 2 * y)
 */
__inline Word32 qdsub(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        qdsub ret, x, y;
    }
    return ret;
}

/*
 * Multiply two signed 16-bit numbers, taken from the bottom or top
 * halves of the input words. For example, smulbt(x,y) multiplies
 * the bottom 16 bits of x with the top 16 bits of y.
 */

/* smulbb - multiply bottom half of x and bottom half of y */
__inline Word32 smulbb(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        smulbb ret, x, y;
    }
    return ret;
}

/* smulbt - multiply bottom half of x and top half of y */
__inline Word32 smulbt(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        smulbt ret, x, y;
    }
    return ret;
}

/* smultb - multiply top half of x and bottom half of y */
__inline Word32 smultb(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        smultb ret, x, y;
    }
    return ret;
}

/* smultt - multiply top half of x and top half of y */
__inline Word32 smultt(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        smultt ret, x, y;
    }
    return ret;
}

/*
 * Multiply two signed 16-bit numbers, taken from the bottom or top
 * halves of the input words x and y, and add to the input word a.
 * For example, smlabt(x,y,a) multiplies the bottom 16 bits of x with
 * the top 16 bits of y, and then adds a.
 */

/* smlabb - multiply bottom half of x and bottom half of y */
__inline Word32 smlabb(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
    __asm {
        smlabb ret, x, y, a;
    }
    return ret;
}

/* smlabt - multiply bottom half of x and top half of y */
__inline Word32 smlabt(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
    __asm {
        smlabt ret, x, y, a;
    }
    return ret;
}

/* smlatb - multiply top half of x and bottom half of y */
__inline Word32 smlatb(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
    __asm {
        smlatb ret, x, y, a;
    }
    return ret;
}

/* smlatt - multiply top half of x and top half of y */
__inline Word32 smlatt(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
    __asm {
        smlatt ret, x, y, a;
    }
    return ret;
}

/*
 * Multiply two signed 16-bit numbers, taken from the bottom or top
 * halves of the input words x and y, and add to the 64-bit integer
 * stored in the input words lo and hi. For example,
 * smlalbt(&lo,&hi,x,y) multiplies the bottom 16 bits of x with the
 * top 16 bits of y, and then adds the result to (hi,lo).
 * 
 * WARNING: if you use these primitives too often inside the same
 * function, they may fail to be inlined.
 */

/* smlalbb - multiply bottom half of x and bottom half of y */
__inline void smlalbb(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
    __asm {
        smlalbb *lo, *hi, x, y;
    }
}

/* smlalbt - multiply bottom half of x and top half of y */
__inline void smlalbt(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
    __asm {
        smlalbt *lo, *hi, x, y;
    }
}

/* smlaltb - multiply top half of x and bottom half of y */
__inline void smlaltb(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
    __asm {
        smlaltb *lo, *hi, x, y;
    }
}

/* smlaltt - multiply top half of x and top half of y */
__inline void smlaltt(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
    __asm {
        smlaltt *lo, *hi, x, y;
    }
}

/*
 * Multiply a 32-bit signed integer (the input word x) with a
 * 16-bit signed integer (taken from either the top or the bottom
 * half of the input word y). Discard the bottom 16 bits of the
 * 48-bit product.
 */

/* smulwb - multiply x by the bottom half of y */
__inline Word32 smulwb(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        smulwb ret, x, y;
    }
    return ret;
}

/* smulwt - multiply x by the top half of y */
__inline Word32 smulwt(Word32 x, Word32 y)
{
    Word32 ret;
    __asm {
        smulwt ret, x, y;
    }
    return ret;
}

/*
 * Multiply a 32-bit signed integer (the input word x) with a
 * 16-bit signed integer (taken from either the top or the bottom
 * half of the input word y). Discard the bottom 16 bits of the
 * 48-bit product, and add to the input word a.
 */

/* smlawb - multiply x by the bottom half of y */
__inline Word32 smlawb(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
    __asm {
        smlawb ret, x, y, a;
    }
    return ret;
}

/* smlawt - multiply x by the top half of y */
__inline Word32 smlawt(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
    __asm {
        smlawt ret, x, y, a;
    }
    return ret;
}

#ifdef __cplusplus
}
#endif

#endif /* ARMDSP_ARMDSP_H */

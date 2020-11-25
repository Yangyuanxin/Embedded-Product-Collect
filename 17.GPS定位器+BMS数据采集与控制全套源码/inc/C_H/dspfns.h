/* dspfns.h
 *
 * Copyright 2001 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.1.2.2 $
 * Checkin $Date: 2001/07/31 16:10:40 $
 * Revising $Author: statham $
 */

/* ----------------------------------------------------------------------
 * This header file provides a set of DSP-type primitive
 * operations, such as 16-bit and 32-bit saturating arithmetic. The
 * operations it provides are similar to the ones used by the ITU
 * for publishing specifications of DSP algorithms.
 * 
 * This header file is intended as an example implementation. It
 * demonstrates how to use the inline assembly feature in the ARM
 * compilers, to construct intrinsic functions that provide
 * C-language access to the ARM9E's DSP capability.
 * 
 * This header file is NOT SUFFICIENT to be used alone - you need
 * to include "dspfns.c" from the examples\dsp directory as well.
 */

#ifndef ARMDSP_DSPFNS_H
#define ARMDSP_DSPFNS_H

#include <assert.h>

#ifndef ARMDSP_WORD32_DEFINED
#define ARMDSP_WORD32_DEFINED
typedef signed int Word32;
typedef signed short Word16;
#define ARMDSP_WORD32_MAX ((Word32)0x7FFFFFFF)
#define ARMDSP_WORD32_MIN ((Word32)-0x80000000)
#define ARMDSP_WORD16_MAX ((Word16)0x7FFF)
#define ARMDSP_WORD16_MIN ((Word16)-0x8000)
#endif

typedef int Flag;

/*
 * This global variable needs to exist somewhere in the compiled
 * program in order for the flag-using functions to work. You can
 * either include the declaration
 * 
 *   armdsp_flagdata_union armdsp_flagdata;
 * 
 * in at least one of your source files (that includes this
 * header), or compile in "dspfns.c" from the examples\dsp
 * directory.
 */
typedef union {
    unsigned int armdsp_flags_word;
    struct {
#ifdef __BIG_ENDIAN
        Flag armdsp_n:1, armdsp_z:1, armdsp_c:1, armdsp_v:1, armdsp_q:1, armdsp_unused:27;
#else
        Flag armdsp_unused:27, armdsp_q:1, armdsp_v:1, armdsp_c:1, armdsp_z:1, armdsp_n:1;
#endif
    } armdsp_bitfields;
} armdsp_flagdata_union;
extern armdsp_flagdata_union armdsp_flagdata;

#define Carry ( armdsp_flagdata.armdsp_bitfields.armdsp_c )
#define Overflow ( armdsp_flagdata.armdsp_bitfields.armdsp_q )

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/*
 * Convert a 32-bit signed integer into a 16-bit signed integer by
 * saturation.
 */
__inline Word16 saturate(Word32 x)
{
    if (x > ARMDSP_WORD16_MAX)
        Overflow = 1, x = ARMDSP_WORD16_MAX;
    else if (x < ARMDSP_WORD16_MIN)
        Overflow = 1, x = ARMDSP_WORD16_MIN;
    return (Word16) x;
}

/*
 * Add two 16-bit signed integers with saturation.
 */
__inline Word16 add(Word16 x, Word16 y)
{
    Word32 xs, ys, rs;
    __asm {
        mov xs, x, lsl #16;
        mov ys, y, lsl #16;
        qadd rs, xs, ys;
    }
    return (Word16) (rs >> 16);
}

/*
 * Subtract one 16-bit signed integer from another with saturation.
 */
__inline Word16 sub(Word16 x, Word16 y)
{
    Word32 xs, ys, rs;
    __asm {
        mov xs, x, lsl #16;
        mov ys, y, lsl #16;
        qsub rs, xs, ys;
    }
    return (Word16) (rs >> 16);
}

/*
 * Absolute value of a 16-bit signed integer. Saturating, so
 * abs(-0x8000) becomes +0x7FFF.
 */
__inline Word16 abs_s(Word16 x)
{
    if (x >= 0)
        return x;
    else if (x == ARMDSP_WORD16_MIN)
        return ARMDSP_WORD16_MAX;
    else
        return (Word16) -x;
}

/*
 * Shift a 16-bit signed integer left (or right, if the shift count
 * is negative). Saturate on overflow.
 */
__inline Word16 shl(Word16 x, Word16 shift)
{
    if (shift <= 0 || x == 0)
        return (Word16) (x >> (-shift));
    if (shift > 15)
        shift = 16;
    return saturate(x << shift);
}

/*
 * Shift a 16-bit signed integer right (or left, if the shift count
 * is negative). Saturate on overflow.
 */
__inline Word16 shr(Word16 x, Word16 shift)
{
    if (shift >= 0 || x == 0)
        return (Word16) (x >> shift);
    if (shift < -15)
        shift = -16;
    return saturate(x << (-shift));
}

/*
 * Multiply two 16-bit signed integers, shift the result right by
 * 15 and saturate it. (Saturation is only necessary if both inputs
 * were -0x8000, in which case the result "should" be 0x8000 and is
 * saturated to 0x7FFF.)
 */
__inline Word16 mult(Word16 x, Word16 y)
{
    Word32 product;
    __asm {
        smulbb product, x, y;
        qadd product, product, product;
    }
    return (Word16) (product >> 16);   /* the qadd added one to the 15 */
}

/*
 * Multiply two 16-bit signed integers to give a 32-bit signed
 * integer. Shift left by one, and saturate the result. (Saturation
 * is only necessary if both inputs were -0x8000, in which case the
 * result "should" be 0x40000000 << 1 = +0x80000000, and is
 * saturated to +0x7FFFFFFF.)
 */
__inline Word32 L_mult(Word16 x, Word16 y)
{
    Word32 product;
    __asm {
        smulbb product, x, y;
        qadd product, product, product;
    }
    return product;
}

/*
 * Negate a 16-bit signed integer, with saturation. (Saturation is
 * only necessary when the input is -0x8000.)
 */
__inline Word16 negate(Word16 x)
{
    if (x == ARMDSP_WORD16_MIN)
        return ARMDSP_WORD16_MAX;
    return (Word16) -x;
}

/*
 * Return the top 16 bits of a 32-bit signed integer.
 */
__inline Word16 extract_h(Word32 x)
{
    return (Word16) (x >> 16);
}

/*
 * Return the bottom 16 bits of a 32-bit signed integer, with no
 * saturation, just coerced into a two's complement 16 bit
 * representation.
 */
__inline Word16 extract_l(Word32 x)
{
    return (Word16) x;
}

/*
 * Divide a 32-bit signed integer by 2^16, rounding to the nearest
 * integer (round up on a tie). Equivalent to adding 0x8000 with
 * saturation, then shifting right by 16.
 */
__inline Word16 round(Word32 x)
{
    __asm {
        qadd x, x, 0x8000;
    }
    return extract_h(x);
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and add to
 * another 32-bit integer with saturation.
 * 
 * Note the intermediate saturation operation in the definition:
 * 
 *    L_mac(-1, -0x8000, -0x8000)
 * 
 * will give 0x7FFFFFFE and not 0x7FFFFFFF:
 *    the unshifted product is:   0x40000000
 *    shift left with saturation: 0x7FFFFFFF
 *    add to -1 with saturation:  0x7FFFFFFE
 */
__inline Word32 L_mac(Word32 accumulator, Word16 x, Word16 y)
{
    Word32 product;
    __asm {
        smulbb product, x, y;
        qdadd accumulator, accumulator, product;
    }
    return accumulator;
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and subtract
 * from another 32-bit integer with saturation.
 * 
 * Note the intermediate saturation operation in the definition:
 * 
 *    L_msu(1, -0x8000, -0x8000)
 * 
 * will give 0x80000002 and not 0x80000001:
 *    the unshifted product is:         0x40000000
 *    shift left with saturation:       0x7FFFFFFF
 *    subtract from 1 with saturation:  0x80000002
 */
__inline Word32 L_msu(Word32 accumulator, Word16 x, Word16 y)
{
    Word32 product;
    __asm {
        smulbb product, x, y;
        qdsub accumulator, accumulator, product;
    }
    return accumulator;
}

/*
 * Add two 32-bit signed integers with saturation.
 */
__inline Word32 L_add(Word32 x, Word32 y)
{
    Word32 result;
    __asm {
        qadd result, x, y;
    }
    return result;
}

/*
 * Subtract one 32-bit signed integer from another with saturation.
 */
__inline Word32 L_sub(Word32 x, Word32 y)
{
    Word32 result;
    __asm {
        qsub result, x, y;
    }
    return result;
}

/*
 * Add together the Carry variable and two 32-bit signed integers,
 * without saturation.
 */
__inline Word32 L_add_c(Word32 x, Word32 y)
{
    Word32 result, flags;

    __asm {
        msr CPSR_f, armdsp_flagdata.armdsp_flags_word;
        adcs result, x, y;
        mrs flags, CPSR;
        orrvs flags, flags, #0x08000000; // set Q if V is set
        mov armdsp_flagdata.armdsp_flags_word, flags;
    }
    return result;
}

/*
 * Subtract one 32-bit signed integer, together with the Carry
 * variable, from another 32-bit signed integer, without
 * saturation.
 */
__inline Word32 L_sub_c(Word32 x, Word32 y)
{
    Word32 result, flags;

    __asm {
        msr CPSR_f, armdsp_flagdata.armdsp_flags_word;
        sbcs result, x, y;
        mrs flags, CPSR;
        orrvs flags, flags, #0x08000000; // set Q if V is set
        mov armdsp_flagdata.armdsp_flags_word, flags;
    }
    return result;
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one _with_ saturation, and add
 * with carry to another 32-bit integer _without_ saturation.
 */
__inline Word32 L_macNs(Word32 accumulator, Word16 x, Word16 y)
{
    return L_add_c(accumulator, L_mult(x, y));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one _with_ saturation, and
 * subtract with carry from another 32-bit integer _without_
 * saturation.
 */
__inline Word32 L_msuNs(Word32 accumulator, Word16 x, Word16 y)
{
    return L_sub_c(accumulator, L_mult(x, y));
}

/*
 * Negate a 32-bit signed integer, with saturation. (Saturation is
 * only necessary when the input is -0x80000000.)
 */
__inline Word32 L_negate(Word32 x)
{
    if (x == ARMDSP_WORD32_MIN)
        return ARMDSP_WORD32_MAX;
    return -x;
}

/*
 * Multiply two 16-bit signed integers, shift the result right by
 * 15 with rounding, and saturate it. (Saturation is only necessary
 * if both inputs were -0x8000, in which case the result "should"
 * be 0x8000 and is saturated to 0x7FFF.)
 */
__inline Word16 mult_r(Word16 x, Word16 y)
{
    Word32 product;
    __asm {
        smulbb product, x, y;
        qdadd product, 0x8000, product;
    }
    return (Word16) (product >> 16);   /* qdadd adds one to the 15 */
}

/*
 * Shift a 32-bit signed integer left (or right, if the shift count
 * is negative). Saturate on overflow.
 */
__inline Word32 L_shl(Word32 x, Word16 shift)
{
    int lz;
    int absx;
    if (shift <= 0)
        return x >> (-shift);
    absx = (x < 0 ? -x : x);
    __asm {
        clz lz, absx;
    }
    if (shift < lz || x == 0)
        return x << shift;
    if (x < 0)
        return ARMDSP_WORD32_MIN;
    else
        return ARMDSP_WORD32_MAX;
}

/*
 * Shift a 32-bit signed integer right (or left, if the shift count
 * is negative). Saturate on overflow.
 */
__inline Word32 L_shr(Word32 x, Word16 shift)
{
    int lz;
    int absx;
    if (shift >= 0)
        return x >> shift;
    absx = (x < 0 ? -x : x);
    __asm {
        clz lz, absx;
    }
    if (-shift < lz || x == 0)
        return x << (-shift);
    if (x < 0)
        return ARMDSP_WORD32_MIN;
    else
        return ARMDSP_WORD32_MAX;
}

/*
 * Shift a 16-bit signed integer right, with rounding. Shift left
 * with saturation if the shift count is negative.
 */
__inline Word16 shr_r(Word16 x, Word16 shift)
{
    if (shift == 0 || x == 0)
        return (Word16) (x >> shift);
    if (shift > 0)
        return (Word16) (((x >> (shift-1)) + 1) >> 1);
    if (shift < -15)
        shift = -16;
    return saturate(x << (-shift));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and add to
 * another 32-bit integer with saturation (like L_mac). Then shift
 * the result right by 15 bits with rounding (like round).
 */
__inline Word16 mac_r(Word32 accumulator, Word16 x, Word16 y)
{
    return round(L_mac(accumulator, x, y));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and subtract
 * from another 32-bit integer with saturation (like L_msu). Then
 * shift the result right by 15 bits with rounding (like round).
 */
__inline Word16 msu_r(Word32 accumulator, Word16 x, Word16 y)
{
    return round(L_msu(accumulator, x, y));
}

/*
 * Shift a 16-bit signed integer left by 16 bits to generate a
 * 32-bit signed integer. The bottom 16 bits are zeroed.
 */
__inline Word32 L_deposit_h(Word16 x)
{
    return ((Word32)x) << 16;
}

/*
 * Sign-extend a 16-bit signed integer by 16 bits to generate a
 * 32-bit signed integer.
 */
__inline Word32 L_deposit_l(Word16 x)
{
    return (Word32)x;
}

/*
 * Shift a 32-bit signed integer right, with rounding. Shift left
 * with saturation if the shift count is negative.
 */
__inline Word32 L_shr_r(Word32 x, Word16 shift)
{
    int lz;
    int absx;
    if (shift == 0 || x == 0)
        return x >> shift;
    if (shift > 0) {
        Word32 x2 = x >> (shift-1);
        return (x2 >> 1) + (x2 & 1);
    }
    absx = (x < 0 ? -x : x);
    __asm {
        clz lz, absx;
    }
    if (-shift < lz || x == 0)
        return x << (-shift);
    if (x < 0)
        return ARMDSP_WORD32_MIN;
    else
        return ARMDSP_WORD32_MAX;
}

/*
 * Absolute value of a 32-bit signed integer. Saturating, so
 * abs(-0x80000000) becomes +0x7FFFFFFF.
 */
__inline Word32 L_abs(Word32 x)
{
    if (x >= 0)
        return x;
    else if (x == ARMDSP_WORD32_MIN)
        return ARMDSP_WORD32_MAX;
    else
        return -x;
}

/*
 * Return a saturated value appropriate to the most recent carry-
 * affecting operation (L_add_c, L_macNs, L_sub_c, L_msuNs).
 * 
 * In other words: return the argument if the Q flag is clear.
 * Otherwise, return -0x80000000 or +0x7FFFFFFF depending on
 * whether the Carry flag is set or clear (respectively).
 */
__inline Word32 L_sat(Word32 x)
{
    int qflag;
    __asm {
        mrs qflag, CPSR;
    }
    if (!(qflag & 0x08000000))
        return x;
    if (Carry)
        return ARMDSP_WORD32_MIN;
    else
        return ARMDSP_WORD32_MAX;
}

/*
 * Return the number of bits of left shift needed to arrange for a
 * 16-bit signed integer to have value >= 0x4000 or <= -0x4001.
 * 
 * Returns zero if x is zero.
 */
__inline Word16 norm_s(Word16 x)
{
    int lz;
    if (x == 0)
        return 0;                      /* 0 is a special case */
    x = (Word16) (x ^ (x >> 15));      /* invert x if it's negative */
    __asm {
        clz lz, x;
    }
    return (Word16) (lz - 17);
}

/*
 * Divide one 16-bit signed integer by another, and produce a
 * 15-bit fixed point fractional result (by multiplying the true
 * mathematical result by 0x8000). The divisor (denominator) is
 * assumed to be non-zero and also assumed to be greater or equal
 * to the dividend (numerator). Hence the (unscaled) result is
 * necessarily within the range [0,1].
 * 
 * Both operands are assumed to be positive.
 * 
 * After division, the result is saturated to fit into a 16-bit
 * signed integer. (The only way this can happen is if the operands
 * are equal, so that the result would be 1, i.e. +0x8000 in 15-bit
 * fixed point.)
 */
__inline Word16 div_s(Word16 x, Word16 y)
{
    Word32 quot;
    assert(y > 0);
    assert(x >= 0);
    assert(x <= y);
    quot = 0x8000 * x;
    quot /= y;
    if (quot > ARMDSP_WORD16_MAX)
        return ARMDSP_WORD16_MAX;
    else
        return (Word16)quot;
}

/*
 * Return the number of bits of left shift needed to arrange for a
 * 32-bit signed integer to have value >= 0x40000000 (if 
 * 
 * Returns zero if x is zero.
 */
__inline Word16 norm_l(Word32 x)
{
    int lz;
    if (x == 0)
        return 0;                      /* 0 is a special case */
    x ^= (x >> 31);                    /* invert x if it's negative */
    __asm {
        clz lz, x;
    }
    return (Word16) (lz - 1);
}

#ifdef __cplusplus
}
#endif

#endif /* ARMDSP_DSPFNS_H */

/* rt_fp.h: prototypes for the fplib basic-arithmetic functions
 *
 * Copyright 1999 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.4 $
 * Checkin $Date: 2000/11/08 15:43:29 $
 * Revising $Author: statham $
 */

#ifndef __RT_FP_H
#define __RT_FP_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Trivial routines.
 */
extern __softfp float _fneg(float);
extern __softfp float _fabs(float);
extern __softfp double _dneg(double);
extern __softfp double _dabs(double);

/*
 * Single-precision arithmetic routines.
 */
extern __softfp float _fadd(float, float);
extern __softfp float _fsub(float, float);
extern __softfp float _frsb(float, float);
extern __softfp float _fmul(float, float);
extern __softfp float _fdiv(float, float);
extern __softfp float _frdiv(float, float);
extern __softfp float _frem(float, float);
extern __softfp float _frnd(float);
extern __softfp float _fsqrt(float);

/*
 * Double-precision arithmetic routines.
 */
extern __softfp double _dadd(double, double);
extern __softfp double _dsub(double, double);
extern __softfp double _drsb(double, double);
extern __softfp double _dmul(double, double);
extern __softfp double _ddiv(double, double);
extern __softfp double _drdiv(double, double);
extern __softfp double _drem(double, double);
extern __softfp double _drnd(double);
extern __softfp double _dsqrt(double);

/*
 * Boolean-return compares ({f,d}cmp{eq,ge,le} are not listed
 * because they return results in the flags and so cannot be
 * described by C prototypes).
 */
extern __softfp int _feq(float, float);
extern __softfp int _fneq(float, float);
extern __softfp int _fgeq(float, float);
extern __softfp int _fgr(float, float);
extern __softfp int _fleq(float, float);
extern __softfp int _fls(float, float);
extern __softfp int _deq(double, double);
extern __softfp int _dneq(double, double);
extern __softfp int _dgeq(double, double);
extern __softfp int _dgr(double, double);
extern __softfp int _dleq(double, double);
extern __softfp int _dls(double, double);

/*
 * Floating-to-floating format conversions.
 */
extern __softfp double _f2d(float);
extern __softfp float _d2f(double);

/*
 * Integer-to-floating format conversions.
 */
extern __softfp float _fflt(int);
extern __softfp float _ffltu(unsigned int);
extern __softfp double _dflt(int);
extern __softfp double _dfltu(unsigned int);
extern __softfp float _ll_sto_f(long long);
extern __softfp float _ll_uto_f(unsigned long long);
extern __softfp double _ll_sto_d(long long);
extern __softfp double _ll_uto_d(unsigned long long);

/*
 * Floating-to-integer format conversions, rounding toward zero
 * always.
 */
extern __softfp int _ffix(float);
extern __softfp unsigned int _ffixu(float);
extern __softfp int _dfix(double);
extern __softfp unsigned int _dfixu(double);
extern __softfp long long _ll_sfrom_f(float);
extern __softfp unsigned long long _ll_ufrom_f(float);
extern __softfp long long _ll_sfrom_d(double);
extern __softfp unsigned long long _ll_ufrom_d(double);

/*
 * Floating-to-integer format conversions, rounding to nearest or
 * configurably.
 */
extern __softfp int _ffix_r(float);
extern __softfp unsigned int _ffixu_r(float);
extern __softfp int _dfix_r(double);
extern __softfp unsigned int _dfixu_r(double);
extern __softfp long long _ll_sfrom_f_r(float);
extern __softfp unsigned long long _ll_ufrom_f_r(float);
extern __softfp long long _ll_sfrom_d_r(double);
extern __softfp unsigned long long _ll_ufrom_d_r(double);

/*
 * Call this before using any fplib routines, if you're trying to
 * use fplib on the bare metal.
 */
extern void _fp_init(void);

/*
 * This returns a pointer to the FP status word, when it's stored
 * in memory.
 */
extern unsigned *__rt_fp_status_addr(void);

#ifdef __cplusplus
}
#endif

#endif

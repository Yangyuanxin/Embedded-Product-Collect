/*
 * math.h: ANSI 'C' (X3J11 Oct 88) library header, section 4.5
 * Copyright (C) Codemist Ltd., 1988
 * Copyright 1991-1998 ARM Limited. All rights reserved
 */

/*
 * RCS $Revision: 1.34.2.1 $ Codemist 0.03
 * Checkin $Date: 2001/02/22 16:23:35 $
 * Revising $Author: cadeniyi $
 */


#ifndef __math_h
#define __math_h

  #ifndef __MATH_DECLS
  #define __MATH_DECLS

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


#ifndef HUGE_VAL
#ifndef __STRICT_ANSI__
#define HUGE_VAL (0d_7FF0000000000000)
#else
extern const double __huge_val;
#define HUGE_VAL (__huge_val)
#endif
#endif

#ifndef __STRICT_ANSI__                /* C99 extensions: */
#define INFINITY (0f_7F800000)         /* positive float infinity */
#define NAN (0f_7FC00001)              /* float quiet NaN */
#endif

extern double acos(double /*x*/);
   /* computes the principal value of the arc cosine of x */
   /* a domain error occurs for arguments not in the range -1 to 1 */
   /* Returns: the arc cosine in the range 0 to Pi. */
extern double asin(double /*x*/);
   /* computes the principal value of the arc sine of x */
   /* a domain error occurs for arguments not in the range -1 to 1 */
   /* and -HUGE_VAL is returned. */
   /* Returns: the arc sine in the range -Pi/2 to Pi/2. */

extern __pure double atan(double /*x*/);
   /* computes the principal value of the arc tangent of x */
   /* Returns: the arc tangent in the range -Pi/2 to Pi/2. */

extern double atan2(double /*y*/, double /*x*/);
   /* computes the principal value of the arc tangent of y/x, using the */
   /* signs of both arguments to determine the quadrant of the return value */
   /* a domain error occurs if both args are zero, and -HUGE_VAL returned. */
   /* Returns: the arc tangent of y/x, in the range -Pi to Pi. */


extern __pure double cos(double /*x*/);
   /* computes the cosine of x (measured in radians). A large magnitude */
   /* argument may yield a result with little or no significance */
   /* Returns: the cosine value. */
extern __pure double sin(double /*x*/);
   /* computes the sine of x (measured in radians). A large magnitude */
   /* argument may yield a result with little or no significance */
   /* Returns: the sine value. */

extern void __use_accurate_range_reduction(void);
   /* reference this to select the larger, slower, but more accurate */
   /* range reduction in sin, cos and tan */

extern double tan(double /*x*/);
   /* computes the tangent of x (measured in radians). A large magnitude */
   /* argument may yield a result with little or no significance */
   /* Returns: the tangent value. */
   /*          if range error; returns HUGE_VAL. */

extern double cosh(double /*x*/);
   /* computes the hyperbolic cosine of x. A range error occurs if the */
   /* magnitude of x is too large. */
   /* Returns: the hyperbolic cosine value. */
   /*          if range error; returns HUGE_VAL. */
extern double sinh(double /*x*/);
   /* computes the hyperbolic sine of x. A range error occurs if the */
   /* magnitude of x is too large. */
   /* Returns: the hyperbolic sine value. */
   /*          if range error; returns -HUGE_VAL or HUGE_VAL depending */
   /*          on the sign of the argument */

extern __pure double tanh(double /*x*/);
   /* computes the hyperbolic tangent of x. */
   /* Returns: the hyperbolic tangent value. */

extern double exp(double /*x*/);
   /* computes the exponential function of x. A range error occurs if the */
   /* magnitude of x is too large. */
   /* Returns: the exponential value. */
   /*          if underflow range error; 0 is returned. */
   /*          if overflow range error; HUGE_VAL is returned. */

extern double frexp(double /*value*/, int * /*exp*/);
   /* breaks a floating-point number into a normalised fraction and an */
   /* integral power of 2. It stores the integer in the int object pointed */
   /* to by exp. */
   /* Returns: the value x, such that x is a double with magnitude in the */
   /* interval 0.5 to 1.0 or zero, and value equals x times 2 raised to the */
   /* power *exp. If value is zero, both parts of the result are zero. */

extern double ldexp(double /*x*/, int /*exp*/);
   /* multiplies a floating-point number by an integral power of 2. */
   /* A range error may occur. */
   /* Returns: the value of x times 2 raised to the power of exp. */
   /*          if range error; HUGE_VAL is returned. */
extern double log(double /*x*/);
   /* computes the natural logarithm of x. A domain error occurs if the */
   /* argument is negative, and -HUGE_VAL is returned. A range error occurs */
   /* if the argument is zero. */
   /* Returns: the natural logarithm. */
   /*          if range error; -HUGE_VAL is returned. */
extern double log10(double /*x*/);
   /* computes the base-ten logarithm of x. A domain error occurs if the */
   /* argument is negative. A range error occurs if the argument is zero. */
   /* Returns: the base-ten logarithm. */
extern double modf(double /*value*/, double * /*iptr*/);
   /* breaks the argument value into integral and fraction parts, each of */
   /* which has the same sign as the argument. It stores the integral part */
   /* as a double in the object pointed to by iptr. */
   /* Returns: the signed fractional part of value. */

extern double pow(double /*x*/, double /*y*/);
   /* computes x raised to the power of y. A domain error occurs if x is */
   /* zero and y is less than or equal to zero, or if x is negative and y */
   /* is not an integer, and -HUGE_VAL returned. A range error may occur. */
   /* Returns: the value of x raised to the power of y. */
   /*          if underflow range error; 0 is returned. */
   /*          if overflow range error; HUGE_VAL is returned. */
extern double sqrt(double /*x*/);
   /* computes the non-negative square root of x. A domain error occurs */
   /* if the argument is negative, and -HUGE_VAL returned. */
   /* Returns: the value of the square root. */
#ifdef __TARGET_FPU_VFP
    extern double _sqrt(double);    /* native VFP double precision sqrt */
    extern float _sqrtf(float);     /* native VFP single precision sqrt */
#else
    __inline double _sqrt(double __x) { return sqrt(__x); }
    __inline float _sqrtf(float __x) { return (float)sqrt(__x); }
#endif

extern __pure double ceil(double /*x*/);
   /* computes the smallest integer not less than x. */
   /* Returns: the smallest integer not less than x, expressed as a double. */
extern __pure double fabs(double /*x*/);
   /* computes the absolute value of the floating-point number x. */
   /* Returns: the absolute value of x. */

extern float (_fabsf)(float);
#ifndef __STRICT_ANSI__
   #define fabsf (_fabsf) /* C99 */
#endif

extern __pure double floor(double /*d*/);
   /* computes the largest integer not greater than x. */
   /* Returns: the largest integer not greater than x, expressed as a double */

extern double fmod(double /*x*/, double /*y*/);
   /* computes the floating-point remainder of x/y. */
   /* Returns: the value x - i * y, for some integer i such that, if y is */
   /*          nonzero, the result has the same sign as x and magnitude */
   /*          less than the magnitude of y. If y is zero, a domain error */
   /*          occurs and -HUGE_VAL is returned. */

    /* Additional Mathlib functions not defined by the ANSI standard.
     * Not guaranteed, and not necessarily very well tested.
     * C99 requires the user to include <math.h> to use these functions
     * declaring them "by hand" is not sufficient
     */
#ifndef __STRICT_ANSI__

#if defined __thumb || defined __TARGET_FPU_SOFTVFP || defined __TARGET_FPU_SOFTVFP_VFP || \
                       defined __TARGET_FPU_SOFTFPA || defined __TARGET_FPU_SOFTFPA_FPA
#define _FPMANGLE(name) __softfp_ ## name
#else
#define _FPMANGLE(name) __hardfp_ ## name
#endif

extern double _FPMANGLE(acosh) (double /*x*/);
__inline double acosh(double __x) { return _FPMANGLE(acosh)(__x); }
    /*
     * Inverse cosh. EDOM if argument < 1.0
     */
extern double _FPMANGLE(asinh) (double /*x*/);
__inline double asinh(double __x) { return _FPMANGLE(asinh)(__x); }
    /*
     * Inverse sinh.
     */
extern double _FPMANGLE(atanh) (double /*x*/);
__inline double atanh(double __x) { return _FPMANGLE(atanh)(__x); }
    /*
     * Inverse tanh. EDOM if |argument| > 1.0
     */
extern double _FPMANGLE(cbrt) (double /*x*/);
__inline double cbrt(double __x) { return _FPMANGLE(cbrt)(__x); }
    /*
     * Cube root.
     */
extern double _FPMANGLE(copysign) (double /*x*/, double /*y*/);
__inline double copysign(double __x, double __y) { return _FPMANGLE(copysign)(__x,__y); }
    /*
     * Returns x with sign bit replaced by sign of y.
     */
extern double _FPMANGLE(erf) (double /*x*/);
__inline double erf(double __x) { return _FPMANGLE(erf)(__x); }
    /*
     * Error function. (2/sqrt(pi)) * integral from 0 to x of exp(-t*t) dt.
     */
extern double _FPMANGLE(erfc) (double /*x*/);
__inline double erfc(double __x) { return _FPMANGLE(erfc)(__x); }
    /*
     * 1-erf(x). (More accurate than just coding 1-erf(x), for large x.)
     */
extern double _FPMANGLE(expm1) (double /*x*/);
__inline double expm1(double __x) { return _FPMANGLE(expm1)(__x); }
    /*
     * exp(x)-1. (More accurate than just coding exp(x)-1, for small x.)
     */
extern int _FPMANGLE(finite) (double /*x*/);
__inline double finite(double __x) { return _FPMANGLE(finite)(__x); }
    /*
     * TRUE if argument is finite (ie not infinity or NaN).
     */
extern double _FPMANGLE(gamma) (double /*x*/);
__inline double gamma(double __x) { return _FPMANGLE(gamma)(__x); }
    /*
     * The log of the absolute value of the gamma function of x. The sign
     * of the gamma function of x is returned in the global `signgam'.
     */
extern double _FPMANGLE(gamma_r) (double /*x*/, int * /*signgam*/);
__inline double gamma_r(double __x, int *__p) { return _FPMANGLE(gamma_r)(__x,__p); }
    /*
     * The log of the absolute value of the gamma function of x. The sign
     * of the gamma function of x is returned in the second argument.
     */
extern double _FPMANGLE(hypot) (double /*x*/, double /*y*/);
__inline double hypot(double __x, double __y) { return _FPMANGLE(hypot)(__x,__y); }
    /*
     * sqrt(x*x+y*y), ie the length of the vector (x,y) or the
     * hypotenuse of a right triangle whose other two sides are x
     * and y. Won't overflow unless the _answer_ is too big, even
     * if the intermediate x*x+y*y is too big.
     */
extern int _FPMANGLE(ilogb) (double /*x*/);
__inline int ilogb(double __x) { return _FPMANGLE(ilogb)(__x); }
    /*
     * Exponent of x (returns 0 for 1.0, 1 for 2.0, -1 for 0.5, etc.)
     */
extern int _FPMANGLE(isnan) (double /*x*/);
__inline double isnan(double __x) { return _FPMANGLE(isnan)(__x); }
    /*
     * Returns TRUE if x is a NaN.
     */
extern double _FPMANGLE(j0) (double /*x*/);
__inline double j0(double __x) { return _FPMANGLE(j0)(__x); }
    /*
     * Bessel function of the first kind, order 0. Returns ERANGE
     * if |x| > 2^52 (total loss of significance).
     */
extern double _FPMANGLE(j1) (double /*x*/);
__inline double j1(double __x) { return _FPMANGLE(j1)(__x); }
    /*
     * Bessel function of the first kind, order 1. Returns ERANGE
     * if |x| > 2^52 (total loss of significance).
     */
extern double _FPMANGLE(jn) (int /*n*/, double /*x*/);
__inline double jn(int __n, double __x) { return _FPMANGLE(jn)(__n, __x); }
    /*
     * Bessel function of the first kind, order n. Returns ERANGE
     * if |x| > 2^52 (total loss of significance).
     */
extern double _FPMANGLE(lgamma) (double /*x*/);
__inline double lgamma(double __x) { return _FPMANGLE(lgamma)(__x); }
    /*
     * The log of the absolute value of the gamma function of x. The sign
     * of the gamma function of x is returned in the global `signgam'.
     */
extern double _FPMANGLE(lgamma_r) (double /*x*/, int * /*signgam*/);
__inline double lgamma_r(double __x, int *__p) { return _FPMANGLE(lgamma_r)(__x,__p); }
    /*
     * The log of the absolute value of the gamma function of x. The sign
     * of the gamma function of x is returned in the second argument.
     */
extern double _FPMANGLE(log1p) (double /*x*/);
__inline double log1p(double __x) { return _FPMANGLE(log1p)(__x); }
    /*
     * log(1+x). (More accurate than just coding log(1+x), for small x.)
     */
extern double _FPMANGLE(logb) (double /*x*/);
__inline double logb(double __x) { return _FPMANGLE(logb)(__x); }
    /*
     * Like ilogb but returns a double.
     */
extern double _FPMANGLE(nextafter) (double /*x*/, double /*y*/);
__inline double nextafter(double __x, double __y) { return _FPMANGLE(nextafter)(__x,__y); }
    /*
     * Returns the next representable number after x, in the
     * direction toward y.
     */
extern double _FPMANGLE(remainder) (double /*x*/, double /*y*/);
__inline double remainder(double __x, double __y) { return _FPMANGLE(remainder)(__x,__y); }
    /*
     * Returns the remainder of x by y, in the IEEE 754 sense.
     */
extern double _FPMANGLE(rint) (double /*x*/);
__inline double rint(double __x) { return _FPMANGLE(rint)(__x); }
    /*
     * Rounds x to an integer, in the IEEE 754 sense.
     */
extern double _FPMANGLE(scalb) (double /*x*/, double /*n*/);
__inline double scalb(double __x, double __n) { return _FPMANGLE(scalb)(__x,__n); }
    /*
     * Compute x times 2^n quickly. Undefined if n is not an integer.
     */
extern double _FPMANGLE(scalbn) (double /*x*/, int /*n*/);
__inline double scalbn(double __x, int __n) { return _FPMANGLE(scalbn)(__x,__n); }
    /*
     * Compute x times 2^n quickly.
     */
extern double _FPMANGLE(significand) (double /*x*/);
__inline double significand(double __x) { return _FPMANGLE(significand)(__x); }
    /*
     * Return the fraction part of x, in the range 1.0 to 2.0
     * (including 1.0, excluding 2.0).
     */
extern double _FPMANGLE(y0) (double /*x*/);
__inline double y0(double __x) { return _FPMANGLE(y0)(__x); }
    /*
     * Bessel function of the second kind, order 0. Returns ERANGE
     * if x > 2^52 (total loss of significance).
     */
extern double _FPMANGLE(y1) (double /*x*/);
__inline double y1(double __x) { return _FPMANGLE(y1)(__x); }
    /*
     * Bessel function of the second kind, order 1. Returns ERANGE
     * if x > 2^52 (total loss of significance).
     */
extern double _FPMANGLE(yn) (int /*n*/, double /*x*/);
__inline double yn(int __n, double __x) { return _FPMANGLE(yn)(__n, __x); }
    /*
     * Bessel function of the second kind, order n. Returns ERANGE
     * if x > 2^52 (total loss of significance).
     */
#endif

#ifdef __cplusplus
  extern float __mathlib_tofloat(double); /* internal; used below */
  extern "C++" {
    inline double abs(double __x) 
        { return fabs(__x); }
    inline float abs(float __x)
        { return (_fabsf)(__x); }
    inline float acos(float __x) // acosf
                { return (float)(acos((double) __x)); }
    inline float asin(float __x) // asinf
                { return (float)(asin((double) __x)); }
    inline float atan(float __x) // atanf
                { return (float)(atan((double) __x)); }
    inline float atan2(float __y, float __x)  // atan2f
                { return (float)(atan2((double) __y, (double) __x)); }
    inline float ceil(float __x) // ceilf
                { return (float)(ceil((double) __x)); }
    inline float cos(float __x) // cosf
                { return (float)(cos((double) __x)); }
    inline float cosh(float __x) // coshf
                { return __mathlib_tofloat(cosh((double) __x)); }
    inline float exp(float __x) // expf
                { return __mathlib_tofloat(exp((double) __x)); }
    inline float (fabs)(float __x) // fabsf
                { return (float)(fabs((double) __x)); }
    inline float floor(float __x) // floorf
                { return (float)(floor((double) __x)); }
    inline float fmod(float __x, float __y) //fmodf
                { return (float)(fmod((double) __x, (double) __y)); }
    inline float frexp(float __x, int* __exp) // frexpf
        { return (float)(frexp((double) __x, __exp)); }
    inline float ldexp(float __x, int __exp) // ldexpf
        { return __mathlib_tofloat(ldexp((double) __x, __exp));} 
    inline float log(float __x) // logf
                { return (float)(log((double) __x)); }
    inline float log10(float __x) // log10f
                { return (float)(log10((double) __x)); }
    inline float modf(float __x, float* __iptr) // modff
        { double __d; float __ret;
          __ret = (float)(modf((double) __x, &__d));
          *__iptr = (float)(__d); return __ret; }
    inline float pow(float __x, float __y) // powf
                { return __mathlib_tofloat(pow((double) __x, (double) __y)); }
    inline float pow (float __x, int __y)
                { return __mathlib_tofloat(pow((double) __x, (double) __y)); }
    inline float sin(float __x) //sinf
                { return (float)(sin((double) __x)); }
    inline float sinh(float __x)  //sinhf
                { return __mathlib_tofloat(sinh((double) __x)); }
    inline float sqrt(float __x) // sqrtf
                { return (float)(sqrt((double) __x)); }
    inline float _sqrt(float __x) // _sqrtf
                { return _sqrtf(__x); }
    inline float tan(float __x) // tanf
                { return __mathlib_tofloat(tan((double) __x)); }
    inline float tanh(float __x) // tanhf
                { return (float)(tanh((double) __x)); }

    inline double pow(double __x, int __y) 
                { return pow(__x, (double) __y); }

    inline long double abs(long double __x) // fabsl
                { return (long double)abs((double) __x); }
    inline long double acos(long double __x) // acosl
                { return (long double)acos((double) __x); }
    inline long double asin(long double __x) // asinl
                { return (long double)asin((double) __x); }
    inline long double atan(long double __x) // atanl
                { return (long double)atan((double) __x); }
    inline long double atan2(long double __y, long double __x)  //atan2l
                { return (long double)atan2((double) __y, (double) __x); }
    inline long double ceil(long double __x) // ceill
                { return (long double)ceil((double) __x); }
    inline long double cos(long double __x) // cosl
                { return (long double)cos((double) __x); }
    inline long double cosh(long double __x) // coshl
                { return (long double)cosh((double) __x); }
    inline long double exp(long double __x) // expl
                { return (long double)exp((double) __x); }
    inline long double (fabs)(long double __x) // fabsl
                { return (long double)fabs((double) __x); }
    inline long double floor(long double __x) // floorl
                { return (long double)floor((double) __x); }
    inline long double fmod(long double __x, long double __y) // fmodl
                { return (long double)fmod((double) __x, (double) __y); }
    inline long double frexp(long double __x, int* __p) // frexpl
                { return (long double)frexp((double) __x, __p); }
    inline long double ldexp(long double __x, int __exp) // ldexpl
                { return (long double)ldexp((double) __x, __exp); }
    inline long double log(long double __x)  // logl
                { return (long double)log((double) __x); }
    inline long double log10(long double __x)  // log10l
                { return (long double)log10((double) __x); }
    inline long double modf(long double __x, long double* __p) //modfl
                { return (long double)modf((double) __x, (double *) __p); }
    inline long double pow(long double __x, long double __y) // powl
                { return (long double)pow((double) __x, (double) __y); }
    inline long double pow (long double __x, int __y) 
                { return (long double)pow((double) __x, __y); }
    inline long double sin(long double __x) //sinl
                { return (long double)sin((double) __x); }
    inline long double sinh(long double __x) // sinhl
                { return (long double)sinh((double) __x); }
    inline long double sqrt(long double __x) // sqrtl
                { return (long double)sqrt((double) __x); }
    inline long double _sqrt(long double __x) // _sqrtl
                { return (long double)_sqrt((double) __x); }
    inline long double tan(long double __x) // tanl
                { return (long double)tan((double) __x); }
    inline long double tanh(long double __x) // tanhl
                { return (long double)tanh((double) __x); }
  }
#endif


    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __MATH_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __MATH_NO_EXPORTS
      using std::__mathlib_tofloat;
      using std::__use_accurate_range_reduction;
      using std::abs;
      using std::acos;
      using std::asin;
      using std::atan2;
      using std::atan;
      using std::ceil;
      using std::cos;
      using std::cosh;
      using std::exp;
      using std::fabs;
      using std::floor;
      using std::fmod;
      using std::frexp;
      using std::ldexp;
      using std::log10;
      using std::log;
      using std::modf;
      using std::pow ;
      using std::pow;
      using std::sin;
      using std::sinh;
      using std::sqrt;
      using std::tan;
      using std::tanh;
      using std::_fabsf;
      #ifndef __STRICT_ANSI__
      using std::acosh;
      using std::asinh;
      using std::atanh;
      using std::cbrt;
      using std::copysign;
      using std::erf;
      using std::erfc;
      using std::expm1;
      using std::finite;
      using std::gamma;
      using std::gamma_r;
      using std::hypot;
      using std::ilogb;
      using std::isnan;
      using std::j0;
      using std::j1;
      using std::jn;
      using std::lgamma;
      using std::lgamma_r;
      using std::log1p;
      using std::logb;
      using std::nextafter;
      using std::remainder;
      using std::rint;
      using std::scalb;
      using std::scalbn;
      using std::significand;
      using std::y0;
      using std::y1;
      using std::yn;
      #endif

    #endif 
  #endif 

#endif 

/* end of math.h */

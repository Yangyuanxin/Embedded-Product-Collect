/* stdlib.h: ANSI draft (X3J11 May 88) library header, section 4.10 */
/* Copyright (C) Codemist Ltd., 1988-1993.                          */
/* Copyright 1991-1998 ARM Limited. All rights reserved.            */
/*
 * RCS $Revision: 1.21.2.7 $
 * Checkin $Date: 2001/10/19 15:18:20 $
 * Revising $Author: cadeniyi $
 */
 
/*
 * stdlib.h declares four types, several general purpose functions,
 * and defines several macros.
 */

#ifndef __stdlib_h
#define __stdlib_h

  #ifndef __STDLIB_DECLS
  #define __STDLIB_DECLS

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

#if defined(__cplusplus) || !defined(__STRICT_ANSI__)
 /* unconditional in C++ and non-strict C for consistency of debug info */
  typedef unsigned int size_t;
#elif !defined(__size_t)
  #define __size_t 1
  typedef unsigned int size_t;   /* see <stddef.h> */
#endif

#undef NULL
#define NULL 0                   /* see <stddef.h> */

#ifndef __cplusplus  /* wchar_t is a builtin type for C++ */
  #if !defined(__STRICT_ANSI__)
   /* unconditional in non-strict C for consistency of debug info */
    typedef unsigned short wchar_t; /* see <stddef.h> */
  #elif !defined(__wchar_t)
    #define __wchar_t 1
    typedef unsigned short wchar_t; /* see <stddef.h> */
  #endif
#endif

typedef struct div_t { int quot, rem; } div_t;
   /* type of the value returned by the div function. */
typedef struct ldiv_t { long int quot, rem; } ldiv_t;
   /* type of the value returned by the ldiv function. */
#ifndef __STRICT_ANSI__
typedef struct lldiv_t { long long quot, rem; } lldiv_t;
   /* type of the value returned by the lldiv function. */
#endif

#ifdef __EXIT_FAILURE
#  define EXIT_FAILURE __EXIT_FAILURE
   /*
    * an integral expression which may be used as an argument to the exit
    * function to return unsuccessful termination status to the host
    * environment.
    */
#else
#  define EXIT_FAILURE 1  /* unixoid */
#endif
#define EXIT_SUCCESS 0
   /*
    * an integral expression which may be used as an argument to the exit
    * function to return successful termination status to the host
    * environment.
    */

   /*
    * Defining __USE_ANSI_EXAMPLE_RAND at compile time switches to
    * the example implementation of rand() and srand() provided in
    * the ANSI C standard. This implementation is very poor, but is
    * provided for completeness.
    */
#ifdef __USE_ANSI_EXAMPLE_RAND
#define srand _ANSI_srand
#define rand _ANSI_rand
#define RAND_MAX 0x7fff
#else
#define RAND_MAX 0x7fffffff
#endif
   /*
    * RAND_MAX: an integral constant expression, the value of which
    * is the maximum value returned by the rand function.
    */
#define MB_CUR_MAX 1
   /*
    * a positive integer expression whose value is the maximum number of bytes
    * in a multibyte character for the extended character set specified by the
    * current locale (category LC_CTYPE), and whose value is never greater
    * than MB_LEN_MAX.
    */

extern double atof(const char * /*nptr*/);
   /*
    * converts the initial part of the string pointed to by nptr to double
    * representation.
    * Returns: the converted value.
    */
extern int atoi(const char * /*nptr*/);
   /*
    * converts the initial part of the string pointed to by nptr to int
    * representation.
    * Returns: the converted value.
    */
extern long int atol(const char * /*nptr*/);
   /*
    * converts the initial part of the string pointed to by nptr to long int
    * representation.
    * Returns: the converted value.
    */
#ifndef __STRICT_ANSI__
extern long long int atoll(const char * /*nptr*/);
   /*
    * converts the initial part of the string pointed to by nptr to
    * long long int representation.
    * Returns: the converted value.
    */
#endif

extern double strtod(const char * /*nptr*/, char ** /*endptr*/);
   /*
    * converts the initial part of the string pointed to by nptr to double
    * representation. First it decomposes the input string into three parts:
    * an initial, possibly empty, sequence of white-space characters (as
    * specified by the isspace function), a subject sequence resembling a
    * floating point constant; and a final string of one or more unrecognised
    * characters, including the terminating null character of the input string.
    * Then it attempts to convert the subject sequence to a floating point
    * number, and returns the result. A pointer to the final string is stored
    * in the object pointed to by endptr, provided that endptr is not a null
    * pointer.
    * Returns: the converted value if any. If no conversion could be performed,
    *          zero is returned. If the correct value is outside the range of
    *          representable values, plus or minus HUGE_VAL is returned
    *          (according to the sign of the value), and the value of the macro
    *          ERANGE is stored in errno. If the correct value would cause
    *          underflow, zero is returned and the value of the macro ERANGE is
    *          stored in errno.
    */
extern long int strtol(const char * /*nptr*/, char **/*endptr*/, int /*base*/);
   /*
    * converts the initial part of the string pointed to by nptr to long int
    * representation. First it decomposes the input string into three parts:
    * an initial, possibly empty, sequence of white-space characters (as
    * specified by the isspace function), a subject sequence resembling an
    * integer represented in some radix determined by the value of base, and a
    * final string of one or more unrecognised characters, including the
    * terminating null character of the input string. Then it attempts to
    * convert the subject sequence to an integer, and returns the result.
    * If the value of base is 0, the expected form of the subject sequence is
    * that of an integer constant (described in ANSI Draft, section 3.1.3.2),
    * optionally preceded by a '+' or '-' sign, but not including an integer
    * suffix. If the value of base is between 2 and 36, the expected form of
    * the subject sequence is a sequence of letters and digits representing an
    * integer with the radix specified by base, optionally preceded by a plus
    * or minus sign, but not including an integer suffix. The letters from a
    * (or A) through z (or Z) are ascribed the values 10 to 35; only letters
    * whose ascribed values are less than that of the base are permitted. If
    * the value of base is 16, the characters 0x or 0X may optionally precede
    * the sequence of letters and digits following the sign if present.
    * A pointer to the final string is stored in the object
    * pointed to by endptr, provided that endptr is not a null pointer.
    * Returns: the converted value if any. If no conversion could be performed,
    *          zero is returned and nptr is stored in *endptr.
    *          If the correct value is outside the range of
    *          representable values, LONG_MAX or LONG_MIN is returned
    *          (according to the sign of the value), and the value of the
    *          macro ERANGE is stored in errno.
    */
extern unsigned long int strtoul(const char * /*nptr*/,
                                       char ** /*endptr*/, int /*base*/);
   /*
    * converts the initial part of the string pointed to by nptr to unsigned
    * long int representation. First it decomposes the input string into three
    * parts: an initial, possibly empty, sequence of white-space characters (as
    * determined by the isspace function), a subject sequence resembling an
    * unsigned integer represented in some radix determined by the value of
    * base, and a final string of one or more unrecognised characters,
    * including the terminating null character of the input string. Then it
    * attempts to convert the subject sequence to an unsigned integer, and
    * returns the result. If the value of base is zero, the expected form of
    * the subject sequence is that of an integer constant (described in ANSI
    * Draft, section 3.1.3.2), optionally preceded by a '+' or '-' sign, but
    * not including an integer suffix. If the value of base is between 2 and
    * 36, the expected form of the subject sequence is a sequence of letters
    * and digits representing an integer with the radix specified by base,
    * optionally preceded by a '+' or '-' sign, but not including an integer
    * suffix. The letters from a (or A) through z (or Z) stand for the values
    * 10 to 35; only letters whose ascribed values are less than that of the
    * base are permitted. If the value of base is 16, the characters 0x or 0X
    * may optionally precede the sequence of letters and digits following the
    * sign, if present. A pointer to the final string is stored in the object
    * pointed to by endptr, provided that endptr is not a null pointer.
    * Returns: the converted value if any. If no conversion could be performed,
    *          zero is returned and nptr is stored in *endptr.
    *          If the correct value is outside the range of
    *          representable values, ULONG_MAX is returned, and the value of
    *          the macro ERANGE is stored in errno.
    */

#ifndef __STRICT_ANSI__
extern long long int strtoll(const char * /*restrict*/ /*nptr*/,
                             char ** /*restrict*/ /*endptr*/, int /*base*/);
   /*
    * as strtol but returns a long long int value.  If the correct value is
    * outside the range of representable values,  LLONG_MAX or LLONG_MIN is
    * returned (according to the sign of the value), and the value of the
    * macro ERANGE is stored in errno.
    */
extern unsigned long long int strtoull(const char * /*restrict*/ /*nptr*/,
                                       char ** /*restrict*/ /*endptr*/, int /*base*/);
   /*
    * as strtoul but returns an unsigned long long int value.  If the correct
    * value is outside the range of representable values, ULLONG_MAX is returned,
    * and the value of the macro ERANGE is stored in errno.
    */
#endif

extern int rand(void);
   /*
    * Computes a sequence of pseudo-random integers in the range 0 to RAND_MAX.
    * Uses an additive generator (Mitchell & Moore) of the form:
    *   Xn = (X[n-24] + X[n-55]) MOD 2^31
    * This is described in section 3.2.2 of Knuth, vol 2. It's period is
    * in excess of 2^55 and its randomness properties, though unproven, are
    * conjectured to be good. Empirical testing since 1958 has shown no flaws.
    * Returns: a pseudo-random integer.
    */
extern void srand(unsigned int /*seed*/);
   /*
    * uses its argument as a seed for a new sequence of pseudo-random numbers
    * to be returned by subsequent calls to rand. If srand is then called with
    * the same seed value, the sequence of pseudo-random numbers is repeated.
    * If rand is called before any calls to srand have been made, the same
    * sequence is generated as when srand is first called with a seed value
    * of 1.
    */

extern void *calloc(size_t /*nmemb*/, size_t /*size*/);
   /*
    * allocates space for an array of nmemb objects, each of whose size is
    * 'size'. The space is initialised to all bits zero.
    * Returns: either a null pointer or a pointer to the allocated space.
    */
extern void free(void * /*ptr*/);
   /*
    * causes the space pointed to by ptr to be deallocated (i.e., made
    * available for further allocation). If ptr is a null pointer, no action
    * occurs. Otherwise, if ptr does not match a pointer earlier returned by
    * calloc, malloc or realloc or if the space has been deallocated by a call
    * to free or realloc, the behaviour is undefined.
    */
extern void *malloc(size_t /*size*/);
   /*
    * allocates space for an object whose size is specified by 'size' and whose
    * value is indeterminate.
    * Returns: either a null pointer or a pointer to the allocated space.
    */
extern void *realloc(void * /*ptr*/, size_t /*size*/);
   /*
    * changes the size of the object pointed to by ptr to the size specified by
    * size. The contents of the object shall be unchanged up to the lesser of
    * the new and old sizes. If the new size is larger, the value of the newly
    * allocated portion of the object is indeterminate. If ptr is a null
    * pointer, the realloc function behaves like a call to malloc for the
    * specified size. Otherwise, if ptr does not match a pointer earlier
    * returned by calloc, malloc or realloc, or if the space has been
    * deallocated by a call to free or realloc, the behaviour is undefined.
    * If the space cannot be allocated, the object pointed to by ptr is
    * unchanged. If size is zero and ptr is not a null pointer, the object it
    * points to is freed.
    * Returns: either a null pointer or a pointer to the possibly moved
    *          allocated space.
    */
typedef int (*__heapprt)(void *, char const *, ...);
extern void __heapstats(int (* /*dprint*/)(void * /*param*/,
                                           char const * /*format*/, ...),
                        void * /*param*/);
   /*
    * reports current heap statistics (eg. number of free blocks in
    * the free-list). Output is as implementation-defined free-form
    * text, provided via the dprint function. `param' gives an
    * extra data word to pass to dprint. You can call
    * __heapstats(fprintf,stdout) by casting fprintf to the above
    * function type; the typedef `__heapprt' is provided for this
    * purpose.
    *
    * `dprint' will not be called while the heap is being examined,
    * so it can allocate memory itself without trouble.
    */
extern int __heapvalid(int (* /*dprint*/)(void * /*param*/,
                                           char const * /*format*/, ...),
                       void * /*param*/, int /*verbose*/);
   /*
    * performs a consistency check on the heap. Errors are reported
    * through dprint, like __heapstats. If `verbose' is nonzero,
    * full diagnostic information on the heap state is printed out.
    *
    * This routine probably won't work if the heap isn't a
    * contiguous chunk (for example, if __user_heap_extend has been
    * overridden).
    *
    * `dprint' may be called while the heap is being examined or
    * even in an invalid state, so it must perform no memory
    * allocation. In particular, if `dprint' calls (or is) a stdio
    * function, the stream it outputs to must already have either
    * been written to or been setvbuf'ed, or else the system will
    * allocate buffer space for it on the first call to dprint.
    */
extern void abort(void);
   /*
    * causes abnormal program termination to occur, unless the signal SIGABRT
    * is being caught and the signal handler does not return. Whether open
    * output streams are flushed or open streams are closed or temporary
    * files removed is implementation-defined.
    * An implementation-defined form of the status 'unsuccessful termination'
    * is returned to the host environment by means of a call to
    * raise(SIGABRT).
    */
extern int atexit(void (* /*func*/)(void));
   /*
    * registers the function pointed to by func, to be called without its
    * arguments at normal program termination. It is possible to register at
    * least 32 functions.
    * Returns: zero if the registration succeeds, nonzero if it fails.
    */
extern void exit(int /*status*/);
   /*
    * causes normal program termination to occur. If more than one call to the
    * exit function is executed by a program, the behaviour is undefined.
    * First, all functions registered by the atexit function are called, in the
    * reverse order of their registration.
    * Next, all open output streams are flushed, all open streams are closed,
    * and all files created by the tmpfile function are removed.
    * Finally, control is returned to the host environment. If the value of
    * status is zero or EXIT_SUCCESS, an implementation-defined form of the
    * status 'successful termination' is returned. If the value of status is
    * EXIT_FAILURE, an implementation-defined form of the status
    * 'unsuccessful termination' is returned. Otherwise the status returned
    * is implementation-defined.
    */

extern char *getenv(const char * /*name*/);
   /*
    * searches the environment list, provided by the host environment, for a
    * string that matches the string pointed to by name. The set of environment
    * names and the method for altering the environment list are
    * implementation-defined.
    * Returns: a pointer to a string associated with the matched list member.
    *          The array pointed to shall not be modified by the program, but
    *          may be overwritten by a subsequent call to the getenv function.
    *          If the specified name cannot be found, a null pointer is
    *          returned.
    */
extern int  system(const char * /*string*/);
   /*
    * passes the string pointed to by string to the host environment to be
    * executed by a command processor in an implementation-defined manner.
    * A null pointer may be used for string, to inquire whether a command
    * processor exists.
    *
    * Returns: If the argument is a null pointer, the system function returns
    *          non-zero only if a command processor is available. If the
    *          argument is not a null pointer, the system function returns an
    *          implementation-defined value.
    */

extern void *bsearch(const void * /*key*/, const void * /*base*/,
              size_t /*nmemb*/, size_t /*size*/,
              int (* /*compar*/)(const void *, const void *));
   /*
    * searches an array of nmemb objects, the initial member of which is
    * pointed to by base, for a member that matches the object pointed to by
    * key. The size of each member of the array is specified by size.
    * The contents of the array shall be in ascending sorted order according to
    * a comparison function pointed to by compar, which is called with two
    * arguments that point to the key object and to an array member, in that
    * order. The function shall return an integer less than, equal to, or
    * greater than zero if the key object is considered, respectively, to be
    * less than, to match, or to be greater than the array member.
    * Returns: a pointer to a matching member of the array, or a null pointer
    *          if no match is found. If two members compare as equal, which
    *          member is matched is unspecified.
    */
extern void qsort(void * /*base*/, size_t /*nmemb*/, size_t /*size*/,
           int (* /*compar*/)(const void *, const void *));
   /*
    * sorts an array of nmemb objects, the initial member of which is pointed
    * to by base. The size of each object is specified by size.
    * The contents of the array shall be in ascending order according to a
    * comparison function pointed to by compar, which is called with two
    * arguments that point to the objects being compared. The function shall
    * return an integer less than, equal to, or greater than zero if the first
    * argument is considered to be respectively less than, equal to, or greater
    * than the second. If two members compare as equal, their order in the
    * sorted array is unspecified.
    */

extern int abs(int /*j*/);
   /*
    * computes the absolute value of an integer j. If the result cannot be
    * represented, the behaviour is undefined.
    * Returns: the absolute value.
    */

extern div_t div(int /*numer*/, int /*denom*/);
   /*
    * computes the quotient and remainder of the division of the numerator
    * numer by the denominator denom. If the division is inexact, the resulting
    * quotient is the integer of lesser magnitude that is the nearest to the
    * algebraic quotient. If the result cannot be represented, the behaviour is
    * undefined; otherwise, quot * denom + rem shall equal numer.
    * Returns: a structure of type div_t, comprising both the quotient and the
    *          remainder. the structure shall contain the following members,
    *          in either order.
    *          int quot; int rem;
    */
extern long int labs(long int /*j*/);
   /*
    * computes the absolute value of an long integer j. If the result cannot be
    * represented, the behaviour is undefined.
    * Returns: the absolute value.
    */
#ifdef __cplusplus
   extern "C++" inline long abs(long int x) { return labs(x); }
#endif

extern ldiv_t ldiv(long int /*numer*/, long int /*denom*/);
   /*
    * computes the quotient and remainder of the division of the numerator
    * numer by the denominator denom. If the division is inexact, the sign of
    * the resulting quotient is that of the algebraic quotient, and the
    * magnitude of the resulting quotient is the largest integer less than the
    * magnitude of the algebraic quotient. If the result cannot be represented,
    * the behaviour is undefined; otherwise, quot * denom + rem shall equal
    * numer.
    * Returns: a structure of type ldiv_t, comprising both the quotient and the
    *          remainder. the structure shall contain the following members,
    *          in either order.
    *          long int quot; long int rem;
    */
#ifdef __cplusplus
   extern "C++" inline ldiv_t div(long int __numer, long int __denom) {
       return ldiv(__numer, __denom);
   }
#endif

#ifndef __STRICT_ANSI__
extern long long llabs(long long /*j*/);
   /*
    * computes the absolute value of a long long integer j. If the
    * result cannot be represented, the behaviour is undefined.
    * Returns: the absolute value.
    */
#ifdef __cplusplus
   extern "C++" inline long long abs(long long x) { return llabs(x); }
#endif

extern lldiv_t lldiv(long long /*numer*/, long long /*denom*/);
   /*
    * computes the quotient and remainder of the division of the numerator
    * numer by the denominator denom. If the division is inexact, the sign of
    * the resulting quotient is that of the algebraic quotient, and the
    * magnitude of the resulting quotient is the largest integer less than the
    * magnitude of the algebraic quotient. If the result cannot be represented,
    * the behaviour is undefined; otherwise, quot * denom + rem shall equal
    * numer.
    * Returns: a structure of type lldiv_t, comprising both the quotient and the
    *          remainder. the structure shall contain the following members,
    *          in either order.
    *          long long quot; long long rem;
    */
#ifdef __cplusplus
   extern "C++" inline lldiv_t div(long long __numer, long long __denom) {
       return lldiv(__numer, __denom);
   }
#endif
#endif

/*
 * ARM real-time divide functions for guaranteed performance
 */
typedef struct __sdiv32by16 { int quot, rem; } __sdiv32by16;
typedef struct __udiv32by16 { unsigned int quot, rem; } __udiv32by16;
   /* used int so that values return in separate regs, although 16-bit */
typedef struct __sdiv64by32 { int rem, quot; } __sdiv64by32;

__value_in_regs extern __sdiv32by16 __rt_sdiv32by16(
     int /*numer*/,
     short int /*denom*/);
   /*
    * Signed divide: (16-bit quot), (16-bit rem) = (32-bit) / (16-bit)
    */
__value_in_regs extern __udiv32by16 __rt_udiv32by16(
     unsigned int /*numer*/,
     unsigned short /*denom*/);
   /*
    * Unsigned divide: (16-bit quot), (16-bit rem) = (32-bit) / (16-bit)
    */
__value_in_regs extern __sdiv64by32 __rt_sdiv64by32(
     int /*numer_h*/, unsigned int /*numer_l*/,
     int /*denom*/);
   /*
    * Signed divide: (32-bit quot), (32-bit rem) = (64-bit) / (32-bit)
    */

/*
 * ARM floating-point mask/status function (for both hardfp and softfp)
 */
extern unsigned int __fp_status(unsigned int /*mask*/, unsigned int /*flags*/);
   /*
    * mask and flags are bit-fields which correspond directly to the
    * floating point status register in the FPE/FPA and fplib.  
    * __fp_status returns the current value of the status register,
    * and also sets the writable bits of the word
    * (the exception control and flag bytes) to:
    *
    *     new = (old & ~mask) ^ flags;
    */
#define __fpsr_IXE  0x100000
#define __fpsr_UFE  0x80000
#define __fpsr_OFE  0x40000
#define __fpsr_DZE  0x20000
#define __fpsr_IOE  0x10000

#define __fpsr_IXC  0x10
#define __fpsr_UFC  0x8
#define __fpsr_OFC  0x4
#define __fpsr_DZC  0x2
#define __fpsr_IOC  0x1

/*
 * Multibyte Character Functions.
 * The behaviour of the multibyte character functions is affected by the
 * LC_CTYPE category of the current locale. For a state-dependent encoding,
 * each function is placed into its initial state by a call for which its
 * character pointer argument, s, is a null pointer. Subsequent calls with s
 * as other than a null pointer cause the internal state of the function to be
 * altered as necessary. A call with s as a null pointer causes these functions
 * to return a nonzero value if encodings have state dependency, and a zero
 * otherwise. After the LC_CTYPE category is changed, the shift state of these
 * functions is indeterminate.
 */
extern int mblen(const char * /*s*/, size_t /*n*/);
   /*
    * If s is not a null pointer, the mblen function determines the number of
    * bytes compromising the multibyte character pointed to by s. Except that
    * the shift state of the mbtowc function is not affected, it is equivalent
    * to   mbtowc((wchar_t *)0, s, n);
    * Returns: If s is a null pointer, the mblen function returns a nonzero or
    *          zero value, if multibyte character encodings, respectively, do
    *          or do not have state-dependent encodings. If s is not a null
    *          pointer, the mblen function either returns a 0 (if s points to a
    *          null character), or returns the number of bytes that compromise
    *          the multibyte character (if the next n of fewer bytes form a
    *          valid multibyte character), or returns -1 (they do not form a
    *          valid multibyte character).
    */
extern int mbtowc(wchar_t * /*pwc*/, const char * /*s*/, size_t /*n*/);
   /*
    * If s is not a null pointer, the mbtowc function determines the number of
    * bytes that compromise the multibyte character pointed to by s. It then
    * determines the code for value of type wchar_t that corresponds to that
    * multibyte character. (The value of the code corresponding to the null
    * character is zero). If the multibyte character is valid and pwc is not a
    * null pointer, the mbtowc function stores the code in the object pointed
    * to by pwc. At most n bytes of the array pointed to by s will be examined.
    * Returns: If s is a null pointer, the mbtowc function returns a nonzero or
    *          zero value, if multibyte character encodings, respectively, do
    *          or do not have state-dependent encodings. If s is not a null
    *          pointer, the mbtowc function either returns a 0 (if s points to
    *          a null character), or returns the number of bytes that
    *          compromise the converted multibyte character (if the next n of
    *          fewer bytes form a valid multibyte character), or returns -1
    *          (they do not form a valid multibyte character).
    */
extern int wctomb(char * /*s*/, wchar_t /*wchar*/);
   /*
    * determines the number of bytes need to represent the multibyte character
    * corresponding to the code whose value is wchar (including any change in
    * shift state). It stores the multibyte character representation in the
    * array object pointed to by s (if s is not a null pointer). At most
    * MB_CUR_MAX characters are stored. If the value of wchar is zero, the
    * wctomb function is left in the initial shift state).
    * Returns: If s is a null pointer, the wctomb function returns a nonzero or
    *          zero value, if multibyte character encodings, respectively, do
    *          or do not have state-dependent encodings. If s is not a null
    *          pointer, the wctomb function returns a -1 if the value of wchar
    *          does not correspond to a valid multibyte character, or returns
    *          the number of bytes that compromise the multibyte character
    *          corresponding to the value of wchar.
    */

/*
 * Multibyte String Functions.
 * The behaviour of the multibyte string functions is affected by the LC_CTYPE
 * category of the current locale.
 */
extern size_t mbstowcs(wchar_t * /*pwcs*/, const char * /*s*/, size_t /*n*/);
   /*
    * converts a sequence of multibyte character that begins in the initial
    * shift state from the array pointed to by s into a sequence of
    * corresponding codes and stores not more than n codes into the array
    * pointed to by pwcs. No multibyte character that follow a null character
    * (which is converted into a code with value zero) will be examined or
    * converted. Each multibyte character is converted as if by a call to
    * mbtowc function, except that the shift state of the mbtowc function is
    * not affected. No more than n elements will be modified in the array
    * pointed to by pwcs. If copying takes place between objects that overlap,
    * the behaviour is undefined.
    * Returns: If an invalid multibyte character is encountered, the mbstowcs
    *          function returns (size_t)-1. Otherwise, the mbstowcs function
    *          returns the number of array elements modified, not including
    *          a terminating zero code, if any.
    */
extern size_t wcstombs(char * /*s*/, const wchar_t * /*pwcs*/, size_t /*n*/);
   /*
    * converts a sequence of codes that correspond to multibyte characters
    * from the array pointed to by pwcs into a sequence of multibyte
    * characters that begins in the initial shift state and stores these
    * multibyte characters into the array pointed to by s, stopping if a
    * multibyte character would exceed the limit of n total bytes or if a
    * null character is stored. Each code is converted as if by a call to the
    * wctomb function, except that the shift state of the wctomb function is
    * not affected. No more than n elements will be modified in the array
    * pointed to by s. If copying takes place between objects that overlap,
    * the behaviour is undefined.
    * Returns: If a code is encountered that does not correspond to a valid
    *          multibyte character, the wcstombs function returns (size_t)-1.
    *          Otherwise, the wcstombs function returns the number of bytes
    *          modified, not including a terminating null character, if any.
    */

extern void __use_realtime_heap(void);
extern void __use_realtime_division(void);
extern void __use_two_region_memory(void);
extern void __use_no_heap(void);
extern void __use_no_heap_region(void);

extern char const *__C_library_version_string(void);
extern int __C_library_version_number(void);

    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __STDLIB_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __STDLIB_NO_EXPORTS
      #ifndef __STRICT_ANSI__
        using std::atoll;       
        using std::strtoll;
        using std::strtoull;
        using std::lldiv_t;
      #endif
      using std::div_t;
      using std::ldiv_t;
      using std::atof;
      using std::atoi;
      using std::atol;
      using std::strtod;
      using std::strtol;
      using std::strtoul;
      using std::rand;
      using std::srand;
      using std::calloc;
      using std::free;
      using std::malloc;
      using std::realloc;
      using std::__heapprt;
      using std::__heapstats;
      using std::__heapvalid;
      using std::abort;
      using std::atexit;
      using std::exit;
      using std::getenv;
      using std::system;
      using std::bsearch;
      using std::qsort;
      using std::abs;
      using std::div;
      using std::labs;
      using std::ldiv;
      #ifndef __STRICT_ANSI__
        using std::llabs;
        using std::lldiv;
      #endif
      using std::__sdiv32by16;
      using std::__udiv32by16;
      using std::__sdiv64by32;
      using std::__rt_sdiv32by16;
      using std::__rt_udiv32by16;
      using std::__rt_sdiv64by32;
      using std::__fp_status;
      using std::mblen;
      using std::mbtowc;
      using std::wctomb;
      using std::mbstowcs;
      using std::wcstombs;
      using std::__use_realtime_heap;
      using std::__use_two_region_memory;
      using std::__C_library_version_string;
      using std::__C_library_version_number;
      using std::size_t;
    #endif
  #endif
#endif
/* end of stdlib.h */

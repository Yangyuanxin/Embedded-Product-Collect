/* signal.h
 *
 * Copyright (C) Codemist Ltd., 1988-1993.
 * Copyright 1991-1999 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.9.2.1 $
 * Checkin $Date: 2001/02/22 16:23:35 $
 * Revising $Author: cadeniyi $
 */

/*
 * signal.h declares a type and two functions and defines several macros, for
 * handling various signals (conditions that may be reported during program
 * execution).
 */

#ifndef __signal_h
#define __signal_h

  #ifndef __SIGNAL_DECLS
  #define __SIGNAL_DECLS

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


typedef int sig_atomic_t;
   /* type which is the integral type of an object that can be modified as */
   /* an atomic entity, even in the presence of asynchronous interrupts. */

extern void __SIG_DFL(int);
extern void __SIG_ERR(int);
extern void __SIG_IGN(int);
   /*
    * Each of the following macros expand to a constant expression with a
    * distinct value and has the same type as the second argument to, and the
    * return value of the signal function, and whose value compares unequal to
    * the address of any declarable function.
    */
#define SIG_DFL &__CLIBNS __SIG_DFL
#define SIG_ERR &__CLIBNS __SIG_ERR
#define SIG_IGN &__CLIBNS __SIG_IGN

   /*
    * Each of the following macros expand to a positive integral constant
    * expression that is the signal number corresponding the the specified
    * condition.
    */
#define SIGABRT 1   /* abort                         */
#define SIGFPE  2   /* arithmetic exception          */
#define SIGILL  3   /* illegal instruction           */
#define SIGINT  4   /* attention request from user   */
#define SIGSEGV 5   /* bad memory access             */
#define SIGTERM 6   /* termination request           */
   /* (these following macros are not part of the ANSI standard,
    * but private to this implementation)
    */
#define SIGSTAK 7   /* stack overflow                */
#define SIGRTRED 8  /* run-time redirection error    */
#define SIGRTMEM 9  /* run-time memory error         */
/* Signal numbers 10 and 11 are available for the user */
#define SIGUSR1 10
#define SIGUSR2 11
#define SIGPVFN 12 /* pure virtual function called   */
#define SIGCPPL 13 /* exception from C++ library     */
/* Signal numbers 14-31 are reserved to the implementation */
/* Signal numbers 32 and larger are for more user signals */

extern void (*signal (int /*sig*/, void (* /*func*/ )(int)))(int);
   /*
    * Chooses one of three ways in which receipt of the signal number sig is to
    * be subsequently handled. If the value of func is SIG_DFL, default
    * handling for that signal will occur. If the value of func is SIG_IGN, the
    * signal will be ignored. Otherwise func shall point to a function to be
    * called when that signal occurs.
    * When a signal occurs, if func points to a function, first the
    * equivalent of signal(sig, SIG_DFL); is executed. (If the value of sig
    * is SIGILL, whether the reset to SIG_DFL occurs is implementation-
    * defined). Next the equivalent of (*func)(sig); is executed. The
    * function may terminate by calling the abort, exit or longjmp function.
    * If func executes a return statement and the value of sig was SIGFPE or
    * any other implementation-defined value corresponding to a computational
    * exception, the behaviour is undefined. Otherwise, the program will
    * resume execution at the point it was interrupted.
    * If the signal occurs other than as a result of calling the abort or raise
    * function, the behaviour is undefined if the signal handler calls any
    * function in the standard library other than the signal function itself
    * or refers to any object with static storage duration other than by
    * assigning a value to a volatile static variable of type sig_atomic_t.
    * At program startup, the equivalent of signal(sig, SIG_IGN); may be
    * executed for some signals selected in an implementation-defined manner;
    * the equivalent of signal(sig, SIG_DFL); is executed for all other
    * signals defined by the implementation.
    * Returns: If the request can be honoured, the signal function returns the
    *          value of func for most recent call to signal for the specified
    *          signal sig. Otherwise, a value of SIG_ERR is returned and the
    *          integer expression errno is set to indicate the error.
    */

extern int raise(int /*sig*/);
   /* sends the signal sig to the executing program. */
   /* Returns: zero if successful, non-zero if unsuccessful. */

#ifndef __STRICT_ANSI__
#define DIVBYZERO 2                    /* SIGFPE param for integer div-by-0 */
#endif

    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __SIGNAL_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __SIGNAL_NO_EXPORTS
      using std::sig_atomic_t;
      using std::__SIG_DFL;
      using std::__SIG_ERR;
      using std::__SIG_IGN;
      using std::raise;
      using std::signal;
    #endif 
  #endif 

#endif

/* end of signal.h */

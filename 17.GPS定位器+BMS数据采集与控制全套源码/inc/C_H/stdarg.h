/* stdarg.h: ANSI 'C' (X3J11 Oct 88) library header, section 4.8 */
/* Copyright (C) Codemist Ltd., 1988                            */
/* Copyright (C) ARM Ltd., 1991-1999. All rights reserved */

/*
 * RCS $Revision: 1.9.2.1 $
 * Checkin $Date: 2001/02/22 16:23:35 $
 * Revising $Author: cadeniyi $
 */

#ifndef __stdarg_h
#define __stdarg_h

  #ifndef __STDARG_DECLS
  #define __STDARG_DECLS

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

/*
 * stdarg.h declares a type and defines macros for advancing through a
 * list of arguments whose number and types are not known to the called
 * function when it is translated. A function may be called with a variable
 * number of arguments of differing types. Its parameter list contains one or
 * more parameters. The rightmost parameter plays a special role in the access
 * mechanism, and will be called parmN in this description.
 */

/* N.B. <stdio.h> is required to declare vfprintf() without defining      */
/* va_list.  Clearly the type __va_list there must keep in step.          */
typedef int *va_list[1];
   /*
    * an array type suitable for holding information needed by the macro va_arg
    * and the function va_end. The called function shall declare a variable
    * (referred to as ap) having type va_list. The variable ap may be passed as
    * an argument to another function.
    * Note: va_list is an array type so that when an object of that type
    * is passed as an argument it gets passed by reference.
    */

#define va_start(ap, parmN) (void)(*(ap) = __va_start(parmN))
   /*
    * The va_start macro shall be executed before any access to the unnamed
    * arguments. The parameter ap points to an object that has type va_list.
    * The va_start macro initialises ap for subsequent use by va_arg and
    * va_end. The parameter parmN is the identifier of the rightmost parameter
    * in the variable parameter list in the function definition (the one just
    * before the '...'). If the parameter parmN is declared with the register
    * storage class an error is given.
    * If parmN is a narrow type (char, short, float) an error is given in
    * strict ANSI mode, or a warning otherwise.
    * Returns: no value.
    */

#define va_arg(ap, type) __va_arg(*(ap), type)
   /*
    * The va_arg macro expands to an expression that has the type and value of
    * the next argument in the call. The parameter ap shall be the same as the
    * va_list ap initialised by va_start. Each invocation of va_arg modifies
    * ap so that successive arguments are returned in turn. The parameter
    * 'type' is a type name such that the type of a pointer to an object that
    * has the specified type can be obtained simply by postfixing a * to
    * 'type'. If type is a narrow type, an error is given in strict ANSI
    * mode, or a warning otherwise. If the type is an array or function type,
    * an error is given.
    * In non-strict ANSI mode, 'type' is allowed to be any expression.
    * Returns: The first invocation of the va_arg macro after that of the
    *          va_start macro returns the value of the argument after that
    *          specified by parmN. Successive invocations return the values of
    *          the remaining arguments in succession. 
    *          The result is cast to 'type', even if 'type' is narrow.
    */

#ifndef __STRICT_ANSI__
#define va_copy(dest, src) ((void)(*(dest) = *(src)))
   /* The va_copy macro makes the va_list dest be a copy of
    * the va_list src, as if the va_start macro had been applied
    * to it followed by the same sequence of uses of the va_arg
    * macro as had previously been used to reach the present state
    * of src.
    */
#endif


#define va_end(ap) ((void)(*(ap) = 0))
   /*
    * The va_end macro facilitates a normal return from the function whose
    * variable argument list was referenced by the expansion of va_start that
    * initialised the va_list ap. If the va_end macro is not invoked before
    * the return, the behaviour is undefined.
    * Returns: no value.
    */

    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __STDARG_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __STDARG_NO_EXPORTS
      using std::va_list;
    #endif 
  #endif 

#endif

/* end of stdarg.h */

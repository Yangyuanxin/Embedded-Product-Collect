#ifndef __STD_RWCOMPILER_H__
#define __STD_RWCOMPILER_H__ 1

/***************************************************************************
 *
 * Compiler and system related foibles and directives
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 **************************************************************************/

/****************************************************************
 ****************************************************************
 *								*
 *		U S E R   T U N A B L E   S E C T I O N		*
 *								*
 ****************************************************************
 ****************************************************************/


// Library version number
// 
#define _RWSTD_VER 0x020101
//                   AABBCC
//                    | | |
//                    | | +-- CC = Maintenance number
//                    | +---- BB = Minor Release number
//                    +------ AA = Major Release number
//
// Note that former version numbers were of the form: 0xAABC
// i.e., they showed only one digit each for Minor and Maintenance.
// e.g.  "0x0203" would be "0x020003" in the new, extended scheme.

/*
 * This section has various preprocessor constants that can
 * be set to reflect the properties of your compiler.  For most
 * compilers (particularly, MS-DOS compilers) there is no need
 * to do anything --- most settings can be autodetected.
 *
 * For many Unix compilers you may have to tune the settings below.
 * This is most easily done by running the "config" shell script
 * which will try various test programs to discover the properties
 * of your compiler.
 *
 *       THIS IS FAR EASIER THAN SETTING THESE BY HAND!
 */

/*
 *                   AT&T "CFRONT" USERS
 */


/* 
 * Most compilers have a built in "manifest constant".
 * For the following compilers you must supply one by uncommenting
 * an appropriate line:
 *
 *   AT&T cfront V2.X:       __ATT2__
 *   AT&T cfront V3.0:       __ATT3__
 */

/* #define __ATT2__ 1 */
/* #define __ATT3__ 1 */

/**
 **                     *** ALL USERS ***
 **/


#define _RWSTD_NOMSG    0x00
#define _RWSTD_CATGETS  0x01
#define _RWSTD_GETTEXT  0x02
#define _RWSTD_DGETTEXT 0x03

/*
 * Set _RWSTD_MESSAGE to the type of messaging facility you want:
 *   _RWSTD_NOMSG	 No messaging facility
 *   _RWSTD_CATGETS  Use catgets()
 *   _RWSTD_GETTEXT  Use gettext()
 *   _RWSTD_DGETTEXT Use dgettext()
 */

#define _RWSTD_MESSAGE _RWSTD_NOMSG


#if !defined( __TURBOC__) && !defined(_MSC_VER) && !defined(_OS2)

/******************** OPTIONAL LIBRARY FEATURES********************/
/*  turned off by default                                         */

/* Uncomment the following if you wish for bounds checking to be  
 * performed for operator[] for deque and vector
 */

/* #define _RWSTD_BOUNDS_CHECKING 1 */

/* Uncomment the following if you wish to use localized error messages
 */

/* #define _RWSTD_LOCALIZED_ERRORS 1 */


/******************** COMPILER WORD SIZES, ETC ********************/

/*
 * Uncomment the following and set to the number of decimal digits
 * of precision for type double.
 * If you do nothing, the default will be 16.
 */

/* #define _RWSTD_DEFAULT_PRECISION 16 */


/*************** COMPILER QUIRKS AND LIMITATIONS ******************/

/*
 * Uncomment the following if your compiler does not support
 * exceptions.
 */

/* #define _RWSTD_NO_EXCEPTIONS 1 */

/*
 * Uncomment the following if your compiler does not support
 * exceptions specifications.
 */

/* #define _RWSTD_NO_EX_SPEC 1 */


/*
 * Uncomment the following if your compiler does not support
 * throwing of exceptions from a shared library.
 */

/* #define _RWSTD_NO_THROW_WITH_SHARED 1 */

/*************************** TEMPLATES **********************************/

/*
 * Uncomment the following if your compiler does not instantiates only those
 * functions, member functions, classes or member classes that are
 * required; i.e. your compiler instantiates things that your program
 * doesn't actually use.
 */

/* #define _RWSTD_NO_STRICT_TEMPLATE_INSTANTIATE 1 */

/*
 * Uncomment the following if your compiler does not allow an unused
 * T *operator-> in a template<class T> when T is of non-class type.
 */

/* #define _RWSTD_NO_NONCLASS_ARROW_RETURN 1 */

/*
 * Uncomment the following if your compiler does not support the new syntax
 * for functions that are templatized only on the return type, e.g.
 *     template<class T> T func (void);  // Declare the function template
 *     int my_int = func<int>();         // Call the function
 */

/* #define _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE 1 */

/*
 * Uncomment the following if your compiler does template
 * instantiation at compile time.
 */

/* #define _RWSTD_COMPILE_INSTANTIATE 1 */


/*************************** STRINGS ****************************/

/*
 * Uncomment the following if your sprintf() does not
 * return the size of the buffer as an int, as ANSI C requires.
 */

/* #define _RWSTD_NO_ANSI_SPRINTF 1 */


/*
 * Uncomment the following if your compiler does not have the
 * ANSI C function memmove().
 */

/* #define _RWSTD_NO_MEMMOVE 1 */

/*
 * Uncomment the following if your compiler's stdio.h does not define
 * an fpos_t type.
 */

/* #define _RWSTD_NO_FPOS_T 1 */

/*
 * Uncomment the following if your compiler's stdlib.h does not provide
 * an ldiv function.
 */

/* #define _RWSTD_NO_LDIV 1 */

/*
 * Uncomment the following if your compiler's <typeinfo> header does not
 * define a bad_cast exception type.
 */

/* #define _RWSTD_NO_BAD_CAST 1 */

/****************** INTERNATIONALIZATION ************************/

/*
 * Uncomment the following if your compiler does not support
 * wide characters strings (e.g., functions wslen(), etc.).
 */

/* #define _RWSTD_NO_WSTR 1 */

/*
 * Uncomment the following if your compiler's support for
 * wide character strings does not include support for any
 * of the following: wctomb(), mbtowc(), iswspace(), wmemset(), 
 * wmemchr() and wcsstr().
 */

/* #define _RWSTD_NOT_ALL_WSTR_CFUNCTIONS 1 */

/*
 * Uncomment the following your compiler does not define type wint_t
 * in either wchar.h or wctype.h.
 */

/* #define _RWSTD_NO_WINT_TYPE 1 */

/*
 * Uncomment the following if your compiler does not provide wide
 * character functions swscanf and swprintf.
 */

/* #define _RWSTD_NO_SWPRINTF 1 */

/*
 * Uncomment the following if your compiler cannot distinguish wchar_t from
 * other integer types in template argument lists.
 */

/* #define _RWSTD_NO_OVERLOAD_WCHAR 1 */


/*
 * Uncomment the following if your compiler does not support
 * the ANSI C locale facility fully, or if it does not support
 * it at all (in particular, uncomment if setlocale(), strxform(),
 * or strcoll() are not present or don't work).
 */

/* #define _RWSTD_NO_LOCALE 1 */

 
/*
 * Uncomment the following if your compiler does not have
 * the %C directive to strftime().
 */

/* #define _RWSTD_NO_STRFTIME_CAPC 1 */


/************************** TIME ********************************/

/*
 * Uncomment the following if your compiler does not have global
 * variables "_daylight", "_timezone", and "_tzname", or corresponding
 * variables without a leading underscore (generally
 * this is true only for pure Berkeley systems).
 */

/* #define _RWSTD_NO_GLOBAL_TZ 1 */


/*
 * Uncomment the following if your system supplies a global variable
 * named "daylight" instead of the nominally more correct "_daylight".
 */

/* #define _RWSTD_NO_LEADING_UNDERSCORE 1 */


/*
 * If your system does not have global variables "daylight" and
 * "timezone" (see directive immediately above) and does not have
 * the Berkeley function gettimeofday() either, then uncomment
 * the following:
 */

/* #define _RWSTD_NO_GETTIMEOFDAY 1 */


/*
 * If the struct tm defined in your <time.h> has extra member data
 * "tm_zone" and "tm_gmtoff" (this is true for SunOs 4.X), then you
 * should uncomment the following:
 */

/* #define _RWSTD_STRUCT_TM_TZ 1 */


/* Uncomment the following if you have a Solaris platform that
 * supports threads.  (We choose this if you have it, in
 * preference to other thread packages. You may prefer a
 * different package.)
 */

/* #define _RWSTD_SOLARIS_THREADS 1 */

/* Uncomment the following if you have a threads package which
 * meets an early Posix draft, and you don't have Solaris threads.
 */

/* #define _RWSTD_POSIX_THREADS 1 */

/* As above, if you have threads which meet
 * the Posix Draft 10 threads description. If you define this,
 * then you must also define RW_POSIX_THREADS
 */

/* #define _RWSTD_POSIX_D10_THREADS 1 */

/* Uncomment the following if pthread_mutexattr_default is
 * provided by your threads package. The posix standard (draft 10)
 * does not require a pthreads package to provide this value.
 */

/* #define _RWSTD_MUTEXATTR_DEFAULT 1 */

/* Uncomment the following if you have no threads, or your package doesn't
 * meet our expectations for header and function names.
 */

/* #define _RWSTD_NO_THREADS 1 */


/************************** STANDARD LIBRARY ****************************/

/*
 * Uncomment the following if your compiler does not support the bool type.
 * This means that bool is not a unique type.
 */

/* #define _RWSTD_NO_BOOL 1 */

/*
 * Uncomment the following if your compiler does not support simple
 * default templates. e.g:
 * template<class T = int> ...
 */

/* #define _RWSTD_NO_SIMPLE_DEFAULT_TEMPLATES 1 */

/*
 * Uncomment the following if your compiler does not support complex
 * default templates. e.g:
 * template<class T = foo<T> > ...
 */

/* #define _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 1 */

/*
 * Uncomment the following if your compiler doesn't support ~T() 
 * where T is a builtin.
 */

/* #define _RWSTD_NO_DESTROY_BUILTIN 1 */

/*
 * Uncomment the following if your compiler doesn't support ~T()
 * where T is a non-builtin.
 */

/* #define _RWSTD_NO_DESTROY_NONBUILTIN 1 */

/*
 * If your compiler does not support complicated exceptions, such as
 * throwing a domain_error exception, then uncomment the following:
 */

/* #define _RWSTD_NO_COMPLICATED_EXCEPTIONS 1 */

/*
 * Uncomment the following if your compiler cannot handle nested
 * templates, or if you have to define the body of a class within
 * the template (mostly sun compilers!)
 */

/* #define _RWSTD_NO_NESTING_TEMPLATES 1 */

/*
 * If your compiler does not support long double operators in iostreams, then
 * uncomment the following:
 */

/* #define _RWSTD_NO_STREAM_LONG_DOUBLE 1 */

/*
 * Uncomment the following if your compiler does not support the
 * _MUTABLE keyword
 */

/* #define _RWSTD_NO_MUTABLE 1 */

/*
 * If your compiler does not support namespaces, uncomment the following
 */

/* #define _RWSTD_NO_NAMESPACE 1 */

/*
 * Uncomment the following if your compiler does not support
 * member template functions:
 * template<class T> class C {
 *  template<class F> void foo();
 * };
 */

/* #define _RWSTD_NO_MEMBER_TEMPLATES 1 */


/*
 * Uncomment the following if your compiler does not support
 * member template classes:
 * class C {
 *  template <class F> class N {};
 * };
 */

/* #define _RWSTD_NO_MEM_CLASS_TEMPLATES 1 */

/*
 * Uncomment the following if your compiler does not support declaring a
 * template function to be a friend:
 *   class C {
 *     template <class T> friend void foo (T);
 *   };
 */

/* #define _RWSTD_NO_FRIEND_TEMPLATES 1 */

/*
 * If nontype-args are not allowed in function declarations, then uncomment
 * the following.  template<int i> void foo(char f[10][i])
 */

/* #define _RWSTD_NO_NONTYPE_ARGS 1 */

/*
 * If simple static initialization of const member variables is not allowed,
 * uncomment the following
 */

/* #define _RWSTD_NO_STI_SIMPLE 1 */

/*
 * If simple static initialization of const member variables in a
 * template is not allowed, then uncomment the following
 */

/* #define _RWSTD_NO_STI_TEMPLATE 1 */

/*
 * If FLT_ROUNDS is a constant and not a variable,
 * uncomment the following
 */

/* #define _RWSTD_FLT_ROUNDS_IS_CONSTANT 1 */

/*
 * If nested static template variables cannot be defined outside the
 * class, uncomment this.
*/

/* #define _RWSTD_NO_STATIC_DEF 1 */

/*
 * If static template variable definitions do not require initializers,
 * uncomment this.
 */

/* #define _RWSTD_NO_STATIC_DEF2 1 */


/*
 * If templatized static data members do not work correctly, uncomment this.
 */

/* #define _RWSTD_NO_STATIC_DEF3 1 */


/*
 * Are long mangled names handled correctly by your compiler/linker?
 * If not, uncomment the following
 */

/* #define _RWSTD_NO_LONG_NAME 1 */

/*
 * Are complicated typedefs handled by your compiler?
 * If not, uncomment the following
 */

/* #define _RWSTD_NO_COMPLICATED_TYPEDEF 1 */

/*
 * Are embedded typedefs supported?
 * If not, uncomment the following
*/

/* #define _RWSTD_NO_EMBEDDED_TYPEDEF 1 */

/*
 * If your compiler does not support template template classes, then
 * uncomment the following:
 * template<class T, template<class U> allocator>
 */

/* #define _RWSTD_NO_TEMPLATE_TEMPLATE 1 */

/*
 * If your compiler does not have a wchar_t type, uncomment
 * the following
 */

/* #define _RWSTD_NO_WIDE_CHAR 1 */

/*
 * If your compiler does not have a wchar.h header file, uncomment
 * the following
 */

/* #define _RWSTD_NO_WCHAR_H 1 */

/*
 * If your compiler does not handle typedef scoping correctly,
 * then uncomment the following.
 */

/* #define _RWSTD_NO_TYPEDEF_OVERLOAD 1 */

/*
 * If your compiler does not function match on template base classes
 * correctly, then uncomment the following.
 */

/* #define _RWSTD_NO_BASE_CLASS_MATCH 1 */

/*
 * If your compiler does not handle forward specializations
 * correctly, then uncomment the following.
 */

/* #define _RWSTD_NO_FORWARD_SPECIALIZATIONS 1 */

/*
 * If your compiler does not handle template types as return types 
 * uncomment the following.
 */

/* #define _RWSTD_NO_RET_TEMPLATE 1 */

/*
 * If your compiler does not understand explicit constructors, uncomment
 * the following.
 */

/* #define _RWSTD_NO_EXPLICIT 1 */

/*
 * If your compiler does not understand explicit argument qualification, 
 * uncomment the following.
 */

/* #define _RWSTD_NO_EXPLICIT_ARG 1 */


/*
 * If your compiler does not understand the typename keyword, uncomment
 * the following.
 */

/* #define _RWSTD_NO_TYPENAME 1 */

/*
 * Does your compiler instantiate typedefs of itself correctly?  
 * If not, uncomment the following
 */

/* #define _RWSTD_NO_TYPEDEF_INST 1 */

/*
 * Does your compiler instantiate templates with const types correctly?  
 * If not, uncomment the following
 */

/* #define _RWSTD_NO_CONST_INST 1 */

/*
 * Does your compiler assume trait typedefs are int? If not, uncomment
 * the following
 */

/* #define _RWSTD_NO_INT_TYPEDEF 1 */

/*
 * Does your compiler give an ambiguity error on allocate()? If so, uncomment
 * the following
 */

/* #define _RWSTD_NO_ARG_MATCH 1 */

/*
 * Does your compiler supply the new C++-style C headers?  If not,
 * uncomment the following
 */

/* #define _RWSTD_NO_NEW_HEADER 1 */


/*
 * does your compiler provide a placement new definition?  If no,
 * uncomment the following
 */

/* #define _RWSTD_NO_NEW_DECL 1 */

/*
 * does your compiler inherit typedefs in templates correctly?  If no,
 * uncomment the following
 */

/* #define _RWSTD_NO_INHERITED_TYPEDEFS 1 */

/*
 * Does your compiler have difficulty with constructors in a return
 * statement?  If so then uncomment the following.
 */

/* #define _RWSTD_NO_CTOR_RETURN 1 */

/*
 * Does your compiler have difficulty with unDEFINED friends?  If so
 * then uncomment the following.
 */

/* #define _RWSTD_NO_UNDEFINED_FRIEND 1 */

/*
 * Does your compiler have trouble with structures that lack a default
 * constructor even when their instantiation is indirect?  If so
 * then uncomment the following.
 */

/* #define _RWSTD_NO_MEMBER_WO_DEF_CTOR 1 */

/* 
 * Does your compiler allow un-initialized static members?  If not
 * then uncomment the following.
 */

/* #define _RWSTD_NO_UNINITIALIZED_STATIC_DEF 1 */

/*
 * Does your compiler allow member types as template parameters?  If not
 * then uncomment the following.
 */

/* #define _RWSTD_NO_MEMBER_TYPE_TPARAM 1 */

/*
 * Does your compiler dis-allow the use of 'static' in the out of line
 * initialization of a static const member?  If so then uncomment the
 * following.
 */

/* #define _RWSTD_NO_STATIC_MEM_DEF 1 */

/*
 * Does your compiler not allow default constructor syntax on built in
 * types. (e.g. int i = int();).  If so then uncomment the following.
 */

/* #define _RWSTD_NO_BUILTIN_CTOR 1 */

/* Does your compiler not allow defaults for parameters in function
 * templates when the function parameter is a template parameter type.
 * If so then uncomment the following.
 */

/* #define _RWSTD_NO_DEFAULT_FOR_TPARAM 1 */

/* Does your compiler not allow name injection.  For instance, does it 
 * allow member function specializations to be declared but not defined,
 * so that the template versions of these functions will be used.
 * If not, then uncomment the following.
 */

/* #define _RWSTD_NO_NAME_INJECTION 1 */

/* Does your compiler have problems overloading on function template
 * arguments that are partial specializations?  
 * If so, then uncomment the following.
 */

/* #define _RWSTD_NO_PART_SPEC_OVERLOAD 1 */

/* Does your compiler vendor supply wctype.h?
 * If not, then uncomment the following.
 */

/* #define _RWSTD_NO_WCTYPE_H 1 */

/* Can your compiler handle explicit instantiations?
 * If not, then uncomment the following.
 */

/* #define _RWSTD_NO_EXPLICIT_INSTANTIATION 1 */

/* Can your compiler handle explicit instantiations of
 * function templates?
 * If not, then uncomment the following.
 */

/* #define _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION 1 */

/* If your compiler requires the non-standard
 * include file <ieeefp.h>, then uncomment the
 * following.
 */

/*  #define _RWSTD_REQUIRES_IEEEFP 1 */

/* If your compiler typedefs long double to 
 * double, then uncomment the following.
 */

/*  #define _RWSTD_NO_LONGDOUBLE 1 */

/* If your compiler does not support static_cast<>,
   then uncomment the following.
*/

/*  #define _RWSTD_NO_STATIC_CAST 1 */

/* If your compiler doesn't support default values for const template
 * reference arguements, then uncomment the following.
 */

/*  #define  _RWSTD_NO_INIT_CONST_TEMPLATE_REF_ARG  1 */

/*  If your compiler does not support using a template argument as a
 *  default parameter, then uncomment the following.
 */

/*  #define _RWSTD_NO_DEFAULT_TEMPLATE_ARGS 1 */

/* If your compiler does not support wide string null being
 * defined as L"" but will work correctly when defined as L"\0",
 * then uncomment the following.
 */ 
/*  #define _RWSTD_WIDE_STRING_NULL_PROBLEM 1 */

/* If your compiler does not support partial specialisation
 * of template function, then uncomment the following.
 */ 

/*  #define _RWSTD_NO_FUNC_PARTIAL_SPEC 1 */


/* If your compiler does not support partial specialisation
 * of template classes with default parameters, then uncomment
 * the following.
 */

/*  #define _RWSTD_NO_CLASS_PARTIAL_SPEC 1 */

/* If your compiler does not support overload of template function,
 * then uncomment the following.
 */

/*  #define _RWSTD_NO_OVERLOAD_OF_TEMPLATE_FUNCTION 1 */

/* If your compiler has difficulties with multi dimensional arrays of
 * container classes, then uncomment the following.
 */

/*  #define _RWSTD_NO_MULTI_DIM_ARRAY 1 */

/* If your compiler does not provide a catopen/catgets style message
 * catalog defined in header <nl_types.h>, then uncomment the following.
 */

/*  #define _RWSTD_NO_CATOPEN_CATGETS 1 */


/* If the exception handler functions are located in namespace std,
 * then uncomment the following.
 */

/*  #define _RWSTD_EXCEPTION_HANDLER_IN_STD 1 */

/* If your compiler has the class bad_alloc defined in new.h then
 * uncomment the following.
 */

/*  #define _RWSTD_BAD_ALLOC_DEFINED 1 */

/* If your compiler has the exception classes already defined 
 * then uncomment the following.
 */

/*  #define _RWSTD_EXCEPTION_PREDEFINED 1 */

/* If your compiler has the class exception in its own exception
 * file then uncomment the following.
 */

/*  #define _RWSTD_EXCEPTION_DEFINED 1 */

/* If your compiler has the class bad_exception in its own exception
 * file then uncomment the following.
 */

/*  #define _RWSTD_BAD_EXCEPTION_DEFINED 1 */

/* If your compiler will not accept opterator::new[]
 * then uncomment the following.
 */

/*  #define _RWSTD_NO_NEW_BRACKETS 1 */

/* If your compiler try to instantiate member function when
 * creating variable of a class.
 */

/*  #define _RWSTD_NO_ONLY_NEEDED_INSTANTIATION 1 */

/* If your 'C' library does not provide overloads for the pow function
 * (i.e. pow(float,float), and pow(long double, long double) if appropriate),
 * then uncommment the following.
 */

/*  #define _RWSTD_NO_OVERLOAD_C_POW 1 */

/* If your compiler does not have an mbstate_t type then uncomment
 * the following.
 */

/*  #define _RWSTD_NO_MBSTATE_T 1 */

/* If your compiler does not support the new template specialization
 * syntax then umcomment the following
 */

/* #define _RWSTD_NO_NEW_TEMPLATE_SYNTAX 1 */

/* If your compiler does not a have a throw specification on operator new
 * then uncomment the following.
 */

/*  #define _RWSTD_NO_THROW_SPEC_ON_NEW 1 */

/* 
 * Uncomment this for EDG 2.36
 */

/*  #define __NO_EDG_EXCEPTION_CLASSES=1 */

/* 
 * If you don't want to use RW extensions (to file streams) please 
 * uncomment this. See documentation for a detailed description about
 * the class wise extensions added to the library.
 */

/* #define _RWSTD_NO_EXTENSION 1 */

#ifdef __ARMCC_VERSION
  /* library is compiled /ropi or /ropi/rwpi which does not allow
   * initializing data with pointers to data
   */
  #define ARM_NO_DATA_PTR_INIT 1
  //     Enable iostream support for extra-long integer type.
  #define _RWSTD_LONG_LONG                       __int64
  #define _RWSTD_LONG_LONG_PRINTF_PREFIX         "ll"
#endif


/*************************************************************************
**************************************************************************
**									**
**		From here on, it's pretty much boilerplate		**
**		and rarely requires any tuning.				**
**									**
**************************************************************************
**************************************************************************/

/************************ Cfront derivatives ******************************/

/* Any of these defines a cfront style compiler: */
#if defined(__ATT1__) || defined(__ATT2__) || defined(__ATT3__)
#  define __ATT__ 1
#endif

#endif // !(__TURBOC__) && !(_MSC_VER) &&!(__OS2__)

#define STARTWRAP
#define ENDWRAP

#ifndef RW_NEVER_ENTER_DEFAULT_HEADER_BLOCK
#define RW_MESSAGE_SET_NUMBER 1

/* #define RW_INLINE86_ASSEMBLY 1 */
/* #define RW_KR_ONLY 1 */
/* #define RW_MESSAGE 1 */
/* #define RW_NOT_POSIX_FSTAT 1 */
/* #define RW_NO_OSTR_REF_CAST 1 */

#define RW_NO_POSIX_RE 1

/* #define RW_NO_SCHAR 1 */
/* #define RW_NO_STRFTIME_CAPC 1 */

#define RW_NO_XMSG 1

/* #define __ATT2__ 1 */
/* #define __ATT3__ 1 */
/* #define __GLOCK__ 1 */
/* #define RWOSVERSION 1 */

#define RWCOMPVERSION 120805
#define RW_BYTES_PER_PTR 4
#define RW_BYTES_PER_WORD 4
#define RW_BYTES_PER_LONG 4
#define RW_DEFAULT_PRECISION 16

/* #define RW_POSIX_D10_THREADS 1 */
/* #define RW_DCE_THREADS 1 */
/* #define RW_SOLARIS_THREADS 1 */

#define RW_NO_THREADS 1

/* #define RW_NO_DCE_PTHREAD_H 1 */
/* #define RW_CMA_WRAPPERS 1 */
/* #define RWSTD_WRAP_C 1 */
/* #define _RWSTD_WRAP_C 1 */
/* #define RWSTD_GLOBAL_ENUMS 1 */
/* #define _RWSTD_NO_BOOL 1 */
/* #define RWSTD_NO_BOOL 1 */
/* #define RWSTD_NO_ACCESS_ADJUSTMENT 1 */
/* #define RWSTD_BROKEN_ACCESS_ADJUST 1 */
/* #define _RWSTD_BROKEN_ACCESS_ADJUST 1 */
/* #define RWSTD_NO_CONST_OVERLOAD 1 */

#define _RWSTD_NO_EXCEPTIONS 1
#define RWSTD_NO_EXCEPTIONS 1

/* #define RWSTD_NO_THROW_WITH_SHARED 1 */
/* #define RWSTD_NO_OVERLOAD_UCHAR 1 */
/* #define RWSTD_NO_OVERLOAD_SCHAR 1 */
/* #define RWSTD_POSIX_D10_THREADS 1 */
/* #define _RWSTD_POSIX_D10_THREADS 1 */
/* #define RWSTD_DCE_THREADS 1 */
/* #define _RWSTD_DCE_THREADS 1 */
/* #define RWSTD_SOLARIS_THREADS 1 */
/* #define _RWSTD_SOLARIS_THREADS 1 */

#define RWSTD_NO_THREADS 1
#define _RWSTD_NO_THREADS 1

/* #define RWSTD_NO_DCE_PTHREAD_H 1 */
/* #define _RWSTD_NO_DCE_PTHREAD_H 1 */
/* #define RWSTD_NO_TYPENAME 1 */
/* #define _RWSTD_NO_TYPENAME 1 */

#define RWSTD_COMPILE_INSTANTIATE 1
#define _RWSTD_COMPILE_INSTANTIATE 1

/* #define RWSTD_HEADER_REQUIRES_HPP 1 */
/* #define RWSTD_NO_ANSI_SPRINTF 1 */

#define RWSTD_NO_STRICMP 1
#define RWSTD_NO_STRNICMP 1
#define _RWSTD_NO_WCHAR_H 1
#define RWSTD_NO_WCHAR_H 1
#define RWSTD_NO_OVERLOAD_WCHAR 1
#define _RWSTD_NO_OVERLOAD_WCHAR 1
#define _RWSTD_NO_GLOBAL_TZ 1

/* #define _RWSTD_NO_LEADING_UNDERSCORE 1 */
/* #define RWSTD_STRUCT_TM_TZ 1 */
/* #define _RWSTD_STRUCT_TM_TZ 1 */

#define RWSTD_NO_WSTR 1
#define _RWSTD_NO_WSTR 1
#define RWSTD_NOT_ALL_WSTR_CFUNCTIONS 1
#define _RWSTD_NOT_ALL_WSTR_CFUNCTIONS 1

/* #define RWSTD_SUPPLY_WSTR 1 */
/* #define _RWSTD_SUPPLY_WSTR 1 */
/* #define RWSTD_WSTR_C_HEADERS 1 */
/* #define _RWSTD_WSTR_C_HEADERS 1 */
/* #define RWSTD_USE_WCHEADERS 1 */
/* #define _RWSTD_USE_WCHEADERS 1 */

#define RWSTD_NO_WCSXFRM 1
#define _RWSTD_NO_WCSXFRM 1

/* #define RWSTD_NO_NEW_HEADER 1 */
/* #define _RWSTD_NO_NEW_HEADER 1 */
/* #define _RWSTD_NO_NEW_TEMPLATE_SYNTAX 1 */
/* #define RWSTD_NO_NEW_TEMPLATE_SYNTAX 1 */
/* #define _RWSTD_STRICT_ANSI 1 */
/* #define RWSTD_STRICT_ANSI 1 */
/* #define _RWSTD_BOUNDS_CHECKING 1 */
/* #define RWSTD_BOUNDS_CHECKING 1 */
/* #define _RWSTD_FAST_TEMP_BUF 1 */
/* #define RWSTD_FAST_TEMP_BUF 1 */
/* #define _RWSTD_ONE_STRING_MUTEX 1 */
/* #define RWSTD_ONE_STRING_MUTEX 1 */
/* #define _RWSTD_NO_FPOS_T 1 */
/* #define RWSTD_NO_FPOS_T 1 */
/* #define _RWSTD_NO_LDIV 1 */
/* #define RWSTD_NO_LDIV 1 */

#define _RWSTD_NO_NAMESPACE 1
#define RWSTD_NO_NAMESPACE 1
#define _RWSTD_NO_WCTYPE_H 1
#define RWSTD_NO_WCTYPE_H 1
#define _RWSTD_NO_SWPRINTF 1
#define RWSTD_NO_SWPRINTF 1
#define _RWSTD_NO_WINT_TYPE 1
#define RWSTD_NO_WINT_TYPE 1

/* #define _RWSTD_NO_SIMPLE_DEFAULT_TEMPLATES 1 */
/* #define RWSTD_NO_SIMPLE_DEFAULT_TEMPLATES 1 */

#define _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 1
#define RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 1

/* #define _RWSTD_NO_DESTROY_BUILTIN 1 */
/* #define RWSTD_NO_DESTROY_BUILTIN 1 */
/* #define _RWSTD_EXPLICIT_SCOPE_DESTROY 1 */
/* #define RWSTD_EXPLICIT_SCOPE_DESTROY 1 */
/* #define _RWSTD_NO_DESTROY_NONBUILTIN 1 */
/* #define RWSTD_NO_DESTROY_NONBUILTIN 1 */
/* #define _RWSTD_NO_NESTING_TEMPLATES 1 */
/* #define RWSTD_NO_NESTING_TEMPLATES 1 */
/* #define _RWSTD_NO_NONTYPE_ARGS 1 */
/* #define RWSTD_NO_NONTYPE_ARGS 1 */
/* #define _RWSTD_NO_MUTABLE 1 */
/* #define RWSTD_NO_MUTABLE 1 */

#define _RWSTD_NO_STREAM_LONG_DOUBLE 1
#define RWSTD_NO_STREAM_LONG_DOUBLE 1
#define _RWSTD_NO_BAD_CAST 1
#define RWSTD_NO_BAD_CAST 1

/* #define _RWSTD_NO_MEMBER_TEMPLATES 1 */
/* #define RWSTD_NO_MEMBER_TEMPLATES 1 */
/* #define _RWSTD_NO_FRIEND_TEMPLATES 1 */
/* #define RWSTD_NO_FRIEND_TEMPLATES 1 */
/* #define _RWSTD_NO_TYPENAME 1 */
/* #define RWSTD_NO_TYPENAME 1 */
/* #define _RWSTD_NO_MEM_CLASS_TEMPLATES 1 */
/* #define RWSTD_NO_MEM_CLASS_TEMPLATES 1 */
/* #define _RWSTD_NO_STI_SIMPLE 1 */
/* #define RWSTD_NO_STI_SIMPLE 1 */
/* #define _RWSTD_NO_STI_TEMPLATE 1 */
/* #define RWSTD_NO_STI_TEMPLATE 1 */
/* #define _RWSTD_REQUIRES_IEEEFP 1 */
/* #define RWSTD_REQUIRES_IEEEFP 1 */

#define _RWSTD_FLT_ROUNDS_IS_CONSTANT 1
#define RWSTD_FLT_ROUNDS_IS_CONSTANT 1

/* #define _RWSTD_NO_STATIC_DEF 1 */
/* #define RWSTD_NO_STATIC_DEF 1 */
/* #define _RWSTD_NO_STATIC_DEF2 1 */
/* #define RWSTD_NO_STATIC_DEF2 1 */
/* #define _RWSTD_NO_STATIC_DEF3 1 */
/* #define RWSTD_NO_STATIC_DEF3 1 */
/* #define _RWSTD_NO_LONG_NAME 1 */
/* #define RWSTD_NO_LONG_NAME 1 */
/* #define _RWSTD_NO_COMPLICATED_TYPEDEF 1 */
/* #define RWSTD_NO_COMPLICATED_TYPEDEF 1 */
/* #define _RWSTD_NO_EMBEDDED_TYPEDEF 1 */
/* #define RWSTD_NO_EMBEDDED_TYPEDEF 1 */

#define _RWSTD_NO_TEMPLATE_TEMPLATE 1
#define RWSTD_NO_TEMPLATE_TEMPLATE 1
#define _RWSTD_NO_WIDE_CHAR 1
#define RWSTD_NO_WIDE_CHAR 1

/* #define _RWSTD_NO_LONGDOUBLE 1 */
/* #define RWSTD_NO_LONGDOUBLE 1 */

#define _RWSTD_NO_STATIC_CAST 1
#define RWSTD_NO_STATIC_CAST 1

/* #define _RWSTD_NO_CONST_CAST 1 */
/* #define RWSTD_NO_CONST_CAST 1 */
/* #define _RWSTD_NO_REINTERPRET_CAST 1 */
/* #define RWSTD_NO_REINTERPRET_CAST 1 */
/* #define _RWSTD_NO_EXPLICIT_ARG 1 */
/* #define RWSTD_NO_EXPLICIT_ARG 1 */
/* #define _RWSTD_NO_TYPEDEF_OVERLOAD 1 */
/* #define RWSTD_NO_TYPEDEF_OVERLOAD 1 */
/* #define _RWSTD_NO_BASE_CLASS_MATCH 1 */
/* #define RWSTD_NO_BASE_CLASS_MATCH 1 */
/* #define _RWSTD_NO_FO_RWARD_SPECIALIZATIONS 1 */
/* #define RWSTD_NO_FO_RWARD_SPECIALIZATIONS 1 */
/* #define _RWSTD_NO_RET_TEMPLATE 1 */
/* #define RWSTD_NO_RET_TEMPLATE 1 */
/* #define _RWSTD_NO_EXPLICIT 1 */
/* #define RWSTD_NO_EXPLICIT 1 */
/* #define _RWSTD_NO_TYPEDEF_INST 1 */
/* #define RWSTD_NO_TYPEDEF_INST 1 */
/* #define _RWSTD_NO_CONST_INST 1 */
/* #define RWSTD_NO_CONST_INST 1 */
/* #define _RWSTD_NO_INT_TYPEDEF 1 */
/* #define RWSTD_NO_INT_TYPEDEF 1 */
/* #define _RWSTD_NO_ARG_MATCH 1 */
/* #define RWSTD_NO_ARG_MATCH 1 */
/* #define _RWSTD_NO_NEW_DECL 1 */
/* #define RWSTD_NO_NEW_DECL 1 */
/* #define _RWSTD_NO_INHERITED_TYPEDEFS 1 */
/* #define RWSTD_NO_INHERITED_TYPEDEFS 1 */
/* #define _RWSTD_NO_CTOR_RETURN 1 */
/* #define RWSTD_NO_CTOR_RETURN 1 */
/* #define _RWSTD_NO_UNDEFINED_FRIEND 1 */
/* #define RWSTD_NO_UNDEFINED_FRIEND 1 */
/* #define _RWSTD_NO_MEMBER_WO_DEF_CTOR 1 */
/* #define RWSTD_NO_MEMBER_WO_DEF_CTOR 1 */
/* #define _RWSTD_NO_UNINITIALIZED_STATIC_DEF 1 */
/* #define RWSTD_NO_UNINITIALIZED_STATIC_DEF 1 */
/* #define _RWSTD_NO_MEMBER_TYPE_TPARAM 1 */
/* #define RWSTD_NO_MEMBER_TYPE_TPARAM 1 */

#define _RWSTD_NO_STATIC_MEM_DEF 1
#define RWSTD_NO_STATIC_MEM_DEF 1

/* #define _RWSTD_NO_DEFAULT_FOR_TPARAM 1 */
/* #define RWSTD_NO_DEFAULT_FOR_TPARAM 1 */
/* #define _RWSTD_NO_PART_SPEC_OVERLOAD 1 */
/* #define RWSTD_NO_PART_SPEC_OVERLOAD 1 */
/* #define _RWSTD_NO_EXPLICIT_INSTANTIATION 1 */
/* #define RWSTD_NO_EXPLICIT_INSTANTIATION 1 */
/* #define _RWSTD_NO_INIT_CONST_TEMPLATE_REF_ARG 1 */
/* #define RWSTD_NO_INIT_CONST_TEMPLATE_REF_ARG 1 */
/* #define _RWSTD_NO_CLASS_PARTIAL_SPEC 1 */
/* #define RWSTD_NO_CLASS_PARTIAL_SPEC 1 */
/* #define _RWSTD_NO_FUNC_PARTIAL_SPEC 1 */
/* #define RWSTD_NO_FUNC_PARTIAL_SPEC 1 */
/* #define _RWSTD_NO_OVERLOAD_OF_TEMPLATE_FUNCTION 1 */
/* #define RWSTD_NO_OVERLOAD_OF_TEMPLATE_FUNCTION 1 */
/* #define _RWSTD_NO_MULTI_DIM_ARRAY 1 */
/* #define RWSTD_NO_MULTI_DIM_ARRAY 1 */

#define _RWSTD_NO_CATOPEN_CATGETS 1
#define RWSTD_NO_CATOPEN_CATGETS 1

/* #define _RWSTD_EXCEPTION_HANDLER_IN_STD 1 */
/* #define RWSTD_EXCEPTION_HANDLER_IN_STD 1 */
/* #define _RWSTD_BAD_ALLOC_DEFINED 1 */
/* #define RWSTD_BAD_ALLOC_DEFINED 1 */
/* #define RWSTD_EXCEPTION_PREDEFINED 1 */
/* #define _RWSTD_EXCEPTION_PREDEFINED 1 */
/* #define _RWSTD_EXCEPTION_DEFINED 1 */
/* #define _RWSTD_LOGIC_ERROR_PREDEFINED 1 */
/* #define RWSTD_LOGIC_ERROR_PREDEFINED 1 */
/* #define _RWSTD_BAD_EXCEPTION_DEFINED 1 */
/* #define RWSTD_BAD_EXCEPTION_DEFINED 1 */
/* #define _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE 1 */
/* #define RWSTD_NO_TEMPLATE_ON_RETURN_TYPE 1 */
/* #define _RWSTD_NO_NEW_BRACKETS 1 */
/* #define RWSTD_NO_NEW_BRACKETS 1 */
/* #define _RWSTD_NO_ONLY_NEEDED_INSTANTIATION 1 */
/* #define RWSTD_NO_ONLY_NEEDED_INSTANTIATION 1 */
/* #define _RWSTD_NO_EX_SPEC 1 */
/* #define RWSTD_NO_EX_SPEC 1 */
/* #define _RWSTD_NO_STRICT_TEMPLATE_INSTANTIATE 1 */
/* #define RWSTD_NO_STRICT_TEMPLATE_INSTANTIATE 1 */
/* #define _RWSTD_NO_NONCLASS_ARROW_RETURN 1 */
/* #define RWSTD_NO_NONCLASS_ARROW_RETURN 1 */
/* #define _RWSTD_NO_OVERLOAD_C_POW 1 */
/* #define RWSTD_NO_OVERLOAD_C_POW 1 */

#define _RWSTD_NO_MBSTATE_T 1
#define RWSTD_NO_MBSTATE_T 1

/* #define _RWSTD_NO_THROW_SPEC_ON_NEW 1 */
/* #define RWSTD_NO_THROW_SPEC_ON_NEW 1 */
/* #define _RWSTD_NOTHROW_IN_STD 1 */
/* #define _RWSTD_NO_BUILTIN_CTOR 1 */
/* #define RWSTD_NO_BUILTIN_CTOR 1 */

#define _RWSTD_ANSI_FILEIO 1
#endif /* RW_NEVER_ENTER_DEFAULT_HEADER_BLOCK */
// [Removed settings for various non-ARM compilers]

/********************** Miscellaneous *********************************/

/* No Pi for these compilers: */
#if defined(_RWSTD_MSC_BACKEND) || defined(__OREGON__) || defined(__HIGHC__) || defined(applec) || defined(CII) || defined(__WATCOMC__)
#  ifndef M_PI
#    define M_PI 3.14159265358979323846
#  endif
#endif

/*
 * Only Sun defines strftime("%C", ...)
 */
#if !defined(_RWSTD_NO_STRFTIME_CAPC) && !defined(sun)
#define _RWSTD_NO_STRFTIME_CAPC 1
#endif

#if defined(__SUNPRO_CC)&&(__SUNPRO_CC>=0x420)
#define _RWSTD_NO_TEMPLATE_STATIC_ID 1
#define _RWSTD_NO_MBSTATE_COMPARE
#define _RWSTD_EXCEPTION_DEFINED
#endif

/********************** Environment *********************************/
/*
 * This is the section for setting things which depend on the properties
 * of the operating systems rather than specific compilers.  It follows
 * the compiler section so we have the chance to rationalize the different
 * preprocessor constants (e.g. _MSDOS vs. __MSDOS__,  _M_I86LM vs. __LARGE__)
 */

#ifndef _RWSTD_DEFAULT_PRECISION
#   define _RWSTD_DEFAULT_PRECISION 16	/* Assume standard IEEE format */
#endif

#if defined(__cplusplus)&&(__cplusplus >= 199707L) && defined(__STDCPP__) && (__STDCPP__ == 1)
#define _HPACC_ 1
#undef _RWSTD_NO_NEW_HEADER
#if defined(__cplusplus)&&(__cplusplus >= 199711L)
#ifndef _RWSTD_BAD_ALLOC_DEFINED
#define _RWSTD_BAD_ALLOC_DEFINED
#endif
#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
#define _RWSTD_NO_CLASS_PARTIAL_SPEC
#endif
#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
#define _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
#endif
#ifndef _RWSTD_NO_MEMBER_TEMPLATES
#define _RWSTD_NO_MEMBER_TEMPLATES
#endif
#endif
#endif

/*
 * Most (but not all) non-unix systems convert new line to carriage
 * return / line feed on output:
 */
#if defined(__MSDOS__) || defined(__OS2__) || defined(__WIN32__) || defined(__NT__) || defined(__WINDOWS__)
#  define _RWSTD_CRLF_CONVENTION 1
#endif

/*
** Miscellaneous workarounds.
*/

#ifdef _RWSTD_NO_BOOL
# ifdef _RWSTD_MSVC_BOOL_WARNING
#  pragma warning ( disable : 4237 )
# endif
typedef int     bool;
# ifndef true
#  define true    1
# endif
# ifndef false
#  define false   0
# endif
#endif // _RWSTD_NO_BOOL

#ifndef _RWSTD_NO_TYPENAME
#define _TYPENAME typename
#else
#define _TYPENAME
#endif

#ifndef _RWSTD_NO_EXPLICIT
#define _EXPLICIT explicit
#else
#define _EXPLICIT 
#endif

#ifndef _RWSTD_NO_MUTABLE
#define _MUTABLE mutable
#else
#define _MUTABLE
#endif

#ifndef _RWSTD_NO_TRICKY_INLINES
#define _RWSTD_TRICKY_INLINE inline
#else
#define _RWSTD_TRICKY_INLINE
#endif

#ifdef _RWSTD_NO_MEMBER_WO_DEF_CTOR
#define _RWSTD_NO_CONST_INST 1
#endif

#if defined(_RWSTD_NO_STI_SIMPLE) && !defined(_RWSTD_NO_STI_TEMPLATE)
#define _RWSTD_NO_STI_TEMPLATE
#endif

#ifdef _RWSTD_COMPILE_INSTANTIATE
/* explicit instantiation doesn't help if _RWSTD_COMPILE_INSTANTIATE */
# define _RWSTD_NO_EXPLICIT_INSTANTIATION   1
# define _RWSTD_NO_INSTANTIATE 1
#endif

#ifdef _RWSTD_NO_EXPLICIT_INSTANTIATION   
#    define _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION    1
#endif

#ifdef _RWSTD_NO_WIDE_CHAR
# ifndef _RWSTD_NO_OVERLOAD_WCHAR /* ARM: avoid redefinition warning */
#  define _RWSTD_NO_OVERLOAD_WCHAR 1
# endif
# ifndef _RWSTD_NO_WSTR /* ARM: avoid redefinition warning */
#  define _RWSTD_NO_WSTR 1
# endif
#endif


//
// Macro for forming or omitting default template arguments in constructors
//

#ifndef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS
#  define _RWSTD_DEFAULT_ARG(n) = n
#else
#  define _RWSTD_DEFAULT_ARG(n) 
#endif

//
// Macro for forming or ommitting default template parameters.
//
#ifndef _RWSTD_NO_SIMPLE_DEFAULT_TEMPLATES
#  define _RWSTD_SIMPLE_DEFAULT(a)  = a
#  ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
#    define _RWSTD_COMPLEX_DEFAULT(a)  = a
#  else
#    define _RWSTD_COMPLEX_DEFAULT(a)
#  endif
#else
#  ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
#    define _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
#  endif
#  define _RWSTD_SIMPLE_DEFAULT(a)
#  define _RWSTD_COMPLEX_DEFAULT(a)
#  ifndef _RWSTD_NO_DEFAULT_TEMPLATES
#    define _RWSTD_NO_DEFAULT_TEMPLATES
#  endif
#endif

//
// Macros for adding 'std' or '__rwstd' to names
//
#ifndef _RWSTD_NO_NAMESPACE
#  ifdef _MSC_VER
#    define _RW_STD ::std
#    define __RWSTD ::__rwstd
#  else
#    define _RW_STD std
#    define __RWSTD __rwstd
#  endif
#else
#  define _RW_STD 
#  define __RWSTD 
# endif

//
// Macro for casting, using either the "old" method
// or the new C++ cast system
//

#ifdef _RWSTD_NO_STATIC_CAST
#  define _RWSTD_STATIC_CAST(x,y) ((x)(y))
#  define _RWSTD_REINTERPRET_CAST(x,y) ((x)(y))
#  define _RWSTD_CONST_CAST(x,y) ((x)(y))
#  define _RWSTD_REINTERPRET_CONST_CAST(x,y,z) ((x)(z))
#else
#  define _RWSTD_STATIC_CAST(x,y) static_cast< x >(y)
#  define _RWSTD_REINTERPRET_CAST(x,y) reinterpret_cast< x >(y)
#  define _RWSTD_CONST_CAST(x,y) const_cast< x >(y)
#  define _RWSTD_REINTERPRET_CONST_CAST(x,y,z) reinterpret_cast< x >(const_cast< y >(z))
#endif

//  
// Macro for the new template specialization syntax
//
#ifdef _RWSTD_NO_NEW_TEMPLATE_SYNTAX
#  define _RWSTD_TEMPLATE 
#else
#  define _RWSTD_TEMPLATE template<>
#endif

//
// If compiler supports member and default templates then it support
// the _RWSTD_ALLLOCATOR
//
#if !defined(_RWSTD_NO_MEMBER_TEMPLATES) && !defined(_RWSTD_NO_SIMPLE_DEFAULT_TEMPLATES) && !defined(_RWSTD_NO_MEM_CLASS_TEMPLATES)
#define _RWSTD_ALLOCATOR
#endif

#ifdef _HPACC_
#ifdef _RWSTD_ALLOCATOR
#undef _RWSTD_ALLOCATOR
#endif
#endif

//
// Define typedef macro
//
#ifdef _RWSTD_NO_EMBEDDED_TYPEDEF
#  define _RWSTD_ALLOC_SIZE_TYPE        Allocator::size_type
#  define _RWSTD_ALLOC_DIFF_TYPE        Allocator::difference_type
#else
#  define _RWSTD_ALLOC_SIZE_TYPE        allocator_type::size_type
#  define _RWSTD_ALLOC_DIFF_TYPE        allocator_type::difference_type
#endif
 
#define _RWSTD_MB_CUR_MAX 16  //???  Need to find absolute maximum for this

#include <compnent.h>

//
// Define a generic macro for throw.  To be used as in the following:
//
//    _RWSTD_THROW(i < j && j < k, out_of_range, "j is out of range!");
//
//                ^ predicate     ^ exception   ^ message
//
// literally, if (predicate) throw exception(message)
//
#ifdef _RWSTD_NO_EXCEPTIONS
//
// If we don't have exceptions, then we'll use assert.
// We don't allow them to turn off the assert() in which such
// a thrown exception would result.
//

#ifdef  NDEBUG
#define __RW_NDEBUG
#undef  NDEBUG
#endif
#ifdef NOT_FOR_ARM
#ifndef _RWSTD_NO_NEW_HEADER
#include <cassert>
#else
#include <assert.h>
#endif
#define _RWSTD_THROW_NO_MSG(PRED,EXC)  assert(!(PRED))
#define _RWSTD_THROW(PRED,EXC,MESG) assert(!(PRED))
#else
#define _RWSTD_NO_EX_MSGS 1
extern "C" void __assert(const char *, const char *, int);
#define __cheap_assert(e) ((e) ? (void)0 : __assert(#e, __MODULE__, 0))
#define _RWSTD_THROW_NO_MSG(PRED,EXC)  __cheap_assert(!(PRED))
#define _RWSTD_THROW(PRED,EXC,MESG) __cheap_assert(!(PRED))
#endif
//
// We must also turn off expansion of assert() if that's what the user expects.
//
#ifdef  __RW_NDEBUG
#define NDEBUG
#undef  __RW_NDEBUG
#endif
#else /*!_RWSTD_NO_EXCEPTIONS*/
//
// We must check to see if we can use <stdexcept> or just a string.
//
#ifdef _RW_STD_EXCEPT
#define _RWSTD_THROW_NO_MSG(PRED,EXC) if (PRED) throw EXC()
#define _RWSTD_THROW(PRED,EXC,MESG) if (PRED) throw EXC(MESG)  
#else
#define _RWSTD_THROW(PRED,EXC,MESG) if (PRED) throw (MESG)  
#endif
#endif /*_RWSTD_NO_EXCEPTIONS*/

//
// Define two generic throw specification macros.  One to illustrate
// the exceptions that a function can throw and the other to indicate
// that a function doesn't throw any exceptions.
//
//   _RWSTD_THROW_SPEC(ExceptionList)
//
//   _RWSTD_THROW_SPEC_NULL
//
// Owing to the face that the first macro must be able to take a
// variable number of arguments, we must simulate this by always
// passing the exceptions in parentheses; i.e.
//
//  void f () _RWSTD_THROW_SPEC((out_of_range));
//  void g () _RWSTD_THROW_SPEC((domain_error, invalid_argument));
//  void h () _RWSTD_THROW_SPEC((out_of_range, invalid_argument, length_error));
//

#if defined(_RWSTD_NO_EXCEPTIONS) || defined(_RWSTD_NO_EX_SPEC)
#define _RWSTD_THROW_SPEC(EXCEPTIONS) /**/
#define _RWSTD_THROW_SPEC_NULL        /**/
#else
#ifdef _RW_STD_EXCEPT
//
// _RWSTD_THROW will use the exceptions in <stdexcept>
//
#define _RWSTD_THROW_SPEC(ExceptionList) throw ExceptionList
#define _RWSTD_THROW_SPEC_NULL           throw()
#else
//
// _RWSTD_THROW will only throw const char *
//
#define _RWSTD_THROW_SPEC(ExceptionList) throw(const char *)
#define _RWSTD_THROW_SPEC_NULL           throw()
#endif
#endif /*_RWSTD_NO_EXCEPTIONS||_RWSTD_NO_EX_SPEC*/

#ifndef _RWSTD_NO_TRICKY_INLINES
#define _RWSTD_INLINE_NO_THROW _RWSTD_THROW_SPEC_NULL
#else
#define _RWSTD_INLINE_NO_THROW
#endif

// 
// Macro for path to the ANSI 'C' headers 
// Must be set specifically for each platform when the
// C++ wrappers for 'C' headers are used.
//
#define _RWSTD_ANSIC(x) </usr/include/x>

#define _RWSTD_UNUNSED(x) ((void)(x))

#endif /*__RWSTDCOMPILER_H__*/

/* time.h: ANSI 'C' (X3J11 Oct 88) library header, section 4.12 */
/* Copyright (C) Codemist Ltd., 1988-1993.                      */
/* Copyright 1991-1993 ARM Limited. All rights reserved.        */
/* version 0.03 */

/*
 * time.h declares two macros, four types and several functions for
 * manipulating time. Many functions deal with a calendar time that
 * represents the current date (according to the Gregorian
 * calendar) and time. Some functions deal with local time, which
 * is the calendar time expressed for some specific time zone, and
 * with Daylight Savings Time, which is a temporary change in the
 * algorithm for determining local time.
 */

/*
 * RCS $Revision: 1.8.2.2 $
 * Checkin $Date: 2001/05/23 21:56:08 $
 * Revising $Author: sdouglas $
 */

#ifndef __time_h
#define __time_h

  #ifndef __TIME_DECLS
  #define __TIME_DECLS

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

    /* CLOCKS_PER_SEC: the number per second of the value returned by the
     * clock function. */
#ifdef __CLK_TCK
#  define CLOCKS_PER_SEC  __CLK_TCK
#else
#  define CLOCKS_PER_SEC  100
#endif
#ifndef __STRICT_ANSI__
#define CLK_TCK CLOCKS_PER_SEC
#endif

typedef unsigned int clock_t;    /* cpu time type */
typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */

struct tm {
  int tm_sec;   /* seconds after the minute, 0 to 60
                   (0 - 60 allows for the occasional leap second) */
  int tm_min;   /* minutes after the hour, 0 to 59 */
  int tm_hour;  /* hours since midnight, 0 to 23 */
  int tm_mday;  /* day of the month, 1 to 31 */
  int tm_mon;   /* months since January, 0 to 11 */
  int tm_year;  /* years since 1900 */
  int tm_wday;  /* days since Sunday, 0 to 6 */
  int tm_yday;  /* days since January 1, 0 to 365 */
  int tm_isdst; /* Daylight Savings Time flag */
};
   /* struct tm holds the components of a calendar time, called the broken-down
    * time. The value of tm_isdst is positive if Daylight Savings Time is in
    * effect, zero if Daylight Savings Time is not in effect, and negative if
    * the information is not available.
    */

extern clock_t clock(void);
   /* determines the processor time used.
    * Returns: the implementation's best approximation to the processor time
    *          used by the program since program invocation. The time in
    *          seconds is the value returned divided by the value of the macro
    *          CLK_TCK. The value (clock_t)-1 is returned if the processor time
    *          used is not available.
    */
extern double difftime(time_t /*time1*/, time_t /*time0*/);
   /*
    * computes the difference between two calendar times: time1 - time0.
    * Returns: the difference expressed in seconds as a double.
    */
extern time_t mktime(struct tm * /*timeptr*/);
   /*
    * converts the broken-down time, expressed as local time, in the structure
    * pointed to by timeptr into a calendar time value with the same encoding
    * as that of the values returned by the time function. The original values
    * of the tm_wday and tm_yday components of the structure are ignored, and
    * the original values of the other components are not restricted to the
    * ranges indicated above. On successful completion, the values of the
    * tm_wday and tm_yday structure components are set appropriately, and the
    * other components are set to represent the specified calendar time, but
    * with their values forced to the ranges indicated above; the final value
    * of tm_mday is not set until tm_mon and tm_year are determined.
    * Returns: the specified calendar time encoded as a value of type time_t.
    *          If the calendar time cannot be represented, the function returns
    *          the value (time_t)-1.
    */
extern time_t time(time_t * /*timer*/);
   /*
    * determines the current calendar time. The encoding of the value is
    * unspecified.
    * Returns: the implementations best approximation to the current calendar
    *          time. The value (time_t)-1 is returned if the calendar time is
    *          not available. If timer is not a null pointer, the return value
    *          is also assigned to the object it points to.
    */

extern char *asctime(const struct tm * /*timeptr*/);
   /*
    * converts the broken-down time in the structure pointed to by timeptr into
    * a string in the form "Sun Sep 16 01:03:52 1973\n\0".
    * Returns: a pointer to the string containing the date and time.
    */
extern char *ctime(const time_t * /*timer*/);
   /*
    * converts the calendar time pointed to by timer to local time in the form
    * of a string. It is equivalent to asctime(localtime(timer));
    * Returns: the pointer returned by the asctime function with that
    *          broken-down time as argument.
    */
extern struct tm *gmtime(const time_t * /*timer*/);
   /*
    * converts the calendar time pointed to by timer into a broken-down time,
    * expressed as Greenwich Mean Time (GMT).
    * Returns: a pointer to that object or a null pointer if GMT not available.
    */
extern struct tm *localtime(const time_t * /*timer*/);
   /*
    * converts the calendar time pointed to by timer into a broken-down time,
    * expressed a local time.
    * Returns: a pointer to that object.
    */
extern size_t strftime(char * /*s*/, size_t /*maxsize*/,
                       const char * /*format*/, const struct tm * /*timeptr*/);
   /*
    * places characters into the array pointed to by s as controlled by the
    * string pointed to by format. The format string consists of zero or more
    * directives and ordinary characters. A directive consists of a % character
    * followed by a character that determines the directive's behaviour. All
    * ordinary characters (including the terminating null character) are copied
    * unchanged into the array. No more than maxsize characters are placed into
    * the array. Each directive is replaced by appropriate characters  as
    * described in the following list. The appropriate characters are
    * determined by the LC_TIME category of the current locale and by the
    * values contained in the structure pointed to by timeptr.
    * %a is replaced by the locale's abbreviated weekday name.
    * %A is replaced by the locale's full weekday name.
    * %b is replaced by the locale's abbreviated month name.
    * %B is replaced by the locale's full month name.
    * %c is replaced by the locale's appropriate date and time representation.
    * %d is replaced by the day of the month as a decimal number (01-31).
    * %H is replaced by the hour (24-hour clock) as a decimal number (00-23).
    * %I is replaced by the hour (12-hour clock) as a decimal number (01-12).
    * %j is replaced by the day of the year as a decimal number (001-366).
    * %m is replaced by the month as a decimal number (01-12).
    * %M is replaced by the minute as a decimal number (00-59).
    * %p is replaced by the locale's equivalent of either AM or PM designations
    *       associated with a 12-hour clock.
    * %S is replaced by the second as a decimal number (00-61).
    * %U is replaced by the week number of the year (Sunday as the first day of
    *       week 1) as a decimal number (00-53).
    * %w is replaced by the weekday as a decimal number (0(Sunday) - 6).
    * %W is replaced by the week number of the year (Monday as the first day of
    *       week 1) as a decimal number (00-53).
    * %x is replaced by the locale's appropriate date representation.
    * %X is replaced by the locale's appropriate time representation.
    * %y is replaced by the year without century as a decimal number (00-99).
    * %Y is replaced by the year with century as a decimal number.
    * %Z is replaced by the timezone name or abbreviation, or by no characters
    *       if no time zone is determinable.
    * %% is replaced by %.
    * If a directive is not one of the above, the behaviour is undefined.
    * Returns: If the total number of resulting characters including the
    *          terminating null character is not more than maxsize, the
    *          strftime function returns the number of characters placed into
    *          the array pointed to by s not including the terminating null
    *          character. otherwise, zero is returned and the contents of the
    *          array are indeterminate.
    */

    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __TIME_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __TIME_NO_EXPORTS
      using std::clock_t;
      using std::time_t;
      using std::tm;
      using std::tm;
      using std::clock;
      using std::difftime;
      using std::mktime;
      using std::time;
      using std::asctime;
      using std::ctime;
      using std::gmtime;
      using std::localtime;
      using std::strftime;
    #endif
  #endif

#endif

/* end of time.h */














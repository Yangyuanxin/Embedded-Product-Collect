/* alloca.h: Stack based allocation */
/* Copyright 1999 ARM Limited. All rights reserved */

/*
 * RCS $Revision: 1.5.2.2 $
 * Checkin $Date: 2001/05/23 21:56:07 $
 * Revising $Author: sdouglas $
 */

#ifndef __alloca_h
#define __alloca_h

  #ifndef __ALLOCA_DECLS
  #define __ALLOCA_DECLS

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
#define NULL 0                  /* see <stddef.h> */

typedef struct {
    void *__chunk;
    int __pos;
} __alloca_state;

#define alloca(n) __CLIBNS __alloca(n)

void *__alloca(size_t /* size */);

/* Allocate a block of at least size bytes. Returns a pointer to the first
 * word, or NULL if the allocation isn't successful.
 * Deallocation of all blocks allocated in a function is done on return.
 * Alloca can not be used as a function pointer.
 */

    #ifdef __cplusplus
      }  /* extern "C" */
    #endif

    #ifdef __EDG_RUNTIME_USES_NAMESPACES
      }  /* namespace std */
    #endif
  #endif /* __ALLOCA_DECLS */

  #ifdef __EDG_RUNTIME_USES_NAMESPACES
    #ifndef __ALLOCA_NO_EXPORTS
      using std::__alloca_state;
      using std::__alloca;
    #endif
  #endif
#endif

/* end of alloca.h */

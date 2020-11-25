/* rt_heap.h: definitions to enable retargetting of heap implementation
 *
 * Copyright 1999 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.8 $
 * Checkin $Date: 2000/11/08 15:43:29 $
 * Revising $Author: statham $
 */

#ifndef __RT_HEAP_H
#define __RT_HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This is the structure that defines the heap descriptor. The
 * first section of it is used by the C library and so the format
 * is fixed. Space after that can be used as the user wishes.
 */
struct __Heap_Descriptor;

/*
 * Define all these functions to override the heap. __Heap_DescSize
 * must return the size of the __Heap_Descriptor structure.
 */
extern int __Heap_DescSize(int zero);
extern void __Heap_Initialize(struct __Heap_Descriptor *h);
extern void __Heap_ProvideMemory(struct __Heap_Descriptor *, void *, size_t);
extern void *__Heap_Alloc(struct __Heap_Descriptor *, size_t);
extern void __Heap_Free(struct __Heap_Descriptor *, void *);
extern void *__Heap_Realloc(struct __Heap_Descriptor *, void *, size_t);
extern void *__Heap_Stats(struct __Heap_Descriptor *,
                          int (* /*print*/)(void *, char const *format, ...),
                          void * /*printparam*/);
extern int __Heap_Valid(struct __Heap_Descriptor *,
                        int (* /*print*/)(void *, char const *format, ...),
                        void * /*printparam*/, int /*verbose*/);

/*
 * The heap can call these functions when it is full or
 * inconsistent. __Heap_Full is passed the extra size that the heap
 * needs, and will call back to __Heap_ProvideMemory if it can find
 * some more storage. These are not functions you should override.
 */

extern int __Heap_Full(struct __Heap_Descriptor *, size_t);
extern void __Heap_Broken(struct __Heap_Descriptor *);

/*
 * Call _init_alloc, and define __rt_heap_extend, if you are trying
 * to run a heap on the bare metal.
 */
extern void _init_alloc(unsigned /*base*/, unsigned /*top*/);
extern unsigned __rt_heap_extend(unsigned /*size*/, void ** /*block*/);

#ifdef __cplusplus
}
#endif

#endif

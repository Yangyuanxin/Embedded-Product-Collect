/*****************************************************************************
*
* www.goome.net
*
*****************************************************************************/

 
#ifndef __GM_MEMORY_H__
#define __GM_MEMORY_H__

#include "gm_type.h"

extern char goome_memoryInit(void);

/*****************************************************************
* Function:     GM_MemoryAlloc 
* 
* Description:
*               Allocates memory with the specified size in the memory heap.
* Parameters:
*               Size: 
*                  [in]Number of bytes of memory to be allocated.
* Return:        
*               A pointer of void type to the allocated space.
*****************************************************************/
extern void* GM_MemoryAlloc (u32 size);


/*****************************************************************
* Function:     GM_MemoryRealloc 
* 
* Description:
*               e-allocate a memory buffer
* Parameters:
*               Ptr: 
*                  [in]old memory buffer pointer. 
*               size:
*                  [in]request memory size.
* Return:        
*               re-allocated memory buffer pointer. return NULL if no memory available.
*               
*****************************************************************/
extern void *GM_MemoryRealloc (void *ptr, u32 size);


/*****************************************************************
* Function:     GM_MemoryFree 
* 
* Description:
*               Free memory 
* Parameters:
*               Ptr: 
*                  [in]Previously allocated memory block to be freed. 
* Return:        
*               none
*               
*****************************************************************/
extern void GM_MemoryFree (void *ptr);


#endif  // End of __GM_MEMORY_H__


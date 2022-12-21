
/* MEMORY */

#ifndef _DC_MEMORY_H
#define _DC_MEMORY_H

#include <stddef.h>

/* Initialize system heap memory so it can be used */
void dcMemory_Init();


/* Simple stack allocator */
typedef struct SDC_StackAllocator
{
    unsigned char* base;
    unsigned char* current;
    unsigned char* end;

} SDC_StackAllocator;

/* Initializes an stack allocator with the specified size bytes */
void dcStackAllocator_Init(SDC_StackAllocator* allocator, size_t size);

/* Release back to the heap all the memory used by the stack allocator */
void dcStackAllocator_Release(SDC_StackAllocator* allocator);

/* Allocate size bytes from the stack allocator, with the specified alignment */
void* dcStackAllocator_Alloc(SDC_StackAllocator* allocator, size_t size, size_t align);

/* Frees memory up to ptr */
void dcStackAllocator_Free(SDC_StackAllocator* allocator, void* ptr);

/* Rewinds the allocator to start */
void dcStackAllocator_Rewind(SDC_StackAllocator* allocator);



#endif /* _DC_MEMORY_H */

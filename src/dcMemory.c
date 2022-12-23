

#include "dcMemory.h"
#include <malloc.h>
#include <stdio.h>

unsigned long __ramsize = 0x00200000; // 2MB RAM
unsigned long __stacksize = 0x00004000; // 16k stack

void dcMemory_Init()
{
    #ifdef __GNUC__
    // The GCC compiler we are using does not provide sectend() mips function, instead it provides those globals:
    extern unsigned char __heap_start[];
    extern unsigned char __stack_start[]; // Stack start from the end of ram and grows backwards   
    
    #else // psyq compiler
    // unsigned long bssend;
    // __asm__ volatile (
    //      "la %0,sectend(.bss)" :"=r" (bssend)
    //  );
    // ...
    #endif

    size_t AvailableHeap = __stack_start - __heap_start - __stacksize;

    printf("Heap start 0x%08x\n", __heap_start );
    printf("Ram size %d\n", __ramsize);
    printf("Available Heap %d\n", AvailableHeap);

    unsigned long int* HeapAllocInit = (unsigned long int*)((unsigned long int)(__heap_start + 4) & 0xFFFFFFFC); // align to 4 byte boundary just in case
    InitHeap3(HeapAllocInit, AvailableHeap - ((unsigned long int)__heap_start & 0x00000003) ); // remove the bytes wasted for alignment
}

void dcStackAllocator_Init(SDC_StackAllocator* allocator, size_t size)
{
    allocator->base = (unsigned char*)malloc3(size);
    allocator->current = allocator->base;
    allocator->end = allocator->base + size;
}

void dcStackAllocator_Release(SDC_StackAllocator* allocator)
{
    free3(allocator->base);
}

void* dcStackAllocator_Alloc(SDC_StackAllocator* allocator, size_t size, size_t align)
{
    align = align > 0 ? align : 1;
    unsigned char* alignedAddr = (unsigned char*)(((unsigned long)allocator->current + align - 1) & ~(align - 1));
    if( alignedAddr + size < allocator->end)
    {
        allocator->current = alignedAddr + size;
        return alignedAddr;
    }
    return NULL;
}

void dcStackAllocator_Free(SDC_StackAllocator* allocator, void* ptr)
{
    if( allocator->current > (unsigned char*)ptr)
    {
        allocator->current = ptr;
    }
}

void dcStackAllocator_Rewind(SDC_StackAllocator* allocator)
{
    allocator->current = allocator->base;
}

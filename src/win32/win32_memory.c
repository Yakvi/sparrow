#include "types.h"

global_variable struct memory* Win32_MainMemory;

local b32
Win32_MemoryFree(void* address)
{
    b32 Result = VirtualFree(address, 0, MEM_RELEASE);
    Assert(Result);
    return (Result);
}

inline void*
Win32_MemoryAlloc(memory_index Size)
{
    void* Result = VirtualAlloc(0, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#if SPARROW_DEV
    Assert(Size > 0);
    Assert(Result);
    Assert(*((u8*)Result + (Size - 1)) == 0);
#endif

    return (Result);
}

local struct memory*
Win32_MainMemoryInit(memory_index Size)
{
    struct memory* Result = Win32_MemoryAlloc(Size + sizeof(struct memory));
    Result->Size = Size;
    Result->Data = Result + sizeof(struct memory);

    return (Result);
}

#if !defined(SPARROW_MEMORY_H)
EXTERN_C_START

#define _GET_SIZE(Memory, Size, ...) PushMemory(Memory, Size, __VA_ARGS__, 4, MemoryFlag_Clear)
#define EXPAND(x) x
#define GetSize(Memory, Size, ...) EXPAND(_GET_SIZE(Memory, Size, __VA_ARGS__))
#define GetStruct(MemoryBank, Structure, ...) (Structure*)GetSize(MemoryBank, sizeof(Structure), ##__VA_ARGS__)
#define Reset(MemoryBank, ...) ResetMemory(MemoryBank, ##__VA_ARGS__)

struct memory
{
    memory_index Size;
    u8*          Data;
    memory_index Used;
};

enum memory_flags
{
    MemoryFlag_Clear   = 0b1,
    MemoryFlag_NoClear = 0b10,
};

inline void
ClearMemory(void* Memory, memory_index Size)
{
    // TODO: since we always align by 4 by default, can clear by u32s?
    for (memory_index Index = 0; Index < Size; ++Index) {
        ((u8*)Memory)[Index] = 0;
    }
}

inline void
ResetMemory(struct memory* Memory, u32 Size)
{
    Assert(Memory);
    if (Size) {
        Memory->Size = Size; // Can write from 0 to Size - 1
    }
    Memory->Used = 0;
}

inline memory_index
GetEffectiveSize(struct memory* Memory, memory_index DesiredSize, u32 Alignment)
{
    // TODO: This function must look at the final address!
    memory_index EffectiveSize = DesiredSize;
    memory_index Mask          = Alignment - 1;

    Assert((Alignment & Mask) == 0);
    Assert(Memory->Size >= Memory->Used + Mask);

    memory_index CurrentBlock  = (memory_index)Memory->Data + Memory->Used + DesiredSize;
    memory_index AlignmentMask = Alignment - 1;

    memory_index Offset = 0;
    if (CurrentBlock & AlignmentMask) {
        Offset = Alignment - (CurrentBlock & AlignmentMask);
    }

    return (EffectiveSize + Offset);
}

inline void*
PushMemory(struct memory* Memory, memory_index SizeInit, u32 Alignment, u32 Flags, ...)
{

    void* Result = Memory->Data + Memory->Used;

    memory_index Size = GetEffectiveSize(Memory, SizeInit, Alignment);

    Assert(Memory->Size >= Memory->Used + Size);
    Memory->Used += Size;
    Assert(Memory->Size >= Memory->Used);

    if (Flags & MemoryFlag_Clear) {
        ClearMemory(Result, Size);
    }

    return Result;
}

local struct memory*
MemoryInit(struct memory* Memory, memory_index Size)
{
    struct memory* Result = (struct memory*)GetSize(Memory, (Size + sizeof(struct memory)));
    Result->Size          = Size;
    Result->Data          = (u8*)Result + sizeof(struct memory);

    return (Result);
}

//
// NOTE: Memory Utilities
//
#define AlignPow2(Value, Alignment) \
    ((Value + ((Alignment)-1)) & ~((Alignment)-1))
#define Align4(Value) (((Value) + 3) & ~3)
#define Align8(Value) (((Value) + 7) & ~7)
#define Align16(Value) (((Value) + 15) & ~15)

#define Real32Maximum FLT_MAX
#define UIntMin 0
#define UInt32Max 0xFFFFFFFF
#define UInt16Max 65535
#define Int16Min -32768
#define Int16Max 32767

EXTERN_C_END
#define SPARROW_MEMORY_H
#endif

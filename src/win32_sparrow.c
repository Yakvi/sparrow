#include <yak_defines.h>
#include "sparrow_platform.h"
#include "fake_windows.h"

local void*
Win32_MemoryAlloc(memory_index Size)
{
    void* Result = VirtualAlloc(0, Size, 0x3000, 0x4); // MEM_RESERVE | MEM_COMMIT, PAGE_READ_WRITE
    Assert(Result);
    return (Result);
}

local struct memory*
MainMemoryRequest(memory_index Size)
{
    struct memory* Result = Win32_MemoryAlloc(Size);
    // TODO: shape up memory
    return (Result);
}

local struct screen_buffer*
RequestScreenBuffer(u16 width, u16 height)
{
    struct screen_buffer* Result = Win32_MemoryAlloc(sizeof(struct screen_buffer) +
                                                     (width * height * sizeof(u32)));
    Result->Width = width;
    Result->Height = height;
    Result->Pixels = (void*)(Result + sizeof(struct screen_buffer));
    return (Result);
}

local struct user_input*
InitializeInput(void)
{
    struct user_input* Result = Win32_MemoryAlloc(sizeof(struct user_input));
    return (Result);
}

local void
ReadInput(struct user_input* Input)
{
}

local void
UpdateState(struct memory* Memory,
            struct screen_buffer* Buffer,
            struct user_input* Input)
{
}

local b32
IsTimeToRender(void)
{
    return true;
}

local void
Render(struct screen_buffer* Buffer)
{
}

int __stdcall WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    char* CmdLine,
    int ShowCmd)
{
    struct screen_buffer* Buffer = RequestScreenBuffer(1920, 1080);
    struct user_input* Input = InitializeInput();
    struct memory* Memory = MainMemoryRequest(Kilobytes(10));

    b32 Running = true;

    while (Running)
    {
        ReadInput(Input);
        UpdateState(Memory, Buffer, Input);

        if (IsTimeToRender())
        {
            OutputDebugStringA("Hello world");
            Render(Buffer);
            Running = false;
        }
    }

    return (0);
}
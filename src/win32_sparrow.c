// #include <Windows.h>

#include <yak_defines.h>
#include "sparrow_platform.h"
#include "fake_windows.h"

global_variable u8 GlobalRunning = false;

void* __stdcall Win32_DefaultWindowProc(void* Window, u32 Message, u32 WParam, s64 LParam)
{
    void* Result = 0;

    switch (Message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            GlobalRunning = false;
        }
        break;

        case WM_PAINT:
        {
            paint_struct ps;
            void* DeviceContext = BeginPaint(Window, &ps);
            Assert(DeviceContext);
            EndPaint(Window, &ps);
        }
        break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        }
        break;
    }

    return (Result);
}

local void
Win32_ProcessWindowMessages(void)
{
    message Message;
    while (PeekMessageA(&Message, 0, 0, 0, 0x0001))
    {
        switch (Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            }
            break;

            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            }
            break;
        }
    }
}

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
    Win32_ProcessWindowMessages();
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
    void* Instance,
    void* PrevInstance,
    char* CmdLine,
    int ShowCmd)
{
    win_class WindowClass = {0};
    WindowClass.instance = Instance;
    WindowClass.className = "ProjectSparrowWindowClass";
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.cursor = LoadCursorA(0, IDC_ARROW);
    WindowClass.windowProc = Win32_DefaultWindowProc;
    Assert(RegisterClassA(&WindowClass));

    void* Window = CreateWindowExA(0,
                                   WindowClass.className,
                                   "Sparrow",
                                   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   0, 0, Instance, 0);
    Assert(Window);

    struct screen_buffer* Buffer = RequestScreenBuffer(1920, 1080);
    struct user_input* Input = InitializeInput();
    struct memory* Memory = MainMemoryRequest(Kilobytes(10));

    GlobalRunning = true;
    while (GlobalRunning)
    {
        ReadInput(Input);
        UpdateState(Memory, Buffer, Input);

        // TODO: Render asynchronously?
        if (IsTimeToRender())
        {
            Render(Buffer);
        }
        // Running = false;
    }

    return (0);
}
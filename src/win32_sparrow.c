#include <yak_defines.h>
#include "sparrow_platform.h"
#include "fake_windows.h"

global_variable u8 GlobalRunning = true;

local void*
Win32_MemoryAlloc(memory_index Size)
{
    void* Result = VirtualAlloc(0, Size, 0x3000, 0x4); // MEM_RESERVE | MEM_COMMIT, PAGE_READ_WRITE
    Assert(Result);
    return (Result);
}

#include "scratch.c" // TODO: reimplement dummy functions!
#include "win32_input.c"
void* __stdcall Win32_DefaultWindowProc(void* Window, u32 Message, u32 WParam, s64 LParam)
{
    void* Result = 0;

    switch (Message) {

    case WM_SIZE: {
    } break;

    case WM_ACTIVATEAPP: {
    } break;

    case WM_CLOSE:
    case WM_DESTROY: {
        GlobalRunning = false;
    } break;

    case WM_PAINT: {
        paint_struct ps;
        void* DeviceContext = BeginPaint(Window, &ps);
        u32 x = ps.paintRect.left;
        u32 y = ps.paintRect.top;
        u32 width = ps.paintRect.right - x;
        u32 height = ps.paintRect.bottom - y;
        PatBlt(DeviceContext, x, y, width, height, WHITENESS);
        Assert(DeviceContext);
        EndPaint(Window, &ps);
    } break;

    default: {
        Result = DefWindowProcA(Window, Message, WParam, LParam);
    } break;
    }

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

int __stdcall WinMain(void* Instance, void* PrevInstance, char* CmdLine, int ShowCmd)
{
    win_class WindowClass = {0};
    WindowClass.instance = Instance;
    WindowClass.className = "ProjectSparrowWindowClass";
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.cursor = LoadCursorA(0, IDC_ARROW);
    WindowClass.windowProc = Win32_DefaultWindowProc;
    Assert_exec(RegisterClassA(&WindowClass));

    // TODO: show window after all initialized?
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
    while (GlobalRunning) {
        ReadInput(Input);
        UpdateState(Memory, Buffer, Input);

        // TODO: Render asynchronously?
        if (IsTimeToRender()) {
            Render(Buffer);
        }
        // Running = false;
    }

    return (0);
}
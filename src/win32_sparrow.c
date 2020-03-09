#include <yak_defines.h>
#include "sparrow_platform.h"
#include "fake_windows.h"

#include "sparrow.c" // TODO: Move this away!

global_variable u8 GlobalRunning;

global_variable struct frame_buffer Win32_FrameBuffer;
global_variable struct memory* Win32_MainMemory;

inline void*
Win32_MemoryAlloc(memory_index Size)
{
    void* Result = VirtualAlloc(0, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#if SPARROW_DEV
    Assert(Result);
    *((u8*)Result + (Size - 1)) = 0;
#endif

    return (Result);
}

#include "win32_input.c"

local b32
Win32_MemoryFree(void* address)
{
    b32 Result = VirtualFree(address, 0, MEM_RELEASE);
    Assert(Result);
    return (Result);
}

local void
Win32_AllocateFrameBuffer(u16 width, u16 height)
{
    u16 BytesPerPixel = 4;

    if (Win32_FrameBuffer.Pixels) {
        Win32_MemoryFree(Win32_FrameBuffer.Pixels);
    }

    u32 PixelSize = height * width * BytesPerPixel;
    Win32_FrameBuffer.Width = width;
    Win32_FrameBuffer.Height = height;
    Win32_FrameBuffer.BytesPerPixel = BytesPerPixel;
    Win32_FrameBuffer.Pixels = Win32_MemoryAlloc(PixelSize);

    // Render(0, 0, &Win32_FrameBuffer); // TODO: Do we even want to bother?
}

local void
Win32_UpdateBuffer(void* DeviceContext, rect WindowRect)
{
    Assert(Win32_FrameBuffer.Pixels);

    s32 BitmapWidth = Win32_FrameBuffer.Width;
    s32 BitmapHeight = Win32_FrameBuffer.Height;

    u32 WindowWidth = WindowRect.right - WindowRect.left;
    u32 WindowHeight = WindowRect.bottom - WindowRect.top;

    bitmap_info bi = {0};

    bi.header.size = sizeof(bi.header);
    bi.header.width = BitmapWidth;
    bi.header.height = BitmapHeight; // rows will go top-down
    bi.header.planes = 1;
    bi.header.bitsPerPixel = Win32_FrameBuffer.BytesPerPixel * 8;
    bi.header.compression = BI_RGB;

    StretchDIBits(DeviceContext,
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWidth, WindowHeight,
                  Win32_FrameBuffer.Pixels, &bi,
                  DIB_RGB_COLORS, SRCCOPY);
}

void* __stdcall Win32_DefaultWindowProc(void* Window, u32 Message, u32 WParam, s64 LParam)
{
    void* Result = 0;

    switch (Message) {

        case WM_SIZE: {
            rect ClientRect;
            GetClientRect(Window, &ClientRect);
            u16 Width = (u16)(ClientRect.right - ClientRect.left);
            u16 Height = (u16)(ClientRect.bottom - ClientRect.top);
            Win32_AllocateFrameBuffer(Width, Height);
        } break;

        case WM_ACTIVATEAPP: {
        } break;

        case WM_CLOSE: {
            GlobalRunning = false;
        } break;

        case WM_DESTROY: {
            // TODO: Is destroy something that we want happening? Recreate a window?
            GlobalRunning = false;
        } break;

        case WM_PAINT: {
            paint_struct ps;
            void* DeviceContext = BeginPaint(Window, &ps);
            Assert(DeviceContext);

            // u32 x = ps.paintRect.left;
            // u32 y = ps.paintRect.top;
            // u32 width = ps.paintRect.right - x;
            // u32 height = ps.paintRect.bottom - y;
            // rect clientRect;
            // GetClientRect(Window, &clientRect);

            Win32_UpdateBuffer(DeviceContext, ps.paintRect);
            EndPaint(Window, &ps);
        } break;

        default: {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return (Result);
}

local void
Win32_RegisterKeyInput(u64 Key)
{
    switch (Key) {

        case VK_ESCAPE: {
            GlobalRunning = false;
        } break;

        default: {
        } break;
    }
}

local void
Win32_ProcessMessages(void)
{
    message Message;
    while (PeekMessageA(&Message, 0, 0, 0, true)) {
        switch (Message.message) {

            case WM_MOUSEMOVE:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                // TODO: Do we even want to handle any mouse code here?
            } break;

            case WM_SYSKEYUP:
            case WM_KEYUP: {
                Win32_RegisterKeyInput(Message.WParam);
            } break;

            default: {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
}

local struct memory*
Win32_MainMemoryInit(memory_index Size)
{
    struct memory* Result = Win32_MemoryAlloc(Size + sizeof(struct memory));
    Result->Size = Size;
    Result->Data = Result + sizeof(struct memory);

    return (Result);
}

local b32
Win32_IsTimeToRender(void)
{
    return true;
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

    // TODO: implement custom size buffers
    //  struct frame_buffer* Buffer = {0}; //RequestScreenBuffer(1920, 1080);
    struct user_input* Input = InitializeInput();
    // TODO: Allocate main buffer!
    // Win32_AllocateFrameBuffer(u16 width, u16 height);
    Win32_MainMemory = Win32_MainMemoryInit(KiB(10));

    GlobalRunning = true;
    while (GlobalRunning) {
        Win32_ProcessMessages();
        ReadInput(Input);

        // TODO: Render asynchronously?
        if (Win32_IsTimeToRender()) {
            UpdateState(Win32_MainMemory, Input);
            Render(Win32_MainMemory, &Win32_FrameBuffer);

            Assert(Window);
            void* DeviceContext = GetDC(Window);
            Assert(DeviceContext);
            rect WindowRect;
            GetClientRect(Window, &WindowRect);
            Win32_UpdateBuffer(DeviceContext, WindowRect);
            ReleaseDC(Window, DeviceContext);
        }
    }

    return (0);
}
#include <yak_defines.h>
#include "sparrow_platform.h"
#include "fake_windows.h"

global_variable u8 GlobalRunning;

global_variable bitmap_info Win32_BitmapInfo;
global_variable void* Win32_BitmapMemory;
global_variable void* Win32_BitmapHandle;
global_variable void* Win32_DeviceContext;

local void*
Win32_MemoryAlloc(memory_index Size)
{
    void* Result = VirtualAlloc(0, Size, 0x3000, 0x4); // MEM_RESERVE | MEM_COMMIT, PAGE_READ_WRITE
    Assert(Result);
    return (Result);
}

#include "scratch.c" // TODO: reimplement dummy functions!
#include "win32_input.c"

local struct screen_buffer*
Win32_RequestFrameBuffer(u32 width, u32 height)
{
    if (Win32_BitmapHandle) {
        DeleteObject(Win32_BitmapHandle);
    }

    if (!Win32_DeviceContext) {
        Win32_DeviceContext = CreateCompatibleDC(0);
    }

    // u32 PixelSize = height * AlignedWidth(width);
    // struct screen_buffer* Result = Win32_MemoryAlloc(sizeof(struct screen_buffer) + PixelSize);
    // Result->Width = width;
    // Result->Height = height;
    // Result->Pixels = (void*)(Result + sizeof(struct screen_buffer));
    // return (Result);
    Win32_BitmapInfo.header.size = sizeof(Win32_BitmapInfo.header);
    Win32_BitmapInfo.header.width = width;
    Win32_BitmapInfo.header.height = height;
    Win32_BitmapInfo.header.planes = 1;
    Win32_BitmapInfo.header.pixelSize = 32;
    Win32_BitmapInfo.header.compression = BI_RGB;

    Win32_BitmapHandle = CreateDIBSection(Win32_DeviceContext, &Win32_BitmapInfo,
                                          DIB_RGB_COLORS, &Win32_BitmapMemory, 0, 0);
    return 0;
}
local void
Win32_UpdateFrameBuffer(void* DeviceContext, u32 X, u32 Y, u32 Width, u32 Height)
{
    StretchDIBits(DeviceContext, X, Y, Width, Height,
                  X, Y, Width, Height,
                  &Win32_BitmapHandle, &Win32_BitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

void* __stdcall Win32_DefaultWindowProc(void* Window, u32 Message, u32 WParam, s64 LParam)
{
    void* Result = 0;

    switch (Message) {

        case WM_SIZE: {
            rect ClientRect;
            GetClientRect(Window, &ClientRect);
            u32 Width = ClientRect.right - ClientRect.left;
            u32 Height = ClientRect.bottom - ClientRect.top;
            Win32_RequestFrameBuffer(Width, Height);
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

            u32 x = ps.paintRect.left;
            u32 y = ps.paintRect.top;
            u32 width = ps.paintRect.right - x;
            u32 height = ps.paintRect.bottom - y;
            Win32_UpdateFrameBuffer(DeviceContext, x, y, width, height);
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
MainMemoryRequest(memory_index Size)
{
    struct memory* Result = Win32_MemoryAlloc(Size);
    // TODO: shape up memory
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

    struct screen_buffer* Buffer = {0}; //RequestScreenBuffer(1920, 1080);
    struct user_input* Input = InitializeInput();
    struct memory* Memory = MainMemoryRequest(KiB(10));

    GlobalRunning = true;
    while (GlobalRunning) {
        Win32_ProcessMessages();
        ReadInput(Input);
        UpdateState(Memory, Buffer, Input);

        // TODO: Render asynchronously?
        if (IsTimeToRender()) {
            Render(Buffer);
        }
    }

    return (0);
}
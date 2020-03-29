#include <yak_defines.h>
#include "sparrow_platform.h"
#include "fake_windows.h"

#include "sparrow.c" // TODO: Move this away!

global_variable u8 GlobalRunning;

global_variable struct memory* Win32_MainMemory;
global_variable struct frame_buffer Win32_FrameBuffer;

inline void*
Win32_MemoryAlloc(memory_index Size)
{
    void* Result = VirtualAlloc(0, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#if SPARROW_DEV
    Assert(Result);
    Assert(*((u8*)Result + (Size - 1)) == 0);
#endif

    return (Result);
}

local b32
Win32_MemoryFree(void* address)
{
    b32 Result = VirtualFree(address, 0, MEM_RELEASE);
    Assert(Result);
    return (Result);
}

#include "win32_input.c"

inline struct dim_2d
Win32_DimFromRect(rect Source)
{
    struct dim_2d Result;
    Result.Width = (u16)(Source.right - Source.left);
    Result.Height = (u16)(Source.bottom - Source.top);

    return (Result);
}

inline struct dim_2d
Win32_GetWindowDim(void* Window)
{
    Assert(Window);
    rect WindowRect;
    GetClientRect(Window, &WindowRect);

    struct dim_2d Result = Win32_DimFromRect(WindowRect);

    return (Result);
}

local void
Win32_AllocateFrameBuffer(struct frame_buffer* Buffer, u32 Width, u32 Height)
{
    u16 BytesPerPixel = 4;

    if (Buffer->Pixels) {
        Win32_MemoryFree(Buffer->Pixels);
    }

    u32 PixelSize = Height * Width * BytesPerPixel;
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = BytesPerPixel;
    Buffer->Pixels = Win32_MemoryAlloc(PixelSize);

    Assert(Buffer->Pixels);
}

local void
Win32_UpdateBuffer(void* DeviceContext, struct frame_buffer* Buffer, struct dim_2d Window)
{
    Assert(DeviceContext);
    Assert(Buffer->Pixels);
    Assert(Buffer->Width > 0);
    Assert(Buffer->Height > 0);
    Assert(Buffer->BytesPerPixel > 0);

    bitmap_info bi = {0};

    bi.header.size = sizeof(bi.header);
    bi.header.width = Buffer->Width;
    bi.header.height = Buffer->Height; // rows will go top-down
    bi.header.planes = 1;
    bi.header.bitsPerPixel = Buffer->BytesPerPixel * 8;
    bi.header.compression = BI_RGB;

    StretchDIBits(DeviceContext,
                  0, 0, Window.Width, Window.Height,
                  0, 0, Buffer->Width, Buffer->Height,
                  Buffer->Pixels, &bi,
                  DIB_RGB_COLORS, SRCCOPY);
}

void* __stdcall Win32_DefaultWindowProc(void* Window, u32 Message, u32 WParam, s64 LParam)
{
    void* Result = 0;

    switch (Message) {

        case WM_SIZE: {
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
            Assert(Window);
            void* DeviceContext = BeginPaint(Window, &ps);
            Assert(DeviceContext);

            struct dim_2d Dim = Win32_DimFromRect(ps.paintRect);
            Win32_UpdateBuffer(DeviceContext, &Win32_FrameBuffer, Dim);
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
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.cursor = LoadCursorA(0, IDC_ARROW);
    WindowClass.windowProc = Win32_DefaultWindowProc;
    Assert_exec(RegisterClassA(&WindowClass));

    Win32_AllocateFrameBuffer(&Win32_FrameBuffer, 1280, 720);

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
    void* DeviceContext = GetDC(Window);

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
            // NOTE: Game logic
            UpdateState(Win32_MainMemory, Input);
            Render(Win32_MainMemory, &Win32_FrameBuffer);

            // NOTE: Display buffer on screen
            Win32_UpdateBuffer(DeviceContext, &Win32_FrameBuffer, Win32_GetWindowDim(Window));
            ReleaseDC(Window, DeviceContext);
        }
    }

    return (0);
}
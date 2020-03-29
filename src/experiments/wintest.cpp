#include <yak_defines.h>
#include <windows.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
struct frame_buffer
{
    u16 Width;
    u16 Height;
    u16 BytesPerPixel;
    u16 Pad;
    u8* Pixels;
};

static b32 GlobalRunning;
global_variable struct frame_buffer Win32_FrameBuffer;
static s32 Offset;

local void*
Win32_MemoryAlloc(memory_index Size)
{
    void* Result = VirtualAlloc(0, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    Assert(Result);
#if SPARROW_DEV
    *((u8*)Result + (Size - 1)) = 0;
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

local void
Render(frame_buffer* Buffer)
{
    if (Buffer) {
        u32 Pitch = Buffer->Width * Buffer->BytesPerPixel;
        u8* Row = Buffer->Pixels;
        for (u32 y = 0;
             y < Buffer->Height;
             ++y) {
            u32* Pixel = (u32*)Row;
            for (u32 x = 0;
                 x < Buffer->Width;
                 ++x) {
                u8 Red = (u8)(y + Offset);
                u8 Green = (u8)(x + y);
                u8 Blue = (u8)(x + Offset);

                *Pixel++ = ((Red << 0) | (Green << 8) | (Blue << 16) | 0);
            }
            Row += Pitch;
        }
    }
    ++Offset;
}

static void
Win32_RequestFrameBuffer(u16 width, u16 height)
{
    u16 BytesPerPixel = 4;

    if (Win32_FrameBuffer.Pixels) {
        Win32_MemoryFree(Win32_FrameBuffer.Pixels);
    }

    u32 PixelSize = height * width * BytesPerPixel;
    Win32_FrameBuffer.Width = width;
    Win32_FrameBuffer.Height = height;
    Win32_FrameBuffer.BytesPerPixel = BytesPerPixel;
    Win32_FrameBuffer.Pixels = (u8*)Win32_MemoryAlloc(PixelSize);

    Render(&Win32_FrameBuffer);
}

local void
Win32_UpdateBuffer(HDC DeviceContext, RECT WindowRect)
{
    Assert(Win32_FrameBuffer.Pixels);

    s32 BitmapWidth = Win32_FrameBuffer.Width;
    s32 BitmapHeight = Win32_FrameBuffer.Height;

    u32 WindowWidth = WindowRect.right - WindowRect.left;
    u32 WindowHeight = WindowRect.bottom - WindowRect.top;

    BITMAPINFO bi = {0};

    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = BitmapWidth;
    bi.bmiHeader.biHeight = BitmapHeight; // rows will go top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = Win32_FrameBuffer.BytesPerPixel * 8;
    bi.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(DeviceContext,
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWidth, WindowHeight,
                  Win32_FrameBuffer.Pixels, &bi,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT __stdcall Win32_DefaultWindowProc(HWND Window, u32 Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message) {

        case WM_SIZE: {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            u16 Width = (u16)(ClientRect.right - ClientRect.left);
            u16 Height = (u16)(ClientRect.bottom - ClientRect.top);
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

        default: {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return (Result);
}

int CALLBACK
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
        char* CmdLine, int ShowCmd)
{
    WNDCLASS WindowClass = {0};
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "ProjectSparrowWindowClass";
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.hCursor = LoadCursorA(0, IDC_ARROW);
    WindowClass.lpfnWndProc = Win32_DefaultWindowProc;
    Assert_exec(RegisterClassA(&WindowClass));

    // TODO: show window after all initialized?
    HWND Window = CreateWindowExA(0,
                                  WindowClass.lpszClassName,
                                  "Sparrow",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0, 0, Instance, 0);
    Assert(Window);

    GlobalRunning = true;
    while (GlobalRunning) {
        MSG Message;
        while (PeekMessageA(&Message, 0, 0, 0, true)) {
            TranslateMessage(&Message);
            DispatchMessageA(&Message);
        }
        Render(&Win32_FrameBuffer);

        HDC DeviceContext = GetDC(Window);
        RECT WindowRect;
        GetClientRect(Window, &WindowRect);
        Win32_UpdateBuffer(DeviceContext, WindowRect);

        ReleaseDC(Window, DeviceContext);
    }
    return (0);
}
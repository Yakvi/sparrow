#include <yak_defines.h>
#include <windows.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

static b32 GlobalRunning;

static HDC Win32_DeviceContext;
static void* Win32_BitmapMemory;
static HBITMAP Win32_BitmapHandle;
static BITMAPINFO Win32_BitmapInfo;

static void
Win32_RequestFrameBuffer(u16 width, u16 height)
{
    if (Win32_BitmapHandle) {
        DeleteObject(Win32_BitmapHandle);
    }

    if (!Win32_DeviceContext) {
        Win32_DeviceContext = CreateCompatibleDC(0);
    }

    // struct screen_buffer* Result = Win32_MemoryAlloc(sizeof(struct screen_buffer) + PixelSize);
    // Result->Width = width;
    // Result->Height = height;
    // Result->Pixels = (void*)(Result + sizeof(struct screen_buffer));
    // return (Result);
    Win32_BitmapInfo.bmiHeader.biSize = sizeof(Win32_BitmapInfo.bmiHeader);
    Win32_BitmapInfo.bmiHeader.biWidth = width;
    Win32_BitmapInfo.bmiHeader.biHeight = height;
    Win32_BitmapInfo.bmiHeader.biPlanes = 1;
    Win32_BitmapInfo.bmiHeader.biBitCount = 32;
    Win32_BitmapInfo.bmiHeader.biCompression = BI_RGB;

    Win32_BitmapHandle = CreateDIBSection(Win32_DeviceContext, &Win32_BitmapInfo,
                                          DIB_RGB_COLORS, &Win32_BitmapMemory, 0, 0);
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
            switch (Message.message) {

                case WM_MOUSEMOVE:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP: {
                    // TODO: Do we even want to handle any mouse code here?
                } break;

                case WM_SYSKEYUP:
                case WM_KEYUP: {
                    if (Message.wParam == VK_ESCAPE) {
                        GlobalRunning = false;
                    }
                } break;

                default: {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                } break;
            }
        }
    }
    return (0);
}
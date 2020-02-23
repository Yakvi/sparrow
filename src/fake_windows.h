#if !defined(FAKE_WINDOWS)
#ifndef _WINDOWS_

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// NOTE: Window Class flags
enum WindowClass
{
    CS_VREDRAW = 0x1,
    CS_HREDRAW = 0x2,
    CS_OWNDC = 0x40,
    CW_USEDEFAULT = 0x80000000
};

void* __stdcall DefWindowProcA(void* Window, u32 Message, u32 WParam, s64 LParam);
typedef void*(__stdcall* window_proc)(void* Window, u32 Message, u32 WParam, s64 LParam);
typedef struct
{
    u32 style;
    u32 unused;
    window_proc windowProc;
    s64 unused2;
    void* instance;
    void* icon;
    void* cursor;
    void* background;
    char* menuName;
    char* className;
} win_class;

u8* RegisterClassA(win_class* WindowClass);

enum WindowProperties
{
    WS_OVERLAPPED = 0,
    WS_MAXIMIZEBOX = 0x10000L,
    WS_MINIMIZEBOX = 0x20000L,
    WS_THICKFRAME = 0x40000,
    WS_SYSMENU = 0x80000,
    WS_CAPTION = 0xC00000,
    WS_OVERLAPPEDWINDOW = 0xCF0000, // WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    WS_VISIBLE = 0x10000000
};

typedef struct
{
    u32 x;
    u32 y;
} v2, p;

typedef struct
{
    void* window;
    u64 message;
    u64 WParam;
    s64 LParam;
    u32 time;
    v2 point;
    u8 __pad[4];
} message;

typedef struct
{
    long left;
    long top;
    long right;
    long bottom;
} rect;

void* __stdcall CreateWindowExA(u32 ExStyle, char* ClassName, char* WindowName, u32 Style, s32 X, s32 Y, s32 Width, s32 Height, void* ParentWindow, void* Menu, void* Instance, s64 LParam);
b32 __stdcall ShowWindow(void* Window, u32 ShowCmd);

#define IDC_ARROW (u8*)32512
void* __stdcall LoadCursorA(int, u8* CursorRef);

// NOTE: Windows messages
enum WindowMessage
{
    WM_DESTROY = 0x0002,
    WM_SIZE = 0x5,
    WM_CLOSE = 0x10,
    WM_QUIT = 0x12,
    WM_PAINT = 0xF,
    WM_ACTIVATEAPP = 0x1C,
    WM_KEYUP = 0x0101,
    WM_SYSKEYUP = 0x105,
    WM_MOUSEMOVE = 0x200,
    WM_MBUTTONDOWN = 0x207,
    WM_MBUTTONUP = 0x208,
};

b32 __stdcall GetMessageA(message* mess, void* window, u32 unused, u32 unused2);
b32 __stdcall PeekMessageA(message* mess, void* window, u32 unused, u32 unused2, b32 RemoveMessage);
void __stdcall TranslateMessage(message* mess);
void __stdcall DispatchMessageA(message* mess);
void __stdcall PostQuitMessage(int exit_code);
// NOTE: Key presses
enum KeyPress
{
    VK_ESCAPE = 0x1B,
};
enum Mouse
{
    MK_LBUTTON = 0x1,
    MK_MBUTTON = 0x10,
    MK_RBUTTON = 0x2,
};
// NOTE: Painting
enum RasterOp
{
    BLACKNESS = 0x00000042,
    WHITENESS = 0x00FF0062
};
typedef struct
{
    void* hdc;
    b32 eraseBg;
    rect paintRect;
    b32 reserved0;
    b32 reserved1;
    u8 reserved2[36];
} paint_struct;

void* __stdcall BeginPaint(void* Window, paint_struct* PaintStruct);
void* __stdcall EndPaint(void* Window, paint_struct* PaintStruct);
b32 PatBlt(void* Hdc, s32 X, s32 Y, s32 Width, s32 Height, u64 RasterOp);

// NOTE: Memory
enum MemoryFlags
{
    PAGE_READWRITE = 0x4,
    MEM_RESERVE = 0x2000,
    MEM_COMMIT = 0x1000,
};
void* __stdcall VirtualAlloc(void* address, memory_index size, u32 type, u32 protect);

// NOTE: Debug calls
#if defined(SPARROW_DEV)
void __stdcall OutputDebugStringA(char* string);
#else
#define OutputDebugStringA
#endif

#endif // !_WINDOWS_
#define FAKE_WINDOWS
#endif

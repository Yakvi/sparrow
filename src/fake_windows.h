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
    u32 left;
    u32 top;
    u32 right;
    u32 bottom;
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
    SRCCOPY = 0x00CC0020,
    WHITENESS = 0x00FF0062
};

enum DIBColors
{
    DIB_RGB_COLORS = 0x00,
    DIB_PAL_COLORS = 0x01,
    DIB_PAL_INDICES = 0x02
};

enum BitmapCompression
{
    BI_RGB = 0,
    BI_RLE8 = 1,
    BI_RLE4 = 2,
    BI_BITFIELDS = 3,
    BI_JPEG = 4,
    BI_PNG = 5,
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

typedef struct
{
    u32 type;
    u32 width;
    u32 height;
    u32 widthBytes;
    u32 planes;
    u32 bitsPerPixel;
    void* data;
} bitmap;

typedef struct
{
    u32 size;
    u32 width;
    u32 height;
    u16 planes;
    u16 pixelSize;
    u32 compression;
    u32 sizeImage;
    u32 xPPM;
    u32 yPPM;
    u32 clrUsed;
    u32 clrImportant;
} bitmap_info_header;

typedef struct
{
    u8 blue;
    u8 green;
    u8 red;
    u8 reserved;
} rbg_quad;

typedef struct
{
    bitmap_info_header header;
    rbg_quad colors[1];
} bitmap_info;

void* __stdcall BeginPaint(void* Window, paint_struct* PaintStruct);
void* __stdcall EndPaint(void* Window, paint_struct* PaintStruct);
b32 __stdcall GetClientRect(void* Window, rect* OutRect);
b32 __stdcall PatBlt(void* HDC, s32 X, s32 Y, s32 Width, s32 Height, u64 RasterOp);
void* __stdcall CreateDIBSection(void* HDC, bitmap_info* info, u32 mode, void** bits, void* section, u32 offset);
s32 __stdcall StretchDIBits(void* HDC, u32 xDest, u32 yDest, u32 destWidth, u32 destHeight, u32 xSrc, u32 ySrc, u32 srcWidth, u32 srcHeight, void* bits, bitmap_info* bitsInfo, u32 mode, u32 rop);
b32 __stdcall DeleteObject(void* GDIHandle);
void* __stdcall CreateCompatibleDC(s32);
s32 __stdcall ReleaseDC(void* Window, void* HDC);
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
#define OutputDebugStringA(string)
#endif

#endif // !_WINDOWS_
#define FAKE_WINDOWS
#endif

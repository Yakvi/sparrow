#if !defined(FAKE_WINDOWS)
#ifndef _WINDOWS_

typedef s64(__stdcall* window_proc)(void* Window, u32 Message, u32 WParam, s64 LParam);
s64 __stdcall DefWindowProcA(void* Window, u32 Message, u32 WParam, s64 LParam);

// NOTE: Entry handles
#define CS_OWNDC 0x40
#define CS_VREDRAW 0x1
#define CS_HREDRAW 0x2
#define CW_USEDEFAULT ((int)0x80000000)

struct win_class
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
};
u8* RegisterClassA(const struct win_class* WindowClass);

#define WS_OVERLAPPED 0x00000000L
#define WS_POPUP 0x80000000L
#define WS_CHILD 0x40000000L
#define WS_MINIMIZE 0x20000000L
#define WS_VISIBLE 0x10000000L
#define WS_DISABLED 0x08000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_CLIPCHILDREN 0x02000000L
#define WS_MAXIMIZE 0x01000000L
#define WS_CAPTION 0x00C00000L /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER 0x00800000L
#define WS_DLGFRAME 0x00400000L
#define WS_VSCROLL 0x00200000L
#define WS_HSCROLL 0x00100000L
#define WS_SYSMENU 0x00080000L
#define WS_THICKFRAME 0x00040000L
#define WS_GROUP 0x00020000L
#define WS_TABSTOP 0x00010000L

#define WS_MINIMIZEBOX 0x00020000L
#define WS_MAXIMIZEBOX 0x00010000L

#define WS_TILED WS_OVERLAPPED
#define WS_ICONIC WS_MINIMIZE
#define WS_SIZEBOX WS_THICKFRAME
#define WS_TILEDWINDOW WS_OVERLAPPEDWINDOW

/*
 * Common Window Styles
 */
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED |  \
                             WS_CAPTION |     \
                             WS_SYSMENU |     \
                             WS_THICKFRAME |  \
                             WS_MINIMIZEBOX | \
                             WS_MAXIMIZEBOX)

struct v2
{
    u32 x;
    u32 y;
};

struct message
{
    void* window;
    u32 message;
    u32 WParam;
    s64 LParam;
    u32 time;
    struct v2 point;
    u32 unused;
};

void* __stdcall CreateWindowExA(u32 ExStyle, char* ClassName, char* WindowName, u32 Style, s32 X, s32 Y, s32 Width, s32 Height, void* ParentWindow, void* Menu, void* Instance, s64 LParam);
b32 __stdcall ShowWindow(void* Window, u32 ShowCmd);

b32 __stdcall GetMessageA(struct message* mess, void* window, u32 unused, u32 unused2);
void __stdcall TranslateMessage(struct message* mess);
void __stdcall DispatchMessageA(struct message* mess);

// NOTE: Memory
#define MEM_RESERVE 0x2000
#define MEM_COMMIT 0x1000
#define PAGE_READWRITE 0x4
void* __stdcall VirtualAlloc(void* address, memory_index size, u32 type, u32 protect);

// NOTE: Debug calls
#if defined(SPARROW_DEV)
void __stdcall OutputDebugStringA(char* string);
#else
#define OutputDebugStringA
#endif

#endif // !WINDOWS
#define FAKE_WINDOWS
#endif

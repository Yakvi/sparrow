#include "win32_sparrow.h"

/**
 * GLOBAL PLATFORM LAYER TODO LIST
 * (to be considered barebones)
 *
 * NOTE: Graphics
 * Fullscreen toggle
 * Fixed Framerate
 * Hardware acceleration
 *
 * NOTE: Game Update
 * DeltaTime for frame
 *
 * NOTE: Engine tech
 * Multithreading
 * Multiple DLL handling
 *    - DLL reloading only if unloaded or DLL changed
 *    - DLL resources shop
 *    - Higher level modules
 *    - Arbitrary DLL load
 *    - Arbitrary loading sequence
 *    - Production-level hot reloading (e.g. for mod development)
 * Advanced input capture system (Mouse, Keyboard, Gamepad)
 * Audio reproduction system
 * Asset loading
 *    - Lazy asset loading
 *
*/

global_variable u8 GlobalRunning;

#define PATH_BUFFER_LENGTH STR_MAX
global_variable struct frame_buffer Win32_FrameBuffer;

#define BIT_BLT_PATH 1
#if BIT_BLT_PATH
global_variable void* BitmapDeviceContext;
global_variable void* BitmapHandle;
#endif

inline dim_2i
Win32_DimFromRect(rect Source)
{
    dim_2i Result;
    Result.Width  = Source.right - Source.left;
    Result.Height = Source.bottom - Source.top;

    return (Result);
}

inline dim_2i
Win32_GetClientDim(void* Window)
{
    Assert(Window);
    rect WindowRect;
    GetClientRect(Window, &WindowRect);

    dim_2i Result = Win32_DimFromRect(WindowRect);

    return (Result);
}

#include "win32_memory.c"
#include "win32_input.c"

local void
Win32_AllocateFrameBuffer(struct frame_buffer* Buffer, dim_2i Dim)
{
    // Assert(Dim);
    if (Dim.Width > 0 && Dim.Height > 0) {
        u16 BytesPerPixel = 4;

        Buffer->Width         = (u16)Dim.Width;
        Buffer->Height        = (u16)Dim.Height;
        Buffer->BytesPerPixel = BytesPerPixel;
        Buffer->Pitch         = (u16)Dim.Width * BytesPerPixel;
#if BIT_BLT_PATH
        if (!BitmapDeviceContext) {
            BitmapDeviceContext = CreateCompatibleDC(0);
            Assert(BitmapDeviceContext);
        }
        bitmap_info bi = {0};

        bi.header.size         = sizeof(bi.header);
        bi.header.width        = Buffer->Width;
        bi.header.height       = Buffer->Height; // rows will go top-down
        bi.header.planes       = 1;
        bi.header.bitsPerPixel = Buffer->BytesPerPixel * 8;
        bi.header.compression  = BI_RGB;

        BitmapHandle = CreateDIBSection(BitmapDeviceContext, &bi, DIB_RGB_COLORS, &Buffer->Pixels, 0, 0);
        Assert(BitmapHandle);
#else
        if (Buffer->Pixels) {
            Win32_MemoryFree(Buffer->Pixels);
        }
        u32 PixelSize  = Dim.Height * Dim.Width * BytesPerPixel;
        Buffer->Pixels = Win32_MemoryAlloc(PixelSize);
#endif
        Assert(Buffer->Pixels);
    }
}

local void
Win32_UpdateBuffer(void* DeviceContext, struct frame_buffer* Buffer, dim_2i Window)
{
    Assert(DeviceContext);
    Assert(Buffer->Pixels);
    Assert(Buffer->Width > 0);
    Assert(Buffer->Height > 0);
    Assert(Buffer->BytesPerPixel > 0);

#if BIT_BLT_PATH
    Assert(BitmapDeviceContext);
    Assert(BitmapHandle);
    SelectObject(BitmapDeviceContext, BitmapHandle);
    u32 BltResult = BitBlt(DeviceContext, 0, 0, Buffer->Width, Buffer->Height,
                           BitmapDeviceContext, 0, 0, SRCCOPY);
    BltResult     = 0;
#else
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
#endif
}

void* __stdcall Win32_DefaultWindowProc(void* Window, u32 Message, u32 WParam, s64 LParam)
{
    void* Result = 0;

    switch (Message) {

        case WM_SIZE: {
            // TODO: Do we even want dynamic buffer resolution?
            Win32_AllocateFrameBuffer(&Win32_FrameBuffer, Win32_GetClientDim(Window));
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

            dim_2i Dim = Win32_DimFromRect(ps.paintRect);
            Win32_UpdateBuffer(DeviceContext, &Win32_FrameBuffer, Dim);
            EndPaint(Window, &ps);
        } break;

        default: {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return (Result);
}

local b32
Win32_IsTimeToRender(void)
{
    return true;
}

local b32
Win32_FileExists(char* Filename)
{
    s32 Attributes = GetFileAttributesA(Filename);
    b32 Result     = Attributes != -1;

    return (Result);
}

local void
Win32_TryLoadDLL(char*                Lockfile,
                 struct win32_module* Module,
                 b32                  IsMod)
{
    b32 LockFileExists = Win32_FileExists(Lockfile);
    if (!LockFileExists) {
        if (!Module->IsLoaded) {
            Assert(Module->Name);
            Module->Library = LoadLibraryA(Module->Name);
            if (Module->Library) {
                // TODO(yakvi): Compress this into an array
                if (IsMod) {
                    Module->Main = GetProcAddress(Module->Library, "ModuleMain");
                }
                else {
                    Module->Main   = GetProcAddress(Module->Library, "UpdateState");
                    Module->Render = GetProcAddress(Module->Library, "Render");
                }
            }
        }
    }
    else {
        if (Module->Library && FreeLibrary(Module->Library)) {
            Module->Library = 0;
            Module->Main    = 0;
            Module->Render  = 0;
        }
    }
    if (IsMod) {
        Module->IsLoaded = Module->Main != 0;
    }
    else {
        Module->IsLoaded = (Module->Library && Module->Main && Module->Render);
    }
}

local struct text_buffer
Win32_AllocatePathBuffer(char* Input)
{
    char*              Buffer = Win32_MemoryAlloc(PATH_BUFFER_LENGTH);
    struct text_buffer Result = Text(0, PATH_BUFFER_LENGTH, Buffer);
    if (Input) {
        TextConcat(&Result, Input);
    }
    return (Result);
}

local struct win32_module
Win32_InitModule(struct text_buffer* ModuleDirectory, char* Filename)
{
    struct win32_module Result = {0};
    struct text_buffer  Path   = Win32_AllocatePathBuffer(ModuleDirectory->Data);
    TextConcat(&Path, Filename);
    Assert(Path.Length > 0);
    Result.Name = Path.Data;

    return (Result);
}

local struct text_buffer
Win32_GetWorkingDirectory()
{
    struct text_buffer Result = Win32_AllocatePathBuffer(0);
    Result.Length             = GetCurrentDirectoryA(Result.MaxLength, Result.Data); // TODO: Replace this with wide function!
    Assert(Result.Length > 0);
    return (Result);
}

local struct text_buffer
Win32_GetModuleDirectory(char* WorkingDirectory)
{
    struct text_buffer Result = Win32_AllocatePathBuffer(WorkingDirectory);
    Assert(Result.Length > 0);
#if SPARROW_DEV
    TextConcat(&Result, "\\build\\");
#else
    // TODO: Copy core inside the main exe?
    // TextConcat(&Result, "\\Modules\\");
    TextConcat(&Result, "\\build\\");
    // TextConcat(&Result, "\\");
#endif SPARROW_DEV

    return (Result);
}

inline s64
Win32_GetWallClock()
{
    s64 Result;
    QueryPerformanceCounter(&Result);

    return (Result);
}

local struct platform*
Win32_InitializePlatform(void)
{
    struct platform* Result  = Win32_MemoryAlloc(sizeof(struct platform));
    Result->Input            = Win32_InitializeInput();
    Result->FrameDeltaMs     = 0.0f;
    Result->FrameDeltaCycles = 0;

    return (Result);
}

#define MULTITHREADING_DEV 0
#if MULTITHREADING_DEV
u32 __stdcall Test(void* Param)
{
    char* Input = (char*)Param;
    OutputDebugStringA(Input);
    return (0);
}
#endif

int __stdcall WinMain(void* Instance, void* PrevInstance, char* CmdLine, int ShowCmd)
{
#if MULTITHREADING_DEV
    char* Param = "Test thread\n";
    u32   ThreadId;
    void* ThreadHandle = CreateThread(0, 0, Test, Param, 0, &ThreadId);
    CloseHandle(ThreadHandle);
#endif

    win_class WindowClass  = {0};
    WindowClass.instance   = Instance;
    WindowClass.className  = "ProjectSparrowWindowClass";
    WindowClass.style      = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.cursor     = LoadCursorA(0, IDC_ARROW); // TODO: We'll need to investigate removing cursor and implementing our own at some point
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
    void* DeviceContext = GetDC(Window);

    Win32_AllocateFrameBuffer(&Win32_FrameBuffer, Win32_GetClientDim(Window));
    // Win32_AllocateFrameBuffer(&Win32_FrameBuffer, 1280, 720); // TODO(yakvi): Fixed window dim?

    Win32_MainMemory             = Win32_MemoryInit(GiB(6));
    struct platform* PlatformAPI = Win32_InitializePlatform();

    struct text_buffer WorkingDirectory = Win32_GetWorkingDirectory();
    struct text_buffer ModuleDirectory  = Win32_GetModuleDirectory(WorkingDirectory.Data);
    struct text_buffer LockfilePath     = Win32_AllocatePathBuffer(ModuleDirectory.Data);
    TextConcat(&LockfilePath, "lock.tmp");

    struct win32_module Game = Win32_InitModule(&ModuleDirectory, "sparrow.dll");

    // char* Modname = "mod_everscroll.dll";
    // char* Modname = "mod_writer.dll";
    char*               Modname = "mod_weekend.dll";
    struct win32_module Mod     = Win32_InitModule(&ModuleDirectory, Modname);

    s64 CyclesPerSecond;
    QueryPerformanceFrequency(&CyclesPerSecond);
    s64 FrameStartTime = Win32_GetWallClock();
    s64 FrameEndTime   = Win32_GetWallClock();

    GlobalRunning = true;
    while (GlobalRunning) {

        Win32_TryLoadDLL(LockfilePath.Data, &Game, false);
        Win32_TryLoadDLL(LockfilePath.Data, &Mod, true);

        Win32_ReadInput(Window, PlatformAPI->Input);

        FrameEndTime = Win32_GetWallClock();
        if (Game.IsLoaded && Mod.IsLoaded) {
            PlatformAPI->FrameDeltaCycles = (u32)(FrameEndTime - FrameStartTime);
            PlatformAPI->FrameDeltaMs     = ((f32)PlatformAPI->FrameDeltaCycles / CyclesPerSecond) * 1000;
            FrameStartTime                = FrameEndTime;
            // TODO: Render asynchronously?
            if (Win32_IsTimeToRender()) {
                // NOTE: Game logic
                // TODO: Run this for each module
                Game.Main(Win32_MainMemory, PlatformAPI, Mod.Main);
                Game.Render(Win32_MainMemory, &Win32_FrameBuffer);
            }

            FrameEndTime = Win32_GetWallClock();
        }
        else {
#if SPARROW_DEV
            OutputDebugStringA("Module not loaded: ");
            OutputDebugStringA(Modname);
            OutputDebugStringA("\n");
#endif
            // NOTE(yakvi): Game not loaded (waiting for DLL)
            // Assert(!"Game DLL not loaded!");
            // TODO(yakvi): Some softer logging maybe?
        }
        // NOTE: Display buffer on screen
        Win32_UpdateBuffer(DeviceContext, &Win32_FrameBuffer, Win32_GetClientDim(Window));
    }

    return (0);
}

void
WinMainCRTStartup(void)
{
    char BigArray[4096];
    BigArray[0]  = 0;
    u32 ExitCode = WinMain(GetModuleHandleA(0), 0, 0, 0);
    ExitProcess(ExitCode);
}
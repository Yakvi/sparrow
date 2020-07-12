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

inline struct dim_2d
Win32_DimFromRect(rect Source)
{
    struct dim_2d Result;
    Result.Width = Source.right - Source.left;
    Result.Height = Source.bottom - Source.top;

    return (Result);
}

inline struct dim_2d
Win32_GetClientDim(void* Window)
{
    Assert(Window);
    rect WindowRect;
    GetClientRect(Window, &WindowRect);

    struct dim_2d Result = Win32_DimFromRect(WindowRect);

    return (Result);
}

#include "win32_memory.c"
#include "win32_input.c"

local void
Win32_AllocateFrameBuffer(struct frame_buffer* Buffer, struct dim_2d Dim)
{
    // Assert(Dim);
    if (Dim.Width > 0 && Dim.Height > 0) {
        u16 BytesPerPixel = 4;

        if (Buffer->Pixels) {
            Win32_MemoryFree(Buffer->Pixels);
        }

        u32 PixelSize = Dim.Height * Dim.Width * BytesPerPixel;
        Buffer->Width = Dim.Width;
        Buffer->Height = Dim.Height;
        Buffer->BytesPerPixel = BytesPerPixel;
        Buffer->Pitch = Dim.Width * BytesPerPixel;
        Buffer->Pixels = Win32_MemoryAlloc(PixelSize);

        Assert(Buffer->Pixels);
    }
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

local b32
Win32_IsTimeToRender(void)
{
    return true;
}

local b32
Win32_FileExists(char* Filename)
{
    s32 Attributes = GetFileAttributesA(Filename);
    b32 Result = Attributes != -1;

    return (Result);
}

local void
Win32_TryLoadDLL(char* Lockfile,
                 struct win32_module* Module,
                 b32 IsMod)
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
                    Module->Main = GetProcAddress(Module->Library, "UpdateState");
                    Module->Render = GetProcAddress(Module->Library, "Render");
                }
            }
        }
    }
    else {
        if (Module->Library && FreeLibrary(Module->Library)) {
            Module->Library = 0;
            Module->Main = 0;
            Module->Render = 0;
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
    char* Buffer = Win32_MemoryAlloc(PATH_BUFFER_LENGTH);
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
    struct text_buffer Path = Win32_AllocatePathBuffer(ModuleDirectory->Data);
    TextConcat(&Path, Filename);
    Assert(Path.Length > 0);
    Result.Name = Path.Data;

    return (Result);
}

local struct text_buffer
Win32_GetWorkingDirectory()
{
    struct text_buffer Result = Win32_AllocatePathBuffer(0);
    Result.Length = GetCurrentDirectoryA(Result.MaxLength, Result.Data); // TODO: Replace this with wide function!
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
    TextConcat(&Result, "\\");
#endif SPARROW_DEV

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
    u32 ThreadId;
    void* ThreadHandle = CreateThread(0, 0, Test, Param, 0, &ThreadId);
    CloseHandle(ThreadHandle);
#endif

    win_class WindowClass = {0};
    WindowClass.instance = Instance;
    WindowClass.className = "ProjectSparrowWindowClass";
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.cursor = LoadCursorA(0, IDC_ARROW); // TODO: We'll need to investigate removing cursor and implementing our own at some point
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

    struct user_input* Input = InitializeInput();

    Win32_AllocateFrameBuffer(&Win32_FrameBuffer, Win32_GetClientDim(Window));
    // Win32_AllocateFrameBuffer(&Win32_FrameBuffer, 1280, 720); // TODO(yakvi): Fixed window dim?

    Win32_MainMemory = Win32_MainMemoryInit(MiB(200));

    struct text_buffer WorkingDirectory = Win32_GetWorkingDirectory();
    struct text_buffer ModuleDirectory = Win32_GetModuleDirectory(WorkingDirectory.Data);
    struct text_buffer LockfilePath = Win32_AllocatePathBuffer(ModuleDirectory.Data);
    TextConcat(&LockfilePath, "lock.tmp");

    struct win32_module Game = Win32_InitModule(&ModuleDirectory, "sparrow.dll");
    struct win32_module Mod = Win32_InitModule(&ModuleDirectory, "mod_weekend.dll");

    GlobalRunning = true;
    while (GlobalRunning) {
        // TODO(yakvi): DeltaTime calculations!
        Win32_TryLoadDLL(LockfilePath.Data, &Game, false);
        Win32_TryLoadDLL(LockfilePath.Data, &Mod, true);

        Win32_ReadInput(Window, Input);
        if (Game.IsLoaded && Mod.IsLoaded) {
            // TODO: Render asynchronously?
            if (Win32_IsTimeToRender()) {
                // NOTE: Game logic
                // TODO: Run this for each module
                Game.Main(Win32_MainMemory, Input, Mod.Main);
                Game.Render(Win32_MainMemory, &Win32_FrameBuffer);
            }
        }
        else {
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
    BigArray[0] = 0;
    u32 ExitCode = WinMain(GetModuleHandleA(0), 0, 0, 0);
    ExitProcess(ExitCode);
}
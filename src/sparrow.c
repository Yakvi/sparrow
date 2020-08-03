// #include "writer/writer.c"
#include "sparrow.h"

#include "core/sparrow_core_render.c"
#include "console/console_render.c"
#include "console/console_update.c"

global_variable struct game_state StubGameState;

// BOOKMARK: Game Update

local struct game_state*
LoadGameState(struct memory* Memory)
{
    struct game_state* Result = (struct game_state*)Memory->Data;
    if (Result) {
        Assert(Memory->Size > sizeof(struct game_state));
        if (!Result->IsInitialized) {
            Result = GetStruct(Memory, struct game_state);
            InitConsole(&Result->Console, 140, 50, Console_BottomUp, Color_Black);
            Result->Player.Pos = V2F(0, 0);

            Result->UpdateCompleted = false;
            Result->IsInitialized   = true;
            Result->ModuleMemory    = MemoryInit(Memory, GiB(5));
        }
    }
    else {
        InvalidCodePath;
        Result = &StubGameState;
    }

    return (Result);
}

local void
ShowFPSCounter(struct console* Console, f32 MsDelta, u32 Cycles)
{
    char   MSCount[100];
    color3 FPSState = Color_Red;
    if (MsDelta < 16.0f) {
        FPSState = Color_Green;
    }
    else if (MsDelta < 33.0f) {
        FPSState = Color_Yellow;
    }
    FormatText(MSCount, "MS: %.1f, FPS: %.3f, Cycles: %$_u", MsDelta, 1000 / MsDelta, Cycles);
    TextBox(Console, V2I(0, 0), FPSState, MSCount, Color_Black);
}

local void
ShowMemoryCounter(struct console* Console, struct memory* ModuleMemory, dim_2i Pos, char* Label)
{
    char   MemoryBuf[100];
    color3 MemoryState   = Color_Red;
    f32    InvMemorySize = 1.0f / ModuleMemory->Size;
    f32    MemoryDelta   = (ModuleMemory->Size - ModuleMemory->Used) * InvMemorySize;
    if (MemoryDelta > 0.30f) {
        MemoryState = Color_Green;
    }
    else if (MemoryDelta > 0.15f) {
        MemoryState = Color_Yellow;
    }
    FormatText(MemoryBuf, "%s Memory: %$_I64d, Used: %$_I64d", Label, ModuleMemory->Size, ModuleMemory->Used);
    TextBox(Console, Pos, MemoryState, MemoryBuf, Color_Black);
}

#include "mods/tests/console_tests.c"

void
UpdateState(struct memory*   Memory,
            struct platform* Platform,
            console_module   Main)
{
    // NOTE: This is a "console" grid.
    // We need to be able to set each "pixel" with a specific colour
    // Then, at the render phase, these would be painted as squares.
    struct game_state* GameState = LoadGameState(Memory);
    struct user_input* Input     = Platform->Input;
    if (!GameState->UpdateCompleted) {
        struct console* Console = &GameState->Console;
        ClearConsole(Console, Input);

        VerticalGradient(Console, RGB(20, 130, 200), RGB(6, 146, 180));

#if 1
        Main(GameState->ModuleMemory, Input, Console);
#else
        f32 AspectRatio = 16.0f / 9.0f;
        s32 ImageWidth  = 400;
        s32 ImageHeight = (s32)(ImageWidth / AspectRatio);
        InitConsole(Console, ImageWidth, ImageHeight, Console_BottomUp, Color_White);
        VerticalGradient(Console, RGB(20, 130, 200), RGB(6, 146, 180));
#endif

        // NOTE: "Post-processing"
        u32 PixelOrder = Console->PixelOrder;
        SetConsoleMode(Console, Console_TopDown);
#if 0
        RunConsoleTests(Console, Input);
#endif

        ShowFPSCounter(Console, Platform->FrameDeltaMs, Platform->FrameDeltaCycles);
        // ShowMemoryCounter(Console, Memory, V2I(0, 10), "Main");
        // ShowMemoryCounter(Console, GameState->ModuleMemory, V2I(0, 20), "Module");
        SetConsoleMode(Console, PixelOrder);

// #define RUNONCE 1
#ifdef RUNONCE
        GameState->UpdateCompleted = true;
#endif
    }
}

// BOOKMARK: Render

void
Render(struct memory* Memory, struct frame_buffer* Buffer)
{
    struct game_state* GameState = LoadGameState(Memory);

    Clear(Buffer, Color_Pink);
    DrawAllPixels(Buffer, &GameState->Console);
}
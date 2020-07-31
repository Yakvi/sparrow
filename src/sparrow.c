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
            Result->ModuleMemory    = MemoryInit(Memory, MiB(100));
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
    color3 FPSPixel = Color_Red;
    if (MsDelta < 16.0f) {
        FPSPixel = Color_Green;
    }
    else if (MsDelta < 33.0f) {
        FPSPixel = Color_Yellow;
    }
    FormatText(MSCount, "MS: %.1f, FPS: %.1f, Cycles: %'u", MsDelta, 1000 / MsDelta, Cycles);
    TextBox(Console, V2I(0, 0), FPSPixel, MSCount, Color_Black);
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
// #include "writer/writer.c"
#include "sparrow.h"

#include "core/sparrow_core_render.c"
#include "console/console_render.c"
#include "console/console_update.c"
#include "console/console_tests.c"

global_variable struct game_state StubGameState;

// BOOKMARK: Game Update

local struct game_state*
LoadGameState(struct memory* Memory)
{
    struct game_state* Result = 0;
    if (Memory) {
        Assert(Memory->Size > sizeof(struct game_state));
        Result = (struct game_state*)Memory->Data;
        if (!Result->IsInitialized) {
            InitConsole(&Result->Console, (v3)Color_Black);
            Result->Player.Pos = (v2f){0, 0};

            Result->UpdateCompleted = false;
            Result->IsInitialized = true;
        }
    }
    else {
        Result = &StubGameState;
    }

    return (Result);
}

void
UpdateState(struct memory* Memory,
            struct user_input* Input,
            console_module Main)
{
    // NOTE: This is a "console" grid.
    // We need to be able to set each "pixel" with a specific colour
    // Then, at the render phase, these would be painted as squares.
    struct game_state* GameState = LoadGameState(Memory);
    if (!GameState->UpdateCompleted) {
        struct console* Console = &GameState->Console;
        ClearConsole(Console, Input);

        struct pixel* Pixels = Console->Pixels;
        VerticalGradient(Pixels, (color){20, 130, 200}, (color){6, 146, 180});

#if 1
        Main(&GameState->ModuleMemory, Input, Pixels);
#endif
#if 0
#include "everscroll/everscroll.c"
        EverScroll(&GameState->Scroll, Input, Pixels);
#endif
        // RunConsoleTests(Console, Input);
        // RaytracingWeekend(Pixels);

        // #define RUNONCE 1
        // #ifdef RUNONCE
        //         GameState->UpdateCompleted = true;
        // #endif
    }
}

// BOOKMARK: Render

void
Render(struct memory* Memory, struct frame_buffer* Buffer)
{
    struct game_state* GameState = LoadGameState(Memory);

    Clear(Buffer, (v3)Color_Pink);
    DrawAllPixels(Buffer, GameState->Console.Pixels);
}
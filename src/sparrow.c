#include "sparrow.h"

#include "core/sparrow_core_render.c"
#include "console/sparrow_console_render.c"
#include "console/sparrow_console_update.c"

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
            InitConsolePixels(Result, (v3)Color_Black);
            Result->Player.Pos = (v2){0, 0};

            Result->IsInitialized = true;
        }
    }
    else {
        Result = &StubGameState;
    }

    return (Result);
}

local b32
MovePlayer(struct pixel* Pixels, struct player* Player, v2 Offset)
{
    b32 Result = true;

    AddV2(&Player->Pos, Offset);
    v2i Pos = RoundV2ToV2(Player->Pos);

    if (Pos.x >= CONSOLE_WIDTH) {
        Player->Pos.x = 0;
        Pos.x = 0;
    }
    else if (Pos.x < 0) {
        Player->Pos.x = CONSOLE_WIDTH - 1;
        Pos.x = CONSOLE_WIDTH - 1;
    }

    if (Pos.y >= CONSOLE_HEIGHT) {
        Player->Pos.y = 0;
        Pos.y = 0;
    }
    else if (Pos.y < 0) {
        Player->Pos.y = CONSOLE_HEIGHT - 1;
        Pos.y = CONSOLE_HEIGHT - 1;
    }

    struct pixel* PlayerPixel = GetPixel(Pixels, Pos);
    PlayerPixel->Color = (v3)Color_Gray05;

    return (Result);
}

void
UpdateState(struct memory* Memory,
            struct user_input* Input)
{
    // NOTE: This is a "console" grid.
    // We need to be able to set each "pixel" with a specific colour
    // Then, at the render phase, these would be painted as squares.
    struct game_state* GameState = LoadGameState(Memory);
    InitConsolePixels(GameState, (v3)Color_Cyan);
    PixelOverlay(GameState->Pixels);

    SetStructuredArt(GameState->Pixels);

    MovePlayer(GameState->Pixels, &GameState->Player, Input->MovementKeys);
}

// BOOKMARK: Render

void
Render(struct memory* Memory, struct frame_buffer* Buffer)
{
    struct game_state* GameState = LoadGameState(Memory);

    Clear(Buffer, (v3)Color_Pink);
    DrawAllPixels(Buffer, GameState->Pixels);
}

#if _WIN32
// NOTE(yakvi): Required entry point for all win32 dlls (with stripped down items)
b32 __stdcall _DllMainCRTStartup()
{
    return true;
}
#endif
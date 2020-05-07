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
    v2i Pos = RoundV2ToV2i(Player->Pos);

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

local void
PixelOverlay(struct pixel* Pixels)
{
    struct pixel* Row = Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {
            // TODO: Clamp!
            Pixel++->Color.r += (f32)(Y % 2) * 70;
        }
        Row += CONSOLE_WIDTH;
    }
}

local void
SetStructuredArt(struct pixel* Pixels)
{
    GetPixel(Pixels, (v2i){0, 0})->Color = (v3)Color_Yellow;
    GetPixel(Pixels, (v2i){(CONSOLE_WIDTH - 1), 0})->Color = (v3)Color_Red;
    GetPixel(Pixels, (v2i){(CONSOLE_WIDTH - 1), (CONSOLE_HEIGHT - 1)})->Color = (v3)Color_Blue;
    GetPixel(Pixels, (v2i){0, (CONSOLE_HEIGHT - 1)})->Color = (v3)Color_Black;

    PrintString(Pixels, "A string here", (v2i){10, 10}, (v3)Color_Black);
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

    PrintString(GameState->Pixels, "Cursor", RoundV2ToV2i(GameState->Player.Pos), (v3)Color_Black);
    // char At = 'a' + (char)RoundF32ToInt(GameState->Player.Pos.x);
    // PrintString(GameState->Pixels, &At, (v2i){10, 20}, (v3)Color_Black);
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
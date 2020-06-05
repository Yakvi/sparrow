#include "sparrow.h"

#include "core/sparrow_core_render.c"
#include "console/sparrow_console_render.c"
#include "console/sparrow_console_update.c"
#include "everscroll/everscroll.c"

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

    Player->Pos = AddV2(Player->Pos, Offset);
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

void
DemoFont(struct pixel* Pixels)
{
    PrintString(Pixels, "THE QUICK BROWN FOX JUMPS OVER LAZY DOG", (v2i){10, 10}, (v3){0x55, 0xDD, 0xFF});
    PrintString(Pixels, "the quick brown fox jumps over lazy dog", (v2i){10, 20}, (v3){0x55, 0xDD, 0xFF});
}

void
DemoShowCursor(struct pixel* Pixels, v2i CursorPos)
{
    Assert(Pixels);
    if (CursorPos.x < CONSOLE_WIDTH && CursorPos.x >= 0 &&
        CursorPos.y < CONSOLE_HEIGHT && CursorPos.y >= 0) {
        GetPixel(Pixels, CursorPos)->Color = (color)Color_White;
    }
}

void
UpdateState(struct memory* Memory,
            struct user_input* Input)
{
    // NOTE: This is a "console" grid.
    // We need to be able to set each "pixel" with a specific colour
    // Then, at the render phase, these would be painted as squares.
    struct game_state* GameState = LoadGameState(Memory);
    struct console* Console = &GameState->Console;
    InitConsole(Console, (v3)Color_Cyan);

    struct pixel* Pixels = Console->Pixels;
    VerticalGradient(Pixels, (color){6, 146, 180}, (color){153, 255, 0});

#if 0
    // NOTE: Everscroll module
     s32 ScreenId = GetScreenId((s32)Input->MovementKeys.y, &GameState->Scroll);
    LoadScreen(Pixels, ScreenId);
#endif

#if 0
    DemoFont(Pixels);
#endif

    v2i CursorPos;
    CursorPos.x = RoundF32ToInt(Input->CursorNorm.x * CONSOLE_WIDTH);
    CursorPos.y = RoundF32ToInt(Input->CursorNorm.y * CONSOLE_HEIGHT);
    DemoShowCursor(Pixels, CursorPos);

#if 0
    // NOTE: cursor test
    MovePlayer(Pixels, &GameState->Player, Input->MovementKeys);
    PrintString(Pixels, "Cursor", RoundV2ToV2i(GameState->Player.Pos), (v3)Color_Black);
    // char At = 'a' + (char)RoundF32ToInt(GameState->Player.Pos.x);
    // PrintString(Pixels, &At, (v2i){10, 20}, (v3)Color_Black);
#endif
}

// BOOKMARK: Render

void
Render(struct memory* Memory, struct frame_buffer* Buffer)
{
    struct game_state* GameState = LoadGameState(Memory);

    Clear(Buffer, (v3)Color_Pink);
    DrawAllPixels(Buffer, GameState->Console.Pixels);
}

#if _WIN32
// NOTE(yakvi): Required entry point for all win32 dlls (with stripped down items)
b32 __stdcall _DllMainCRTStartup()
{
    return true;
}
#endif
#include "sparrow.h"

#include "core/sparrow_core_render.c"
#include "console/sparrow_console_render.c"
#include "console/sparrow_console_update.c"
// #include "everscroll/everscroll.c"
#include "writer/writer.c"

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

            Result->IsInitialized = true;
        }
    }
    else {
        Result = &StubGameState;
    }

    return (Result);
}

local b32
DemoKeyboardCursor(struct pixel* Pixels, struct player* Player, v2f Offset)
{
    b32 Result = true;

    Player->Pos = AddV2(Player->Pos, Offset);

    V2fGridClamp(&Player->Pos);
    Point(Pixels, RoundV2ToV2i(Player->Pos), (color)Color_Gray05);

    return (Result);
}

void
DemoFont(struct pixel* Pixels)
{
    PrintString(Pixels, "THE QUICK BROWN FOX JUMPS OVER LAZY DOG", (v2i){10, 10}, (v3){0x55, 0xDD, 0xFF});
    PrintString(Pixels, "the quick brown fox jumps over lazy dog", (v2i){10, 20}, (v3){0x55, 0xDD, 0xFF});
}

void
DemoMouseCursor(struct console* Console)
{
    struct pixel* Pixel = GetPixel(Console->Pixels, Console->CursorPos);
    Assert(Pixel->Flags & Pixel_Hovered);
    Pixel->Color = (color)Color_White;
}

void
DemoBoxes(struct pixel* Pixels, v2i CursorPos)
{
    Box(Pixels, (v2i){10, 10}, (dim_2d){15, 8}, (color)Color_Gray13);
    TextBox(Pixels, (v2i){10, 20}, (color)Color_Gray11, "I'm boxxy", (color)Color_Black);
    if (Button(Pixels, CursorPos, AddV2i((v2i){0, -20}, (v2i)CONSOLE_CENTER), "Hover me!")) {
        PrintString(Pixels, "YAY!", (v2i)CONSOLE_CENTER, (color)Color_Black);
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
    ClearConsole(Console, Input);

    struct pixel* Pixels = Console->Pixels;
    VerticalGradient(Pixels, (color){20, 130, 200}, (color){6, 146, 180});

#if 1
    WriterSim(&GameState->Writer, Input, Pixels);
#endif
#if 0
    EverScroll(&GameState->Scroll, Input, Pixels);
#endif
#if 0
    Point(Pixels, (v2i){5, 5}, (color)Color_Cyan);
#endif
#if 0
    // TODO
    Line(Pixels, (v2i){5, 7}, 10, Line_Vertical, (color)Color_Red);
    Line(Pixels, (v2i){7, 7}, 10, Line_Horizontal, (color)Color_Red);
#endif
#if 1
    DemoBoxes(Pixels, Console->CursorPos);
#endif
#if 0
    DemoFont(Pixels);
#endif
#if 1
    DemoMouseCursor(Console);
#endif
#if 0
    DemoKeyboardCursor(Pixels, &GameState->Player, Input->MovementKeys);
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
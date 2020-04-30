#include "sparrow.h"

global_variable struct game_state StubGameState;

// BOOKMARK: Game Update

local void
InitConsolePixels(struct game_state* GameState, color Color)
{
    Assert(GameState);
    struct pixel* Row = GameState->Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {
            Assert(Pixel);
            Pixel->Pos = (v2u){X, Y};
            Pixel->Color = Color;
            ++Pixel;
        }
        Row += CONSOLE_WIDTH;
    }
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

local struct game_state*
LoadGameState(struct memory* Memory)
{
    struct game_state* Result = 0;
    if (Memory) {
        Assert(Memory->Size > sizeof(struct game_state));
        Result = (struct game_state*)Memory->Data;
        if (!Result->IsInitialized) {
            InitConsolePixels(Result, (v3)Color_Black);
            Result->Player.Pos = (v2u){0, 0};

            Result->IsInitialized = true;
        }
    }
    else {
        Result = &StubGameState;
    }

    return (Result);
}

local struct pixel*
GetPixel(struct pixel* Pixels, v2u Coords)
{
    Assert(Coords.x < CONSOLE_WIDTH);
    Assert(Coords.y < CONSOLE_HEIGHT);
    u32 Y = CONSOLE_WIDTH * Coords.y;
    u32 Pos = Y + Coords.x;

    Assert(Pos < CONSOLE_SIZE);

    struct pixel* Result = Pixels + Pos;

    Assert(Result->Pos.x == Coords.x);
    Assert(Result->Pos.y == Coords.y);

    return (Result);
}

#define MAP_WIDTH 79
#define MAP_HEIGHT 8
const char map[] = {
    "#     #                                    #     #                             "
    "#     # ###### #      #       ####         #  #  #  ####  #####  #      #####  "
    "#     # #      #      #      #    #        #  #  # #    # #    # #      #    # "
    "####### #####  #      #      #    #        #  #  # #    # #    # #      #    # "
    "#     # #      #      #      #    # ###    #  #  # #    # #####  #      #    # "
    "#     # #      #      #      #    # ###    #  #  # #    # #   #  #      #    # "
    "#     # ###### ###### ######  ####   #      ## ##   ####  #    # ###### #####  "
    "                                    #                                          "};

local void
SetStructuredArt(struct pixel* Pixels)
{
    Pixels[0].Color = (v3)Color_Yellow;
    Pixels[CONSOLE_WIDTH - 1].Color = (v3)Color_Red;
    Pixels[CONSOLE_HEIGHT * CONSOLE_WIDTH - 1].Color = (v3)Color_Blue;
    Pixels[CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH].Color = (v3)Color_Black;

    for (u32 Y = 0;
         Y < MAP_HEIGHT;
         ++Y) {
        for (u32 X = 0;
             X < MAP_WIDTH;
             ++X) {
            if (map[MAP_WIDTH * Y + X] == '#') {
                GetPixel(Pixels, (v2u){X + 10, Y + 10})->Color = (v3)Color_Blue;
            }
        }
    }
}

void
UpdateState(struct memory* Memory,
            struct user_input* Input)
{
    // NOTE: Let's try today making a "console" grid.
    // We need to be able to set each "square" with a specific colour
    // Then, at the render phase, these would be painted as squares.
    struct game_state* GameState = LoadGameState(Memory);
    InitConsolePixels(GameState, (v3)Color_Cyan);
    PixelOverlay(GameState->Pixels);

    SetStructuredArt(GameState->Pixels);

    struct player* Player = &GameState->Player;
    if (Player->Pos.x < CONSOLE_WIDTH - 1) {
        ++Player->Pos.x;
    }
    else {
        Player->Pos.x = 0;
    }
    struct pixel* PlayerPixel = GetPixel(GameState->Pixels, Player->Pos);
    PlayerPixel->Color = (v3)Color_Gray05;
}

// BOOKMARK: Render

local void
Clear(struct frame_buffer* Buffer, color Color)
{
    if (Buffer) {
        u32 Pitch = Buffer->Width * Buffer->BytesPerPixel;
        u8* Row = Buffer->Pixels;
        for (u32 y = 0;
             y < Buffer->Height;
             ++y) {
            u32* Pixel = (u32*)Row;
            for (u32 x = 0;
                 x < Buffer->Width;
                 ++x) {
                u8 Red = (u8)Color.r;
                u8 Green = (u8)Color.g;
                u8 Blue = (u8)Color.b;

                *Pixel++ = ((Red << 16) | (Green << 8) | Blue | 0);
            }
            Row += Pitch;
        }
    }
}

local void
DrawPixel(struct frame_buffer* Buffer, struct pixel* Pixel, dim PixelSize)
{
    if (Buffer && Pixel) {
        u32 Pitch = Buffer->Width * Buffer->BytesPerPixel;

        Assert(Pixel->Pos.x >= 0);
        Assert(Pixel->Pos.y >= 0);
        Assert(Pixel->Pos.x < CONSOLE_WIDTH);
        Assert(Pixel->Pos.y < CONSOLE_HEIGHT);

        u32 PixelBaseX = (u32)(Pixel->Pos.x * PixelSize.Width);
        // NOTE(yakvi): The rows are laid out in memory bottom up. We want to go top-down
        u32 PixelBaseY = (u32)((CONSOLE_HEIGHT - 1 - Pixel->Pos.y) * PixelSize.Height);

        v2u PixelPos = {
            PixelBaseX + (s32)PixelSize.Width,
            PixelBaseY + (s32)PixelSize.Height};

        // NOTE(yakvi): Anti-overflow measures
        s32 OverflowX = PixelPos.x - Buffer->Width;
        if (OverflowX > 0) PixelBaseX -= OverflowX;

        s32 OverflowY = PixelPos.y - Buffer->Height;
        if (OverflowY > 0) PixelBaseY -= OverflowY;

        Assert(PixelPos.x <= Buffer->Width);
        Assert(PixelPos.y <= Buffer->Height);

        u8* Row = Buffer->Pixels + PixelBaseY * Pitch;
        for (u32 Y = 0;
             Y < PixelSize.Height;
             ++Y) {
            u32* ScreenPixel = (u32*)Row + PixelBaseX;
            for (u32 X = 0;
                 X < PixelSize.Width;
                 ++X) {
                *ScreenPixel++ = (((u8)Pixel->Color.r << 16) |
                                  ((u8)Pixel->Color.g << 8) |
                                  (u8)Pixel->Color.b | 0);
            }
            Row += Pitch;
        }
    }
}

local void
DrawAllPixels(struct frame_buffer* Buffer, struct pixel* Pixels)
{
    dim PixelSize = {
        (f32)Buffer->Width / (f32)CONSOLE_WIDTH,
        (f32)Buffer->Height / (f32)CONSOLE_HEIGHT};

    struct pixel* Row = Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {

            Assert(Pixel);
            // TODO: This can be easily job-ified
            DrawPixel(Buffer, Pixel++, PixelSize);
        }
        Row += CONSOLE_WIDTH;
    }
}

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
#include "sparrow_console_update.h"

local struct pixel*
GetPixel(struct pixel* Pixels, v2i Coords)
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
            Pixel->Pos = (v2i){X, Y};
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
                GetPixel(Pixels, (v2i){X + 10, Y + 10})->Color = (v3)Color_Blue;
            }
        }
    }
}

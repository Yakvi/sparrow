#include "sparrow_console_platform.h"

local void
InitConsole(struct console* Console, color Color)
{
    Assert(Console);
    struct pixel* Row = Console->Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {
            Assert(Pixel);
            Pixel->Pos = (v2i){X, (CONSOLE_HEIGHT - 1 - Y)};
            Pixel->Color = Color;
            ++Pixel;
        }
        Row += CONSOLE_WIDTH;
    }
}

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
    Assert(Result->Pos.y == CONSOLE_HEIGHT - 1 - Coords.y);

    return (Result);
}

local void
PaintPixel(struct pixel* Pixels, v2i Pos, v3 Color)
{
    GetPixel(Pixels, Pos)->Color = Color;
}

#include "sparrow_console_glyphs.h"

local void
PrintGlyph(struct pixel* Pixels, char* Char, v2i Pos, v3 Color)
{
    struct glyph Glyph = GetGlyph(Char);
    for (u32 Y = 0;
         Y < Glyph.Dim.Height;
         ++Y) {
        for (u32 X = 0;
             X < Glyph.Dim.Width;
             ++X) {
            if (Glyph.Data[Glyph.Dim.Width * Y + X] == '#') {
                PaintPixel(Pixels, (v2i){Pos.x + X, Pos.y + Y}, Color);
            }
        }
    }
}

local void
VerticalGradient(struct pixel* Pixels, color Start, color End)
{
    // f32 InvertedWidth = 1.0f / CONSOLE_WIDTH;
    f32 InvertedHeight = 1.0f / CONSOLE_HEIGHT;

    struct pixel* Row = Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        color Color = ColorLerp(Start, End, (f32)Y * InvertedHeight);
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {

            // color NewColor = ColorLerp(Color, End, (f32)X * InvertedWidth);
            Pixel++->Color = Color;
        }
        Row += CONSOLE_WIDTH;
    }
}

local void
PrintString(struct pixel* Pixels, char* String, v2i Pos, v3 Color)
{
    if ((Pos.y + GLYPH_HEIGHT) < CONSOLE_HEIGHT) {
        for (char* At = String;
             *At;
             ++At) {
            if ((Pos.x + GLYPH_WIDTH) >= CONSOLE_WIDTH)
                break;

            PrintGlyph(Pixels, At, Pos, Color);
            Pos.x += GLYPH_WIDTH;
        }
    }
}
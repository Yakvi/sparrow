#include "sparrow_console_platform.h"
#include "sparrow_console_glyphs.h"

local void
InitConsole(struct console* Console, color Color)
{
    struct pixel* Row = Console->Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {

            Assert(Pixel);
            if (!Console->IsInitialized) {
                Pixel->Pos = (v2i){X, (CONSOLE_HEIGHT - 1 - Y)};
            }
            Pixel->Color = Color;
            Pixel->Flags = 0;
            ++Pixel;
        }
        Row += CONSOLE_WIDTH;
    }
    Console->CursorPos = (v2i){0, 0};
    Console->IsInitialized = true;
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
ClearConsole(struct console* Console, struct user_input* Input)
{
    Assert(Console->IsInitialized);
    InitConsole(Console, (color)Color_Pink);

    v2i CursorPos;
    CursorPos.x = (s32)(Input->CursorNorm.x * CONSOLE_WIDTH);
    CursorPos.y = (s32)(Input->CursorNorm.y * CONSOLE_HEIGHT);
    // NOTE: Cursor clamping
    V2iGridClamp(&CursorPos);

    Console->CursorPos = CursorPos;
    GetPixel(Console->Pixels, Console->CursorPos)->Flags = Pixel_Hovered;
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

inline void
Point(struct pixel* Pixels, v2i Pos, v3 Color)
{
    GetPixel(Pixels, Pos)->Color = Color;
}

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
                Point(Pixels, (v2i){Pos.x + X, Pos.y + Y}, Color);
            }
        }
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

enum LineDirection
{
    Line_Horizontal,
    Line_Vertical,
    Line_Diagonal
};

local void
Line(struct pixel* Pixels, v2i Left, u32 Length, u32 Direction, color Color)
{
#if 1
    Assert(Direction == Line_Vertical || Direction == Line_Horizontal);

    for (u32 Offset = 0;
         Offset < Length;
         ++Offset) {
        if (Direction == Line_Vertical) {
            Point(Pixels, AddV2i(Left, (v2i){0, Offset}), Color);
        }
        else {
            Point(Pixels, AddV2i(Left, (v2i){Offset, 0}), Color);
        }
    }
#else
    for (s32 Y = Start.y;
         Y < End.y;
         ++Y) {
        for (s32 X = Start.x;
             X < End.x;
             ++X) {
            // TODO: For now this is good only for strictly diagonal lines!!!
            s32 XLength = (End.x - Start.x) * (X - Start.x);
            s32 YLength = (End.y - Start.y) * (Y - Start.y);

            if (!(XLength - YLength)) {
                Point(Pixels, (v2i){X, Y}, Color);
            }
        }
    }
#endif
}

local void
Box(struct pixel* Pixels, v2i TopLeft, dim_2d Dim, color BoxColor)
{
    Assert(Dim.Width >= 0);
    Assert(Dim.Height >= 0);
    for (u32 Offset = 0;
         Offset < Dim.Height;
         ++Offset) {
        Line(Pixels, AddV2i(TopLeft, (v2i){0, Offset}), Dim.Width, Line_Horizontal, BoxColor);
    }
    Line(Pixels, TopLeft, Dim.Width, Line_Horizontal, (color)Color_White);
    Line(Pixels, TopLeft, Dim.Height, Line_Vertical, (color)Color_White);
    Line(Pixels, AddV2i(TopLeft, (v2i){Dim.Width, 0}), Dim.Height, Line_Vertical, (color)Color_Black);
    Line(Pixels, AddV2i(TopLeft, (v2i){0, Dim.Height}), Dim.Width + 1, Line_Horizontal, (color)Color_Black);
}

local dim_2d
TextBox(struct pixel* Pixels, v2i TopLeft, color BoxColor, char* Input, color StringColor)
{
    dim_2d Dim = {4 + GLYPH_WIDTH * StringLength(Input), 3 + GLYPH_HEIGHT};
    Box(Pixels, TopLeft, Dim, BoxColor);
    PrintString(Pixels, Input, AddV2i(TopLeft, (v2i){2, 2}), StringColor);
    return (Dim);
}

local b32
Button(struct pixel* Pixels, v2i CursorPos, v2i TopLeft, char* Input)
{
    b32 Result = false;

    dim_2d Dim = TextBox(Pixels, TopLeft, (color)Color_Gray13, Input, (color)Color_Black);
    if ((CursorPos.x >= TopLeft.x) && (CursorPos.x < (TopLeft.x + (s32)Dim.Width)) &&
        (CursorPos.y >= TopLeft.y) && (CursorPos.y < (TopLeft.y + (s32)Dim.Height))) {
            
        Line(Pixels, TopLeft, Dim.Width, Line_Horizontal, (color)Color_Black);
        Line(Pixels, TopLeft, Dim.Height, Line_Vertical, (color)Color_Black);
        Line(Pixels, AddV2i(TopLeft, (v2i){Dim.Width, 0}), Dim.Height, Line_Vertical, (color)Color_White);
        Line(Pixels, AddV2i(TopLeft, (v2i){0, Dim.Height}), Dim.Width + 1, Line_Horizontal, (color)Color_White);
        Result = true;
    }

    return (Result);
}
#include "console_platform.h"
#include "console_glyphs.h"
#include "sparrow_text.h"

local void
InitConsole(struct console* Console, u32 Width, u32 Height, color3 Color)
{
    Console->IsInitialized = false;

    Console->Size = V2I(Width, Height);
    Console->Length = Console->Size.Width * Console->Size.Height;
    Console->Center = V2I((s32)(Console->Size.Width * 0.5f),
                          (s32)(Console->Size.Height * 0.5f));
    Console->CursorPos = V2I(0, 0);

    struct pixel* Row = Console->Pixels;
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        struct pixel* Pixel = Row;
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {

            Assert(Pixel);
            if (!Console->IsInitialized) {
                Pixel->Pos = V2I(X, (Console->Size.Height - 1 - Y));
            }
            Pixel->Color = Color;
            Pixel->Flags = 0;
            ++Pixel;
        }
        Row += Console->Size.Width;
    }
    Console->IsInitialized = true;
}

local struct pixel*
GetPixel(struct console* Console, v2i Coords)
{
    Assert(Coords.x >= 0);
    Assert(Coords.y >= 0);
    Assert(Coords.x < Console->Size.Width);
    Assert(Coords.y < Console->Size.Height);

    u32 Y = Console->Size.Width * Coords.y;
    u32 Pos = Y + Coords.x;

    Assert(Pos < Console->Length);

    struct pixel* Result = Console->Pixels + Pos;

    Assert(Result->Pos.x == Coords.x);
    Assert(Result->Pos.y == Console->Size.Height - 1 - Coords.y);

    return (Result);
}

local void
ClearConsole(struct console* Console, struct user_input* Input)
{
    Assert(Console->IsInitialized);

    v2i CursorPos;
    CursorPos.x = (s32)(Input->CursorNorm.x * Console->Size.Width);
    CursorPos.y = (s32)(Input->CursorNorm.y * Console->Size.Height);
    // NOTE: Cursor clamping
    V2iGridClamp(Console, &CursorPos);

    Console->CursorPos = CursorPos;
    GetPixel(Console, Console->CursorPos)->Flags = Pixel_Hovered;
}

local void
VerticalGradient(struct console* Console, color3 Start, color3 End)
{
    struct pixel* Pixels = Console->Pixels;
    // f32 InvertedWidth = 1.0f / Console->Size.Width;
    f32 InvertedHeight = 1.0f / Console->Size.Height;

    struct pixel* Row = Console->Pixels;
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        struct pixel* Pixel = Row;
        color3 Color = ColorLerp(Start, End, (f32)Y * InvertedHeight);
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {

            // color3 NewColor = ColorLerp(Color, End, (f32)X * InvertedWidth);
            Pixel++->Color = Color;
        }
        Row += Console->Size.Width;
    }
}

inline void
Point(struct console* Console, v2i Pos, v3 Color)
{
    Assert(Pos.x >= 0);
    Assert(Pos.y >= 0);
    Assert(Pos.x < Console->Size.Width);
    Assert(Pos.y < Console->Size.Height);

    GetPixel(Console, Pos)->Color = Color;
}

local void
PrintGlyph(struct console* Console, char* Char, v2i Pos, v3 Color)
{
    struct glyph Glyph = GetGlyph(Char);
    for (s32 Y = 0;
         Y < Glyph.Dim.Height;
         ++Y) {
        for (s32 X = 0;
             X < Glyph.Dim.Width;
             ++X) {
            if (Glyph.Data[Glyph.Dim.Width * Y + X] == '#') {
                Point(Console, V2I(Pos.x + X, Pos.y + Y), Color);
            }
        }
    }
}

local void
PrintString(struct console* Console, char* String, v2i Pos, v3 Color)
{
    if ((Pos.y + GLYPH_HEIGHT) < Console->Size.Height) {
        for (char* At = String;
             *At;
             ++At) {
            if ((Pos.x + GLYPH_WIDTH) >= Console->Size.Width)
                break;

            PrintGlyph(Console, At, Pos, Color);
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
Line(struct console* Console, v2i Left, u32 Length, u32 Direction, color3 Color)
{
#if 1
    Assert(Direction == Line_Vertical || Direction == Line_Horizontal);

    v2i Pos = Left;
    for (s32 Offset = 0;
         Offset < (s32)Length;
         ++Offset) {
        // Offset
        if (Direction == Line_Vertical) {
            ++Pos.y;
        }
        else {
            ++Pos.x;
        }
        // Try to draw
        if ((Pos.x < Console->Size.Width) &&
            ((Pos.y < Console->Size.Height))) {
            Point(Console, Pos, Color);
        }
    }
#else
    // TODO: Arbitrary lines
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
Box(struct console* Console, v2i TopLeft, dim_2i Dim, color3 BoxColor)
{
    Assert(Dim.Width >= 0);
    Assert(Dim.Height >= 0);
    for (s32 Offset = 0;
         Offset < Dim.Height;
         ++Offset) {
        Line(Console, AddV2i(TopLeft, V2I(0, Offset)), Dim.Width, Line_Horizontal, BoxColor);
    }
    Line(Console, TopLeft, Dim.Width, Line_Horizontal, Color_White);
    Line(Console, TopLeft, Dim.Height, Line_Vertical, Color_White);
    Line(Console, AddV2i(TopLeft, V2I(Dim.Width, 0)), Dim.Height, Line_Vertical, Color_Black);
    Line(Console, AddV2i(TopLeft, V2I(0, Dim.Height)), Dim.Width + 1, Line_Horizontal, Color_Black);
}

local dim_2i
TextBox(struct console* Console, v2i TopLeft, color3 BoxColor, char* Input, color3 StringColor)
{
    dim_2i Dim = {(s32)(4 + GLYPH_WIDTH * StringLength(Input)), (s32)(3 + GLYPH_HEIGHT)};
    Box(Console, TopLeft, Dim, BoxColor);
    PrintString(Console, Input, AddV2i(TopLeft, V2I(2, 2)), StringColor);
    return (Dim);
}

local b32
ButtonHover(struct console* Console, v2i CursorPos, v2i TopLeft, char* Input)
{
    b32 Result = false;

    dim_2i Dim = TextBox(Console, TopLeft, Color_Gray13, Input, Color_Black);
    if ((CursorPos.x >= TopLeft.x) && (CursorPos.x < (TopLeft.x + (s32)Dim.Width)) &&
        (CursorPos.y >= TopLeft.y) && (CursorPos.y < (TopLeft.y + (s32)Dim.Height))) {
        TextBox(Console, TopLeft, RGB(0xCC, 0xCC, 0xCC), Input, Color_Black);
        // Line(Console, TopLeft, Dim.Width, Line_Horizontal, Color_Black);
        // Line(Console, TopLeft, Dim.Height, Line_Vertical, Color_Black);
        // Line(Console, AddV2i(TopLeft, V2I(Dim.Width, 0)), Dim.Height, Line_Vertical, (color)Color_White);
        // Line(Console, AddV2i(TopLeft, V2I(0, Dim.Height)), Dim.Width + 1, Line_Horizontal, (color)Color_White);
        Result = true;
    }

    return (Result);
}
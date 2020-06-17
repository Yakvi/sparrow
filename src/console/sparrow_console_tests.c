#include "sparrow_console_platform.h"

local b32
DemoKeyboardCursor(struct pixel* Pixels, v2f Pos, v2f Offset)
{
    b32 Result = true;

    Pos = AddV2(Pos, Offset);

    V2fGridClamp(&Pos);
    Point(Pixels, RoundV2ToV2i(Pos), (color)Color_Gray05);

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
    Box(Pixels, (v2i){10, 30}, (dim_2d){15, 8}, (color)Color_Gray13);
    TextBox(Pixels, (v2i){10, 40}, (color)Color_Gray11, "I'm boxxy", (color)Color_Black);
    if (ButtonHover(Pixels, CursorPos, (v2i){10, 60}, "Hover me!")) {
        PrintString(Pixels, "YAY!", (v2i){10, 80}, (color)Color_Black);
    }
}

local void
RunConsoleTests(struct console* Console, struct user_input* Input)
{
    struct pixel* Pixels = Console->Pixels;

#if 1
    Point(Pixels, (v2i){5, 5}, (color)Color_Cyan);
#endif
#if 1
    // TODO
    Line(Pixels, (v2i){5, 7}, 10, Line_Vertical, (color)Color_Red);
    Line(Pixels, (v2i){7, 7}, 10, Line_Horizontal, (color)Color_Red);
#endif
#if 1
    DemoBoxes(Pixels, Console->CursorPos);
#endif
#if 1
    DemoFont(Pixels);
#endif
#if 1
    DemoMouseCursor(Console);
#endif
#if 1
    DemoKeyboardCursor(Pixels, (v2f){3, 3}, Input->MovementKeys);
#endif
}
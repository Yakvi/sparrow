#include "console/console_platform.h"

local b32
DemoKeyboardCursor(struct console* Console, v2f Pos, v2f Offset)
{
    b32 Result = true;

    Pos = AddV2f(Pos, Offset);

    V2fGridClamp(Console, &Pos);
    Point(Console, V2fToV2i(Pos), Color_Gray05);

    return (Result);
}

void
DemoFont(struct console* Console)
{
    PrintString(Console, "THE QUICK BROWN FOX JUMPS OVER LAZY DOG", V2I(10, 10), RGB(0x55, 0xDD, 0xFF));
    PrintString(Console, "the quick brown fox jumps over lazy dog", V2I(10, 20), RGB(0x55, 0xDD, 0xFF));
    char* TestString = "Text formatting test";
    char  Buffer[120];
    PrintString(Console, TestString, V2I(10, 100), Color_Black);

    FormatText(Buffer, "Int formatting test: %i", -10);
    PrintString(Console, Buffer, V2I(10, 110), Color_Black);

    FormatText(Buffer, "UInt formatting test: %u", 10);
    PrintString(Console, Buffer, V2I(10, 120), Color_Black);

    FormatText(Buffer, "Float formatting test: %0.10f", Pi32);
    PrintString(Console, Buffer, V2I(10, 130), Color_Black);

    FormatText(Buffer, "Double formatting test: %0.12f", Pi64);
    PrintString(Console, Buffer, V2I(10, 140), Color_Black);
}

void
DemoMouseCursor(struct console* Console)
{
    struct pixel* Pixel = GetPixel(Console, Console->CursorPos);
    if (Pixel) {
        // Assert(Pixel->Flags & Pixel_Hovered);
        Pixel->Color = Color_White;
    }
}

void
DemoBoxes(struct console* Console, v2i CursorPos)
{
    Box(Console, V2I(10, 30), DIM_2I(15, 8), Color_Gray13);
    TextBox(Console, V2I(10, 40), Color_Gray11, "I'm boxxy", Color_Black);
    if (ButtonHover(Console, CursorPos, V2I(10, 60), "Hover me!")) {
        PrintString(Console, "YAY!", V2I(10, 80), Color_Black);
    }
}

local void
RunConsoleTests(struct console* Console, struct user_input* Input)
{
#if 1
    Point(Console, V2I(5, 5), Color_Cyan);
#endif
#if 1
    // TODO
    Line(Console, V2I(5, 7), 10, Line_Vertical, Color_Red);
    Line(Console, V2I(7, 7), 10, Line_Horizontal, Color_Red);
#endif
#if 1
    DemoBoxes(Console, Console->CursorPos);
#endif
#if 1
    DemoFont(Console);
#endif
#if 1
    DemoMouseCursor(Console);
#endif
#if 1
    DemoKeyboardCursor(Console, V2F(3, 3), Input->MovementKeys);
#endif
}
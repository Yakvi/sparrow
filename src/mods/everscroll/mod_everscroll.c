#include "everscroll.h"
#include "console/console_update.c"

local void
PixelOverlay(struct console* Console)
{
    struct pixel* Row = Console->Pixels;
    b32 IsRowOdd = false;
    for (u32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        struct pixel* Pixel = Row;
        IsRowOdd = Y % 2;

        for (u32 X = 0;
             X < Console->Size.Width;
             ++X) {
            // TODO: Clamp!
            // Pixel->Color.b = (f32)(X % 2) * 255;
            Pixel++->Color.r += (f32)(IsRowOdd)*200;
        }
        Row += Console->Size.Width;
    }
}

local void
SetStructuredArt(struct console* Console)
{
    GetPixel(Console, V2I(0, 0))->Color = Color_Yellow;
    GetPixel(Console, V2I((Console->Size.Width - 1), 0))->Color = Color_Red;
    GetPixel(Console, V2I((Console->Size.Width - 1), (Console->Size.Height - 1)))->Color = Color_Blue;
    GetPixel(Console, V2I(0, (Console->Size.Width - 1)))->Color = Color_Black;

    PrintString(Console, "EVERSCROLL", SubV2i(Console->Center, V2I((8 * 5), (8))), Color(0x55, 0xDD, 0xFF));
}

local void
TextInCenterWithShadow(struct console* Console, char* Input, color TextColor)
{
    struct text_buffer Title = Text(Input, 0, 0);
    v2i TitleCenter = SubV2i(Console->Center, V2I(Title.Length * 4, 8));

    PrintString(Console, Title.Data, AddV2i(TitleCenter, (v2i){1, 1}), Color_Black);
    PrintString(Console, Title.Data, TitleCenter, Color(0x55, 0xDD, 0xFF));
}

local void
LoadScreen(struct console* Console, s32 ScreenIndex)
{
    switch (ScreenIndex % SCREEN_COUNT) {
        case 0: {
            VerticalGradient(Console, Color_Blue, Color_Black);
            TextInCenterWithShadow(Console, "EVERSCROLL", Color(0x55, 0xDD, 0xFF));
        } break;

        case 1: {
            VerticalGradient(Console, Color_Black, Color_Blue);
            // TextInCenterWithShadow(Console, "EVERSCROLL", Color(0x55, 0xDD, 0xFF));
        } break;

        case 2: {
            VerticalGradient(Console, Color_Blue, Color_Green);
            // TextInCenterWithShadow(Console, "EVERSCROLL", Color(0x55, 0xDD, 0xFF));
        } break;

        case 3: {
            VerticalGradient(Console, Color_Green, Color_White);
            // TextInCenterWithShadow(Console, "EVERSCROLL", Color(0x55, 0xDD, 0xFF));
        } break;

        case 4: {

            // TextInCenterWithShadow(Console, "EVERSCROLL", Color(0x55, 0xDD, 0xFF));
        } break;

        case SCREEN_COUNT: {
            // NOTE: Debug
            PixelOverlay(Console);
            SetStructuredArt(Console);
        } break;

            InvalidDefaultCase;
    }
}

local s32
GetScreenId(s32 Input, struct everscroll_state* Scroll)
{
    s32 Result = Scroll->CurrentScreen + Input;

    if (Result < 0) {
        Result = SCREEN_COUNT - 1;
    }
    Scroll->CurrentScreen = Result;

    return (Result);
}

MODULE_MAIN(struct everscroll_state* Scroll)
{
    s32 ScreenId = GetScreenId((s32)Input->MovementKeys.y, Scroll);
    LoadScreen(Console, ScreenId);
}

#include "everscroll.h"

local void
PixelOverlay(struct pixel* Pixels)
{
    struct pixel* Row = Pixels;
    b32 IsRowOdd = false;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        IsRowOdd = Y % 2;

        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {
            // TODO: Clamp!
            // Pixel->Color.b = (f32)(X % 2) * 255;
            Pixel++->Color.r += (f32)(IsRowOdd)*200;
        }
        Row += CONSOLE_WIDTH;
    }
}

local void
SetStructuredArt(struct pixel* Pixels)
{
    GetPixel(Pixels, (v2i){0, 0})->Color = (v3)Color_Yellow;
    GetPixel(Pixels, (v2i){(CONSOLE_WIDTH - 1), 0})->Color = (v3)Color_Red;
    GetPixel(Pixels, (v2i){(CONSOLE_WIDTH - 1), (CONSOLE_HEIGHT - 1)})->Color = (v3)Color_Blue;
    GetPixel(Pixels, (v2i){0, (CONSOLE_HEIGHT - 1)})->Color = (v3)Color_Black;

    PrintString(Pixels, "EVERSCROLL", (v2i){(CONSOLE_WIDTH / 2) - (8 * 5), CONSOLE_HEIGHT / 2 - 8}, (v3){0x55, 0xDD, 0xFF});
}

local void
TextInCenterWithShadow(struct pixel* Pixels, char* Input, color TextColor)
{
    struct text_buffer Title = Text(Input, 0, 0);
    v2i TitleCenter = SubV2i((v2i)CONSOLE_CENTER, (v2i){Title.Length * 4, 8});

    PrintString(Pixels, Title.Data, AddV2i(TitleCenter, (v2i){1, 1}), (color)Color_Black);
    PrintString(Pixels, Title.Data, TitleCenter, (color){0x55, 0xDD, 0xFF});
}

local void
LoadScreen(struct pixel* Pixels, s32 ScreenIndex)
{
    switch (ScreenIndex % SCREEN_COUNT) {
        case 0: {
            VerticalGradient(Pixels, (color)Color_Blue, (color)Color_Black);
            TextInCenterWithShadow(Pixels, "EVERSCROLL", (color){0x55, 0xDD, 0xFF});
        } break;

        case 1: {
            VerticalGradient(Pixels, (color)Color_Black, (color)Color_Blue);
            // TextInCenterWithShadow(Pixels, "EVERSCROLL", (color){0x55, 0xDD, 0xFF});
        } break;

        case 2: {
            VerticalGradient(Pixels, (color)Color_Blue, (color)Color_Green);
            // TextInCenterWithShadow(Pixels, "EVERSCROLL", (color){0x55, 0xDD, 0xFF});
        } break;

        case 3: {
            VerticalGradient(Pixels, (color)Color_Green, (color)Color_White);
            // TextInCenterWithShadow(Pixels, "EVERSCROLL", (color){0x55, 0xDD, 0xFF});
        } break;

        case 4: {

            // TextInCenterWithShadow(Pixels, "EVERSCROLL", (color){0x55, 0xDD, 0xFF});
        } break;

        case SCREEN_COUNT: {
            // NOTE: Debug
            PixelOverlay(Pixels);
            SetStructuredArt(Pixels);
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

local void
EverScroll(struct everscroll_state* Scroll, struct user_input* Input, struct pixel* Pixels)
{
    s32 ScreenId = GetScreenId((s32)Input->MovementKeys.y, Scroll);
    LoadScreen(Pixels, ScreenId);
}

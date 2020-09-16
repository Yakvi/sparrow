#include "console_platform.h"

local void
DrawPixel(struct frame_buffer* Buffer, struct pixel* Pixel, dim_2i ConsoleSize, dim RealSize, dim_2i Size)
{
    if (Pixel) {
        Assert(Pixel->Pos.x >= 0);
        Assert(Pixel->Pos.y >= 0);
        Assert(Pixel->Pos.x < ConsoleSize.Width);
        Assert(Pixel->Pos.y < ConsoleSize.Height);

        v2i PixelBase = {0};
        PixelBase.x   = (u32)(Pixel->Pos.x * RealSize.Width);
        PixelBase.y   = (u32)(Pixel->Pos.y * RealSize.Height);

        s32 OverflowX = (PixelBase.x + Size.Width) - Buffer->Width;
        if (OverflowX >= 0) { Size.Width -= OverflowX; }
        s32 OverflowY = (PixelBase.y + Size.Height) - Buffer->Height;
        if (OverflowY >= 0) { Size.Height -= OverflowY; }

        DrawSolidColorRectangle(Buffer, Pixel->Color, PixelBase, Size);
    }
}

local void
DrawAllPixels(struct frame_buffer* Buffer, struct console* Console)
{
    dim RealSize    = {0};
    RealSize.Width  = (f32)Buffer->Width / (f32)Console->Size.Width;
    RealSize.Height = (f32)Buffer->Height / (f32)Console->Size.Height;

    dim_2i Size = {0};
    Size.Width  = (u32)(RealSize.Width + 1);
    Size.Height = (u32)(RealSize.Height + 1);

    struct pixel* Row = Console->Pixels;
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        struct pixel* Pixel = Row;
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {

            // TODO: Consistent pixel size!
            DrawPixel(Buffer, Pixel++, Console->Size, RealSize, Size);
        }
        Row += Console->Size.Width;
    }
}
#include "sparrow_console_platform.h"

local void
DrawPixel(struct frame_buffer* Buffer, struct pixel* Pixel, dim RealSize, dim_2d Size)
{
    if (Pixel) {
        Assert(Pixel->Pos.x >= 0);
        Assert(Pixel->Pos.y >= 0);
        Assert(Pixel->Pos.x < CONSOLE_WIDTH);
        Assert(Pixel->Pos.y < CONSOLE_HEIGHT);

        v2i PixelBase = {
            (u32)(Pixel->Pos.x * RealSize.Width),
            (u32)(Pixel->Pos.y * RealSize.Height)};

        Assert(PixelBase.x + Size.Width <= Buffer->Width);
        Assert(PixelBase.y + Size.Height <= Buffer->Height);

        DrawSolidColorRectangle(Buffer, Pixel->Color, PixelBase, Size);
    }
}

local void
DrawAllPixels(struct frame_buffer* Buffer, struct pixel* Pixels)
{
    dim RealSize = {
        (f32)Buffer->Width / (f32)CONSOLE_WIDTH,
        (f32)Buffer->Height / (f32)CONSOLE_HEIGHT};

    dim_2d Size = {
        (u32)(RealSize.Width + 1),
        (u32)(RealSize.Height + 1)};

    struct pixel* Row = Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {

            // TODO: Consistent pixel size!
            DrawPixel(Buffer, Pixel++, RealSize, Size);
        }
        Row += CONSOLE_WIDTH;
    }
}
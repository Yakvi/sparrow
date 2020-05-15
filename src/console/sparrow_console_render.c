#include "sparrow_console_render.h"

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

#if 0
local f32
GetSizeRemainder(u32 BucketSize, u32 Count, u32* Size)
{
    Assert(Count);
    f32 SizeReal = (f32)BucketSize / (f32)Count;
    *Size = (u32)(BucketSize / Count);
    f32 SizeRemainder = SizeReal - *Size;

    return (SizeRemainder);
}
#endif

local void
DrawAllPixels(struct frame_buffer* Buffer, struct pixel* Pixels)
{
#if 0
    dim_2d Size = {0};
    f32 WidthOffsetRate = GetSizeRemainder(Buffer->Width, CONSOLE_WIDTH, &Size.Width);
    f32 HeightOffsetRate = GetSizeRemainder(Buffer->Height, CONSOLE_HEIGHT, &Size.Height);
#else
    dim RealSize = {
        (f32)Buffer->Width / (f32)CONSOLE_WIDTH,
        (f32)Buffer->Height / (f32)CONSOLE_HEIGHT};

    dim_2d Size = {
        (u32)(RealSize.Width + 1),
        (u32)(RealSize.Height + 1)};
#endif

    struct pixel* Row = Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {

            Assert(Pixel);
            Assert(Pixel->Pos.x >= 0);
            Assert(Pixel->Pos.y >= 0);
            Assert(Pixel->Pos.x < CONSOLE_WIDTH);
            Assert(Pixel->Pos.y < CONSOLE_HEIGHT);

#if 0
            dim_2d PixelSize = Size;
            v2i PixelBase = {
                Pixel->Pos.x * PixelSize.Width,
                Pixel->Pos.y * PixelSize.Height};

            f32 WidthOffset = WidthOffsetRate * Pixel->Pos.x;
            PixelSize.Width += 1;
            PixelBase.x += (u32)WidthOffset;

            u32 HeightOffset = (u32)(HeightOffsetRate * Pixel->Pos.y);
            PixelSize.Height += (u32)(HeightOffsetRate * Pixel->Pos.y + 1) - HeightOffset;
            PixelBase.y += HeightOffset;

            Assert(PixelBase.x + PixelSize.Width <= Buffer->Width);
            Assert(PixelBase.y + PixelSize.Height <= Buffer->Height);

            DrawSolidColorRectangle(Buffer, Pixel++->Color, PixelBase, PixelSize);
#else
            // TODO: Consistent pixel size!
            DrawPixel(Buffer, Pixel++, RealSize, Size);
#endif
        }
        Row += CONSOLE_WIDTH;
    }
}
#include "sparrow_console_render.h"

local void
DrawPixel(struct frame_buffer* Buffer, struct pixel* Pixel, dim PixelSize)
{
    if (Pixel) {
        Assert(Pixel->Pos.x >= 0);
        Assert(Pixel->Pos.y >= 0);
        Assert(Pixel->Pos.x < CONSOLE_WIDTH);
        Assert(Pixel->Pos.y < CONSOLE_HEIGHT);

        v2i PixelBase = {
            (u32)(Pixel->Pos.x * PixelSize.Width),
            // NOTE(yakvi): The rows are laid out in memory bottom up. We want to go top-down
            (u32)((CONSOLE_HEIGHT - 1 - Pixel->Pos.y) * PixelSize.Height)};

        v2i PixelPos = {
            PixelBase.x + (s32)PixelSize.Width,
            PixelBase.y + (s32)PixelSize.Height};

        // NOTE(yakvi): Anti-overflow measures
        s32 OverflowX = PixelPos.x - Buffer->Width;
        if (OverflowX > 0) PixelBase.x -= OverflowX;

        s32 OverflowY = PixelPos.y - Buffer->Height;
        if (OverflowY > 0) PixelBase.y -= OverflowY;

        Assert(PixelPos.x <= Buffer->Width);
        Assert(PixelPos.y <= Buffer->Height);

        DrawSolidColorRectangle(Buffer, Pixel->Color, PixelBase, PixelSize);
    }
}

local void
DrawAllPixels(struct frame_buffer* Buffer, struct pixel* Pixels)
{
    // TODO: Consistent pixel size! 
    dim PixelSize = {
        (f32)Buffer->Width / (f32)CONSOLE_WIDTH,
        (f32)Buffer->Height / (f32)CONSOLE_HEIGHT};

    struct pixel* Row = Pixels;
    for (u32 Y = 0;
         Y < CONSOLE_HEIGHT;
         ++Y) {
        struct pixel* Pixel = Row;
        for (u32 X = 0;
             X < CONSOLE_WIDTH;
             ++X) {

            Assert(Pixel);
            // TODO: This can be easily job-ified
            DrawPixel(Buffer, Pixel++, PixelSize);
        }
        Row += CONSOLE_WIDTH;
    }
}
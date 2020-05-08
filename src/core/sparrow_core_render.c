#include "sparrow_core_render.h"

local void
DrawSolidColorRectangle(struct frame_buffer* Buffer, color Color, v2i Base, dim Dim)
{
    Assert(Base.x + Dim.Width <= Buffer->Width);
    Assert(Base.y + Dim.Height <= Buffer->Height);

    if (Buffer) {
        u8* Row = Buffer->Pixels + Base.y * Buffer->Pitch;
        for (u32 Y = 0;
             Y < Dim.Height;
             ++Y) {
            u32* Pixel = (u32*)Row + Base.x;
            for (u32 X = 0;
                 X < Dim.Width;
                 // TODO(yakvi): Fix the hidden bug of implicit rounding?
                 ++X) {
                *Pixel++ = (((u8)Color.r << 16) |
                            ((u8)Color.g << 8) |
                            (u8)Color.b | 0);
            }
            Row += Buffer->Pitch;
        }
    }
}

local void
Clear(struct frame_buffer* Buffer, color Color)
{
    DrawSolidColorRectangle(Buffer, Color, (v2i){0, 0}, (dim){Buffer->Width, Buffer->Height});
}

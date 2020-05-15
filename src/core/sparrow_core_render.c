#include "sparrow_core_render.h"

local void
DrawSolidColorRectangle(struct frame_buffer* Buffer, color Color, v2i Base, dim_2d Size)
{
    Assert(Base.x + Size.Width <= Buffer->Width);
    Assert(Base.y + Size.Height <= Buffer->Height);

    if (Buffer) {
        u8* Row = Buffer->Pixels + Base.y * Buffer->Pitch;
        for (u32 Y = 0;
             Y < Size.Height;
             ++Y) {
            u32* Pixel = (u32*)Row + Base.x;
            for (u32 X = 0;
                 X < Size.Width;
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
    dim_2d Size = {Buffer->Width, Buffer->Height};
    DrawSolidColorRectangle(Buffer, Color, (v2i){0, 0}, Size);
}

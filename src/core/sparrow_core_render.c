#include "sparrow_core_render.h"

local void
DrawSolidColorRectangle(struct frame_buffer* Buffer, color3 Color, v2i Base, dim_2i Size)
{
    Assert(Base.x + Size.Width <= Buffer->Width);
    Assert(Base.y + Size.Height <= Buffer->Height);

    if (Buffer) {
        u8* Row = Buffer->Pixels + Base.y * Buffer->Pitch;
        for (s32 Y = 0;
             Y < Size.Height;
             ++Y) {
            u32* Pixel = (u32*)Row + Base.x;
            for (s32 X = 0;
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
Clear(struct frame_buffer* Buffer, color3 Color)
{
    dim_2i Size = {Buffer->Width, Buffer->Height};
    DrawSolidColorRectangle(Buffer, Color, V2I(0, 0), Size);
}

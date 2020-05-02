#include "sparrow_core_render.h"

local void
DrawSolidColorRectangle(struct frame_buffer* Buffer, color Color, v2i Base, dim Dim)
{
    if (Buffer) {
        u32 Pitch = Buffer->Width * Buffer->BytesPerPixel; // TODO(yakvi): recalculate it globally at the start of render cycle?
        u8* Row = Buffer->Pixels + Base.y * Pitch;
        for (u32 Y = 0;
             Y < Dim.Height;
             ++Y) {
            u32* Pixerl = (u32*)Row + Base.x;
            for (u32 X = 0;
                 X < Dim.Width;
                 ++X) {
                *Pixerl++ = (((u8)Color.r << 16) |
                             ((u8)Color.g << 8) |
                             (u8)Color.b | 0);
            }
            Row += Pitch;
        }
    }
}

local void
Clear(struct frame_buffer* Buffer, color Color)
{
    DrawSolidColorRectangle(Buffer, Color, (v2i){0, 0}, (dim){Buffer->Width, Buffer->Height});
}

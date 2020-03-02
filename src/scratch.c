
local void
UpdateState(struct memory* Memory,
            struct frame_buffer* Buffer,
            struct user_input* Input)
{
}

local b32
IsTimeToRender(void)
{
    return true;
}

local void
Render(struct frame_buffer* Buffer)
{
    if (Buffer) {
        u32 Pitch = Buffer->Width * Buffer->BytesPerPixel;
        u8* Row = Buffer->Pixels;
        for (u32 y = 0;
             y < Buffer->Height;
             ++y) {
            u8* Pixel = Row;
            for (u32 x = 0;
                 x < Buffer->Width;
                 ++x) {
                *Pixel = (u8)x;
                ++Pixel;
                *Pixel = (u8)y;
                ++Pixel;
                *Pixel = 0;
                ++Pixel;
                *Pixel = 0;
                ++Pixel;
            }
            Row += Pitch;
        }
    }
}
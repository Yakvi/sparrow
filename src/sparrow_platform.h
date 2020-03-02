#if !defined(SPARROW_PLATFORM)

struct frame_buffer
{
    u16 Width;
    u16 Height;
    u16 BytesPerPixel;
    u16 Pad;
    u8* Pixels;
};
struct user_input
{
    b32 LeftMouseClick;
};
struct memory;

#define SPARROW_PLATFORM
#endif

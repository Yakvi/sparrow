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
struct memory
{
    memory_index Size;
    void* Data;
};

struct dim_2d{
    u32 Width;
    u32 Height;
};

#define SPARROW_PLATFORM
#endif

#if !defined(SPARROW_PLATFORM)

struct screen_buffer {
    u16 Width;
    u16 Height;
    u32 *Pixels;
};
struct user_input {
    b32 LeftMouseClick;
};
struct memory;

#define SPARROW_PLATFORM
#endif

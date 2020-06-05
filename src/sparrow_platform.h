#if !defined(SPARROW_PLATFORM)

#include "vector.h"

typedef struct dim_2d
{
    u32 Width;
    u32 Height;
} dim_2d;

struct frame_buffer
{
    u16 Width;
    u16 Height;
    u16 BytesPerPixel;
    u16 Pitch;
    u8* Pixels;
};
struct user_input
{
    b32 LeftMouseClick;
    v2i Cursor;
    v2 CursorNorm;
    v2 MovementKeys;
};
struct memory
{
    memory_index Size;
    void* Data;
};

#define SPARROW_PLATFORM
#endif

#if !defined(SPARROW_PLATFORM)

#include "sparrow_types.h"
#include "sparrow_memory.h"

typedef struct frame_buffer
{
    u16 Width;
    u16 Height;
    u16 BytesPerPixel;
    u16 Pitch;
    u8* Pixels;
} frame_buffer;

typedef struct user_input
{
    b32 LeftMouseClick;
    v2i Cursor;
    v2f CursorNorm;
    v2f MovementKeys;
} user_input;

typedef struct platform
{
    user_input* Input;
    f32         FrameDeltaMs;
    u32         FrameDeltaCycles;
} platform;

#define SPARROW_PLATFORM
#endif

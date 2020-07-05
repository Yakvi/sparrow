#if !defined(SPARROW_PLATFORM)

#include "vector.h"

typedef struct dim_2d
{
    u32 Width;
    u32 Height;
} dim_2d;

inline dim_2d
DIM_2D(u32 Width, u32 Height)
{
    dim_2d Result = {Width, Height};
    return (Result);
}

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

typedef struct memory
{
    memory_index Size;
    void* Data;
}memory;

#define SPARROW_PLATFORM
#endif

#if !defined(SPARROW_PLATFORM)

#include "sparrow_types.h"

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
} memory;

#define SPARROW_PLATFORM
#endif

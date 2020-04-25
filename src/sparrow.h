#if !defined(SPARROW_H)

#include "min_crt.h"
#include "types.h"
#include "sparrow_platform.h"
#include "sparrow_vector.h"

/**
 * TODO on Engine side
 * Memory Arena
 * SIMD software renderer
 * Debug logging
 * 
*/

struct pixel
{
    p Pos;
    color Color;
};

#define CONSOLE_WIDTH 120
#define CONSOLE_HEIGHT 40

struct game_state
{
    struct pixel Pixels[CONSOLE_WIDTH * CONSOLE_HEIGHT];
    b32 IsInitialized;
};

#define SPARROW_H
#endif

#if !defined(SPARROW_H)

#include "min_crt.h"
#include "types.h"
#include "sparrow_platform.h"

/**
 * TODO on Engine side
 * Memory Arena
 * SIMD software renderer
 * Debug logging
*/

struct pixel
{
    v2i Pos;
    color Color;
};

#define CONSOLE_WIDTH 120
#define CONSOLE_HEIGHT 40
#define CONSOLE_SIZE ((CONSOLE_WIDTH) * (CONSOLE_HEIGHT))

struct player
{
    v2 Pos;
};

struct game_state
{
    struct pixel Pixels[CONSOLE_WIDTH * CONSOLE_HEIGHT];
    struct player Player;
    b32 IsInitialized;
};

#define SPARROW_H
#endif

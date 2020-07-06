#if !defined(SPARROW_H)

#include "min_crt.h"
#include "types.h"
#include "text.h"
#include "vector.h"
#include "sparrow_platform.h"
#include "core/sparrow_core_render.h"
#include "console/console_platform.h"

/**
 * TODO on Engine side
 * Memory Arena
 * SIMD software renderer
 * Debug logging
*/

struct player
{
    v2f Pos;
};

struct game_state
{
    struct console Console;
    struct player Player;
    b32 UpdateCompleted;
    b32 IsInitialized;
    char ModuleMemory[MiB(2)];
};

#define SPARROW_H
#endif

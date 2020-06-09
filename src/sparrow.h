#if !defined(SPARROW_H)

#include "min_crt.h"
#include "types.h"
#include "text.h"
#include "sparrow_platform.h"
#include "core/sparrow_core_render.h"
#include "console/sparrow_console_platform.h"

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

// #include "everscroll/everscroll.h"
#include "writer/writer.h"

struct game_state
{
    struct console Console;
    struct player Player;
    // struct everscroll_state Scroll;
    struct writer_state Writer;
    b32 IsInitialized;
};

#define SPARROW_H
#endif

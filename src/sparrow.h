#if !defined(SPARROW_H)

#include "min_crt.h"
#include "types.h"
#include "text.h"
#include "sparrow_platform.h"
#include "core/sparrow_core_render.h"
#include "console/sparrow_console_platform.h"
#include "console/sparrow_console_render.h"
#include "console/sparrow_console_update.h"
#include "everscroll/everscroll.h"

/**
 * TODO on Engine side
 * Memory Arena
 * SIMD software renderer
 * Debug logging
*/

struct player
{
    v2 Pos;
};

struct game_state
{
    struct pixel Pixels[CONSOLE_WIDTH * CONSOLE_HEIGHT]; // TODO(yakvi): Abstract it out? Overkill?
    struct player Player;
    struct everscroll_state Scroll;
    b32 IsInitialized;
};

#define SPARROW_H
#endif

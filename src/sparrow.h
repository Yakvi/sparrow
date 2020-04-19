#if !defined(SPARROW_H)

#include "min_crt.h"
#include "types.h"
#include "sparrow_platform.h"

struct game_state
{
    u16 GradientXOffset;
    u16 GradientYOffset;
    b32 IsInitialized;
};

#define SPARROW_H
#endif

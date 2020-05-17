#if !defined(SPARROW_MATH_H)

#include "types.h"

inline f32
Lerp(f32 A, f32 B, f32 t)
{
    f32 Result = (1 - t) * A + (B * t);
    return (Result);
}

inline s32
RoundF32ToInt(f32 number)
{
    s32 Result = 0;
    if (number > 0) {
        Assert(number + 0.5f > 0);
        Result = (s32)(number + 0.5f);
    }
    else if (number < 0) {
        Assert(number - 0.5f < 0);
        Result = (s32)(number - 0.5f);
    }

    return (Result);
}

#define SPARROW_MATH_H
#endif

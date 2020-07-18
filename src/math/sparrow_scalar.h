#if !defined(SPARROW_SCALAR_H)

#include "core/sparrow_types.h"
#include "core/sparrow_intrin.h"

EXTERN_C_START
inline s32
RoundF32ToInt(f32 number)
{
    // TODO: Use cpuid to potentially invoke SSE4.1's ceil/floor?
    s32 Result = 0;
    if (number > 0) {
        Assert(number + 0.5f > 0); // TODO: overflow protection
        Result = (s32)(number + 0.5f);
    }
    else if (number < 0) {
        Assert(number - 0.5f < 0); // TODO: underflow protection
        Result = (s32)(number - 0.5f);
    }

    return (Result);
}

inline f32
DegToRadian(f32 DegreeAngle)
{
    f32 Result = DegreeAngle * Pi180;

    return (Result);
}

// NOTE: Utility / Scalar functions

inline f32
Lerp(f32 A, f32 B, f32 t)
{
    f32 Result = (1 - t) * A + (B * t);

    return (Result);
}

inline f32
Clamp(f32 Min, f32 Value, f32 Max)
{
    f32 Result = Value;

    if (Result < Min) {
        Result = Min;
    }
    else if (Result > Max) {
        Result = Max;
    }

    return (Result);
}

inline f32
Clamp01(f32 Value)
{
    return Clamp(0.0f, Value, 1.0f);
}

// "Lerp-clamp"
inline f32
Clamp01MapToRange(f32 Min, f32 t, f32 Max)
{
    f32 Result = 0.0f;

    f32 Range = Max - Min;
    if (Range != 0.0f) {
        Result = Clamp01((t - Min) / Range);
    }

    return (Result);
}
EXTERN_C_END

#define SPARROW_SCALAR_H
#endif

#if !defined(SPARROW_INTRIN_H)
// TODO(yakvi): CRT-free way of implementing:
// ACos: https://hg.libsdl.org/SDL/file/5c894fec85b9/src/stdlib/SDL_stdlib.c ?
// Tan, Cot, ATan, ATan2: https://github.com/to-miz/sse_mathfun_extension
// TODO(yavki): __cpuid to check for SSE 4.1 usage!

#include "core/sparrow_math_optimized.h"

inline f32
SquareRoot(f32 Value)
{
    f32 Result = sqrt_f(Value);
    return (Result);
}

// Computes the sine of Angle (measured in radians).
inline f32
Sin(f32 Angle)
{
    f32 Result = sin_f(Angle);
    return (Result);
};

// Computes the cosine of Angle (measured in radians).
inline f32
Cos(f32 Angle)
{
    f32 Result = cos_f(Angle);
    return (Result);
}

// Computes both the sine and cosine of Angle (measured in radians).
inline void
SinCos(f32 Angle, f32* OutSine, f32* OutCosine)
{
    sincos_f(Angle, OutSine, OutCosine);
}

// Computes the e (Euler's number, 2.7182818) raised to the given power Value.
inline f32
Exp(f32 Value)
{
    f32 Result = exp_f(Value);
    return (Result);
}

// Computes the natural (base e) logarithm of Value.
inline f32
Log(f32 Value)
{
    f32 Result = log_f(Value);
    return (Result);
}

inline f32 Tan();
inline f32 ACos();
inline f32 ATan();
inline f32 ATan2();

#define SPARROW_INTRIN_H
#endif
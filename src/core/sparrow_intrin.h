#if !defined(SPARROW_INTRIN_H)
// TODO(yakvi): CRT-free way of implementing:
// ACos: https://hg.libsdl.org/SDL/file/5c894fec85b9/src/stdlib/SDL_stdlib.c ?
// Tan, Cot, ATan, ATan2: https://github.com/to-miz/sse_mathfun_extension
// TODO(yavki): __cpuid to check for SSE 4.1 usage!

#include "math/sparrow_trig.h"

inline f32
SquareRoot(f32 Value)
{
    f32 Result = sqrt_f(Value);
    return (Result);
}

inline f32
InvSquareRoot(f32 Value)
{
    f32 Result = rsqrt_f(Value);
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

// Elevates the base to the specified power
inline f32
Power(f32 Base, f32 Exponent)
{
    f32 Result = exp_f(Exponent * log_f(Base));
    return (Result);
}

// Computes tangent based on provided angle
inline f32
Tan(f32 X)
{
    f32 Result = tan_f(X);
    return (Result);
}

// Computes the Cotangent of X
inline f32
Cot(f32 X)
{
    f32 Result = cot_f(X);
    return (Result);
}

// Computes the arctangent of X
inline f32
ArcTan(f32 X)
{
    f32 Result = atan_f(X);
    return (Result);
}

// Computes the arctangent of y / x given y and x, but with a range of (−π, π].
// In other words, atan2(y, x) is the angle between the positive x-axis of a plane
// and the point (x, y) on it, with positive sign for counter-clockwise angles
// (upper half-plane, y > 0), and negative sign for clockwise angles (lower half-plane, y < 0).
inline f32
ArcTan2(f32 Y, f32 X)
{
    f32 Result = atan2_f(Y, X);
    return (Result);
}

#define SPARROW_INTRIN_H
#endif
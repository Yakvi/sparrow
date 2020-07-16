#if !defined(SPARROW_VECTOR_H)

#include "core/sparrow_types.h"
#include "core/sparrow_intrin.h"

//
// NOTE: Vector/Matrix Initializations
//

// Vector2 with floating point elements
inline v2f
V2F(f32 X, f32 Y)
{
    v2f Result;

    Result.x = X;
    Result.y = Y;

    return (Result);
}

// Create a floating point vector2 from an integer pair
inline v2f
V2Fi(s32 X, s32 Y)
{
    v2f Result = V2F((f32)X, (f32)Y);
    return (Result);
}

#ifdef __cplusplus
inline v2f
V2F(u32 X, u32 Y)
{
    // TODO: ensure that X and Y are lower than FLT_MAX
    return V2F((f32)X, (f32)Y);
}
inline v2f
V2F(s32 X, s32 Y)
{
    return V2Fi(X, Y);
}
#endif // __cplusplus

inline dim
DIM_2F(f32 Width, f32 Height)
{
    dim Result = V2F(Width, Height);
    return (Result);
}

// Vector2 with integer elements
inline v2i
V2I(s32 X, s32 Y)
{
    v2i Result;

    Result.x = X;
    Result.y = Y;

    return (Result);
}

inline dim_2i
DIM_2I(s32 Width, s32 Height)
{
    dim_2i Result = V2I(Width, Height);
    return (Result);
}

// Create an integer vector2 from a floating point pair
inline v2i
V2If(f32 X, f32 Y)
{
    v2i Result = V2I((s32)X, (s32)Y); // TODO: floor?
    return (Result);
}

#ifdef __cplusplus
inline v2i
V2I(u32 X, u32 Y)
{
    // TODO: ensure that unsigned integers are smaller than INT_MAX
    return V2I((s32)X, (s32)Y);
}
inline v2i
V2I(f32 X, f32 Y)
{
    return V2If(X, Y);
}
#endif // __cplusplus

// Vector 3
inline v3
V3F(f32 X, f32 Y, f32 Z)
{
    v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return (Result);
}

inline v3
V3(f32 R, f32 G, f32 B)
{
    v3 Result = V3F(R, G, B);
    return (Result);
}

inline color3
RGB(f32 R, f32 G, f32 B)
{
    color3 Result = V3F(R, G, B);
    return (Result);
}

// Create a floating point vector3 from integers
inline v3f
V3Fi(s32 X, s32 Y, s32 Z)
{
    v3f Result = V3F((f32)X, (f32)Y, (f32)Z);
    return (Result);
}
#ifdef __cplusplus
inline v3
V3(s32 X, s32 Y, s32 Z)
{
    return V3Fi(X, Y, Z);
}
#endif // __cplusplus

// Vector 4
inline v4f
V4F(f32 X, f32 Y, f32 Z, f32 W)
{
    v4f Result;
    Result.VectorString = _mm_setr_ps(X, Y, Z, W);
    return (Result);
}

inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;
    Result.VectorString = _mm_setr_ps(X, Y, Z, W);
    return (Result);
}

inline rgba
RGBA(f32 R, f32 G, f32 B, f32 A)
{
    rgba Result;
    Result.VectorString = _mm_setr_ps(R, G, B, A);
    return (Result);
}

// Create a floating point vector4 from integers
inline v4
V4Fi(s32 X, s32 Y, s32 Z, s32 W)
{
    v4 Result;
    Result.VectorString = _mm_setr_ps((f32)X, (f32)Y, (f32)Z, (f32)W);
    return (Result);
}

#ifdef __cplusplus
inline v4
V4(s32 X, s32 Y, s32 Z, s32 W)
{
    v4 Result;
    Result.VectorString = _mm_setr_ps((f32)X, (f32)Y, (f32)Z, (f32)W);
    return (Result);
}
#endif // __cplusplus

//
// NOTE: Scalar convertions
//

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

#define SPARROW_VECTOR_H
#endif

#if !defined(SPARROW_COLOR_H)

#include "core/sparrow_types.h"
//
// NOTE: Vector Initializations
//
EXTERN_C_START

inline color3
RGB(f32 R, f32 G, f32 B)
{
    color3 Result = V3F(R, G, B);
    return (Result);
}

inline rgba
RGBA(f32 R, f32 G, f32 B, f32 A)
{
    rgba Result;
    Result.VectorString = _mm_setr_ps(R, G, B, A);
    return (Result);
}

//
// NOTE: Utilities
//

inline b32
ColorsEqual(v3 A, v3 B)
{
    b32 Result = (A.r == B.r) && (A.g == B.g) && (A.b == B.b);
    return (Result);
}

inline color3
ColorLerp(color3 A, color3 B, f32 t)
{
    color3 Result;
    Result.r = Lerp(A.r, B.r, t);
    Result.g = Lerp(A.g, B.g, t);
    Result.b = Lerp(A.b, B.b, t);

    return (Result);
}
EXTERN_C_END

#ifdef __cplusplus
// clang-format off
inline color3 RGB(u32 R, u32 G, u32 B) { return RGB((f32)R, (f32)G, (f32)B); }
inline color3 RGB(s32 R, s32 G, s32 B) { return RGB((f32)R, (f32)G, (f32)B); }
inline color3 RGB(f64 R, f64 G, f64 B) { return RGB((f32)R, (f32)G, (f32)B); }
// clang-format on
#endif // __cplusplus

// NOTE: Color constants
// clang-format off
# define Color_Black     RGB(0,    0,    0)
# define Color_White     RGB(0xFF, 0xFF, 0xFF)

# define Color_Gray05    RGB(0x55, 0x55, 0x55)
# define Color_Gray11    RGB(0xAA, 0xAA, 0xAA)
# define Color_Gray13    RGB(0xDD, 0xDD, 0xDD)

# define Color_Red       RGB(0xFF, 0,    0)
# define Color_Green     RGB(0,    0xFF, 0)
# define Color_Blue      RGB(0,    0,    0xFF)

# define Color_Yellow    RGB(0xFF, 0xFF, 0)
# define Color_Cyan      RGB(0,    0xFF, 0xFF)
# define Color_Pink      RGB(0xFF, 0,    0xFF)
// clang-format on

#define SPARROW_COLOR_H
#endif

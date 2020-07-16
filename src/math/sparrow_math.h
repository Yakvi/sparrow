#if !defined(SPARROW_MATH_H)
//
// NOTE: Math constants
//
#define Pi32 3.14159265359f
#define Pi180 0.01745329251f
#define Pi64 3.14159265358979323846
#define Tau32 6.28318530717958647692f

// TODO: fast floor/ceil? https://gist.github.com/mmozeiko/56db3df14ab380152d6875383d0f4afd

#include "core/sparrow_intrin.h"
#include "sparrow_vector.h"

// BOOKMARK: Vector 2

inline v2f
AddV2(v2f A, v2f B)
{
    v2f Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return (Result);
}

inline v2i
SubV2i(v2i A, v2i B)
{
    v2i Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return (Result);
}

inline v2i
AddV2i(v2i A, v2i B)
{
    v2i Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return (Result);
}

inline v2i
RoundV2ToV2i(v2f Source)
{
    v2i Result;
    Result.x = RoundF32ToInt(Source.x);
    Result.y = RoundF32ToInt(Source.y);
    return (Result);
}

// BOOKMARK: Color constants
// TODO: Move this out somewhere else?
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

inline b32
ColorsEqual(v3 A, v3 B)
{
    b32 Result = (A.r == B.r) &&
                 (A.g == B.g) &&
                 (A.b == B.b);
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

#define SPARROW_MATH_H
#endif

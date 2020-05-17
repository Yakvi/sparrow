#if !defined(SPARROW_VECTOR_H)

#include "types.h"
#include "math.h"
// BOOKMARK: Vector 2

typedef struct
{
    f32 x;
    f32 y;
} p, v2;

typedef struct
{
    s32 x;
    s32 y;
} v2i;

typedef struct
{
    f32 Width;
    f32 Height;
} dim;

inline void
AddV2(v2* A, v2 B)
{
    A->x += B.x;
    A->y += B.y;
}

inline v2i
RoundV2ToV2i(v2 Source)
{
    v2i Result = {0};
    Result.x = RoundF32ToInt(Source.x);
    Result.y = RoundF32ToInt(Source.y);
    return (Result);
}

// BOOKMARK: Vector 3

typedef struct
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };
        struct
        {
            f32 r;
            f32 g;
            f32 b;
        };
        struct
        {
            p xy;
            f32 unused;
        };
    };

} color, v3;

inline v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result = {X, Y, Z};
    return (Result);
}

// typedef struct
// {
//     union
//     {
//         struct
//         {
//             p top_left;
//             p bottom_right;
//         };
//         struct
//         {
//             p origin;
//             f32 width;
//             f32 height;
//         };
//     };

// } rect;

// BOOKMARK: Color constants
// TODO: Move this out somewhere else?
// clang-format off
# define Color_Black     { 0,    0,    0}
# define Color_White     { 0xFF, 0xFF, 0xFF}

# define Color_Gray05    { 0x55, 0x55, 0x55}
# define Color_Gray11    { 0xAA, 0xAA, 0xAA}
# define Color_Gray13    { 0xDD, 0xDD, 0xDD}

# define Color_Red       { 0xFF, 0,    0}
# define Color_Green     { 0,    0xFF, 0}
# define Color_Blue      { 0,    0,    0xFF}

# define Color_Yellow    { 0xFF, 0xFF, 0}
# define Color_Cyan      { 0,    0xFF, 0xFF}
# define Color_Pink      { 0xFF, 0,    0xFF}
// clang-format on

inline b32
ColorsEqual(v3 A, v3 B)
{
    b32 Result = (A.r == B.r) &&
                 (A.g == B.g) &&
                 (A.b == B.b);
    return (Result);
}

#define SPARROW_VECTOR_H
#endif

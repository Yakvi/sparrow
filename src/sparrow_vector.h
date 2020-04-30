#if !defined(SPARROW_VECTOR_H)

#include "types.h"

typedef struct
{
    f32 x;
    f32 y;
} p, v2;

typedef struct
{
    u32 x;
    u32 y;
} v2u;

typedef struct
{
    f32 Width;
    f32 Height;
} dim;

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

typedef struct
{
    union
    {
        struct
        {
            p top_left;
            p bottom_right;
        };
        struct
        {
            p origin;
            f32 width;
            f32 height;
        };
    };

} rect;

// BOOKMARK: Color constants
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

#define SPARROW_VECTOR_H
#endif

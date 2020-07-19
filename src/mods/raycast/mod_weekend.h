#if !defined(RAYTRACING_IN_A_WEEKEND_H)

#include "min_crt.h"
#include "core/sparrow_platform.h"
#include "math/sparrow_math.h"
#include "console/console_platform.h"

struct ray
{
    p3  Origin;
    v3  Direction;
    v3  Normal;
    f32 InvLength;
};

#include "mod_raycast_geometry.h"

struct camera
{
    b32 IsInitialized;
    f32 AspectRatio;
    s32 ImageWidth;
    s32 ImageHeight;

    f32 ViewportHeight;
    f32 ViewportWidth;
    f32 FocalLength;

    v3 Origin;
    v3 Horizontal;
    v3 Vertical;
    v3 LowerLeftCorner;
};

#define SPHERES_COUNT 1

struct world
{
    camera MainCamera;
    sphere Spheres[SPHERES_COUNT];
};

inline ray
CreateRay(p3 Origin, v3 Direction)
{
    ray Result;

    Result.Origin    = Origin;
    Result.Direction = Direction;
    Result.Normal    = Normalize(Direction);
    Result.InvLength = 1.0f / LenSquared(Result.Direction);

    return (Result);
}

#define RAYTRACING_IN_A_WEEKEND_H
#endif
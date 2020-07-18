#if !defined(RAYTRACING_IN_A_WEEKEND_H)

#include "min_crt.h"
#include "core/sparrow_platform.h"
#include "math/sparrow_math.h"
#include "console/console_platform.h"

struct ray
{
    p3 Origin;
    v3 Direction;
};

// TODO: implement proper unit vector in sparrow_vector!
inline v3
UnitVector(v3 A)
{
    f32 LenSquared = (A.x * A.x) + (A.y * A.y) + (A.z * A.z);
    v3 Result = A * (1.0f / SquareRoot(LenSquared));
    return (Result);
}

inline ray
RAY(p3 Origin, v3 Direction)
{
    ray Result;

    Result.Origin = Origin;
    Result.Direction = UnitVector(Direction);

    return (Result);
}

#define RAYTRACING_IN_A_WEEKEND_H
#endif
#if !defined(RAYCAST_DIFFUSE_H)

#define Diffuse_UnitRadius 1
#define Diffuse_Lambertian 2
#define Diffuse_Hemisphere 3
#define DIFFUSE_METHOD Diffuse_Lambertian

#if DIFFUSE_METHOD == Diffuse_UnitRadius
// We need a way to pick a random point in a unit radius sphere.
// We’ll use what is usually the easiest algorithm: a rejection method.
// First, pick a random point in the unit cube where x, y, and z all range from −1 to +1.
// Reject this point and try again if the point is outside the sphere.
local v3
Diffuse(rng_state* State, ...)
{
    v3 Result;

    while (1) {
        Result = RandomRangeV3(State, -1, 1);
        if (LenSquared(Result) >= 1) {
            break;
        }
    }

    return (Result);
}
#elif DIFFUSE_METHOD == Diffuse_Lambertian
// True Lambertian distribution
local v3
Diffuse(rng_state* State, ...)
{
    f32 a = RandomRangeF(State, 0, Tau32);
    f32 z = RandomRangeF(State, -1, 1);
    f32 r = SquareRoot(1 - Square(z));

    v3 Result = V3(r * Cos(a), r * Sin(a), z);

    return (Result);
}
#elif DIFFUSE_METHOD == Hemisphere
local v3
Diffuse(rng_state* State, v3 Normal)
{
    v3 Result;
    while (1) {
        Result = RandomRangeV3(State, -1, 1);
        if (LenSquared(Result) >= 1) {
            break;
        }
    }

    if (Inner(Result, Normal) <= 0.0f) // In the opposite hemisphere from the normal
    {
        Result = -Result;
    }

    return (Result);
}
#endif

#undef DIFFUSE_METHOD
#undef Diffuse_UnitRadius
#undef Lambertian
#undef Hemispehere

#define RAYCAST_DIFFUSE_H
#endif

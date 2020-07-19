#if !defined(MOD_RAYCAST_GEOMETRY_H)

struct hit_record
{
    p3  Pos;
    v3  Normal;
    f32 RayLength;
};

struct sphere
{
    p3  Center;
    f32 Radius;
};

inline sphere
CreateSphere(p3 Center, f32 Radius)
{
    sphere Result;

    Result.Center = Center;
    Result.Radius = Radius;

    return (Result);
}

local f32
Hit(ray Ray, sphere* Sphere, f32 MinDistance, f32 MaxDistance)
{
    f32 Result = -1.0f;

    // Attempt to throw the ray at the sphere center
    v3  OC = Ray.Origin - Sphere->Center;
    f32 a  = LenSquared(Ray.Direction);
    f32 b  = Inner(OC, Ray.Direction);
    f32 c  = LenSquared(OC) - Square(Sphere->Radius);

    f32 discriminant = Square(b) - (a * c);

    if (discriminant >= 0) {
        // Find the ray length at hit
        Result = (-b - SquareRoot(discriminant)) * Ray.InvLength;
    }

    return (Result);
}

#define MOD_RAYCAST_GEOMETRY_H
#endif

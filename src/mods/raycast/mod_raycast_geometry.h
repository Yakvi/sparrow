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
    f32 RadiusSq;
};

inline sphere
CreateSphere(p3 Center, f32 Radius)
{
    sphere Result;

    Result.Center   = Center;
    Result.Radius   = Radius;
    Result.RadiusSq = Square(Radius);

    return (Result);
}

local b32
RayHit(ray Ray, sphere* Sphere, f32 MinDistance, f32 MaxDistance, p3* HitNormal)
{
    b32 Result = false;

    // Attempt to throw the ray at the sphere center
    // assume ax² + bx + c = 0 quadratic equation
    Validate(Ray.Direction);
    v3  DirectHitVector   = Ray.Origin - Sphere->Center;
    f32 RayDirectionLenSq = LenSquared(Ray.Direction);                      // a
    f32 AngleCoefficient  = Inner(DirectHitVector, Ray.Direction);          // h
    f32 SphereNormalLenSq = LenSquared(DirectHitVector) - Sphere->RadiusSq; // c

    // find the discriminant based on formula h² - ac
    f32 Discriminant = Square(AngleCoefficient) - (RayDirectionLenSq * SphereNormalLenSq);

    if (Discriminant > 0) {
        f32 DiscRoot = SquareRoot(Discriminant);

        // Find the ray length at hit
        f32 HitPoint1 = (-AngleCoefficient - DiscRoot) * Ray.InvLenSquared;
        f32 HitPoint2 = (-AngleCoefficient + DiscRoot) * Ray.InvLenSquared;
        // Check if any hit is valid and record it
        if (MinDistance < HitPoint1 && HitPoint1 < MaxDistance) {
            p3 HitPos  = RayAt(Ray, HitPoint1);
            *HitNormal = Normalize(HitPos - Sphere->Center); // / Sphere->Radius;
            Result     = true;
        }
        else if (MinDistance < HitPoint2 && HitPoint2 < MaxDistance) {
            p3 HitPos  = RayAt(Ray, HitPoint2);
            *HitNormal = Normalize(HitPos - Sphere->Center); // / Sphere->Radius;
            Result     = true;
        }
    }

    return (Result);
}

#define MOD_RAYCAST_GEOMETRY_H
#endif

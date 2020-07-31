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
    f32 InvRadius;
};

// Sphere

inline sphere
CreateSphere(p3 Center, f32 Radius)
{
    sphere Result;

    Result.Center    = Center;
    Result.Radius    = Radius;
    Result.RadiusSq  = Square(Radius);
    Result.InvRadius = 1.0f / Radius;

    return (Result);
}

local b32
RayHit(ray* Ray, sphere Sphere, f32 MinDistance, f32 MaxDistance, ray_hit_info* HitInfo)
{
    b32 Result = false;

    // Attempt to throw the ray at the sphere center
    // assume ax² + bx + c = 0 quadratic equation
    Validate(Ray->Direction);
    v3  DirectHitVector   = Ray->Origin - Sphere.Center;
    f32 RayDirectionLenSq = Ray->DirectionLenSquared;                      // a
    f32 AngleCoefficient  = Inner(DirectHitVector, Ray->Direction);        // h
    f32 SphereNormalLenSq = LenSquared(DirectHitVector) - Sphere.RadiusSq; // c

    // find the discriminant based on formula h² - ac
    f32 Discriminant = Square(AngleCoefficient) - (RayDirectionLenSq * SphereNormalLenSq);

    if (Discriminant > 0) {
        f32 DiscRoot = SquareRoot(Discriminant);

        // Find the ray length at hit
        f32 HitPoint1 = (-AngleCoefficient - DiscRoot) * Ray->DirectionInvLenSquared;
        f32 HitPoint2 = (-AngleCoefficient + DiscRoot) * Ray->DirectionInvLenSquared;
        // Check if any hit is valid and record it
        if (MinDistance < HitPoint1 && HitPoint1 < MaxDistance) {
            HitInfo->DistanceFromOrigin = HitPoint1;
            HitInfo->Pos                = RayAt(Ray, HitPoint1);
            v3 OutNormal                = (HitInfo->Pos - Sphere.Center) * Sphere.InvRadius;
            SetFaceNormal(Ray, HitInfo, OutNormal);
            Result = true;
        }
        else if (MinDistance < HitPoint2 && HitPoint2 < MaxDistance) {
            HitInfo->DistanceFromOrigin = HitPoint2;
            HitInfo->Pos                = RayAt(Ray, HitPoint2);
            v3 OutNormal                = (HitInfo->Pos - Sphere.Center) * Sphere.InvRadius;
            SetFaceNormal(Ray, HitInfo, OutNormal);
            Result = true;
        }
    }

    return (Result);
}

#define MOD_RAYCAST_GEOMETRY_H
#endif

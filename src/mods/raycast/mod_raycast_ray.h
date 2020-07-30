#if !defined(MOD_RAYCAST_RAY_H)

struct ray
{
    p3  Origin;
    v3  Direction;
    v3  Normal;
    f32 InvLenSquared;
};

struct ray_hit_info
{
    f32 DistanceFromOrigin;
    p3  Pos;
    v3  Normal;
    b32 FrontFace;
};

inline void
SetFaceNormal(ray* Ray, ray_hit_info* HitInfo, v3 OutwardNormal)
{
    HitInfo->FrontFace = Inner(Ray->Direction, OutwardNormal) < 0;
    HitInfo->Normal    = HitInfo->FrontFace ? OutwardNormal : -OutwardNormal;
}

inline ray
CreateRay(p3 Origin, v3 Direction)
{
    ray Result;

    Result.Origin        = Origin;
    Result.Direction     = Direction;
    Result.Normal        = Normalize(Direction);
    Result.InvLenSquared = 1.0f / LenSquared(Result.Direction);

    return (Result);
}

inline p3
RayAt(ray* Ray, f32 At)
{
    p3 Result = Ray->Origin + (Ray->Direction * At);
    return (Result);
}

#define MOD_RAYCAST_RAY_H
#endif
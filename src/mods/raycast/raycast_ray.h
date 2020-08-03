#if !defined(RAYCAST_RAY_H)

struct ray
{
    p3  Origin;
    v3  Direction;
    f32 DirectionLenSquared;
    f32 DirectionInvLenSquared;
    f32 InvLenSquared;
    v3  Normal;

    ray* Next;
};

struct ray_hit_info
{
    f32 DistanceFromOrigin;
    p3  Pos;
    v3  Normal;
    b32 FrontFace;
};

struct camera
{
    b32 IsInitialized;
    s32 SamplesPerPixel;
    f32 Scale;

    v3 Origin;
    v3 Horizontal;
    v3 Vertical;
    v3 LowerLeftCorner;
    v3 Base;

    ray* Rays;

    ray* FirstAvailableRay;
};

inline void
SetFaceNormal(ray* Ray, ray_hit_info* HitInfo, v3 OutwardNormal)
{
    HitInfo->FrontFace = Inner(Ray->Direction, OutwardNormal) < 0;
    HitInfo->Normal    = HitInfo->FrontFace ? OutwardNormal : -OutwardNormal;
}

inline void
CreateRay(ray* Ray, v3 Origin, v3 Direction)
{
    Ray->Origin                 = Origin;
    Ray->Direction              = Direction;
    Ray->DirectionLenSquared    = LenSquared(Ray->Direction);
    Ray->DirectionInvLenSquared = 1.0f / Ray->DirectionLenSquared;
    Ray->Normal                 = Normalize(Ray->Direction);
}

inline ray
CreateRay(v3 Origin, v3 Direction)
{
    ray Result;

    CreateRay(&Result, Origin, Direction);

    return (Result);
}

inline ray
CreateRay(camera* Camera, f32 u, f32 v)
{
    v3 Direction = Camera->Base + Camera->Horizontal * u + Camera->Vertical * v;

    ray Result = CreateRay(Camera->Origin, Direction);

    return (Result);
}

inline ray*
SpawnRay(memory* Memory, camera* Camera)
{
    ray* Result = 0;
    if (Camera->FirstAvailableRay) {
        Result                    = Camera->FirstAvailableRay;
        Camera->FirstAvailableRay = Camera->FirstAvailableRay->Next;
    }
    else {
        Result = GetStruct(Memory, ray);
    }

    return (Result);
}

inline void
RemoveRay(camera* Camera, ray* Ray)
{
    Ray->Next                 = Camera->FirstAvailableRay;
    Camera->FirstAvailableRay = Ray;
}

inline p3
RayAt(ray* Ray, f32 At)
{
    p3 Result = Ray->Origin + (Ray->Direction * At);
    return (Result);
}

#define MOD_RAYCAST_RAY_H
#endif
#include "mod_weekend.h"
#include "console/console_update.c"

local void
SetColor(console* Console, v2i Coords, rgb Color, f32 Scale)
{
    Color *= Scale;            // Divide the color by the number of samples
    Color = SquareRoot(Color); // Gamma Correction, 1/gamma
    Color = Clamp01(Color);    // Clamping
    Color *= 255.0f;           // Denormalize the color

    Point(Console, Coords, Color);
}

local void
SpawnEntities(memory* Memory, world* World)
{
    SpawnCollider(Memory, World)->Sphere = CreateSphere(V3(0, 0, -1), 0.5f);
    SpawnCollider(Memory, World)->Sphere = CreateSphere(V3(-5, 0, -10), 3.0f);
    SpawnCollider(Memory, World)->Sphere = CreateSphere(V3(0.0f, -100.5f, -1.0f), 100.0f);
}

local void
UpdateEntities(user_input* Input, world* World)
{
    sphere* Sphere   = &World->ColliderSentinel->Next->Sphere;
    Sphere->Center.x = (Input->CursorNorm.x - 0.5f) * 4;
    Sphere->Center.y = -(Input->CursorNorm.y - 0.5f) * 4;
}

// We need a way to pick a random point in a unit radius sphere.
// We’ll use what is usually the easiest algorithm: a rejection method.
// First, pick a random point in the unit cube where x, y, and z all range from −1 to +1.
// Reject this point and try again if the point is outside the sphere.
local v3
RandomInUnitSphere(rng_state* State)
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

inline u32
GetRayIndex(console* Console, s32 X, s32 Y, s32 Sample, s32 SamplesPerPixel)
{
    u32 Result = Y * Console->Size.Width * SamplesPerPixel;
    Result += X * SamplesPerPixel;
    Result += Sample;
    Assert(Result < Console->PixelCount * SamplesPerPixel);
    // Result = (Y * Console->Size.Width + X) * Sample; // cool bug
    return (Result);
}

local void
InitCamera(memory* Memory, camera* Camera, console* Console, rng_state* RNG,
           dim_2i ImageSize, f32 AspectRatio, s32 SamplesPerPixel)
{
    // Init Camera

    f32 ViewportHeight = 2.0f;
    f32 ViewportWidth  = AspectRatio * ViewportHeight;
    f32 FocalLength    = 1.0f;

    Camera->SamplesPerPixel = SamplesPerPixel;
    Camera->Scale           = 1.0f / SamplesPerPixel;

    Camera->Origin          = V3(0, 0, 0);
    Camera->Horizontal      = V3(ViewportWidth, 0.0f, 0.0f);
    Camera->Vertical        = V3(0.0f, ViewportHeight, 0.0f);
    Camera->LowerLeftCorner = Camera->Origin
                              - (Camera->Horizontal * 0.5f)
                              - (Camera->Vertical * 0.5f)
                              - V3(0.0f, 0.0f, FocalLength);
    Camera->Base = Camera->LowerLeftCorner - Camera->Origin;

    //NOTE: Init all rays
    memory_index RaySize = ImageSize.Width * ImageSize.Height;
    if (!Camera->Rays || RaySize > Console->PixelCount) {
        RaySize *= sizeof(ray) * Square(Camera->SamplesPerPixel);
        Camera->Rays = (ray*)GetSize(Memory, RaySize, 4, MemoryFlag_NoClear);
    }

    InitConsole(Console, ImageSize.Width, ImageSize.Height, Console_BottomUp, Color_White);

    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {
            for (s32 Sample = 0;
                 Sample < Camera->SamplesPerPixel;
                 ++Sample) {
                u32 RayIndex = GetRayIndex(Console, X, Y, Sample, Camera->SamplesPerPixel);

                f32 u = (X + RandomNext01(RNG)) * Console->InvertedSize.Width;
                f32 v = (Y + RandomNext01(RNG)) * Console->InvertedSize.Height;

                v3 Direction           = Camera->Base + Camera->Horizontal * u + Camera->Vertical * v;
                Camera->Rays[RayIndex] = CreateRay(Camera->Origin, Direction);

                color3 PixelColor = Color_Cyan;
            }
        }
    }

    Camera->IsInitialized = true;
}

local void
InitWorld(memory* Memory, world* World, console* Console)
{
    if (!World->IsInitialized) {
        World = GetStruct(Memory, world);

        World->ColliderSentinel       = GetStruct(Memory, collider);
        World->ColliderSentinel->Next = World->ColliderSentinel;
        World->ColliderSentinel->Prev = World->ColliderSentinel;

        SpawnEntities(Memory, World);

        World->IsInitialized = true;
    }
}

local b32
DetectCollisions(world* World, ray* Ray, f32 MinDistance, f32 MaxDistance, ray_hit_info* HitInfo)
{
    b32          HitDetected  = false;
    f32          ClosestSoFar = MaxDistance;
    ray_hit_info LatestHit;
    u32          x = true;

    for (collider* Collider = World->ColliderSentinel->Next;
         Collider != World->ColliderSentinel;
         Collider = Collider->Next) {

        if (RayHit(Ray, Collider->Sphere, MinDistance, ClosestSoFar, &LatestHit)) {
            ClosestSoFar = LatestHit.DistanceFromOrigin;
            if (Collider->Sphere.Radius != 100.0f) {
                x = false;
            }
            HitDetected = true;
        }
    }
    *HitInfo = LatestHit;

    return (HitDetected);
}

local color3
RayColor(world* World, ray Ray, rng_state* RNGState, s32 RecurseDepth)
{
    color3 Result = Color_Black;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (RecurseDepth > 0) {
        ray_hit_info HitInfo;
        f32          MinEpsilon = 0.001f;
        if (DetectCollisions(World, &Ray, MinEpsilon, 2000.0f, &HitInfo)) {
            p3 Target = HitInfo.Pos + HitInfo.Normal + RandomInUnitSphere(RNGState);
            // recursive land
            ray RecurseRay = CreateRay(HitInfo.Pos, Target - HitInfo.Pos);
            Result         = RayColor(World, RecurseRay, RNGState, RecurseDepth - 1) * 0.5f;
        }
        else {
            // No hit, draw background
            f32 Pos = 0.5f * (Ray.Direction.y + 1.0f);
            Result  = ColorLerp(RGB(1, 1, 1), RGB(0.5, 0.7, 1.0), Pos);
        }
    }

    return (Result);
}

MODULE_MAIN()
{
    world*    World = (world*)Memory->Data; // Always point at the base of memory
    rng_state RNGState;
    if (!World->IsInitialized) {
        InitWorld(Memory, World, Console);
        InitRandomizer(&RNGState, 123);
    }

    UpdateEntities(Input, World);

    camera* Camera = &World->MainCamera;
    if (!Camera->IsInitialized) {
        f32    AspectRatio = 16.0f / 9.0f;
        dim_2i ImageSize;
        ImageSize.Width     = 400;
        ImageSize.Height    = (s32)(ImageSize.Width / AspectRatio);
        s32 SamplesPerPixel = 50;
        InitCamera(Memory, Camera, Console, &RNGState, ImageSize, AspectRatio, SamplesPerPixel);
    }
    s32 MaxRecurseDepth = 50;

    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {

            // Supersampling
            color3 PixelColor = {};
            for (s32 Sample = 0;
                 Sample < Camera->SamplesPerPixel;
                 ++Sample) {

                u32 RayIndex = GetRayIndex(Console, X, Y, Sample, Camera->SamplesPerPixel);
                PixelColor += RayColor(World, Camera->Rays[RayIndex], &RNGState, MaxRecurseDepth);
            }

            SetColor(Console, V2I(X, Y), PixelColor, Camera->Scale);
        }
    }
}

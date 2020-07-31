#include "mod_weekend.h"
#include "console/console_update.c"

local void
SetColor(console* Console, v2i Coords, rgb Color)
{
    Color.r = (255.0f * Color.r);
    Color.g = (255.0f * Color.g);
    Color.b = (255.0f * Color.b);

    Point(Console, Coords, Color);
}

local void
InitRaycaster(memory* Memory, camera* Camera, console* Console)
{
    if (!Camera->IsInitialized) {
        // Init Console
        Camera->AspectRatio = 16.0f / 9.0f;
        Camera->ImageWidth  = 400;
        Camera->ImageHeight = (s32)(Camera->ImageWidth / Camera->AspectRatio);
        InitConsole(Console, Camera->ImageWidth, Camera->ImageHeight, Console_BottomUp, Color_White);

        // Init Camera

        Camera->ViewportHeight = 2.0f;
        Camera->ViewportWidth  = Camera->AspectRatio * Camera->ViewportHeight;
        Camera->FocalLength    = 1.0f;

        Camera->Origin          = V3(0, 0, 0);
        Camera->Horizontal      = V3(Camera->ViewportWidth, 0.0f, 0.0f);
        Camera->Vertical        = V3(0.0f, Camera->ViewportHeight, 0.0f);
        Camera->LowerLeftCorner = Camera->Origin
                                  - (Camera->Horizontal * 0.5f)
                                  - (Camera->Vertical * 0.5f)
                                  - V3(0.0f, 0.0f, Camera->FocalLength);
        Camera->Base = Camera->LowerLeftCorner - Camera->Origin;

        //NOTE: Init all rays
        Camera->Rays = (ray*)GetSize(Memory, sizeof(ray) * Console->PixelCount);
        for (s32 Y = 0;
             Y < Console->Size.Height;
             ++Y) {

            for (s32 X = 0;
                 X < Console->Size.Width;
                 ++X) {
                u32 Raypos = Y * Console->Size.Width + X;
                Assert(Raypos < Console->PixelCount);
                f32 u = X * Console->InvertedSize.Width;
                f32 v = Y * Console->InvertedSize.Height;

                v3 Direction         = Camera->Base + Camera->Horizontal * u + Camera->Vertical * v;
                Camera->Rays[Raypos] = CreateRay(Camera->Origin, Direction);

                color3 PixelColor = Color_Cyan;
            }
        }

        Camera->IsInitialized = true;
    }
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

local world*
InitWorld(memory* Memory, console* Console)
{
    world* Result = (world*)Memory->Data;
    if (!Result->IsInitialized) {
        Result = GetStruct(Memory, world);
        InitRaycaster(Memory, &Result->MainCamera, Console);

        Result->ColliderSentinel       = GetStruct(Memory, collider);
        Result->ColliderSentinel->Next = Result->ColliderSentinel;
        Result->ColliderSentinel->Prev = Result->ColliderSentinel;

        SpawnEntities(Memory, Result);

        Result->IsInitialized = true;
    }

    return (Result);
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
RayColor(world* World, ray* Ray)
{
    color3 Result = Color_Pink;

    ray_hit_info HitInfo;
    if (DetectCollisions(World, Ray, 0.0f, 2000.0f, &HitInfo)) {
        Result = (HitInfo.Normal + RGB(1, 1, 1)) * 0.5f;
    }
    else {
        // No hit, draw background
        f32 Pos = 0.5f * (Ray->Direction.y + 1.0f);
        Result  = ColorLerp(RGB(1, 1, 1), RGB(0.5, 0.7, 1.0), Pos);
    }

    return (Result);
}

local void
DrawScene(console* Console, world* World)
{
    camera* Camera = &World->MainCamera;
    Assert(Camera->IsInitialized);
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {
            u32 RayIndex = Y * Console->Size.Width + X;
            Assert(RayIndex < Console->PixelCount);

            color3 PixelColor = RayColor(World, Camera->Rays + RayIndex);

            SetColor(Console, V2I(X, Y), PixelColor);
        }
    }
}

MODULE_MAIN()
{
    world* World = InitWorld(Memory, Console);

    UpdateEntities(Input, World);

    DrawScene(Console, World);
}

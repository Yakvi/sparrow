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
InitRaycaster(camera* Camera, console* Console)
{
    if (!Camera->IsInitialized) {
        // Init Console
        Camera->AspectRatio = 16.0f / 9.0f;
        Camera->ImageWidth  = 400;
        Camera->ImageHeight = (s32)(Camera->ImageWidth / Camera->AspectRatio);
        InitConsole(Console, Camera->ImageWidth, Camera->ImageHeight, Console_TopDown, Color_White);

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

        Camera->IsInitialized = true;
    }
}

local color3
RayColor(world* World, ray* Ray)
{
    color3 Result       = Color_Pink;
    b32    PixelColored = false;

    for (u32 Index = 0; Index < ArrayCount(World->Spheres); ++Index) {
        sphere*      Sphere = World->Spheres + Index;
        ray_hit_info HitInfo;
        if (RayHit(Ray, Sphere, 0.0f, 20.0f, &HitInfo)) {

            Result = (HitInfo.Normal + RGB(1, 1, 1)) * 0.5f;

            PixelColored = true;
            break;
        }
    }

    if (!PixelColored) {
        f32 Pos = 0.5f * (Ray->Direction.y + 1.0f);
        Result  = ColorLerp(RGB(1, 1, 1), RGB(0.5, 0.7, 1.0), Pos);
    }

    return (Result);
}

local void
DrawScene(console* Console, world* World)
{
    camera Camera = World->MainCamera;
    Assert(Camera.IsInitialized);
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {
            f32 u = X * Console->InvertedSize.Width;
            f32 v = Y * Console->InvertedSize.Height;

            v3  Direction = Camera.Base + Camera.Horizontal * u + Camera.Vertical * v;
            ray Ray       = CreateRay(Camera.Origin, Direction);

            color3 PixelColor = RayColor(World, &Ray);

            SetColor(Console, V2I(X, Y), PixelColor);
        }
    }
}

local void
SpawnEntities(world* World)
{
    World->Spheres[0] = CreateSphere(V3(0, 0, -1), 0.5f);
}

local void
UpdateEntities(user_input* Input, world* World)
{
    sphere* Sphere   = World->Spheres;
    Sphere->Center.x = (Input->CursorNorm.x - 0.5f) * 4;
    Sphere->Center.y = -(Input->CursorNorm.y - 0.5f) * 4;
}

MODULE_MAIN(world* World)
{
    if (!World->MainCamera.IsInitialized) {
        InitRaycaster(&World->MainCamera, Console);
    }
    SpawnEntities(World);

    // UpdateEntities(Input, World);

    DrawScene(Console, World);
}

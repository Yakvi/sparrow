#include "mod_weekend.h"
#include "console/console_update.c"

local void
SetColor(console* Console, v2i Coords, rgb Color)
{
    Color.r = (255.999f * Color.r);
    Color.g = (255.999f * Color.g);
    Color.b = (255.999f * Color.b);

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

        Camera->IsInitialized = true;
    }
}

inline p3
RayAt(ray Ray, f32 At)
{
    p3 Result = Ray.Origin + (Ray.Direction * At);
    return (Result);
}

local color3
RayColor(world* World, ray Ray)
{
    color3 Result       = Color_Pink;
    b32    PixelColored = false;

    for (u32 Index = 0; Index < ArrayCount(World->Spheres); ++Index) {
        sphere* Sphere      = World->Spheres + Index;
        f32     HitDistance = Hit(Ray, Sphere, 0.0f, 20.0f);

        if (HitDistance > 0.0f) {
            p3 HitPos = RayAt(Ray, HitDistance);
            v3 Normal = Normalize(HitPos - Sphere->Center);
            Result    = RGB(Normal.x + 1.0f, Normal.y + 1.0f, Normal.z + 1.0f) * 0.5f;

            PixelColored = true;
            break;
        }
    }

    if (!PixelColored) {
        f32 Pos = 0.5f * (Ray.Direction.y + 1.0f);
        Result  = ColorLerp(RGB(1, 1, 1), RGB(0.5, 0.7, 1.0), Pos);
    }

    return (Result);
}

local void
DrawScene(console* Console, world* World)
{
    camera Camera = World->MainCamera;
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {
            f32 u = X * Console->InvertedSize.Width;
            f32 v = Y * Console->InvertedSize.Height;

            ray Raycast = CreateRay(Camera.Origin, (Camera.LowerLeftCorner
                                                    + Camera.Horizontal * u
                                                    + Camera.Vertical * v)
                                                       - Camera.Origin);

            color3 PixelColor = RayColor(World, Raycast);

            SetColor(Console, V2I(X, Y), PixelColor);
        }
    }
}

local void
SpawnEntities(world* World)
{
    World->Spheres[0] = CreateSphere(V3(0, 0, -5), 0.5f);
}

local void
UpdateEntities(user_input* Input, world* World)
{
    sphere* Sphere   = World->Spheres;
    Sphere->Center.x = -(Input->CursorNorm.x - 0.5f) * Sphere->Center.z * 2;
    Sphere->Center.y = (Input->CursorNorm.y - 0.5f) * Sphere->Center.z * 2;
}

MODULE_MAIN(world* World)
{
    if (!World->MainCamera.IsInitialized) {
        InitRaycaster(&World->MainCamera, Console);
        SpawnEntities(World);
    }

    UpdateEntities(Input, World);

    DrawScene(Console, World);
}

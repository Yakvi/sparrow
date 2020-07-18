#include "mod_weekend.h"
#include "console/console_update.c"

inline p3
RayAt(ray Ray, f32 Pos)
{
    p3 Result = Ray.Origin + (Ray.Direction * Pos);
    return (Result);
}

local color3
RayColor(ray Ray)
{
    f32 Pos = 0.5f * (Ray.Direction.y + 1.0f);
    color3 Result = ColorLerp(RGB(1.0f, 1.0f, 1.0f), RGB(0.5f, 0.7f, 1.0f), Pos);

    return (Result);
}

local void
SetColor(console* Console, v2i Coords, rgb Color)
{
    Color.r = (255.999f * Color.r);
    Color.g = (255.999f * Color.g);
    Color.b = (255.999f * Color.b);

    Point(Console, Coords, Color);
}

MODULE_MAIN(void* Memory)
{
    f32 AspectRatio = 16.0f / 9.0f;
    s32 ImageWidth = 384;
    s32 ImageHeight = static_cast<s32>(ImageWidth / AspectRatio);
    InitConsole(Console, ImageWidth, ImageHeight, Color_White);

    f32 ViewportHeight = 2.0f;
    f32 ViewportWidth = AspectRatio * ViewportHeight;
    f32 FocalLength = 1.0f;

    v3 Origin = V3(0, 0, 0);
    v3 Horizontal = V3(ViewportWidth, 0.0f, 0.0f);
    v3 Vertical = V3(0.0f, ViewportHeight, 0.0f);
    v3 LowerLeftCorner = Origin - Horizontal * 0.5f - Vertical * 0.5f - V3(0.0f, 0.0f, FocalLength);

    f32 InvConsoleWidth = 1.0f / (Console->Size.Width - 1);
    f32 InvConsoleHeight = 1.0f / (Console->Size.Height - 1);
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {
            f32 u = X * InvConsoleWidth;
            f32 v = Y * InvConsoleHeight;

            ray Ray = RAY(Origin, LowerLeftCorner + Horizontal * u + Vertical * v - Origin);

            SetColor(Console, V2I(X, Y), RayColor(Ray));
        }
    }
}

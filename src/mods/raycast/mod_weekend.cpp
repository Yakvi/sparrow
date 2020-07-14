#include "min_crt.h"
#include "sparrow_platform.h"
#include "sparrow_math.h"
#include "console/console_platform.h"

#include "console/console_update.c"

MODULE_MAIN(void* Memory)
{
    InitConsole(Console, 256, 256, Color_White);

    color PixelColor;
    for (u32 Y = 0;
         Y < (s32)Console->Size.Height;
         ++Y) {
        for (u32 X = 0;
             X < (s32)Console->Size.Width;
             ++X) {
            f64 r = X / Sin((f32)(Console->Size.Width - 1));
            f64 g = Y / Cos((f32)(Console->Size.Height - 1));
            f64 b = Log((f32)X + Y);
            r += Exp((f32)X - Y);

            PixelColor.r = static_cast<f32>(255.999 * r);
            PixelColor.g = static_cast<f32>(255.999 * g);
            PixelColor.b = static_cast<f32>(255.999 * b);

            Point(Console, V2I(X, Y), PixelColor);
        }
    }
}

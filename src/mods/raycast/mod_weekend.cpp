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
            f64 r = double(X) / (Console->Size.Width - 1);
            f64 g = double(Y) / (Console->Size.Height - 1);
            f64 b = 0.25;

            PixelColor.r = static_cast<f32>(255.999 * r);
            PixelColor.g = static_cast<f32>(255.999 * g);
            PixelColor.b = static_cast<f32>(255.999 * b);

            Point(Console, V2I(X, Y), PixelColor);
        }
    }
}

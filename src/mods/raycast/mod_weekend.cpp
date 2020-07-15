#include "min_crt.h"
#include "core/sparrow_platform.h"
#include "math/sparrow_math.h"
#include "console/console_platform.h"
#include "console/console_update.c"

MODULE_MAIN(void* Memory)
{
    InitConsole(Console, 256, 256, Color_White);

    color PixelColor;
    for (s32 Y = 0;
         Y < Console->Size.Height;
         ++Y) {
        for (s32 X = 0;
             X < Console->Size.Width;
             ++X) {
            f64 r = 0;
            f64 g = Sin(Pi32 - (Y + 2));
            f64 b = Cos(Pi32 - (X + 2));

            PixelColor.r = static_cast<f32>(255.999 * r);
            PixelColor.g = static_cast<f32>(255.999 * g);
            PixelColor.b = static_cast<f32>(255.999 * b);

            Point(Console, V2I(X, Y), PixelColor);
        }
    }
}

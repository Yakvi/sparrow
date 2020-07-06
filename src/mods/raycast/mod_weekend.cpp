#include "min_crt.h"
#include "types.h"
#include "vector.h"
#include "sparrow_platform.h"
#include "console/console_platform.h"

#include "console/console_update.c"

MODULE_MAIN(void* Memory)
{
    InitConsole(Console, 256, 256, Color_White);

}

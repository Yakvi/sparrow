#include "../../min_crt.h"
#include "../../sparrow_platform.h"
#include "../../console/console_update.c"

MODULE_MAIN(void* Memory)
{
    InitConsole(Console, 256, 256, Color_White);
    TextBox(Console, V2I(10, 10), Color_Gray11, "Hello Raycaster!", Color_Black);
}

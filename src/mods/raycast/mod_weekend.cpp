#include "../../min_crt.h"
#include "../../sparrow_platform.h"
#include "../../console/console_update.c"

void
ModuleMain(void* Memory, struct user_input* Input, struct pixel* Pixels)
{
    TextBox(Pixels, V2I(10, 40), Color_Gray11, "Hello Raycaster!", Color_Black);
}

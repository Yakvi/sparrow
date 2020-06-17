#include "../min_crt.h"
#include "../sparrow_platform.h"
#include "../console/sparrow_console_update.c"

void
ModuleMain(void* Memory, struct user_input* Input, struct pixel* Pixels)
{
    TextBox(Pixels, (v2i){10, 40}, (color)Color_Gray11, "Hello World", (color)Color_Black);
}

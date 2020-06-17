#include "writer.h"
#include "../console/sparrow_console_update.c"

void
ModuleMain(struct writer_state* State, struct user_input* Input, struct pixel* Pixels)
{
    TextBox(Pixels, (v2i){10, 40}, (color)Color_Gray11, "Hello Writer!", (color)Color_Black);
}

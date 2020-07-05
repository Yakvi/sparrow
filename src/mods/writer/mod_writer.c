#include "writer.h"
#include "../../console/console_update.c"

MODULE_MAIN(struct writer_state* State)
{
    TextBox(Console, (v2i){10, 10}, Color_Gray11, "Hello Writer!", Color_Black);
}

#if !defined(SPARROW_CONSOLE_GLYPHS_H)

#define GLYPH_WIDTH 8
#define GLYPH_HEIGHT 8
struct glyph
{
    struct dim_2d Dim;
    char* Data;
};

local struct glyph
GetGlyph(char* Code)
{
    struct glyph Result = {0};
    Result.Dim = (struct dim_2d){GLYPH_WIDTH, GLYPH_HEIGHT};
    switch (*Code) {
        case '0': {
            Result.Data =
                "        "
                "  ###   "
                " #   #  "
                "#     # "
                "#     # "
                "#     # "
                " #   #  "
                "  ###   ";
        } break;
        case '1': {
            Result.Data =
                "        "
                "   #    "
                "  ##    "
                " # #    "
                "   #    "
                "   #    "
                "   #    "
                " #####  ";
        } break;
        case '2': {
            Result.Data =
                "        "
                " #####  "
                "#     # "
                "      # "
                " #####  "
                "#       "
                "#       "
                "####### ";
        } break;
        case '3': {
            Result.Data =
                "        "
                " #####  "
                "#     # "
                "      # "
                " #####  "
                "      # "
                "#     # "
                " #####  ";
        } break;
        case '4': {
            Result.Data =
                "        "
                "#       "
                "#    #  "
                "#    #  "
                "#    #  "
                "####### "
                "     #  "
                "     #  ";
        } break;
        case '5': {
            Result.Data =
                "        "
                "####### "
                "#       "
                "#       "
                "######  "
                "      # "
                "#     # "
                " #####  ";
        } break;
        case '6': {
            Result.Data =
                "        "
                " #####  "
                "#     # "
                "#       "
                "######  "
                "#     # "
                "#     # "
                " #####  ";
        } break;
        case '7': {
            Result.Data =
                "        "
                "####### "
                "#    #  "
                "    #   "
                "   #    "
                "  #     "
                "  #     "
                "  #     ";
        } break;
        case '8': {
            Result.Data =
                "        "
                " #####  "
                "#     # "
                "#     # "
                " #####  "
                "#     # "
                "#     # "
                " #####  ";
        } break;
        case '9': {
            Result.Data =
                "        "
                " #####  "
                "#     # "
                "#     # "
                " ###### "
                "      # "
                "#     # "
                " #####  ";
        } break;
        case 'A': {
            Result.Data =
                "   #    "
                "  # #   "
                " #   #  "
                "#     # "
                "####### "
                "#     # "
                "#     # "
                "        ";
        } break;

        case 'B': {
            Result.Data =
                "######  "
                "#     # "
                "#     # "
                "######  "
                "#     # "
                "#     # "
                "######  "
                "        ";
        } break;
        case 'C': {
            Result.Data =
                " #####  "
                "#     # "
                "#       "
                "#       "
                "#       "
                "#     # "
                " #####  "
                "        ";
        } break;
        case 'D': {
            Result.Data =
                "######  "
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                "######  "
                "        ";
        } break;
        case 'E': {
            Result.Data =
                "####### "
                "#       "
                "#       "
                "#####   "
                "#       "
                "#       "
                "####### "
                "        ";
        } break;
        case 'F': {
            Result.Data =
                "####### "
                "#       "
                "#       "
                "#####   "
                "#       "
                "#       "
                "#       "
                "        ";
        } break;
        case 'G': {
            Result.Data =
                " #####  "
                "#     # "
                "#       "
                "#  #### "
                "#     # "
                "#     # "
                " #####  "
                "        ";
        } break;
        case 'H': {
            Result.Data =
                "#     # "
                "#     # "
                "#     # "
                "####### "
                "#     # "
                "#     # "
                "#     # "
                "        ";
        } break;
        case 'I': {
            Result.Data =
                "####### "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "####### "
                "        ";
        } break;
        case 'J': {
            Result.Data =
                "      # "
                "      # "
                "      # "
                "      # "
                "#     # "
                "#     # "
                " #####  "
                "        ";
        } break;
        case 'K': {
            Result.Data =
                "#    #  "
                "#   #   "
                "#  #    "
                "###     "
                "#  ##   "
                "#    #  "
                "#     # "
                "        ";
        } break;
        case 'L': {
            Result.Data =
                "#       "
                "#       "
                "#       "
                "#       "
                "#       "
                "#       "
                "####### "
                "        ";
        } break;
        case 'M': {
            Result.Data =
                "#     # "
                "##   ## "
                "# # # # "
                "#  #  # "
                "#     # "
                "#     # "
                "#     # "
                "        ";
        } break;
        case 'N': {
            Result.Data =
                "#     # "
                "##    # "
                "# #   # "
                "#  #  # "
                "#   # # "
                "#    ## "
                "#     # "
                "        ";
        } break;
        case 'O': {
            Result.Data =
                "####### "
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                "####### "
                "        ";
        } break;
        case 'P': {
            Result.Data =
                "######  "
                "#     # "
                "#     # "
                "######  "
                "#       "
                "#       "
                "#       "
                "        ";
        } break;
        case 'Q': {
            Result.Data =
                " #####  "
                "#     # "
                "#     # "
                "#     # "
                "#   # # "
                "#    #  "
                " #### # "
                "        ";
        } break;
        case 'R': {
            Result.Data =
                "######  "
                "#     # "
                "#     # "
                "######  "
                "#   #   "
                "#    #  "
                "#     # "
                "        ";
        } break;
        case 'S': {
            Result.Data =
                " #####  "
                "#     # "
                "#       "
                " #####  "
                "      # "
                "#     # "
                " #####  "
                "        ";
        } break;
        case 'T': {
            Result.Data =
                "####### "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "        ";
        } break;
        case 'U': {
            Result.Data =
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                " #####  "
                "        ";
        } break;
        case 'V': {
            Result.Data =
                "#     # "
                "#     # "
                "#     # "
                "#     # "
                " #   #  "
                "  # #   "
                "   #    "
                "        ";
        } break;
        case 'W': {
            Result.Data =
                "#     # "
                "#  #  # "
                "#  #  # "
                "#  #  # "
                "#  #  # "
                "#  #  # "
                " ## ##  "
                "        ";
        } break;
        case 'X': {
            Result.Data =
                "#     # "
                " #   #  "
                "  # #   "
                "   #    "
                "  # #   "
                " #   #  "
                "#     # "
                "        ";
        } break;
        case 'Y': {
            Result.Data =
                "#     # "
                " #   #  "
                "  # #   "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "        ";
        } break;
        case 'Z': {
            Result.Data =
                "####### "
                "     #  "
                "    #   "
                "   #    "
                "  #     "
                " #      "
                "####### "
                "        ";
        } break;
        case 'a': {
            Result.Data =
                "        "
                "   ##   "
                "  #  #  "
                " #    # "
                " ###### "
                " #    # "
                " #    # "
                "        ";
        } break;
        case 'b': {
            Result.Data =
                "        "
                " #####  "
                " #    # "
                " #####  "
                " #    # "
                " #    # "
                " #####  "
                "        ";
        } break;
        case 'c': {
            Result.Data =
                "        "
                "  ####  "
                " #    # "
                " #      "
                " #      "
                " #    # "
                "  ####  "
                "        ";
        } break;
        case 'd': {
            Result.Data =
                "        "
                " #####  "
                " #    # "
                " #    # "
                " #    # "
                " #    # "
                " #####  "
                "        ";
        } break;
        case 'e': {
            Result.Data =
                "        "
                " ###### "
                " #      "
                " #####  "
                " #      "
                " #      "
                " ###### "
                "        ";
        } break;
        case 'f': {
            Result.Data =
                "        "
                " ###### "
                " #      "
                " #####  "
                " #      "
                " #      "
                " #      "
                "        ";
        } break;
        case 'g': {
            Result.Data =
                "        "
                "  ####  "
                " #    # "
                " #      "
                " #  ### "
                " #    # "
                "  ####  "
                "        ";
        } break;
        case 'h': {
            Result.Data =
                "        "
                " #    # "
                " #    # "
                " ###### "
                " #    # "
                " #    # "
                " #    # "
                "        ";
        } break;
        case 'i': {
            Result.Data =
                "        "
                " #####  "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                " #####  "
                "        ";
        } break;
        case 'j': {
            Result.Data =
                "        "
                "      # "
                "      # "
                "      # "
                "      # "
                " #    # "
                "  ####  "
                "        ";
        } break;
        case 'k': {
            Result.Data =
                "        "
                " #    # "
                " #   #  "
                " ####   "
                " #  #   "
                " #   #  "
                " #    # "
                "        ";
        } break;
        case 'l': {
            Result.Data =
                "        "
                " #      "
                " #      "
                " #      "
                " #      "
                " #      "
                " ###### "
                "        ";
        } break;
        case 'm': {
            Result.Data =
                "        "
                " #    # "
                " ##  ## "
                " # ## # "
                " #    # "
                " #    # "
                " #    # "
                "        ";
        } break;
        case 'n': {
            Result.Data =
                "        "
                " #    # "
                " ##   # "
                " # #  # "
                " #  # # "
                " #   ## "
                " #    # "
                "        ";
        } break;
        case 'o': {
            Result.Data =
                "        "
                "  ####  "
                " #    # "
                " #    # "
                " #    # "
                " #    # "
                "  ####  "
                "        ";
        } break;
        case 'p': {
            Result.Data =
                "        "
                " #####  "
                " #    # "
                " #    # "
                " #####  "
                " #      "
                " #      "
                "        ";
        } break;
        case 'q': {
            Result.Data =
                "        "
                "  ####  "
                " #    # "
                " #    # "
                " #  # # "
                " #   #  "
                "  ### # "
                "        ";
        } break;
        case 'r': {
            Result.Data =
                "        "
                " #####  "
                " #    # "
                " #    # "
                " #####  "
                " #   #  "
                " #    # "
                "        ";
        } break;
        case 's': {
            Result.Data =
                "        "
                "  ####  "
                " #      "
                "  ####  "
                "      # "
                " #    # "
                "  ####  "
                "        ";
        } break;
        case 't': {
            Result.Data =
                "        "
                " #####  "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "        ";
        } break;
        case 'u': {
            Result.Data =
                "        "
                " #    # "
                " #    # "
                " #    # "
                " #    # "
                " #    # "
                "  ####  "
                "        ";
        } break;
        case 'v': {
            Result.Data =
                "        "
                " #    # "
                " #    # "
                " #    # "
                " #    # "
                "  #  #  "
                "   ##   "
                "        ";
        } break;
        case 'w': {
            Result.Data =
                "        "
                " #    # "
                " #    # "
                " #    # "
                " # ## # "
                " ##  ## "
                " #    # "
                "        ";
        } break;
        case 'x': {
            Result.Data =
                "        "
                " #    # "
                "  #  #  "
                "   ##   "
                "   ##   "
                "  #  #  "
                " #    # "
                "        ";
        } break;
        case 'y': {
            Result.Data =
                "        "
                " #   #  "
                "  # #   "
                "   #    "
                "   #    "
                "   #    "
                "   #    "
                "        ";
        } break;
        case 'z': {
            Result.Data =
                "        "
                " ###### "
                "     #  "
                "    #   "
                "   #    "
                "  #     "
                " ###### "
                "        ";
        } break;
        case '.': {
            Result.Data =
                "        "
                "        "
                "        "
                "        "
                "        "
                " ###    "
                " ###    "
                " ###    ";
        } break;
        case ',': {
            Result.Data =
                "        "
                "        "
                "        "
                "        "
                " ###    "
                " ###    "
                "  #     "
                " #      ";
        } break;
        case '!': {
            Result.Data =
                " ###    "
                " ###    "
                " ###    "
                "  #     "
                "        "
                " ###    "
                " ###    ";
        } break;
        case '?': {
            Result.Data =
                " #####  "
                "#     # "
                "      # "
                "   ###  "
                "   #    "
                "        "
                "   #    ";
        } break;
        case ':': {
            Result.Data =
                "  #     "
                " ###    "
                "  #     "
                "        "
                "  #     "
                " ###    "
                "  #     ";
        } break;
        case ';': {
            Result.Data =
                "        "
                " ###    "
                " ###    "
                "        "
                " ###    "
                " ###    "
                "  #     "
                " #      ";
        } break;
        case '(': {
            Result.Data =
                "   ##   "
                "  #     "
                " #      "
                " #      "
                " #      "
                "  #     "
                "   ##   ";
        } break;
        case ')': {
            Result.Data =
                " ##     "
                "   #    "
                "    #   "
                "    #   "
                "    #   "
                "   #    "
                " ##     ";
        } break;
        case '{': {
            Result.Data =
                "   ###  "
                "  #     "
                "  #     "
                " ##     "
                "  #     "
                "  #     "
                "   ###  ";
        } break;
        case '}': {
            Result.Data =
                " ###    "
                "    #   "
                "    #   "
                "    ##  "
                "    #   "
                "    #   "
                " ###    ";
        } break;
        case '[': {
            Result.Data =
                " #####  "
                " #      "
                " #      "
                " #      "
                " #      "
                " #      "
                " #####  ";
        } break;
        case ']': {
            Result.Data =
                " #####  "
                "     #  "
                "     #  "
                "     #  "
                "     #  "
                "     #  "
                " #####  ";
        } break;
        case ' ': {
            Result.Data =
                "        "
                "        "
                "        "
                "        "
                "        "
                "        "
                "        "
                "        ";
        } break;
        case '#': {
            Result.Data =
                "  # #   "
                "  # #   "
                "####### "
                "  # #   "
                "####### "
                "  # #   "
                "  # #   ";
        } break;
        case '|': {
            Result.Data =
                " #      "
                " #      "
                " #      "
                "        "
                " #      "
                " #      "
                " #      ";
        } break;

            InvalidDefaultCase;
    }
    return (Result);
}

#define SPARROW_CONSOLE_GLYPHS_H
#endif

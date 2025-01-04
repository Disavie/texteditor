#ifndef TWILIGHTSKY
#define TWILIGHTSKY

#include "colors.h"

const short twilightSky[] = {
    COLOR_WHITE,           // text_color
    NONE,                  // bg_file_color (transparent background)
    NONE,                  // bg_unused_color (transparent background)
    COLOR_PURPLE_VIOLET,   // comment_color
    COLOR_GRAY_3,          // border_color
    COLOR_RED_CRIMSON,     // error_color
    COLOR_YELLOW_SUNSHINE, // status_bar_bg_color
    COLOR_GRAY_8,          // status_bar_text_color
    COLOR_GREEN_JADE,      // mode_color
    COLOR_BLUE_NAVY,       // datatype keyword
    COLOR_PURPLE_AMETHYST, // control flow keyword
    COLOR_BLUE_SKY,        // function keyword
    COLOR_YELLOW_LEMON,    // misc keywords
    COLOR_ORANGE_TANGERINE,// strings
    COLOR_PURPLE_VIOLET,   // preprocessor
    COLOR_MINT_GREEN,      // numbers
};
#endif

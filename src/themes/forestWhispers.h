#ifndef FORESTWHISPERS
#define FORESTWHISPERS

#include "colors.h"

const short forestWhispers[] = {
    COLOR_GRAY_14,         // text_color
    NONE,                  // bg_file_color (transparent background)
    NONE,                  // bg_unused_color (transparent background)
    COLOR_GREEN_MOSS,      // comment_color
    COLOR_BLUE_TURQUOISE,  // border_color
    COLOR_RED_RUBY,        // error_color
    COLOR_YELLOW_SUNSHINE, // status_bar_bg_color
    COLOR_GRAY_16,         // status_bar_text_color
    COLOR_GREEN_LIME,      // mode_color
    COLOR_BLUE_SKY,        // datatype keyword
    COLOR_PURPLE_LAVENDER, // control flow keyword
    COLOR_BLUE_OCEAN,      // function keyword
    COLOR_GREEN_SPRING,    // misc keywords
    COLOR_ORANGE_BRIGHT,   // strings
    COLOR_PURPLE_VIOLET,   // preprocessor
    COLOR_MINT_GREEN,      // numbers
};

#endif

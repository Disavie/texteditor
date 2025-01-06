#ifndef DEFAULT2
#define DEFAULT2

#include "colors.h"

const short default2[] = {
    COLOR_WHITE,   // text_color (bright white for general text)
    NONE,                 // bg_file_color (uses terminal's default background color)
    NONE,                 // bg_unused_color (uses terminal's default background color)
    COLOR_GRAY_12,        // comment_color (grayish color for comments)
    COLOR_GRAY_2,         // border_color (dark gray for borders)
    COLOR_RED_RUBY,    // error_color (red for errors)
    COLOR_GRAY_5,         // status_bar_bg_color (very dark gray for status bar)
    COLOR_GRAY_22,        // status_bar_text_color (light gray for status bar text)
    COLOR_MINT_GREEN,// mode_color
    COLOR_LIGHT_BLUE, //datatyoe keyword
    COLOR_PURPLE_SOFT_LAVENDER, //control flow keyword
    COLOR_PURPLE_SOFT_LAVENDER, //function keyword
    COLOR_BLUE_JEAN, //misc keywords
    COLOR_ORANGE_LIGHT, //strings
    COLOR_PURPLE_SOFT_LAVENDER, //preprocessor
    COLOR_FADED_GREEN,
    COLOR_ORANGE_LIGHT, //anglebrackets
};

#endif

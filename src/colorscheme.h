#ifndef COLORSCHEME
#define COLORSCHEME

const short colors[] = {
    252,  // text_color
    -1,   // bg_file_color
    -1,   // bg_unused_color
    242,  // comment_color
    252,  // border_color
    161,  // error_color
    103,  // status_bar_bg_color
    233,   // status_bar_text_color
    36    // green 
};

const short gptcolors[] = {
    250,  // text_color
    17,   // bg_file_color
    236,  // bg_unused_color
    243,  // comment_color
    51,   // border_color
    196,  // error_color
    226,  // status_bar_bg_color
    16,    // status_bar_text_color
    36    // green 
};

const short gptcolors2[] = {
    255,  // text_color (bright white for general text)
    -1,   // bg_file_color (uses terminal's default background color)
    -1,   // bg_unused_color (uses terminal's default background color)
    242,  // comment_color (grayish color for comments)
    233,  // border_color (dark gray for borders)
    196,  // error_color (red for errors)
    235,  // status_bar_bg_color (very dark gray for status bar)
    254,   // status_bar_text_color (light gray for status bar text)
    36,   //green

};


#endif

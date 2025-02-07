#include "colorscheme.h"
#include "themes/default1.h"
#include "themes/default2.h"
#include "themes/default3.h"
#include "themes/default4.h"
#include "themes/default5.h"
#include "themes/gptcolors.h"
#include "themes/twilightSky.h"
#include "themes/midnightBloom.h"
#include "themes/chillz.h"
#include "themes/retro_orange.h"
#include "themes/retro_green.h"
#include "themes/light.h"
#include "themes/dark.h"

const Theme zthemes[] = {
    {"default1", default1},
    {"default2", default2},
    {"default3", default3},
    {"default4", default4},
    {"default5", default5},
    {"gptcolors", gptcolors},
    {"twilightSky", twilightSky},
    {"midnightBloom", midnightBloom},
    {"chillz", chillz},
    {"retro_orange", retro_orange},
    {"retro_green", retro_green},
    {"light", light},
    {"dark", dark},
};

// Define the count of themes
const size_t zthemecount = sizeof(zthemes) / sizeof(zthemes[0]);


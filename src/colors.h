#ifndef COLORS
#define COLORS
typedef enum {
    
    NONE = -1,
    RESET = 0,

    // Grayscale (ANSI 232–255)
    COLOR_GRAY_1 = 232,  // Darkest gray
    COLOR_GRAY_2 = 233,
    COLOR_GRAY_3 = 234,
    COLOR_GRAY_4 = 235,
    COLOR_GRAY_5 = 236,
    COLOR_GRAY_6 = 237,
    COLOR_GRAY_7 = 238,
    COLOR_GRAY_8 = 239,
    COLOR_GRAY_9 = 240,
    COLOR_GRAY_10 = 241,
    COLOR_GRAY_11 = 242,
    COLOR_GRAY_12 = 243,
    COLOR_GRAY_13 = 244,
    COLOR_GRAY_14 = 245,
    COLOR_GRAY_15 = 246,
    COLOR_GRAY_16 = 247,
    COLOR_GRAY_17 = 248,
    COLOR_GRAY_18 = 249,
    COLOR_GRAY_19 = 250,
    COLOR_GRAY_20 = 251,
    COLOR_GRAY_21 = 252,
    COLOR_GRAY_22 = 253,
    COLOR_GRAY_23 = 254,
    COLOR_WHITE = 255,  // Brightest white

    // Reds
    COLOR_RED_BRIGHT = 1,
    COLOR_RED_LIGHT = 9,
    COLOR_RED_FLAME = 88,
    COLOR_RED_CRIMSON = 124,
    COLOR_RED_RUBY = 196,
    COLOR_RED_ROSE = 160,
    COLOR_RED_DARK = 52,

    // Greens
    COLOR_GREEN_BRIGHT = 2,
    COLOR_GREEN_LIGHT = 10,
    COLOR_GREEN_FOREST = 22,
    COLOR_GREEN_JADE = 28,
    COLOR_GREEN_LIME = 46,
    COLOR_GREEN_SPRING = 34,
    COLOR_GREEN_OLIVE = 58,
    COLOR_GREEN_MOSS = 100,
    COLOR_MINT_GREEN = 36,
    COLOR_MINT_GREEN2 = 42,

    // Blues
    COLOR_BLUE_BRIGHT = 4,
    COLOR_BLUE_LIGHT = 12,
    COLOR_BLUE_SKY = 33,
    COLOR_BLUE_OCEAN = 19,
    COLOR_BLUE_COBALT = 21,
    COLOR_BLUE_JEAN = 25,
    COLOR_LIGHT_BLUE = 39,
    COLOR_BLUE_NAVY = 17,
    COLOR_BLUE_TURQUOISE = 122,

    // Yellows
    COLOR_YELLOW_BRIGHT = 3,
    COLOR_YELLOW_LIGHT = 11,
    COLOR_YELLOW_GOLD = 226,
    COLOR_YELLOW_SUNSHINE = 148,
    COLOR_YELLOW_LEMON = 190,
    COLOR_YELLOW_MUSTARD = 94,

    // Oranges
    COLOR_ORANGE_BRIGHT = 214,
    COLOR_ORANGE_LIGHT = 215,
    COLOR_ORANGE_RUST = 202,
    COLOR_ORANGE_TANGERINE = 208,
    COLOR_ORANGE_CARAMEL = 130,

    // Purples
    COLOR_PURPLE_BRIGHT = 13,
    COLOR_PURPLE_LAVENDER = 141,
    COLOR_PURPLE_SOFT_LAVENDER = 139,
    COLOR_PURPLE_ORCHID = 171,
    COLOR_PURPLE_PLUM = 90,
    COLOR_PURPLE_VIOLET = 129,
    COLOR_PURPLE_AMETHYST = 93,
    COLOR_PURPLE_DEEP = 92,

    // Pinks
    COLOR_PINK_LIGHT = 218,
    COLOR_PINK_BUBBLEGUM = 219,
    COLOR_PINK_CORAL = 210,
    COLOR_PINK_ROSE = 205,
    COLOR_PINK_FUCHSIA = 199,
    COLOR_PINK_HOT = 201,

    // Browns
    COLOR_BROWN_LIGHT = 130,
    COLOR_BROWN_CLAY = 95,
    COLOR_BROWN_CARAMEL = 136,
    COLOR_BROWN_UMBER = 58,
    COLOR_BROWN_WOOD = 94,

    // Teals, Turquoises, and Aquas
    COLOR_TURQUOISE_BRIGHT = 51,
    COLOR_TURQUOISE_LIGHT = 122,
    COLOR_TURQUOISE_MIDNIGHT = 43,
    COLOR_AQUA_BRIGHT = 14,
    COLOR_AQUA_SOFT = 44,
    COLOR_TEAL_DARK = 23,
    COLOR_TEAL_LIGHT = 30,

    // Special and Extended Colors
    COLOR_PEACH = 215,
    COLOR_MAROON = 88,
    COLOR_INDIGO = 54,
    COLOR_TAN = 180,
    COLOR_CHAMPAGNE = 223,
    COLOR_SALMON = 217,
    COLOR_TERRACOTTA = 166,
    COLOR_CREAM = 187,
    COLOR_IVORY = 230,
    COLOR_CHARCOAL = 232,
    COLOR_ICE = 195,
    COLOR_SAGE = 108,
    COLOR_LIME = 82,
    COLOR_CYAN = 6,

    // Metallics
    COLOR_GOLD_METAL = 220,
    COLOR_SILVER_METAL = 249,
    COLOR_BRONZE_METAL = 136,
    COLOR_EMERALD = 34,
    COLOR_RUBY = 160,
    COLOR_AMBER = 214

} TColors;


#endif

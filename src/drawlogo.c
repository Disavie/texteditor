#include "mytui.h"
#include "ansihelpers.h"


#include <stdio.h>
#include <string.h>

void print_with_offset(const char *line, int xOffset) {
    for (int i = 0; i < xOffset; i++) {
      moveright();
    }
    printf("%s\n", line);
}

void drawLogo(int HEIGHT, int WIDTH,const short colors[]) {

    setTextColor(colors[0]);
    setBgColor(colors[2]);

    // Define the ASCII art lines
    const char *art[] = {
        "             Zep v0.0.19",
        "  Open Source Terminal Text Editor",
        "",
        "         @@PMTTTTTTTT]@@@@M",
        "        j@          g@@@P",
        "        ?`        ,@@@@M",
        "                 g@@@P",
        "               ,@@@@|",
        "         TTTTT$@@@MTTTTTTT",
        "            ,@@@@\"",
        "           g@@@M",
        "         ,@@@@T           ,E",
        "        @@@@E            y@",
        "      y@@@@mmmmmmmmmmm@@@@@",
    };

    int art_height = sizeof(art) / sizeof(art[0]);
    int art_width = 0;

    // Find the width of the longest line in the ASCII art
    for (int i = 0; i < art_height; i++) {
        int line_len = strlen(art[i]);
        if (line_len > art_width) {
            art_width = line_len;
        }
    }

    // Calculate xOffset and yOffset
    int xOffset = (WIDTH - art_width) / 2;
    int yOffset = (HEIGHT - art_height) / 2;

    // Print the ASCII art with the offsets
    for (int i = 0; i < yOffset; i++) {
        printf("\n");  // Printing blank lines to center vertically
    }

    for (int i = 0; i < art_height; i++) {
        print_with_offset(art[i], xOffset);
    }

    setTextColor(0);
    setBgColor(0);
}


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

void drawLogo(int HEIGHT, int WIDTH, short colors[]) {

    setTextColor(colors[0]);
    setBgColor(colors[2]);

    // Define the ASCII art lines
    const char *art[] = {
        "                         Zep v0.0.15",
        "               Open source, ANSI-code text editor",
        "",
        "                     @@PMTTTTTTTT]@@@@M",
        "                    j@          g@@@P",
        "                    ?`        ,@@@@M",
        "                             g@@@P",
        "                           ,@@@@|",
        "                     TTTTT$@@@MTTTTTTT",
        "                        ,@@@@\"",
        "                       g@@@M",
        "                     ,@@@@T           ,E",
        "                    @@@@E            y@",
        "                  y@@@@mmmmmmmmmmm@@@@@",
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



void create_window_inoutRANGE(int startx, int starty, int win_height,int max_width, char ** args,int yIn, int xIn,size_t linecount,short colors[]){ 

    setlocale(LC_CTYPE,"");
    //BORDERS
    const char DEF = ' ';
    wchar_t BLOCK= 0x2588;
    wchar_t VERTICAL_BORDER= 0x2502;
    wchar_t HORIZONTAL_BORDER= 0x2500;
    wchar_t TOPLEFTCORNER = 0x250C;
    wchar_t TOPRIGHTCORNER = 0x2510;
    wchar_t BOTTOMLEFTCORNER = 0x2514;
    wchar_t BOTTOMRIGHTCORNER = 0x2518;

    short text_color = colors[0];
    short bg_file_color = colors[1];
    short bg_unused_color = colors[2]; 
    short comment_color = colors[3];
    short border_color = colors[4]; 
    
    short tab_width = 4;
    move00();
    hidecursor();

    setBgColor(bg_file_color);


    int y = 0;
    //setting the starty;
    while(y < starty){
        printf("%d\n",y);
        y++;
    }
    int x = 0;
    char letter;
    for(int i = yIn-1 ; i < yIn+win_height+1 ; i++){
        short isReal = 0;
        while(x < startx){
            printf(" ");
            x++;
        }
        size_t len = 0;
        if(i >= 0 && i < yIn+win_height){
            if(i < linecount){
                isReal = 1;
                len = strlen(args[i]);
                setBgColor(bg_file_color);
            }else{
                setBgColor(bg_unused_color);
            }

        }
        if(i == yIn-1){
            setTextColor(border_color);
            printf("%lc",TOPLEFTCORNER);
            for(int i = 0 ; i < 5 ; i++)
                printf("%lc",HORIZONTAL_BORDER);
            setTextColor(0);
        }else if(i == yIn+win_height){
            setTextColor(border_color);
            printf("%lc",BOTTOMLEFTCORNER);
            for(int i = 0 ; i < 5 ; i++)
                printf("%lc",HORIZONTAL_BORDER);
            setTextColor(0);
        }else{
            setTextColor(border_color);
            printf("%lc",VERTICAL_BORDER);
            setTextColor(0);
            if(isReal){
                setTextColor(comment_color);
                printf("%4d ",i+1);
                setTextColor(0);
            }else{
                setTextColor(comment_color);
                printf("   ~ ");
                setTextColor(0);
            }
        }

    


        for(int j = xIn; j < xIn+max_width ; j++){
            if( i == yIn-1){
                setTextColor(border_color);
                printf("%lc",HORIZONTAL_BORDER);
                setTextColor(0);
            }else if ( i == yIn+win_height){
                setTextColor(border_color);
                printf("%lc",HORIZONTAL_BORDER);
                setTextColor(0);
            }else{
                if(j < len){
                    setTextColor(text_color);
                    if(args[i][j] == '\t'){
                      for(int i = 0 ; i < tab_width ; i++){
                        printf(" ");
                      }
                    }else{
                      printf("%c",args[i][j]);
                    }
                    setTextColor(0);
                }else{
                    printf("%c",DEF);
                }
            }
        }
        x = 0;
        if(i == yIn -1){
            setTextColor(border_color);
            printf("%lc",TOPRIGHTCORNER );
            setTextColor(0);
        }else if(i == yIn+win_height){
            setTextColor(border_color);
            printf("%lc",BOTTOMRIGHTCORNER);
            setTextColor(0);
        }else{
            setTextColor(border_color);
            printf("%lc\n",VERTICAL_BORDER);
            setTextColor(0);
        }
            
    }
    showcursor();
    setTextColor(0);
    setBgColor(0);
}

void drawStatusBar(char * text,  int width , short colors[],short isError){

  setTextColor(colors[2]);
  if(isError) setTextColor(colors[5]);
  setBgColor(colors[0]);


  int i = 0;
  while( i < width ){
    if( i < strlen(text))
      printf("%c",text[i]);
    else{
      printf(" ");
    }
    i++;
  }
  setTextColor(0);
  setBgColor(0);
}


void update_statusbad(char * words,int widith, int height, char ** modes, char mode, int cRow, int cCol, int yOffset,char * filename,short colors[],short isError){

      hidecursor();

      char text[widith];
      strcpy(text," ");

      if(words[0] == '\0'){
          if(mode == 'n')
            strcat(text,modes[0]);
          else if(mode == 'i')
            strcat(text,modes[1]);
          strcat(text,"    ");
          strcat(text,filename);
      }else{
        strcpy(text,words);
      }
    logLine("\nwrote:");logLine(words);
      movecurs(height+yOffset+1,0);
      drawStatusBar(text,widith,colors,isError);
      movecurs(cRow,cCol);
      showcursor();

}


#include "ansihelpers.h"
#include "mytui.h"
#include <termios.h>


void create_window_inoutRANGE(int startx, int starty, int win_height,int max_width, char ** args,int yIn, int xIn,size_t linecount){ 

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

    short border_color = 230; 
    short text_color = 230;
    short bg_file_color = 236;
    short bg_unused_color = 235; 
    short comment_color = 242;
    
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
                    printf("%c",args[i][j]);
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
}

void printRightAlign(int num, short width){




}



void set_input_mode(struct termios *old_termios) {
    struct termios new_termios;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, old_termios);
    new_termios = *old_termios;

    // Disable buffered input (canonical mode) and echo
    new_termios.c_lflag &= ~(ICANON | ECHO );
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;

    // Set the terminal mode immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void restore_input_mode(struct termios *old_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_termios);
}

double get_time_ms(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (tv.tv_sec * 1000.0)+(tv.tv_usec / 1000.0);

}


// Function to wait for input with a timeout
int timed_input(double timeout_sec,double * elapsed_time_count) {
    fd_set set;
    struct timeval timeout;
    int result;

    // Clear the set and add STDIN (file descriptor 0)
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    // Set timeout value
    timeout.tv_sec = (int)timeout_sec;
    timeout.tv_usec = (timeout_sec - (int)timeout_sec) * 1e6;

    // Wait for input on STDIN
    double starttime = get_time_ms();
    result = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
    double endtime = get_time_ms();
    *elapsed_time_count = endtime-starttime;


    if (result > 0) {
        // Input is ready
        return getchar(); // Read the character
    } else if (result == 0) {
        // Timeout
        return -1; // Return -1 to indicate timeout
    } else {
        // Error
        perror("select");
        return -2;
    }
}


int get_cursor_pos(int * row, int * col){
    char buf[32];
    int i = 0;

    // Send the ANSI escape sequence to query cursor position
    printf("\033[6n");

    // Read the response: ESC [ rows ; cols R
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, buf + i, 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';


    if (buf[0] == '\033' && buf[1] == '[') {
        sscanf(buf + 2, "%d;%d", row, col);
        return 1;
    } else {
        return 0;
    }

}

size_t countLines(FILE * f){
    char ch;
    size_t linecount = 0;
    int nonemptyline = 0;
    while((ch = fgetc(f))!=EOF){
        if ( ch == '\n' ){
            linecount++;
            if(nonemptyline){
                nonemptyline = 0;
            }
        }else if(ch != ' ' &&  ch != '\t' && nonemptyline == 0){
            nonemptyline = 1;
        }
    }
    if(nonemptyline){
        linecount++;
    }


    rewind(f);
    return linecount;
}



size_t countLongestLine(FILE * f){
 if (f == NULL) {
        fprintf(stderr, "Invalid f pointer\n");
        return 0;
    }

    size_t max_length = 0; // Variable to store the length of the longest line
    size_t current_length = 0; // Variable to track the current line's length
    int c;

    // Loop through the f character by character
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            // If we encounter a newline, check the current line length
            if (current_length > max_length) {
                max_length = current_length;
            }
            current_length = 0; // Reset the current length for the next line
        } else {
            current_length++;
        }
    }

    // Handle the case where the last line doesn't end with a newline
    if (current_length > max_length) {
        max_length = current_length;
    }

    // Reset f pointer to the beginning for reuse
    rewind(f);

    return max_length;


}


void copy_2d_arr(int HEIGHT, int WIDTH, int orig_height,char dest[HEIGHT][WIDTH], char source[orig_height][WIDTH], short start_range, short end_range){

    for(int i = 0 ; i < orig_height && i < end_range; i++){
        strcpy(dest[i],source[i+start_range]);  
    }

}

void flush_stdin(){
    char buf;
    double dummy;
    do{
        buf = timed_input(0.0001,&dummy);
    }while(buf!= -1);
}

int snap_left(char** buffer, int * cursRow, int * cursCol, int *yIn, int *xIn, short yOffset,short xOffset){

    size_t line_len = strlen(buffer[(*cursRow)+(*yIn)-yOffset]);

    int status = 1;
    if(line_len < (*cursCol)+(*xIn)-xOffset){
        if(*xIn <= line_len){
            movecurs(*cursRow,(int)line_len-(*xIn)+xOffset); 
        }else{
            status = 0;
        }
    }

    get_cursor_pos(cursRow,cursCol);

    //logLine("\nMOVED TO:");
    return status;
}

void snapCursorRight(char ** f_buf, int * cRow, int * cCol, int * yStart, int* xStart, int yOffset, int xOffset, int rend_HEIGHT, int rend_WIDTH, size_t linecount){


    char * line = f_buf[*cRow+(*yStart)-yOffset];
    size_t len = strlen(line);
    logLine("\nlen = ");logNum(len);
    logLine("\nrend_WIDTH = ");logNum(rend_WIDTH);
    int moveFlag = 0;
    if(len < rend_WIDTH){
        *xStart = 0;
        size_t len = strlen(line);
    }else{
        *xStart = len-rend_WIDTH+1;
        moveFlag = 1;
   }
    create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,*yStart,*xStart,linecount);
    if(moveFlag){ movecurs(*cRow,rend_WIDTH+xOffset-1);}
    else {movecurs(*cRow,(int)(len+xOffset));}

}



void snapCursorLeft(char ** f_buf, int * cRow, int  * cCol, int * yStart, int * xStart, int yOffset, int xOffset,int rend_HEIGHT,int WIDTH,size_t linecount){
        if(!snap_left(f_buf, cRow,cCol,yStart,xStart,yOffset,xOffset)){
            get_cursor_pos(cRow,cCol);
            size_t len = strlen(f_buf[(*cRow)+(*yStart)-yOffset]);
            if(len != 0){
                *xStart = len-1;
                *cCol = xOffset+1;
            }else{
                *xStart = len;
                *cCol = xOffset;
            }
            create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,*yStart,*xStart,linecount);
            movecurs(*cRow,*cCol);
        }
}

char *insert_line(char ***buf, char *line, int buf_row, size_t *buf_height) {

    char **new_buf = realloc(*buf, (*buf_height + 1) * sizeof(char *));
    *buf = new_buf;

    // Shift lines down from buf_row to make room for the new line
    for (int i = *buf_height; i > buf_row; i--) {
        (*buf)[i] = (*buf)[i - 1];
    }

    // Allocate memory for the new line and copy it
    (*buf)[buf_row] = malloc(strlen(line) + 1);
    if ((*buf)[buf_row] == NULL) {
        return "Memory allocation failed";
    }
    strcpy((*buf)[buf_row], line);

    // Update the buffer height
    (*buf_height)++;

    return NULL;  // Return NULL to indicate success
}


char *remove_line(char ***buf, int buf_row, size_t *buf_height) {

    char *removed_line = (*buf)[buf_row];

    // Shift all lines after buf_row up by one position
    for (size_t i = buf_row; i < *buf_height - 1; i++) {
        (*buf)[i] = (*buf)[i + 1];
    }

    (*buf_height)--;

    // Reallocate memory for the buffer to fit the new height
    char **new_buf = realloc(*buf, (*buf_height) * sizeof(char *));

    // Update the buffer pointer
    *buf = new_buf;

    // Return the removed line
    return removed_line;
}


char * insert_to_line(char ** buf, char * line, int buffer_row, int index_in_line,char ch){

    //logLine("in insert_to_line");
    //logLine(line);
    size_t len = strlen(line);
    //logLine("got length");
    line = (char *)realloc(line,(len+2)*sizeof(char));
    if(line == NULL){
        logLine("LINE WAS NULL");
    }

    //logLine("realloc was fine");

    // Shift elements to the right to make space for the new element
    for (size_t i = len+1; i > index_in_line; i--) {
        line[i] = line[i - 1];
    }

    //logLine("shifted correctly");

    // Insert the new element at the specified index
    line[index_in_line] = ch;
    buf[buffer_row] = line;

    // Update the length of the array
    return buf[buffer_row];
}



char * remove_from_line(char ** buf, char * line, int buf_row, int index){

    size_t len = strlen(line);

    for(size_t i = index ; i < len; i++)
        line[i] = line[i+1];

    line = (char *)realloc(line,len*sizeof(char));
    buf[buf_row] = line;

    return line;
}


int smart_moveup(int cRow,int *yStart,short yOffset){
    if(cRow > yOffset){
        moveup(); 
    }else if(*yStart!= 0){
        (*yStart)--;
        return 1;;

    }
    return 0;
}

int smart_movedown(int cRow, int *yStart,int flag, int linecount, int rend_HEIGHT){
    logLine("-in smart_movedown-");
    logLine("\ncRow = ");logNum(cRow);
    logLine("\n*yStart = ");logNum(*yStart);
    logLine("\nlinecount = ");logNum(linecount);
    logLine("\nrend_HEIGHT = ");logNum(rend_HEIGHT);
    if(*yStart+cRow <= linecount){
        if(cRow >= rend_HEIGHT && (flag || *yStart+cRow != linecount)){ 
            logLine("scrolling down!!!!");
            (*yStart)++;   
            return 1;
        }else{
            logLine("moving down!   ");
            movedown();
        }
    }
    return 0;

}
int smart_moveleft(int cCol,int * xStart,short xOffset){

    if(cCol > xOffset){
        moveleft(); 
        return 0;
    }else if(*xStart!= 0){
        (*xStart)--;
        return 1;
    }

    return 2;

}

int smart_moveright2(int cCol, int *xStart, int xOffset,size_t length,int WIDTH){


    logLine("\n in SMART_MOVERIGHT2----------\n");
    logLine("\ncCol = ");logNum(cCol);
    logLine("\nxStart = ");logNum(*xStart);
    logLine("\nxOffset = ");logNum(xOffset);
    logLine("\nlength = ");logNum(length);
    logLine("\nwidth = ");logNum(WIDTH);
    if(cCol-xOffset == WIDTH-1 && (*xStart)+cCol-xOffset < length){
        logLine("increating xStart");
        (*xStart)++;
        return 1;
    }

    if((*xStart+cCol-xOffset) < length){
        moveright();
        return 0;
    }
    logLine("returning 2");
    return 2;
    logLine("DONE");
}

size_t countLongestLineBuffer(char ** buffer, size_t length){
    size_t longest = 0;
    for(size_t i = 0 ; i < length ; i++){
        size_t curr = strlen(buffer[i]);
        if(curr > longest)
            longest = curr;
    }
    return longest;
}


void logLine(char * line){

    FILE * log = fopen("main.log","a");
    fprintf(log,"%s",line);
    fclose(log);
}
void logChar(char ch){

    FILE * log = fopen("main.log","a");
    if(ch != '\0')
        fprintf(log,"\\%c\\ ",ch);
    else
        fprintf(log,"\\\\");
    fclose(log);
}

void logNum(double num){

    FILE * log = fopen("main.log","a");
    fprintf(log,"%lf",num);
    fclose(log);
}

void clearLog(){ FILE * log = fopen("main.log","w"); fclose(log);}


void print_with_offset(const char *line, int xOffset) {
    for (int i = 0; i < xOffset; i++) {
      moveright();
    }
    printf("%s\n", line);
}


void drawLogo(int HEIGHT, int WIDTH){

    
    short text_color = 230;
    setTextColor(text_color);
    // Define the ASCII art lines
    const char *art[] = {
        "                       Phi 0.0.1",
        "                Open Source Text Editor",
        "                                  ,",
        "                 ,g@@@@MT    g@@@@@@@@@@g",
        "               g@@@@M       @@@@NMM%@@@@@@@,",
        "             g@@@@@        @@@@`     \"%@@@@@@",
        "            @@@@@@         @@@@        ^@@@@@@",
        "           @@@@@@E         @@@@         \"@@@@@@",
        "          j@@@@@@          @@@@          ]@@@@@E",
        "          ]@@@@@@          @@@@           @@@@@[",
        "          ]@@@@@@          @@@@           @@@@@@",
        "           @@@@@@W         @@@@           @@@@@\"",
        "           $@@@@@@         @@@@          ]@@@@@",
        "            $@@@@@W        @@@@         ,@@@@@",
        "             ?@@@@@@       @@@@        a@@@@C",
        "               T@@@@@p     @@@@      g@@@@M",
        "                 |Y0@@@@mg,@@@@,ag@@@@@M\"",
        "                     .TTMM%@@@@MMMT!",
        "                           @@@@",
        "                           @@@@",
        "                           @@@@",
        "                           @@@@",
        "                           \\@@/",
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

}



char updateMode(char inputch, char *mode){

  if(inputch == 'i' && *mode != 'i'){
    *mode = 'i';
    return 1;
  }else return 0; 



}

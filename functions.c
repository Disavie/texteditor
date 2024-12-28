
#include "ansihelpers.h"
#include "mytui.h"
#include <termios.h>

void create_window(int startx, int starty, int HEIGHT,int WIDTH, char args[HEIGHT][WIDTH]){
    const char DEF = ' ';
    setlocale(LC_CTYPE,"");
    //BORDERS
    wchar_t BLOCK= 0x2588;
    wchar_t LEFTBORD = 0x2595;
    wchar_t RIGHTBORD = 0x258F;
    wchar_t BOTTOMBORD = 0x2581;
    wchar_t TOPBORD  = 0x2594;
    
    printf("\033[2J\033[H");

    //setting the starty;
    int y = 0;
    while(y < starty){
        printf("%d\n",y);
        y++;
    }
    int x = 0;
    char letter;
    for(int i = -1 ; i < HEIGHT +1 ; i ++){
        while(x < startx){
            printf(" ");
            x++;
        }
        printf("%lc",LEFTBORD);
        for(int j = 0; j < WIDTH ; j++){
            if( i == -1){
                printf("%lc",TOPBORD);
            }else if ( i == HEIGHT ){
                printf("%lc",BOTTOMBORD);
            }else{
                letter = args[i][j];
                if (letter == '\0'){
                    printf("\\");
                }else{
                    printf("%c",letter);
                }
            }
        }
        x = 0;
        printf("%lc\n",RIGHTBORD);
    }
}

void draw_borders(int startx, int starty, int win_height, int win_width){

    setlocale(LC_CTYPE,"");
    //BORDERS
    const char DEF = ' ';
    wchar_t BLOCK= 0x2588;
    wchar_t LEFTBORD = 0x2595;
    wchar_t RIGHTBORD = 0x258F;
    wchar_t BOTTOMBORD = 0x2581;
    wchar_t TOPBORD  = 0x2594;
    
    clear_screen();
    move00();
    hidecursor();
    //setting the starty;
    int y = 0;
    while(y < starty){
        printf("%d\n",y);
        y++;
    }
    int x = 0;
    char letter;
    for(int i = -1 ; i < win_height+1 ; i++){
        while(x < startx){
            printf(" ");
            x++;
        }
        printf("%lc",LEFTBORD);

        for(int j = 0; j < win_width; j++){
            if( i == -1){
                printf("%lc",TOPBORD);
            }else if ( i == win_height){
                printf("%lc",BOTTOMBORD);
            }else{
                printf("\033[2;39m%c\033[0m",DEF);
            }
        }
        x = 0;
        printf("%lc\n",RIGHTBORD);
    }
    showcursor();
}
    

void noflicker_create_window_inoutRANGE(int startx, int starty, int win_height, int max_width, char **args, int yIn, int xIn) {
    setlocale(LC_CTYPE, "");
    // BORDERS
    const char DEF = ' ';
    wchar_t BLOCK = 0x2588;
    wchar_t LEFTBORD = 0x2595;
    wchar_t RIGHTBORD = 0x258F;
    wchar_t BOTTOMBORD = 0x2581;
    wchar_t TOPBORD = 0x2594;

    // Estimate buffer size
    size_t buffer_size = 1024 * 1024; // 1 MB buffer, adjust as needed
    char *output_buffer = malloc(buffer_size);
    if (!output_buffer) {
        perror("Failed to allocate buffer");
        exit(1);
    }

    // Pointer to track the current position in the buffer
    char *buffer_ptr = output_buffer;

    // Clear screen, move cursor to 0,0 and hide cursor
    snprintf(buffer_ptr, buffer_size, "\033[H\033[?25l");
    buffer_ptr += strlen(buffer_ptr);

    int y = 0;
    // Setting the starty
    while (y < starty) {
        buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "%d\n", y);
        y++;
    }

    int x = 0;
    for (int i = yIn - 1; i < yIn + win_height + 1; i++) {
        while (x < startx) {
            buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), " ");
            x++;
        }

        buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "%lc", LEFTBORD);
        size_t len = 0;

        if (i >= 0 && i < yIn + win_height) {
            len = strlen(args[i]);
        }

        for (int j = xIn; j < xIn + max_width; j++) {
            if (i == yIn - 1) {
                buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "%lc", TOPBORD);
            } else if (i == yIn + win_height) {
                buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "%lc", BOTTOMBORD);
            } else {
                if (j < len) {
                    buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "%c", args[i][j]);
                } else {
                    buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "\033[2;39m%c\033[0m", DEF);
                }
            }
        }

        x = 0;
        buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "%lc\n", RIGHTBORD);
    }

    // Show cursor again
    buffer_ptr += snprintf(buffer_ptr, buffer_size - (buffer_ptr - output_buffer), "\033[?25h");

    // Write the entire buffer to stdout at once
    fwrite(output_buffer, 1, buffer_ptr - output_buffer, stdout);

    // Free the buffer
    free(output_buffer);
}


void create_window_inoutRANGE(int startx, int starty, int win_height,int max_width, char ** args,int yIn, int xIn){ 

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
    
    move00();
    hidecursor();

    setTextColor(147);
    setBgColor(236);


    int y = 0;
    //setting the starty;
    while(y < starty){
        printf("%d\n",y);
        y++;
    }
    int x = 0;
    char letter;
    for(int i = yIn-1 ; i < yIn+win_height+1 ; i++){
        while(x < startx){
            printf(" ");
            x++;
        }
        if(i == yIn -1)
            printf("%lc",TOPLEFTCORNER);
        else if(i == yIn+win_height)
            printf("%lc",BOTTOMLEFTCORNER);
        else
            printf("%lc",VERTICAL_BORDER);

        size_t len;
        if(i >= 0 && i < yIn+win_height)
             len = strlen(args[i]);

        if(len){
            setBgColor(236);
        }else{
            setBgColor(235);
        }


        for(int j = xIn; j < xIn+max_width ; j++){
            if( i == yIn-1){
                printf("%lc",HORIZONTAL_BORDER);
            }else if ( i == yIn+win_height){
                printf("%lc",HORIZONTAL_BORDER);
            }else{
                if(j < len){
                    printf("%c",args[i][j]);
                }else{
                    printf("%c",DEF);
                }
            }
        }
        x = 0;
        if(i == yIn -1)
            printf("%lc",TOPRIGHTCORNER );
        else if(i == yIn+win_height)
            printf("%lc",BOTTOMRIGHTCORNER);
        else
            printf("%lc\n",VERTICAL_BORDER);
    }
    showcursor();
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

int snap_left(char** buffer, int * cursRow, int * cursCol, int *yIn, int *xIn, int yOffset, int xOffset){

    logLine("\nIN SNAP LEFT ------------");
    //current line is buffer[cursRow]
    logLine("\nxIn = "); logNum(*xIn);
    logLine("\nyIn = "); logNum(*yIn);
    logLine("\ncCol = "); logNum(*cursCol);
    logLine("\ncRow = "); logNum(*cursRow);
    size_t line_len = strlen(buffer[(*cursRow)+(*yIn)-yOffset]);
    logLine("\nline_len = ");logNum(line_len);

    int status = 1;
    if(line_len < (*cursCol)+(*xIn)-xOffset){
        if(*xIn <= line_len){
            movecurs(*cursRow,(int)line_len-(*xIn)+xOffset); 
        }else{
            status = 0;
        }
    }

    logLine("\nstatus = "); logNum(status);
    get_cursor_pos(cursRow,cursCol);

    //logLine("\nMOVED TO:");
    return status;
}

char *insert_line(char ***buf, char *line, int buf_row, size_t *buf_height) {
    // Validate input parameters
    if (buf == NULL || *buf == NULL || line == NULL || buf_row < 0 || buf_row > *buf_height) {
        return "Invalid arguments";
    }

    // Reallocate the buffer to accommodate one more line
    char **new_buf = realloc(*buf, (*buf_height + 1) * sizeof(char *));
    if (new_buf == NULL) {
        return "Memory allocation failed";
    }
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


int smart_moveup(int cRow,int *yStart){
    if(cRow > 2){
        moveup(); 
    }else if(*yStart!= 0){
        (*yStart)--;
        return 1;;

    }
    return 0;
}

int smart_movedown(int cRow, int *yStart, int linecount, int rend_HEIGHT){

    if(*yStart+cRow <= linecount){
        if(cRow == rend_HEIGHT && *yStart+cRow != linecount){ 
            (*yStart)++;   
            return 1;
        }else{
            movedown();
        }
    }
    return 0;

}

int smart_moveleft(int cCol,int * xStart){

    if(cCol > 2){
        moveleft(); 
    }else if(*xStart!= 0){
        (*xStart)--;
        return 1;
    }
    return 0;

}

int smart_moveright(int cCol, int *xStart, int longestLineLength, int WIDTH){

    if((*xStart)+cCol <= longestLineLength+1){
        if(cCol == WIDTH && cCol+(*xStart) != longestLineLength+1){
            (*xStart)++;
            return 1;
        }
        else{
            moveright();
        }

    }
    return 0;

}

int smart_moveright2(int cCol, int *xStart, int xOffset,size_t length,int WIDTH){


    logLine("\n in SMART_MOVERIGHT2----------\n");
    logLine("linelength = ");logNum(length);
    logLine("\ncCol = ");logNum(cCol);
    logLine("\nxStart = ");logNum(*xStart);
    logLine("\nxOffset = ");logNum(xOffset);
    logLine("\nlength = ");logNum(length);


    if(cCol == WIDTH+1 && (*xStart)+cCol-xOffset < length){
        (*xStart)++;
        return 1;
    }

    if((*xStart+cCol-xOffset) < length)
        moveright();

    return 0;



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
    fprintf(log,"%s\n",line);
    fclose(log);
}
void logChar(char ch){

    FILE * log = fopen("main.log","a");
    fprintf(log,"\\%c\\ ",ch);
    fclose(log);
}

void logNum(double num){

    FILE * log = fopen("main.log","a");
    fprintf(log,"%lf",num);
    fclose(log);
}

void clearLog(){ FILE * log = fopen("main.log","w"); fclose(log);}


#include "ansihelpers.h"
#include "mytui.h"

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

void snapCursorRight(char ** f_buf, int * cRow, int * cCol, int * yStart, int* xStart, int yOffset, int xOffset, int rend_HEIGHT, int rend_WIDTH, size_t linecount,short colors[]){


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
    create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,*yStart,*xStart,linecount,colors);
    if(moveFlag){ movecurs(*cRow,rend_WIDTH+xOffset-1);}
    else {movecurs(*cRow,(int)(len+xOffset));}

}



void snapCursorLeft(char ** f_buf, int * cRow, int  * cCol, int * yStart, int * xStart, int yOffset, int xOffset,int rend_HEIGHT,int WIDTH,size_t linecount,short colors[]){
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
            create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,*yStart,*xStart,linecount,colors);
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

int smart_movedown(int cRow, int *yStart,int flag, size_t linecount, int rend_HEIGHT){

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

    if(cCol-xOffset == WIDTH-1 && (*xStart)+cCol-xOffset < length){
        logLine("increating xStart");
        (*xStart)++;
        return 1;
    }

    if((*xStart+cCol-xOffset) < length-1){
        moveright();
        return 0;
    }
    return 2;
}

int smart_moveright2_insert(int cCol, int *xStart, int xOffset,size_t length,int WIDTH){

    if(cCol-xOffset == WIDTH-1 && (*xStart)+cCol-xOffset < length){
        logLine("increating xStart");
        (*xStart)++;
        return 1;
    }

    if((*xStart+cCol-xOffset) <= length-1){
        moveright();
        return 0;
    }
    return 2;
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

char updateMode(char inputch, char *mode){

  if(inputch == 'i' && *mode != 'i'){
    *mode = 'i';
    return 1;
  }else return 0; 
}

int saveFile(char * filename,size_t linecount, char *** f_buf){
    
    FILE * f;

    f = fopen(filename,"w");
    if (f == NULL){
      return 1;
    }else{
        for(int i = 0 ; i < linecount; i++){
            fprintf(f,"%s\n",(*f_buf)[i]);
        }
        fclose(f);
    }
    return 0;


}


void trim_trailing_spaces(char *str) {
    int len = strlen(str);
    while (len > 0 && isspace(str[len - 1])) {
        str[len - 1] = '\0';  // Null-terminate to remove trailing spaces
        len--;
    }
}


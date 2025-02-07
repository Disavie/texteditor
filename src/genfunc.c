#include "mytui.h"
#include "stdint.h"

void handle_signal(int d){
    ;
}

void set_input_mode(struct termios *old_termios) {
    struct termios new_termios;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, old_termios);
    new_termios = *old_termios;

    // Disable buffered input (canonical mode) and echo
    new_termios.c_lflag &= ~(ICANON | ECHO );
    new_termios.c_iflag &= ~IXON;
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

void get24Htime(char * put,size_t size){
    double time_in_ms = get_time_ms();
    time_t time_in_sec = (time_t)(time_in_ms / 1000);  // Convert milliseconds to seconds
    struct tm *local_time = localtime(&time_in_sec);  // Convert seconds to local time

    strftime(put, size, "%H:%M:%S", local_time);
}

int get_terminal_size(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        return 1; // Failure
    }
    *rows = ws.ws_row;
    *cols = ws.ws_col;
    return 0; // Success
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

char updateMode(char inputch, char * mode, char ** modestr){

    if(inputch == 'i' && *mode != 'i'){
        *mode = 'i';
        strcpy(*modestr,"--INSERT--");
        return 1;
    }else {
        return 0; 
    }
}

void strcpyf(char *destination, size_t dest_size, const char *format, ...) {
//    for(size_t i = 0 ; i < dest_size ; i++) {
//        destination[i] = '\0';
//    }
    va_list args;

    // Start processing variable arguments
    va_start(args, format);

    // Write formatted string into the destination buffer safely
    vsnprintf(destination, dest_size, format, args);

    // End processing variable arguments
    va_end(args);
}

void trim_trailing_spaces(char *str) {
    int len = strlen(str);
    while (len > 0 && isspace(str[len - 1])) {
        str[len - 1] = '\0';  // Null-terminate to remove trailing spaces
        len--;
    }
}

char* size_t_to_string(size_t num) {

    size_t buffer_size = 64; 
    char *str = (char*)malloc(buffer_size * sizeof(char));
    if (str != NULL) {
        snprintf(str, buffer_size, "%zu", num); // Converts size_t to string
    }
    
    return str;
}

char * substr(char * line, size_t start, size_t end){

    if(end > strlen(line))
        end = strlen(line);

    if(start > end)
        return NULL;

    size_t len = end-start;
    char * out = (char *)malloc((len+1)*sizeof(char));
    strncpy(out,line+start,len);
    out[len] = '\0';
    return out;

}


char * substr_2(char * start, size_t len){
    char * out = (char *)malloc((len+1)*sizeof(char));
    strncpy(out,start,len);
    out[len] = '\0';
    return out;

}

size_t calcbytes(char * filename){
    FILE * f = fopen(filename,"r");
    fseek(f,0L,SEEK_END);
    size_t sz = ftell(f);
    return sz;
}

size_t calcsizeKB(Buffer * buf){

    size_t bytes = 0;
    for(int i = 0 ; i < buf->linecount ; i++){
        bytes+=strlen(buf->contents[i]);
        bytes++;
    }
    return bytes/1024;
}


size_t calcsize(Buffer * buf){

    size_t bytes = 0;
    for(int i = 0 ; i < buf->linecount ; i++){
        bytes+=strlen(buf->contents[i]);
        bytes++; //extra ++ to account for \n in a real file
    }
    return bytes;
}

int getnum(char *line) {
    if (!line) {
        return 1; // Return -1 if the input line is NULL
    }

    if (*line == '\0') {
        return 1; // Return 0 if the line contains only '\0'
    }

    int num = 0;
    int found_number = 0;

    while (*line) {
        if (isdigit(*line)) {
            found_number = 1;
            num = num * 10 + (*line - '0');
        } else if (found_number) {
            break; // Stop parsing after the first continuous number
        }
        line++;
    }

    return found_number ? num : 1; // Return 1 if no number was found
}


#include "ansihelpers.h"
#include "mytui.h"

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
    wchar_t LEFTBORD = 0x2595;
    wchar_t RIGHTBORD = 0x258F;
    wchar_t BOTTOMBORD = 0x2581;
    wchar_t TOPBORD  = 0x2594;

    move00();
    hidecursor();

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
        printf("%lc",LEFTBORD);
        size_t len;
        if(i >= 0 && i < yIn+win_height)
             len = strlen(args[i]);


        for(int j = xIn; j < xIn+max_width ; j++){
            if( i == yIn-1){
                printf("%lc",TOPBORD);
            }else if ( i == yIn+win_height){
                printf("%lc",BOTTOMBORD);
            }else{
                if(j < len){
                    printf("%c",args[i][j]);
                }else{
                    printf("\033[2;39m%c\033[0m",DEF);
                }
            }
        }
        x = 0;
        printf("%lc\n",RIGHTBORD);
    }
    showcursor();
}





void set_input_mode(struct termios *old_termios) {
    struct termios new_termios;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, old_termios);
    new_termios = *old_termios;

    // Disable buffered input (canonical mode) and echo
    new_termios.c_lflag &= ~(ICANON | ECHO);

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


void get_cursor_pos(int * row, int * col){

    printf("\033[6n");
    char ch;
    int num;
    scanf("%c",&ch);

    scanf("%c",&ch); //gets rid of the [
    scanf("%d",&num); 
    *row = num; 

    scanf("%c",&ch); //gets rid of the ; 
    scanf("%d",&num); 
    *col = num;
    scanf("%c",&ch); //gets rid of the 'R' left in buffer

    fflush(stdin);

}

int countlines(FILE * f){
    char ch;
    short linecount = 0;
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
    return linecount;
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

void snap_left(char** buffer, int * cursRow, int * cursCol, int yIn, int xIn, int yOffset, int xOffset){

    //current line is buffer[cursRow]
    size_t row_len = strlen(buffer[(*cursRow)+yIn-yOffset]);
    if(row_len < (*cursCol)-xOffset){
       movecurs(*cursRow,row_len+xOffset); 
    }
    get_cursor_pos(cursRow,cursCol);
    flush_stdin();

}

void movecurs(int row, int col){ printf("\033[%d;%dH",row,col);}




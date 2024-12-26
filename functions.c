
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


void create_window_inoutRANGE(int startx, int starty, int HEIGHT,int WIDTH, char ** args,int yIn, int xIn){ 

    const char DEF = '.';
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
    for(int i = yIn-1 ; i < yIn+HEIGHT+1 ; i++){
        while(x < startx){
            printf(" ");
            x++;
        }
        printf("%lc",LEFTBORD);
        for(int j = xIn; j < xIn+WIDTH ; j++){
            if( i == yIn-1){
                printf("%lc",TOPBORD);
            }else if ( i == yIn+HEIGHT){
                printf("%lc",BOTTOMBORD);
            }else{
                letter = args[i][j];
                if (letter == '\0'){
                    printf("%c",DEF);
                }else{
                    printf("%c",letter);
                }
            }
        }
        x = 0;
        printf("%lc\n",RIGHTBORD);
    }
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

void snap_left(char** buffer, int * cursRow, int * cursCol, int row_offset, int col_offset){

    //current line is buffer[cursRow]
    int shifted = 0;
    int hitboreder = 0;
    while(buffer[(*cursRow)+row_offset][(*cursCol)+col_offset] == '\0'){
        if((*cursCol)+col_offset == 0){
            hitboreder = 1; 
            break;
        }
        shifted = 1;
        moveleft();
        get_cursor_pos(cursRow,cursCol);
        flush_stdin();
    }
    if(shifted && !hitboreder)
        moveright();
    get_cursor_pos(cursRow,cursCol);
    flush_stdin();

}

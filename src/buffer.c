#include "mytui.h"
#include "Buffer.h"
#include "cursor.h"


void loadFile(Buffer * dest, char * filename){
    FILE * f = fopen(filename,"r");
    if ( f == NULL ){
        createFile(dest ,filename);
    }else{
        strcpy(dest->filename,filename);
        dest->filename = filename;
        dest->linecount = countLines(f);
        dest->ypos = 0;
        dest->xpos = 0;
        dest->contents = (char **)malloc((dest->linecount)*sizeof(char *));
        
        int i = 0;
        while(!feof(f) && i < (*dest).linecount) {
    
            char line[1024];
            fgets(line,sizeof(line),f);
            size_t len = strlen(line);
            dest->contents[i] = (char *)malloc((len+1)*sizeof(char));
            strcpy(dest->contents[i],line);
            if(dest->contents[i][len-1] == '\n')
                dest->contents[i][len-1] = '\0';
            i++;
        }
        fclose(f);
    }
}


void createFile(Buffer * dest,char * filename){

    dest->linecount = 1;
    dest->ypos = 0;
    dest->xpos = 0;
    dest->contents = (char **)malloc(1*sizeof(char *));
    dest->contents[0] = (char *)malloc(sizeof(char));
    dest->contents[0][0] = '\0';

    
}

int saveFile(Buffer * file){

    FILE * f;

    f = fopen(file->filename,"w");
    if (f == NULL){
        return 1;
    }else{
        for(int i = 0 ; i < file->linecount; i++){
            fprintf(f,"%s\n",file->contents[i]);
        }
        fclose(f);
    }
    return 0;


}

char *insert_line(Buffer * buf, char * line,size_t index) {

    char ** newcontents =  (char **)realloc(buf->contents, (buf->linecount+ 1) * sizeof(char *));
    buf->contents = newcontents;

    // Shift lines down from buf_row to make room for the new line
    for (int i = buf->linecount; i > index; i--) {
        buf->contents[i] = buf->contents[i-1];
    }

    // Allocate memory for the new line and copy it
    buf->contents[index] = malloc(strlen(line)+1);
    strcpy(buf->contents[index], line);

    // Update the buffer height
    buf->linecount++;

    return NULL;  // Return NULL to indicate success
}

char *remove_line(Buffer * buf, int buf_row) {

    char *removed_line = buf->contents[buf_row];

    // Shift all lines after buf_row up by one position
    for (size_t i = buf_row; i < buf->linecount- 1; i++) {
        buf->contents[i] = buf->contents[i+1];
    }

    buf->linecount--;

    // Reallocate memory for the buffer to fit the new height
    char ** newcontents = realloc(buf->contents, (buf->linecount) * sizeof(char *));

    // Update the buffer pointer
    buf->contents = newcontents;

    // Return the removed line
    return removed_line;
}

char * insert_to_line(Buffer * buf, size_t row, size_t index_in_line,char ch){

    char * line = buf->contents[row];
    size_t len = strlen(line);

    line = (char *)realloc(line,(len+2)*sizeof(char));


    for (size_t i = len+1; i > index_in_line; i--) {
        line[i] = line[i - 1];
    }

    line[index_in_line] = ch;
    buf->contents[row] = line;

    return line;
}

char * remove_from_line(Buffer * buf, size_t row, size_t index) {
    char * line = buf->contents[row];
    size_t len = strlen(line);
    

    for(size_t i = index ; i < len; i++)
        line[i] = line[i+1];

    line = (char *)realloc(line,len*sizeof(char));
    buf->contents[row] = line;

    return line;
}
void drawbuffer(short starty, short startx, int win_height, int win_width, Buffer *buffer, short colors[]) {
    win_width -= 6; // Done so that you can see line numbers

    setlocale(LC_CTYPE, "");
    const char DEF = ' ';
    wchar_t BLOCK = 0x2588;
    wchar_t VERTICAL_BORDER = 0x2502;
    wchar_t HORIZONTAL_BORDER = 0x2500;
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
    setTextColor(text_color);

    int end = 0;

    // Iterate over visible rows
    for (int row = (int)buffer->ypos; row < buffer->ypos + win_height; row++) {
        setTextColor(comment_color);

        // Check if the current row is beyond the actual content of the buffer
        if (row >= buffer->linecount) {
            setBgColor(bg_unused_color);
            printf("    ~ "); // Indicate unused lines
            end = 1;
        } else {
            setBgColor(bg_file_color);
            printf("%5d ", row + 1); // Print line number (1-based index)
        }

        setTextColor(text_color);

        // Iterate over visible columns for the current row
        for (int col = (int)buffer->xpos; col < buffer->xpos + win_width; col++) {
            if (end || row >= buffer->linecount || col >= strlen(buffer->contents[row])) {
                printf("%c", DEF);
            } else {
                char letter = buffer->contents[row][col];
                printf("%c", letter);
            }
        }
        if(row != buffer->ypos + win_height-1)
            printf("\n"); // Move to the next line
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


void update_statusbar(char * words,short ypos, short width, char ** modes, char mode, Buffer * buf,short colors[],short isError){

    hidecursor();
    size_t tmp1, tmp2;
    get_cursor_pos(&tmp1,&tmp2);

    char text[width];
    strcpy(text," ");

    if(words[0] == '\0'){
        if(mode == 'n')
            strcat(text,modes[0]);
        else if(mode == 'i')
            strcat(text,modes[1]);
        strcat(text,"    ");
        strcat(text,buf->filename);;
    }else{
        strcpy(text,words);
    }
    movecurs((size_t)ypos,(size_t)0);
    drawStatusBar(text,width,colors,isError);
    movecurs(tmp1,tmp2);
    showcursor();

}

#include <stdlib.h>

int main(int arc, char ** argv){

//    printf("Testing Unicode: ü ✓\n");
//    printf("what about this \'%c\'",160); 
    system("gcc src/colorscheme.c -c -o bin/colorscheme.o");
    system("gcc src/genfunc.c -c -o bin/genfunc.o");
    system("gcc src/drawlogo.c -c -o bin/drawlogo.o");
    system("gcc src/normal.c -c -o bin/normal.o");
    system("gcc src/buffer.c -c -o bin/buffer.o");
    system("gcc src/cursor.c -c -o bin/cursor.o");
    system("gcc src/lexer.c -c -o bin/lexer.o");
    system("gcc src/textedit.c -c -o bin/textedit.o");

    system("gcc bin/*.o -o zep");

}

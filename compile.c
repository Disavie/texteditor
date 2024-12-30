#include <stdlib.h>
#include <stdio.h>

int main(int arc, char ** argv){

  system("gcc src/functions.c -c -o bin/functions.o");
  system("gcc src/screenfunc.c -c -o bin/screenfunc.o");
  system("gcc src/normal.c -c -o bin/normal.o");
  system("gcc src/textedit.c -c -o bin/textedit.o");

  system("gcc bin/*.o -o zep");

}

#include "mytui.h"
#include "ansihelpers.h"



int mimic_vim_w(char *** f_buf, int * yStart, int * xStart,int * cRow, int *cCol, int yOffset, int xOffset, int rend_WIDTH){
    int updated = 0;
    if((*cCol)+(*xStart)-xOffset >= strlen((*f_buf)[(*yStart)+(*cRow)-yOffset])-1) return 0;
            while((*f_buf)[(*yStart)+(*cRow)-yOffset][(*xStart)+(*cCol)-xOffset] != ' '){
              if((*cCol)-xOffset == rend_WIDTH-1){
                (*xStart)++;
                updated = 1;
              }else{
                (*cCol)++;
              }
              if((*cCol)+(*xStart)-xOffset >= strlen((*f_buf)[(*yStart)+(*cRow)-yOffset])-1) break;
            }
            movecurs(*cRow,*cCol);
            smart_moveright2(*cCol, xStart, xOffset,strlen((*f_buf)[(*cRow)+(*yStart)-yOffset]),rend_WIDTH);

    return updated;
}

int mimic_vim_b(char *** f_buf, int * yStart, int * xStart, int * cRow, int * cCol, int yOffset, int xOffset,int rend_WIDTH){

            int update_made = 0;

            if((*xStart)+(*cCol)-xOffset == 0)return 0;

            if((*cCol) == xOffset) {(*xStart)--;}
            else {(*cCol)--;}
        
            if((*f_buf)[(*yStart)+(*cRow)-yOffset][(*xStart)+(*cCol)-xOffset] == ' '){

              if((*cCol) == xOffset) {(*xStart)--;}
              else {(*cCol)--;}
              
            }

            while((*f_buf)[(*yStart)+(*cRow)-yOffset][(*xStart)+(*cCol)-xOffset] != ' '){
              if((*cCol) == xOffset && (*xStart) != 0){
                (*xStart)--;
                update_made = 1;
              }else{
                (*cCol)--;
              }

              if((*cCol)+(*xStart)-xOffset == 0) {  break; };
            }
            movecurs((*cRow),(*cCol));
            if((*cCol)+(*xStart)-xOffset != 0)
              smart_moveright2((*cCol), &(*xStart), xOffset,strlen((*f_buf)[(*cRow)+(*yStart)-yOffset]),rend_WIDTH);
            return update_made;

}




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mytui.h"

typedef enum {
    NONE = 0, 
    DATA_TYPE = 1, //int float double char void size_t short long
    CONTROL_FLOW = 2, //if else for while do goto break continue switch case default
    FUNCTION = 3, //return
    MISC = 4, //const typedef struct
    NUMBER = 5,
    STRING = 6,
    COMMENT = 7,
    PREPROCESSOR = 8
}keyword;

const char * DATA_TYPE_KEYWORDS[] = {
    "int","float","double","char","void","size_t","short","long","auto"
};
const char * CONTROL_FLOW_KEYWORDS[] = {
    "if","else","for","while","do","goto","break","continue","switch","case"
};
const char * FUNCTION_KEYWORDS[] = {
    "return"
};
const char * MISC_KEYWORDS[] = {
    "const","typedef","struct"
};

char * build_word(char * line, size_t *index){
    size_t start = *index;
    size_t len = 0;

    while(isspace(line[start])){
        start++;
        (*index)++;
    }

    size_t current = start;
    if (line[current] == '"' || line[current] == '\'') {
        char quote_type = line[current];
        current++;
        (*index)++;
        len++;
        while (line[current] != quote_type && line[current] != '\0') {
            current++;
            len++;
            (*index)++;
        }
        if(line[current] != '\0'){
            current++;
            len++;
            (*index)++;
        }
    }else if (line[current] == '/' && line[current + 1] == '/') {
        current++;
        (*index)++;
        len++;
        while (line[current] != '\0') {
            current++;
            (*index)++;
            len++;
        }
    }else if (line[current] == '#') {
        current++;
        (*index)++;
        len++;
        while (line[current] != '\0') {
            current++;
            (*index)++;
            len++;
        }
    }else {
        while (line[current] != '\0' && (isalnum(line[current]) || line[current] == '_')) {
            current++;
            len++;
            (*index)++;
        }
    }

    char * word = (char *)malloc((len+1)*sizeof(char));
    strncpy(word,line+start,len);
    word[len] = '\0';
    return word;
}

int isnumber(char * word){

    if(strlen(word) == 0)
        return 0;

    for(size_t i = 0 ; i < strlen(word) ; i++) {
        if(!isdigit(word[i]))
            return 0;
    }

    return 1;
}

keyword iskeyword(char * word){

    keyword key;
    
    if(isnumber(word)){
        return NUMBER;
    }else if(word[0] == '\'' || word[0] == '"'){
        return STRING;
    }else if(word[0] == '/' && word[1] == '/'){
        return COMMENT;
    }else if(word[0] == '#'){
        return PREPROCESSOR;
}

    for(int i = 0 ; i < sizeof(DATA_TYPE_KEYWORDS)/sizeof(DATA_TYPE_KEYWORDS[0]);i++){
        if(strcmp(word,DATA_TYPE_KEYWORDS[i]) == 0 ){
            return DATA_TYPE;
        }
    }
    for(int i = 0 ; i < sizeof(CONTROL_FLOW_KEYWORDS)/sizeof(CONTROL_FLOW_KEYWORDS[0]);i++){
        if(strcmp(word,CONTROL_FLOW_KEYWORDS[i]) == 0 ){
            return CONTROL_FLOW;
        }
    }
    for(int i = 0 ; i < sizeof(FUNCTION_KEYWORDS)/sizeof(FUNCTION_KEYWORDS[0]);i++){
        if(strcmp(word,FUNCTION_KEYWORDS[i]) == 0 ){
            return FUNCTION;
        }
    }
    for(int i = 0 ; i < sizeof(MISC_KEYWORDS)/sizeof(MISC_KEYWORDS[0]);i++){
        if(strcmp(word,MISC_KEYWORDS[i]) == 0 ){
            return MISC;
        }
    }

    return NONE;
}



typedef struct{
    short keywordtype;
    size_t index;
    char * text;
    size_t true_length; //length INCLUDING \033...
    size_t length; //length EXCLUDING \033...
}Token;


void changetext(Token *token, short color,const short colors[]) {
    char col_str[32];
    snprintf(col_str, sizeof(col_str), "%d", color);

    size_t new_len = strlen(token->text) + 32 + strlen(col_str);
    char *newtext = (char *)malloc(new_len + 1); // +1 for null terminator
    newtext[new_len] = '\0';
    snprintf(newtext, new_len + 1, "\033[38;5;%dm%s\033[38;5;%dm", color, token->text,colors[0]);
    token->true_length = strlen(newtext);
    free(token->text);
    token->text = newtext;
}
void colortokens(Token * tokens, size_t tokencount,const short colors[]){

    for(size_t i = 0 ; i < tokencount ; i++) {
        switch (tokens[i].keywordtype){

            case(DATA_TYPE):
                changetext(tokens+i,colors[9],colors);
                break;
            case(CONTROL_FLOW):
                changetext(tokens+i,colors[10],colors);
                break;
            case(FUNCTION):
                changetext(tokens+i,colors[11],colors);
                break;
            case(MISC):
                changetext(tokens+i,colors[12],colors);
                break;
            case(NUMBER):
                changetext(tokens+i,colors[15],colors);
                break;
            case(COMMENT):
                changetext(tokens+i,colors[3],colors);
                break;
            case(PREPROCESSOR):
                changetext(tokens+i,colors[14],colors);
                break;
            case(STRING):
                changetext(tokens+i,colors[13],colors);
                break;
        }
    }
}

void remove_range(char **line, size_t index, size_t length) {
    memmove(*line + index, *line + index + length, strlen(*line) - index - length + 1);
}

void insert_word(char **line, char *insert, size_t index) {
    size_t len_line = strlen(*line);
    size_t len_insert = strlen(insert);

    char *newline = (char *)malloc(len_line + len_insert + 1);
    if(newline != NULL){
        strncpy(newline, *line, index);
        strcpy(newline + index, insert);
        strcpy(newline + index + len_insert, *line + index);
        *line = newline;
    }
}
void reinsert_tokens(Token * tokens, size_t tokencount,char ** line){

    int index_helper = 0; 
    for(int i = 0 ; i < tokencount ; i++){
        Token token = tokens[i];
        remove_range(line,token.index+index_helper,token.length);
        insert_word(line,token.text,token.index+index_helper);
        index_helper+=(token.true_length-token.length);
    }
}

char * highlightLine(char *line,const short colors[]){
    if(line == NULL || strlen(line) == 0) return NULL;

    Token * tokens = NULL; 
    size_t tokencount = 0;
    char * highlighted = (char *)malloc((strlen(line)+1)*sizeof(char));
    strcpy(highlighted,line);
    highlighted[strlen(line)] = '\0';


    size_t i  = 0;
    while(i < strlen(line)){

        char * word = build_word(highlighted,&i);

        keyword type;
        if((type = iskeyword(word))){
            Token token = {
                .keywordtype = type,
                .index = i-strlen(word),
                .length = strlen(word),
            };
            token.text = (char *)malloc((strlen(word)+1)*sizeof(char));
            strcpy(token.text,word);
            token.text[strlen(word)] = '\0';
            Token * new_tokens = (Token *)realloc(tokens,(tokencount+1) * sizeof(Token));
            tokens = new_tokens;
            tokens[tokencount] = token;
            tokencount++;

        }
        free(word);
        i++;
    }
    colortokens(tokens,tokencount,colors);
    reinsert_tokens(tokens,tokencount,&highlighted);

    for(int i = 0 ; i < tokencount ; i++) {
        free(tokens[i].text);
    }

    free(tokens);

    return highlighted;
}



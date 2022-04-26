/*Chanany shok*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "parser.h"
#include "assm_syntx.h"
#include "error_handler.h"

/* Small macro to check if still in a word after new line char or eof flag try to return
 *  so no last character will disappear*/
#define IN_WORD { \
      if (inWord) {\
word_location[num_of_words[0] * 2 + 1] = word_len;\
num_of_words[0]++;\
word_len = 0;\
inWord = FALSE;\
}           \
};


int check_space_inside_word(char *word){
    int i =0 ,j =0;
    
    /*trim lead character until hit space.*/
    for(i =0; i < (int)strlen(word); i++) {
        if (!(isspace(*(word + i)))) continue;
        else break;
    }
     
    j = i++;
    /* if hit another charachters after the space, we have to words inside one token*/
    while(i <(int)strlen(word) ){
   
        if(!(isspace(*(word + i)))){ 
       
           return FALSE;
        }
        i++;
    }
      
    return j;
    /*return the actual length without white spaces*/
}


char *get_operand_with_comma(char * operand){
	
    int oper_len =0, i;
    char *tmp_oper ="";
    char holder[MAX_LABEL + ONE_OPERND_COMM] = {0};
    /* trim leading withe spaces*/
    while(isspace(*(operand))) operand++;

    if(!(int)strlen(operand)){ /* the word was empty which mean the comm isn't in place*/
        print_error("comma need to be between the operands.");
        return NULL;
    }
    /*check if token has white space between words, before or after the comma*/
    
    oper_len = check_space_inside_word(operand);
    
    if(!oper_len){ /*token is not one word*/
        print_error( "to many words");
        return NULL;
    }
    /* Load the actual operand, adding '\0' at the end*/
    for(i = 0; i < oper_len; i++){
        holder[i] = *(operand+i);
    }
    holder[i++] = '\0';

    tmp_oper = (char *)malloc(sizeof(char)* oper_len +1);
    strcpy( tmp_oper, holder);

    clean_line(holder, i, CHAR);
    return tmp_oper;
}


int get_line(FILE *fd, char *line, int *word_location, int *num_of_words) {

    int tmpChar;/* hold the temporary char*/
    int word_len = 0; /* word length counter*/
    int i = 0, lineLen =0; /* keep track of how many characters*/
    Flag endFile = FALSE; /* EOF flag*/
    Flag inWord = FALSE;  /*inside a word flag*/

    /* clean the line and the word-locater from garbage values*/
    CLEAN_LINE(line,(int) strlen(line), CHAR );
    CLEAN_LINE(word_location,*(num_of_words) * 2, INT);

    num_of_words[0] = 0;
    /* Read charrachters one by one from stream file. stop when hit new line, or the end of the file*/
    while ((tmpChar = fgetc(fd)) != EOF) {
        lineLen++;

        if(lineLen  == MAX_LINE +1 ){/* 80 characters not including new line char*/
            print_error("Line is to long");
            return FALSE;
        }

        if (tmpChar != '\n') {
            /* need to check if last char was non-white char*/
            if (isspace(tmpChar)) {
                /*if so add it to the line stats*/
                IN_WORD

            /* got non-white char*/
            } else {
                /*new word here*/
                if (!inWord) {
                    inWord = TRUE;
                    word_location[(num_of_words[0]) * 2] = i;
                }
                /*increase the counter of existed word*/
                word_len++;
            }
            /* add to the line var*/
            line[i++] = tmpChar;

            /*here is the end of a line*/
        } else {
            /*using go to statment to get back here after eof flag is true for the first time*/
            lineEnd:
            /*check to see if a word need to be close by the last char*/
                IN_WORD
                line[i] = '\n';

            if (endFile) return NOT_FOUND;
            return 0;
        }
    }
    /* check for the EOF flag and go back to complete the line*/
    if (!endFile) {
        endFile = TRUE;
        goto lineEnd;
    }
return TRUE;

}

/* using the get_line function' return the Nth token in the line
 * if not found return null. caller must free the word*/
char *get_the_Nth_Word(char *line, int num_of_word, int wrd_wanted, int *locater) {
    char *tmp = NULL;
    int word_start = 0;
    int word_len = 0;
    if (num_of_word > wrd_wanted) {
        word_start = locater[wrd_wanted * 2];
        word_len = locater[(wrd_wanted * 2) + 1];

        tmp = (char *) malloc(1 + (word_len)*sizeof(char));
        *(tmp + word_len) = '\0';
        strncpy(tmp, line + word_start, word_len);
        return tmp;

    }
    return NULL;
}

void clean_line(void *line, int count, int mode) {
    int i;
    for (i = 0; i <= count && i < MAX_LINE; i++) {
        switch (mode) {
            case INT:
                (*((int *) line + i)) = 0;
                break;
            case CHAR:
                (*((char *) line + i)) = '\0';
        }

    }
}


FILE * open_file(char *fileName, char * mode, Flag type){
    char *fn ="";
    FILE * fd;
    fn = generate_files_suffix(fileName, type);
    fd = fopen(fn, mode);
    /*check to see if file open failed (file name not existed, or machine error fail).*/
    if(fd == NULL){
        print_error("couldn't open file \"%s\"", fn);
    }
    return fd;
}

char * generate_files_suffix(char *name, int suffix_mode){
    char *tmp;
    char *suf= "";
    tmp = (char *)malloc(sizeof(char)*(int)strlen(name)+1);
    strcpy(tmp, name);
    switch(suffix_mode){
        case(ASSM):
            suf = ".as";
            break;
        case(MACRO):
            suf = ".am";
            break;
        case(OBJ):
            suf = ".ob";
            break;
        case(ENT):
            suf = ".ent";
            break;
        case(EXT):
            suf = ".ext";
            break;
    }
    /*add the suffix to the end of the file name*/
    return strcat(tmp, suf);
}

void free_arguments(char **arguments, int numOfArg){
    int i;
    
    for(i =0; i< numOfArg; i++){
        if((arguments[i]) == NULL) continue;
        free(arguments [i]);
        /*initialize the arguments*/
        arguments[i] = "";
    }
   }


int get_reg_in_method(char *operand, int method) {
    if (operand != NULL) {
        /* either the word is registry or it's an index address method*/
        if (method == INDEX_ADDRESS) {
            return get_reg_fom_indx(operand);
        }
        else if (method == REG_ADDRESS) return get_reg(operand);
    }
    return FALSE;
}


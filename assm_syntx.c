
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "assm_syntx.h"

char *commands[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
                    "dec", "jmp", "bne", "jsr", "red", "prn","rts", "stop"};

char *instructions[] = {".entry", ".extern", ".data", ".string"};

char *regs[] = {"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10",
                "r11","r12","r13","r14","r15", "PSW"};

char *macr[] = {"macro", "endm"};

char *specialChar[] = { ",", ";", ":", "#", "[", "]", "\"", "+", "-"};

int methods [16][8] = { 
	{1,1, 1, 1,	 0,1,1,1},
	{1,1, 1, 1,	 1,1,1,1},
    	{1,1, 1, 1,      0,1,1,1},
    	{1,1, 1, 1,      0,1,1,1},
    	{0,1, 1, 0,      0,1,1,1},
    	{0,0, 0, 0,      0,1,1,1},
    	{0,0, 0, 0,      0,1,1,1},
    	{0,0, 0, 0,      0,1,1,1},
   	{ 0,0, 0, 0,      0,1,1,1},
    {0,0, 0, 0,      0,1,1,0},
    {0,0, 0, 0,  0,1,1,0},
    {0,0, 0, 0,  0,1,1,0},
    {0,0, 0, 0,  0,1,1,1},
    {0,0, 0, 0,  1,1,1,1},
    {0,0, 0, 0,  0,0, 0, 0},
    {0,0, 0, 0,  0,0, 0, 0}


};
int is_saved_word(char *word){
    if((get_command(word) < 0) && (get_instruct(word) < 0) && (get_reg(word) < 0)) return FALSE;
    return TRUE;
}
 int get_command(char * word) {
     return traverse_list(commands ,word, NUM_OF_COMM);
 }

int get_instruct(char * word) {
    return traverse_list(instructions ,word, FORTH+1);
}
int get_reg(char * word) {
    return traverse_list(regs ,word, NUM_OF_COMM);
}
char * get_special_char(Flag type){
    return specialChar[type];
}
int is_data_num(char *num){
    int i=0;
    long big_num =0;
    char *tmp= "";
    tmp = num;


    if(num != NULL){
        if(*tmp != SPECIAL_CHAR(HASHTAG)) return IMMI_MUST_OPEN_HASHTAG;
        if((int)strlen(tmp) == 1) return IMMI_ONLY_HASHTAG;
        tmp++;
        if(*tmp == SPECIAL_CHAR(PLUS) || *tmp == SPECIAL_CHAR(MINUS)) {
            if((int)strlen(num) == 1 ) {
                return IMMI_ONLY_OPERAND;
            }
            tmp++;
        }
        for(i =0; i < (int)strlen(tmp); i++){
            if (!isdigit(*(tmp+i))) {
                return IMMI_NON_DIGIT_CHAR;
            }
        }
        big_num = atol(tmp);
        if(big_num < (-1)*MAX_NUM_IMMID || big_num > MAX_NUM_IMMID -1)
            return IMMI_NUM_TO_BIG;

        return VALID;
    }
    return NOT_FOUND;
}
 int is_index(char *word) {
     char *open = "", *close = "";
     char lal[4] = {0};
     char lab[MAX_LABEL] = {0};
     int len = (int) strlen(word);
     int i = 0, lblRes = NOT_FOUND;
     char *tmp = "";
     if (word != NULL) {
         tmp = word;
        /*split the word, label part and reg part*/
         if ((open = strchr(word, SPECIAL_CHAR(LEFT_BRACK))) != NULL) {
             while (tmp != open) {
                 lab[i++] = *tmp;
                 tmp++;
             }
             /*check if label is valid*/
             if (!(lblRes = is_label(lab, MIDDLE))) {
                 /*check if there id closing bracket*/
                 if ((close = strrchr(word, SPECIAL_CHAR(CLOSE_BRACK))) != NULL && *close == *(word + len - 1)) {
                     open++;
                     /*get the reg*/
                     if (get_reg(strncpy(lal, open, close - open)) >= 0)
                         return VALID;

                     else return INDX_NO_REG;
                 }
                 return NO_CLOSE_BRACKETS;

             } else return lblRes;
         }
         return NO_OPEN_BRACKETS;
     }
     return NOT_FOUND;
 }

int traverse_list(char **list, char *word, int counter){
    int i =0;
    char **trvs;
    trvs = list;
    if(word != NULL) {
        while (i < counter) {
            if (strlen(*(trvs + i)) == (int) strlen(word)) {
                if (!strcmp(*(trvs + i), word)) return i;
            }
            i++;
        }
    }
    return NOT_FOUND;
}

int is_comment_line(char *line) {
    return *line == SPECIAL_CHAR(SEMI_COLON) ? TRUE : FALSE;
}

/* get the command type, check to see how many operand neddedreturn */
int get_command_num_of_operand(int command_type){
    if (command_type <ONE_OPERND_COMM) return 2;
    else if (command_type <ZERO_OPERND_COMM) return  1;
    else return 0;

}

int is_label(char *word, Flag type){
    int i;
    int word_len =0;
    char *tmp;
    if(word == NULL) return NOT_FOUND;

    word_len = strlen(word);

    tmp = word;
        if(is_saved_word(tmp)) return LABEL_IS_SAVED_WORD;
        if(word_len <= MAX_LABEL+type){
            if(!isalpha(*tmp)) return LABEL_MUST_OPEN_IN_ALPHA;
            if(type){
                if(*(tmp+word_len-1) != SPECIAL_CHAR(COLON)) return OPEN_LABEL_WITHOUT_COLON;
            }
            for(i =1; i < word_len-type; i++){
                if(!isalnum(*(tmp+i))) return LABEL_ONLY_ALPAHNUM;
            }
             return VALID;
        }
        else return LABEL_TO_LONG;

}
int has_the_address_method(int command, int method, Flag type_address){
    return methods[command][type_address +method];
}
int get_reg_fom_indx(char *operand){

    char *ptr;
    ptr =  strtok(operand, &SPECIAL_CHAR(LEFT_BRACK));
    ptr =  strtok(NULL,&SPECIAL_CHAR(CLOSE_BRACK));
    return get_reg(ptr);
}

int get_num_of_code_words(int method) {
    switch (method) {
        case (IMMID_ADDRESS):
            return 1;
        case (DIRECT_ADDRESS):
        case (INDEX_ADDRESS):
            return 2;

        case (REG_ADDRESS):
            return 0;
    }
    return NOT_FOUND;
}



int get_opcode(int command_type){
    switch(command_type){
        case(0): 
            return 0;
        case(1):
            return 1;
        case(2): 
        case(3):
            return 2;
        case(4): 
            return 4;
        case(5): 
        case(6):
        case(7):
        case(8):
            return 5;

        case(9): 
        case(10):
        case(11):
            return 9;
        default: return command_type;
    }
}
int get_funct(int commadType){
    if(commadType == 2 || commadType == 5 ||commadType == 9) return 10;
    if(commadType == 3 || commadType == 6 ||commadType == 10) return 11;
    if(commadType == 7 || commadType == 11) return  12;
    if(commadType == 8) return 13;
    return 0;
}





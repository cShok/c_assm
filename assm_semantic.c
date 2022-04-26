
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "assm_semantic.h"
#include "assm_syntx.h"
#include "parser.h"
#include "error_handler.h"

/* This file is the main semantic rules for the assembler. Gets a line (with line parameters) from parser, and using
 * the main syntax library, to determined if it's valid.
 */

/* Global vars for adding decode instructions*/
extern int gIc, gDc;


int analyz_data(char *line, int *word_loc, int num_of_words, Flag in_label, char **arguments, Flag type) {

    char * tmp_wrd = "";
    int instrct = NOT_FOUND;
    int num_of_words_to_code = NOT_FOUND;/* The return num of words in the arguments var*/
    /*to see that isn't null*/
    if (line == NULL) return NOT_FOUND;
    /* Get the first word, to check for the right instruct code (skips labels). */
    tmp_wrd = get_the_Nth_Word(line, num_of_words, FIRST + in_label, word_loc);
    instrct =get_instruct( tmp_wrd);
    free( tmp_wrd);

    if (instrct == STRING){

        /* calls another assm_semantic function to check for valid '.string'.*/
        num_of_words_to_code = decode_string(line, num_of_words, word_loc, arguments, in_label);
    }
    else if(instrct == DATA){

        /* calls the '.data' function, only without the .data instruction itself*/
        num_of_words_to_code = get_data_nums(line + word_loc[2 * (SECOND + in_label)], arguments);
    }

    return num_of_words_to_code;
}

int anlyz_refrenced(char *line, int *word_loc, int num_of_words, Flag in_label, Flag type, symbol_list **headSym) {

    char * tmp_wrd = "";
    Flag tmp_label = NOT_FOUND; /*if label exits, flag to skip it*/
    int sts = NOT_FOUND; /* by default it's not-valid*/
    symbol_list *tmpSym;/* tmp symbol list node to hold the node for an entry check*/

    /*See that isn't null*/
    if (line == NULL) return NOT_FOUND;

    tmp_wrd = get_the_Nth_Word(line, num_of_words, FIRST + in_label, word_loc);

    if ((get_instruct( tmp_wrd) == EXTERN || get_instruct( tmp_wrd) == ENTRY)) {
        free( tmp_wrd);
        tmp_wrd = get_the_Nth_Word(line, num_of_words, SECOND + in_label, word_loc);
        /* No other words in line beside the instruction word*/
        if (tmp_wrd == NULL) {
            print_error("empty referenced definition");

        }
        /* To-many words*/
        else if (num_of_words > SECOND + in_label + 1) {
            print_error("to many words in referenced definition");
        }
        /* First check if word is a valid label*/
        if (!(is_label( tmp_wrd, MIDDLE))) {
            /*try to find it in Symbol list*/
            tmpSym = find_in_symb_list(headSym, tmp_wrd);
            /* Extern in first_pass*/
            if (type == EXTERN) {
                /* In first cycle we don't want two label the same*/
                if (tmpSym != NULL) {
                    print_error("same label twice");
                }
                else {
                    /* Here we add the extern reference*/
                    add_to_symb_list(headSym, tmp_wrd, 0, EXTERN);
                    sts = TRUE;
                }
            }
            /*Entry in second_pass*/
            else {
                /* Inside the second cycle, we need a match in the symbol list*/
                if (tmpSym == NULL) {
                    print_error("no entry definition of the label \"%s\"", tmp_wrd);
                }

                else if (tmpSym->extrn) {
                    print_error("entry label  \"%s\" can't be also extern label", tmp_wrd);
                }
                else {
                    /* Update symbol to entry flag*/
                    tmpSym->entry = TRUE;
                    sts = TRUE;

                }
            }
        }
        /* The word isn't a valid label. Get the label error and return*/
        else {
            get_syntx_error(tmp_label);
        }
    }
    free( tmp_wrd);
    return sts;
    }


int anlyz_command(char *line, int *word_loc, int num_of_words, Flag in_label, char **return_args) {

    char *tmp_wrd = "";
    char *first_oper = "", *second_oper = ""; /*holds the temporary operands*/
    int command_type = NOT_FOUND;/* hold the command type (0-15)*/
    int switchType = NOT_FOUND;/* switch for the number of operands matching for each command type*/
    int num_of_code_word = NOT_FOUND; /* 1 - 6 if valid*/
    int code_word_first = NOT_FOUND, code_word_second = NOT_FOUND;/* hold the method of each operand*/

    /*See that isn't null*/
    if (line == NULL) return NOT_FOUND;

    tmp_wrd = get_the_Nth_Word(line, num_of_words, FIRST + in_label, word_loc);
    /* Get the command number*/
    command_type = get_command( tmp_wrd);
    /*Add the command to the first slot in 'arguments'*/
    *return_args = tmp_wrd;
    /* Get the supposed number of operands for each command*/
    switch (switchType = get_command_num_of_operand(command_type)) {
        case (0):
            if (num_of_words - in_label == 1) {
                num_of_code_word = 1;
            } else print_error("to many arguments for command \"%s\" with no operands", tmp_wrd);
            break;
        case (1):
            if (num_of_words - in_label == 2) {
                first_oper = get_the_Nth_Word(line, num_of_words, SECOND + in_label, word_loc);
                /* The method_anlyz function determine how many code words need for each method (0-4).*/
                num_of_code_word = method_anlyz(get_address_method(first_oper), command_type, DES);
                if (num_of_code_word != NOT_FOUND) {
                    /* adding two anyhow for the opcode word and the funct word*/
                    num_of_code_word += 2;
                    /*laoding the operand to arguments*/
                    *(return_args + 1) = first_oper;
                }
            } else {
                (num_of_words - in_label) > 2 ?
                    print_error("to many arguments for command \"%s\", need one operand", tmp_wrd) :
                    print_error("to little arguments for command \"%s\" need one operand", tmp_wrd);
            }
            break;

        case (2):
            /* start by looking for a comma*/
            if (( tmp_wrd = strchr(line + word_loc[SECOND + in_label], SPECIAL_CHAR(COMMA_C))) != NULL) {
                /*screen out line with to many or to little words*/
                if (num_of_words - in_label < 2) print_error("to little arguments for command \"%s\", need two operand", tmp_wrd);
                else if (num_of_words - in_label > 4) print_error("to many arguments for for command \"%s\", need two operand", tmp_wrd);
                /*get the two operands, if it is valid */
                if (!get_two_operands(line + word_loc[2 * (SECOND + in_label)], &first_oper, &second_oper)) {
                  /* get how many code words each method need , return (0 -4).*/
                    code_word_first = method_anlyz(get_address_method(first_oper), command_type, SRC);
                    code_word_second = method_anlyz(get_address_method(second_oper), command_type, DES);
                    /* Each method detected by method_anlyz is valid*/
                    if (code_word_first != NOT_FOUND && code_word_second != NOT_FOUND) {
                        num_of_code_word = code_word_first + code_word_second + 2;
                        /*load the operands here*/
                        *(return_args + 1) = first_oper;
                        *(return_args + 2) = second_oper;
                    }
                }
            } 
            else print_error("need a comma between operands in command \"%s\"", tmp_wrd);
            break;

        default:
            print_error("command not found");
    }
    return num_of_code_word;
}


int get_two_operands(char *line,char **first_oper ,char ** second_oper){

    int com_num = 0; /*comma checker*/
    char *fTmp ="", *sTmp ="";/* two char pointer to hold the two half of the line around the comma*/
    int i =0;

    if(first_oper == NULL) return NOT_FOUND;

    /* Travers over and check the number of comma. Only need one*/
    for(i =0; i < (int)strlen(line); i++){
        if(*(line +i) == SPECIAL_CHAR(COMMA_C)) com_num++;
    }
    if(com_num != 1) {
        com_num > 1 ? print_error("to many commas, only one needed.") :
        print_error("need a comma to separate the operands.");
        return NOT_FOUND;
    }
    /*split the line round the comma*/
    fTmp = strtok(line, &SPECIAL_CHAR(COMMA_C));
    sTmp= strtok(NULL, &SPECIAL_CHAR(COMMA_C));

    /* using small function to determined if there is no two words insdie the split token*/
    *first_oper = get_operand_with_comma(fTmp);
    *second_oper = get_operand_with_comma(sTmp);

    if(*first_oper != NULL && *second_oper != NULL) return 0;
    return NOT_FOUND;
}


int method_anlyz(int method,int command_type, Flag type_address){

    int num_of_code_words = NOT_FOUND;/* hold the number of word code needed*/
    
    if(method != NOT_FOUND){
        /*check to see if the method is legal with that command*/
        if(has_the_address_method(command_type, method, type_address)){
            /* if so, return the number needed*/
            return num_of_code_words =get_num_of_code_words(method);
        }
        else{
            print_error("this address method isn't legal with this operator");
        }
    }
    else print_error("address method isn't recognized");

    return num_of_code_words;
}


int line_decider(char *line, int *word_loc, int num_of_words, int *with_label) {
    char *tmp_wrd ="";

    Flag type_checker = FALSE, type_line = NOT_FOUND; /*two flags to determined which words we have*/
    if (num_of_words < 1) return EMPTY;/*empty line*/
    if (is_comment_line(line)) return COMMENT;/*comment line*/

    tmp_wrd = get_the_Nth_Word(line, num_of_words, FIRST, word_loc);
    /* Label checker*/
    if (!is_label( tmp_wrd, OPEN)) {
        with_label[0] = TRUE;
        if(num_of_words == 1) {
            type_line = NOT_FOUND;
            print_error("Error, label \"%s\" is without any assigmnet", tmp_wrd);
        }
        free( tmp_wrd);

    }
    tmp_wrd = get_the_Nth_Word(line, num_of_words, with_label[0] + FIRST, word_loc);
    /* check to see if the word is an instruction saved word*/
    if ((type_checker = get_instruct( tmp_wrd)) != NOT_FOUND) {
        type_line = type_checker;
    }
    /* Check to see for a command name*/
    else if ((type_checker = get_command( tmp_wrd)) != NOT_FOUND) {
        type_line = COMMAND;
    }
    free( tmp_wrd);
    return type_line;
}


int get_address_method(char *word) {
  
    char *tmp = "";
    int res = NOT_FOUND;

    if (word != NULL) {

        tmp = word;
        /* Hashtag valid only in immediate addressing*/
        if (*(tmp) == SPECIAL_CHAR(HASHTAG)){
            /*check for a valid number after the hashtag*/
            if(!(res = is_data_num(tmp))) return IMMID_ADDRESS; ;
        }
        /* Left brackets and right brackets valid only in indexing method*/
        else if (strchr(tmp, SPECIAL_CHAR(LEFT_BRACK)) != NULL) {
            /* check for valid label and regestry*/
            if(!(res = is_index(tmp))) return INDEX_ADDRESS;
        }
        /* check for registry addressing*/
        else if (get_reg(word) != NOT_FOUND) return REG_ADDRESS;
        /* check for direct addressing (label only).*/
        else if (!(is_label(word, MIDDLE))) return DIRECT_ADDRESS;
    }
    /*get the error if needed*/
    if(res > 0) get_syntx_error(res);
    return NOT_FOUND;
}


int get_imm_num(char *word){
    
    int mask =1; /* bit mask*/
    int  i, return_num;
    Flag minus =0; /* if the number is with leading minus, need to 'flip' it*/
    char *trev ="";
    octal_bit num;
    num.mis = 0;
    trev = word;
    trev++; /* skip the hashtag*/
    /*check for '+' or '-'*/
    if(!isdigit(*(trev))){
        if(*(trev) == SPECIAL_CHAR(MINUS)) minus = 1;
        trev++;
    }
    /* cast the number from the given word*/
    return_num = atoi(trev);
    /*if minus, flip each bit and then add 1.*/
    if(minus){
        num.mis = return_num;
        for(i =0 ; i <16; i++){
            num.mis = num.mis ^ mask;
            mask = mask << 1;
        }
        return_num = num.mis + 1;
    }

    return return_num;
}


int decode_string(char *line,int num_of_words,int * word_loc,char ** arguments, Flag in_label){

    char * tmp_wrd ="", *str_open ="", *str_close = "", *last_word ="";
    char tmpChar[2]= {0};
    int num_of_words_to_code = NOT_FOUND;
    tmp_wrd = get_the_Nth_Word(line, num_of_words, SECOND + in_label, word_loc);
    /* Check for quotation at the start and in the end, and that they aren't the same*/
    if (* tmp_wrd != SPECIAL_CHAR(QUOTATION)) {
        print_error("string instruction must open in \" .");
        return NOT_FOUND;
    }
    str_open = strchr(line, SPECIAL_CHAR(QUOTATION));
    str_open++;
    last_word = get_the_Nth_Word(line, num_of_words, num_of_words - 1, word_loc);
    /*gets the last character in the sentence*/
    str_close = (last_word + (int) strlen(last_word) - 1);
    if (*str_close != SPECIAL_CHAR(QUOTATION)) {
        print_error("string must finish in \" .");
        return NOT_FOUND;
    }
    if (strchr(line, SPECIAL_CHAR(QUOTATION)) == (str_close = strrchr(line, SPECIAL_CHAR(QUOTATION)))) {
        print_error("string opened but not closed.");
    }

    if (str_open == str_close) {
        print_error("empty string.");
        return NOT_FOUND;
    }
    /*if we are here surely with open and close quotation*/
    num_of_words_to_code = 0;
    free( tmp_wrd);
    free(last_word);
    /*for each ASCII character add new arguments to 'arguments' at the end add the '\0' char.*/
    while (str_open != str_close) {
        tmp_wrd = (char *)malloc(sizeof(char) + 1);
        tmpChar[0] = *str_open;
        strcpy( tmp_wrd,tmpChar);
        *(arguments + num_of_words_to_code++) = tmp_wrd;
        str_open++;
    }
    tmp_wrd = (char *)malloc(sizeof(char) + 1);
    tmpChar[0] = '\0';
    strcpy( tmp_wrd, tmpChar);
    *(arguments + num_of_words_to_code++) = tmp_wrd;

    return num_of_words_to_code;

}

void decodeCode(code_list **head, char **arguments, Flag data_type, int num_word_to_code, symbol_list **head_symbol) {

    code_list tmp; /*temporary pointer to load to code list*/
    int i = 0, tmpChar = 0;
    int command = NOT_FOUND;
    char *first_oper = NULL, *sec_oper = NULL;

    /*initlize the temporary pointer to the most common case, A is true*/
    tmp.BitFiled.iter.T = 0;
    tmp.BitFiled.operand.A = 1;
    tmp.next = NULL;
    /*according to type know what to add*/
    switch (data_type) {
        case (COMMAND):
            tmp.data_type = OPER_CODE; /*regular code, add under the gIc counter*/
            tmp.address = gIc++;
            /* quick calculation of the opcode*/
            tmp.BitFiled.operand.opcode = pow(2, get_opcode(command = get_command(*arguments)));
            add_to_code_list(head, &tmp);
            /* command with no opernads stop here*/
            if (num_word_to_code > 1) {
                first_oper = *(arguments + 1);
                if (command < ONE_OPERND_COMM) {
                    sec_oper = *(arguments + 2);
                }
                /*send the operand(s) to the recursive function*/
                addExtraWords(head, first_oper, sec_oper, command, FUNCT);
            }
            break;
            /* data and string instruction are similar.*/
        case (DATA):
        case (STRING):
            tmp.data_type = DATA_CODE; /*data code, increase the gDc counter, flag for the second pass to upadte*/
            for (i = 0; i < num_word_to_code; i++) {
                if (data_type == DATA) {
                    tmpChar = atoi(*(arguments + i));
                } else tmpChar =(unsigned char) *(*(arguments + i));
                tmp.BitFiled.operand.opcode = (int) tmpChar;
                tmp.address = gDc++;
                add_to_code_list(head, &tmp);
            }
            break;
    }
}


void get_syntx_error(int num){

    switch (num) {
        case(OPEN_LABEL_WITHOUT_COLON):
            print_error("Label definition must end with colon.");
            break;
        case(LABEL_MUST_OPEN_IN_ALPHA):
            print_error("Label open with alphabet char.");
            break;
        case(LABEL_ONLY_ALPAHNUM):
            print_error("Label can only contain alphanumeric chars.");
            break;
        case(LABEL_TO_LONG):
            print_error("Label to long.");
            break;
        case(LABEL_IS_SAVED_WORD):
            print_error("Label can't be a saved word.");
            break;
        case(IMMI_MUST_OPEN_HASHTAG):
            print_error("Immediate address must open with hashtag.");
            break;
        case(IMMI_ONLY_HASHTAG):
            print_error("Immediate number is only hashtag.");
            break;
        case(IMMI_ONLY_OPERAND):
            print_error("Immediate number with only operand.");
            break;
        case(IMMI_NON_DIGIT_CHAR):
            print_error("Immediate number with non-digit char.");
            break;
        case(IMMI_NUM_TO_BIG):
            print_error("Immediate number overflow.");
            break;
        case(NO_OPEN_BRACKETS):
            print_error("Index address without opening bracket '[' .");
            break;
        case(NO_CLOSE_BRACKETS):
            print_error("Index address without closing bracket ']' .");
            break;
        case(INDX_NO_REG):
            print_error("Index address without proper registry.");
            break;

    }
}

void addExtraWords(code_list **head_code,char * first_oper,char * sec_oper, int command_type, Flag type) {

    code_list tmp_code;/* temporary pointer to add to code list*/

    int fMethod = NOT_FOUND, sMethod = NOT_FOUND; /*method to decode*/
    int dest = 0;
    /*initialize tmp_code to the most common option. A true, later may change*/
    tmp_code.BitFiled.iter.T = 0;
    tmp_code.data_type = OPER_CODE;
    tmp_code.BitFiled.operand.A = TRUE;
    /*stop the recursive*/
    if (first_oper == NULL && sec_oper == NULL) return;

    tmp_code.address = gIc++;
    fMethod = get_address_method(first_oper);
    if (sec_oper != NULL) {
        sMethod = get_address_method(sec_oper);
    }
    /* the switch determent in which words we are*/
    switch (type) {
        case (FUNCT):
            /* decode the second word - (i.e. FUNCT) switch between source and destination in case of two operands command.*/
            tmp_code.BitFiled.data.funct = get_funct(command_type);
            tmp_code.BitFiled.data.desMethod = fMethod;
            dest = tmp_code.BitFiled.data.desReg = get_reg_in_method(first_oper, fMethod);
            if (sec_oper != NULL) {
                tmp_code.BitFiled.data.desMethod = tmp_code.BitFiled.data.desReg = 0;
                tmp_code.BitFiled.data.srcMethod = fMethod;
                tmp_code.BitFiled.data.srcReg = dest;
                tmp_code.BitFiled.data.desMethod = sMethod;
                tmp_code.BitFiled.data.desReg = get_reg_in_method(sec_oper, sMethod);
            }

            add_to_code_list(head_code, &tmp_code);
            /*recursively send the information gathered to decode the extra words for method (0, 1, 2).*/
            if (fMethod != NOT_FOUND && fMethod != REG_ADDRESS) {
                addExtraWords(head_code, first_oper, sec_oper, command_type, fMethod);
            }
            if (sMethod != NOT_FOUND && sMethod != REG_ADDRESS) {
                addExtraWords(head_code, sec_oper, NULL, command_type, sMethod);
            }
            break;

        case (IMMID_ADDRESS):
            tmp_code.BitFiled.operand.opcode = get_imm_num(first_oper);
            add_to_code_list(head_code, &tmp_code);
            break;
        case (INDEX_ADDRESS):
        case (DIRECT_ADDRESS):
        case (OFFSET):
                tmp_code.BitFiled.iter.T = 0;
                tmp_code.BitFiled.operand.R = TRUE;
                add_to_code_list(head_code,&tmp_code);
                if (type == OFFSET) return; /* second word of ,method 1 or 2 stops here*/
                /*first word of method 1 or 2, send recursivley the again to add the offset code word.*/
                addExtraWords(head_code, first_oper, sec_oper, command_type, OFFSET);
            break;

    }
}

void update_label_in_method(code_list **head_code,symbol_list *head_symbol,generic_list **head_extern,char * first_oper, char *sec_oper){
code_list *ptr_code;
symbol_list *tmpPtr;
int method = NOT_FOUND;
ptr_code = *head_code;

if (first_oper == NULL) return;
method = get_address_method(first_oper);
switch(method) {
case (REG_ADDRESS):
    break;
case (IMMID_ADDRESS):
    ptr_code = ptr_code->next;
    break;
case (INDEX_ADDRESS):
case (DIRECT_ADDRESS):
    tmpPtr = find_in_symb_list(&head_symbol, method == DIRECT_ADDRESS ? first_oper : strtok(first_oper, &SPECIAL_CHAR(
            LEFT_BRACK)));
    if (tmpPtr == NULL) {
        print_error("label not found in operanda method");
        return;
    } else {
    
        if (tmpPtr->code || tmpPtr->data) {
            ptr_code->BitFiled.operand.opcode = tmpPtr->base;
            ptr_code->next->BitFiled.operand.opcode = tmpPtr->offSet;
           

        }

        else {
         
            ptr_code->BitFiled.operand.E = TRUE;
            ptr_code->BitFiled.operand.R = FALSE;
            add_to_list(head_extern, tmpPtr->name, "BASE", ptr_code->address);
            ptr_code->next->BitFiled.operand.E = TRUE;
            ptr_code->next->BitFiled.operand.R = FALSE;
            add_to_list(head_extern, tmpPtr->name, "OFFSET", ptr_code->next->address);
        }
        ptr_code = ptr_code->next->next;
    }
    break;
}
    update_label_in_method(&ptr_code, head_symbol, head_extern,sec_oper, NULL);
}

int check_data_definition(char *line) {

int i =0, j =0;
int tmpChar = NOT_FOUND, prev = NOT_FOUND;
char trim[MAX_LINE] = {0};
int err =0;

for (i = 0; i < (int) strlen(line) - 1; i++) {
tmpChar = *(line + i);

if (isdigit(tmpChar)) {

    if (isdigit(prev)) continue;

    else {
        trim[j++] = 'd';
        prev = tmpChar;
    }

} else if (isspace(tmpChar)) {

    if (isspace((prev))) continue;
    else {
        trim[j++] = 's';
        prev = tmpChar;
    }
} else if (tmpChar == SPECIAL_CHAR(COMMA_C)) {
    trim[j++] = 'c';
    prev = tmpChar;
} else if (tmpChar == SPECIAL_CHAR(PLUS) || tmpChar == SPECIAL_CHAR(MINUS)) {
    trim[j++] = 'p';
    prev = tmpChar;
} else {
    print_error("non digit charchter in data defintion");
    err++;
}
}

if (*trim == SPECIAL_CHAR(COMMA_C)) {
print_error("data definition can't start with a comma");
err++;
}
if (*(trim + j - 1) != 'd' && strstr(trim + j - 2, "ds") == NULL) {
print_error("data defenition musr end with a number");
err++;
}
if ((strstr(trim, "pp") != NULL) || (strstr(trim, "cc") != NULL)) {
print_error("two non-digit characters in a row");
err++;

}
if (strstr(trim, "dp") != NULL) {
print_error("after digit cant come plus or minus.");
err++;
}
if ((strstr(trim, "dsd") != NULL) || (strstr(trim, "dspd") != NULL)) {
print_error("two numbers without comma in between.");
err++;

}

if ((strstr(trim, "ps") != NULL) || (strstr(trim, "pc") != NULL)) {
print_error("after plus or minus must come the number.");
err++;
}

return err;
}

int get_data_nums(char *line, char **return_size){
int numOfNums =0;


char *ptr ="", * tmp_wrd = "";
long int check = NOT_FOUND;


if(!check_data_definition(line)){
ptr = strtok(line, &SPECIAL_CHAR(COMMA_C));
while (ptr != NULL) {
    check = atol(ptr);
    if (check < (-1 * MAX_NUM_16) || check >= MAX_NUM_16) {
        print_error("the number is to big, overflow", check);
    }
    tmp_wrd = (char *) malloc(sizeof(char) * (int) strlen(ptr) + 1);
    strcpy( tmp_wrd, ptr);
    return_size[numOfNums++] = tmp_wrd;

    ptr = strtok(NULL, &SPECIAL_CHAR(COMMA_C));

}
return numOfNums;
}
return FALSE;
}

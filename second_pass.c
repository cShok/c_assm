/* Chanany Shok*/
#include <stdlib.h>
#include "second_pass.h"
#include "parser.h"
#include "assm_semantic.h"


/* get the head of the code table and the symbol table from first pass*/
extern code_list  *head_code;
extern symbol_list *head_symbol;

/* Global vars for memory counter. Separate Data and regular Code, for adding data type at the end of the code picture*/
    extern int gIc, gDc;

/* Global vars for error handling*/
    extern char *gLineNum;
    extern Flag gState;
    extern int gErr;


int second_pass(FILE *fd, char *fn){

    /* Three vars used by the get_line function, to store the processed lines' data.*/
    int num_of_words[1]= {0}; /* Return with the number of words in the lines (max 40 words)*/
    char line[MAX_LINE+1] = {0}; /* Get the line here*/
    int word_loc[MAX_LINE / 2] = {0}; /* Get the starting point for each word (odd), and the length (even)*/
    int word_to_code = NOT_FOUND; /* store the number of memory words need for this line*/
 	int i =0;
    char *arguments[MAX_LINE/ 2] = {0}; /* Maximum arguments in code line. numbers, characters or operands*/

    int in_label[1] = {0}; /* if label detected*/
    int line_type = NOT_FOUND; /* used by the line_decider ,switching between 6 possible lines in the assembler*/
    Flag eof = FALSE; /* Read file until parser return EOF (getc)*/

        /* initialize head node to hold the extern table, two node a reference -one for the base the other
         * for the offset, utilizing the generic list struct.*/
    generic_list *head_extern;

    /*travers over the code list, to get to the words needed update (method with labels*/
    code_list *ptr_code;
   

    head_extern =NULL;
    ptr_code = head_code;
    gLineNum = 0; /*new count for the second pass*/

    while (!eof) {
        /*Same architecture from the first pass. Line by Line, only need to update entry instruction and
         * commands with labels referenced in the operands (i.e. address method 1 and 2).
         * parsing while assuming, nly if the first pass passed with no error, can get here/*/

        eof = get_line(fd, line, word_loc, num_of_words);
        gLineNum++;

        in_label[0] = 0;
        line_type = line_decider(line, word_loc, *num_of_words, in_label);

        switch(line_type) {

            case (ENTRY):
                /* Analyze entry instruction, make sure it's exit, and also not already defined as extern label*/
                anlyz_refrenced(line, word_loc,num_of_words[0],in_label[0], line_type, &head_symbol);
                break;
            case (EXTERN):
            case (DATA):
            case (STRING):
                break;
            case (COMMAND):
                /* By the word_to_code traverse over the code table. If command with address method 1 or 2, need to update*/
                word_to_code = anlyz_command(line, word_loc,num_of_words[0],in_label[0],arguments);
                for(i =0; i < word_to_code; i++){
                    ptr_code= ptr_code->next;
                    if(i==1) {
                        /* Only can fail if not finding the labeles in the operand(s).*/
                      
                        update_label_in_method(&ptr_code,head_symbol,&head_extern, *(arguments+1), *(arguments+2));
                    }
                }
                free_arguments(arguments, 1 + get_command_num_of_operand(get_command(*arguments)));
                word_to_code = NOT_FOUND;
                break;
            case (EMPTY):
            case (COMMENT):
                break;
        }

        CLEAN_LINE(line, MAX_LINE, CHAR);
    } /* end of parser*/

    /* If no error accrued till here, compiled the file successfully*/
    if(gErr == FALSE)
        generate_compiler_files(&head_extern, fn);


    /* Free the allocated lists*/
    free_list(head_extern);
    free_code_list(head_code);
    free_symb_list(head_symbol);
    return gErr;
    }


void generate_compiler_files(generic_list **headExt ,char * fn) {

    /*Tree pointer for each table. */
    symbol_list *tmp_symb;
    code_list *tmp_code;
    generic_list *head_extern;
	FILE *fd;
    

    head_extern = *headExt;

    tmp_code = head_code;
    
    tmp_symb = head_symbol;
    /* Only open file if there is entry instruction at-all*/
    if (hasEntry(tmp_symb)) {
        fd = open_file(fn, "w", ENT);
        while (tmp_symb != NULL) {
            if (tmp_symb->entry) {
                fprintf(fd, "%s,%d,%d\n", tmp_symb->name, tmp_symb->base, tmp_symb->offSet);
            }
        tmp_symb = tmp_symb->next;
        }
        fclose(fd);
    }

    /* Only open file, if there is extern instruction*/
    if(head_extern != NULL){
        fd = open_file(fn, "w", EXT);
        while(head_extern != NULL){
            /*one for the base one for the offset*/
            fprintf(fd, "%s %s %d\n", head_extern->name,head_extern->content, head_extern->address);
            head_extern = head_extern->next;
            fprintf(fd, "%s %s %d\n\n", head_extern->name,head_extern->content, head_extern->address);
            head_extern = head_extern->next;
        }
    }
    if(tmp_code != NULL){
        /* Can be a file with no error yet no code-byte either*/
        fd = open_file(fn, "w", OBJ);
        fprintf(fd, "\t\t\t%d\t%d\n", gIc-100,gDc);
            while (tmp_code != NULL) {
            /*fprintf(fd, "%04d ", tmp_code->address);*/
            fprintf(fd,"A%x-B%x-C%x-D%x-E%x\n", (int)tmp_code->BitFiled.fb.A,(int)tmp_code->BitFiled.fb.B,
                (int)tmp_code->BitFiled.fb.C,(int)tmp_code->BitFiled.fb.D, (int)tmp_code->BitFiled.fb.E);
                tmp_code = tmp_code->next;
             }
    }
}


int hasEntry(void){
    symbol_list *tmp;
    tmp = head_symbol;
    while(tmp != NULL){
        if(tmp->entry) return TRUE;
        tmp= tmp->next;
    }
    return FALSE;
}




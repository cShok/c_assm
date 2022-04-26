/* Channay Shok*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "first_pass.h"
#include "parser.h"
#include "error_handler.h"
#include "assm_semantic.h"

/*initialization of the code list and the symbol list*/
code_list *head_code;
symbol_list *head_symbol;

/* Global vars for memory counter. Separate Data and regular Code, for adding data type at the end of the code picture*/
extern int gIc, gDc;

/* Global vars for error handling*/
extern char *gLineNum;
extern Flag gState;
extern int gErr;

int first_pass(FILE *fd) {

    /* Three vars used by the get_line function, to store the processed lines' data.*/
    int num_of_words[1]= {0}; /* Return with the number of words in the lines (max 40 words)*/
    char line[MAX_LINE+1] = {0}; /* Get the line here*/
    int word_loc[MAX_LINE / 2] = {0}; /* Get the starting point for each word (odd), and the length (even)*/

    char *arguments[MAX_LINE/ 2] = {0}; /* Maximum arguments in code line. numbers, characters or operands*/

    int in_label[1] = {0}; /* if label detected*/
    int line_type = NOT_FOUND; /* used by the line_decider ,switching between 6 possible lines in the assembler*/
    Flag eof = FALSE; /* Read file until parser return EOF (getc)*/
    Flag address = NOT_FOUND; /* address type, to separate data code( String a Data instruction), and commands*/

    char *tmp_wrd = ""; /* Used to get words from the line, using get_the_Nth_word function*/
    char *tmp_label = ""; /*used to store potential label name*/
    int word_to_code = NOT_FOUND; /* store the number of memory words need for this line*/

    symbol_list *tmp_symb_ptr =NULL; /* Temporary pointer to check if label is already existed*/

    /* initialize the symbol table and the code table*/
    head_symbol = NULL;
    head_code= NULL;

    gLineNum =0;/* Initialize line counter after macro*/
    gState = MACRO; /*file name id changed to '*.am'*/

    while (!eof) {

        gLineNum++;
        in_label[0] =0;
        /*get line data*/
        eof = get_line(fd, line, word_loc, num_of_words);
        /* check two first words to determined if there is opening label, and what kind of line is it.*/
        line_type = line_decider(line, word_loc, *num_of_words, in_label);
	
        switch(line_type) {

            /* Skip to the second pass */
            case (ENTRY):
                break;

            /* anlize if the label is valid, if so, add it to the symbol table under extern flag*/
            case (EXTERN):
               
                anlyz_refrenced(line, word_loc,num_of_words[0],in_label[0], line_type,&head_symbol);
                if(in_label[0] == 1){
                    printf("warning, label for en extern instrcition\n");
                }
                break;

            /* Generic function to handle dtat insruction, the number of word nedde to code, is returing in case
             * of zero-error, else return NOUT_FOUND*/
            case (DATA):
            case (STRING):
                word_to_code = analyz_data(line, word_loc,num_of_words[0],in_label[0], arguments, line_type);
                break;

            /* the main analyzer. Get each command instruct and crack if it is valid*/
            case (COMMAND):
            	
                word_to_code = anlyz_command(line, word_loc,num_of_words[0],in_label[0],arguments);
             
                break;
            /* Only space characters, or line opened with semicolon, get skipped as-well.*/
            case (EMPTY):
            case (COMMENT):
                break;

            /* the line decider failed to detect. meaning first or second word are unrecognized words by the assembler*/
            default:
                tmp_wrd = get_the_Nth_Word(line, *num_of_words, FIRST +*in_label,word_loc);
                if(in_label[0] == TRUE) /* Label definition is good, second line problematic*/
                    print_error("second word \"%s\" isn't an assembler saved word", tmp_wrd);
                    /* first word isn't recognize */
                else  print_error("first word \"%s\" isn't a valid saved word neither a label", tmp_wrd);

                free(tmp_wrd);
        }

        /* According to the assigment in the switch on Word_to_code,
         * determined if error accrued or not, proceeding to add label if exist.*/
        if(word_to_code > 0) {
      
            /*memory over-flow on the assembler's machine*/
            if (gIc + gDc + word_to_code >= MAX_MEMORY) {
                print_error("not enough memory. maximum memory is %d.", MAX_MEMORY);

            } else {
                /* label definition*/
                if (in_label[0] != 0) {
               
                    address = (line_type == COMMAND) ? gIc : gDc; /*address type*/
                    tmp_wrd = get_the_Nth_Word(line, *num_of_words, FIRST, word_loc);
                    tmp_label = strtok(tmp_wrd, &SPECIAL_CHAR(COLON)); /*get rid of the colon*/
                    /* Label aren't exist*/
                    if((tmp_symb_ptr = find_in_symb_list(&head_symbol, tmp_label)) == NULL)
                            add_to_symb_list(&head_symbol, tmp_label, address, line_type);
                    /* Label already defined*/
                    else print_error("Can't define the same label twice \"%s\"", tmp_label);
                    free( tmp_wrd);
                }
                /* this function decode only command instructions, data and string already decoded*/
                
                decodeCode(&head_code, arguments, line_type, word_to_code, &head_symbol);

                /*free the arguments, one + number of operands for commands, and the numbers of numbers or characters + 1
                 * for data definition.*/
                if (line_type == COMMAND) line_type = 1 + get_command_num_of_operand(get_command(*arguments));
                else line_type = word_to_code;

            }
        }
              /*free_arguments(arguments, line_type);*/
              word_to_code = NOT_FOUND;
              CLEAN_LINE(line, MAX_LINE, CHAR);
    }/*end of file*/

    /*if error accrued, need to free the lists*/
    if(gErr){
        free_code_list(head_code);
        free_symb_list(head_symbol);
    }
    /*Move the data type to the end, and then update the symbol tale accordingly*/
    else{
        head_code = rearrange_code(head_code);
        update_symb_list(head_symbol);
    }
return gErr;

}


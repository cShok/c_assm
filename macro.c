/*Chanay Shok*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"
#include "assm_syntx.h"
#include "structs.h"
#include "parser.h"
#include "error_handler.h"
/* Global vars for error handler*/
extern int gErr;
extern int gLineNum;
extern Flag gState;

/* Main macro function. return zero if unfolding macro succeeded. using generic list to dynamically store the macros.
 * only return NOT_FOUND if line is too long.*/

int macro_anlz(FILE *fd, char * fileName) {

    generic_list *head_macro = NULL; /* Initialize the head of the macro list, each line in a new node*/
    generic_list *tmp_macro = NULL;

    /* Three vars used by the get_line function, to store the processed lines' data.*/
    int num_of_words[1]= {0}; /* Return with the number of words in the lines (max 40 words)*/
    char line[MAX_LINE+1] = {0}; /* Get the line here*/
    int word_loc[MAX_LINE / 2] = {0}; /* Get the starting point for each word (odd), and the length (even)*/

    char tmp_name[MAX_LINE] = {0};  /*hold the temporary name of the macro (no label length limitation).*/
    int line_in_macro =0; /*save in each node its line number inside the macro*/

    Flag eof = FALSE; /* Read file until parser return EOF (getc)*/

    Flag in_macro = 0;
    char *tmp_wrd = ""; /* Used to get words from the line, using get_the_Nth_word function*/
 	FILE *new_file;
    gLineNum=0; /* Initialize line counter.*/
    gState = ASSM; /* Suffix for error_handler is now '*as'.*/

    /* New file '*.am', for the unfold file*/
   
    new_file = open_file(fileName, "w", MACRO);
    if(new_file == NULL){
        print_error("Machine Error, can't open the assembler file");
    }

    while(!eof){ /* Get line by line, maximum 80 characters plus 1 for the end of line char*/

        eof = get_line(fd, line, word_loc, num_of_words);
        gLineNum++;
	
            /*empty line get skkiped in macro */
            if (num_of_words[0] == FALSE) {
                continue;
            }
            /* Aren't in macro definition. Check to see if it is a macro definition,
             * or if not, check if a macro referenced here.*/
            else if (!in_macro) {

                /*get the second word as the name of the macro, (caller must free allocated memory).*/
                tmp_wrd = get_the_Nth_Word(line, *num_of_words, FIRST, word_loc);

                /*macro defifnition is here*/
                if (!strcmp( tmp_wrd, "macro")) {

                    free( tmp_wrd);

                    tmp_wrd = get_the_Nth_Word(line, *num_of_words, SECOND, word_loc);
                    strncpy(tmp_name, tmp_wrd, (int) strlen( tmp_wrd));
                    free( tmp_wrd);
                    in_macro = TRUE;

                /*Not macro check if the first word is macro already in the macro list*/
                } else {

                    tmp_wrd = get_the_Nth_Word(line, *num_of_words, FIRST, word_loc);
                    tmp_macro = find_in_list(head_macro, tmp_wrd);
                        if (tmp_macro != NULL) { /*Find that it is a macro*/
                            /*Travers over the macro list to get each line with the macro's name.*/
                            while(tmp_macro !=  NULL && (!strncmp(tmp_macro->name, tmp_wrd, (int)strlen( tmp_wrd)))) {
                            fprintf(new_file, "%s", tmp_macro->content);
                            tmp_macro = tmp_macro->next;
                            }
                        }
                        /* Just copy the line to the new_file*/
                        else fprintf(new_file, "%s", line);
                    free( tmp_wrd);

                }
            }

            /*In macro definition.*/
            else {
                tmp_wrd = get_the_Nth_Word(line, *num_of_words, FIRST, word_loc);

                /*End of macro definition. Initialize the macro flag, and clean the tmp_name */
                if (!strncmp( tmp_wrd, "endm", 4)) {
                    line_in_macro =0;
                    in_macro = FALSE;
                    CLEAN_LINE(tmp_name, (int)strlen(tmp_name), CHAR);

                } else {
                    /* Adding another line to the specific macro*/
                    add_to_list(&head_macro, tmp_name, line, line_in_macro++);
                }

                free( tmp_wrd);

            }
        CLEAN_LINE(line,MAX_LINE,CHAR);
        }


    free_list(head_macro); /*free the allocated memory of the list*/
    fclose(new_file); /*close the original file*/
    return gErr;
}

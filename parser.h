

#ifndef MMN14_PARSER_H
#define MMN14_PARSER_H

#include <stdio.h>
#define DIGIT 2
#define OPER 3
#define COMMA 4
#define SPACE 5

#define CLEAN_LINE(line,count,mode) {\
clean_line(line,count,mode);\
}


/* The main parser function. Use getc() to get char by char from file stream. Send back to caller line by line,
 * with stats: how many words (maximum 40), where each word start and how long is it. Return zero if stream isn't ended
 * or EOF if file ended. If line to long (over 80 characters) add an error/
 * */
int get_line(FILE *fd, char *line, int *word_location, int * num_of_words);


/* The main word function. Use the stats from get_line to get the required token in the line.
 * Return pointer to the word or NULL if word wanted isn't exist. Word is malloced caller must free/
 * */
char * get_the_Nth_Word(char *line, int num_of_word ,int wrd_wanted,int *locater);


/* The main function to handle the assembler files extensions. Get the name of the file, the mode to open it,
 * and the type for the suffix required. Use the fopen() function with check for errors.
 * */
FILE * open_file(char *file_name, char * mode, int type);


/* Small function to handle the assembler files extensions. Get the name of the file and the type needed.
 * Return pointer to the full file name*/
char * generate_files_suffix(char *name, int suffix_mode);


/* Utility function to clean any static array inside a running funtions
 * */
void clean_line(void *line, int count, int mode);


/*small function to free the arguments collected in string data or command instructions.
 * */
void free_arguments(char **arguments, int num_of_args);


/* The main function to pars the '.data' instructions. Gets the line from the start of the numbers, and determent
 * if its valid data. Load the numbers into 'arguments'. Return the number of numbers collected, or NOT_FOUND if error
 * accrued.*/
int get_data_nums(char *line, char **arguments);


/* Parent function to find a registry inside a word (address method). Use the get_reg() and strtok() library function
 * to find reg inside REG_ADDRESS or INDEX_ADDRESS. Return the reg number if find it, or NOT_FOUND if not.*/
int get_reg_in_method(char *operand, int method);

/* Small utility function. Gets a token and check if it is an actully few words separated by white spaces.
 * return positive number representing the actual length of the word, or FALSE if there are more than one word.
 * */
int check_space_inside_word(char *word);

/* Get operand word, trim white spaces, and check if inside the words there is a comma. Return the trimmed word if TRUE,
 * * or NULL if error accrued.*/
char *get_operand_with_comma(char * operand);


#endif 

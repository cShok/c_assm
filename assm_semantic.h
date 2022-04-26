

#ifndef MMN14_ASSM_SEMANTIC_H
#define MMN14_ASSM_SEMANTIC_H

#include "structs.h"


/*This function gets a line, after the line_decider function determined if it is a Data/String instruction.
 * (Try to generalize the process). Using specific functions for Data and String, get the number of words to
 * decode. Return the number of arguments saved in the 'arguments' var, or NOT_FOUND (null) in case of an error.
 * The 'arguments' var return inside the numbers/ASCII characters need to be decoded.
 * */
int analyz_data(char *line, int *word_loc, int num_of_words, Flag in_label, char **arguments, Flag type);


/*This function gets a line, after the line_decider function determined it is an Entry/Extern instruction.
 * Used in two different cycle of the compiler. Generalize the main checks for a valid reference.
 * Return True if the reference is valid or NOT_FOUND in error.
 * */
int anlyz_refrenced(char *line, int *word_loc, int num_of_words, Flag in_label, Flag type, symbol_list **head);


/*This function gets a line, after the line_decider function determined it is command instruction.
 * Return the number of words to code (1-6) or NOT_FOUND if an error occurred. In the 'arguments' var, return the command
 * and the operand(s).
 * */
int anlyz_command(char *line, int *word_loc, int num_of_words, Flag in_label, char **return_args);


/* This function calculate if a specific command can have the specific address method.
 * If true, return the number of code word needed, else return NOT_FOUND.
 * */
int method_anlyz(int method,int command_type, int address_type);


/*This function is the '.data' instructions' verifier. Each number has to have a comma after,
 * number has a limit of 16-bit number, number is only digit and '+' or '-' operand.
 * Return the number of numbers to be decoded, or NOT_FOUND otherwise.
 * */
int check_data_definition(char *line);


/* This function peak at the start of a line to give an approximate analysis of which 6 valid sentences there is in the
 * assembler. Comment or Empty line, or the other four instructions sentences. Return a number for each sentence
 * or NOT_FOUND if the first two words aren't eligible. Also, return if there is label definition in the first word.
 * */
int line_decider(char *line,int * word_loc,int words_num, int *with_label);

/* This function is the main brain behind the method address analyzer. Get only word, and figure out if it is a
 * valid address method. Four kind: Immediate = 0, direct = 1, index = 2 and registry = 3.
 * Return (0-4) if the method is right or NOT_FOUND if not.
 * */
int get_address_method(char *word);

/* Small function to get the number in immediate address method.
 * Return the number*/
int get_imm_num(char *word);


/*Small function to print error coming from the syntax part (no error there).
 * */
void get_syntx_error(int num);

/* This function is decoding the string instructions. First make sure it is a valid string statement, then add
 * char by char to 'arguments'. Return number of character to code or NOT_FOUND if error accrued.
 * */
int decode_string(char *line,int num_of_words,int * word_loc,char ** arguments, Flag in_label);


/* The main decode function. gets the number of words, the type of the data and the arguments.
 * add to the code list accordingly.
 * */
void decodeCode(code_list **head_code, char **arguments, Flag data_type, int num_word_to_code, symbol_list **head_symbol);

/* Recursive function to update the labels inside command in the second pass (i.e. address method 1 and 2).
 * Use recursively to maintain the chronological updating. Handle errors (no label found). stops when first operand is NULL.
 * First operand finish and calling the second operand as the first one.
 * */
void update_label_in_method(code_list **ptr_code,symbol_list *head_symbol,generic_list **head_extern,char * first_oper, char *sec_oper);

/* Recursive function to add any extra code word after the opcode word is added.
 * Use recursively to maintain the chronological updating. No errors in this part. stops when first operand and second
 * is NULL. First operand finish and calling the second operand as the first one.
 * */
void addExtraWords(code_list **head_code,char * first_oper,char * sec_oper, int command_type, Flag type);


/*This function get a line (cut after the command) and produce the operands from two operands command line.
 * Check for comma between the operands and other syntax errors.
 * Return zero if valid, or NOT_FOUND if there is a syntax error.
 * */
int get_two_operands(char *line,char ** first_oper,char ** sec_oper);


#endif 

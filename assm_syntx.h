

#ifndef MMN14_ASSM_SYNTX_H
#define MMN14_ASSM_SYNTX_H


#define NOT_FOUND -1

/* Assembler constants*/

#define MAX_LINE 80
#define NUM_OF_COMM 16
#define MAX_LABEL 31
#define MAX_NUM_16 pow(2,16)
#define MAX_NUM_IMMID pow(2,15)
#define MAX_MEMORY 8192

/* small macro for ASCII char use in the code - syntax signs*/
#define SPECIAL_CHAR(name) *(get_special_char(name))

typedef int Flag;

/* All assm_syntx function check for the basic syntax rules of the assembler, no error handing*/

/* Function to check if the first char is ';'. Return TRUE (i.e. comment line), or FALSE else.*/
int is_comment_line(char *line);

/* Functions to check if word is valid in the assembler .
 * Return valid ( =0) if good, or other NON_ZERO number to report the syntax error.*/

    /*function for labels. type represent if it is a label definition or reference*/
    int is_label(char *word, Flag type);

    /*check if the word is combination of label definition and a valid registry.*/
    int is_index(char *word);

    /* check immidiete numbers.*/
    int is_data_num(char *num);


/* Functions to get the basic saved words and constants.*/

    /*check if the word is a saved word in the assembler. Return TRUE or FALSE*/
    int is_saved_word(char *word);

    /*check if the specific command has the specific method, using the const array "method".
     * return TRUE if found or FALSE if not exited.*/
    int has_the_address_method(int command_type,int method, Flag type_address);

    /*according to the method determent how many code words needed. Return between 0-2 if TRUE, or NOT_FOUND
     * if method type isn't recognize*/
    int get_num_of_code_words(int method);

    /* Return the opcode of the command using the command number*/
    int get_opcode(int command_type);

    /* Return the funct filed of the command using the command number*/
    int get_funct(int command_type);

    /* Travers over the command names. Return the command number (0 -15) or NOT_FOUND if not a command name.*/
    int get_command(char * word);

    /* Travers over the instructions names. Return the enum number (0 -3) or NOT_FOUND if not an instruction name.*/
    int get_instruct(char * word);

    /* Travers over the registry names 'r(0-15)'. Return the number (0 -3) or NOT_FOUND if not an instruction name.*/
    int get_reg(char * word);

    /* split a word around the square brackets, and get the word inside */
    int get_reg_fom_indx(char *operand);

    /* Gets the command number return the number of operands needed in the command (0 -2).*/
    int get_command_num_of_operand(int command_type);

 /* small utlty function to traverse the const array of the assm_syntax. Gets the word to search, and the
  * length of the array. Return the number of the word in the const array, or NOT_FOUND.*/
int traverse_list(char **list, char *word, int num);

/* Using the const array specialChar to check for the special syntax signs inside the code. Return a pointer
   * to the specific char needed*/
char * get_special_char(int type);


enum boolean {FALSE, TRUE};

enum words_in_line {FIRST , SECOND, THIRD, FORTH};

enum file_types {ASSM, MACRO, OBJ, ENT, EXT};

enum types {INT, CHAR};

    /*label referenced or label definition, used by is_label*/
enum labels {MIDDLE, OPEN};

    /*used by the get_special_char*/
enum spec_char {COMMA_C, SEMI_COLON, COLON, HASHTAG, LEFT_BRACK, CLOSE_BRACK, QUOTATION, PLUS, MINUS};

enum instructions { ENTRY , EXTERN, DATA, STRING, COMMAND, EMPTY, COMMENT};

    /* No error hanlding inside assm-syntx, need to cascade to semantic*/
enum address_method_errors {VALID , OPEN_LABEL_WITHOUT_COLON , LABEL_MUST_OPEN_IN_ALPHA,
            LABEL_ONLY_ALPAHNUM, LABEL_TO_LONG, LABEL_IS_SAVED_WORD,
            IMMI_MUST_OPEN_HASHTAG, IMMI_ONLY_HASHTAG, IMMI_ONLY_OPERAND, IMMI_NON_DIGIT_CHAR, IMMI_NUM_TO_BIG,
            NO_OPEN_BRACKETS, NO_CLOSE_BRACKETS, INDX_NO_REG};

    /* code list node in firs_pass either data or instruction*/
enum bit_type {DATA_CODE, OPER_CODE};

    /* The four main address method, and two who used later*/
enum address_method {IMMID_ADDRESS, DIRECT_ADDRESS, INDEX_ADDRESS, REG_ADDRESS, OFFSET, FUNCT};

    /* disgusting between source method and destention method, used to fine in the const array 'method'.*/
enum type_address{SRC =0, DES =4};

    /*type of commands*/
enum num_of_oper {TWO_OPERAN_COMM ,ONE_OPERND_COMM = 5,ZERO_OPERND_COMM=14};

#endif 

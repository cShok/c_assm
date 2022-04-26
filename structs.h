/*Chanany Shok*/

#ifndef MMN14_STRUCTS_H
#define MMN14_STRUCTS_H

#include "assm_syntx.h"

/* octal bit-filed to calculate the 2's complements for the immediate address method*/
typedef struct sixteen{
    unsigned int mis : 16;
}octal_bit;

/* generic list node*/
typedef struct List{
    char *name;
    char *content;
    int address;
    struct List *next;

}generic_list;

/* symbol list node*/

typedef struct Symbol{
    char *name;
    int address;
    int base;
    int offSet;
    Flag code;
    Flag entry;
    Flag extrn;
    Flag data;
    struct Symbol *next;
}symbol_list;

/* Four kinds of bit-filed, each represent 20 bit word, with other way to assign the data*/
/* The main bit-filed to store almost most of the word type except the funct word*/
typedef struct OperandBit{
    unsigned int opcode : 16;
    unsigned int E : 1;
    unsigned int R : 1;
    unsigned int A : 1;
    unsigned int empty : 1;

}oper_bit;

/* Funct bit-filed to assign the address method(s), the registries, and the funct code*/
typedef struct DataBit{
    unsigned int desMethod :2;
    unsigned int desReg : 4;
    unsigned int srcMethod :2;
    unsigned int srcReg : 4;
    unsigned int funct : 4;
    unsigned int E : 1;
    unsigned int R : 1;
    unsigned int A : 1;
    unsigned int empty : 1;
}funct_bit;

/* to initilize and assign the entire word, using trev bit-Filed*/
typedef struct TreverseBit{
    unsigned int T : 20;
}trev_bit;

/* Utility bit-filed to access the 4 bit pattern needed in the obj files*/
typedef struct FinalBit{

    unsigned int E : 4;
    unsigned int D : 4;
    unsigned int C : 4;
    unsigned int B : 4;
    unsigned int A : 4;
}final_bit;

/* The main struct used to store the code list, has the union BitFiled to maximums flexibility.
 * In addition, the address  and the data type flag */

typedef struct Code{
     union {
         oper_bit operand;
         funct_bit data;
         trev_bit iter;
         final_bit fb;
     }BitFiled;
    int address;
    Flag data_type;
    struct Code *next;
}code_list;


/*This function add new node to the generic list, add to the end of the list.
 * Allocated memory for also the name of the node and the content.*/
void add_to_list(generic_list **head, char *name, char * content, int address);

/* Function to travers over a generic list and find a node by the name.
 * Return pointer to the node found or NULL if not*/
generic_list * find_in_list(generic_list *head, char *name);

/* Free allocated memory for the generic list. also free the name and content*/
void free_list(generic_list *head);



/*This function add new node to the code list, add to the end of the list*/
void add_to_code_list(code_list **head , code_list *bitFiled);

/* Utility function to update the data type nodes in the code list.First rearrange the head (if it is data type),
 * then continue pushing the data type to the end of the list. Return te new head*/
code_list * rearrange_code(code_list *head);

/* Free the allocated symbol list, add NULL to each free.*/
void free_code_list(code_list *head);


/* Utility function to update the addresses of the label which belong to data type. Travers the list and
 * adding the gIc, to each of node under data type flag*/
void update_symb_list(symbol_list *head);

/* This function add new node to the symbol list, add to the end of the list*/
void add_to_symb_list(symbol_list **head, char *name, int address, Flag type);

/* Free the allocated symbol list, add NULL to each free, include the symbol names*/
void  free_symb_list(symbol_list *head);

/* Function to travers over the symbol list and find a node by the name.
 * Return pointer to the node found or NULL if not*/
symbol_list * find_in_symb_list(symbol_list **head, char *name);

#endif 

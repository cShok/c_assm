

#ifndef MMN14_second_pass_H
#define MMN14_second_pass_H
#include "first_pass.h"
#include "structs.h"
#include <stdio.h>

/*Main function to end the parsing. Get the file descriptor, and the symbol table and the code table.
 * Only decode entry instruction and commands with labels referenced in the operands (i.e. address method 1 and 2).
 * Return non-Zero if failed, or zero if succeeded.*/
int second_pass(FILE *fd, char *fileName);

/* Function to generate the files of the compiler. Entry referenced, Extern referenced and the byte-code
* in specific pattern. If there is no entry references or extern, not open the files.*/
void generate_compiler_files(generic_list **head_extern,char * fn);

/* Small function to search the symbol table for an entry node. Return true if finds entry entry, or
 * false if failed*/
int hasEntry();

#endif 

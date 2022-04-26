

#ifndef MMN14_first_pass_H
#define MMN14_first_pass_H

#include <stdio.h>

/*Main function to start the parsing. Get the file descriptor, parse it, and generate two struct,
 * the code table and the symbol table. Return non-zero in case of an error, or zero if succeeded.*/
int first_pass(FILE *fd);

#endif 

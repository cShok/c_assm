/*chanany Shok*/

#include "error_handler.h"
#include "parser.h"

/* gErr is the main global var of the error_handler*/
extern int gErr;

void print_error(char *errMassage, ...) {
	 va_list args;
    gErr++;
    /*initialize the stdarg macros*/

    /* Print generic label for error, then use the vprintf to have flexibility with the error messages
     * and the number of arguments possible.*/
    printf("Error: In file %s Line %d: ", generate_files_suffix(getName(), getState()), getLineNum());

    va_start(args, errMassage); /*Here is the message from the parsing unit*/
    vprintf(errMassage, args);
    puts("");
}

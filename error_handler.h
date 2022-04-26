
#ifndef MMN14_ERROR_HANDLER_H
#define MMN14_ERROR_HANDLER_H
#include <stdarg.h>
#include <stdio.h>
#include "assm.h"
#include "first_pass.h"

/* Flexible function with changing number of arguments, to handle the error messages. using the stdarg library, and
 * the vprintf build function*/
void print_error(char *message, ...);


#endif 

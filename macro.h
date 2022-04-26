/*Chanany Shok*/

#ifndef MMN14_MACRO_H
#define MMN14_MACRO_H

#include <stdio.h>
#include <stdlib.h>

/*Main function to unfold the macros. Get the file descriptor, parse it, and generate new post-fold files,
 * for the next cycle. Return non-zero in case of an error, or zero if succeeded.*/
int macro_anlz(FILE *, char * fileName);

#endif 

/* Chanany Shok*/

#ifndef MMN14_ASSM_H
#define MMN14_ASSM_H

/*three small function to get the current gErr, file name or file state*/

/* return the file name (no suffixes).*/

char * getName();

/*return the current line number*/

int getLineNum();

/* return the state of the processing, to distinguished between errors in different files (after macro unfold,
 * the lines' numeration may change)*/

int getState();

#endif 

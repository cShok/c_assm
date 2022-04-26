#include <stdio.h>
#include "macro.h"
#include "assm_syntx.h"
#include "parser.h"
#include "first_pass.h"
#include "second_pass.h"
#include "error_handler.h"


/* Those are the global vars to determine the main flow of the assembler.*/
int gIc; /*instruction counter*/
int gDc; /*data counter*/

/*error global vars, used as the lock from macro to first pass and then the second pass.*/
int gErr; /*counter*/
int gLineNum; /*current line invoking the error*/
char *gFileName; /*current file name*/
Flag gState; /*current position to know in which file we have the error (*.as or *.am).*/

/*
 * Entry to the assembler program. Constant and basic assembler syntax is described in the assm_syntx.
 * 20_bit words with 8192 memory slots, using the command-line arguments to process '*.as' files. */
int main(int argc, char *argv[]) {


    FILE *fd;
    int i;
    /* check for arguments, i.e. files*/
    if (argc < 2) {
        printf("No file entered in the command line\n");
        return NOT_FOUND;
    }

    for (i = 1; i < argc; i++) {

        /*initialize the global vars*/
        gIc = 100;
        gDc = 0;
        gErr = 0;
        gFileName = argv[i];

        fd = open_file(gFileName, "r", ASSM);
        if (fd == NULL) {
            printf("couldn't open file \"%s\'", gFileName);
            continue;
        }

        /* Macro expending, assumed each macro definition ends, and referenced macro comes after the definition.
         * If length of a line is over MAX_LINE constant, do consider as an error.*/
        if (macro_anlz(fd, gFileName)) {
            break;
        }
        fclose(fd);
        /*open the '*.am' file, i.e. the unfold version of the code.*/
        fd = open_file(gFileName, "r", MACRO);

        /*first pass is the main cycle of parsing the file. The idea is to get line by line, and to build two main
         * data structures- one for the decoded lines, and the other to store the referenced symbols in the code.
         *  skips empty lines, comment lines (start with ';') and the entry instruction definition.*/

        if (!first_pass(fd)) {

            /*if succeeded, continue to the second pass. which is mainly to update the decoded lines,
             * with the information about refrenced symbols, and to build entry for symbol defined in the program
             * generate the compiler files '*.ob', '*.ext'  and '*.ent', if there are zero errors*/
            rewind(fd);

            second_pass(fd, gFileName);

            /*printf("File %s compiled successfully\n", gFileName);*/

        }
        fclose(fd);
        gFileName = NULL;
        gErr = 0;
    }
    return 0;
}

char *getName() {
    return gFileName;
}

int getLineNum() {
    return gLineNum;
}

int getState() {
    return gState;
}

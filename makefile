assembler: clean assm.o assm_syntx.o first_pass.o second_pass.o assm_semantic.o parser.o error_handler.o structs.o macro.o
	gcc -g -ansi -Wall -pedantic assm.o structs.o assm_semantic.o assm_syntx.o error_handler.o first_pass.o second_pass.o macro.o parser.o -lm -o assembler

assm.o: assm.c macro.h assm.h parser.h structs.h error_handler.h first_pass.h second_pass.h assm_syntx.h
	gcc -g -c -ansi -Wall -pedantic  assm.c -o assm.o

structs.o: structs.c structs.h error_handler.h
	gcc -g -c -ansi -Wall -pedantic  structs.c -o structs.o

assm_semantic.o: assm_semantic.c parser.h assm_syntx.h assm_semantic.h error_handler.h
	gcc -g -c -ansi -Wall -pedantic  assm_semantic.c -o assm_semantic.o -lm
	
assm_syntx.o: assm_syntx.c assm_syntx.h
	gcc -g -c -ansi -Wall -pedantic  assm_syntx.c -o assm_syntx.o -lm
	
error_handler.o: error_handler.c error_handler.h parser.h
	gcc -g -c -ansi -Wall -pedantic  error_handler.c -o error_handler.o 

first_pass.o: first_pass.c parser.h error_handler.h first_pass.h assm_semantic.h
	gcc -g -c -ansi -Wall -pedantic  first_pass.c -o first_pass.o
	
second_pass.o: second_pass.c second_pass.h parser.h  assm_semantic.h
	gcc -g -c -ansi -Wall -pedantic  second_pass.c -o second_pass.o

macro.o: macro.c macro.h assm_syntx.h structs.h parser.h error_handler.h
	gcc -g -c -ansi -Wall -pedantic  macro.c -o macro.o
	
parser.o: parser.c parser.h assm_syntx.h error_handler.h
	gcc -g -c -ansi -Wall -pedantic  parser.c -o parser.o

clean:
	-rm -f *.o 


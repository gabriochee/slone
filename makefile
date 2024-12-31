# --------------------------
#  	    VARIABLES
# --------------------------

CFLAGS  = -Wall -Werror

# --------------------------
#  	  COMPILATION
# --------------------------

# Il faut alléger ce Makefile mon vier ! TODO : utiliser les paramètres des commandes make

all: main.o lexer.o lexer_test.o parser.o parser_test.o
	gcc $(CFLAGS) main.o lexer.o lexer_test.o parser.o parser_test.o -o interpreter

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

lexer.o: sources/lexer.c
	gcc $(CFLAGS) -c sources/lexer.c -o lexer.o

lexer_test.o: sources/lexer_test.c
	gcc $(CFLAGS) -c sources/lexer_test.c -o lexer_test.o

parser.o: sources/parser.c
	gcc $(CFLAGS) -c sources/parser.c -o parser.o

parser_test.o: sources/parser_test.c
	gcc $(CFLAGS) -c sources/parser_test.c -o parser_test.o

clean:
	rm *.o

mrproper: clean
	rm interpreter


# --------------------------
#  	    VARIABLES
# --------------------------

CC		= gcc
CFLAGS  = -Wall -Werror -g -lm

# --------------------------
#  	  COMPILATION
# --------------------------

# Il faut alléger ce Makefile mon vier ! TODO : utiliser les paramètres des commandes make

all: main.o lexer.o lexer_test.o parser.o parser_test.o error.o interpreter.o
	$(CC) $(CFLAGS) main.o lexer.o lexer_test.o parser.o parser_test.o error.o interpreter.o -o interpreter

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

lexer.o: sources/lexer.c
	$(CC) $(CFLAGS) -c sources/lexer.c -o lexer.o

lexer_test.o: sources/lexer_test.c
	$(CC) $(CFLAGS) -c sources/lexer_test.c -o lexer_test.o

parser.o: sources/parser.c
	$(CC) $(CFLAGS) -c sources/parser.c -o parser.o

parser_test.o: sources/parser_test.c
	$(CC) $(CFLAGS) -c sources/parser_test.c -o parser_test.o

error.o: sources/error.c
	$(CC) $(CFLAGS) -c sources/error.c -o error.o

interpreter.o: sources/interpreter.c
	$(CC) $(CFLAGS) -c sources/interpreter.c -o interpreter.o

clean:
	rm *.o

mrproper: clean
	rm interpreter


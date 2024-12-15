# --------------------------
#  	    VARIABLES
# --------------------------

CFLAGS  = -Wall -Werror

# --------------------------
#  	  COMPILATION
# --------------------------

all: main.o lexer.o
	gcc $(CFLAGS) main.o lexer.o -o interpreter

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

lexer.o: sources/lexer.c
	gcc $(CFLAGS) -c sources/lexer.c -o lexer.o

clean:
	rm *.o
# --------------------------
#  	    VARS	
# --------------------------

CFLAGS = -Wall -Werror
C      = gcc
EXEC   = interpreter
#
# --------------------------
#  	  COMPILE	
# --------------------------

all: $(EXEC)

$(EXEC): main.o
	$(C) -o $(EXEC) main.o $(CFLAGS)

main.o :
	$(C) -o main.o -c main.c $(CFLAGS)

clean: 
	rm *.o

mrproper: clean
	rm $(EXEc)

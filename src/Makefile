CC = ${CROSS_PREFIX}gcc

all : clean
	$(CC) -Wall -o calc calc.c -lm

.PHONY : all clean

clean : 
	-del calc calc.exe
	-rm calc calc.exe

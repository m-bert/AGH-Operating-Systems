CC = gcc
CFLAGS = -Wall

all: reverse.o
	ar rcs libreverse.a reverse.o
	${CC} ${CFLAGS} main.c -o main -L. -lreverse


reverse.o: reverse.c
	${CC} ${CFLAGS} reverse.c -c 

clean:
	rm -rf *o *a main
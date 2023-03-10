CC = gcc
CFLAGS = -Wall

all: reverse.o
	ar rcs libreverse.a reverse.o
	${CC} ${CFLAGS} main.c -o main -L. -lreverse

myTests:
	@make -s clean
	@make -s all
	./main lipsum.txt
	@make -s clean


reverse.o: reverse.c
	${CC} ${CFLAGS} reverse.c -c 

clean:
	rm -rf *o *a main
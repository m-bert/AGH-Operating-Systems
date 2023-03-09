CC = gcc
CFLAGS = -Wall

all: replace.o
	ar rcs libreplace.a replace.o
	${CC} ${CFLAGS} main.c -o main -L. -lreplace


replace.o: replace.c
	${CC} ${CFLAGS} replace.c -c 

clean:
	rm -rf *o *a res.txt
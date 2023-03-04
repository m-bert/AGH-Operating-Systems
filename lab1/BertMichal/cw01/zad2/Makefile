CC = gcc
CFLAGS = -Wall

main: main.o
	${CC} ${CFLAGS} -o main main.o

compileStatic: main.o lib.a
	${CC} ${CFLAGS} main.o lib.a -o main

main.o: main.c
	${CC} ${CFLAGS} -c main.c

# Static library
lib.a: lib.c
	${CC} ${CFLAGS} -c lib.c
	ar rcs lib.a lib.o

# Shared library
lib.so: lib.c
	${CC} ${CFLAGS} -fPIC -c lib.c
	${CC} ${CFLAGS} -shared -o lib.so lib.o

clean:
	rm -rf *o *a
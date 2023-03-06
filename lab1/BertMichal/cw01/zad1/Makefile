CC = gcc
CFLAGS = -Wall

all:
	make compileStatic
	make compileShared

lib.o: lib.c
	${CC} ${CFLAGS} -c lib.c

compileStatic: lib.c
	${CC} ${CFLAGS} -c lib.c
	ar rcs lib.a lib.o

compileShared: lib.c
	${CC} ${CFLAGS} -fPIC -c lib.c
	${CC} ${CFLAGS} -shared -o lib.so lib.o

clean:
	rm -rf *o *a

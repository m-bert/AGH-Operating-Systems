CC = gcc
CFLAGS = -Wall

compileStatic: lib.o
	ar rcs liblib.a lib.o
	${CC} ${CFLAGS} main.c -o main -L. -llib

compileShared: lib.o
	${CC} ${CFLAGS} -shared -fPIC -o liblib.so lib.o
	${CC} ${CFLAGS} main.c -o main -L. -llib -Wl,-rpath=`pwd`

compileDLL: lib.o
	${CC} ${CFLAGS} -shared -fPIC -o liblib.so lib.o
	${CC} ${CFLAGS} main.c -o main -L. -ldl -D DLL

lib.o: lib.c
	${CC} ${CFLAGS} -c lib.c

clean:
	rm -rf *o *a
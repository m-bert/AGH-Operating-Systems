CC = gcc
CFLAGS = -Wall

all:
	make compileStatic
	make compileShared
	make compileDLL

compileStatic: lib.o
	ar rcs liblib.a lib.o
	${CC} ${CFLAGS} main.c -o main_static -L. -llib

compileShared: lib.o
	${CC} ${CFLAGS} -shared -fPIC -o liblib.so lib.o
	${CC} ${CFLAGS} main.c -o main_shared -L. -llib -Wl,-rpath=`pwd`

compileDLL: lib.o
	${CC} ${CFLAGS} -shared -fPIC -o liblib.so lib.o
	${CC} ${CFLAGS} main.c -o main_DLL -L. -ldl -D DLL

lib.o: lib.c
	${CC} ${CFLAGS} -c lib.c

clean:
	rm -rf *o *a
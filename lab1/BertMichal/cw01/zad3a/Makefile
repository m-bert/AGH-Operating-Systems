CC = gcc
CFLAGS = -Wall

all:
	make clean
	make compileStatic
	cat test | ./main >> report_static.txt

	make clean
	make compileShared
	cat test | ./main >> report_shared.txt
	
	make clean
	make compileDLL
	cat test | ./main >> report_DLL.txt

compileStatic: lib.o
	ar rcs liblib.a lib.o
	${CC} ${CFLAGS} ${OPT} main.c -o main -L. -llib

compileShared: lib.o
	${CC} ${CFLAGS} -shared -fPIC -o liblib.so lib.o
	${CC} ${CFLAGS} main.c -o main -L. -llib -Wl,-rpath=`pwd`

compileDLL: lib.o
	${CC} ${CFLAGS} -shared -fPIC -o liblib.so lib.o
	${CC} ${CFLAGS} main.c -o main -L. -ldl -D DLL

lib.o: lib.c
	${CC} ${CFLAGS} -c lib.c

testStatic:
	make clean
	make compileStatic

	cat test | ./main >> report_static.txt

testShared:
	make clean
	make compileShared

	cat test | ./main >> report_shared.txt

testDLL:
	make clean
	make compileDLL

	cat test | ./main >> report_DLL.txt

clean:
	rm -rf *o *a
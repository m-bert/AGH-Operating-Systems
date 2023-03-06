CC = gcc
CFLAGS = -Wall

TYPE = Static
OPT = -O0

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

	cat test | ./main >> report_static_${OPT}.txt

testShared:
	make clean
	make compileShared

	cat test | ./main >> report_shared_${OPT}.txt

testDLL:
	make clean
	make compileDLL

	cat test | ./main >> report_DLL_${OPT}.txt

performTesting:
	make clean
	make compileStatic
	cat test | ./main >> report_static_O0.txt

	make clean
	make compileStatic OPT="-O1"
	cat test | ./main >> report_static_O1.txt
	
	make clean
	make compileStatic OPT="-Os"
	cat test | ./main >> report_static_Os.txt

	make clean
	make compileShared
	cat test | ./main >> report_shared_O0.txt

	make clean
	make compileShared OPT="-O1"
	cat test | ./main >> report_shared_O1.txt
	
	make clean
	make compileShared OPT="-Os"
	cat test | ./main >> report_shared_Os.txt

	make clean
	make compileDLL
	cat test | ./main >> report_DLL_O0.txt

	make clean
	make compileDLL OPT="-O1"
	cat test | ./main >> report_DLL_O1.txt
	
	make clean
	make compileDLL OPT="-Os"
	cat test | ./main >> report_DLL_Os.txt

clean:
	rm -rf *o *a
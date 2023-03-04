CC = gcc
CFLAGS = -Wall

testmake:
	${CC} -c lib.c test.c
	${CC} -o test lib.o test.o

cleartmp:
	rm -rf /tmp/count*

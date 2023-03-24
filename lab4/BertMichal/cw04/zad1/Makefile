CC = gcc
CFLAGS = -Wall -g -fsanitize=address

all:
	@${CC} ${CFLAGS} exec_test.c -o exec_test
	@${CC} ${CFLAGS} main.c -o main

clean:
	rm -rf main exec_test
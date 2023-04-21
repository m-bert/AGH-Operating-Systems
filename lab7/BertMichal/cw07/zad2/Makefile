CC = gcc
CFLAGS = -Wall -g

all:
	@make -s clean
	@${CC} ${CFLAGS} main.c -pthread -lrt -o main

clean:
	@rm -rf main
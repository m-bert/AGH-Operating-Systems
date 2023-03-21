CC = gcc
CFLAGS = -Wall -g

DIR = "."

all:
	@make -s clean
	@${CC} ${CFLAGS} main.c -o main

	@./main ${DIR}

	@make -s clean

clean:
	@rm -rf main

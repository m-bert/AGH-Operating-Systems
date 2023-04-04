CC = gcc
CFLAGS = -Wall -g -fsanitize=address

all:
	@make -s clean
	@${CC} ${CFLAGS} main.c -o main

clean:
	@rm -rf main
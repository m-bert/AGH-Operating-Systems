CC = gcc
CFLAGS = -Wall -g

COUNT = 1

all:
	@make -s clean
	@${CC} ${CFLAGS} main.c -o main

	@./main ${COUNT}
	
	@make -s clean

clean:
	@rm -rf main
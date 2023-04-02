CC = gcc
CFLAGS = -Wall -g -fsanitize=address

all:
	@make -s clean
	@${CC} ${CFLAGS} main.c -o main
	@${CC} ${CFLAGS} fragment_calculator.c -o fragment_calculator

clean:
	@rm -rf main fragment_calculator tmp_pipe 
CC = gcc
CFLAGS = -Wall -g

all:
	@make -s clean
	@${CC} ${CFLAGS} main.c -pthread -lrt -o main

test:
	@make -s
	@echo "Testing with parameters: 3 hairdressers, 2 chairs, 3 waiting room seats"
	@./main 3 2 3

clean:
	@rm -rf main
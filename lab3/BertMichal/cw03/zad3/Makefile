CC = gcc
CFLAGS = -Wall -g

all:
	@${CC} ${CFLAGS} main.c -o main

test_current:
	@make -s clean
	@make -s all

	@./main . abc

	@make -s clean

test_parent:
	@make -s clean
	@make -s all

	@./main .. abc

	@make -s clean

test_grandparent:
	@make -s clean
	@make -s all

	@./main ../.. abc

	@make -s clean

test_home:
	@make -s clean
	@make -s all

	@./main ~/ abc

	@make -s clean

test_root:
	@make -s clean
	@make -s all

	@./main /root abc

	@make -s clean

clean:
	@rm -rf main
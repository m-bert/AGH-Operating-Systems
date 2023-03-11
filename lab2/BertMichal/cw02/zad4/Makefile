CC = gcc
CFLAGS = -Wall


all:
	@${CC} ${CFLAGS} main.c -o main 

run_tests:
	@make -s clean
	@make -s all

	@./main .
	@find . | xargs wc --bytes | grep "total"

	@make -s clean


clean:
	rm -rf *o  main 
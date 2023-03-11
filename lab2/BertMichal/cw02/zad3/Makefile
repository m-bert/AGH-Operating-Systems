CC = gcc
CFLAGS = -Wall


all:
	@${CC} ${CFLAGS} main.c -o main 

run_tests:
	@make -s clean
	@make -s all

	@./main
	@wc --bytes * | grep otal

	@make -s clean


clean:
	rm -rf *o  main res*
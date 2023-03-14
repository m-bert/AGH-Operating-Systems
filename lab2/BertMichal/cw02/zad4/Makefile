CC = gcc
CFLAGS = -Wall
DIR = .

all:
	@${CC} ${CFLAGS} main.c -o main 

run_tests:
	@make -s clean
	@make -s all

	@./main ${DIR}
	@find ${DIR} | xargs wc --bytes | grep "total"

	@make -s clean


clean:
	rm -rf *o  main 
CC = gcc
CFLAGS = -Wall -g -fsanitize=address

all:
	@make -s clean
	@${CC} ${CFLAGS} main.c -o main

report:
	@make -s
	@./main 0.000000001 1 >> report.txt
	@./main 0.000000001 2 >> report.txt
	@./main 0.000000001 5 >> report.txt
	@./main 0.000000001 10 >> report.txt
	@./main 0.000000001 100 >> report.txt
	@ rm -rf main

clean:
	@rm -rf main report.txt
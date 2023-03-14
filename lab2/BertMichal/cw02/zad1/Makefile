CC = gcc
CFLAGS = -Wall

all: replace.o
	ar rcs libreplace.a replace.o
	${CC} ${CFLAGS} main.c -o main -L. -lreplace


replace.o: replace.c
	${CC} ${CFLAGS} replace.c -c 

clean:
	rm -rf *o *a main

lipsum_test:
	@make -s clean
	@make -s all

	./main l r ./lipsum.txt ./res_main.txt
	tr l r < ./lipsum.txt > ./res_tr.txt
	diff ./res_main.txt ./res_tr.txt
	@echo "This line should be visible right after diff command!"

	@make -s clean
	@rm -rf ./res_main.txt ./res_tr.txt

make worst_case_test:
	@make -s clean
	@make -s all

	./main a b ./worst_case_test.txt ./res_main.txt
	tr a b < ./worst_case_test.txt > ./res_tr.txt
	diff ./res_main.txt ./res_tr.txt
	@echo "This line should be visible right after diff command!"

	@make -s clean
	@rm -rf ./res_main.txt ./res_tr.txt
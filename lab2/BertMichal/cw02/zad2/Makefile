CC = gcc
CFLAGS = -Wall
BLOCKS = 1

all: reverse.o
	ar rcs libreverse.a reverse.o
	${CC} ${CFLAGS} main.c -o main -L. -lreverse

selfTest:
	@make -s clean
	@make -s all

	./main lipsum.txt ${BLOCKS}
	./main rev_lipsum.txt ${BLOCKS}
	diff rev_rev_lipsum.txt lipsum.txt

	@echo "This should be the the first line after diff"
	@make -s clean

sysTest:
	@make -s clean
	@make -s all

	./main lipsum.txt ${BLOCKS}
	tac lipsum.txt | rev > sys_lipsum.txt
	diff rev_lipsum.txt sys_lipsum.txt

	@make -s clean

reverse.o: reverse.c
	${CC} ${CFLAGS} reverse.c -c 

clean:
	rm -rf *o *a main rev_* ./sys_*
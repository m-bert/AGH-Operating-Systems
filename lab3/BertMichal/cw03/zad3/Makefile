CC = gcc
CFLAGS = -Wall -g
PAT = "abc"

all:
	@${CC} ${CFLAGS} main.c -o main

test_current:
	@make -s create_test_structure
	@make -s all

	@./main . ${PAT}

	@make -s clean

test_parent:
	@@make -s create_test_structure
	@make -s all

	@./main .. ${PAT}

	@make -s clean

test_grandparent:
	@make -s create_test_structure
	@make -s all

	@./main ../.. ${PAT}

	@make -s clean

test_home:
	@make -s create_test_structure
	@make -s all

	@./main ~/ ${PAT}

	@make -s clean

test_root:
	@make -s all

	@./main /root ${PAT}

	@make -s clean

create_test_structure:
	@echo "abcdef" >> valid.txt
	@echo "ghijkl" >> invalid.txt

	@mkdir -p ./test ./test/test2 ./test/test3
	@echo "./test ./test/test2 ./test/test3" | xargs -n 1 cp -v valid.txt
	@echo "./test ./test/test2 ./test/test3" | xargs -n 1 cp -v invalid.txt
	@echo "================================================================"
	@echo "Program output:"
	@echo "================================================================"

clean:
	@rm -rf main valid.txt invalid.txt ./test
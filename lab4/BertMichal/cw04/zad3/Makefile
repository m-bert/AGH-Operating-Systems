CC = gcc
CFLAGS = -Wall -g -fsanitize=address

all:
	@${CC} ${CFLAGS} catcher.c -o catcher
	@${CC} ${CFLAGS} sender.c -o sender
	./catcher&

clean:
	rm -rf sender catcher
CC = gcc
CFLAGS = -Wall -g -fsanitize=address

all:
	@${CC} ${CFLAGS} server.c -o server
	@${CC} ${CFLAGS} client.c -o client

clean:
	@rm -rf server client
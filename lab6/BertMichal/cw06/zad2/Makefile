CC = gcc
CFLAGS = -Wall -g -fsanitize=address

all:
	@${CC} ${CFLAGS} server.c server
	@${CC} ${CFLAGS} client.c client

clean:
	rm -rf server client
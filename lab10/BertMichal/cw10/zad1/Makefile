CC = gcc
CFLAGS = -Wall

all:
	@make -s clean
	@${CC} ${CFLAGS} client.c -o client
	@${CC} ${CFLAGS} server.c -o server

clean:
	@rm -rf client server
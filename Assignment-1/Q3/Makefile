CC=gcc

all: client server

client: client.c
server: server.c
	$(CC) $? -o $@ -lpthread

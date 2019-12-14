all: client server

client: client.c client.h parser.c parser.h
	gcc -g -o client client.c client.h parser.c

server: server.c
	gcc -pthread -g -o server server.c
all: client server proxy

client: client.c client.h parser.c parser.h
	gcc -g -o client client.c client.h parser.c

server: server.c server.h
	gcc -pthread -g -o server server.c server.h

proxy: proxy.c proxy.h
	gcc -pthread -g -o proxy proxy.c proxy.h

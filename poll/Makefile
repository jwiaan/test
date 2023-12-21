.PHONY: all clean
all: server client

server: server.c
	cc -pthread -Wall server.c -oserver -g3

client: client.c
	cc -pthread -Wall client.c -oclient

clean:
	rm -f server client

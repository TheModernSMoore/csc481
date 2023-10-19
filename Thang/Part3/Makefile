all: server client

server:
	g++ server.cpp -o server -lzmq

client:
	g++ client.cpp -o client -lzmq

clean:
	rm -f server client
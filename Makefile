CXXFLAGS=-O2 -g -Wall -Wextra -pedantic -std=c++11
LDFLAGS=-Wl,-rpath=/usr/local/lib/gcc49/
CLIENT=client.cpp client.h
SERVER=server.cpp server.h
all: client server

client: $(CLIENT)

server: $(SERVER)

clean:
	rm -f client server

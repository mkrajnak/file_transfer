CXXFLAGS=-O2 -g -Wall -Wextra -pedantic -std=c++11
LDFLAGS=-Wl,-rpath=/usr/local/lib/gcc49/
all: client server

client: client.cpp

server: server.cpp

clean:
	rm -f client server

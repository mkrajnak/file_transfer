#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void help();
void handle_communication(int server_socket);
void init_server(int port);
void serve(int client_socket);
int get_new_client(int server_socket);
int fork_handler();
void listen_wrapper(int server_socket);
int get_socket();

#endif

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
using namespace std;


/*
* HELP
*/
void help()
{
  printf("Simple File Server:\n\tUSAGE: server -p <port_number>\n" );
}
/*
*   Handles communication between server and client
*/
void handle_communication(int server_socket)
{
  if ((listen(server_socket, 1)) < 0) // listen for incomming connection
  {
    perror("ERROR: listen");
    exit(EXIT_FAILURE);
  }

  while(1)
  {
    struct sockaddr_in client;              //new client address
    socklen_t client_len = sizeof(client);  //size
		int comm_socket = 0;
		if((comm_socket = accept(server_socket, (struct sockaddr*)&client, &client_len)) > 0)
		{
			printf("INFO: New connection:\n");
      char buff[1024];
			int res = 0;
      while((res = recv(comm_socket, buff, 1024,0)) > 0){   //handle data
				send(comm_socket, buff, strlen(buff), 0);
			}
		}
    close(comm_socket);
    printf("Connection closed\n");
	}
}

/**
* INITIOALIZING connection
*/
void init_server(int port)
{

  int server_socket = 0;
  if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)  // creating socket
  {
    perror("ERR: socket");
    exit(EXIT_FAILURE);
  }

  // prepare address for bind
  struct sockaddr_in sw;            //setting up server address
  memset(&sw, 0, sizeof(sw));       // setting up struct for connect
  sw.sin_family = AF_INET;
  sw.sin_addr.s_addr = INADDR_ANY; // setting properly destination ip address
  sw.sin_port = htons(port);              // set destination port
  //bind network socket to socket file descriptor

  int connection = 0;
  if((connection = bind (server_socket, (struct sockaddr *)&sw, sizeof(struct sockaddr))) < 0 )
  {
    perror("ERR: socket");
    exit(EXIT_FAILURE);
  }
  cout<< "Hello server" <<endl;
  handle_communication(server_socket);
}
/*
* MAIN
*/
int main(int argc, char const *argv[])
{
  if (argc == 2  && (strcmp(argv[1],"--help") == 0))  {
    help();
    exit(EXIT_SUCCESS);
  }
  if(argc != 3 || (strcmp(argv[1],"-p") != 0))
  {
    fprintf(stderr, "Invalid args\n" );
    help();
    exit(EXIT_FAILURE);
  }

  init_server((int)strtod(argv[2],NULL));
  return 0;
}

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


/**
* INITIOALIZING connection
*/
void init_connection()
{
  int port = 6666;
  int connection;
  int server_socket;
  // create socket
  if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // creating socket
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
  if((connection = bind (server_socket, (struct sockaddr *)&sw, sizeof(struct sockaddr))) < 0 )
  {
    perror("ERR: socket");
    exit(EXIT_FAILURE);
  }
  // listen for incomming connection
  if ((listen(server_socket, 1)) < 0)
	{
		perror("ERROR: listen");
		exit(EXIT_FAILURE);
	}
  while(1)
	{
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
		int comm_socket = accept(server_socket, (struct sockaddr*)&client, &client_len);
		if (comm_socket > 0)
		{
			printf("INFO: New connection:\n");

			char buff[1024];
			int res = 0;
			for (;;)
			{
				res = recv(comm_socket, buff, 1024,0);
                if (res <= 0)
                    break;

			    send(comm_socket, buff, strlen(buff), 0);
			}
		}
		else
		{
			printf(".");
		}
		printf("Connection closed\n");
		close(comm_socket);
	}
  printf("Done\n" );
}
/*
* MAIN
*/
int main(int argc, char const *argv[])
{
  cout<< "Hello server" <<endl;
  init_connection();
  return 0;
}

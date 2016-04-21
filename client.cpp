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
  printf("Simple Client capable of transfering files from and to local custom server"
  "\n\tUsage: client -h <hostname> -p <port> [-d|-u] <file>\n");
}

/*
* 
*/
void handle_transfer(int socket)
{
  const int BUFSIZE = 1024;
  char buf[BUFSIZE];
  bzero(buf, BUFSIZE);

  printf("Please enter msg: ");
  fgets(buf, BUFSIZE, stdin);

  int sended = send(socket, buf, strlen(buf), 0);
  if (sended < 0)
    perror("ERROR in sendto");

  int received = recv(socket, buf, BUFSIZE, 0);
  if (received < 0)
    perror("ERROR in recv");

  printf("Echo from server: %s", buf);
}

/*
* Prepare connection
*/
int get_connection(struct sockaddr_in dest)
{
  int mysocket;
  if((mysocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // creating socket
    fprintf(stderr,"SOCKERR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
   }

  printf("Connected to:%s:%d\n", inet_ntoa(dest.sin_addr), ntohs(dest.sin_port));
  if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1 )
  {
    fprintf(stderr,"CONNERR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return mysocket;
}

/*
* Returns s_addr with translated address upon succesfull tranlation
*/
unsigned long get_translated_addr(char *address)
{
  struct hostent *server_address;        //provide DNS translation
  server_address = gethostbyname(address);

  if ( server_address == NULL) {        //check if translation was succesfull
    fprintf(stderr,"DNSERR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  struct in_addr ip_addr;
  memcpy(&ip_addr, server_address->h_addr_list[0], sizeof(struct in_addr));

  return inet_addr(inet_ntoa(ip_addr)); //convert to righ byte order an and return
}
/*
* INITIALIZE CONNECTION TO SERVER
*/
void init_connection(int port,char *address)
{
  struct sockaddr_in dest;
  memset(&dest, 0, sizeof(dest));             // setting up struct for connect
  dest.sin_family = AF_INET;                  // ipv4
  dest.sin_addr.s_addr = get_translated_addr(address); // translating addr to right order
  dest.sin_port = htons(port);                // set destination port

  int socket = get_connection(dest);
  handle_transfer(socket);

  close(socket);
}
/*
* Parsing numeric values + error detection
*/
int check_num_args(char *arg)
{
   	char *white;
   	int num = (int)strtod(arg,&white);
   	if(strlen(white) != 0)
   	{
     	fprintf(stderr,"Unexpected input \"%s\"\n",white);
     	exit(EXIT_FAILURE);
   	}
   	else
   	  return num;   //no char detected, numeric value is returned
}
/**
* CHECKS if client was executed with right args
*/
void check_args(int argc, char **argv)
{
  if (argc == 7)
  {
    if((strcmp(argv[1],"-h")) == 0 && (strcmp(argv[3],"-p")) == 0 &&
      ((strcmp(argv[5],"-u")) == 0 || (strcmp(argv[5],"-d")) == 0))
        return;
  }
  help();
  exit(EXIT_FAILURE);
}
/*
* MAIN
*/
int main(int argc, char *argv[])
{
  if (argc == 2  && (strcmp(argv[1],"--help") == 0))
  {
    help();
    exit(EXIT_SUCCESS);
  }

  check_args(argc, argv);
  int port = check_num_args(argv[4]);
  init_connection(port,argv[2]);

  return 0;
}

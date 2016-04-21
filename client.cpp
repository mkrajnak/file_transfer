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
* INITIALIZE CONNECTION TO SERVER
*/
int init_connection(char *address)
{
  if (address == NULL) {
    exit(EXIT_FAILURE);
  }

  struct hostent *web_address;        //provide DNS translation
  web_address = gethostbyname(address);

  if ( web_address == NULL) {        //check if translation was succesfull
    fprintf(stderr,"DNSERR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  // in_addr is struct required by inet_ntoa
  // which is needed to translate from network byte order
  struct in_addr ip_addr;
  memcpy(&ip_addr, web_address->h_addr_list[0], sizeof(struct in_addr));

  int mysocket;
  if((mysocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // creating socket
    fprintf(stderr,"SOCKERR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
   }
  int port_number = 6666;
  struct sockaddr_in dest;
  memset(&dest, 0, sizeof(dest));                       // setting up struct for connect
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = inet_addr(inet_ntoa(ip_addr)); // translating addr to right order
  dest.sin_port = htons(port_number);              // set destination port

  printf("INFO: Server socket: %s : %d \n", inet_ntoa(dest.sin_addr), ntohs(dest.sin_port));

  if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1 )
  {
    fprintf(stderr,"CONNERR: %s\n", strerror(errno));
    return -1;
  }

  const int BUFSIZE = 1024;
  char buf[BUFSIZE];
  /* nacteni zpravy od uzivatele */
  bzero(buf, BUFSIZE);
  printf("Please enter msg: ");
  fgets(buf, BUFSIZE, stdin);

  /* odeslani zpravy na server */
  int sended = send(mysocket, buf, strlen(buf), 0);
  if (sended < 0)
    perror("ERROR in sendto");

  /* prijeti odpovedi a jeji vypsani */
  int received = recv(mysocket, buf, BUFSIZE, 0);
  if (received < 0)
    perror("ERROR in recv");

  printf("Echo from server: %s", buf);
  close(mysocket);
  return 0;
}

/*
* MAION
*/
int main(int argc, char const *argv[])
{
  int client_socket = init_connection("localhost");

  return 0;
}

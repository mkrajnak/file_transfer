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
using namespace std;


/**
* INITIOALIZING connection
*/
int init_connection(struct url_info_t *url)
{
  if ( url == NULL) {
    return -1;
  }
  // printf("**********CONNECTING***************\n" );
  //    printf("ADD:\t%s \n",url->address );
  //    printf("DNS:\t%s \n",url->base_address);
  //    printf("PATH:\t%s \n",url->path );
  //    printf("FILE\t%s \n",url->filename);
  //    printf("Port:\t%d\n",url->port_number );

  struct hostent *web_address;
  if (!strcmp(url->base_address,FIT)){ //hack for fit url
    web_address = gethostbyname(FIT);
  }
  else
    web_address = gethostbyname(url->base_address);


  if ( web_address == NULL) {                         //check if translation was succesfull
    fprintf(stderr,"DNSERR: %s\n", strerror(errno));
    return -1;
  }
  // in_addr is struct required by inet_ntoa
  // which is needed to translate from network byte order
  struct in_addr ip_addr;
  memcpy(&ip_addr, web_address->h_addr_list[0], sizeof(struct in_addr));

  int mysocket;
   if((mysocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // creating socket
     fprintf(stderr,"SOCKERR: %s\n", strerror(errno));
     return -1;
   }

  struct sockaddr_in dest;
  memset(&dest, 0, sizeof(dest));                       // setting up struct for connect
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = inet_addr(inet_ntoa(ip_addr)); // setting properly destination ip address
  dest.sin_port = htons(url->port_number);              // set destination port

  if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1 )
  {
    fprintf(stderr,"CONNERR: %s\n", strerror(errno));
    return -1;
  }
  return mysocket;
}


int main(int argc, char const *argv[])
{
  cout<< "Hello server" <<endl;
  return 0;
}

/**
* Project: Simple file transfer client
*         created for IPK course at fit.vutbr.cz
* Author: Martin Krajnak
* Date: 22.4.2016
*/
#include "client.h"
using namespace std;

/**
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

  //printf("Please enter msg: ");
  fgets(buf, BUFSIZE, stdin);

  int sended = send(socket, buf, strlen(buf), 0);
  if (sended < 0)
    perror("ERROR in sendto");

  int received = recv(socket, buf, BUFSIZE, 0);
  if (received < 0)
    perror("ERROR in recv");

  //printf("Echo from server: %s", buf);
}

/**
* Prepare connection
*/
int get_connection(struct sockaddr_in dest)
{
  int mysocket;
  if((mysocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // creating socket
    fprintf(stderr,"SOCKERR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
   }

  //printf("Connected to:%s:%d\n", inet_ntoa(dest.sin_addr), ntohs(dest.sin_port));
  if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1 )
  {
    fprintf(stderr,"CONNERR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return mysocket;
}

/**
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

/**
* INITIALIZE CONNECTION TO SERVER
*/
int init_connection(int port, char *address)
{
  struct sockaddr_in dest;
  memset(&dest, 0, sizeof(dest));             // setting up struct for connect
  dest.sin_family = AF_INET;                  // ipv4
  dest.sin_addr.s_addr = get_translated_addr(address); // translating addr to right order
  dest.sin_port = htons(port);                // set destination port

  return get_connection(dest);
}

/**
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

void send_msg(int socket, char *msg)
{
  int sended = send(socket, msg, strlen(msg), 0);
  if (sended < 0)
    perror("SENDERR");
}
/**
* Function return first match achieved rgx_string
*/
string get_regex_match(char *haystack,char * rgx_string)
{
  cmatch match;         //store matches
  regex rgx(rgx_string);//create and compile regex
  if (!(regex_search(haystack, match, rgx))) {//try to find
    fprintf(stderr, "FATALERR: Message unmatched\n" );
    exit(EXIT_FAILURE);
  }
  return match.str(1);
}
/**
* Download
*/
void download (int socket, char * filename)
{
  char msg[100];
  strcpy(msg,"#DWN#RQT#");
  strcat(msg,filename);
  strcat(msg,"#");
  send_msg(socket, msg);

  char buffer[1024];
  int received = recv(socket, buffer, 1023, 0);
  if (received < 0)
    perror("ERROR in recv");

  string response = string(buffer);
  if (response.find("#DWN#FER#") != string::npos){
      fprintf(stderr, "Server cannot locate file: Try again\n" );
      exit(EXIT_FAILURE);
  }
  string filesize = get_regex_match(buffer,(char *)"#DWN#ACK#.+#([0-9]+)#");
  //cout << "File: " << filename << " Size: " << filesize << " B" << endl;

  send_msg(socket,(char *)"#DWN#ACK#");// Confirmation for client
  //cout << "The tranfer shall begin !" << endl;
  ofstream file;
  file.open(filename,ios::binary);
  if (!file.is_open()) {
    fprintf(stderr,"Could not open a file \n");
    exit(EXIT_FAILURE);
  }

  char upload_buffer[1024];                          // initialize buffer
  int written = 0;
  int file_int_size = atoi(filesize.c_str());         // get filesize as integer

  while (written < file_int_size)            // download file from client
  {
    received = recv(socket, upload_buffer ,1023, 0);
    file.write(upload_buffer, received);
    written += received;
    memset(upload_buffer, 0, 1024);              //clean the buffer before next tranfer
  }
  send_msg(socket,(char *)"#DWN#ACK#");
  //cout << "Transfered: " << written << " B" << endl;
  file.close();
}
/**
* UPLOAD
*/
void upload(int socket, char *filename)
{
  const int BUFSIZE = 1024;
  char buf[BUFSIZE];
  bzero(buf, BUFSIZE);

  ifstream file;
  file.open(filename,ios::binary);
  if (!file.is_open()){
    fprintf(stderr,"Could not open a file %s\n", filename );
    exit(EXIT_FAILURE);
  }

  file.seekg(0, file.end);
  std::stringstream ss;
  ss << file.tellg();
  string file_size = ss.str();
  file.seekg(0, file.beg);

  char msg[100];
  strcpy(msg,"#UPL#RQT#");
  strcat(msg,filename);
  strcat(msg,"#");
  strcat(msg,file_size.c_str());
  strcat(msg,"#");

  //cout << "size" << endl;
  //printf("%s\n",msg );
  send_msg(socket, msg);

  int received = recv(socket, buf, BUFSIZE, 0);
  if (received < 0)
    perror("ERROR in recv");

  string response = string (buf);
  //size_t found = response.find("UPLOAD#ACK#");

  char c[1024];
  while(file.good())
  {
    file.read(c,1024);
    int sended = send(socket, c, file.gcount(), 0);
    if (sended < 0)
      perror("SENDERR");
  }
  //send_msg(socket,(char*)"#UPL#ACK#");
  //cout << "Done" <<endl;
  //printf("Echo from server: %s", buf);
}

/**
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
  int socket = init_connection(port, argv[2]);

  if ((strcmp(argv[5],"-d")) == 0)
    download(socket,argv[6]);
  else
    upload(socket,argv[6]);

  close(socket);

  return 0;
}

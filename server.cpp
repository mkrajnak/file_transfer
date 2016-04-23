/**
* Project: Simple file transfer server
*         created for IPK course at fit.vutbr.cz
* Author: Martin Krajnak
* Date: 22.4.2016
*/
#include "server.h"
using namespace std;

/**
* HELP
*/
void help()
{
  printf("Simple File Server:\n\tUSAGE: server -p <port_number>\n" );
}
/*
* Listen for new connections handle errors
*/
void listen_wrapper(int server_socket)
{
  if ((listen(server_socket, 1)) < 0) // listen for incomming connection
  {
    perror("ERROR: listen");
    exit(EXIT_FAILURE);
  }
}

/*
* Try to fork and handle errors
*/
int fork_handler()
{
  int pid;
  if((pid = fork()) < 0)  //Try to create new process
  {
    perror("FORKERR");
    exit(EXIT_FAILURE);
  }
  return pid;
}

/*
* Will return new socket for serving new client
*/
int get_new_client(int server_socket)
{
  struct sockaddr_in client;              //new client address
  socklen_t client_len = sizeof(client);  //size
  return accept(server_socket, (struct sockaddr*)&client, &client_len);
}
/**
* Send message
*/
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
/*
* Communicate with client
*/
void serve(int client_socket)
{
  printf("INFO: New connection:\n");
  char buff[1024];
  string filesize;
  string filename;
  while((recv(client_socket, buff, 1023,0)) > 0)//handle message
  {
    string received = string (buff);
    size_t found = received.find("#UPLOAD#RQT#");

    filesize = get_regex_match(buff,"#UPLOAD#RQT#.+#([0-9]+)#");
    filename = get_regex_match(buff,"#UPLOAD#RQT#(.+)#[0-9]+#");
    cout << "File " << filename;
    cout << " Size " << filesize << endl;
    cout << "The tranfer shall begin !" << endl;

    if (found != string::npos)
      send_msg(client_socket,"#UPLOAD#ACK#");
    else
      send(client_socket, buff, strlen(buff), 0);
    break;
    memset(buff, 0,1024);
  }

  ofstream file;
  file.open(filename,ios::binary);
  if (!file.is_open()) {
    fprintf(stderr,"Could not open a file \n");
    exit(EXIT_FAILURE);
  }
  unsigned int buffer_size = 1024;
  char buffer[buffer_size];
  int received;
  int file_int_size = stoi(filesize);
  int written = 0;
  while (1)//read data
  {
    received = recv(client_socket, buffer, buffer_size - 1, 0);
    file.write(buffer, received);
    written += received;
    if (written >= file_int_size) {
      break;
    }
    memset(buffer, 0, buffer_size);
  }
  cout << "Downloaded " << written << "bytes" << endl;
  file.close();
  send(client_socket, "#UPLOAD#ACK#", strlen("#UPLOAD#ACK#")-1, 0);
  close(client_socket);
  printf("Connection closed\n");
}

/**
*   Handles communication between server and client
*/
void handle_communication(int server_socket)
{
  cout<< "Hello server" << endl;
  listen_wrapper(server_socket);

  while(1)
  {
    int client_socket = get_new_client(server_socket);
		{
      int pid = fork_handler();
      if (pid == 0) //handle new connection inside new process
        serve(client_socket);
      else
        close(client_socket); //parent
		}
	}
}

/**
* Try to get socket
*/
int get_socket()
{
  int server_socket = 0;
  if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)  // creating socket
  {
    perror("SOCKERR");
    exit(EXIT_FAILURE);
  }
  return server_socket;
}

/**
* INITIALIZING connection
*/
void init_server(int port)
{
  int server_socket = get_socket();
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
    perror("CONNERR: socket");
    exit(EXIT_FAILURE);
  }
  handle_communication(server_socket);
}

/**
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

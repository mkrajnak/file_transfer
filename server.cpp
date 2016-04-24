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
/**
* opens and chel file
*/
ofstream file_opener(string filename)
{
  ofstream file;
  file.open(filename,ios::binary);
  if (!file.is_open()) {
    fprintf(stderr,"Could not open a file \n");
    exit(EXIT_FAILURE);
  }
  return file;
}
/**
* Receive file from client
*/
void get_file_from_client(int client_socket, char * buffer)
{
  string filesize = get_regex_match(buffer,(char *)"#UPL#RQT#.+#([0-9]+)#");
  string filename = get_regex_match(buffer,(char *)"#UPL#RQT#(.+)#[0-9]+#");
  cout << "File: " << filename << " Size: " << filesize << " B" << endl;

  send_msg(client_socket,(char *)"#UPL#ACK#");// Confirmation for client
  cout << "The tranfer shall begin !" << endl;
  ofstream file = file_opener(filename);      // open a file

  char upload_buffer[1024];                          // initialize buffer
  int received;
  int written = 0;
  int file_int_size = stoi(filesize);         // get filesize as integer

  while (written < file_int_size)            // download file from client
  {
    received = recv(client_socket, upload_buffer ,1023, 0);
    file.write(upload_buffer, received);
    written += received;
    memset(upload_buffer, 0, 1024);              //clean the buffer before next tranfer
  }
  cout << "Transfered: " << written << " B" << endl;
  file.close();
}
/**
*
*/
void deliver_file_to_client(int client_socket,char * buffer)
{
  printf("%s\n",buffer );
  string filename = get_regex_match(buffer,(char *)"#DWN#RQT#(.+)#");
  cout << "File: " << filename;

  ifstream file;
  file.open(filename,ios::binary);
  if (!file.is_open()){
    //fprintf(stderr,"Could not open a file %s\n", filename );
    exit(EXIT_FAILURE);
  }

  file.seekg(0, file.end);
  string file_size = to_string(file.tellg());
  file.seekg(0, file.beg);

  char msg[100];
  strcpy(msg,"#DWN#ACK#");
  strcat(msg,filename.c_str());
  strcat(msg,"#");
  strcat(msg,file_size.c_str());
  strcat(msg,"#");

  printf("%s\n",msg );
  send_msg(client_socket, msg);
  char buf[1024];
  int received = recv(client_socket, buf, 1023, 0);
  if (received < 0)
    perror("ERROR in recv");

  string response = string (buf);
  //size_t found = response.find("UPLOAD#ACK#");

  char c[1024];
  while(file.good())
  {
    file.read(c,1024);
    cout << "1";
    int sended = send(client_socket, c, file.gcount(), 0);
    if (sended < 0)
      perror("SENDERR");
  }
  cout << "Done" <<endl;
}
/*
* Communicate with client
*/
void serve(int client_socket)
{
  printf("INFO: New connection:\n");
  char buffer[1024];
  while((recv(client_socket, buffer, 1023,0)) > 0)//handle message
  {
    string received = string (buffer);  //conrversion to string

    if (received.find("#UPL#RQT#") != string::npos) //Requested upload
      get_file_from_client(client_socket, buffer);
    else if ( received.find("#DWN#RQT#") != string::npos )  //Requested download
      deliver_file_to_client(client_socket, buffer);
    else
      send_msg(client_socket,(char *)"#ERR#ACK#");//Message not recognized ERR
    memset(buffer, 0, 1024);
  }

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

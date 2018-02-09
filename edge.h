#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <bitset>
#include <fstream>
#include <netdb.h>

using namespace std;

const char* HOST_TO_LISTEN_ON = "localhost";
const char* TCP_PORT_TO_LISTEN_ON = "23074";
const char* UDP_PORT_TO_LISTEN_ON = "24074";
const int NUM_BACKLOG = 20;
const char* AND_SERVER_HOST_ADDR = "localhost";
const char* AND_SERVER_PORT = "22074";
const char* OR_SERVER_HOST_ADDR = "localhost";
const char* OR_SERVER_PORT = "21074";

//Job Struct
struct job{
  bool isAnd;
  unsigned long a;
  unsigned long b;
};

//Prototypes
bool beginListening();
void computeAnd(char* c);
void computeOr(char* c);
void getA(string a, char* c);
void getB(string b, char* c);
int seperateJobsIntoArray(job* j, char* c);
char* j_to_opstr(job j);

string trim0s(string in);

//Member variables
  int status;
  struct addrinfo tcp_hints;
  struct addrinfo udp_hints;
  struct addrinfo *serverinfo;
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  int sock;
  int new_fd;

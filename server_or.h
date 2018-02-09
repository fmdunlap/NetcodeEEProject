#include <iostream>
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
#include <bitset>

using namespace std;

const char* myHost = "localhost";
const char* myPort = "21074";

int sock;
struct addrinfo hints, *info;
struct sockaddr_storage their_addr;
int status;

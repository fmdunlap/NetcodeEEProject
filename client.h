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

//CONSTANTS
const char* toHostName = "localhost"; //Send to Localhost
const char* toPortNum = "23074"; //Edge-Server port

//Function Prototypes
void loadJobs(vector<string> vecStr);
vector<string> loadFile(char* ch);

void loadFileIntoJobs(char* ch);

void sendJobsToServer();

void outputResults(char res[]);

string trim0s(string in);

//Job Struct
struct job{
  bool isAnd;
  unsigned long a;
  unsigned long b;
};

//Member Variables
  vector<job> jobs;
  int status;
  struct addrinfo hints;
  struct addrinfo *serverinfo;

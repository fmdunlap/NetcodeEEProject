#include "server_or.h"

//Our friend the trim fn from both client and edge
string trim0s(string in){
        int i = in.find_first_of("1");
        if(i != string::npos) return in.substr(i);
        else return "0";
}

int main(){
  //make our buffer,
  char buf[1000];

  //make sure hints is clear, and then set the family as "I don't care" and the
  //socket as a datagram socket.
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  //Get the info!
  status = getaddrinfo(myHost, myPort, &hints, &info);
  cout << "The OR server is up and has booted with status: " << status << endl;
  cout << "Running on port: " << myPort << endl;

  //Use the info! (Bind the listening socket.)
  sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
  bind(sock, info->ai_addr, info->ai_addrlen);
  freeaddrinfo(info);

  while(1){
    //their (the edge's) address.
    socklen_t addr_len = sizeof their_addr;
    cout << "Wating for next job." << endl;

    //if the recieve is valid put it in the buffer...
    if(recvfrom(sock, buf, sizeof buf, 0, (struct sockaddr *) &their_addr, &addr_len) == -1){
      cout << "Recvfrom error" << endl;
      exit(1);
    }
    //... save it in a string. Then,
    cout << "Recieved job! " << endl;
    string resString(buf);

    //if the string isn't the exit string (Not actually implemented...)
    if(strcmp(resString.c_str(), "EXIT") == 0){
      close(sock);
      exit(0);
    }

    //do the maths
    string a = resString.substr(0, 10);
    string b = resString.substr(10);
    bitset<10> result = bitset<10>(resString.substr(0, 10)) | bitset<10>(resString.substr(10));

    //spit out the result for the user, and handle the data shuffling
    cout << "Result: " << trim0s(a) << " or " << trim0s(b) << " = " << trim0s(result.to_string()) << endl;
    memset(&buf, 0, sizeof buf);
    strcpy(buf, result.to_string().c_str());

    //then send it back! ☜(⌒▽⌒)☞
    sendto(sock, buf, sizeof buf, 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
    cout << "Successfully sent result back to sender." << endl;
  }

  return 0;
}

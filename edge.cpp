#include "edge.h"

int main(int argc, char* argv[]){
        if(!beginListening()) {
                cout << "BEGIN LISTENING ERROR" << endl;
                return -1;
        }
}

bool beginListening(){
        memset(&tcp_hints, 0, sizeof tcp_hints);
        tcp_hints.ai_family = AF_UNSPEC;
        tcp_hints.ai_socktype = SOCK_STREAM;

        if(getaddrinfo(HOST_TO_LISTEN_ON, TCP_PORT_TO_LISTEN_ON, &tcp_hints, &serverinfo) != 0) return false;

        sock = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
        bind(sock, serverinfo->ai_addr, serverinfo->ai_addrlen);
        listen(sock, NUM_BACKLOG);

        //Don't need that stuff anymore!
        freeaddrinfo(serverinfo);

        cout << "The edge server is up and running." << endl;

        addr_size = sizeof their_addr;
        while(1) {
                new_fd = accept(sock, (struct sockaddr *)&their_addr, &addr_size);

                if(!fork()) {
                        char buf[1000];
                        close(sock);
                        int numbytes;
                        job jobs[200];
                        int numJobs = 0;
                        vector<string> results;
                        memset(&jobs, 0, sizeof jobs);
                        int andJobNum = 0;
                        int orJobNum = 0;

                        cout << "Client connected." << endl;

                        while(1) {
                                if ((numbytes = recv(new_fd, buf, 1000-1, 0)) == -1) {
                                        perror("recv");
                                        exit(1);
                                        break;
                                }
                                buf[numbytes] = '\0';
                                if(numbytes != 0) {
                                        if(strcmp("EXIT", buf) == 0) {
                                                close(new_fd);
                                                exit(0);
                                                break;
                                        } else {
                                                numJobs = seperateJobsIntoArray(jobs, buf);
                                                cout << "The edge server has recieved " << numJobs << " from the client using TCP over port " << TCP_PORT_TO_LISTEN_ON << endl;
                                                for(int i = 0; i < numJobs; i++) {

                                                        //1) Determine which compute server to send to
                                                        //2) send
                                                        //3) wait for reciept
                                                        //4) add to return array
                                                        //repeat until done
                                                        //and voila!

                                                        const char* port;
                                                        const char* host;

                                                        //2) Determine
                                                        string aor;
                                                        if(jobs[i].isAnd) {
                                                                andJobNum++;
                                                                port = AND_SERVER_PORT;
                                                                host = AND_SERVER_HOST_ADDR;
                                                                aor = " AND ";
                                                        } else {
                                                                orJobNum++;
                                                                port = OR_SERVER_PORT;
                                                                host = OR_SERVER_HOST_ADDR;
                                                                aor = " OR ";
                                                        }

                                                        //Yeah. We're not gonna talk about it.
                                                        //I.E. this is just an ugly way for me to cling onto my bitsets
                                                        //because bitsets are ~AWESOME~ ☜(˚▽˚)☞
                                                        char jstr[21]; //<- The maximum possible stringsize given constraints
                                                        string retStr = "";
                                                        retStr += bitset<10>(jobs[i].a).to_string();
                                                        retStr += bitset<10>(jobs[i].b).to_string();
                                                        strcpy(jstr, retStr.c_str());

                                                        //make sure stuff is cleared
                                                        memset(&udp_hints, 0, sizeof udp_hints);
                                                        memset(&serverinfo, 0, sizeof serverinfo);
                                                        //Tell hints to use UDP for these
                                                        udp_hints.ai_family = AF_UNSPEC;
                                                        udp_hints.ai_socktype = SOCK_DGRAM;


                                                        //get their address, and length of it and stuff.
                                                        //Then create the 'sendUDPTo int'
                                                        struct sockaddr_storage their_addr;
                                                        socklen_t addr_len = sizeof their_addr;
                                                        int info = getaddrinfo(host, port, &udp_hints, &serverinfo);
                                                        int sudpto = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);

                                                        //SEND IT!
                                                        sendto(sudpto, jstr, sizeof jstr, 0, serverinfo->ai_addr, serverinfo->ai_addrlen);
                                                        cout << "Sent job to " << aor << " server." << endl;
                                                        cout << "Waiting for result from " << aor << " server on port " << port << endl;
                                                        //Wait to recieve the thing back from the UDP server
                                                        recvfrom(sudpto, buf, sizeof buf, 0, (struct sockaddr *) &their_addr, &addr_len);

                                                        //subject line
                                                        string toStore = buf;
                                                        results.push_back(toStore);

                                                        //Let the user know!
                                                        cout << "Recieved: " << trim0s(retStr.substr(1, 10)) << aor << trim0s(retStr.substr(11)) << " = " << trim0s(toStore) << endl;
                                                }
                                                /*
                                                   This is due to an idiosyncracy with my design choices. In short, it has to do with the
                                                   fact that my primary goal was to minimize the number of messages sent back and for between
                                                   client and edge. So, I don't send things back and forth immediately, instead I send
                                                   everything in one packet. In practice, that means I can count the number of jobs and
                                                   display them after they're done, but that is just simple data manipulation. Thus, the "I SENT
                                                   X jobs to AND/OR" is displayed AFTER the results are recieved.
                                                 */
                                                cout << "The edge has successfully sent " << orJobNum << " lines to Backend-Server OR." << endl;
                                                cout << "The edge has successfully sent " << andJobNum << " lines to Backend-Server AND." << endl;
                                                cout << "The edge server has successfully finished recieving all computation results from the AND and OR servers" << endl;
                                                string sendString = "";
                                                for (unsigned int i = 0; i < results.size(); i++) {
                                                        sendString += results[i];
                                                        sendString += ",";
                                                }


                                                char sendBuf[1400];
                                                strcpy(sendBuf, sendString.c_str());
                                                sleep(1);
                                                send(new_fd, sendBuf, sizeof sendBuf, 0);
                                                cout << "The edge server has successfully finished sending all results to the client." << endl << endl;
                                        }
                                        //Finished transaction
                                        close(new_fd);
                                        exit(0);
                                } else {
                                        close(new_fd);
                                        perror("numbytes");
                                        exit(1);
                                        break;
                                }
                        }
                        close(new_fd);
                }
        }
        return true;
}


//I used this and the computeOr fn for debugging purposes.
void computeAnd(char* c){
        string a;
        string b;

        for(int i = 1; i < 11; i++) {
                a += c[i];
        }

        for(int i = 11; i < 21; i++) {
                b += c[i];
        }

        cout << "AND: " << a << " " << b << endl;
}

void computeOr(char* c){
        string a;
        string b;

        for(int i = 1; i < 11; i++) {
                a += c[i];
        }

        for(int i = 11; i < 21; i++) {
                b += c[i];
        }

        cout << "OR: " << a << " " << b << endl;
}


//TODO: make this ONE function that I can give bounds to... Much prettier.
//Nah...
void getA(string a, char* c){
        a = string(c);
}

void getB(string b, char* c){
        for(int i = 11; i < 21; i++) {
                b += c[i];
        }
}

//Same as in the client file, really.
int seperateJobsIntoArray(job* j, char* c){
        int numJobs = 0;
        string str = c;

        int i;

        while((i = str.find(",")) != string::npos) {
                string currJob = str.substr(0, i);
                job newJob;

                if(currJob.at(0) =='1') newJob.isAnd = true;
                else newJob.isAnd = false;

                newJob.a = bitset<10>(currJob.substr(1, 10)).to_ulong();
                newJob.b = bitset<10>(currJob.substr(11, 10)).to_ulong();

                str = str.substr(i+1);
                j[numJobs] = newJob;
                numJobs++;
        }

        return numJobs;
}

//Literally the exact same function as is in the client.cpp
string trim0s(string in){
        int i = in.find_first_of("1");
        if(i != string::npos) return in.substr(i);
        else return "0";
}

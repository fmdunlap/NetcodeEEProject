#include "client.h"

//happily abstracted main! Basically, we just tell the user that the file is up,
//then we load the file into the job vector, and ~magically~ send it to the
//edge server. (Unless they opened the file wrong...)
int main(int argc,char* argv[]){
        if(argc == 2) {
                cout << "Client is up." << endl;
                loadFileIntoJobs(argv[1]);

                sendJobsToServer();
                return 0;
        } else {
                cout << "INCORRECT FORMAT: $./out.o relative/or/absolute/path/to/input/file"
                     << endl;
                return -1;
        }
}

//Simple helper to load in the input file
void loadFileIntoJobs(char* ch){
  //Pull the strings from the file into a vector of strings...
        vector<string> vS = loadFile(ch);
  //Reformat that vector into actual 'job' items. Just for management of data.
        loadJobs(vS);
}

//Another pretty straight forward helper. Not my best work, but it's effective.
void loadJobs(vector<string> vecStr){
        for (std::vector<string>::iterator it = vecStr.begin();
             it != vecStr.end();
             ++it) {
                job j;
                //AndOr detection. Really not pretty or robust, but w/e.
                string str = *it;
                if(str.size() > 0) {

                  //EEWWW ugly (but effective) string parsing.
                        int locOfFirstComma = str.find(',');
                        if(locOfFirstComma == 2) j.isAnd = false;
                        else j.isAnd = true;

                        int locOfSecondComma = str.find(',', locOfFirstComma+1);
                        j.a = bitset<16>(str.substr(locOfFirstComma+1,
                                                    locOfSecondComma-locOfFirstComma-1)).to_ulong();

                        j.b = bitset<16>(str.substr(locOfSecondComma+1)).to_ulong();

                        jobs.push_back(j);
                }
        }
}

//Just a helper function to get the file into a vector of strings.
vector<string> loadFile(char* ch){
        vector<string> retVec;

        string filePath(ch);
        string line;
        fstream ifs(ch);

        if(ifs.is_open()) {
                while(getline(ifs, line)) {
                        retVec.push_back(line);
                }
        } else cout << "Unable to open file!" << endl;

        ifs.close();
        return retVec;
}

//Send those jobs from the job array to the server!!
void sendJobsToServer(){
  //Make sure that the hints struct is ~actually~ empty.
        memset(&hints, 0, sizeof(hints));
        //Set the address family to unspecified
        hints.ai_family = AF_UNSPEC;
        //Tell the hints to use TCP
        hints.ai_socktype = SOCK_STREAM;

        //If the whole thing works out... (And we populate serverinfo properly)
        if((status = getaddrinfo(toHostName, toPortNum, &hints, &serverinfo)) != 0)
        {
                cout << "ADDRINFO ERROR: ";
                throw gai_strerror(status);
        }

        //Create the socket info thing!
        int s = socket(serverinfo->ai_family,
                       serverinfo->ai_socktype,
                       serverinfo->ai_protocol);

        //Now actually connect to the server using the info from the serverinfo
        //struct created in line 82
        connect(s, serverinfo->ai_addr, serverinfo->ai_addrlen);

        //"I'm gonna be a buffer!!"
        char* c = new char[4200];
        string toSend = "";

        for(unsigned int i = 0; i < jobs.size(); i++) {
                job currJob = jobs.at(i);

                /*
                   TODO: Get send-size down from 21 Bytes to 3 Bytes (definitely
                   doable via bitset) Or better yet, serialization?! Or well...
                   I could have. It is doable to translate the data into straight
                   bits, but frankly, this works.
                 */
                if(currJob.isAnd) toSend += "1";
                else toSend += "0";
                toSend += bitset<10>(currJob.a).to_string();
                toSend += bitset<10>(currJob.b).to_string();
                toSend += ",";
        }

        //"I'M ANOTHER BUFFER FOR THE RECIEVED INFO!!! :)"
        char buf[1500];

        //Now send the stuff we just put into a string to the server we got the info of earlier.
        strcpy(c, toSend.c_str());
        send(s, c, strlen(c), 0);
        //Tell the user that we sent the stuff.
        cout << "The client has successfully finished sending " << jobs.size() << " lines to the server." << endl;

        //Now wait for a response from the server!
        recv(s, buf, 1500, 0);
        cout << "The client has successfully finished recieving all computation results from the edge server." << endl << "The final computation results are:" << endl;

        //Print stuff and close the server
        outputResults(buf);
        close(s);

        //And clear out the address info.
        freeaddrinfo(serverinfo);
}

//Helper function to print things nicely.
void outputResults(char res[]){
        string str(res);
        int jobNum = 0;
        int i = 0;

        while((i = str.find(",")) != string::npos) {
                job currJob = jobs[jobNum];
                string a = bitset<10>(currJob.a).to_string();
                string b = bitset<10>(currJob.b).to_string();
                string aor;

                if(currJob.isAnd) aor = " and ";
                else aor = " or ";

                //TODO: Really could be prettier. It's effective but it doesn't handle the "WHAT IF I'M ZERO" issue.
                //Wait did I fix this or nah?

                string result = str.substr(0, i);
                result = trim0s(result);

                cout << trim0s(a) << aor << trim0s(b) << ": " << result << endl;

                str = str.substr(i+1);
                jobNum++;
        }
}
//This piece of code is in (almost) all of my files, I believe. It's hacky, but it works marvels.
string trim0s(string in){
        int i = in.find_first_of("1");
        if(i != string::npos) return in.substr(i);
        else return "0";
}

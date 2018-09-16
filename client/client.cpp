/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
/*
 * ./client <CLIENT_IP>:<UPLOAD_PORT> <TRACKER_IP_1>:<TRACKER_PORT_1> <TRACKER_IP_2>:<TRACKER_PORT_2> <log_file>
 * ./client 10.1.38.138:6528 10.1.38.138:6565 10.1.38.138:6567 log
 */
#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "client_header.h"
#endif

#ifndef CL_GLOBAL_H
#define CL_GLOBAL_H
#include "cl_global.h"
#endif

/*
 * Process command line arguments and stores
 * all ip/port in apropriate fields in global
 * variables.
 */
void process_args(char *argv[])
{
    char *token = strtok(argv[1], ":");
    cl_ip = token;
    token = strtok(NULL, ":");
    cl_port = stoi(token);

    token = strtok(argv[2], ":");
    tr1_ip = token;
    token = strtok(NULL, ":");
    tr1_port = stoi(token);

    token = strtok(argv[3], ":");
    tr2_ip = token;
    token = strtok(NULL, ":");
    tr2_port = stoi(token);

    log_file = argv[4];
}
/*
 * Creates Socket by using provided ip and ports.
 */
int soc_creation()
{
    struct sockaddr_in tr1_addr;
    struct sockaddr_in tr2_addr;
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        cout << "Socket creation error" << endl;
        exit(EXIT_FAILURE);
    }
    memset(&tr1_addr, '0', sizeof(tr1_addr));
    memset(&tr2_addr, '0', sizeof(tr2_addr));

    tr1_addr.sin_family = AF_INET;
    tr2_addr.sin_family = AF_INET;
    tr1_addr.sin_port = htons(tr1_port);
    tr2_addr.sin_port = htons(tr2_port);

    if (inet_pton(AF_INET, tr1_ip.c_str(), &tr1_addr.sin_addr) <= 0)
    {
        cout << "Invalid Tracker 1 address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }
    if (inet_pton(AF_INET, tr2_ip.c_str(), &tr2_addr.sin_addr) <= 0)
    {
        cout << "Invalid Tracker 1 address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&tr1_addr, sizeof(tr1_addr)) < 0)
    {
        cout << " Tracker 1 seems Busy.. Trying to connect with Tracker 2.. " << endl;
        if (connect(sock, (struct sockaddr *)&tr2_addr, sizeof(tr2_addr)) < 0)
        {
            cout << "Connection Failed :( \n Try again later" << endl;
        }
    }
    return sock;
}

vector<string> split_command(string command_string)
{
    vector<string> my_command;
    string tmp = "";
    unsigned int i = 0;
    for (; i <= command_string.size(); i++)
    {
        if (command_string[i] == ' ')
        {
            my_command.push_back(tmp);
            tmp = "";
        }
        else if (command_string[i] == '\\')
            tmp += command_string[++i];
        else
            tmp += command_string[i];
    }
    my_command.push_back(tmp);
    // for(auto i:my_command) cout << i << endl;

    return my_command;
}


void notify_server(string filename, int sock)
{   
    string msg,sh,f_path,s;
    ifstream i_file;
    i_file.open(filename);
    for(int i=0;i<5;i++){
        if(i==0 || i==1 || i==3){
            getline( i_file, s);
            continue;
        }
        else if ( i == 2){
            getline(i_file, f_path);
        }
        else if(i == 4)
        {
            getline(i_file, sh);
        }
    }
    cout << "File Path = " << f_path << endl;
    sh = get_SHA1((char *)sh.c_str(),sh.size());
    msg = sh + "|" + cl_ip + "|" + to_string(cl_port) +  "|" + f_path ;
    send(sock, msg.c_str() , msg.size() , 0);
    cout << "MSG SENT" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cout << "improper arguments :( \n"
             << endl;
        exit(1);
    }
    else
    {
        process_args(argv);
        int sock = soc_creation();
        string sh_string = "share ../tmp_data/mycheck.pdf ../tmp_data/mycheck.mtorrent";
        vector<string> command = split_command(sh_string);
        // for(auto i:command) cout << i << endl;
        if(command[0] == "share"){
            mtorrent_generator(command[1], command[2]);
            notify_server(command[2],sock);
        }
        // char rushit[] = "Hello there, This is Jasani :) ";
        // send(sock, rushit, strlen(rushit), 0);
        // cout << "MSG SENT" << endl;
        char buffer[1024] = {0};
        read(sock, buffer, 1024);
        cout << buffer << endl;
        //check for existing .mtorrent files
    }
    return 0;
}
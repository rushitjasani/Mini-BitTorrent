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

// void add_to_torrentlist(string data)
// {
//     // lock_guard<mutex> locker(my_mutex);
//     fstream seeder_file;
//     seeder_file.open(torrnet_record, fstream::app);
//     seeder_file << data << endl;
//     seeder_file.close();
// }

void notify_server(string filename, int sock)
{
    string msg, sh, f_path, s;
    ifstream i_file;
    i_file.open(filename);
    for (int i = 0; i < 5; i++)
    {
        if (i == 0 || i == 1 || i == 3)
        {
            getline(i_file, s);
            continue;
        }
        else if (i == 2)
        {
            getline(i_file, f_path);
        }
        else if (i == 4)
        {
            getline(i_file, sh);
        }
    }
    i_file.close();
    // cout << "File Path = " << f_path << endl;
    sh = get_SHA1((char *)sh.c_str(), sh.size());
    msg = "0|" + sh + "|" + cl_ip + ":" + to_string(cl_port) + "|" + f_path;
    send(sock, msg.c_str(), msg.size(), 0);
    cout << msg << endl;
    cout << "MSG SENT" << endl;
}

void listen_for_client(){
    //in this thread client will accept request and do all jobs.
    return;
}

void getData(string file_path){

}

void remove_from_server(string torrent_file, int sock){
    string msg, sh, f_path, s;
    ifstream i_file;
    i_file.open(torrent_file);
    for (int i = 0; i < 5; i++)
    {
        if (i == 0 || i == 1 || i == 3)
        {
            getline(i_file, s);
            continue;
        }
        else if (i == 2)
        {
            getline(i_file, f_path);
        }
        else if (i == 4)
        {
            getline(i_file, sh);
        }
    }
    i_file.close();
    // cout << "File Path = " << f_path << endl;
    sh = get_SHA1((char *)sh.c_str(), sh.size());
    msg = "1|" + sh + "|" + cl_ip + ":" + to_string(cl_port) + "|" + f_path;
    send(sock, msg.c_str(), msg.size(), 0);
    cout << msg << endl;
    cout << "MSG SENT" << endl;
}
string create_absolute_path(string r_path)
{
    string abs_path = "";
    if (r_path[0] == '.')
    {
        abs_path = string(cur_dir) + r_path.substr(1, r_path.length());
    }
    else if (r_path[0] == '/' || r_path[0] == '~')
        ;
    else
        abs_path = string(cur_dir) + '/' + r_path;
    // cout << abs_path << " ABS PATH" << endl;
    return abs_path;
}

/*
 * Creates Socket by using provided ip and ports.
 */
int soc_creation()
{
    struct sockaddr_in tr1_addr;
    // struct sockaddr_in tr2_addr;
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Socket creation error" << endl;
        exit(EXIT_FAILURE);
    }
    memset(&tr1_addr, '0', sizeof(tr1_addr));
    // memset(&tr2_addr, '0', sizeof(tr2_addr));

    tr1_addr.sin_family = AF_INET;
    // tr2_addr.sin_family = AF_INET;
    tr1_addr.sin_port = htons(tr1_port);
    // tr2_addr.sin_port = htons(tr2_port);

    if (inet_pton(AF_INET, tr1_ip.c_str(), &tr1_addr.sin_addr) <= 0)
    {
        cout << "Invalid Tracker 1 address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }
    // if (inet_pton(AF_INET, tr2_ip.c_str(), &tr2_addr.sin_addr) <= 0)
    // {
    //     cout << "Invalid Tracker 1 address/ Address not supported" << endl;
    //     exit(EXIT_FAILURE);
    // }
    cout << "CONNECTING..." << endl;
    if (connect(sock, (struct sockaddr *)&tr1_addr, sizeof(tr1_addr)) < 0)
    {
        cout << " Tracker 1 seems Busy.. Trying to connect with Tracker 2.. " << endl;
        // if (connect(sock, (struct sockaddr *)&tr2_addr, sizeof(tr2_addr)) < 0)
        // {
        //     cout << "Connection Failed :( \n Try again later" << endl;
        // }
    }
    cout << "CONNECTED with sock =  " << sock << endl;
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

void update_wakeup(){
    //At time of wakeup, notify server about all available files.
    return;
}

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
        getcwd(cur_dir, sizeof(cur_dir));
        process_args(argv);
        // bool isSharing=false;
        update_wakeup();
        //read stored .mtorrent file and notify server about it.
        while (1)
        {
            // string sh_string = "share ./tmp_data/mycheck.pdf ./tmp_data/mycheck.mtorrent";
            cout << "Taking string" << endl;
            string sh_string;
            cin >> sh_string;
            vector<string> command = split_command(sh_string);
            for(auto i:command) cout << i << endl;
            fflush(stdin);
            fflush(stdout);
            sleep(1);
            if (command[0] == "share ")
            {
                cout << "in share command" << endl;
                int sock_1 = soc_creation();
                mtorrent_generator(create_absolute_path(command[1]), command[2]);
                cout << "torrent generated." << endl;
                notify_server(command[2], sock_1);
                // if(!isSharing){
                //     thread serverThread(listen_for_client);
                //     isSharing = true;
                // } 
                close(sock_1);
                continue;
            }
            if (command[0] == "get")
            {
                string torrent_file = command[1];
                getData(torrent_file);
                continue;
            }
            if (command[0] == "remove")
            {
                int sock_1 = soc_creation();
                remove_from_server(command[1],sock_1);
                remove(command[1].c_str());
                close(sock_1);
                continue;
            }
            if(command[0] == "exit"){
                exit(0);
            }
            cout << "NOTHING MATCHED" << endl;

        }
    }
    return 0;
}
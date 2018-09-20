/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/

#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "client_header.h"
#endif

#ifndef CL_GLOBAL_H
#define CL_GLOBAL_H
#include "cl_global.h"
#endif

/*
 * checks if file already exist or not.
 */
int isFileExist(string path)
{
    FILE *file;
    if ((file = fopen(path.c_str(), "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

/*
 *check if directory exist or not. 
 */
bool isDirectory(string path)
{
    struct stat sb;
    if (stat(path.c_str(), &sb) != 0)
    {
        perror(path.c_str());
        return false;
    }
    if (S_ISDIR(sb.st_mode))
        return true;
    else
        return false;
}

/*
 * give protected fstream object of logfile to 
 * write log in logfile.
 */
fstream getLogFile()
{
    logfile_mutex.lock();
    fstream my_file;
    my_file.open(log_file, ios::app);
    return my_file;
}

/*
 * write Logs to log_file
 */
void writeLog(string message)
{
    fstream logfile_fd;
    logfile_fd = getLogFile();
    time_t cur = time(NULL);
    logfile_fd << ctime(&cur) << "" << message << endl;
    logfile_mutex.unlock();
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

/*
 * Inform server about exit so that server can
 * remove all entries for given client.
 */
void call_me_at_exit(int sock)
{
    string msg = "3|a|" + cl_ip + ":" + to_string(cl_port) + "|a";
    send(sock, msg.c_str(), msg.size(), 0);
    cout << "informed tracker about shutdown." << endl;
    return;
}

/*
 * Take string entered by user, splits by space and interpret it.
 * paths with spaces are also handled by this method.
 */
vector<string> split_command(string command_string)
{
    vector<string> my_command;
    string tmp = "";
    unsigned int i = 0;
    for (; i < command_string.size(); i++)
    {
        if (command_string[i] == ' ' || command_string[i] == '\n')
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
    return my_command;
}

/*
 * Takes path as input and if path starts with "."(dot) then 
 * convert it to absolute path. 
 * path starts with ~ (HOME) are intact because home will remain
 * intact all the time for same client.
 */
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
    return abs_path;
}

/*
 * Creates Socket by using provided ip and ports 
 * of tracker server.
 */
int socket_creation_to_server()
{
    struct sockaddr_in tr1_addr;
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Socket creation error" << endl;
        exit(EXIT_FAILURE);
    }

    memset(&tr1_addr, '0', sizeof(tr1_addr));

    tr1_addr.sin_family = AF_INET;
    tr1_addr.sin_port = htons(tr1_port);

    if (inet_pton(AF_INET, tr1_ip.c_str(), &tr1_addr.sin_addr) <= 0)
    {
        cout << "Invalid Tracker 1 address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "CONNECTING..." << endl;
    if (connect(sock, (struct sockaddr *)&tr1_addr, sizeof(tr1_addr)) < 0)
    {
        cout << " Tracker seems Busy.. Trying to connect with Tracker 2.. " << endl;
        return -1;
    }

    cout << "CONNECTED with sock =  " << sock << endl;
    return sock;
}
/*
 * Helper call to SHARE functionality which takes 
 * user input, validates it and send request to
 * server if found everything ok.
 */
void share_call_to_server(vector<string> user_input)
{
    if (user_input.size() != 3)
    {
        cout << "Improper Arguments" << endl;
        return;
    }
    string source_file = create_absolute_path(user_input[1]);
    string mtorrent_file = user_input[2];
    if (!isFileExist(source_file))
    {
        cout << "Source Not Exist" << endl;
        return;
    }
    if (isFileExist(mtorrent_file))
    {
        cout << "Already shared the file" << endl;
        return;
    }
    int sock_1 = socket_creation_to_server();
    mtorrent_generator(source_file, mtorrent_file);
    notify_server(mtorrent_file, sock_1);
    close(sock_1);
}

/*
 * Helper call to GET functionality which takes 
 * user input, validates it and send request to
 * server if found everything ok and also receives 
 * response if any.
 */
void get_call_to_server(vector<string> user_input)
{
    if (user_input.size() != 3)
    {
        cout << "Improper Arguments" << endl;
        return;
    }
    string mtorrent_file = create_absolute_path(user_input[1]);
    string down_path = create_absolute_path(user_input[2]);
    if (!isFileExist(mtorrent_file))
    {
        cout << "mtorrent file not exist." << endl;
        return;
    }
    if (!isDirectory(down_path))
    {
        cout << "Destination is not a directory." << endl;
        return;
    }
    if (mtorrent_file.substr(mtorrent_file.find_last_of(".") + 1) != "mtorrent")
    {
        cout << "Not mtorrent file." << endl;
        return;
    }
    int sock_1 = socket_creation_to_server();
    vector<pair<string, string>> seeders = getData(mtorrent_file, sock_1);
    cout << seeders.size() << endl;
    for (auto i : seeders)
    {
        cout << i.first << " " << i.second << endl;
    }
    close(sock_1);
}

/*
 * Helper call to REMOVE functionality which takes 
 * user input, validates it and send request to
 * server if found everything ok.
 */
void remove_call_to_server(vector<string> user_input)
{
    if (user_input.size() != 2)
    {
        cout << "Improper Arguments" << endl;
        return;
    }
    string mtorrent_file = create_absolute_path(user_input[1]);
    if (!isFileExist(mtorrent_file))
    {
        cout << "mtorrent file not exist." << endl;
        return;
    }
    string ext = mtorrent_file.substr(mtorrent_file.find_last_of(".") + 1, 8);
    if (ext != "mtorrent")
    {
        cout << "Not mtorrent file." << endl;
        return;
    }
    int sock_1 = socket_creation_to_server();
    remove_from_server(mtorrent_file, sock_1);
    remove(user_input[1].c_str());
    close(sock_1);
}

/*
 * final call which connect with server and inform
 * about shutdown.
 */
void exit_call_to_server(vector<string> user_input)
{
    if (user_input.size() != 1)
    {
        cout << "Improper Arguments" << endl;
        return;
    }
    int sock_1 = socket_creation_to_server();
    call_me_at_exit(sock_1);
    close(sock_1);
    exit(0);
}

/*
 * Take userinput and call respective function to provide 
 * service.
 */
void client_service(string user_input)
{
    vector<string> command = split_command(user_input);
    if (command[0] == "share")
    {
        share_call_to_server(command);
        return;
    }
    else if (command[0] == "get")
    {
        get_call_to_server(command);
        return;
    }
    else if (command[0] == "remove")
    {
        remove_call_to_server(command);
        return;
    }
    else if (command[0] == "exit")
    {
        exit_call_to_server(command);
    }
    else
    {
        cout << "Invalid Command :(";
        return;
    }
    return;
}

/*
 * by passing path to m.torrent file and socket, 
 * this method generates request by reading file data
 * and send it to server.
 */
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
    sh = get_SHA1((char *)sh.c_str(), sh.size());
    msg = "0" + SEP + sh + SEP + cl_ip + ":" + to_string(cl_port) + SEP + f_path;
    send(sock, msg.c_str(), msg.size(), 0);
    cout << msg << endl;
    cout << "MSG SENT" << endl;
}
/*
 * Actual connection to server and getting response here
 */
vector<pair<string, string>> getData(string torrent_file, int sock)
{
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
    sh = get_SHA1((char *)sh.c_str(), sh.size());
    msg = "2" + SEP + sh + SEP + "a" + SEP + "a";
    send(sock, msg.c_str(), msg.size(), 0);
    cout << msg << endl;
    cout << "MSG SENT" << endl;

    char buffer[1024] = {0};
    long long size_string;
    //receiving size of incoming data and set buffer size to that size.
    read(sock, buffer, 1024);
    stringstream s_z(buffer);
    s_z >> size_string;
    char *buff = (char *)malloc(size_string * sizeof(char));
    //receiving original data.
    read(sock, buff, size_string);
    vector<pair<string, string>> data;
    char *token = strtok(buff, SEP.c_str());
    char *token1 = strtok(NULL, SEP.c_str());
    while (token && token1)
    {
        data.push_back({token, token1});
        token = strtok(NULL, SEP.c_str());
        token1 = strtok(NULL, SEP.c_str());
    }
    return data;
}

/*
 * connect to server and send message about remove 
 * some perticular entry.
 */
void remove_from_server(string torrent_file, int sock)
{
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
    sh = get_SHA1((char *)sh.c_str(), sh.size());
    msg = "1" + SEP + sh + SEP + cl_ip + ":" + to_string(cl_port) + SEP + f_path;
    send(sock, msg.c_str(), msg.size(), 0);
    cout << msg << endl;
    cout << "MSG SENT" << endl;
}

/*
 * whenever client comes up, it checks for existing 
 * mtorrent file in current directory and notify server 
 * about that files.
 */ 
void update_wakeup()
{
    int socket_of_server = socket_creation_to_server();
    DIR *dp;
    dp = opendir(cur_dir);
    struct dirent *d;
    if (dp == NULL)
    {
        perror("opendir");
        return;
    }
    while ((d = readdir(dp)))
    {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;
        else
        {
            string name = create_absolute_path( string(d->d_name) );
            string ext = name.substr(name.find_last_of(".") + 1, 8);
            if( ext == "mtorrent" ){
                notify_server( name, socket_of_server );
            }
        }
    }
    return;
}
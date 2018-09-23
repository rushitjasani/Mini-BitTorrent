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
    string t = ctime(&cur);
    t = t.substr(4, 16);
    logfile_fd << t << ": " << message << endl;
    logfile_mutex.unlock();
    return;
}

/*
 * give protected fstream object of status 
 * file to write and read from it.
 */
fstream getStatusMapFile(int mode)
{
    stastusfile_mutex.lock();
    fstream my_file;
    if (mode == 0)
        my_file.open(".statusFile", ios::in);
    if (mode == 1)
        my_file.open(".statusFile", ios::out);
    if (mode == 2)
        my_file.open(".statusFile", ios::app);
    return my_file;
}

/*
 * APPEND data to status file 
 */
void append_to_statusFile(string data)
{
    fstream status_file;
    status_file = getStatusMapFile(2);
    status_file << data << endl;
    status_file.close();
    stastusfile_mutex.unlock();
    writeLog("status file updated.");
    return;
}

/*
 * take file stream object of seeder file and overwrite whole map to 
 * that file into it. 
 */
void write_to_statusFile()
{
    fstream status_file;
    status_file = getStatusMapFile(1);
    for (auto i : status_map)
    {
        status_file << i.second << SEP << i.first << endl;
    }
    status_file.close();
    stastusfile_mutex.unlock();
    writeLog("status file updated.");
    return;
}

/*
 * At Wakeup time, read status file and 
 * dump that content to statusmap
 */
void read_statusFile()
{
    fstream status_file;
    status_file = getStatusMapFile(0);
    string s;
    while (getline(status_file, s))
    {
        char *token = strtok((char *)s.c_str(), SEP.c_str());
        string status = token;
        token = strtok(NULL, SEP.c_str());
        string f_path = token;
        status_map[f_path] = status;
    }
    status_file.close();
    stastusfile_mutex.unlock();
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
    string msg = "3" + SEP + "a" + SEP + cl_ip + ":" + to_string(cl_port) + SEP + "a";
    send(sock, msg.c_str(), msg.size(), 0);
    writeLog("informed tracker about shutdown.");
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
    if (r_path[0] == '/' || r_path[0] == '~')
        abs_path = r_path;
    else
    {
        abs_path = string(cur_dir) + '/' + r_path;
    }
    return abs_path;
}

/*
 * Creates Socket by using provided ip and ports 
 * of tracker server.
 */
int socket_creation_to_server(string ip_address, int port_address)
{
    struct sockaddr_in tr1_addr;
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        writeLog("Socket creation error.");
        exit(EXIT_FAILURE);
    }
    writeLog("Socket created successfully.");
    memset(&tr1_addr, '0', sizeof(tr1_addr));

    tr1_addr.sin_family = AF_INET;
    tr1_addr.sin_port = htons(port_address);

    if (inet_pton(AF_INET, ip_address.c_str(), &tr1_addr.sin_addr) <= 0)
    {
        writeLog("Invalid Tracker 1 address/ Address not supported. => " + ip_address + " is INVALID IP");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&tr1_addr, sizeof(tr1_addr)) < 0)
    {
        writeLog("Tracker seems Busy.. Trying to connect with Tracker 2.. ");
        return -1;
    }

    writeLog("Connected with sock =  " + to_string(sock));
    return sock;
}

/*
 * whenever client comes up, it checks for existing 
 * mtorrent file in current directory and notify server 
 * about that files.
 */
void update_wakeup()
{
    writeLog("Scanning Current Directory for existing existing mtorrent files..");
    int socket_of_server = socket_creation_to_server(tr1_ip, tr1_port);
    DIR *dp;
    dp = opendir(cur_dir);
    struct dirent *d;
    if (dp == NULL)
    {
        writeLog("Error in opening Directory.");
        return;
    }
    while ((d = readdir(dp)))
    {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;
        else
        {
            string name = create_absolute_path(string(d->d_name));
            string ext = name.substr(name.find_last_of(".") + 1, 8);
            if (ext == "mtorrent")
            {
                notify_server(name, socket_of_server);
            }
        }
    }
    writeLog("Notified Server about all existing files.");
    return;
}

/*
 * final call which connect with server and inform
 * about shutdown.
 */
void exit_call_to_server(vector<string> user_input)
{
    if (user_input.size() != 1)
    {
        cout << "FAILURE : INVALID ARGUMENTS" << endl;
        writeLog("Invalid arguments provided.");
    }
    int sock_1 = socket_creation_to_server(tr1_ip, tr1_port);
    call_me_at_exit(sock_1);
    close(sock_1);
    cout << "THANK YOU :) " << endl;
    writeLog("Client Shut Down");
    exit(0);
}

/*
 * Show current status of downloading and downloaded 
 * files.
 */
void show_downloads(vector<string> user_input)
{
    if (user_input.size() != 1)
    {
        cout << "FAILURE : INVALID ARGUMENTS" << endl;
        writeLog("Invalid arguments provided.");
        return;
    }
    for (auto i : status_map)
    {
        cout << i.second << "  " << i.first << endl;
    }
    return;
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
    writeLog("Created Request : " + msg);
    msg = "1" + SEP + sh + SEP + cl_ip + ":" + to_string(cl_port) + SEP + f_path;
    send(sock, msg.c_str(), msg.size(), 0);
    writeLog("Sent message : " + msg);
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
        cout << "FAILURE : INVALID ARGUMENTS" << endl;
        writeLog("Invalid arguments provided.");
        return;
    }
    string mtorrent_file = create_absolute_path(user_input[1]);
    if (!isFileExist(mtorrent_file))
    {
        cout << "FAILURE : MTORRENT FILE NOT FOUND" << endl;
        writeLog(mtorrent_file + "File Not Found.");
        return;
    }
    string ext = mtorrent_file.substr(mtorrent_file.find_last_of(".") + 1, 8);
    if (ext != "mtorrent")
    {
        cout << "FAILURE : INVALID MTORRENT FILE" << endl;
        writeLog(mtorrent_file + " is not required mtorrent file.");
        return;
    }
    int sock_1 = socket_creation_to_server(tr1_ip, tr1_port);
    remove_from_server(mtorrent_file, sock_1);
    remove(user_input[1].c_str());
    cout << "SUCCESS : " << mtorrent_file << " REMOVED" << endl; 
    close(sock_1);
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
    writeLog("Created Request : " + msg);
    send(sock, msg.c_str(), msg.size(), 0);
    writeLog("Sent message : " + msg);
    int n;
    string recv_data;
    do
    {
        char buffer[1024] = {0};
        memset(buffer, 0, sizeof(buffer));
        n = read(sock, buffer, 1024);
        recv_data += string(buffer);
    } while (n > 0);
    char buff[recv_data.size() + 1];
    strcpy(buff, recv_data.c_str());
    writeLog("Received SeederList :" + recv_data);
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
 * Helper call to GET functionality which takes 
 * user input, validates it and send request to
 * server if found everything ok and also receives 
 * response if any.
 */
void get_call_to_server(vector<string> user_input)
{
    if (user_input.size() != 3)
    {
        cout << "FAILURE : INVALID ARGUMENTS" << endl;
        writeLog("Invalid arguments provided.");
        return;
    }
    string mtorrent_file = create_absolute_path(user_input[1]);
    string down_path = create_absolute_path(user_input[2]);
    if (!isFileExist(mtorrent_file))
    {
        cout << "FAILURE : MTORRENT FILE NOT FOUND" << endl;
        writeLog(mtorrent_file + "File Not Found.");
        return;
    }
    if (isFileExist(down_path))
    {
        cout << "FAILURE : DESTINATION FILE ALREADY EXIST" << endl;
        writeLog(down_path + " file already exist.");
        return;
    }
    if (mtorrent_file.substr(mtorrent_file.find_last_of(".") + 1) != "mtorrent")
    {
        cout << "FAILURE : INVALID MTORRENT FILE" << endl;
        writeLog(mtorrent_file + " is not required mtorrent file.");
        return;
    }
    int sock_1 = socket_creation_to_server(tr1_ip, tr1_port);
    vector<pair<string, string>> seeders = getData(mtorrent_file, sock_1);

    close(sock_1);
    if (seeders.size() > 0)
    {
        /*
         * share info to client that this client also have this file now.
         */
        string msg, sh, f_path, s;
        ifstream i_file;
        i_file.open(mtorrent_file);
        for (int i = 0; i < 5; i++)
        {
            if (i == 0 || i == 1 || i == 2 || i == 3)
            {
                getline(i_file, s);
                continue;
            }
            else if (i == 4)
            {
                getline(i_file, sh);
            }
        }
        i_file.close();

        sh = get_SHA1((char *)sh.c_str(), sh.size());
        writeLog("Sharing " + down_path + " file to tracker..");
        msg = "0" + SEP + sh + SEP + cl_ip + ":" + to_string(cl_port) + SEP + down_path;
        int update_sock = socket_creation_to_server(tr1_ip, tr1_port);
        char message[msg.size() + 1];
        strcpy(message, msg.c_str());
        send(update_sock, message, strlen(message), 0);
        close(update_sock);
        writeLog(down_path + " file shared.");
        writeLog("Downloading in separate thread is started..");
        status_map[down_path] = "D";
        append_to_statusFile("D" + SEP + down_path);
        thread connect_peer(revc_data_from_client, seeders, mtorrent_file, down_path);
        connect_peer.detach();
    }
    else
    {
        cout << "FAILURE : NO PEERS AVAILABLE." << endl;
        writeLog("No peers available for " + mtorrent_file);
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
    writeLog("Created Request : " + msg);
    send(sock, msg.c_str(), msg.size(), 0);
    writeLog("Sent message : " + msg);
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
        cout << "FAILURE : INVALID ARGUMENTS" << endl;
        writeLog("Invalid arguments provided.");
        return;
    }
    string source_file = create_absolute_path(user_input[1]);
    string mtorrent_file = user_input[2];
    if (!isFileExist(source_file))
    {
        cout << "FAILURE : FILE NOT FOUND" << endl;
        writeLog(source_file + "File Not Found.");
        return;
    }
    // if (isFileExist(mtorrent_file))
    // {
    //     cout << "Already shared the file" << endl;
    //     return;
    // }
    int sock_1 = socket_creation_to_server(tr1_ip, tr1_port);
    mtorrent_generator(source_file, mtorrent_file);
    notify_server(mtorrent_file, sock_1);
    cout << "SUCCESS : " << mtorrent_file << endl;
    close(sock_1);
    writeLog("Socket " + to_string(sock_1) + " Closed.");
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
    else if (command[0] == "show_downloads")
    {
        show_downloads(command);
        return;
    }
    else if (command[0] == "close")
    {
        exit_call_to_server(command);
        return;
    }
    else
    {
        cout << "FAILURE : INVALID COMMAND " << endl;
        writeLog("FAILURE : INVALID COMMAND " + command[0]);
        return;
    }
}
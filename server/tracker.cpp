/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
/*
 * ./tracker <my_tracker_ip>:<my_tracker_port> <other_tracker_ip>:<other_tracker_port> <seederlist_file> <log_file>
 * ./tracker 10.1.38.138:6565 10.1.38.138:6567 seed_file log_file
 */

#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "server_header.h"
#endif

#ifndef CL_GLOBAL_H
#define CL_GLOBAL_H
#include "server_global.h"
#endif

/*
 * Process command line arguments and stores
 * all ip/port in apropriate fields in global
 * variables.
 */
void process_args(char *argv[])
{
    char *token = strtok(argv[1], ":");
    tr1_ip = token;
    token = strtok(NULL, ":");
    tr1_port = stoi(token);

    token = strtok(argv[2], ":");
    tr2_ip = token;
    token = strtok(NULL, ":");
    tr2_port = stoi(token);

    seeder_list = argv[3];
    log_file = argv[4];
}

struct sockaddr_in tr1_addr;
// struct sockaddr_in tr2_addr;

int soc_creation()
{
    struct sockaddr_in tr1_addr;
    // struct sockaddr_in tr2_addr;
    int sock = 0;
    int opt = 1;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        cout << "Socket creation error" << endl;
        exit(EXIT_FAILURE);
    }

    // memset(&tr1_addr, '0', sizeof(tr1_addr));
    // memset(&tr2_addr, '0', sizeof(tr2_addr));

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    tr1_addr.sin_family = AF_INET;
    tr1_addr.sin_port = htons(tr1_port);
    tr1_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&tr1_addr, sizeof(tr1_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "BIND DONE" << endl;
    if (listen(sock, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "LISTENING" << endl;
    return sock;
}

// mutex my_mutex;
void add_entry_to_seederlist(string data)
{
    // lock_guard<mutex> locker(my_mutex);
    fstream seeder_file;
    seeder_file.open(seeder_list, fstream::app);
    seeder_file << data << endl;
    seeder_file.close();
}

bool isAvailable(vector<pair<string, string>> lst, pair<string, string> p)
{
    for (auto i : lst)
    {
        if (i.first == p.first && i.second == p.second)
            return true;
    }
    return false;
}


void print_map(){
    cout << "================================================" << endl;
    for (auto i : seeder_map)
    {
        cout << i.first << endl;
        for (auto j : i.second)
            cout << j.first << endl
                 << j.second << endl;
        cout << endl;
    }
    cout << "================================================" << endl;
}

void serve(int cl_soc)
{
    char buffer[1024] = {0};
    read(cl_soc, buffer, 1024);
    string buf_buf = buffer;
    cout << buffer << endl;
    char *token = strtok(buffer, "|");
    string key_hash = token;
    token = strtok(NULL, "|");
    string cl_socket = token;
    token = strtok(NULL, "|");
    string file_path = token;
    // buffer[0]='\0';
    if (seeder_map.find(key_hash) == seeder_map.end())
    {
        // seeder_map[key_hash].push_back({cl_socket, file_path});
        cout << "I AM IN NOT MATCH" << endl;
        seeder_map[key_hash][cl_socket] = file_path;
        add_entry_to_seederlist(buf_buf);
    }
    else
    {
        // pair<string, string> new_pair = {cl_socket, file_path};
        // if (!isAvailable(seeder_map[key_hash], new_pair))
        if (seeder_map[key_hash].find(cl_socket) == seeder_map[key_hash].end())
        {
            cout << "I AM IN MATCH" << endl;
            // seeder_map[key_hash].push_back({cl_socket, file_path});
            seeder_map[key_hash][cl_socket] = file_path;
            add_entry_to_seederlist(buf_buf);
        }
    }
    print_map();
    // char rushit[] = "Hello there, This is Jasani :) ";
    // send(cl_soc, rushit, strlen(rushit), 0);
    // cout << "MSG SENT" << endl;
}

void read_seederlist()
{
    ifstream seed_file;
    seed_file.open(seeder_list);
    string s;
    // cout << "FILE READING START" << endl;
    while (getline(seed_file, s))
    {
        char *token = strtok((char *)s.c_str(), "|");
        string key_hash = token;
        token = strtok(NULL, "|");
        string cl_socket = token;
        token = strtok(NULL, "|");
        string file_path = token;
        seeder_map[key_hash][cl_socket] = file_path;
        // cout << " READING " << endl;
    }
    seed_file.close();
    // cout << "FILE READING END" << endl;
    print_map();
    return;
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
        read_seederlist();
        int sock = soc_creation();
        cout << "SOCKET CREATED " << endl;
        int cl_soc;
        while (1)
        {
            cout << " WAITING FOR CLIENT with sock = " << sock << endl;
            if ((cl_soc = accept(sock, (struct sockaddr *)&tr1_addr, (socklen_t *)&tr1_addr)) < 0)
            {
                cout << "Client Socket: " <<  cl_soc << endl;
                perror("accept");
                // exit(EXIT_FAILURE);
                continue;
            }
            cout << " ACCEPTED " << endl;
            serve(cl_soc);
            // std::thread t1(serve, cl_soc );
            // t1.detach();
            // if (t1.joinable())
            //     t1.join();
        }
    }
    return 0;
}
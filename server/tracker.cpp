/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
/*
 * ./tracker <my_tracker_ip>:<my_tracker_port> <other_tracker_ip>:<other_tracker_port> <seederlist_file> <log_file>
 * ./tracker 10.1.38.138:6565 10.1.38.138:6567 seed_file log_file
 * ./tracker 127.0.0.1:6565 127.0.0.1:6567 seed_file log_file
 */

#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "server_header.h"
#endif

#ifndef CL_GLOBAL_H
#define CL_GLOBAL_H
#include "server_global.h"
#endif

void serve(int cl_soc)
{
    char buffer[1024] = {0};
    int n = read(cl_soc, buffer, 1024);
    if (n <= 0)
    {
        shutdown(cl_soc, SHUT_RDWR);
        close(cl_soc);
        return;
    }
    vector<string> client_req;
    char *token = strtok(buffer, SEP.c_str());
    while (token)
    {
        client_req.push_back(token);
        token = strtok(NULL, SEP.c_str());
    }
    string key_hash = client_req[1];
    string cl_socket = client_req[2];
    string file_path = client_req[3];
    string buf_buf = key_hash + SEP + cl_socket + SEP + file_path;
    writeLog("Request : " + buf_buf);
    if (client_req[0] == "0")
    {
        //share
        if (seeder_map.find(key_hash) == seeder_map.end())
        {
            seeder_map[key_hash][cl_socket] = file_path;
            append_to_seederlist(buf_buf);
        }
        else
        {
            if (seeder_map[key_hash].find(cl_socket) == seeder_map[key_hash].end())
            {
                seeder_map[key_hash][cl_socket] = file_path;
                append_to_seederlist(buf_buf);
            }
        }
    }
    else if (client_req[0] == "1")
    {
        //remove
        if (seeder_map.find(key_hash) != seeder_map.end())
        {
            map<string, string> temp;
            temp.insert(seeder_map[key_hash].begin(), seeder_map[key_hash].end());
            if (temp.size() == 1 && temp.find(cl_socket) != temp.end())
            {
                seeder_map.erase(key_hash);
                write_to_seederlist();
            }
            else if (temp.size() != 1)
            {
                seeder_map[key_hash].erase(cl_socket);
                write_to_seederlist();
            }
        }
    }
    else if (client_req[0] == "2")
    {
        //get
        string res;
        for (auto i : seeder_map[key_hash])
        {
            res = res + i.first + SEP + i.second + SEP;
        }
        writeLog("Response to GET :" + res);
        send(cl_soc, res.c_str(), res.size(), 0);
        writeLog("Data sent to client successfully :) ");
    }
    else if (client_req[0] == "3")
    {
        //EXIT
        map<string, map<string, string>>::iterator i;
        vector<map<string, map<string, string>>::iterator> del;
        for (i = seeder_map.begin(); i != seeder_map.end(); ++i)
        {
            if (i->second.find(cl_socket) != i->second.end())
            {
                i->second.erase(cl_socket);
            }
            if (i->second.size() == 0)
            {
                del.push_back(i);
            }
        }
        for (auto i : del)
        {
            seeder_map.erase(i);
        }
        write_to_seederlist();
    }
    writeLog("REQUEST " + client_req[0] + " SERVED");
    print_map();
    shutdown(cl_soc, SHUT_RDWR);
    close(cl_soc);
    writeLog("Socket : " + to_string(cl_soc) + " closed successfully.");
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
        writeLog("Argument Processed.");
        read_seederlist();
        writeLog("SeededList File Read Successfully.");
        struct sockaddr_in tr1_addr;
        int sock = 0;
        int opt = 1;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            writeLog("Socket creation error");
            exit(EXIT_FAILURE);
        }
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            writeLog("setsockopt Error");
            exit(EXIT_FAILURE);
        }
        tr1_addr.sin_family = AF_INET;
        tr1_addr.sin_port = htons(tr1_port);
        tr1_addr.sin_addr.s_addr = inet_addr(tr1_ip.c_str());

        if (bind(sock, (struct sockaddr *)&tr1_addr, sizeof(tr1_addr)) < 0)
        {
            writeLog("Bind Failed..");
            exit(EXIT_FAILURE);
        }
        writeLog("Bind Successful.");
        if (listen(sock, 5) < 0)
        {
            writeLog("Listen Failed..");
            exit(EXIT_FAILURE);
        }
        writeLog("Socket Created with sock = " + to_string(sock) + ".");
        writeLog("Listening for clients.");
        int cl_soc;
        int addrlen = sizeof(tr1_addr);
        while (1)
        {
            writeLog("Waiting for client :");
            cl_soc = accept(sock, (struct sockaddr *)&tr1_addr, (socklen_t *)&addrlen);
            if (cl_soc < 0)
            {
                writeLog("in accept, something unwanted occured...");
                continue;
            }
            writeLog("Connection accepted.");
            try
            {
                writeLog("Thread Created for new client.");
                std::thread t1(serve, std::ref(cl_soc));
                t1.detach();
            }
            catch (const std::exception &ex)
            {
                writeLog("Thread exited with some exception. :(");
            }
        }
    }
    return 0;
}
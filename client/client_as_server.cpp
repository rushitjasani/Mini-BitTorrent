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

void send_data_to_client(int new_socket)
{
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);
    cout << buffer << endl;
    string f_name(buffer);

    ifstream input_file;
    input_file.open(f_name, ios::binary | ios::in);

    size_t chnk_size = 512 * 1024;          //512KB
    size_t f_size = get_file_size(f_name);

    unsigned long long s = f_size;
    cout << "File size = " << s << endl;
    if (s < chnk_size)
        chnk_size = s;

    char tmp[chnk_size];

    while (input_file.read(tmp, chnk_size))
    {
        s -= chnk_size;
        send(new_socket, tmp, chnk_size, 0);
        if (s == 0)
            break;
        if (s < chnk_size)
            chnk_size = s;
    }
    return;
}

void seeding_files()
{
    struct sockaddr_in cl_listener;
    int sock = 0;
    int opt = 1;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        cout << "Socket creation error" << endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    cl_listener.sin_family = AF_INET;
    cl_listener.sin_port = htons(cl_port);
    cl_listener.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&cl_listener, sizeof(cl_listener)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "BIND DONE" << endl;
    if (listen(sock, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "LISTENING" << endl;
    cout << "SOCKET CREATED " << endl;
    int new_client_socket;
    int addrlen = sizeof(cl_listener);
    while (1)
    {
        cout << "WAITING FOR CLIENT" << endl;
        new_client_socket = accept(sock, (struct sockaddr *)&cl_listener, (socklen_t *)&addrlen);
        if (new_client_socket < 0)
        {
            perror("IN ACCEPT : ");
            continue;
        }
        else
        {
            cout << "CONNECTION ACCEPTED " << endl;
        }
        try
        {
            thread t(send_data_to_client, std::ref(new_client_socket));
            t.join();
        }
        catch (const std::exception &ex)
        {
            std::cout << "Thread exited with exception: " << ex.what() << "\n";
        }
    }
}
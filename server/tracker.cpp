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
struct sockaddr_in tr2_addr;

int soc_creation()
{
    struct sockaddr_in tr1_addr;
    struct sockaddr_in tr2_addr;
    int sock = 0;
    int opt = 1;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        cout << "Socket creation error" << endl;
        exit(EXIT_FAILURE);
    }

    memset(&tr1_addr, '0', sizeof(tr1_addr));
    memset(&tr2_addr, '0', sizeof(tr2_addr));

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
    if (listen(sock, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return sock;
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
        int cl_soc = 0;
        if ((cl_soc = accept(sock, (struct sockaddr *) &tr1_addr , (socklen_t *)&tr1_addr)) <= 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        char buffer[1024] = {0};
        read( cl_soc , buffer, 1024); 
        cout << buffer << endl;
        char rushit[] = "Hello there, This is Jasani :) ";
        send(cl_soc, rushit, strlen(rushit), 0);
        cout << "MSG SENT" << endl;
    }
    return 0;
}
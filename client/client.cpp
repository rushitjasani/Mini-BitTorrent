/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
/*
 * ./client 127.0.0.1:6528 127.0.0.1:6565 127.0.0.1:6567 log
 * ./client 10.1.38.138:6540 10.1.38.138:6565 10.1.38.138:6567 log
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
    // int sock = soc_creation();
    cout << "SOCKET CREATED " << endl;
    int new_client_socket;
    int *new_client;
    int addrlen = sizeof(cl_listener);
    vector<thread> thread_vector;
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
            *new_client = new_client_socket;
        }
        // serve(cl_soc);
        try
        {
            thread t(send_data_to_client, std::ref(new_client));
            thread_vector.push_back(t);
            t.detach();
            std::for_each(thread_vector.begin(), thread_vector.end(), do_join);
        }
        catch (const std::exception &ex)
        {
            std::cout << "Thread exited with exception: " << ex.what() << "\n";
        }
    }
}
*/

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
        update_wakeup();
        // thread server_thread(seeding_files);
        // server_thread.detach();
        while (1)
        {
            try
            {
                string sh_string;
                getline(cin, sh_string);
                client_service(sh_string);
            }
            catch (const std::exception &ex)
            {
                std::cout << "Thread exited with exception: " << ex.what() << "\n";
            }
        }
    }
    return 0;
}
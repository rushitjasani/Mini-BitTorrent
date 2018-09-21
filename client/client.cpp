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
        // update_wakeup();
        thread server_thread(seeding_files);
        server_thread.detach();
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
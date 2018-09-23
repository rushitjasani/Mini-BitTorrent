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
        cout << "FAILURE : INVALID ARGUMENTS" << endl;
        writeLog("Invalid arguments provided to run client.");
        exit(1);
    }
    else
    {
        getcwd(cur_dir, sizeof(cur_dir));
        struct passwd *pw = getpwuid(getuid());
        const char *homedir = pw->pw_dir;
        strcpy(home_dir, homedir);
        writeLog("client initiated in directory : " + string(cur_dir));
        writeLog("HOME is : " + string(home_dir) );
        process_args(argv);
        writeLog("Argument Processed.");
        update_wakeup();
        read_statusFile();
        thread server_thread(seeding_files);
        server_thread.detach();
        while (1)
        {
            try
            {
                string sh_string;
                getline(cin, sh_string);
                writeLog("user entered command : " + sh_string);
                client_service(sh_string);
            }
            catch (const std::exception &ex)
            {
                writeLog("Thread exited with exception");
            }
        }
    }
    return 0;
}
/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
// ./client <CLIENT_IP>:<UPLOAD_PORT> <TRACKER_IP_1>:<TRACKER_PORT_1> <TRACKER_IP_2>:<TRACKER_PORT_2> <log_file>
#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "client_header.h"
#endif

#ifndef CL_GLOBAL_H
#define CL_GLOBAL_H
#include "cl_global.h"
#endif

int main(int argc, char **argv)
{

    if (argc != 4)
    {
        cout << "improper arguments :( \n"
             << endl;
        exit(1);
    }
    else
    {
        cout << argc << endl;
        //store Command line args into client global file.
        //check for existing .mtorrent files
        
    }
    return 0;
}
/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/

#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "client_header.h"
#endif

extern string cl_ip, tr1_ip, tr2_ip, log_file;
extern int cl_port, tr1_port, tr2_port;
extern char cur_dir[4096],home_dir[4096];
extern mutex logfile_mutex, stastusfile_mutex;
extern string SEP;
extern map<string, string> status_map;
/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
#ifndef SR_GLOBAL_H
#define SR_GLOBAL_H
#include "server_global.h"
#endif

string tr1_ip, tr2_ip,seeder_list, log_file;
int tr1_port, tr2_port;
map< string, map< string, string > >  seeder_map;
mutex seedfile_mutex, logfile_mutex;
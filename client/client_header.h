/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
#include <bits/stdc++.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pwd.h>

using namespace std;

#ifndef CL_HEADER_H
#define CL_HEADER_H
#endif

size_t get_file_size(string);
string get_SHA1(char *, int);
void mtorrent_generator(string, string);
int isFileExist(string);
void writeLog(string);
void process_args(char **);
void call_me_at_exit(int);
vector<string> split_command(string);
string create_absolute_path(string);
int socket_creation_to_server(string, int);
void client_service(string s);
bool isDirectory(string);
vector<pair<string, string>> getData(string, int);
void remove_from_server(string, int);
void notify_server(string, int);
void update_wakeup();
void seeding_files();
void revc_data_from_client(vector<pair<string, string>>, string, string);
void append_to_statusFile(string);
void write_to_statusFile();
void read_statusFile();

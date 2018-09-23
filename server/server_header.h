/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/
#include <bits/stdc++.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctime>

using namespace std;

#ifndef SR_HEADER_H
#define SR_HEADER_H
#endif

void append_to_seederlist(string);
void write_to_seederlist();
void process_args(char **);
void read_seederlist();
void print_map();
void writeLog(string);
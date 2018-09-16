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

using namespace std;

#ifndef CL_HEADER_H
#define CL_HEADER_H
#endif

size_t get_file_size(string);
string get_SHA1(char *, int);
void mtorrent_generator(string, string );
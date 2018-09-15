/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/

#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "client_header.h"
#endif

/*
 * Provides File size by providing file name.
 */
size_t get_file_size(string f_name)
{
    struct stat sb;
    int rc = stat(f_name.c_str(), &sb);
    return rc == 0 ? sb.st_size : -1;
}

/*
 * Give 40 char hash string of given chunk.
 */
string get_SHA1(char *data, int to_read)
{
    unsigned char md[20];
    unsigned char buf[40];
    SHA1((unsigned char *)data, to_read, md);
    for (int i = 0; i < 20; i++)
    {
        sprintf((char *)&(buf[i * 2]), "%02x", md[i]);
    }
    string md_1((char *)buf);
    return md_1;
}

/*
 * It will generate mTorrent file for given file name
 */
void mtorrent_generator(string f_name, string tor_name)
{

    // TEMP ALLOC
    string t_ip1 = "192.168.26.5";
    string t_port1 = "6547";
    string t_ip2 = "192.168.26.12";
    string t_port2 = "6248";

    ofstream o_file;
    ifstream i_file;
    i_file.open(f_name, ios::binary | ios::in);
    o_file.open(tor_name);
    size_t f_size = get_file_size(f_name);
    o_file << t_ip1 << ":" << t_port1 << endl;
    o_file << t_ip2 << ":" << t_port2 << endl;
    o_file << f_size << endl;
    size_t to_read = 512 * 1024; //512KB
    unsigned long long s = f_size;
    if (s < to_read)
        to_read = s;
    char tmp[to_read];
    string final_hash;
    while (i_file.read(tmp, to_read))
    {
        s -= to_read;
        string md_1 = get_SHA1(tmp, to_read);
        final_hash.append(md_1.substr(0, 20));
        if (s == 0)
            break;
        if (s < to_read)
            to_read = s;
    }
    cout << final_hash << endl;
    o_file << final_hash << endl;
}

int main()
{
    mtorrent_generator("mycheck.pdf", "server.mtorrent");
    return 0;
}
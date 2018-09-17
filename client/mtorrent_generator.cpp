/*============================================================
    @author - Rushitkumar Jasani   @rollno - 2018201034
=============================================================*/

#ifndef CL_HEADER_H
#define CL_HEADER_H
#include "client_header.h"
#endif

#ifndef CL_GLOBAL_H
#define CL_GLOBAL_H
#include "cl_global.h"
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
string get_SHA1(char *data, int chnk_size)
{
    unsigned char md[20];
    unsigned char buf[40];
    SHA1((unsigned char *)data, chnk_size, md);
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
    // cout << f_name << endl;
    // cout << tor_name << endl;
    // TEMP ALLOC
    string t_ip1 = tr1_ip;
    int t_port1 = tr1_port;
    string t_ip2 = tr2_ip;
    int t_port2 = tr2_port;

    ofstream o_file;
    ifstream i_file;

    i_file.open(f_name, ios::binary | ios::in);
    o_file.open(tor_name);

    size_t f_size = get_file_size(f_name);

    o_file << t_ip1 << ":" << t_port1 << endl;
    o_file << t_ip2 << ":" << t_port2 << endl;
    o_file << f_name << endl; // CREATE FILENAME TO ABSOLUTE PATH
    o_file << f_size << endl;

    size_t chnk_size = 512 * 1024; //512KB

    unsigned long long s = f_size;
    if (s < chnk_size)
        chnk_size = s;
    char tmp[chnk_size];
    string final_hash;

    while (i_file.read(tmp, chnk_size))
    {
        s -= chnk_size;
        string md_1 = get_SHA1(tmp, chnk_size);
        final_hash.append(md_1.substr(0, 20));
        if (s == 0)
            break;
        if (s < chnk_size)
            chnk_size = s;
    }
    // cout << final_hash << endl;
    o_file << final_hash << endl;
}
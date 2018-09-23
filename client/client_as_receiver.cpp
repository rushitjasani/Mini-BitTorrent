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
 * to read data from seeders and write into files.
 */
void revc_data_from_client(vector<pair<string, string>> seeder_available, string mtorrentFilePath, string download_path)
{
    string socket_of_peer = seeder_available[0].first;
    char *token = strtok((char *)socket_of_peer.c_str(), ":");
    string peer_ip = token;
    token = strtok(NULL, ":");
    int peer_port = stoi(token);
    string f_path = seeder_available[0].second;
    
    string msg, sh, s, f_size;
    ifstream i_file;
    i_file.open(mtorrentFilePath);
    for (int i = 0; i < 5; i++)
    {
        if (i == 0 || i == 1 || i == 2)
        {
            getline(i_file, s);
            continue;
        }
        else if (i == 3)
        {
            getline(i_file, f_size);
        }
        else if (i == 4)
        {
            getline(i_file, sh);
        }
    }
    i_file.close();

    int chunk_size = 512 * 1024;

    int sock = socket_creation_to_server(peer_ip, peer_port);
    send(sock, f_path.c_str(), f_path.size(), 0);
    writeLog("Starting download for file : " + download_path );
    fstream dwnld_file;
    dwnld_file.open(download_path, ios::app);
    int n;
    do
    {
        char buffer[chunk_size] = {0};
        n = read(sock, buffer, chunk_size);
        dwnld_file.write(buffer, n);
    } while (n > 0);
    dwnld_file.close();
    close(sock);
    writeLog("Download successful : " + download_path );
    cout << "SUCCESS: " << download_path << endl;
    // terminate();
    return;
}
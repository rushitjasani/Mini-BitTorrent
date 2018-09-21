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

void revc_data_from_client(vector<pair<string, string>> seeder_available, string mtorrentFilePath, string download_path )
{
    cout << "THREAD CREATED " << seeder_available.size() << endl;
    string socket_of_peer = seeder_available[0].first;
    char *token = strtok((char *)socket_of_peer.c_str(), ":");
    string peer_ip = token;
    token = strtok(NULL, ":");
    int peer_port = stoi(token);
    string f_path = seeder_available[0].second;

    string msg, sh, s,f_size ;
    ifstream i_file;
    i_file.open(mtorrentFilePath);
    for (int i = 0; i < 5; i++)
    {
        if (i == 0 || i == 1 || i==2)
        {
            getline(i_file, s);
            continue;
        }
        else if( i == 3){
            getline(i_file, f_size);
        }
        else if (i == 4)
        {
            getline(i_file, sh);
        }
    }
    i_file.close();

    int chunk_size = 512 * 1024;
    int num_of_chunks =  ceil( stoi( f_size) / (float) chunk_size );

    int sock = socket_creation_to_server(peer_ip, peer_port);
    send(sock, f_path.c_str(), f_path.size(),0 );
    fstream dwnld_file;
    dwnld_file.open( download_path, ios::app );
    for(int i=0;i<num_of_chunks;i++){
        char buffer[chunk_size + 500] = {0};
        memset(buffer, '0', sizeof(buffer));
        int len_of_received_data = read(sock, buffer, chunk_size);
        dwnld_file.write( buffer, len_of_received_data );
    }
    dwnld_file.close();
    return;
}